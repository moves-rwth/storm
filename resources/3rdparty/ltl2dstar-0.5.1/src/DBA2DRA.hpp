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


#ifndef DBA2DRA_HPP
#define DBA2DRA_HPP

/** @file 
 * Provides class DBA2DRA, which can convert a 
 * deterministic Büchi automaton to a deterministic Rabin
 * automaton.
 */

#include "common/Exceptions.hpp"

/**
 * Provides conversion from deterministic Büchi to 
 * deterministic Rabin.
 */
class DBA2DRA {
public:
  /**
   * Convert a deterministic Büchi automaton
   * (a nondeterministic Büchi automaton NBA, where every transition
   * has at most one target state) to an equivalent deterministic 
   * Rabin automaton.
   * <p>
   * This involves generation of the appropriate acceptance condition
   * and making sure that the transition function is complete.
   * </p>
   * <p>
   * The DBA can also be complemented on the fly 
   * (by modifying the acceptance condition of the DRA). The resulting DRA can then be
   * regarded as a Streett automaton of the original DBA.
   * @param nba the NBA, the transitions have to be deterministic!
   * @param complement complement the DBA?
   * @return a shared_ptr to the created DRA
   */
  template <class NBA, class DRA>
  static
  typename DRA::shared_ptr 
  dba2dra(NBA& nba, bool complement=false) {
    APSet_cp ap_set=nba.getAPSet_cp();;
    typename DRA::shared_ptr dra_p(new DRA(ap_set));
    
    dba2dra(nba, *dra_p, complement);
    return dra_p;
  }

  /** 
   * Internal helper function to perform the conversion from NBA to DRA.
   * @param nba the NBA (has to be deterministic)
   * @param dra_result the DRA into which the converted automaton is saved
   * @param complement complement the DBA?
   */
  template <class NBA, class DRA>
  static
  void
  dba2dra(NBA& nba, DRA& dra_result, bool complement=false) {
    DRA& dra=dra_result;
    APSet_cp ap_set=dra.getAPSet_cp();;

    dra.acceptance().newAcceptancePair();

    for (unsigned int i=0;
	 i<nba.size();
	 i++) {
      dra.newState();
      
      if (complement) {
	// Final states -> U_0, all states -> L_0
	if (nba[i]->isFinal()) {
	  dra.acceptance().stateIn_U(0, i);
	}
	dra.acceptance().stateIn_L(0, i);
      } else {
	// Final states -> L_0, U_0 is empty
	if (nba[i]->isFinal()) {
	  dra.acceptance().stateIn_L(0, i);
	}
      }
    }

    if (nba.getStartState()!=0) {
      dra.setStartState(dra[nba.getStartState()->getName()]);
    }

    typename DRA::state_type *sink_state=0;

    for (unsigned int i=0;
	 i<nba.size();
	 i++) {
      typename NBA::state_type *nba_state=nba[i];
      typename DRA::state_type *dra_from=dra[i];

      for (APSet::element_iterator label=
	     ap_set->all_elements_begin();
	   label!=ap_set->all_elements_end();
	   ++label) {
	BitSet *to=nba_state->getEdge(*label);

	int to_cardinality=0;
	if (to!=0) {
	  to_cardinality=to->cardinality();
	}


	typename DRA::state_type *dra_to=0;
	if (to==0 ||
	    to_cardinality==0) {
	  // empty to -> go to sink state
	  if (sink_state==0) {
	    // we have to create the sink
	    sink_state=dra.newState();
	   
	    // if we complement, we have to add the sink to
	    // L_0 
	    if (complement) {
	      sink_state->acceptance().addTo_L(0);
	    }	    
	  }
	  dra_to=sink_state;
	} else if (to_cardinality==1) {
	  int to_index=to->nextSetBit(0);

	  //	  std::cerr << "to: " << to_index << std::endl;

	  dra_to=dra[to_index];
	} else {
	  // to_cardinality>1 !
	  THROW_EXCEPTION(IllegalArgumentException, "NBA is no DBA!");
	}

	dra_from->edges().addEdge(*label, dra_to);
      }
    }

    if (sink_state!=0) {
      // there is a sink state
      // make true-loop from sink state to itself
      for (APSet::element_iterator label=
	     ap_set->all_elements_begin();
	   label!=ap_set->all_elements_end();
	   ++label) {
	sink_state->edges().addEdge(*label, sink_state);
      }
    }
  }
};

#endif
