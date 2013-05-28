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


#ifndef STUTTEREDNBA2DA_H
#define STUTTEREDNBA2DA_H


/** @file
 * Provides class StutteredNBA2DA.
 */

#include "common/HashFunction.hpp"
#include "StutterSensitivenessInformation.hpp"

// debug:
// #define STUTTERED_VERBOSE 1


/** 
 * A class representing a tree (state in the original automaton) augmented with an AcceptanceSignature. 
 * Tree should be a shared_ptr type.
 */
template <typename Tree, typename AcceptanceSignature> 
class TreeWithAcceptance {
public:
  /** Constructor. The acceptance signature is initialized from the tree.
   * @param tree the Tree
   */
  TreeWithAcceptance(Tree tree) : 
    _tree(tree), _signature(*tree) {
  }

  /** Get the tree.
   * @return the Tree.
   */
  const Tree& getTree() const {return _tree;}
  
  /** Get the AcceptanceSignature. 
   * @return the AcceptanceSignature */
  const AcceptanceSignature& getSignature() const {return _signature;}

  /** Get the AcceptanceSignature. 
   * @return the AcceptanceSignature */
  AcceptanceSignature& getSignature() {return _signature;}

  /** A shared_ptr to a TreeWithAcceptance */
  typedef boost::shared_ptr<TreeWithAcceptance<Tree,AcceptanceSignature> > ptr;    

  /** Check if this TreeWithAcceptance is equal to another.
   * @param other the other TreeWithAcceptance.
   * @return true iff the two are equal
   */
  bool operator==(const TreeWithAcceptance& other) const {
    return (*_tree == *other.getTree()) && (_signature == other.getSignature());
  }

  /** Check if this TreeWithAcceptance is 'smaller' than another.
   * @param other the other TreeWithAcceptance.
   * @return true iff this is smaller than other
   */
  bool operator<(const TreeWithAcceptance& other) const {
    if (*_tree < *other.getTree()) {
      return true;
    }
    if (*_tree == *other.getTree()) {
      return _signature < other.getSignature();
    }
    return false;
  }

  /** Generate short description of this TreeWithAcceptance
   * @return the description */
  std::string toString() {
    return _tree->toString()+_signature.toString();
  }


  /**
   * Calculate a hash value using HashFunction
   * @param hashfunction the HashFunction functor
   */
  template <class HashFunction>
  void hashCode(HashFunction& hashfunction) {
    _tree->hashCode(hashfunction);
    _signature.hashCode(hashfunction);
  }


  /**
   * Generate the appropriate acceptance signature for Rabin Acceptance for this object
   * @param acceptance the AcceptanceForState accessor to which the signature is copied
   */
  void generateAcceptance(RabinAcceptance::AcceptanceForState acceptance) {
    acceptance.setSignature(_signature);
  }


  /**
   * Generate the appropriate acceptance signature for Rabin Acceptance for this object
   * @param acceptance the acceptance signature to which the signature for this state is copied
   */
  void generateAcceptance(RabinAcceptance::RabinSignature& acceptance) {
    acceptance=_signature;
  }

  /**
   * Set the description for this state.
   * @param description the description
   */
  void setDescription(const std::string& description) {
    _description=description;
  }


  /**
   * Get the description for this state.
   * @return description the description
   */
  const std::string& getDescription() const {
    return _description;
  }


  /**
   * Get the description for this state.
   * @return description the description
   */
  std::string toHTML() {
    if (_description=="") {
      return _tree->toHTML();
    } else {
      return _description;
    }
  }

private:
  /** The tree */
  const  Tree _tree;
  
  /** The acceptance signature */
  AcceptanceSignature _signature;

  /** The description */
  std::string _description;
};


/** Calculate stuttered DA_t given Algorithm_t */
template <typename Algorithm_t,
	  typename DA_t>
class StutteredNBA2DA {
private:
  /** Generate detailed descriptions for the states? */
  bool _detailed_states;
  /** Information which symbols may be stuttered */
  StutterSensitivenessInformation::ptr _stutter_information;

public:
  typedef typename DA_t::acceptance_condition_type Acceptance;

  /** Constructor.
   * detailed_states Generate detailed descriptions for the states? 
   * stutter_information Information which symbols may be stuttered 
   */
  StutteredNBA2DA(bool detailed_states, 
		  StutterSensitivenessInformation::ptr stutter_information) :
    _detailed_states(detailed_states), 
    _stutter_information(stutter_information) {
    assert(_stutter_information);
  }
  
  ~StutteredNBA2DA() {};
  
  /**
   * Perform the stuttered conversion.
   * Throws LimitReachedException if a limit is set (>0) and
   * there are more states in the generated DRA than the limit. 
   * @param algo the underlying algorithm to be used
   * @param da_result the DRA where the result is stored 
   *        (has to have same APSet as the nba)
   * @param limit a limit for the number of states (0 disables the limit).
   */
  void convert(Algorithm_t& algo,
	       DA_t& da_result,
	       unsigned int limit=0) {
    StutteredConvertor conv(algo, da_result, limit, _detailed_states, _stutter_information);
    conv.convert();
  }


  /** 
   * Converting an NBA using Algorithm_t into a DA
   */
  class StutteredConvertor {
  public:
    /** Constructor.
     * @param algo The Algorithm_t to use
     * @param da_result The result automaton
     * @param limit Limit the number of states in the result automaton?
     * @param detailed_states Generate detailed descriptions?
     * @param stutter_information Information about which symbols may be stuttered
     */
    StutteredConvertor(Algorithm_t& algo, 
		       DA_t& da_result, 
		       unsigned int limit, 
		       bool detailed_states,
		       StutterSensitivenessInformation::ptr stutter_information) :
      _da_result(da_result), 
      _limit(limit), 
      _algo(algo),
      _detailed_states(detailed_states),
      _stutter_information(stutter_information) {
    }

    typedef typename DA_t::state_type da_state_t;
    typedef typename Algorithm_t::state_t algo_state_t;
    typedef typename Algorithm_t::result_t algo_result_t;
    typedef TreeWithAcceptance<algo_state_t, typename Acceptance::signature_type> stuttered_state_t;
    typedef typename stuttered_state_t::ptr stuttered_state_ptr_t;
    
    typedef std::pair<stuttered_state_ptr_t, da_state_t*> unprocessed_value_t;
    typedef std::stack<unprocessed_value_t> unprocessed_stack_t;
  
    /** Convert the NBA to the DA */
    void convert() {      
      const APSet& ap_set=_da_result.getAPSet();

      if (_algo.checkEmpty()) {
	_da_result.constructEmpty();
	return;
      }
      
      _algo.prepareAcceptance(_da_result.acceptance());
     
      stuttered_state_ptr_t s_start( new stuttered_state_t( _algo.getStartState() ));
      da_state_t* start_state=_da_result.newState();
      s_start->generateAcceptance(start_state->acceptance());
      if (_detailed_states) {
	start_state->setDescription(s_start->toHTML());
      }
      
      _state_mapper.add(s_start, start_state);
      _da_result.setStartState(start_state);
      
      unprocessed_stack_t unprocessed;
      _unprocessed.push(unprocessed_value_t(s_start, start_state));

      bool all_insensitive=_stutter_information->isCompletelyInsensitive();
      const BitSet& partial_insensitive=
	_stutter_information->getPartiallyInsensitiveSymbols();
      
      while (!_unprocessed.empty()) {
	unprocessed_value_t top=_unprocessed.top();
	_unprocessed.pop();      
	
	stuttered_state_ptr_t from=top.first;
	da_state_t *da_from=top.second;
	
	for (APSet::element_iterator it_elem=ap_set.all_elements_begin();
	     it_elem!=ap_set.all_elements_end();
	     ++it_elem) {
	  APElement elem=*it_elem;

	  if (!da_from->edges().get(elem)) {
	    // the edge was not yet calculated...

	    if (!all_insensitive &&
		!partial_insensitive.get(elem)) {
	      // can't stutter for this symbol, do normal step
	      algo_state_t next_tree=_algo.delta(from->getTree(), elem)->getState();
	      stuttered_state_ptr_t next_state=stuttered_state_ptr_t(new stuttered_state_t(next_tree));
	      add_transition(da_from, next_state, elem);
	      
	      continue;
	    }

	    // normal stuttering...

	    calc_delta(from, da_from, elem);
	    
	    if (_limit!=0 && _da_result.size()>_limit) {
	      THROW_EXCEPTION(LimitReachedException, "");
	    }
	  }
	}
      }
    }
    
    
  private:
    // ---- members ----
    /** The result DA */
    DA_t& _da_result;
    /** Limit for the number of states */
    unsigned int _limit;
    /** The algorithm to use */
    Algorithm_t& _algo;
    /** Generate detailed descriptions? */
    bool _detailed_states;
    /** A state mapper for the already generated states */
    StateMapper<int, stuttered_state_ptr_t, typename DA_t::state_type> _state_mapper;
    /** Information about which symbols are safe to stutter */
    StutterSensitivenessInformation::ptr _stutter_information;
    /** A stack for the unprocessed states */
    unprocessed_stack_t _unprocessed;
    
    // --- private functions ----

    /** Add a transition from DA states da_from to stuttered_state to for edge elem.
     * If the state does not yet exist in the DA, create it.
     * @param da_from the from state in the DA
     * @param to the target state 
     * @param elem the edge label
     */
    da_state_t* add_transition(da_state_t* da_from, stuttered_state_ptr_t to, APElement elem) {
      da_state_t* da_to=_state_mapper.find(to);

      if (!da_to) {
	da_to=_da_result.newState();
	to->generateAcceptance(da_to->acceptance());
	if (_detailed_states) {
	  da_to->setDescription(to->toHTML());
	}
      
	_state_mapper.add(to, da_to);
	_unprocessed.push(unprocessed_value_t(to, da_to));
      }

#ifdef STUTTERED_VERBOSE
      std::cerr << da_from->getName() << " -> " << da_to->getName() << std::endl;
#endif

      da_from->edges().set(elem, da_to);
      
      return da_to;
    }

    typedef std::vector<algo_state_t> intermediate_state_vector_t;
    
    /**
     * Calculate Acceptance for RabinAcceptance conditon
     */
    bool calculate_acceptance(intermediate_state_vector_t& state_vector,
			      unsigned int cycle_point,
			      RabinAcceptance::RabinSignature* prefix_signature,
			      RabinAcceptance::RabinSignature* cycle_signature) {
      unsigned int states=state_vector.size();

      state_vector[cycle_point]->generateAcceptance(*cycle_signature); // start
      for (unsigned int i=cycle_point+1;i<states;i++) {
	cycle_signature->maxMerge(state_vector[i]->generateAcceptance());	
      }

      if (prefix_signature) {
	*prefix_signature=*cycle_signature;
	for (unsigned int i=1;i<cycle_point;i++) {
	  prefix_signature->maxMerge(state_vector[i]->generateAcceptance());	
	}
      }

      if (prefix_signature) {
	// check if prefix can be ommited
	RabinAcceptance::RabinSignature p0_signature(prefix_signature->getSize());
	state_vector[0]->generateAcceptance(p0_signature);

	for (unsigned int j=0;j<prefix_signature->getSize();j++) {
	  if (prefix_signature->getColor(j) <= cycle_signature->getColor(j) ||
	      prefix_signature->getColor(j) <= p0_signature.getColor(j)) {
	    // acceptance pair j is ok
	    ;
	  } else {
	    return false;
	  }
	}
	// all acceptance pairs are ok, return true
	return true;
      }
      
      return false;
    }


    /** Store a prefix and a cycle state */
    struct prefix_and_cycle_state_t {
      prefix_and_cycle_state_t(stuttered_state_ptr_t prefix_,
			       stuttered_state_ptr_t cycle_) :
	prefix_state(prefix_), cycle_state(cycle_) {}

      stuttered_state_ptr_t prefix_state;
      stuttered_state_ptr_t cycle_state;
    };


    /** Calculate the prefix and the cycle state */
    prefix_and_cycle_state_t 
    calculate_prefix_and_cycle_state(intermediate_state_vector_t& state_vector,
				     unsigned int cycle_point) {
      stuttered_state_ptr_t prefix_state, cycle_state;
      unsigned states=state_vector.size();

      unsigned int smallest=cycle_point;
      for (unsigned int i=cycle_point+1;i<states;i++) {
	if (*state_vector[i] < *state_vector[smallest]) {
	  smallest=i;
	}
      }

#ifdef STUTTERED_VERBOSE
      std::cerr << "Smallest: " << smallest << std::endl;
#endif

      cycle_state=stuttered_state_ptr_t(new stuttered_state_t(state_vector[smallest]));
      if (! (cycle_point==0 || cycle_point==1)) {
	prefix_state=
	  stuttered_state_ptr_t(new stuttered_state_t(state_vector[smallest]));
      }

      typename Acceptance::signature_type* signature_prefix=(typename Acceptance::signature_type*)NULL;
      if (prefix_state) {
	signature_prefix=&( prefix_state->getSignature() );
      }
      typename Acceptance::signature_type* signature_cycle=&( cycle_state->getSignature() );
      
      
      bool omit_prefix=calculate_acceptance(state_vector, 
					    cycle_point, 
					    signature_prefix,
					    signature_cycle);
      
      if (omit_prefix) {
	prefix_state.reset();
      }
      
      if (_detailed_states) {
	std::ostringstream prefix_description;
	std::ostringstream cycle_description;
	
	if (prefix_state) {
	  prefix_description << "<TABLE><TR><TD>Prefix</TD><TD>Cycle (" << (smallest-cycle_point) <<")</TD></TR>"
			     << "<TR><TD>";
	  
	  prefix_description << "<TABLE><TR>";
	  for (unsigned int i=1;i<cycle_point;i++) {
	    prefix_description << "<TD>" << state_vector[i]->toHTML() << "</TD>";
	  }
	  prefix_description << "</TR></TABLE></TD>";
	}
	
	cycle_description << "<TD><TABLE><TR>";
	for (unsigned int i=cycle_point; i<state_vector.size();i++) {	  
	  cycle_description << "<TD>";
	  cycle_description << state_vector[i]->toHTML();
	  cycle_description << "</TD>";
	}
	cycle_description << "</TR></TABLE></TD>";
	
	
	if (prefix_state) {
	  prefix_description << cycle_description.str();
	  prefix_description << "</TR></TABLE>";
	  
	  prefix_state->setDescription(prefix_description.str());
	}
	
	cycle_description << "</TR></TABLE>";
	cycle_state->setDescription("<TABLE><TR><TD>Cycle ("+
				    boost::lexical_cast<std::string>(smallest-cycle_point) +
				    ")</TD></TR><TR>" + cycle_description.str());
      }
      
      return prefix_and_cycle_state_t(prefix_state, cycle_state);
    }
    

    /** Calculate and add transitions to the successor state.
     * @param from the source stuttered_state 
     * @param da_from the source DA state
     * @param elem the edge label
     */
    void calc_delta(stuttered_state_ptr_t from, da_state_t* da_from, APElement elem) {
      typedef myHashMap<algo_state_t,
	unsigned int,
	ptr_hash<algo_state_t>,
	PtrComparator<algo_state_t> > intermediate_state_map_t;
      
      intermediate_state_map_t state_map;
      intermediate_state_vector_t state_vector;
      
      algo_state_t start_tree=from->getTree();
      state_map[start_tree]=0;
      state_vector.push_back(start_tree);
      
      #ifdef STUTTERED_VERBOSE
      std::cerr << "Calculate from state [" << da_from->getName() << "], "<< (unsigned int) elem << ":" << std::endl;
      std::cerr << start_tree->toString() << std::endl;
      #endif

      algo_state_t cur_tree=start_tree;
      while (true) {
	algo_state_t next_tree=_algo.delta(cur_tree, elem)->getState();
	
	typename intermediate_state_map_t::iterator it;
	it=state_map.find(next_tree);
	if (it==state_map.end()) {
	  // tree doesn't yet exist...
	  // add tree
	  state_map[next_tree]=state_vector.size();
	  state_vector.push_back(next_tree);
	  
	  cur_tree=next_tree;
	  continue;
	} else {
	  // found the cycle!
	  unsigned int cycle_point=(*it).second;

	  #ifdef STUTTERED_VERBOSE
	    std::cerr << "-----------------------\n";
	    for (unsigned int i=0;i<state_vector.size();i++) {
	    std::cerr << "[" << i << "] ";
	    if (cycle_point==i) {
	    std::cerr << "* ";
	    }
	    std::cerr << "\n" << state_vector[i]->toString() << std::endl;
	    }
	    std::cerr << "-----------------------\n";
	    #endif
	    
	  prefix_and_cycle_state_t pac=
	    calculate_prefix_and_cycle_state(state_vector, cycle_point);

	  da_state_t* da_prefix;
	  da_state_t* da_cycle;

	  if (pac.prefix_state &&
	      !(*pac.prefix_state==*pac.cycle_state)) {
	    da_prefix=add_transition(da_from, pac.prefix_state, elem);
	    da_cycle=add_transition(da_prefix, pac.cycle_state, elem);
	  } else {
	    da_cycle=add_transition(da_from, pac.cycle_state, elem);
	  }

	  da_cycle->edges().set(elem, da_cycle);

	  return;
	}
      }
    }
  };
};


#endif
