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


#ifndef SAFRASALGORITHM_H
#define SAFRASALGORITHM_H

/** @file 
 * Provides an implementation of the transition function of Safra's algorithm. 
 */

#include "SafraTree.hpp"
#include "SafraTreeTemplate.hpp"
#include "NBAAnalysis.hpp"
#include "common/BitSet.hpp"
#include "common/BitSetIterator.hpp"
#include "GraphAlgorithms.hpp"
#include "Configuration.hpp"
#include <vector>


namespace SafrasAlgorithmInternal {

  /**
   * Safra tree visitor that resets the final flag on the Safra tree node.
   */
  class STVisitor_reset_final_flag {
  public:
    /** Node visitor */
    void visit(SafraTree& tree, SafraTreeNode *node) {
      node->setFinalFlag(false);
    }
  };

  /**
   * Safra tree visitor that creates a new child node if
   * the label of the node and the set of final states in the
   * NBA intersect.
   */
  class STVisitor_check_finalset {
  public:
    /**
     * Constructor.
     * @param final_states the states that are accepting (final) in the NBA
     * @param tree_template the tree template to keep track of new nodes
     */
    STVisitor_check_finalset(const BitSet& final_states,
			     SafraTreeTemplate& tree_template) :
      _final_states(final_states),
      _tree_template(tree_template) {}

    /** */
    void visit(SafraTree& tree, SafraTreeNode *node) {
      if (_final_states.intersects(node->getLabeling())) {
	BitSet q_and_f(_final_states);
	q_and_f.Intersect(node->getLabeling());

	SafraTreeNode *new_child=tree.newNode();
	node->addAsYoungestChild(new_child);

	_tree_template.setRenameable(new_child->getID());

	new_child->getLabeling()=q_and_f;
      }
    }
  private:
    const BitSet& _final_states;
    SafraTreeTemplate& _tree_template;
  };

  /**
   * Safra tree visitor that performs the powerset construction
   * on the label of the Safra tree node.
   */
  template <typename NBA_t>
  class STVisitor_powerset {
  public:
    /**
     * Constructor.
     */
    STVisitor_powerset(NBA_t& nba, APElement elem)
      : _nba(nba), _elem(elem) {}

    /** Node visitor */  
    void visit(SafraTree& tree, SafraTreeNode *node) {
      BitSet new_labeling;
      BitSet& old_labeling=node->getLabeling();
      for (int i=old_labeling.nextSetBit(0);
	   i!=-1;
	   i=old_labeling.nextSetBit(i+1)) {
	new_labeling.Union(*(_nba[i]->getEdge(_elem)));
      }

      node->getLabeling()=new_labeling;
    }
  
  private:
    NBA_t& _nba;
    APElement _elem;
  };

  /**
   * A Safra tree visitor that subtracts (minus operator) a BitSet from
   * the label of the tree node.
   */
  class STVisitor_subtract_labeling {
  public:
    STVisitor_subtract_labeling(BitSet &bitset) : _bitset(bitset) {}
    void visit(SafraTree& tree, SafraTreeNode *node) {
      node->getLabeling().Minus(_bitset);
    }  
  private:
    BitSet &_bitset;
  };

  /**
   * A Safra tree visitor that removes all 
   * children of the node.
   */
  class STVisitor_remove_subtree {
  public:
    STVisitor_remove_subtree(SafraTreeTemplate& tree_template) :
      _tree_template(tree_template) {}

    /** Node visitor */
    void visit(SafraTree& tree, SafraTreeNode *node) {
      unsigned int id=node->getID();
      if (_tree_template.isRenameable(id)) {
	// this node was created recently, so we only delete it from
	// the renameableNames, but don't mark it in restrictedNames
	_tree_template.setRenameable(id, false);
      } else {
	_tree_template.setRestricted(id);
      }

      tree.remove(node);
    }

  private:
    SafraTreeTemplate& _tree_template;
  };


  /**
   * A Safra tree visitor that modifies the
   * children so that all children have
   * disjoint labels
   */
  class STVisitor_check_children_horizontal {
  public:
    /** Node visitor */
    void visit(SafraTree& tree, SafraTreeNode *node) {
      if (node->getChildCount()<=1) {
	return;
      }

      BitSet already_seen;
      bool first=true;
      for (SafraTreeNode::child_iterator it=node->children_begin();
	   it!=node->children_end();
	   ++it) {
	SafraTreeNode *cur_child=*it;
	if (first) {
	  already_seen=cur_child->getLabeling();
	  first=false;
	} else {
	  BitSet& current=cur_child->getLabeling();
	
	  BitSet intersection=already_seen; // make copy
	  if (intersection.intersects(current)) {
	    // There are some labels, which occur in older brothers,
	    // remove them from current node and its children
	    STVisitor_subtract_labeling stv_sub(intersection);
	    tree.walkSubTreePostOrder(stv_sub, cur_child);
	  }

	  already_seen.Union(current);
	}
      }
    }
  };



  /**
   * A Safra tree visitor that ensures that 
   * the union of the labels of the children
   * are a proper subset of the label of the
   * parents. Otherwise, the children are
   * removed and the final flag is set on
   * the tree node.
   */
  class STVisitor_check_children_vertical {
  public:
    STVisitor_check_children_vertical(SafraTreeTemplate& tree_template) :
      _tree_template(tree_template) {}

    /** Node visitor */
    void visit(SafraTree& tree, SafraTreeNode *node) {
      if (node->getChildCount()==0) {return;}

      BitSet labeling_union;
      for (SafraTreeNode::child_iterator it=node->children_begin();
	   it!=node->children_end();
	   ++it) {
	labeling_union.Union((*it)->getLabeling());
      }

      if (labeling_union == node->getLabeling()) {
	// The union of the labelings of the children is exactly the 
	// same as the labeling of the parent ->
	//  remove children
	STVisitor_remove_subtree stv_remove(_tree_template);
	tree.walkChildrenPostOrder(stv_remove, node);

	node->setFinalFlag(true);
      }
    }


  private:
    SafraTreeTemplate& _tree_template;
  };


  /**
   * Safra tree visitor that attempts
   * to reorder the independant children 
   * into a canonical order.
   * Two children are independet if
   * their is no state that is reachable by 
   * states in both labels.
   */
  class STVisitor_reorder_children {
  public:
    /**
     * Constructor
     * nba_reachability A vector of BitSets (state index -> BitSet) of states
     *                  in the NBA that are reachable from a state.
     * N                the maximum number of nodes in the Safra tree
     */
    STVisitor_reorder_children(std::vector<BitSet>& nba_reachability, 
			       unsigned int N) :
      _nba_reachability(nba_reachability), _n(N) {
      _node_order=new unsigned int[N];
      _node_reachability=new BitSet[N];
    }

    /** Destructor */
    ~STVisitor_reorder_children() {
      delete[] _node_order;
      delete[] _node_reachability;
    }

    /** Node visitor */
    void visit(SafraTree& tree, SafraTreeNode *node) {
      if (node->getChildCount()<=1) {return;}

      unsigned int i=0;
      for (SafraTreeNode::child_iterator it=
	     node->children_begin();
	   it!=node->children_end();
	   ++it) {
	BitSet& reachable_this=_node_reachability[(*it)->getID()];
	reachable_this.clear();
	_node_order[(*it)->getID()]=i++;

	BitSet& label_this=(*it)->getLabeling();
	for (BitSetIterator label_it(label_this);
	     label_it!=BitSetIterator::end(label_this);
	     ++label_it) {
	  reachable_this.Union(_nba_reachability[*label_it]);
	}
      
	//      std::cerr << "reachability_this: "<<reachable_this << std::endl; 
      }


      // reorder...
      //    std::cerr << "Sorting!" << std::endl;
    
      // Bubble sort, ough!
      bool finished=false;
      while (!finished) {
	finished=true;
      
	for (SafraTreeNode *a=node->getOldestChild();
	     a!=0 && a->getYoungerBrother()!=0;
	     a=a->getYoungerBrother()) {

	  SafraTreeNode* b=a->getYoungerBrother();

	  BitSet &reach_a=_node_reachability[a->getID()];
	  BitSet &reach_b=_node_reachability[b->getID()];

	  if (reach_a.intersects(reach_b)) {
	    // a and b are not independant...
	    // --> keep relative order...
	    assert(_node_order[a->getID()] < _node_order[b->getID()]);
	  } else {
	    // a and b are independant...
	    if (! (a->getLabeling() < 
		   b->getLabeling())) {
	      // swap
	      node->swapChildren(a,b);
	      a=b;
	      finished=false;	    
	    }
	  }
	}
      }
    }

  private:
    std::vector<BitSet>& _nba_reachability;
    BitSet* _node_reachability;
    unsigned int* _node_order;
    unsigned int _n;
  };


  /**
   * A Safra tree visitor that removes tree nodes
   * with empty labels.
   */
  class STVisitor_remove_empty {
  public:
    STVisitor_remove_empty(SafraTreeTemplate& tree_template) :
      _tree_template(tree_template) {}

    /** Node visitor */
    void visit(SafraTree& tree, SafraTreeNode *node) {
      if (node->getLabeling().isEmpty()) {
	unsigned int id=node->getID();
	if (_tree_template.isRenameable(id)) {
	  // this node was created recently, so we only delete it in
	  // renameableNames, but don't mark it in restrictedNodes
	  _tree_template.setRenameable(id, false);
	} else {
	  _tree_template.setRestricted(id);
	}
      
	tree.remove(node);
      }
    }

  private:
    SafraTreeTemplate& _tree_template;
  };


  /**
   * A Safra tree visitor that checks if all
   * the successor states in the NBA of the label
   * are accepting. If this is the case, all
   * children are removed, and the final flag is set.
   */
  class STVisitor_check_for_succ_final {
  private:
    bool _success;
    BitSet const& _nba_states_with_all_succ_final;
    SafraTreeTemplate& _tree_template;
  public:

    /** 
     * Constructor 
     * @param nba_states_with_all_succ_final A BitSet with the indizes of the
     *                                       NBA states that only have accepting (final)
     *                                       successors.
     * @param tree_template                  SafraTreeTemplate to keep track of removed nodes
     */
    STVisitor_check_for_succ_final(const BitSet& nba_states_with_all_succ_final, SafraTreeTemplate& tree_template) 
      : _success(false), 
	_nba_states_with_all_succ_final(nba_states_with_all_succ_final),
	_tree_template(tree_template)  {}
  
    /** Returns true if the condition was triggered. */
    bool wasSuccessful() {return _success;}

    /** Node visitor */
    void visit(SafraTree& tree, SafraTreeNode *node) {

      bool all_final=true;
      for (BitSetIterator it=BitSetIterator(node->getLabeling());
	   it!=BitSetIterator::end(node->getLabeling());
	   ++it) {
	if (!_nba_states_with_all_succ_final.get(*it)) {
	  all_final=false;
	  break;
	}
      }

      if (all_final) {
	// remove all children of node & set final flag
	STVisitor_remove_subtree stv_remove(_tree_template);
	tree.walkChildrenPostOrder(stv_remove, node);
      
	node->setFinalFlag();

	_success=true;
      }

    }
  };

};





/**
 * A class which calculates the transition function of Safra's algorithm.
 * You have to provide at least the final states of the NBA, the other
 * values have to be set, if the corresponding option is set.
 */
template <typename NBA_t>
class SafrasAlgorithm {
public:
  typedef NBAAnalysis<NBA_t> NBAAnalysis_t;

  SafrasAlgorithm(NBA_t& nba, const Options_Safra &options);
  ~SafrasAlgorithm();

  
  typedef SafraTreeTemplate_ptr result_t;
  typedef SafraTree_ptr state_t;

  result_t delta(state_t tree, APElement elem) {
    return process(*tree, elem);
  }

  state_t getStartState() {
    SafraTree_ptr start(new SafraTree(_NODES));
    if (_nba.getStartState()!=0) {
      start->getRootNode()->getLabeling().set(_nba.getStartState()->getName());
    }

    return start;
  }

  void prepareAcceptance(RabinAcceptance& acceptance) {
    acceptance.newAcceptancePairs(_NODES);
  }

  SafraTreeTemplate_ptr process(const SafraTree& tree, 
				APElement element);
 
  bool checkEmpty() {
    if (_nba.size()==0 ||
	_nba.getStartState()==0) {
      return true;
    }
    return false;
  }

private:
  Options_Safra _options;
  NBAAnalysis_t _nba_analysis;
  NBA_t& _nba;
  unsigned int _NODES;

  std::vector<BitSet*> _next;

  /** Cacheing the STVisitor_reorder_children, as it's initialization is complex. */
  SafrasAlgorithmInternal::STVisitor_reorder_children* stv_reorder;
};



/**
 * Constructor
 * @param nba The NBA
 * @param options The options for Safra's algorithm
 */
template <typename NBA_t>
SafrasAlgorithm<NBA_t>::SafrasAlgorithm(NBA_t& nba, const Options_Safra &options):
  _options(options), 
  _nba_analysis(nba),
  _nba(nba),
  _NODES(2*nba.getStateCount()),
  stv_reorder(0) 
{
  _next.resize(nba.getStateCount());
}

/** Destructor */
template <typename NBA_t>
SafrasAlgorithm<NBA_t>::~SafrasAlgorithm() {
  delete stv_reorder;
}

/** 
 * Get the next Safra tree using the
 * transition function as described by Safra.
 * @param tree the original tree
 * @param elem the edge label
 * @return a SafraTreeTemplate containing the new tree and 
 *         bookkeeping which states were created/deleted.
 */
template <typename NBA_t>
SafraTreeTemplate_ptr
SafrasAlgorithm<NBA_t>::process(SafraTree const& tree, 
				APElement elem) {
  // Make copy of original tree
  SafraTree_ptr cur(new SafraTree(tree));

  SafraTreeTemplate_ptr tree_template(new SafraTreeTemplate(cur));

  SafrasAlgorithmInternal::STVisitor_reset_final_flag stv_reset_flag;
  cur->walkTreePostOrder(stv_reset_flag);

#ifdef NBA2DRA_POWERSET_FIRST
  SafrasAlgorithmInternal::STVisitor_powerset<NBA_t> stv_powerset(_nba, elem);
  cur->walkTreePostOrder(stv_powerset);

  SafrasAlgorithmInternal::STVisitor_check_finalset stv_final(_nba_analysis.getFinalStates(),
				     *tree_template);
  cur->walkTreePostOrder(stv_final);
#else
  SafrasAlgorithmInternal::STVisitor_check_finalset stv_final(_nba_analysis.getFinalStates(),
				     *tree_template);
  cur->walkTreePostOrder(stv_final);

  SafrasAlgorithmInternal::STVisitor_powerset<NBA_t> stv_powerset(_nba, elem);
  cur->walkTreePostOrder(stv_powerset);
#endif


  /*
   * Optimization: ACCEPTING_TRUE_LOOPS
   */
  if (_options.opt_accloop) {
    if (cur->getRootNode() != 0) {
      SafraTreeNode *root=cur->getRootNode();

      if (_nba_analysis.getStatesWithAcceptingTrueLoops().intersects(root->getLabeling())) {
	// True Loop
	SafrasAlgorithmInternal::STVisitor_remove_subtree stv_remove(*tree_template);
	cur->walkChildrenPostOrder(stv_remove, root);

	root->getLabeling().clear();
	
	unsigned int canonical_true_loop=
	  _nba_analysis.getStatesWithAcceptingTrueLoops().nextSetBit(0);
	root->getLabeling().set(canonical_true_loop);
	root->setFinalFlag(true);
	
	return tree_template;
      }
    }
  }


  SafrasAlgorithmInternal::STVisitor_check_children_horizontal stv_horizontal;
  cur->walkTreePostOrder(stv_horizontal);

  SafrasAlgorithmInternal::STVisitor_remove_empty stv_empty(*tree_template);
  cur->walkTreePostOrder(stv_empty);

  SafrasAlgorithmInternal::STVisitor_check_children_vertical stv_vertical(*tree_template);
  cur->walkTreePostOrder(stv_vertical);


  /*
   * Optimization: REORDER
   */
  if (_options.opt_reorder) {
    if (stv_reorder==0) {
      stv_reorder=new SafrasAlgorithmInternal::STVisitor_reorder_children(_nba_analysis.getReachability(), cur->getNodeMax());
    }

    cur->walkTreePostOrder(*stv_reorder);
  }



  /*
   * Optimization: ALL SUCCESSORS ARE ACCEPTING
   */
  if (_options.opt_accsucc) {
    SafrasAlgorithmInternal::STVisitor_check_for_succ_final 
      stv_succ(_nba_analysis.getStatesWithAllSuccAccepting(), 
	       *tree_template);
    
    cur->walkTreePostOrder(stv_succ);
    if (stv_succ.wasSuccessful()) {
  #ifdef NBA2DRA_MERGE
      if (stv_succ.wasMerged()) {
	SafrasAlgorithmInternal::STVisitor_remove_empty stv_empty;
	cur->walkTreePostOrder(stv_empty);
	
	SafrasAlgorithmInternal::STVisitor_check_children_vertical stv_vertical;
	cur->walkTreePostOrder(stv_vertical);
      }
  #endif   // NBA2DRA_MERGE
    }
  }

  return tree_template;
}



#endif
