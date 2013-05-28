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


#ifndef DRA2NBA_HPP
#define DRA2NBA_HPP

/** @file
 * Provides class DRA2NBA, which can convert a DRA/DSA to an equivalent 
 * nondeterministic Büchi automaton.
 */

#include "DRA.hpp"
#include "NBA.hpp"

#include "common/hash_map.hpp"
#include "common/HashFunction.hpp"

#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <cassert>

/**
 * Converts a DRA/DSA to an equivalent nondeterministic Büchi automaton.
 */
class DRA2NBA {
public:
  
  /** Generate an NBA that is equivalent to the DRA/DSA.
   * @param dra the DRA/DSA
   * @return a shared_ptr to an equivalent NBA
   */
  template <class DRA, class NBA>
  static 
  typename NBA::shared_ptr 
  dra2nba(DRA& dra) {
    APSet_cp ap_set=dra.getAPSet_cp();;
    typename NBA::shared_ptr nba_p(new NBA(ap_set));

    if (dra.isStreett()) {
      convert_dsa2nba(dra, *nba_p);
    } else {
      convert_dra2nba(dra, *nba_p);
    }

    return nba_p;
  }

private:
  
  /** 
   * Perform the conversion from a DRA to an NBA.
   */
  template <class DRA, class NBA>
  static void convert_dra2nba(DRA& dra, NBA& nba, bool detailed_states=false) {
    typedef typename DRA::state_type dra_state_t;
    typedef typename NBA::state_type nba_state_t;

    assert(dra.isStreett() == false);

    if (!dra.isCompact()) {
      dra.makeCompact();
    }


    std::vector< std::vector<nba_state_t*> > nba_states;

    nba_states.resize(dra.size());
    for (unsigned int i=0;i<dra.size();i++) {
      nba_states[i].resize(dra.acceptance().size()+1);

      // basic level
      nba_states[i][0]=nba.newState();
      if (detailed_states) {
	nba_states[i][0]->
	  setDescription(boost::lexical_cast<std::string>(i));
      }

      for (unsigned int j=0;j<dra.acceptance().size();j++) {
	if (dra.acceptance().isStateInAcceptance_U(j,i)) {
	  ;
	} else {
	  nba_states[i][j+1]=nba.newState();
	  if (detailed_states) {
	    nba_states[i][j+1]->
	      setDescription(boost::lexical_cast<std::string>(i)+"_"+
			     boost::lexical_cast<std::string>(j));
	  }
	  if (dra.acceptance().isStateInAcceptance_L(j,i)) {
	    nba_states[i][j+1]->setFinal(true);
	  }
	}
      }
    }

    assert(dra.getStartState()!=0);
    nba.setStartState(nba_states[dra.getIndexForState(dra.getStartState())][0]);

    for (unsigned int i=0;i<dra.size();i++) {
      for (typename DRA::edge_iterator edge_it=dra[i]->edges().begin();
	   edge_it!=dra[i]->edges().end();
	   ++edge_it) {
	typename DRA::edge_type edge=*edge_it;
	unsigned int dra_to_index=dra.getIndexForState(edge.second);

	nba_states[i][0]->addEdge(edge.first, *nba_states[dra_to_index][0]);

	for (unsigned int j=0;j<dra.acceptance().size();j++) {
	  if (dra.acceptance().isStateInAcceptance_L(j,i) &&
	      !dra.acceptance().isStateInAcceptance_U(j, dra_to_index)) {
	    nba_states[i][0]->addEdge(edge.first, 
				     *nba_states[dra_to_index][j+1]);
	  }

	  if (dra.acceptance().isStateInAcceptance_U(j,i) ||
	      dra.acceptance().isStateInAcceptance_U(j,dra_to_index)) {
	    ;
	  } else {
	    nba_states[i][j+1]->addEdge(edge.first,
				       *nba_states[dra_to_index][j+1]);
	  }
	}
      }
    }
  }



  
  /**
   * Helper class, holds a state in the NBA generated for a Streett automaton.
   * Holds the index of the state in the DSA,
   * and two bitsets _Iset and _Jset which represent the acceptance pairs
   * that have not yet been satisfied.
   */
  struct DSABuchiState {
    /** The index of the DSA state */
    unsigned int _dsa_state_index;
    /** Bitsets representing I and J */
    BitSet _Iset, _Jset;
    /** if true, no I/J exist (base copy of the DSA) */
    bool _no_IJ;
    
    /** Constructor */
    DSABuchiState(unsigned int dsa_state_index) :
      _dsa_state_index(dsa_state_index),
      _no_IJ(true) {}

    /** Constructor */
    DSABuchiState(unsigned int dsa_state_index,
	      BitSet I,
	      BitSet J) : 
      _dsa_state_index(dsa_state_index),
      _Iset(I),
      _Jset(J),
      _no_IJ(false) {}

    /** Equality */
    bool operator==(const DSABuchiState& other) const {
      if (_dsa_state_index==other._dsa_state_index &&
	  _no_IJ==other._no_IJ &&
	  _Iset==other._Iset &&
	  _Jset==other._Jset) {
	return true;
      }
      return false;
    }
    
#define CMP_LESS(x,y) if (x<y) {return true;}	\
    else if (!(x==y)) {return false;}
    
    /** Less-than operator */
    bool operator<(const DSABuchiState& other) const {
      CMP_LESS(_dsa_state_index, other._dsa_state_index);
      CMP_LESS(_no_IJ, other._no_IJ);
      CMP_LESS(_Iset, other._Iset);
      CMP_LESS(_Jset, other._Jset);
      
      return false;
    }
    
    /** return a hash value */
    unsigned int hashCode() const {
      StdHashFunction hash;
      hash.hash(_dsa_state_index);
      if (_no_IJ) {
	hash.hash((unsigned char)'1');
      } else {
	_Iset.hashCode(hash);
	_Jset.hashCode(hash);
      }

      return hash.value();
    }
    
    /** Helper hash_function */
    struct hash_function {
      size_t operator()(const DSABuchiState& state) const {
	return state.hashCode();
      }
    };
  };
  

  /** Convert a DSA to an NBA.*/
  template <class DRA, class NBA>
  static void convert_dsa2nba(DRA& dsa, NBA& nba) {
    typedef typename DRA::state_type dsa_state_t;
    typedef typename NBA::state_type nba_state_t;

    assert(dsa.isStreett() == true);

    if (!dsa.isCompact()) {
      dsa.makeCompact();
    }    

    typedef std::pair<DSABuchiState, nba_state_t*> unprocessed_pair;
    typedef std::stack< unprocessed_pair > unprocessed_vector;

    unprocessed_vector unprocessed;

    const APSet& ap_set=dsa.getAPSet();



    /* Local helper class to keep track of the states in the NBA. */
    class DSA2BuchiCalculator {
    public:
      /* The DSA */
      DRA& _dsa;
      /* The NBA */
      NBA& _nba;
      
      /* hash_map-type to store the DSABuchiStates and corresponding NBA_States */
      typedef myHashMap<DSABuchiState, nba_state_t*, DSABuchiState::hash_function> state_map_t;
      
      /* Type of iterator over the hash_map */
      typedef typename state_map_t::iterator state_map_iterator;
      
      /* Map for the states */
      state_map_t new_states;
      
      /* Constructor */
      DSA2BuchiCalculator(DRA& dsa, 
			  NBA& nba) :
	_dsa(dsa), _nba(nba) {
      }

      /* 
       * Find an already existing DSABuchiState
       * @return a pointer to the DSABuchiState, NULL if it doesn't exist
       */
      nba_state_t*
      findState(DSABuchiState& description) {
	state_map_iterator it;
	
	it = new_states.find(description);
	if (it!=new_states.end()) {
	  return (*it).second;
	} else {
	  return 0;
	}
      }

      /*
       * Create a new NBA state corresponding to the DSABuchiState
       */
      nba_state_t* 
      createState(DSABuchiState& description) {
	nba_state_t* state=_nba.newState();

	new_states[description]=state;

	if (!description._no_IJ &&
	    description._Iset.isEmpty() &&
	    description._Jset.isEmpty()) {
	  state->setFinal(true);
	}

	return state;
      }
    };

    
    DSA2BuchiCalculator calc(dsa, nba);
  
    assert(dsa.getStartState()!=0);
    dsa_state_t *start_dsa=dsa.getStartState();

    DSABuchiState start_dbn(start_dsa->getName());

    nba_state_t *start=calc.createState(start_dbn);
    nba.setStartState(start);

    unprocessed.push(unprocessed_pair(start_dbn, start));

    while (!unprocessed.empty()) {
      unprocessed_pair cur=unprocessed.top();
      unprocessed.pop();

      dsa_state_t* dsa_from=dsa[cur.first._dsa_state_index];
      nba_state_t* nba_from=cur.second;

      for (typename APSet::element_iterator eit=ap_set.all_elements_begin();
	   eit!=ap_set.all_elements_end();
	   ++eit) {
	typename DRA::label_type label=*eit;
	
	dsa_state_t* dsa_to=
	  dsa_from->edges().get(label);

	if (cur.first._no_IJ) {
	  DSABuchiState p(dsa_to->getName());

	  DSABuchiState p00(dsa_to->getName(),
			   BitSet(0),
			   BitSet(0));
	  
	  nba_state_t* nba_p=calc.findState(p);
	  if (nba_p==0) {
	    nba_p=calc.createState(p);
	    unprocessed.push(unprocessed_pair(p, nba_p));
	  }
	  nba_from->addEdge(label, *nba_p);

	  nba_state_t* nba_p00=calc.findState(p00);
	  if (nba_p00==0) {
	    nba_p00=calc.createState(p00);
	    unprocessed.push(unprocessed_pair(p00, nba_p00));
	  }
	  nba_from->addEdge(label, *nba_p00);
	} else {
	  BitSet I_=cur.first._Iset;
	  I_.Union(dsa.acceptance().getAcceptance_L_forState(dsa_from->getName()));

	  BitSet J_=cur.first._Jset;
	  J_.Union(dsa.acceptance().getAcceptance_U_forState(dsa_from->getName()));

	  BitSet subset=I_;
	  subset.Minus(J_);
	  if (subset.isEmpty()) {
	    // I' - J' = 0  <=> I \subseteq J
	    I_.clear();
	    J_.clear();	    
	  }

	  DSABuchiState dbn_to(dsa_to->getName(),
			      I_,
			      J_);

	  nba_state_t* nba_to=calc.findState(dbn_to);
	  if (nba_to==0) {
	    nba_to=calc.createState(dbn_to);
	    unprocessed.push(unprocessed_pair(dbn_to, nba_to));
	  }
	  nba_from->addEdge(label, *nba_to);
	}
      }
    }
  }
};

#endif
