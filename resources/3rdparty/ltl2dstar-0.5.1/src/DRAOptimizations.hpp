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


#ifndef DRAOPTIMIZATIONS_HPP
#define DRAOPTIMIZATIONS_HPP

/** @file
 * Provides optimizations on complete DRAs, notably quotienting using bisimulation.
 */

#include "APSet.hpp"
#include "common/BitSet.hpp"
#include "common/BitSetIterator.hpp"

#include <algorithm>
#include <vector>
#include <cassert>
#include <iostream>
#include <sstream>

/**
 * Provides optimizations on complete DRAs, notably quotienting using bisimulation.
 */
template <class DRA>
class DRAOptimizations {
public:
  /** type of a color */
  typedef unsigned int color_t;
  /** type of vector state indizes */
  typedef std::vector<unsigned int> state_vector;

private:
  /** Helper class, storing a coloring of the states */
  class Coloring {
  private:
    /** The number of colors */
    unsigned int _nr_of_colors;

    /** mapping state_id -> color */
    std::vector<color_t> _coloring;

    /** Keep detailed information of the equivalence classes? */
    bool _detailed;

    /** 
     * mapping from color 
     * -> the state ids which are colored alike
     * only used when _detailed=true */
    std::vector<BitSet> *_color2states;

    /** 
     * mapping from color -> one representative state
     */
    std::vector<unsigned int> _color2state;
  public:
    /** 
     * Constructor, get initial size of the coloring from DRA.
     * @param dra the DRA
     * @param detailed Keep detailed information on the equivalence classes?
     */
    Coloring(DRA& dra, bool detailed=false) 
      : _nr_of_colors(0), _detailed(detailed) {
      _coloring.resize(dra.size());
      if (_detailed) {
	_color2states=new std::vector<BitSet>();
      } else {
	_color2states=0;
      }
    }

    /** 
     * Constructor, explicitly set initial size of the coloring
     * @param size the initial size
     * @param detailed Keep detailed information on the equivalence classes?
     */
    Coloring(unsigned int size, bool detailed=false) 
      : _nr_of_colors(0), _detailed(detailed) {
      _coloring.resize(size);

      if (_detailed) {
	_color2states=new std::vector<BitSet>();
      } else {
	_color2states=0;
      }
    }

    /** Destructor */
    ~Coloring() {
      delete _color2states;
    }
    
    /** Reset (clear) coloring. */
    void reset() {_nr_of_colors=0;}

    /** Get the flag 'detailed' */
    bool getFlagDetailed() {return _detailed;}

    /** Returns the size (number of states) of this coloring. */
    unsigned int size() const {return _coloring.size();}

    /** 
     * Create a new color
     * @return the newly created color
     */
    color_t newColor() {
      _nr_of_colors++;

      _color2state.resize(_nr_of_colors);

      if (_detailed) {
      _color2states->resize(_nr_of_colors);
      }

      return _nr_of_colors-1;
    }

    /** Return the current (last created) color */
    color_t currentColor() const {
      assert(_nr_of_colors>0);
      return _nr_of_colors-1;
    }

    /** Return the number of colors */
    unsigned int countColors() const {
      return _nr_of_colors;
    }

    /** Set the color of a state */
    void setColor(unsigned int state,
		  color_t color) {
      assert(color<_nr_of_colors);

      _coloring[state]=color;
      _color2state[color]=state;

      if (_detailed) {
	(*_color2states)[color].set(state);
      }
    }

    /** Get the color for a state */
    unsigned int state2color(unsigned int state) const {
      return _coloring[state];
    }

    /**
     *Get one representative state for the equivalence class with the 
     * specified color. 
     */
    unsigned int color2state(color_t color) const {
      assert(color<_nr_of_colors);
      return _color2state[color];
    }

    /** 
     * Get the state indizes (in a BitSet) that have the specified color. 
     * Can only be called, when the 'detailed' flag is activated in the
     * constructor.
     */
    const BitSet& color2states(color_t color) const {
      assert(color<_nr_of_colors);
      assert(_detailed && _color2states!=0);
      return (*_color2states)[color];
    }

    /** Print the coloring */
    friend std::ostream& operator<<(std::ostream& out,
				    const Coloring& coloring) {
      for (unsigned int i=0;i<coloring.size();i++) {
	out << "color[" <<  i << "] = " << coloring.state2color(i) << std::endl;
      }

      return out;
    }

  private:
    /** Dummy Copy constructor */
    Coloring(const Coloring& other);
  };

  /** 
   * Functor, provides a 'less-than' Comparator 
   * for the states of the DRA, using the color of 
   * the states themself and the colors of the
   * to-states of the edges.
   */
  class ColoredStateComparator {
  private:
    /** The coloring */
    const Coloring &_coloring;
    /** The DRA */
    DRA &_dra;

  public:
    /** Constructor */
    ColoredStateComparator(const Coloring& coloring,
			  DRA& dra) :
      _coloring(coloring), _dra(dra) {}

    /**
     * Compares two states 'less-than' using the
     * coloring, uses the bisimulation
     * equivalence relation to determine
     * equality.
     */
    bool operator()(unsigned int state_x, 
		    unsigned int state_y) {
      color_t cx=_coloring.state2color(state_x);
      color_t cy=_coloring.state2color(state_y);

      if (cx < cy) {
	return true;
      } else if (cx > cy) {
	return false;
      }

      
      for (APSet::element_iterator label=
	     _dra.getAPSet().all_elements_begin();
	   label!=_dra.getAPSet().all_elements_end();
	   ++label) {
	typename DRA::state_type *to_x=
	  _dra[state_x]->edges().get(*label);
	typename DRA::state_type *to_y=
	  _dra[state_y]->edges().get(*label);
	
	color_t ctx=_coloring.state2color(to_x->getName());
	color_t cty=_coloring.state2color(to_y->getName());
	
	if (ctx < cty) {
	  return true;
	} else if (ctx > cty) {
	  return false;
	}
      }

      // we get here only if x and y are equal with this
      // coloring -> return false
      return false;
    }
  };


  /** 
   * A container that stores (caches) the acceptance signatures of
   * all the states in a DRA.
   */
  class AcceptanceSignatureContainer {
  public:
    /** Type of an acceptance signature */
    typedef std::pair<BitSet*, BitSet*> acceptance_signature_t;

    /** 
     * Constructor, fills the container with the acceptance signatures of the states.
     * @param dra the DRA
     */
    AcceptanceSignatureContainer(DRA& dra) {
      _acceptancesig_vector.resize(dra.size());

      for (unsigned int i=0;
	   i<dra.size();
	   i++) {
	BitSet b=dra.acceptance().getAcceptance_L_forState(i);
	BitSet *bp=new BitSet(b);
	_bitsets.push_back(bp);
	_acceptancesig_vector[i].first=bp;
	  
	b=dra.acceptance().getAcceptance_U_forState(i);
	bp=new BitSet(b);
	_bitsets.push_back(bp);
	_acceptancesig_vector[i].second=bp;
      }
    }
    
    /** Destructor */
    ~AcceptanceSignatureContainer() {
      for (std::vector<BitSet*>::iterator it=_bitsets.begin();
	   it!=_bitsets.end();
	   ++it) {
	delete *it;
      }
    }

    /** 
     * Get the acceptance signature for state i.
     * @param i the state index
     */
    acceptance_signature_t get(unsigned int i) {
      return _acceptancesig_vector[i];
    }

  private:
    /** Type of a vector (state-id -> acceptance_signature) */
    typedef std::vector<acceptance_signature_t> acceptancesig_vector_t;

    /** Storage for the acceptance signatures */
    acceptancesig_vector_t _acceptancesig_vector;

    /**
     * Vector to store the BitSet pointers that have to
     * be cleaned up on destruction.
     */
    std::vector<BitSet*> _bitsets;
  };


  /** 
   * Functor that compares two DRA states based on their
   * acceptance signature.
   */
  class AcceptanceSignatureComparator {
  private:
    /** The acceptance signature container */
    AcceptanceSignatureContainer &_container;
  public:
    /** Constructor */
    AcceptanceSignatureComparator(AcceptanceSignatureContainer& container) :
      _container(container) {}

    /** 
     * Compares (less-than) two DRAState indizes based on their
     * acceptance signature.
     */
    bool operator()(const unsigned int x,
		    const unsigned int y) {
      typename AcceptanceSignatureContainer::acceptance_signature_t px, py;
      
      px = _container.get(x);
      py = _container.get(y);

      if (*px.first < *py.first) {
	return true;
      } else if (*py.first < *px.first) {
	return false;
      }

      if (*px.second < *py.second) {
	return true;
      }

      // py.second >= px.second
      return false;
    }
  };

public:
  /** 
   * Perform quotienting using bisimulation
   * @param dra the DRA to be optimized
   * @param printColoring print colorings on std::cerr?
   * @param detailedStates save detailed information on the interals in the state?
   * @param printStats print statistics on std::cerr?
   * @return shared_ptr to the quotiented DRA
   */
  typename DRA::shared_ptr
  optimizeBisimulation(DRA &dra, 
		       bool printColoring=false,
		       bool detailedStates=false,
		       bool printStats=false) {
    if (!dra.isCompact()) {dra.makeCompact();}

    state_vector states(dra.size());

    for (unsigned int i=0;i<dra.size();i++) {
      states[i]=i;
    }

    AcceptanceSignatureContainer accsig_container(dra);
    AcceptanceSignatureComparator accsig_comp(accsig_container);


    Coloring *coloring=new Coloring(dra, detailedStates);
    // generate initial coloring by running with the 
    // different acceptance signature
    Coloring *new_coloring=generateColoring(states, *coloring, accsig_comp);
    delete coloring;
    coloring=new_coloring;

    unsigned int old_size=dra.size();
    unsigned int initial_partition=coloring->countColors();

    unsigned int oldColors;
    do {
      oldColors=coloring->countColors();

      ColoredStateComparator cnc(*coloring, dra);      

      Coloring* new_coloring=generateColoring(states, *coloring, cnc);
      delete coloring;
      coloring=new_coloring;      
    } while (oldColors != coloring->countColors());

    if (printColoring) {
      std::cerr << *coloring << std::endl;
    }

    typename DRA::shared_ptr dra_new=generateDRAfromColoring(dra, 
							     *coloring,
							     detailedStates);
    delete coloring;

    unsigned int new_size=dra_new->size();
    
    if (printStats) {
      std::cerr << "Bisimulation: From (" << old_size << ") To (" << new_size << ") Initial: (" << initial_partition << ")" << std::endl;
    }
    return dra_new;
  }


private:
  /**
   * Generate a new coloring based on the Comparator comp 
   * (one iteration of refinement)
   * @param states A vector of the states
   * @param coloring The current coloring
   * @param comp the Comparator
   * @return a pointer to a newly created Coloring, memory ownership
   *         passes to the caller
   */
  template <class Comparator>
  Coloring *generateColoring(state_vector &states,
			     Coloring &coloring,
			     Comparator &comp) {
    std::sort(states.begin(), states.end(), comp);

    Coloring* result=new Coloring(coloring.size(), coloring.getFlagDetailed());

    if (states.size()==0) {return result;}

    state_vector::reverse_iterator 
      current=states.rbegin(), 
      last=states.rbegin();

    result->setColor(*current, result->newColor());
    
    while (++current != states.rend()) {
      // because states is sorted and we traverse 
      // from the end, either:
      //    *current  < *last with comp(current,last)==true
      // or *current == *last with !comp(current,last)
      if (comp(*current, *last)) {
	// -> we have to start a new color
	result->setColor(*current, result->newColor());
      } else {
	// -> more of the same, we stay with this color
	result->setColor(*current, result->currentColor());
      }

      last=current;
    }

    return result;
  }


  /**
   * Generate a new DRA from a coloring
   */
  typename DRA::shared_ptr 
  generateDRAfromColoring(DRA &oldDRA,
			  Coloring &coloring,
			  bool detailedStates) {
    typename DRA::shared_ptr 
      newDRA((DRA*)oldDRA.createInstance(oldDRA.getAPSet_cp()));
    

    newDRA->acceptance().newAcceptancePairs(oldDRA.acceptance().size());

    for (unsigned int color=0;
	 color<coloring.countColors();
	 ++color) {
      newDRA->newState();
    }

    unsigned int old_start_state=oldDRA.getStartState()->getName();
    unsigned int start_state_color=coloring.state2color(old_start_state);

    newDRA->setStartState(newDRA->get(start_state_color));

    const APSet &apset=newDRA->getAPSet();

    for (unsigned int color=0;
	 color<coloring.countColors();
	 ++color) {
      typename DRA::state_type *new_state = newDRA->get(color);

      unsigned int old_state_representative=coloring.color2state(color);
      
      typename DRA::state_type *old_state = 
	oldDRA[old_state_representative];

      if (detailedStates) {
	const BitSet& old_states=coloring.color2states(color);

	// create new description...
	if (old_states.cardinality()==1) {
	  if (old_state->hasDescription()) {
	    new_state->setDescription(old_state->getDescription());
	  }
	} else {
	  std::ostringstream s;
	  s << "<TABLE BORDER=\"1\" CELLBORDER=\"0\"><TR><TD>{</TD>";
	  
	  bool first=true;
	  for (BitSetIterator it=BitSetIterator(old_states);
	       it!=BitSetIterator::end(old_states);
	       ++it) {
	    if (first) {
	      first=false; 
	    } else {
	      s << "<TD>,</TD>";
	    }
	    
	    s << "<TD>";
	    if (!oldDRA[*it]->hasDescription()) {
	      s << *it;
	    } else {
	      s << oldDRA[*it]->getDescription();
	    }
	    s << "</TD>";
	  }
	  s << "<TD>}</TD></TR></TABLE>";
	  
	  new_state->setDescription(s.str());
	}
      }

      // Create appropriate acceptance conditions
      unsigned int old_state_index=old_state->getName();
      for (unsigned int i=0;
	   i<oldDRA.acceptance().size();
	   ++i) {
	if (oldDRA.acceptance().isStateInAcceptance_L(i, old_state_index)) {
	  new_state->acceptance().addTo_L(i);
	}

	if (oldDRA.acceptance().isStateInAcceptance_U(i, old_state_index)) {
	  new_state->acceptance().addTo_U(i);
	}
      }

      for (APSet::element_iterator label=
	     apset.all_elements_begin();
	   label!=apset.all_elements_end();
	   ++label) {

	typename DRA::state_type *old_to=
	  old_state->edges().get(*label);

	unsigned to_color=coloring.state2color(old_to->getName());
      
	new_state->edges().addEdge(*label, newDRA->get(to_color));
      }
    }
    
    return newDRA;
  }
};

#endif
