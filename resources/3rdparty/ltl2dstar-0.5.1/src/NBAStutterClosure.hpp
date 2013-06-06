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


#ifndef NBASTUTTERCLOSURE_HPP
#define NBASTUTTERCLOSURE_HPP

/** @file 
 * Provides NBAStutterClosure.
 */

#include "GraphAlgorithms.hpp"
#include "NBAAnalysis.hpp"

#include "APElement.hpp"

#include <memory>


/**
 * Calculate the stutter closure for an NBA.
 */
class NBAStutterClosure {
public:
  /** Calculate the stutter closure for the NBA, for all symbols. 
   * @param nba the NBA
   */
  template<typename NBA_t>
  static std::shared_ptr<NBA_t> stutter_closure(NBA_t& nba) {
    APSet_cp apset=nba.getAPSet_cp();
    
    std::shared_ptr<NBA_t> nba_result_ptr(new NBA_t(apset));
    NBA_t& result=*nba_result_ptr;
    
    unsigned int element_count=apset->powersetSize();
    
    assert(nba.getStartState());
    unsigned int start_state=nba.getStartState()->getName();
    
    for (unsigned int i=0;i<nba.size();i++) {
      unsigned int st=result.nba_i_newState();
      assert(st==i);
      
      if (st==start_state) {
	result.setStartState(result[st]);
      }
      
      if (nba[st]->isFinal()) {
	result[st]->setFinal(true);
      }
    }

    for (unsigned int i=0;i<nba.size();i++) {
      for (unsigned int j=0;j<element_count;j++) {
	unsigned int st=result.nba_i_newState();
	assert( st == nba.size() + (i*element_count)+j);
	result[st]->addEdge(j, *(result[i]));
	result[st]->addEdge(j, *(result[st]));
      }
    }

    std::vector< std::vector<BitSet>* > reachable;
    reachable.resize(element_count);

    for (unsigned int j=0; j<element_count; j++) {
      NBAEdgeSuccessors<NBA_t> edge_successor(j);
      SCCs scc;
      GraphAlgorithms<NBA_t, NBAEdgeSuccessors<NBA_t> >::calculateSCCs(nba,
								       scc,
								       true,
								       edge_successor);
      
      reachable[j]=scc.getReachabilityForAllStates();

      #ifdef VERBOSE
      std::cerr << "SCCs for " << APElement(j).toString(*apset) << std::endl;
      std::cerr << scc << std::endl; 
      
      std::cerr << " Reachability: "<< std::endl;
      std::vector<BitSet>& reach=*reachable[j];
      for (unsigned int t=0; t < reach.size(); t++) {
      	std::cerr << t << " -> " << reach[t] << std::endl;
      }
      
      std::cerr << "  ---\n";
      #endif
    }

    
    for (unsigned int i=0;i<nba.size();i++) {
      typename NBA_t::state_type* from=result[i];
      
      for (unsigned int j=0;j<element_count;j++) {
	BitSet result_to;
	
	BitSet* to=nba[i]->getEdge(j);
	for (BitSetIterator it=BitSetIterator(*to);
	     it!=BitSetIterator::end(*to);
	     ++it) {
	  unsigned int to_state=*it;
	  
	  // We can go directly to the original state
	  result_to.set(to_state);
	  // We can also go to the corresponding stutter state instead
	  unsigned int stutter_state=nba.size() + (to_state*element_count)+j;
	  result_to.set(stutter_state);
	  
	  // ... and then we can go directly to all the states that are j-reachable from to
	  result_to.Union((*(reachable[j]))[to_state]);
	}

	*(from->getEdge(j)) = result_to;
      }
    }

    for (unsigned int i=0;
	 i<reachable.size();
	 ++i) {
      delete reachable[i];
    }
    
    return nba_result_ptr;
  }


  /** Calculate the stutter closure for the NBA, for a certain symbol.
   * @param nba the NBA
   * @param label the symbol for which to perform the stutter closure
   */
  template<typename NBA_t>
  static std::shared_ptr<NBA_t> stutter_closure(NBA_t& nba, APElement label) {
    APSet_cp apset=nba.getAPSet_cp();
    
    std::shared_ptr<NBA_t> nba_result_ptr(new NBA_t(apset));
    NBA_t& result=*nba_result_ptr;
    
    unsigned int element_count=apset->powersetSize();
    
    assert(nba.getStartState());
    unsigned int start_state=nba.getStartState()->getName();
    
    for (unsigned int i=0;i<nba.size();i++) {
      unsigned int st=result.nba_i_newState();
      assert(st==i);
      
      if (st==start_state) {
	result.setStartState(result[st]);
      }
      
      if (nba[st]->isFinal()) {
	result[st]->setFinal(true);
      }
    }

    for (unsigned int i=0;i<nba.size();i++) {
      unsigned int st=result.nba_i_newState();
      assert( st == nba.size() + i);
      result[st]->addEdge(label, *(result[i]));
      result[st]->addEdge(label, *(result[st]));
    }
    
    std::vector<BitSet>* reachable;
    
    NBAEdgeSuccessors<NBA_t> edge_successor(label);
    SCCs scc;
    GraphAlgorithms<NBA_t, NBAEdgeSuccessors<NBA_t> >::calculateSCCs(nba,
								     scc,
								     true,
								     edge_successor);
  
    reachable=scc.getReachabilityForAllStates();
      
    //    std::cerr << "SCCs for " << label.toString(*apset) << std::endl;
    //    std::cerr << scc << std::endl; 
    
    //    std::cerr << " Reachability: "<< std::endl;
    //    for (unsigned int t=0; t < reachable->size(); t++) {
    //      std::cerr << t << " -> " << (*reachable)[t] << std::endl;
    //    }
    
    //    std::cerr << "  ---\n";

    for (unsigned int i=0;i<nba.size();i++) {
      typename NBA_t::state_type* from=result[i];
      
      for (unsigned int j=0;j<element_count;j++) {
	BitSet result_to;
	
	BitSet* to=nba[i]->getEdge(j);
	if (j!=label) {
	  result_to=*to;
	} else {
	  for (BitSetIterator it=BitSetIterator(*to);
	       it!=BitSetIterator::end(*to);
	       ++it) {
	    unsigned int to_state=*it;
	    
	    // We can go directly to the original state
	    result_to.set(to_state);
	    // We can also go to the corresponding stutter state instead
	    unsigned int stutter_state=nba.size() + to_state;
	    result_to.set(stutter_state);
	    
	    // ... and then we can go directly to all the states that are j-reachable from to
	    result_to.Union((*reachable)[to_state]);
	  }
	}
	  
	*(from->getEdge(j)) = result_to;
      }
    }

    delete reachable;

    return nba_result_ptr;
  }
  
private:
  /** The successors reachable via a certain label */
  template <typename NBA_t>
  class NBAEdgeSuccessors {
  public:
    typedef BitSetIterator successor_iterator;

    NBAEdgeSuccessors(APElement label) : _label(label) {};
    
    successor_iterator begin(NBA_t& graph, unsigned int v) {
      return BitSetIterator(*(graph[v]->getEdge(_label)));
    }

    successor_iterator end(NBA_t& graph, unsigned int v) {
      return BitSetIterator::end(*(graph[v]->getEdge(_label)));
    }

  private:
    APElement _label;
  };
};


#endif
