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



#ifndef NBABUILDER_H
#define NBABUILDER_H

/** @file
 * Provides class NBABuilder, a helper class for the NBA parsers.
 */

#include <map>

#include "APSet.hpp"
#include "NBA_I.hpp"
#include "LTLPrefixParser.hpp"
#include "common/Exceptions.hpp"

/**
 * A helper class used by the NBA parsers for building the NBA.
 */
template <class name_t>
class NBABuilder {
public:
  
  /** 
   * Constructor
   * @param nba the interface to the NBA
   */
  NBABuilder(NBA_I *nba) : 
    _nba(nba), 
    _all_states_are_final(false) {}

  /**
   * Find or add a state with a certain name.
   * @param name the name of the state
   * @return the index of the state
   */
  unsigned int findOrAddState(name_t name) {
    typename name2state_map_t::iterator it=name2state.find(name);
    if (it==name2state.end()) {
      unsigned int new_state=_nba->nba_i_newState();
      if (_all_states_are_final) {
	_nba->nba_i_setFinal(new_state, true);
      }
      name2state[name]=new_state;
      return new_state;
    } else {
      return (*it).second;
    }
  }

  /**
   * Add an additional name to an already existing state.
   * @param name the name
   * @param state the index of the state
   */
  void addAdditionalNameToState(name_t name, 
				unsigned int state) {
      name2state[name]=state;
  }

  /**
   * Add an edge from a state to a state with labeling specified
   * by a formula in propositional logic in prefix form.
   * @param from  the 'from' state index
   * @param to    the 'to' state index
   * @param guard the formula
   */
  void addEdge(unsigned int from,
	       unsigned int to,
	       const std::string& guard) {

    EdgeCreator ec(from, to, *_nba);
    
    LTLFormula_ptr ltl_guard=
      LTLPrefixParser::parse(guard, _nba->nba_i_getAPSet());
    //    std::cerr << ltl_guard->toStringPrefix() << std::endl;
    
    LTLFormula_ptr guard_dnf=ltl_guard->toDNF();
    //    std::cerr << guard_dnf->toStringPrefix() << std::endl;
    guard_dnf->forEachMonom(ec);
  }

  /**
   * Set the start state
   * @param state the state index
   */
  void setStartState(unsigned int state) {

    // TODO: Check if start state was already set..
    _nba->nba_i_setStartState(state);
  }

  /**
   * Set the final flag of a state
   * @param state the state index
   * @param value the value of the flag
   */
  void setFinal(unsigned int state, bool value=true) {
    _nba->nba_i_setFinal(state, value);
  }

  /**
   * Mark that the NBA has only accepting (final) states,
   * all states newly created will be automatically final.
   */
  void setAllStatesAreFinal() {
    _all_states_are_final=true;
  }

  /** Check if an atomic proposition is valid */
  bool isAP(std::string& ap) {
    return (_nba->nba_i_getAPSet()->find(ap) != -1);
  }
  
private:
  /** The type of a map from names to state indizes */
  typedef std::map<name_t, unsigned int> name2state_map_t;
  /** A map from names to state indizes */
  name2state_map_t name2state;

  /** The interface to the NBA */
  NBA_I *_nba;
  /** Should all states be final?*/
  bool _all_states_are_final;

  /** Functor to create the edges. */
  struct EdgeCreator {
    int _from, _to;
    NBA_I& _nba;
    
    explicit EdgeCreator(unsigned int from,
			 unsigned int to,
			 NBA_I& nba) : 
      _from(from), _to(to), _nba(nba) {}
    
    void operator()(APMonom& m) {
      _nba.nba_i_addEdge(_from, m, _to);
      }
  };
    

};


#endif
