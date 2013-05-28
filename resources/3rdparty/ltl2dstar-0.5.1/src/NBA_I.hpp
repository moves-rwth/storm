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


#ifndef NBA_I_H
#define NBA_I_H

#include "APMonom.hpp"
#include "APElement.hpp"
#include "APSet.hpp"

/**
 * Simple virtual interface to the functions in a NBA needed by
 * parsers to construct an NBA.
 */
class NBA_I {
public:
  /** Constructor */
  virtual ~NBA_I() {};

  /** 
   * Create a new state.
   * @return the index of the new state
   */
  virtual unsigned int nba_i_newState() = 0;

  /**
   * Add an edge from state <i>from</i> to state <i>to</i>
   * for the edges covered by the APMonom.
   * @param from the index of the 'from' state
   * @param m the APMonom
   * @param to the index of the 'to' state
   */
  virtual void nba_i_addEdge(unsigned int from,
			     APMonom& m,
			     unsigned int to) = 0;

  /**
   * Get the underlying APSet 
   * @return a const pointer to the APSet
   */
  virtual APSet_cp nba_i_getAPSet() = 0;

  /** 
   * Set the final flag (accepting) for a state.
   * @param state the state index
   * @param final the flag
   */
  virtual void nba_i_setFinal(unsigned int state, bool final) = 0;
  
  /**
   * Set the state as the start state.
   * @param state the state index
   */
  virtual void nba_i_setStartState(unsigned int state) = 0;
};


#endif
