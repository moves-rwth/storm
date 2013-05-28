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


#ifndef NBAANALYSIS_HPP
#define NBAANALYSIS_HPP

/** @file
 * Provides class NBAAnalysis for performing analysis on non-deterministic B&auml;chi automata.
 */

#include "GraphAlgorithms.hpp"

#include <boost/shared_ptr.hpp>


/** 
 * Perform (and cache) analysis for a given NBA.
 */
template <typename NBA_t>
class NBAAnalysis {
private:
  /** The analysed NBA */
  NBA_t& _nba;

  /** Information about the SCCs of the NBA (cached) */
  boost::shared_ptr<SCCs> _sccs;
  /** Information about the states where all the successor states are accepting (cached) */
  boost::shared_ptr<BitSet> _allSuccAccepting;
  /** Information about the states that have an accepting true self-loop (cached) */
  boost::shared_ptr<BitSet> _accepting_true_loops;
  /** Information about the reachability of states (cached) */
  boost::shared_ptr<std::vector<BitSet> > _reachability;

public:
  /** Constructor.
   * @param nba the NBA to be analyzed
   */
  NBAAnalysis(NBA_t& nba) :
    _nba(nba) {
  }						

  /** Destructor */
  ~NBAAnalysis() {}

  /** Get the SCCs for the NBA 
   * @return the SCCs
   */
  SCCs& getSCCs() {
    if (!_sccs) {
      _sccs=boost::shared_ptr<SCCs>(new SCCs());
      GraphAlgorithms<NBA_t>::calculateSCCs(_nba, *_sccs);
    }
    return *_sccs;
  }

  /** Get the states for which all successor states are accepting.
   * @return BitSet with the information
   */
  const BitSet& getStatesWithAllSuccAccepting() {
    if (!_allSuccAccepting) {
      calculateStatesWithAllSuccAccepting();
    }
    return *_allSuccAccepting;
  }

  /** Get the states with accepting true self loops
   * @return BitSet with the information
   */
  const BitSet& getStatesWithAcceptingTrueLoops() {
    if (!_accepting_true_loops) {
      calculateAcceptingTrueLoops();
    }
    return *_accepting_true_loops;
  }


  /** Checks to see if NBA has only accepting (final) states.
   * @return true iff all states are accepting
   */
  bool areAllStatesFinal() {
    for (typename NBA_t::iterator it=_nba.begin();
	 it!=_nba.end();
	 ++it) {
      if (!(*it).isFinal()) {
	return false;
      }
    }
    return true;
  }

  /** Get the accepting states from the NBA
   * @return BitSet with the information
   */
  const BitSet& getFinalStates() {
    return _nba.getFinalStates();
  }

  /** Get the reachability analysis for the NBA
   * @return vector of BitSets representing the set of state which are reachable from a given state.
   */
  std::vector<BitSet>& getReachability() {
    if (!_reachability) {
      _reachability=boost::shared_ptr<std::vector<BitSet> > (getSCCs().getReachabilityForAllStates());
    }

    return *_reachability;
  }


  /** Check if the NBA is empty.
   * @return true iff the NBA has no accepting run.
   */
  bool emptinessCheck() {
    SCCs& sccs=getSCCs();

#ifdef VERBOSE
    std::cerr << sccs << "\n";

    std::cerr << " Reachability: "<< std::endl;
    std::vector<BitSet>* reachable=sccs.getReachabilityForAllStates();
    for (unsigned int t=0; t < reachable->size(); t++) {
      std::cerr << t << " -> " << (*reachable)[t] << std::endl;
    }
    delete reachable;
#endif

    for (unsigned int scc=0;
	 scc<sccs.countSCCs();
	 ++scc) {
      const BitSet& states_in_scc=sccs[scc];
      
      // check to see if there is an accepting state in this SCC
      for (BitSetIterator it=BitSetIterator(states_in_scc);
	   it!=BitSetIterator::end(states_in_scc);
	   ++it) {
	unsigned int state=*it;
	
#ifdef VERBOSE
	 std::cerr << "Considering state " << state << std::endl;
#endif
	if (_nba[state]->isFinal()) {
	  // check to see if this SCC is a trivial SCC (can't reach itself)

#ifdef VERBOSE	  
	  std::cerr << " +final";
	  std::cerr << " " << states_in_scc.cardinality();
#endif
	  
	  if (states_in_scc.cardinality()==1) {
	    // there is only one state in this scc ...

#ifdef VERBOSE	    
	    std::cerr << " +single";
#endif

	    if (sccs.stateIsReachable(state,state)==false) {
	      // ... and it doesn't loop to itself
	      // -> can not guarantee accepting run
	    
#ifdef VERBOSE
	      std::cerr << " -no_loop" << std::endl;
#endif
	      continue;
	    }
	  }
	  
	  // if we are here, the SCC has more than 1 state or 
	  // exactly one self-looping state
	  //  -> accepting run

#ifdef VERBOSE
	  std::cerr << "+acc" << std::endl;
#endif
	  
	  // check that SCC can be reached from initial state
	  assert(_nba.getStartState());
	  if (sccs.stateIsReachable(_nba.getStartState()->getName(), state)) {
#ifdef VERBOSE
	    std::cerr << "Found accepting state = "<< state << std::endl;
#endif
	    return false;
	  }
#ifdef VERBOSE
	  std::cerr << "Not reachable!"<< std::endl;
#endif
	  continue;
	}
      }
    }
    return true;
  }


private:

  /** 
   * Calculates BitSet which specifies which states in the NBA 
   * only have accepting successors.
   */
  void calculateStatesWithAllSuccAccepting() {
    _allSuccAccepting=boost::shared_ptr<BitSet>(new BitSet());
    BitSet& result=*_allSuccAccepting;
    SCCs& sccs=getSCCs();

    std::vector<bool> scc_all_final(sccs.countSCCs());
    for (unsigned int i=0;i<scc_all_final.size();i++) {
      scc_all_final[i]=false;
    }

    for (unsigned int i=sccs.countSCCs();
	 i>0;
	 --i) {
      // go backward in topological order...
      unsigned int scc=(sccs.topologicalOrder())[i-1];

      const BitSet& states_in_scc=sccs[scc];

      // check to see if all states in this SCC are final
      scc_all_final[scc]=true;
      for (BitSetIterator it=BitSetIterator(states_in_scc);
	   it!=BitSetIterator::end(states_in_scc);
	   ++it) {
	if (!_nba[*it]->isFinal()) {
	  scc_all_final[scc]=false;
	  break;
	}
      }


      bool might_be_final=false;
      if (scc_all_final[scc]==false) {
	if (states_in_scc.length()==1) {
	  // there is only one state in this scc ...
	  unsigned int state=states_in_scc.nextSetBit(0);

	  if (sccs.stateIsReachable(state,state)==false) {
	    // ... and it doesn't loop to itself
	    might_be_final=true;
	  }
	}
      }

      if (scc_all_final[scc]==true || might_be_final) {
	// Check to see if all successors are final...
	bool all_successors_are_final=true;
	BitSet& scc_succ=sccs.successors(scc);
	for (BitSetIterator it=BitSetIterator(scc_succ);
	     it!=BitSetIterator::end(scc_succ);
	     ++it) {
	  if (!scc_all_final[*it]) {
	    all_successors_are_final=false;
	    break;
	  }
	}
	
	if (all_successors_are_final) {
	  // Add all states in this SCC to the result-set
	  result.Or(states_in_scc);

	  if (might_be_final) {
	    scc_all_final[scc]=true;
	  }
	}
      }
    }
  }
  

  /** 
   * Calculate the set of states that are accepting and have a true self loop.
   */
  void calculateAcceptingTrueLoops() {
    _accepting_true_loops=boost::shared_ptr<BitSet>(new BitSet());
    BitSet& isAcceptingTrueLoop=*_accepting_true_loops;
    SCCs& sccs=getSCCs();
    
    for (unsigned int scc=0;
	 scc<sccs.countSCCs();
	 ++scc) {
      if (sccs[scc].cardinality()==1) {
	unsigned int state_id=sccs[scc].nextSetBit(0);
	typename NBA_t::state_type *state=_nba[state_id];
	
	if (!state->isFinal()) {
	  // not final, consider next
	  continue;
	}
	
	if (!sccs.successors(scc).isEmpty()) {
	  // there are edges leaving this state, consider next
	  continue;
	}
	
	bool no_empty_to=true;
	if (sccs.stateIsReachable(state_id, state_id)) {
	  // state has at least one self-loop
	  // we have to check that there is no edge with empty To
	  for (typename NBA_t::edge_iterator eit=state->edges_begin();
	       eit!=state->edges_end();
	       ++eit) {
	    typename NBA_t::edge_type edge=*eit;
	    if (edge.second->isEmpty()) {
	      // not all edges lead back to the state...
	      no_empty_to=false;
	      break;
	    }
	  }
	  
	  if (no_empty_to) {
	    // When we are here the state is a final true loop
	    isAcceptingTrueLoop.set(state_id);
	    //	  std::cerr << "True Loop: " << state_id << std::endl;
	  }
	}
      }
    }
  }


};


#endif
