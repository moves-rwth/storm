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


#ifndef DA_STATE_H
#define DA_STATE_H

/** @file
 * Provides class DA_State for storing a state of a deterministic omega-automaton.
 */

#include "common/Exceptions.hpp"
#include "common/Indexable.hpp"
#include "common/BitSet.hpp"

#include <iostream>
#include <string>
#include <memory>

template <typename Label, 
	  template <typename State> class EdgeContainer,
	  typename AcceptanceCondition> class DA;

/**
 * A state of a deterministic omega-automaton.
 * For a description of the template parameters, see class DA.
 */
template <typename Label, template <typename N> class EdgeContainer, typename AcceptanceCondition>
class DA_State : public Indexable<DA_State<Label,EdgeContainer,AcceptanceCondition> > {
 public:
  /** The type of the automaton containing this state */
  typedef DA<Label, EdgeContainer,AcceptanceCondition> graph_type;

  /** The type of the edges in the DA. */
  typedef typename graph_type::edge_type edge_type;

  /** The type of the states in the DA (ie this DA_State class). */
  typedef typename graph_type::state_type state_type;

  /** The type of the EdgeContainer for the DA_State. */
  typedef EdgeContainer<DA_State> edge_container_type;

  /** The type of an iterator over all the edges of this state. */
  typedef typename edge_container_type::iterator edge_iterator;

  /** 
   * Constructor.
   * @param graph The automaton (DA) that contains this state.
   */
  DA_State(graph_type& graph) 
    : _graph(graph),
      _edges(graph.getAPSize()) {;}
  ~DA_State() {;}

  /** Get the EdgeContainer to access the edges. */
  edge_container_type& edges() {return _edges;}
  
  /** Get the EdgeContainer to access the edges. */
  const edge_container_type& edges() const {return _edges;}

  /** Get the name (index) of this state. */
  unsigned int getName() const {
    return _graph.getIndexForState(this);
  };

  /** Print the name of the state on an output stream. */
  friend std::ostream& operator<<(std::ostream& out, DA_State& state) {
    out << state.getName();
    return out;    
  }
 

  /** Set an description for the state */
  void setDescription(const std::string& s) {
    _description.reset(new std::string(s));
  }

  /**
   * Get an description for the state (previously set using setDescription()).
   * Should only be called after verifying that the state hasDescription()
   * @return a const string ref to the description
   */
  const std::string& getDescription() {
    assert(hasDescription());
    return *_description;
  }

  /**
   * Check wheter the state has a description.
   */
  bool hasDescription() {
    return _description.get()!=0;
  }


  /**
   * Checks if all transitions originating in this state
   * leed back to itself. 
   */
  bool hasOnlySelfLoop() {
    for (edge_iterator eit=edges().begin();
	 eit!=edges().end();
	 ++eit) {
      if (this != (*eit).second) {
	return false;
      }
    }
    return true;
  }

  /** Get the AcceptanceForState access functor for this state */
  typename AcceptanceCondition::AcceptanceForState acceptance() {
    typename AcceptanceCondition::AcceptanceForState acc(_graph.acceptance(),
							 this->getName());
    return acc;
  }

 
  /** A Functor that gets the name of the to state in an edge */
  struct GetToForEdgeFunctor : 
    public std::unary_function<typename edge_container_type::edge_type, unsigned int> {
    unsigned int operator()(typename edge_container_type::edge_type e) const {
      return e.second->getName();
    }
  };

  /** 
   * The type of an iterator over the names (indizes) of the
   * states that are reachable in one step from this state.
   * Note: A state can occur multiple times!
   */
  typedef boost::transform_iterator<
    GetToForEdgeFunctor,
    typename edge_container_type::EdgeIterator
    > successor_iterator;

  /**
   * Returns an iterator over the names of the successors, pointing to the first.
   * Note: A state can occur multiple times!
   */
  successor_iterator successors_begin() {
    return successor_iterator(_edges.begin(), GetToForEdgeFunctor());
  }

  /**
   * Returns an iterator over the names of the successors, pointing after the last.
   * Note: A state can occur multiple times!
   */
  successor_iterator successors_end() {
    return successor_iterator(_edges.end(), GetToForEdgeFunctor());
  }



 private:
  /** The automaton of which this state is a part. */
  graph_type& _graph;

  /** The edges */
  edge_container_type _edges;

  /** A description */
  std::auto_ptr<std::string> _description;
};


#endif


