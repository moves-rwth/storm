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


#ifndef NBA_H
#define NBA_H

/** @file
 * Provides class NBA to store a nondeterministic Büchi automaton.
 */

#include "common/Exceptions.hpp"
#include "common/Index.hpp"
#include "common/BitSet.hpp"

#include <boost/iterator/iterator_facade.hpp>

#include <memory>
#include <string>
#include <iostream>
#include <iterator>
#include <vector>
#include <map>
#include <utility>

#include "NBA_I.hpp"
#include "APSet.hpp"
#include "GraphAlgorithms.hpp"

// forward declaration of NBA_State
template <typename Label, template <typename N> class EdgeContainer > class NBA_State;


/**
 * A nondeterministic Büchi automaton.
 * See class DA for description of template parameters.
 */
template <typename Label, template <typename N> class EdgeContainer >
class NBA : public NBA_I {
public:
  NBA(APSet_cp apset);
  virtual ~NBA();

  /** The type of the states of the NBA. */  
  typedef NBA_State<Label,EdgeContainer> state_type;

  /** The type of the graph (ie the NBA class itself). */
  typedef NBA<Label,EdgeContainer> graph_type;

  /** The type of an iterator over the edges of a state. */
  typedef typename EdgeContainer<BitSet>::iterator edge_iterator;

  /** 
   * The type of an edge, consisting of the label and a pointer to a BitSet
   * of the target states.
   */
  typedef std::pair<Label, BitSet*> edge_type;

  state_type* newState();

  /** Get number of states. */
  unsigned int size() {return _index.size();}

  /** Type of an iterator over the states (by reference) */
  typedef typename Index<state_type>::ref_iterator iterator;
  
  /** An iterator over the states (by reference) pointing to the first state. */
  iterator begin() {return _index.begin_ref();}

  /** An iterator over the states (by reference) pointing to the first state. */
  iterator end() {return _index.end_ref();}

  /** Array index operator, get the state with index i. */
  state_type* operator[](unsigned int i) {
    return _index.get(i);
  }

  /** Get the size of the underlying APSet. */
  unsigned int getAPSize() const {return _apset->size();};

  /** Get a const reference to the underlying APSet. */
  const APSet& getAPSet() const {return *_apset;};

  /** Get a const pointer to the underlying APSet. */
  APSet_cp getAPSet_cp() const {return _apset;};

  /** Switch the APSet to another with the same number of APs. */
  void switchAPSet(APSet_cp new_apset) {
    if (new_apset->size()!=_apset->size()) {
      THROW_EXCEPTION(IllegalArgumentException, "New APSet has to have the same size as the old APSet!");
    }

    _apset=new_apset;
  }


  /** Get the index for a state. */
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

  /** Get the set of final (accepting) states in the NBA */
  BitSet& getFinalStates() {return _final_states;}


  BitSet calculateFinalTrueLoops(SCCs& sccs);
  void removeRedundantFinalStates(SCCs& sccs);


  void print(std::ostream& out);
  void print_lbtt(std::ostream& out);
  void print_dot(std::ostream& out);

  /** Return number of states. */
  unsigned int getStateCount() {return _state_count;}

  // -- NBA_I virtual functions -----------
  /** 
   * Create a new state.
   * @return the index of the new state
   */
  virtual unsigned int nba_i_newState() { 
    return newState()->getName();
  }

  /**
   * Add an edge from state <i>from</i> to state <i>to</i>
   * for the edges covered by the APMonom.
   * @param from the index of the 'from' state
   * @param m the APMonom
   * @param to the index of the 'to' state
   */
  virtual void nba_i_addEdge(unsigned int from,
			     APMonom& m,
			     unsigned int to) {
    (*this)[from]->addEdge(m, *((*this)[to]));
  }

  /**
   * Get the underlying APSet 
   * @return a const pointer to the APSet
   */
  virtual APSet_cp nba_i_getAPSet() {
    return getAPSet_cp();
  }

  /** 
   * Set the final flag (accepting) for a state.
   * @param state the state index
   * @param final the flag
   */
  virtual void nba_i_setFinal(unsigned int state, bool final) {
    (*this)[state]->setFinal(final);
  }

  /**
   * Set the state as the start state.
   * @param state the state index
   */
  virtual void nba_i_setStartState(unsigned int state) {
    setStartState((*this)[state]);
  }

  /** Type of a std::shared_ptr for this NBA */
  typedef std::shared_ptr< NBA<Label,EdgeContainer> > shared_ptr;


  bool isDeterministic();

  static 
  std::shared_ptr<NBA<Label, EdgeContainer> >
  product_automaton(NBA<Label, EdgeContainer>& nba_1, NBA<Label, EdgeContainer>& nba_2);

private:
  /** Number of states */
  int _state_count;
  
  /** Storage for the states */
  Index<state_type> _index;

  /** The underlying APSet */
  APSet_cp _apset;
  
  /** The start states */
  state_type *_start_state;

  /** The states that are accepting (final) */
  BitSet _final_states;
};


/**
 * Constructor.
 * @param apset The underlying APSet
 */
template <typename Label, template <typename N> class EdgeContainer >
NBA<Label, EdgeContainer>::NBA(APSet_cp apset) 
  : _state_count(0), _apset(apset), _start_state(0) {
}

/**
 * Destructor.
 */
template <typename Label, template <typename N> class EdgeContainer >
NBA<Label, EdgeContainer>::~NBA() {
  for (unsigned int i=0;i<_index.size();i++) {
    if (_index[i]) {
      delete _index[i];
    }
  }
}


/**
 * Add a new state.
 * @return a pointer to the newly generated state
 */
template <typename Label, template <typename N> class EdgeContainer >
typename NBA<Label, EdgeContainer>::state_type* 
NBA<Label, EdgeContainer>::newState() {
  _state_count++;
  state_type *state=new state_type(*this);
  
  _index.add(state);
  return state;
}

/**
 * Print the NBA on the output stream.
 */
template <typename Label, template <typename N> class EdgeContainer >
void
NBA<Label, EdgeContainer>::print(std::ostream& out) {
  for (iterator state_it=begin();
       state_it!=end();
       ++state_it) {
    state_type& state=*state_it;
    out << "State " << state.getName();
    if (getStartState()==&state) {
      out << " *";
    }
    if (state.isFinal()) {
      out << " !";
    }

    out << std::endl;

    for (edge_iterator edge_it=state.edges_begin();
	 edge_it!=state.edges_end();
	 ++edge_it) {
      edge_type edge=*edge_it;

      APElement label=edge.first;
      BitSet* to_states=edge.second;

      out << " " << label.toString(getAPSet()) << " -> " << *to_states << std::endl;
    }
  }
}

/**
 * Print the NBA on the output stream in LBTT format.
 */
template <typename Label, template <typename N> class EdgeContainer >
void
NBA<Label, EdgeContainer>::print_lbtt(std::ostream& out) {
  out << size() << " " << "1" << std::endl; // states sp acc
  
  for (iterator state_it=begin();
       state_it!=end();
       ++state_it) {
    state_type& state=*state_it;
    out << state.getName();
    if (getStartState()==&state) {
      out << " 1";
    } else {
      out << " 0";
    }
    if (state.isFinal()) {
      out << " 0";
    }

    out << " -1"<< std::endl;

    for (edge_iterator edge_it=state.edges_begin();
	 edge_it!=state.edges_end();
	 ++edge_it) {
      edge_type edge=*edge_it;

      APElement label=edge.first;
      BitSet* to_states=edge.second;

      for (int to_i=to_states->nextSetBit(0);
	   to_i!=-1;
	   to_i=to_states->nextSetBit(to_i+1)) {
	out << to_i << " " << label.toStringLBTT(getAPSet()) << std::endl;
      }
    }
    out << "-1" << std::endl;
  }
}


/** 
 * Remove states from the set of accepting (final) states when this is redundant.
 * @param sccs the SCCs of the NBA
 */
template <typename Label, template <typename N> class EdgeContainer >
void
NBA<Label, EdgeContainer>::removeRedundantFinalStates(SCCs& sccs) {
  for (unsigned int scc=0;
       scc<sccs.countSCCs();
       ++scc) {
    if (sccs[scc].cardinality()==1) {
      unsigned int state_id=sccs[scc].nextSetBit(0);
      state_type *state=(*this)[state_id];

      if (state->isFinal()) {
	if (!sccs.stateIsReachable(state_id, state_id)) {
	  // The state is final and has no self-loop
	  //  -> the final flag is redundant
	  state->setFinal(false);
	  //	  std::cerr << "Removing final flag for " << state_id << std::endl;
	}
      }
    }
  }
}

/**
 * Checks if the NBA is deterministic (every edge has at most one target state).
 */
template <typename Label, 
	  template <typename N> class EdgeContainer >
bool
NBA<Label, EdgeContainer>::isDeterministic() {
  for (iterator state_it=begin();
       state_it!=end();
       ++state_it) {
    state_type& state=*state_it;
    for (edge_iterator edge_it=state.edges_begin();
	 edge_it!=state.edges_end();
	 ++edge_it) {
      edge_type edge=*edge_it;
      if (edge.second->cardinality()>1) {
	return false;
      }
    }
  }
  return true;
}

/**
 * Print the NBA in DOT format to the output stream.
 * @param out the output stream 
 */
template <typename Label, 
	  template <typename N> class EdgeContainer >
void
NBA<Label, EdgeContainer>::print_dot(std::ostream& out) {

  if (this->getStartState()==0) {
    // No start state! 
    THROW_EXCEPTION(IllegalArgumentException, "No start state in NBA!");
  }


  #define DOT_STATE_FONT "Helvetica"
  #define DOT_EDGE_FONT  "Helvetica"

  out << "digraph nba {\n";
#ifdef DOT_STATE_FONT
  out << " node [fontname=" << DOT_STATE_FONT << "]\n";
#endif

#ifdef DOT_EDGE_FONT
  out << " edge [constraints=false, fontname=" << DOT_EDGE_FONT << "]\n";
#endif

  const APSet& ap_set=getAPSet();

  for (iterator state_it=begin();
       state_it!=end();
       ++state_it) {
    state_type& state=*state_it;
    
    unsigned int i_state=state.getName();
    
    out << "\"" << i_state << "\" [";
    out << "label= \"" << i_state << "\"";

    if (state.isFinal()) {
      out << ", shape=box";
    } else {
      out << ", shape=circle";
    }
    
    if (getStartState()==&state) {
      out << ", style=filled, color=black, fillcolor=grey";
    }
    

    out << "]\n"; // close parameters for state


    // transitions

    for (edge_iterator edge_it=state.edges_begin();
	 edge_it!=state.edges_end();
	 ++edge_it) {
      edge_type edge=*edge_it;
      
      APElement label=edge.first;
      BitSet* to_states=edge.second;

      for (int to_i=to_states->nextSetBit(0);
	   to_i!=-1;
	   to_i=to_states->nextSetBit(to_i+1)) {

	out << "\"" << i_state << "\" -> \"" << to_i;
	out << "\" [label=\" " << label.toString(ap_set, false) << "\"]\n";
      }
    }
  }

  out << "}" << std::endl;
}



template <typename Label, 
	  template <typename N> class EdgeContainer >
std::shared_ptr<NBA<Label, EdgeContainer> >
NBA<Label, EdgeContainer>::product_automaton(NBA<Label, EdgeContainer>& nba_1,
					     NBA<Label, EdgeContainer>& nba_2) {
  assert(nba_1.getAPSet() == nba_2.getAPSet());
  std::shared_ptr<NBA<Label, EdgeContainer> > product_nba(new NBA<Label, EdgeContainer>(nba_1.getAPSet_cp()));
  
  const APSet& apset=nba_1.getAPSet();
  assert(apset == nba_2.getAPSet());

  for (unsigned int s_1=0;s_1<nba_1.size();s_1++) {
    for (unsigned int s_2=0;s_2<nba_2.size();s_2++) {
      for (unsigned int copy=0; copy<2; copy++) {
	unsigned int s_r=product_nba->nba_i_newState();
	assert(s_r == (s_1*nba_2.size() + s_2)*2 + copy);
#ifdef VERBOSE
	std::cerr << s_1 << ":" << s_2 << ":" << copy << " = " << s_r << std::endl;
#endif

	unsigned int to_copy=copy;
	
	if (copy==0 && nba_1[s_1]->isFinal()) {
	  to_copy=1;
	}
	if (copy==1 && nba_2[s_2]->isFinal()) {
	  (*product_nba)[s_r]->setFinal(true);
	  to_copy=0;
	}
	
	for (typename APSet::element_iterator it=apset.all_elements_begin();
	     it!=apset.all_elements_end();
	     ++it) {
	  APElement label=*it;
	  BitSet *to_s1=nba_1[s_1]->getEdge(label);
	  BitSet *to_s2=nba_2[s_2]->getEdge(label);
	  BitSet to_set;
	  for (BitSetIterator it_e_1=BitSetIterator(*to_s1);
	       it_e_1!=BitSetIterator::end(*to_s1);
	       ++it_e_1) {
	    for (BitSetIterator it_e_2=BitSetIterator(*to_s2);
		 it_e_2!=BitSetIterator::end(*to_s2);
		 ++it_e_2) {
	      unsigned int to=((*it_e_1)*nba_2.size() + (*it_e_2))*2 + to_copy;
	      to_set.set(to);
	    }
	  }
	  
	  *((*product_nba)[s_r]->getEdge(label))=to_set;
	}
      }
    }
  }
  
  unsigned int start_1 = nba_1.getStartState()->getName();
  unsigned int start_2 = nba_2.getStartState()->getName();
  product_nba->setStartState( (*product_nba)[ start_1*nba_2.size() + start_2 ]);
  
  return product_nba;
}

#endif
