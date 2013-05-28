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


#ifndef GRAPHALGORITHMS_HPP
#define GRAPHALGORITHMS_HPP

/** @file
 * Provides algorithms to be used on graphs (automata), notably the calculation
 * of Strongly Connected Components (SCCs).
 */

#include "common/BitSet.hpp"
#include "common/BitSetIterator.hpp"

#include <vector>
#include <stack>
#include <boost/shared_ptr.hpp>
#include <iostream>

/**
 * A class for storing information about the Strongly Connected Components (SCCs)
 * of a graph
 */
class SCCs {
public:
  /** Constructor */
  SCCs() {};
  /** Destructor */
  ~SCCs() {};
  
  /** Get the states that are in SCC scc_index */
  BitSet const& operator[](unsigned int scc_index) const {
    return _sccs[scc_index];
  }
  
  /** Get the number of SCCs */
  unsigned int countSCCs() const {return _sccs.size();}
  
  /** Get the SCC index for state */
  unsigned int state2scc(unsigned int state) {
    return _state_to_scc[state];
  }

  /** Get a vector with a topological order of the states*/
  std::vector<int> const& topologicalOrder() {
    return _topological_order;
  }
  
  /** Get a set of SCCs that are successors of the SCC scc_index */
  BitSet& successors(unsigned int scc_index) {
    return _dag[scc_index];
  }
  
  /** Return true, if state_to is reachable from state_from */
  bool stateIsReachable(unsigned int state_from,
		       unsigned int state_to) {
    return isReachable(state2scc(state_from),
		       state2scc(state_to));
  }

  /** Return true, if SCC scc_to is reachable from SCC_fromom */
  bool isReachable(unsigned int scc_from,
		   unsigned int scc_to) {
    return _reachability[scc_from].get(scc_to);
  }

  /** Print the SCCs on the output stream */
  friend std::ostream& operator<<(std::ostream& out,
				  const SCCs &scc) {
    out << "SCC:" << std::endl;

    for (unsigned int i=0;i<scc.countSCCs();i++) {
      int scc_i=scc._topological_order[i];

      out << scc_i << " : " << scc[scc_i] << std::endl;
    }

    return out;
  }

  /** Get a vector of BitSets with reachability information 
   * (state -> reachable_states)
   */
  std::vector<BitSet> *getReachabilityForAllStates() {
    std::vector<BitSet>* v=new std::vector<BitSet>;
    v->resize(_state_to_scc.size());


    for (unsigned int i=0;
	 i<_state_to_scc.size();
	 ++i) {
      unsigned int scc=state2scc(i);
      BitSet& reachable_sccs=_reachability[scc];

      BitSet reachable_states;
      for (BitSetIterator it(reachable_sccs);
	   it!=BitSetIterator::end(reachable_sccs);
	   ++it) {
	// union with all states from the reachable scc
	reachable_states.Union(_sccs[*it]);
      }      

      (*v)[i]=reachable_states;

      //      std::cerr << "from "<<i<<": "<<reachable_states<<std::endl;
    }

    return v;
  }

  //private:
  std::vector<BitSet> _sccs;
  std::vector<unsigned int> _state_to_scc;
  std::vector<BitSet> _dag;
  std::vector<int> _topological_order;
  std::vector<BitSet> _reachability;

  //  friend class GraphAlgorithms::SCC_DFS;
  
  /** Add a new SCC */
  unsigned int addSCC(BitSet& scc) {
    _sccs.push_back(scc);
    return _sccs.size()-1;
  }
  
  /** Set the SCC for a state */
  void setState2SCC(unsigned int state, unsigned int scc) {
    if (_state_to_scc.size() <= state) {
      _state_to_scc.resize(state+1);
    }
    _state_to_scc[state]=scc;
  }
};


/** Provide access for a given Graph to the successors 
 *  for a state v, using the successors_begin and successors_end
 *  calls on the Graph::state_type* */
template <typename Graph>
class StdSuccessorAccess {
 public:
  typedef typename Graph::state_type::successor_iterator successor_iterator;

  successor_iterator begin(Graph& graph, unsigned int v) {
    return graph[v]->successors_begin();
  }

  successor_iterator end(Graph& graph, unsigned int v) {
    return graph[v]->successors_end();
  }
};


/**
 * Perform graph algorithms
 */
template <typename Graph, 
	  typename SuccessorAccess=StdSuccessorAccess<Graph> >
class GraphAlgorithms {
public:

  /** Calculate the SCCs for Graph graph and save in result. */
  static void calculateSCCs(Graph& graph,
			    SCCs& result,
			    bool disjoint=false,
			    SuccessorAccess successor_access=SuccessorAccess()) {
    SCC_DFS::calculateSCCs(graph, result, disjoint, successor_access);
  }

private:
  
  /** Helper class to calculate the SCCs*/
  class SCC_DFS {
  public:
    /** Calculate the SCCs for Graph graph and save in result. */
    static void calculateSCCs(Graph& graph,
			      SCCs& result,
			      bool disjoint,
			      SuccessorAccess& successor_access) {
      SCC_DFS scc_dfs(graph, result, successor_access);
      scc_dfs.calculate(disjoint);
    }
  private:
    /** Dummy constructor to restrict creation */
    explicit SCC_DFS() {} 

    /** Constructor */
    SCC_DFS(Graph& graph,
	    SCCs& result,
	    SuccessorAccess& successor_access) : _graph(graph), _result(result), _successor_access(successor_access) {};

    /** A class for saving DFS state information */
    class SCC_DFS_Data {
    public:
      unsigned int dfs_nr;
      unsigned int root_index;
      bool inComponent;
    };

    /** The graph */
    Graph& _graph;

    /** The SCCs */
    SCCs& _result;
    
    SuccessorAccess& _successor_access;

    /** The current DFS number */
    unsigned int current_dfs_nr;

    /** The DFS stack */
    std::stack<unsigned int> _stack;

    /** The SCC_DFS_Data for every state (state index -> DFS_DATA) */
    std::vector<boost::shared_ptr<SCC_DFS_Data> > _dfs_data;

    /** The current scc number */
    unsigned int scc_nr;

    /** Calculate the SCCs*/
    void calculate(bool disjoint) {
      current_dfs_nr=0;
      _dfs_data.clear();
      // Ensure there are as many entries as there are graph-states
      _dfs_data.resize(_graph.size());
      scc_nr=0;
      
      typename Graph::state_type *start_state=_graph.getStartState();
      if (start_state==0) {
	return;
      }
      
      if (!disjoint) {
	unsigned int start_idx=start_state->getName();
	visit(start_idx);
      } else {
	// The Graph may be disjoint -> restart DFS on every not yet visited state 
	for (unsigned int v=0;
	     v<_graph.size();
	     ++v) {
	  if (_dfs_data[v].get()==0) {
	    // not yet visited
	    visit(v);
	  }
	}
      }
      
      calculateDAG();
    }
    
    /** Visit a state (perform DFS) */
    void visit(unsigned int v) {
      SCC_DFS_Data *sdd=new SCC_DFS_Data;
      sdd->dfs_nr=current_dfs_nr++;
      sdd->root_index=v;
      sdd->inComponent=false;
      
      _stack.push(v);
      _dfs_data[v].reset(sdd);
      
      for (typename SuccessorAccess::successor_iterator 
	     succ_it=_successor_access.begin(_graph, v);
	   succ_it!=_successor_access.end(_graph, v);
	   ++succ_it) {
	unsigned int w=*succ_it;
	
	if (_dfs_data[w].get()==0) {
	  // not yet visited
	  visit(w);
	}
	
	SCC_DFS_Data *sdd_w=_dfs_data[w].get();
	if (sdd_w->inComponent==false) {
	  unsigned int dfs_nr_root_v=_dfs_data[sdd->root_index]->dfs_nr;
	  unsigned int dfs_nr_root_w=_dfs_data[sdd_w->root_index]->dfs_nr;
	  
	  if (dfs_nr_root_v > dfs_nr_root_w) {
	    sdd->root_index=sdd_w->root_index;
	  }
	}
      }
      
      if (sdd->root_index == v) {
	BitSet set;
	
	unsigned int w;
	do {
	  w=_stack.top();
	  _stack.pop();
	  
	  set.set(w);
	  _result.setState2SCC(w, scc_nr);

	  SCC_DFS_Data *sdd_w=_dfs_data[w].get();
	  sdd_w->inComponent=true;
	} while (w!=v);
	
	scc_nr=_result.addSCC(set)+1;
      }
    }

    /** Calculate the Directed Acyclical Graph (DAG) */
    void calculateDAG() {
      _result._dag.clear();
      _result._dag.resize(_result.countSCCs());
      _result._reachability.resize(_result.countSCCs());

      std::vector<signed int> in_degree(_result.countSCCs());

      for (unsigned int scc=0;
	   scc<_result.countSCCs();
	   ++scc) {
	const BitSet& states_in_scc=_result[scc];
	
	for (BitSetIterator it=BitSetIterator(states_in_scc);
	     it!=BitSetIterator::end(states_in_scc);
	     ++it) {
	  unsigned int from_state=*it;
	  
	  for (typename SuccessorAccess::successor_iterator 
		 succ_it=_successor_access.begin(_graph, from_state);
	       succ_it!=_successor_access.end(_graph, from_state);
	       ++succ_it) {
	    unsigned int to_state=*succ_it;
	    unsigned int to_scc=_result.state2scc(to_state);
	    
	    if (to_scc!=scc) {
	      // Only successor in the DAG if not the same scc
	      if (!_result._dag[scc].get(to_scc)) {
		// This SCC is a new successor, increment in_degree
		in_degree[to_scc]++;
		_result._dag[scc].set(to_scc);
	      }
	    }

	    // Reachability
	    _result._reachability[scc].set(to_scc);
	  }
	}
      }

      bool progress = true;
      int cnt = 0;
      _result._topological_order.clear();
      _result._topological_order.resize(_result.countSCCs());

      std::vector<unsigned int> sort(_result.countSCCs());
      while (progress) {
	progress=false;

	for (unsigned int scc=0;scc<_result.countSCCs();++scc) {
	  if (in_degree[scc]==0) {
	    sort[scc]=cnt++;
	    progress=true;
	    in_degree[scc]=-1;

	    for (BitSetIterator it_neighbors=
		   BitSetIterator(_result._dag[scc]);
		 it_neighbors!=BitSetIterator::end(_result._dag[scc]);
		 ++it_neighbors) {
	      unsigned int scc_to=*it_neighbors;
	      in_degree[scc_to]--;
	    }
	  }
	}
      }

      for (unsigned int i=0;i<_result.countSCCs();i++) {
	_result._topological_order[sort[i]]=i;
      }


      // traverse SCCs in reverse topological order
      for (unsigned int i=_result.countSCCs();
	   i>0;
	   --i) {
	unsigned int cur_scc=_result._topological_order[i-1];
	
	BitSet &reaches=_result._reachability[cur_scc];

	for (BitSetIterator it_neighbors=
	       BitSetIterator(_result._dag[cur_scc]);
	     it_neighbors!=BitSetIterator::end(_result._dag[cur_scc]);
	     ++it_neighbors) {
	  unsigned int scc_to=*it_neighbors;

	  reaches.Union(_result._reachability[scc_to]);
	}
      }
    }
  };
};

#endif
