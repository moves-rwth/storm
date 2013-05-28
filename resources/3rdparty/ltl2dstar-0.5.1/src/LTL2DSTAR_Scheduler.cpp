/*
 * This file is part of the program ltl2dstar (http://www.ltl2dstar.de/).
 * Copyright (C) 2005-2007 Joachim Klein <j.klein@ltl2dstar.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/** @file
 * Implementation of LTL2DSTAR_Scheduler
 */

#include "LTL2DRA.hpp"
#include "LTL2DSTAR_Scheduler.hpp"

#include "DAUnionAlgorithm.hpp"
#include "DRAOptimizations.hpp"

#include "NBA2DRA.hpp"
#include "DRA2NBA.hpp"

#include "LTLSafetyAutomata.hpp"

#include "StutterSensitivenessInformation.hpp"

#include "common/TempFile.hpp"
#include "common/RunProgram.hpp"
#include "common/Exceptions.hpp"

#include <typeinfo>
#include <limits>
#include <algorithm>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

/** Dummy namespace for the building blocks */
class LTL2DSTAR_Scheduler_Trees {
public:
  class LTL2DSTAR_Tree_Start;
  class LTL2DSTAR_Tree_Safra;
  class LTL2DSTAR_Tree_Rabin;
  class LTL2DSTAR_Tree_Streett;
  class LTL2DSTAR_Tree_Union;

  
  /** Base class for the building blocks for the scheduler */
  class LTL2DSTAR_Tree {
  public:
    /** Type of a vector over the children */
    typedef std::vector<LTL2DSTAR_Tree*> child_vector;
    
    /**
     * Constructor 
     * @param ltl The LTL formula
     * @param options the LTL2DSTAR options
     * @param sched a reference back to the scheduler 
     */
    LTL2DSTAR_Tree(LTLFormula_ptr& ltl, 
		 LTL2DSTAR_Options options,
		 LTL2DSTAR_Scheduler& sched) :
      _ltl(ltl), _options(options), _sched(sched) {}
    
    /** Destructor */
    virtual ~LTL2DSTAR_Tree() {
      for (child_vector::iterator it=children.begin();
	   it!=children.end();
	   ++it) {
	delete *it;
      }
    }
    
    /** Print the tree on output stream */
    virtual void printTree(std::ostream& out,
			   unsigned int level=0) {
      for (unsigned int i=0;i<level;i++) {
	out << " ";
      }
      out << typeid(*this).name() << " = " << this <<
	"(" <<  _ltl.get() << ")" << "\n";
      for (child_vector::iterator it=children.begin();
	   it!=children.end();
	   ++it) {
	(*it)->printTree(out, level+1);
      }
    }

    /** Abstract virtual function for tree generation */
    virtual void generateTree() = 0;

    /** Estimate the size of the automaton */
    virtual unsigned int guestimate() {
      return 0;
    }

    /** Hook that is called after calculate() finishes */
    virtual void hook_after_calculate() {};

    /** Calculate the automaton for this building block, by default
     * calculate the automata for the children and then choose the smallest. */
    virtual void calculate(int level=0,
			   unsigned int limit=0) {
      if (_options.verbose_scheduler) {
	std::cerr << "Calculate ("<< level <<"): " << typeid(*this).name() << std::endl;
      }
 
      calculateChildren(level, limit);
      
      bool first=true;
      for (child_vector::iterator it=children.begin();
	   it!=children.end();
	   ++it) {
	if ((*it)->_automaton==0) {
	  continue;
	}
	
	if (first) {
	  _automaton=(*it)->_automaton;
	  _comment=(*it)->_comment;
	} else {
	  if (_automaton->size() > (*it)->_automaton->size()) {
	    _automaton=(*it)->_automaton;
	    _comment=(*it)->_comment;
	  }
	}
	
	first=false;
      }

      hook_after_calculate();
    }
    
    /** Add a new child */
    void addChild(LTL2DSTAR_Tree* child) {
      if (child==0) {return;}
      
      children.push_back(child);
    }

    /** Calculate the automata for the children */
    void calculateChildren(int level=0,
			   unsigned int limit=0) {
      if (_sched.flagOptLimits()) {
	DRA_ptr _min_automaton;
	unsigned int _min_size=0;

	for (child_vector::iterator it=children.begin();
	     it!=children.end();
	     ++it) {
	  unsigned int child_limit;
	  if (_min_size!=0) {
	    if (limit > 0) {
	      child_limit=(std::min)(_sched.calcLimit(_min_size), limit);
	    } else {
	      child_limit=_sched.calcLimit(_min_size);
	    }
	  } else {
	    child_limit=limit;
	  }
	  if (_options.verbose_scheduler) {
	    std::cerr << " Limit (with alpha) = " << child_limit << std::endl;
	  }
	  try {
	    (*it)->calculate(level+1, child_limit);
	    
	    if ((*it)->_automaton.get()!=0) {
	      if (_min_size==0 || 
		  (*it)->_automaton->size() < _min_size) {
		_min_automaton=(*it)->_automaton;
		_min_size=_min_automaton->size();
	      } else {
		// delete automaton as it is bigger
		// than necessary
		(*it)->_automaton.reset();
	      }
	    }
	  } catch (LimitReachedException& e) {
	    (*it)->_automaton.reset();
	  }
	}
      } else {
	for (child_vector::iterator it=children.begin();
	     it!=children.end();
	     ++it) {
	  (*it)->calculate(level+1, limit);
	}
      }
    }

    LTLFormula_ptr _ltl;    
    LTL2DSTAR_Options _options;
    unsigned int priority;
    DRA_ptr _automaton;
    std::string _comment;
    LTL2DSTAR_Scheduler& _sched;
    
    std::vector<LTL2DSTAR_Tree*> children;
  };




  /** The root building block for the calculation of DRA/DSA */
  class LTL2DSTAR_Tree_Start : public LTL2DSTAR_Tree {
  public:
    /**
     * Constructor 
     * @param ltl The LTL formula
     * @param options the LTL2DSTAR options
     * @param sched a reference back to the scheduler 
     */
    LTL2DSTAR_Tree_Start(LTLFormula_ptr ltl, 
		       LTL2DSTAR_Options options,
		       LTL2DSTAR_Scheduler& sched) :
      LTL2DSTAR_Tree(ltl, options, sched) {
      generateTree();
    }
    
    /** Generate the tree */
    virtual void generateTree() {
      LTL2DSTAR_Tree_Rabin* rabin=0;
      LTL2DSTAR_Tree_Streett* streett=0;

      if (_options.automata==LTL2DSTAR_Options::RABIN ||
	  _options.automata==LTL2DSTAR_Options::RABIN_AND_STREETT) {
	rabin=new LTL2DSTAR_Tree_Rabin(_ltl,
				     _options,
				     _sched);
      }

      if (_options.automata==LTL2DSTAR_Options::STREETT ||
	  _options.automata==LTL2DSTAR_Options::RABIN_AND_STREETT) {
	streett=new LTL2DSTAR_Tree_Streett(_ltl->negate()->toPNF(),
					 _options,
					 _sched);
      }

      if (rabin!=0 && streett!=0) {
	unsigned int rabin_est = rabin->guestimate();
	unsigned int streett_est = streett->guestimate();
	
	if (_options.verbose_scheduler) {
	  std::cerr << "NBA-Estimates: Rabin: "<<rabin_est <<
	    " Streett: " << streett_est << std::endl;
	}
	
	if (rabin_est <= streett_est) {
	  addChild(rabin);
	  addChild(streett);
	} else {
	  addChild(streett);
	  addChild(rabin);
	}
      } else {
	if (rabin!=0)
	  addChild(rabin);
	if (streett!=0) 
	  addChild(streett);
      }

    
      if (_options.opt_safra.stutter) {
	StutterSensitivenessInformation::ptr stutter_information(new StutterSensitivenessInformation);
	stutter_information->checkLTL(_ltl);
	
	if (!stutter_information->isCompletelyInsensitive() && _options.opt_safra.partial_stutter_check) {
	  NBA_ptr nba, complement_nba;
	  if (rabin) {
	    nba=rabin->getNBA();
	  } else if (streett) {
	    nba=streett->getNBA();
	  }

	  if (rabin && streett) {
	    complement_nba=streett->getNBA();
	  }

	  if (!nba) {
	    stutter_information->checkPartial(*_ltl, _sched.getLTL2DRA());
	  } else if (!complement_nba) {
	    stutter_information->checkPartial(*nba, *_ltl->negate()->toPNF(), _sched.getLTL2DRA());
	  } else {
	    stutter_information->checkNBAs(*nba, *complement_nba);
	  }
	}
	
	if (rabin) {
	  rabin->setStutterInformation(stutter_information);
	}
	if (streett) {
	  streett->setStutterInformation(stutter_information);
	}
      }
    }
  };

  /** A building block for the calculation of a Rabin automaton 
   * (via Safra, Scheck or Union) */
  class LTL2DSTAR_Tree_Rabin : public LTL2DSTAR_Tree {
  public:
    /**
     * Constructor 
     * @param ltl The LTL formula
     * @param options the LTL2DSTAR options
     * @param sched a reference back to the scheduler 
     */

    LTL2DSTAR_Tree_Rabin(LTLFormula_ptr ltl, 
		       LTL2DSTAR_Options options,
		       LTL2DSTAR_Scheduler& sched) :
      LTL2DSTAR_Tree(ltl, options, sched),
      _tree_normal(0), _tree_union(0) {
      generateTree();
    }
    
    /** Estimate the size of the automaton (use the estimate of Safra's
     * building block ) */
    virtual unsigned int guestimate() {
      if (_tree_normal!=0) {
	return _tree_normal->guestimate();
      }

      return 0;
    }

    /** Hook after calculation */
    virtual void hook_after_calculate() {
      if (_tree_normal!=0 && _sched.flagStatNBA()) {
	_comment = _comment + std::string("+NBAstd=")+
	  (boost::lexical_cast<std::string>(guestimate()));
      }
    }

    /** Generate the tree */
    virtual void generateTree() {
      if (_options.scheck_path!="") {
	if (LTL2DSTAR_Tree_Scheck::worksWith(*_ltl, _options.verbose_scheduler)) {
	  addChild(new LTL2DSTAR_Tree_Scheck(_ltl, _options, _sched));

	}
	// add stuff for path. check here
      }

      if (_options.allow_union &&
	  _ltl->getRootNode()->getType() == LTLNode::T_OR) {
	_tree_union=new LTL2DSTAR_Tree_Union(_ltl, _options, _sched);
	addChild(_tree_union);
      }
      
      
      if (!((_options.only_union && _options.allow_union) ||
	    (_options.only_safety && _options.safety))) {
	_tree_normal=new LTL2DSTAR_Tree_Safra(_ltl, _options, _sched);
	addChild(_tree_normal);
      }
    }

    NBA_ptr getNBA() {
      if (_tree_normal) {
	return _tree_normal->getNBA();
      }
      return NBA_ptr((NBA_t*)0);
    }

    void setStutterInformation(StutterSensitivenessInformation::ptr stutter_information) {
      _stutter_information=stutter_information;
      if (_tree_normal) {
	_tree_normal->setStutterInformation(_stutter_information);
      }

      if (_tree_union) {
	_tree_union->setStutterInformation(_stutter_information);
      }
    }
  private:
    // memory will be freed by normal tree destructor
    LTL2DSTAR_Tree_Safra *_tree_normal;
    LTL2DSTAR_Tree_Union *_tree_union;

    StutterSensitivenessInformation::ptr _stutter_information;
  };


  /** Building block for the translation from LTL to DRA using Safra's algorithm */
  class LTL2DSTAR_Tree_Safra : public LTL2DSTAR_Tree {
  public:
    /**
     * Constructor 
     * @param ltl The LTL formula
     * @param options the LTL2DSTAR options
     * @param sched a reference back to the scheduler 
     */
    LTL2DSTAR_Tree_Safra(LTLFormula_ptr ltl, 
			LTL2DSTAR_Options options,
			LTL2DSTAR_Scheduler& sched) :
      LTL2DSTAR_Tree(ltl, options, sched) {
      generateTree();
    }

    /** Generate the tree */
    virtual void generateTree() {
    }

    /** Translate LTL -> NBA */
    void generateNBA() {
      if (_nba.get()==0) {
	_nba = _sched.getLTL2DRA().ltl2nba(*_ltl);
      }
    }

    NBA_ptr getNBA() {
      generateNBA();
      return _nba;
    }
    
    /** Estimate the size of the DRA (returns the size of the NBA) */
    virtual unsigned int guestimate() {
      generateNBA();
      if (_nba.get()!=0) {
	return _nba->size();
      }

      return 0;
    }

    /** Translate the LTL formula to DRA using Safra's algorithm */
    virtual void calculate(int level, unsigned int limit) {
      if (_options.verbose_scheduler) {
	std::cerr << "Calculate ("<< level <<"): " << typeid(*this).name() << std::endl;
	std::cerr << " Limit = " << limit << std::endl;
      }

      generateNBA();
      
      if (_nba.get()==0) {
	THROW_EXCEPTION(Exception, "Couldn't create NBA from LTL formula");
      };
      
      _automaton=_sched.getLTL2DRA().nba2dra(*_nba, 
					     limit, 
					     _options.detailed_states,
					     _stutter_information);
      _comment=std::string("Safra[NBA=")+
	boost::lexical_cast<std::string>(_nba->size())+
	"]";
      
      if (_options.optimizeAcceptance) {
	_automaton->optimizeAcceptanceCondition();
      }
      
      if (_options.bisim) {
	DRAOptimizations<DRA_t> dra_optimizer;
	_automaton=dra_optimizer.optimizeBisimulation(*_automaton,
						      false,
						      _options.detailed_states,
						      false);
      }
    }


    void setStutterInformation(StutterSensitivenessInformation::ptr stutter_information) {
      _stutter_information=stutter_information;
    }
  private:
    NBA_ptr _nba;

    StutterSensitivenessInformation::ptr _stutter_information;
  };
  

  /** Generate DRA by using the union construction on two DRAs */
  class LTL2DSTAR_Tree_Union : public LTL2DSTAR_Tree {
  public:
    /**
     * Constructor 
     * @param ltl The LTL formula
     * @param options the LTL2DSTAR options
     * @param sched a reference back to the scheduler 
     */

    LTL2DSTAR_Tree_Union(LTLFormula_ptr ltl, 
		       LTL2DSTAR_Options options,
		       LTL2DSTAR_Scheduler& sched) :
      LTL2DSTAR_Tree(ltl, options, sched), _left_tree(0), _right_tree(0) {
      
      _left=
	_ltl->getSubFormula(_ltl->getRootNode()->getLeft());
      
      _right=
	_ltl->getSubFormula(_ltl->getRootNode()->getRight());

      generateTree();
    }

    /**
     * Generate the tree
     */
    virtual void generateTree() {
      LTL2DSTAR_Options rec_opt=_options;
      rec_opt.recursion();
      _left_tree=new LTL2DSTAR_Tree_Rabin(_left, rec_opt, _sched);
      addChild(_left_tree);
      _right_tree=new LTL2DSTAR_Tree_Rabin(_right, rec_opt, _sched);
      addChild(_right_tree);
    }
    
    /**
     * Perform union construction
     */
    virtual void calculate(int level, unsigned int limit) {
      if (_options.verbose_scheduler) {
	std::cerr << "Calculate ("<< level <<"): " << typeid(*this).name() << std::endl;
      }

      try {
	  children[0]->calculate(level+1, limit);
	  children[1]->calculate(level+1, limit);
      } catch (LimitReachedException& e) {
	_automaton.reset();
	return;
      }      

      if (children[0]->_automaton.get()==0 ||
	  children[1]->_automaton.get()==0) {
	return;
      }

      bool union_trueloop=_sched.getLTL2DRA().getOptions().union_trueloop;
      if (_sched.getLTL2DRA().getOptions().stutter) {
	_automaton=DRA_t::calculateUnionStuttered(*children[0]->_automaton,
						  *children[1]->_automaton,
						  _stutter_information,
						  union_trueloop,
						  _options.detailed_states);
      } else {
	_automaton=DRA_t::calculateUnion(*children[0]->_automaton,
					 *children[1]->_automaton,
					 union_trueloop,
					 _options.detailed_states);
	/*      _automaton=DRAOperations::dra_union(*children[0]->_automaton, 
					  *children[1]->_automaton,
					  union_trueloop,
					  _options.detailed_states); */
      }
      _comment=std::string("Union{")+
	children[0]->_comment+","+
	children[1]->_comment+"}";

      if (_options.optimizeAcceptance) {
	_automaton->optimizeAcceptanceCondition();
      }

      if (_options.bisim) {
	DRAOptimizations<DRA_t> dra_optimizer;
	_automaton=dra_optimizer.optimizeBisimulation(*_automaton,
						      false,
						      _options.detailed_states,
						      false);
      }

      hook_after_calculate();
    }
    

    void setStutterInformation(StutterSensitivenessInformation::ptr stutter_information) {
      _stutter_information=stutter_information;

      StutterSensitivenessInformation::ptr 
	left_stutter_info(new StutterSensitivenessInformation(*stutter_information));
      StutterSensitivenessInformation::ptr 
	right_stutter_info(new StutterSensitivenessInformation(*stutter_information));

      if (!stutter_information->isCompletelyInsensitive()) {
	left_stutter_info->checkLTL(_left);
	right_stutter_info->checkLTL(_right);
      }
      
      if (!left_stutter_info->isCompletelyInsensitive()) {
	left_stutter_info->checkPartial(*_left_tree->getNBA(), 
					*_left->negate()->toPNF(), 
					_sched.getLTL2DRA());
      }

      if (!right_stutter_info->isCompletelyInsensitive()) {
	right_stutter_info->checkPartial(*_right_tree->getNBA(), 
					 *_right->negate()->toPNF(), 
					 _sched.getLTL2DRA());
      }

      _left_tree->setStutterInformation(left_stutter_info);
      _right_tree->setStutterInformation(right_stutter_info); 
    }

    LTL2DSTAR_Tree_Rabin *_left_tree;
    LTL2DSTAR_Tree_Rabin *_right_tree;
    
    LTLFormula_ptr _left;
    LTLFormula_ptr _right;

    StutterSensitivenessInformation::ptr _stutter_information;
  };


  /**
   * Generate Streett automaton by calculating the Rabin automaton
   * for the negated formula
   */  
  class LTL2DSTAR_Tree_Streett : public LTL2DSTAR_Tree {
  public:
    /**
     * Constructor 
     * @param ltl The LTL formula
     * @param options the LTL2DSTAR options
     * @param sched a reference back to the scheduler 
     */

    LTL2DSTAR_Tree_Streett(LTLFormula_ptr ltl, 
			 LTL2DSTAR_Options options, 
			 LTL2DSTAR_Scheduler& sched) :
      LTL2DSTAR_Tree(ltl, options, sched) {
      generateTree();
    }
    
    /** Estimate automaton size (use estimate of Rabin building block) */
    virtual unsigned int guestimate() {
      if (children[0]!=0) {
	return children[0]->guestimate();
      }

      return 0;
    }

    NBA_ptr getNBA() {
      if (_tree_rabin) {
	return _tree_rabin->getNBA();
      }
      return NBA_ptr((NBA_t*)0);
    }
    

    /** Generate tree */
    virtual void generateTree() {
      LTL2DSTAR_Options opt=_options;
      opt.automata=LTL2DSTAR_Options::RABIN;
      opt.scheck_path=""; // disable scheck
      _tree_rabin=new LTL2DSTAR_Tree_Rabin(_ltl, 
					   opt,
					   _sched);
      addChild(_tree_rabin);
    }    

    /** Calculate */
    virtual void calculate(int level, unsigned int limit) {
      if (_options.verbose_scheduler) {
	std::cerr << "Calculate ("<< level <<"): " << typeid(*this).name() << std::endl;
      }

      try {
	children[0]->calculate(level, limit);
      } catch (LimitReachedException& e) {
	_automaton.reset();
	return;
      }

      _automaton=children[0]->_automaton;
      _comment=std::string("Streett{")+
	children[0]->_comment+"}";

      if (_automaton.get()!=0) {
	_automaton->considerAsStreett();
      }

      hook_after_calculate();
    }

    void setStutterInformation(StutterSensitivenessInformation::ptr stutter_information) {
      _stutter_information=stutter_information;
      _tree_rabin->setStutterInformation(stutter_information);
    }

  private:
    LTL2DSTAR_Tree_Rabin* _tree_rabin;
    StutterSensitivenessInformation::ptr _stutter_information;
  };


  /**
   * Use Scheck to generate a DBA (transformed into a DRA) for a (co-)safe LTL formula
   */
  class LTL2DSTAR_Tree_Scheck : public LTL2DSTAR_Tree {
  public:
    /**
     * Constructor 
     * @param ltl The LTL formula
     * @param options the LTL2DSTAR options
     * @param sched a reference back to the scheduler 
     */

     LTL2DSTAR_Tree_Scheck(LTLFormula_ptr ltl, 
			 LTL2DSTAR_Options options,
			 LTL2DSTAR_Scheduler& sched) :
       LTL2DSTAR_Tree(ltl, options, sched) {
       generateTree();
     }

    /** Check if the formula is syntactically (co-)safe */
    static bool worksWith(LTLFormula& ltl, bool verbose=false) {
      if (ltl.isSafe()) {
	if (verbose) {
	  std::cerr << "Formula is safe" << std::endl;
	}
	return true;
      } else if (ltl.isCoSafe()) {
	if (verbose) {
	  std::cerr << "Formula is cosafe" << std::endl;
	}
	return true;
      }
      return false;
    }
    
    /** Generate tree */
    virtual void generateTree() {
    }

    /** Calculate */
    virtual void calculate(int level, unsigned int limit) {
      if (_options.verbose_scheduler) {
	std::cerr << "Calculate ("<< level <<"): " << typeid(*this).name() << std::endl;
      }

       LTLSafetyAutomata lsa;
       _automaton=lsa.ltl2dra<DRA_t>(*_ltl, _options.scheck_path);

       if (_automaton.get()==0) {return;}
       _comment=std::string("Scheck");

       if (_options.optimizeAcceptance) {
	 _automaton->optimizeAcceptanceCondition();
       }
      
       /* not really necessary with scheck
	 if (_options.bisim) {
	 DRAOptimizations<DRA_t> dra_optimizer;
	 _automaton=dra_optimizer.optimizeBisimulation(*_automaton,
						      false,
						      _options.detailed_states,
						      false);
	} */

       hook_after_calculate();
     }
  };


};




/** Calculate the new limit using factor alpha (returns 0 if no limit) */
unsigned int LTL2DSTAR_Scheduler::calcLimit(unsigned int limit) {
  if (limit==0) {return limit;}
  if (flagOptLimits()) {
    double new_limit=(limit*_alpha)+1.0;
    if (new_limit > (std::numeric_limits<unsigned int>::max)()) {
      limit = 0;
    } else {
      limit=(unsigned int)new_limit;
    }
  }

  return limit;
}

/** Constructor
 * @param ltl2dra the wrapper for LTL->NBA and NBA->DRA 
 * @param opt_limits use limiting?
 * @param alpha the limiting factor 
*/
LTL2DSTAR_Scheduler::LTL2DSTAR_Scheduler(LTL2DRA& ltl2dra,
				     bool opt_limits,
				     double alpha) :
  _ltl2dra(ltl2dra), _opt_limits(opt_limits), _alpha(alpha),
  _stat_NBA(false)
{}


/** 
 * Generate a DRA/DSA for the LTL formula 
 */
DRA_ptr LTL2DSTAR_Scheduler::calculate(LTLFormula& ltl, LTL2DSTAR_Options ltl_opt) {
  LTLFormula_ptr ltl_p(ltl.toPNF());
  
  if (ltl_opt.verbose_scheduler) {
    std::cerr << ltl_p->toStringInfix() << std::endl;
  }

  LTL2DSTAR_Scheduler_Trees::LTL2DSTAR_Tree_Start root(ltl_p, ltl_opt, *this);
  
  if (ltl_opt.verbose_scheduler) {
    root.printTree(std::cerr);
  }
  
  root.calculate();

  DRA_ptr result=root._automaton;
  if (result.get()!=0) {
    result->setComment(root._comment+getTimingInformation());
  }
  return result;
}

std::string LTL2DSTAR_Scheduler::getTimingInformation() {
  if (StutterSensitivenessInformation::hasTimekeeping()) {
    unsigned long ms=StutterSensitivenessInformation::getTimeKeeper().getElapsedMilliseconds();
    return " TIME(stuttercheck)=:"+ 
      boost::lexical_cast<std::string>(ms)+
      ":";
  } else {
    return "";
  }
}

/** Get the LTL2DRA wrapper class */
LTL2DRA& LTL2DSTAR_Scheduler::getLTL2DRA() {
  return _ltl2dra;
}
