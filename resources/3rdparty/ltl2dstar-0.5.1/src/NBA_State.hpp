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


#ifndef NBA_STATE_H
#define NBA_STATE_H

/** @file
 * Provides class NBA_State for storing a state of a nondeterministic Büchi automaton.
 */


#include "common/Exceptions.hpp"
#include "common/Indexable.hpp"
#include "common/BitSet.hpp"
#include "common/BitSetIterator.hpp"
#include "common/nested_iterator.hpp"
#include "APSet.hpp"
#include "APElement.hpp"
#include "APMonom2APElements.hpp"

#include <iostream>

// forward declaration
template <typename Label, 
	  template <typename State> class EdgeContainer> class NBA;



// forward declaration 
template <typename State,
	  typename Label, 
	  typename EdgeContainer >
struct NBA_State_EdgeManager;

/**
 * A state of a deterministic omega-automaton.
 * For a description of the template parameters, see class NBA.
 */
template <typename Label, template <typename N> class EdgeContainer >
class NBA_State : public Indexable<NBA_State<Label,EdgeContainer> > {
 public:
  /** The type of the automaton containing this state */
  typedef NBA<Label, EdgeContainer> graph_type;

  /** The type of the edges in the NBA. */
  typedef typename graph_type::edge_type edge_type;

  /** The type of the states in the NBA (ie this NBA_State class). */
  typedef typename graph_type::state_type state_type;

  /** The type of the EdgeContainer for the DA_State. */
  typedef EdgeContainer<BitSet> edge_container_type;
  
  /**
   * Constructor.
   * @param graph The automaton (NBA) that contains this state.
   */
  NBA_State(graph_type& graph) 
    : _graph(graph),
      _isFinal(false),
      _edge_manager(*this, graph.getAPSet()) {
  }

  /** Destructor */
  ~NBA_State() {
  }

  /** 
   * Add an edge from this state to the other state
   * @param label the label for the edge
   * @param state the target state 
   */
  void addEdge(APElement label, state_type& state) {
    _edge_manager.addEdge(label, state);
  }


  /** 
   * Add edge(s) from this state to the other state
   * @param monom an APMonom for the label(s)
   * @param to_state the target state
   */
  void addEdge(APMonom monom, state_type& to_state) {
    _edge_manager.addEdge(monom, to_state);
  }

  /*
  edge_container_type& edges() {return _edges;}
  const edge_container_type& edges() const {return _edges;}
  */

  /** 
   * Get the target states of the labeled edge.
   * @return a pointer to a BitSet with the indizes of the target states.
   */
  BitSet* getEdge(APElement label) {
    return _edge_manager.getEdge(label);
  };

  /** 
   * Get the target states of the labeled edge.
   * @return a pointer to a BitSet with the indizes of the target states.
   */
  BitSet* getEdge(APMonom monom) {
    return _edge_manager.getEdge(monom);
  };

  /** Get the name (index) of this state. */
  unsigned int getName() const {
    return _graph.getIndexForState(this);
  };

  /** Print the name of this state on the output stream. */
  friend std::ostream& operator<<(std::ostream& out, NBA_State& state) {
    out << state.getName();
    return out;    
  }

  /** Is this state accepting (final)? */
  bool isFinal() {return _isFinal;}
  
  /** Set the value of the final flag for this state */
  void setFinal(bool final) {
    _isFinal=final;
    _graph.getFinalStates().set(_graph.getIndexForState(this), final);
  }

  /** Returns an iterator over the edges pointing to the first edge. */
  typename edge_container_type::EdgeIterator 
  edges_begin() {
    return _edge_manager.getEdgeContainer().begin();
  }

  /** Returns an iterator over the edges pointing after the last edge. */
  typename edge_container_type::EdgeIterator 
  edges_end() {
    return _edge_manager.getEdgeContainer().end();
  }

  /**
   * Calls operator(state_type*) on the functor UnaryFunction
   * for all states that are reachable in one step from this 
   * state.
   */
  template <class UnaryFunction>
  void forEachSuccessor(UnaryFunction& unary_function) {
    for (typename edge_container_type::EdgeIterator edge_it=edges_begin();
	 edge_it!=edges_end();
	 ++edge_it) {
      BitSet *bs=*edge_it;
      for (BitSetIterator bs_it(*bs);
	   bs_it!=BitSetIterator::end(*bs);
	   ++bs_it) {
	unary_function(_graph[*bs_it]);
      }
    }
  }
  
  /** A helper that provides iterators over the set bits for an edge */
  struct edge_to_bitset_iterator {
    BitSetIterator begin(edge_type const& edge) {
      return BitSetIterator(*edge.second);
    }

    BitSetIterator end(edge_type const& edge) {
      return BitSetIterator::end(*edge.second);
    }
  };

  /** 
   * The type of an iterator over the names (indizes) of the
   * states that are reachable in one step from this state.
   * Note: A state can occur multiple times!
   */
  typedef nested_iterator<typename edge_container_type::EdgeIterator,
			  BitSetIterator,
			  edge_to_bitset_iterator> successor_iterator;

  /**
   * Returns an iterator over the names of the successors, pointing to the first.
   * Note: A state can occur multiple times!
   */
  successor_iterator successors_begin() {
    //    typename edge_container_type::EdgeIterator b=edges.begin(),
    //e=edges().end();
    return successor_iterator(edges_begin(), edges_end());
  }

  /**
   * Returns an iterator over the names of the successors, pointing after the last.
   * Note: A state can occur multiple times!
   */
  successor_iterator successors_end() {
    return successor_iterator(edges_end(), edges_end());
  }

  /** Set the description for this state. */
  void setDescription(const std::string& s) {_description=s;}
  /** Get the description for this state. */
  const std::string& getDescription() {return _description;}

  /** Check if this state has a description. */
  bool hasDescription() {return _description.length()!=0;}

  /** Get the automaton owning this state. */
  graph_type& getGraph() {return _graph;}
 
private:
  /** The automaton */
  graph_type& _graph;

  /** Is this state accepting */
  bool _isFinal;

  /** A description. */
  std::string _description;

  /** The EdgeManager*/
  NBA_State_EdgeManager<NBA_State, Label, edge_container_type> _edge_manager;
};



#include "EdgeContainerExplicit_APElement.hpp"

/** The EdgeManager for the NBA_State */
template <typename State>
struct NBA_State_EdgeManager<State, 
			    APElement, 
			    EdgeContainerExplicit_APElement<BitSet> > {
public:
  /** The type of the EdgeContainer*/
  typedef EdgeContainerExplicit_APElement<BitSet> edge_container_type;

private:
  /** The state owning this EdgeManager */
  State& _state;
  /** The EdgeContainer */
  edge_container_type _container;

  
public:
  /**
   * Constructor.
   * @param state the NBA_State owning this EdgeManager
   * @param apset the underlying APSet   
   */
  NBA_State_EdgeManager(State& state, const APSet& apset) 
    : _state(state), 
      _container(apset.size()) {

    for (APSet::element_iterator eit=apset.all_elements_begin();
	 eit!=apset.all_elements_end();
	 ++eit) {
      _container.addEdge(*eit, new BitSet());
    }
  }
  
  /** Destructor */
  ~NBA_State_EdgeManager() {
    const APSet& ap_set=_state.getGraph().getAPSet();
    for (APSet::element_iterator eit=ap_set.all_elements_begin();
	 eit!=ap_set.all_elements_end();
	 ++eit) {
      delete _container.get(*eit);
    }    
  }

  /** Get the target states */
  BitSet* getEdge(APElement label) {
    return _container.get(label);
  }

  /** Get the target states */
  BitSet* getEdge(APMonom monom) {
    THROW_EXCEPTION(Exception, "Not implemented!");
  }

  /** Add an edge. */
  void addEdge(APElement label, State& state) {
    _container.get(label)->set(state.getName());
  }

  /** Add an edge. */
  void addEdge(APMonom label, State& state) {
    const APSet& ap_set=_state.getGraph().getAPSet();
    
    for (APMonom2APElements it=APMonom2APElements::begin(ap_set, label);
	 it!=APMonom2APElements::end(ap_set, label);
	 ++it) {
      addEdge(*it, state);
    }
  }

  /** Get the EdgeContainer. */
  edge_container_type& getEdgeContainer() {
    return _container;
  }
};

#endif


