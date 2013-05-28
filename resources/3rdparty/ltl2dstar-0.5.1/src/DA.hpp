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


#ifndef DA_H
#define DA_H

/** @file
 * Provide base class DA<>, which can store an deterministic omega-automaton,
 * with Rabin, Streett, Parity or Büchi acceptance condition.
 */

#include "common/Exceptions.hpp"
#include "common/Index.hpp"
#include "common/BitSet.hpp"
#include "common/StringAlgorithms.hpp"

#include <boost/iterator/iterator_facade.hpp>

#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>
#include <map>
#include <utility>
#include <cassert>
#include <sstream>

#include "APSet.hpp"

/** forward declaration of DA_State */
template <typename Label, template <typename N> class EdgeContainer, typename AcceptanceCondition> class DA_State;

/**
 * A class representing a deterministic omega automaton.
 * <p>
 * The template parameters:<br>
 * Label: the type of the labeling on the edges<br>
 * EdgeContainer: a template (taking the DA_State class as parameter)
 *                providing an EdgeContainer for holding the edges in 
 *                the states.<br>
 * </p>
 * <p>
 *  Each state is identified by an index.<br>
 *  There exists one start state.<br>
 *  There exists an acceptance condition <br>
 *  The DA is <i>compact</i>, if there are no holes in the indexes of the
 *  states and the acceptance condition is compact as well.
 * </p>
 */
template <typename Label, template <typename N> class EdgeContainer, typename AcceptanceCondition>
class DA {
public:
  DA(APSet_cp ap_set);
  virtual ~DA();
  
  /** The type of the states of the DA. */
  typedef DA_State<Label,EdgeContainer,AcceptanceCondition> state_type;

  /** The type of the label on the edges. */
  typedef Label label_type;

  /** The type of the graph (ie the DA class itself). */
  typedef DA<Label,EdgeContainer,AcceptanceCondition> graph_type;

  /** The type of an iterator over the edges of a state. */
  typedef typename EdgeContainer<state_type>::iterator edge_iterator;
  
  /** 
   * The type of an edge, consisting of the label and a pointer to the target
   * state.
   */
  typedef std::pair< Label, state_type*> edge_type;
  
  typedef AcceptanceCondition acceptance_condition_type;

  /** Create a new instance of the automaton. */
  virtual DA<Label,EdgeContainer,AcceptanceCondition> *createInstance(APSet_cp ap_set) = 0;

  state_type* newState();

  /** The number of states in the automaton.*/
  unsigned int size() const {return _index.size();}

  /** The type of an iterator over the states (by reference) */
  typedef typename Index<state_type>::ref_iterator iterator;
  
  /** 
   * An iterator over the states (by reference) pointing to the first state. 
   */
  iterator begin() {return _index.begin_ref();}

  /** 
   * An iterator over the states (by reference) pointing after the last state. 
   */
  iterator end() {return _index.end_ref();}

  /**
   * Array index operator, get the state with index i.
   */
  state_type* operator[](unsigned int i) {
    return _index.get(i);
  }
  
  /**
   * Get the state with index i.
   */
  state_type* get(unsigned int i) {
    return _index.get(i);
  }

  /**
   * Get the size of the underlying APSet.
   */
  unsigned int getAPSize() const {return _ap_set->size();};

  /**
   * Get a const reference to the underlying APSet.
   */
  const APSet& getAPSet() const {return *_ap_set;};

  /**
   * Get a const pointer to the underlying APSet.
   */
  APSet_cp getAPSet_cp() const {return _ap_set;}

  /**
   * Switch the APSet to another with the same number of APs.
   */
  void switchAPSet(APSet_cp new_apset) {
    if (new_apset->size()!=_ap_set->size()) {
      THROW_EXCEPTION(IllegalArgumentException, "New APSet has to have the same size as the old APSet!");
    }

    _ap_set=new_apset;
  }

  /**
   * Get the index for a state.
   */
  unsigned int getIndexForState(const state_type *state) const {
    return _index.get_index(state);
  }

  /** Set the start state. */
  void setStartState(state_type *state) {_start_state=state;};

  /**
   * Get the start state.
   * @return the start state, or NULL if it wasn't set.
   */
  state_type* getStartState() {return _start_state;}

  /** Output state label for DOT printing. 
   * @param out the output stream
   * @param state_index the state index
   */
  virtual void formatStateForDOT(std::ostream& out, unsigned int state_index) {
    out << "label = \"" << state_index << "\"";
  }


  /** Checks if the automaton is compact. */
  bool isCompact() const {
    return _is_compact && acceptance().isCompact();
  }

  void makeCompact();

  /** Set a comment for the automaton. */
  void setComment(std::string comment) {
    _comment=comment;
  }
  
  /** Get the comment for the automaton. */
  std::string getComment() const {
    return _comment;
  }

  /** Return reference to the acceptance condition for this automaton.
   * @return reference to the acceptance condition
   */
  AcceptanceCondition& acceptance() {return _acceptance;}
  
  /** Return const reference to the acceptance condition for this automaton.
   * @return reference to the acceptance condition
   */
  const AcceptanceCondition& acceptance() const {return _acceptance;}

protected:
  // ---- Output
  void print_da_explicit_v2(const std::string& da_type,
			    std::ostream& out);
  void print_da_dot(const std::string& da_type,
		    std::ostream& out);

  // Members
private:
  /** The number of states. */
  int _state_count;
  
  /** The storage index for the states. */
  Index<state_type> _index;

  /** The underlying APset. */
  APSet_cp _ap_set;
  
  /** The start state. */
  state_type *_start_state;

  /** Flag to mark that the automaton is compact. */
  bool _is_compact;
  
  /** A comment */
  std::string _comment;
  
  /** The acceptance condition for this automaton. */
  AcceptanceCondition _acceptance;
};






/**
 * Constructor.
 * @param ap_set the underlying APSet.
 */
template <typename Label, template <typename N> class EdgeContainer, typename AcceptanceCondition>
DA<Label, EdgeContainer,AcceptanceCondition>::DA(const APSet_cp ap_set) 
  : _state_count(0), 
    _ap_set(ap_set), 
    _start_state(0), 
    _is_compact(true) {
}


/**
 * Destructor.
 */
template <typename Label, template <typename N> class EdgeContainer, typename AcceptanceCondition>
DA<Label, EdgeContainer,AcceptanceCondition>::~DA() {
  for (unsigned int i=0;i<_index.size();i++) {
    if (_index[i]) {
      delete _index[i];
    }
  }
}

/**
 * Create a new state.
 * @return a pointer to the new state.
 */
template <typename Label, template <typename N> class EdgeContainer, typename AcceptanceCondition>
typename DA<Label, EdgeContainer,AcceptanceCondition>::state_type* 
DA<Label, EdgeContainer,AcceptanceCondition>::newState() {
  state_type *state=new state_type(*this);
  
  _index.add(state);
  _acceptance.addState(state->getName());
  return state;
}


/**
 * Print the DA in v2 format to the output stream.
 * This functions expects that the DA is compact.
 * @param da_type a string specifying the type of automaton ("DRA", "DSA").
 * @param out the output stream 
 */
template <typename Label, template <typename N> class EdgeContainer, typename AcceptanceCondition>
void
DA<Label, EdgeContainer,AcceptanceCondition>::print_da_explicit_v2(const std::string& da_type,
					       std::ostream& out) {
  // Ensure that this DA is compact...
  if (!this->isCompact()) {
    THROW_EXCEPTION(IllegalArgumentException, "DA is not compact!");
  }


  if (this->getStartState()==0) {
    // No start state! 
    THROW_EXCEPTION(IllegalArgumentException, "No start state in DA!");
  }

  out << da_type << " v2 explicit" << std::endl;
  if (_comment!="") {
    out << "Comment: \"" << _comment << "\"" << std::endl;
  }
  out << "States: " << _index.size() << std::endl;
  _acceptance.outputAcceptanceHeader(out);

  int start_state=this->getStartState()->getName();
  out << "Start: " << start_state << std::endl;

  // Enumerate APSet
  out << "AP: " << getAPSize();
  for (unsigned int ap_i=0;ap_i<getAPSize();++ap_i) {
    out << " \"" << getAPSet().getAP(ap_i) << "\"";
  }
  out << std::endl;

  out << "---" << std::endl;

  for (unsigned int i_state=0;i_state<_index.size();i_state++) {
    state_type* cur_state=_index[i_state];
    out << "State: " << i_state;
    if (cur_state->hasDescription()) {
      out << " \"" << cur_state->getDescription() << "\"";
    }
    out << std::endl;


    _acceptance.outputAcceptanceForState(out, i_state);

    const APSet& ap_set=getAPSet();
    for (APSet::element_iterator el_it=ap_set.all_elements_begin();
	 el_it!=ap_set.all_elements_end();
	 ++el_it) {
      APElement label=*el_it;
      state_type *to_state=cur_state->edges().get(label);
      unsigned int to_state_index=to_state->getName();
      out << to_state_index << std::endl;
    }
  }
}


/**
 * Reorder states and acceptance conditions so that
 * the automaton becomes compact.
 */
template <typename Label, template <typename N> class EdgeContainer, typename AcceptanceCondition>
void DA<Label, EdgeContainer,AcceptanceCondition>::makeCompact() {
  acceptance().makeCompact();

  if (!_is_compact) {
    std::pair<bool, std::vector<unsigned int> > r=_index.compact();
    
    bool moved=r.first;
    std::vector<unsigned int>& mapping=r.second;
    
    if (moved) {
      acceptance().moveStates(mapping);
    }
    _is_compact=true;
  }
}


/**
 * Print the DA in DOT format to the output stream.
 * This functions expects that the DA is compact.
 * @param da_type a string specifying the type of automaton ("DRA", "DSA").
 * @param out the output stream 
 */
template <typename Label, template <typename N> class EdgeContainer, typename AcceptanceCondition>
void
DA<Label, EdgeContainer,AcceptanceCondition>::print_da_dot(const std::string& da_type,
				       std::ostream& out) {
  // Ensure that this DA is compact...
  if (!this->isCompact()) {
    THROW_EXCEPTION(IllegalArgumentException, "DA is not compact!");
  }
  

  if (this->getStartState()==0) {
    // No start state! 
    THROW_EXCEPTION(IllegalArgumentException, "No start state in DRA!");
  }


  #define DOT_STATE_FONT "Helvetica"
  #define DOT_EDGE_FONT  "Helvetica"

  out << "digraph " << da_type << " {\n";
#ifdef DOT_STATE_FONT
  out << " node [fontname=" << DOT_STATE_FONT << "]\n";
#endif

#ifdef DOT_EDGE_FONT
  out << " edge [constraints=false, fontname=" << DOT_EDGE_FONT << "]\n";
#endif

  out << "\"type\" [shape=ellipse, label=\"" << da_type << "\"]\n";

  std::string comment=getComment();
  if (comment.size()!=0) {
    out << "\"comment\" [shape=box, label=\"";
    
    // replace \ with \\ .
    StringAlgorithms::replace_all(comment, "\\", "\\\\");
  
    // replace " with \" .
    StringAlgorithms::replace_all(comment, "\"", "\\\"");

    // replace new-line with "\n" <- literal .
    StringAlgorithms::replace_all(comment, "\n", "\\n");

    out << comment << "\"]\n";
  }

  const APSet& ap_set=getAPSet();

  for (unsigned int i_state=0;i_state<_index.size();i_state++) {
    out << "\"" << i_state << "\" [";
    
    formatStateForDOT(out, i_state);
    
    out << "]\n"; // close parameters for state
    
    
    // transitions

    state_type* cur_state=this->get(i_state);
    if (cur_state->hasOnlySelfLoop()) {
      // get first to-state, as all the to-states are the same
      state_type *to=cur_state->edges().get(*(ap_set.all_elements_begin()));

      out << "\"" << i_state << "\" -> \"" << to->getName();
      out << "\" [label=\" true\", color=blue]\n";
    } else {
      for (APSet::element_iterator el_it=ap_set.all_elements_begin();
	   el_it!=ap_set.all_elements_end();
	   ++el_it) {
	APElement label=*el_it;
	state_type *to_state=cur_state->edges().get(label);
	unsigned int to_state_index=to_state->getName();
	out << "\"" << i_state << "\" -> \"" << to_state_index;
	out << "\" [label=\" " << label.toString(getAPSet(), false) << "\"]\n";
      }
    }
  }

  out << "}" << std::endl;
}

#endif
