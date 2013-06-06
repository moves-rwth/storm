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


#ifndef DRA_H
#define DRA_H

/** @file
 * Provide class DRA<> which can store a deterministic Rabin or Streett automaton.
 */

#include "DA.hpp"
#include "DA_State.hpp"
#include "RabinAcceptance.hpp"
#include "APSet.hpp"
#include "APElement.hpp"
#include "DAUnionAlgorithm.hpp"

#include "common/Index.hpp"
#include "common/Exceptions.hpp"
#include <iterator>
#include <iostream>
#include <map>
#include <string>

#include <memory>
/**
 * A class representing a deterministic Rabin automaton.
 * <p>
 * For details on the template parameters, see class DA.
 * </p>
 * The DRA can be considered as a Streett automaton, if
 * a flag is set.
 */
template <typename Label, template <typename N> class EdgeContainer >
class DRA : public DA<Label, EdgeContainer, RabinAcceptance> {
 public:
  DRA(APSet_cp ap_set);
  virtual ~DRA();
  
  /** The type of the states of the DRA. */
  typedef typename DA<Label,EdgeContainer,RabinAcceptance>::state_type state_type;

  /** The type of the label on the edges. */
  typedef typename DA<Label,EdgeContainer,RabinAcceptance>::label_type label_type;

  /** The type of an iterator over the states. */
  typedef typename DA<Label,EdgeContainer,RabinAcceptance>::iterator iterator;

  /** The type of an iterator over the edges of a state. */
  typedef typename DA<Label,EdgeContainer,RabinAcceptance>::edge_iterator edge_iterator;

  /** Type of an iterator over the index of acceptance pairs. */
  typedef typename RabinAcceptance::acceptance_pair_iterator acceptance_pair_iterator;

  typedef RabinAcceptance acceptance_condition_type;

  /** Type of a reference counted pointer to the DRA (std::shared_ptr) */
  typedef std::shared_ptr< DRA<Label,EdgeContainer> > shared_ptr;

  /** Create a new instance of the automaton. */
  virtual DA<Label,EdgeContainer,RabinAcceptance> *createInstance(APSet_cp ap_set) {
    return new DRA<Label,EdgeContainer>(ap_set);
  }

  /** Make this automaton into an never accepting automaton */
  void constructEmpty() {
    state_type* n=DA<Label,EdgeContainer,RabinAcceptance>::newState();
    this->setStartState(n);
    
    for (APSet::element_iterator it_elem=
	   DA<Label,EdgeContainer,RabinAcceptance>::getAPSet().all_elements_begin();
	 it_elem!=DA<Label,EdgeContainer,RabinAcceptance>::getAPSet().all_elements_end();
	 ++it_elem) {
      APElement label=(*it_elem);
      n->edges().addEdge(label, n);
    }
  }

 
  /**
   * Print the DRA/DSA in v2 format to the output stream.
   * This function can compact the automaton, which may invalidate iterators!
   */
  void print_explicit_v2(std::ostream& out) {
    if (!this->isCompact()) {
      this->makeCompact();
    }

    this->print_da_explicit_v2(typeID(),
			       out);
  }

  /**
   * Print the DRA/DSA in DOT format to the output stream.
   * This function can compact the automaton, which may invalidate iterators!
   */
  void print_dot(std::ostream& out) {
    if (!this->isCompact()) {
      this->makeCompact();
    }

    this->print_da_dot(typeID(),
		       out);
  }

  /**
   * Output operator for the DRA/DSA.
   */
  friend std::ostream& operator<<(std::ostream& out, DRA& dra) {
    dra.print_explicit_v2(out);
    return out;
  }

  /** Output state label for DOT printing. 
   * @param out the output stream
   * @param state_index the state index
   */
  virtual void formatStateForDOT(std::ostream& out, unsigned int state_index) {
    state_type* cur_state=this->get(state_index);

    bool is_html=false;

    bool has_pos=false, has_neg=false;

    std::ostringstream acc_sig;
    for (unsigned int pair_index=0;pair_index<this->acceptance().size();pair_index++) {
      if (this->acceptance().isStateInAcceptance_L(pair_index, state_index)) {
	acc_sig << " +" << pair_index;
	has_pos=true;
      }

      if (this->acceptance().isStateInAcceptance_U(pair_index, state_index)) {
	acc_sig << " -" << pair_index;
	has_neg=true;
      }
    }
    
    std::string label;

    if (cur_state->hasDescription()) {
      std::string description=cur_state->getDescription();
      is_html=true;
      
      std::string acc_color;
      if (has_pos) {
	if (has_neg) {
	  acc_color="BGCOLOR=\"lightblue\"";
	} else {
	  acc_color="BGCOLOR=\"green\"";
	}
      } else {
	if (has_neg) {
	  acc_color="BGCOLOR=\"red\"";
	}
      }

      StringAlgorithms::replace_all(description, 
				    "<TABLE>", "<TABLE BORDER=\"0\" CELLBORDER=\"1\">");

      out << "label= < ";
      out << "<TABLE BORDER=\"0\"><TR><TD BORDER=\"1\">";
      out << state_index;
      out << "</TD><TD " << acc_color << ">";
      out << acc_sig.str();
      out << "</TD></TR><TR><TD COLSPAN=\"2\">";
      out << description;
      out << "</TD></TR></TABLE>";
    
      out << " >, shape=box";

    } else {
      out << "label= \"" << state_index;
      if (acc_sig.str().length()!=0) {
	out  << "\\n" << acc_sig.str();
      }
      out << "\"";

      if (!has_pos && !has_neg) {
	  out << ", shape=circle";
      } else {
	  out << ", shape=box";
      }
    }
    
    if (this->getStartState() == cur_state) {
      out << ", style=filled, color=black, fillcolor=grey";
    }
  }

  /**
   * Optimizes the acceptance condition.
   * This function may delete acceptance pairs,
   * which can invalidate iterators.
   */
  void optimizeAcceptanceCondition() {
    RabinAcceptance::acceptance_pair_iterator it=this->acceptance().acceptance_pair_begin();
    while (it!=this->acceptance().acceptance_pair_end()) {
      unsigned int id=*it;

      // increment iterator here so we can eventually delete
      // acceptance pair without side effects
      ++it;

      // L = L \ U
      if (this->acceptance().getAcceptance_L(id).intersects(this->acceptance().getAcceptance_U(id))) {
	BitSet L_minus_U=this->acceptance().getAcceptance_L(id);
	L_minus_U.Minus(this->acceptance().getAcceptance_U(id));
	this->acceptance().getAcceptance_L(id)=L_minus_U;
      }

      // remove if L is empty
      if (this->acceptance().getAcceptance_L(id).isEmpty()) {
	// no state is in L(id) -> remove
	this->acceptance().removeAcceptancePair(id);
      }
    }
  }

  /** Is this DRA considered as a Streett automaton? */
  bool isStreett() const {return _isStreett;}
  /** Consider this DRA as a Streett automaton. */
  void considerAsStreett(bool flag=true) {_isStreett=flag;}
  

  static
  shared_ptr calculateUnion(DRA& dra1, DRA& dra2,
			    bool trueloop_check=true,
			    bool detailed_states=false) {
    if (dra1.isStreett() ||
	dra2.isStreett()) {
      THROW_EXCEPTION(Exception, "Can not calculate union for Streett automata");
    }

    return DAUnionAlgorithm<DRA>::calculateUnion(dra1, dra2,
						 trueloop_check,
						 detailed_states);
  }

  static
  shared_ptr calculateUnionStuttered(DRA& dra1, DRA& dra2,
				     StutterSensitivenessInformation::ptr stutter_information,
				     bool trueloop_check=true,
				     bool detailed_states=false) {
    if (dra1.isStreett() ||
	dra2.isStreett()) {
      THROW_EXCEPTION(Exception, "Can not calculate union for Streett automata");
    }

    return DAUnionAlgorithm<DRA>::calculateUnionStuttered(dra1, dra2,
							  stutter_information,
							  trueloop_check,
							  detailed_states);
  }

  
 private:
  /** Return a string giving the type of the automaton. */
  std::string typeID() const {
    if (isStreett()) {
      return std::string("DSA");
    } else {
      return std::string("DRA");
    }
  }
  
  /** Marker, is this DRA considered as a Streett automaton? */
  bool _isStreett;
};


/**
 * Constructor.
 * @param ap_set the underlying APSet
 */
template <typename Label, template <typename N> class EdgeContainer >
DRA<Label, EdgeContainer>::DRA(APSet_cp ap_set) 
  : DA<Label,EdgeContainer,RabinAcceptance>(ap_set), _isStreett(false) {
}


/**
 * Destructor.
 */
template <typename Label, template <typename N> class EdgeContainer >
DRA<Label, EdgeContainer>::~DRA() {

}

#endif
