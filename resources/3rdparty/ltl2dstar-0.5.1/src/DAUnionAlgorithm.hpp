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


#ifndef DAUNIONALGORITHM_H
#define DAUNIONALGORITHM_H

#include "NBA2DA.hpp"
#include "StutteredNBA2DA.hpp"


/** @file
 * Provides DAUnionAlgorithm for calculating the union of two DA.
 */


// TODO: trueloop again


/**
 * Generic dummy UnionAcceptanceCalculator functor.
 * Any specialization should provide (see RabinAcceptance for example): 
 * <ul>
 * <li>Constructor taking the acceptance for the two DA</li>
 * <li>prepareAcceptance(&acceptance): prepare the acceptance condition in the result DA</li>
 * <li>calculateAcceptance: calculating the resulting acceptance signature for two states</li>
 * </ul>
 */
template <typename Acceptance>
struct UnionAcceptanceCalculator {
};


/**
 * Specialized UnionAcceptanceCalculator for RabinAcceptance, for calculating the acceptance in the union automaton.
 * This approach merges the acceptance signatures of the two states in the union tuple, the union is provided by
 * the semantics of the Rabin acceptance condition (There <i>exists</i> an acceptance pair ....)
 */
template <>
struct UnionAcceptanceCalculator<RabinAcceptance> {
  /** RabinAcceptance signature type. */
  typedef RabinAcceptance::signature_type signature_t;
  /** shared_ptr of signature type. */
  typedef boost::shared_ptr<signature_t> signature_ptr;
  
  /**
   * Constructor. 
   * @param acc_1 The RabinAcceptance condition from automaton 1
   * @param acc_2 The RabinAcceptance condition from automaton 2
   */
  UnionAcceptanceCalculator(RabinAcceptance& acc_1,
			    RabinAcceptance& acc_2) :
    _acc_1(acc_1), _acc_2(acc_2) {
    _acc_size_1=_acc_1.size();
    _acc_size_2=_acc_2.size();
  }

  /**
   * Prepares the acceptance condition in the result union automaton. If the two automata have k1 and k2 
   * acceptance pairs, this function allocates k1+k2 acceptance pairs in the result automaton.
   * @param acceptance_result The acceptance condition in the result automaton.
   */
  void prepareAcceptance(RabinAcceptance& acceptance_result) {
    acceptance_result.newAcceptancePairs(_acc_size_1+_acc_size_2);
  }


  /**
   * Calculate the acceptance signature for the union of two states.
   * @param da_state_1 index of the state in the first automaton
   * @param da_state_2 index of the state in the second automaton
   * @return A shared_ptr Rabin acceptance signature
   */
  signature_ptr calculateAcceptance(unsigned int da_state_1,
				    unsigned int da_state_2) {
    signature_ptr signature_p=signature_ptr(new signature_t(_acc_size_1+
							  _acc_size_2));
    signature_t& signature=*signature_p;

    for (unsigned int i=0;
	 i<_acc_size_1;
	 i++) {
      if (_acc_1.isStateInAcceptance_L(i, da_state_1)) {
	signature.setL(i, true);
      }
      if (_acc_1.isStateInAcceptance_U(i, da_state_1)) {
	signature.setU(i, true);
      }
    }
    
    for (unsigned int j=0;
	 j<_acc_size_2;
	 j++) {
      if (_acc_2.isStateInAcceptance_L(j, da_state_2)) {
	signature.setL(j + _acc_size_1, true);
      }
      if (_acc_2.isStateInAcceptance_U(j, da_state_2)) {
	signature.setU(j + _acc_size_1, true);
      }
    }

    return signature_p;
  }

  /** The acceptance condition of the first automaton. */
  RabinAcceptance& _acc_1;
  /** The acceptance condition of the second automaton. */
  RabinAcceptance& _acc_2;

  /** The size of the acceptance condition in the original automaton. */
  unsigned int _acc_size_1, _acc_size_2;
};


/**
 * An algorithm calculating the union of two DA. Requires the existance of a UnionAcceptanceCalculator for the
 * AcceptanceCondition.
 * @param DA_t the Deterministic Automaton class.
 */
template <typename DA_t>
class DAUnionAlgorithm {
public:
  /** shared_ptr for DA_t type*/
  typedef typename boost::shared_ptr<DA_t> DA_ptr;
  /** state_type from DA_t */
  typedef typename DA_t::state_type da_state_t;
  /** acceptance condition type from DA_t */
  typedef typename DA_t::acceptance_condition_type da_acceptance_t;
  /** acceptance signature type from DA_t */
  typedef typename da_acceptance_t::signature_type da_signature_t;

  /** A state representing a union state from two DA. */
  struct UnionState {
    /** Index of the state from the first automaton */
    unsigned int da_state_1;
    /** Index of the state from the second automaton */
    unsigned int da_state_2;
    /** A shared_ptr with the acceptance signature of this state */
    boost::shared_ptr<da_signature_t> signature;
    /** A shared_ptr to a string containing a detailed description of this state */
    boost::shared_ptr<std::string> description;

    /** Constructor.
     * @param da_state_1_ index of the state in the first automaton
     * @param da_state_2_ index of the state in the second automaton
     * @param acceptance_calculator UnionAcceptanceCalculator
     */
    template <typename AcceptanceCalc>
    UnionState(unsigned int da_state_1_,
	       unsigned int da_state_2_,
	       AcceptanceCalc& acceptance_calculator) :
      da_state_1(da_state_1_), da_state_2(da_state_2_) {
      signature=acceptance_calculator.calculateAcceptance(da_state_1,
							  da_state_2);
    }

    /** Compare this state to another for equality.
     * @param other the other UnionState
     * @returns true iff the two states are equal
     */
    bool operator==(const UnionState& other) const{
      return (da_state_1==other.da_state_1 &&
	      da_state_2==other.da_state_2);
      // we don't have to check the signature as there is a 
      // 1-on-1 mapping between <da_state_1, da_state_2> -> signature
    }


    /** Compare this state to another for less-than-relationship. Uses the natural order on
     * the two indizes (da_state_1, da_state_2)
     * @param other the other UnionState
     * @returns true iff this state is 'smaller' than the other
     */
    bool operator<(const UnionState& other) const {
      if (da_state_1<other.da_state_1)
	return true;

      if (da_state_1==other.da_state_1) {
	return (da_state_2<other.da_state_2);
	// we don't have to check the signature as there is a 
	// 1-on-1 mapping between <da_state_1, da_state_2> -> signature
      }
      return false;
    }
    

    /** Copy acceptance signature for this state
     * @param acceptance (<b>out</b>) AcceptanceForState for the state in the result automaton 
     */
    void generateAcceptance(typename da_acceptance_t::AcceptanceForState acceptance) const {
      acceptance.setSignature(*signature);
    }
    
    /** Copy acceptance signature for this state
     * @param acceptance (<b>out</b>) acceptance signature for the state in the result automaton 
     */
    void generateAcceptance(da_signature_t& acceptance) const {
      acceptance=*signature;
    }
    
    /** Return the acceptance acceptance signature for this state
     * @return the acceptance signature for this state
     */
    const RabinAcceptance::RabinSignature& generateAcceptance() const {
      return *signature;
    }

    /**
     * Set the detailed description for this state
     * @param description_ the description
     */
    void setDescription(const std::string& description_) {
      description=boost::shared_ptr<std::string>(new std::string(description_));
    }
    
    /** Generate a simple representation of this state 
     * @return a string with the representation
     */
    std::string toString() const {
      return "("+boost::lexical_cast<std::string>(da_state_1)+","+boost::lexical_cast<std::string>(da_state_1)+")";
    }
    
    /** Return the detailed description 
     * @return the detailed description
     */
    const std::string& toHTML() const {
      if (!description) {
	THROW_EXCEPTION(Exception, "No description");
      } else {
	return *description;
      }
    }

    /** Calculates the hash for the union state. 
     * @param hash the HashFunction functor used for the calculation
     */
    template <class HashFunction>
    void hashCode(HashFunction& hash) {
      hash.hash(da_state_1);
      hash.hash(da_state_2);
      // we don't have to consider the signature as there is a 
      // 1-on-1 mapping between <da_state_1, da_state_2> -> signature
    }

  };
  /** shared_ptr of UnionState */
  typedef boost::shared_ptr<UnionState> UnionState_ptr;

  /** A simple wrapper for UnionState_Result to accomodate the plugging in with fuzzy matching */
  struct UnionState_Result {
    UnionState_ptr state;

    UnionState_Result(UnionState_ptr state_) : state(state_) {}
    
    UnionState_ptr getState() {
      return state;
    }
  };
  /** shared_ptr for UnionState_Result */
  typedef boost::shared_ptr<UnionState_Result> UnionState_Result_ptr;

  typedef UnionState_Result_ptr result_t;
  typedef UnionState_ptr state_t;


  /** Constructor. 
   * @param da_1 The first DA
   * @param da_2 the second DA
   * @param trueloop_check Check for trueloops?
   * @param detailed_states Generate detailed descriptions of the states? */
  DAUnionAlgorithm(DA_t& da_1, DA_t& da_2,
		   bool trueloop_check=true,
		   bool detailed_states=false) :
    _da_1(da_1), _da_2(da_2), 
    _acceptance_calculator(da_1.acceptance(), da_2.acceptance()),
    _trueloop_check(trueloop_check),
    _detailed_states(detailed_states) {
    
    if (! (_da_1.getAPSet() == _da_2.getAPSet()) ) {
      THROW_EXCEPTION(IllegalArgumentException, "Can't create union of DAs: APSets don't match");
    }

    APSet_cp combined_ap=da_1.getAPSet_cp();

    if (!_da_1.isCompact() || !_da_2.isCompact()) {
      THROW_EXCEPTION(IllegalArgumentException, "Can't create union of DAs: Not compact");
    }

    _result_da=DA_ptr((DA_t*)da_1.createInstance(combined_ap));
  }

  /** Destructor */
  ~DAUnionAlgorithm() {}
  
  /** Get the resulting DA 
   * @return a shared_ptr to the resulting union DA.
   */
  DA_ptr getResultDA() {
    return _result_da;
  }
  
  /** Calculate the successor state.
   * @param from_state The from state
   * @param elem The edge label 
   * @return result_t the shared_ptr of the successor state
   */
  result_t delta(state_t from_state, APElement elem) {
    da_state_t* state1_to=_da_1[from_state->da_state_1]->edges().get(elem);
    da_state_t* state2_to=_da_2[from_state->da_state_2]->edges().get(elem);

    UnionState_ptr to=createState(state1_to->getName(), 
				  state2_to->getName());
    return UnionState_Result_ptr(new UnionState_Result(to));
  }


  /** Get the start state.
   * @return a shared_ptr to the start state 
   */
  state_t getStartState() {
    if (_da_1.getStartState()==0 ||
	_da_2.getStartState()==0) {
      THROW_EXCEPTION(IllegalArgumentException, "DA has no start state!");
    }

    return createState(_da_1.getStartState()->getName(), 
		       _da_2.getStartState()->getName());
  }
  
  /** Prepare the acceptance condition 
   * @param acceptance the acceptance condition in the result DA
   */
  void prepareAcceptance(da_acceptance_t& acceptance) {
    _acceptance_calculator.prepareAcceptance(acceptance);
  }

  /** Check if the automaton is a-priori empty */
  bool checkEmpty() {
    return false;
  }

  /** Calculate the union of two DA. If the DAs are not compact, they are made compact.
   * @param da_1 The first DA
   * @param da_2 the second DA
   * @param trueloop_check Check for trueloops?
   * @param detailed_states Generate detailed descriptions of the states?
   * @return shared_ptr to result DA
   */
  static
  DA_ptr calculateUnion(DA_t& da_1, 
			DA_t& da_2,
			bool trueloop_check=true,
			bool detailed_states=false) {
    if (!da_1.isCompact()) {
      da_1.makeCompact();
    }

    if (!da_2.isCompact()) {
      da_2.makeCompact();
    }
    
    typedef DAUnionAlgorithm<DA_t> algo_t;
    algo_t dua(da_1, da_2, trueloop_check, detailed_states);
    
    NBA2DA<algo_t, DA_t> generator(detailed_states);
    generator.convert(dua, *dua.getResultDA(), 0);

    return dua.getResultDA();
  }


  /** Calculate the union of two DA, using stuttering if possible. If the DAs are not compact, they are made compact.
   * @param da_1 The first DA
   * @param da_2 the second DA
   * @param stutter_information information about the symbols where stuttering is allowed
   * @param trueloop_check Check for trueloops?
   * @param detailed_states Generate detailed descriptions of the states? */
  static
  DA_ptr calculateUnionStuttered(DA_t& da_1, 
				 DA_t& da_2,
				 StutterSensitivenessInformation::ptr stutter_information,
				 bool trueloop_check=true,
				 bool detailed_states=false) {
    if (!da_1.isCompact()) {
      da_1.makeCompact();
    }
    
    if (!da_2.isCompact()) {
      da_2.makeCompact();
    }
    
    typedef DAUnionAlgorithm<DA_t> algo_t;
    algo_t dua(da_1, da_2, trueloop_check, detailed_states);
    
    StutteredNBA2DA<algo_t, DA_t> generator(detailed_states, stutter_information);
    generator.convert(dua, *dua.getResultDA(), 0);
    
    return dua.getResultDA();
  }
  
private:
  /** The first DA */
  DA_t& _da_1;
  /** The second DA */
  DA_t& _da_2;
  /** The result DA */
  DA_ptr _result_da;

  /** The acceptance calculator */
  UnionAcceptanceCalculator<da_acceptance_t> _acceptance_calculator;

  /** Perform trueloop check? */
  bool _trueloop_check;	
  /** Generate detailed descriptions? */
  bool _detailed_states;


  /** Create a UnionState 
   * @param da_state_1
   * @param da_state_2
   * @return the corresponding UnionState
   */
  UnionState_ptr createState(unsigned int da_state_1,
			     unsigned int da_state_2) {
    UnionState_ptr state(new UnionState(da_state_1, da_state_2, _acceptance_calculator));

    // Generate detailed description
    if (_detailed_states) {
      std::ostringstream s;
      
      s << "<TABLE BORDER=\"1\" CELLBORDER=\"0\"><TR><TD>";
      
      if (_da_1[da_state_1]->hasDescription()) {
	s << _da_1[da_state_1]->getDescription();
      } else {
	s << da_state_1;
      }
      
      s << "</TD><TD>U</TD><TD>";

      if (_da_2[da_state_2]->hasDescription()) {
	s << _da_2[da_state_2]->getDescription();
      } else {
	s << da_state_2;
      }
      
      s << "</TD></TR></TABLE>";
      
      state->setDescription(s.str());
    }
    
    return state;
  }
};


#endif
