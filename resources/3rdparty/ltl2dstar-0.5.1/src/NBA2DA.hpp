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


#ifndef NBA2DA_H
#define NBA2DA_H

/** @file
 * Provides class NBA2DA, which converts an NBA to a DA
 */

#include "APSet.hpp"
#include "APElement.hpp"
#include "DRA.hpp"
#include "NBA.hpp"

#include "SafraTree.hpp"
#include "SafrasAlgorithm.hpp"

#include "common/Exceptions.hpp"
#include "common/hash_map.hpp"
#include "common/BitSet.hpp"
#include "common/BitSetIterator.hpp"
#include "GraphAlgorithms.hpp"
#include "DBA2DRA.hpp"
#include "StateMapper.hpp"

#include <vector>
#include <stack>
#include <memory>

#include <fstream>

#include "Configuration.hpp"


/**
 * Convert an NBA to a DA using the specified Algorithm_t and StateMapper_t
 */
template < typename Algorithm_t, 
	   typename DA_t,
	   typename StateMapper_t=StateMapper<typename Algorithm_t::result_t,
					      typename Algorithm_t::state_t,
					      typename DA_t::state_type> >
class NBA2DA {
private:
  /** Save detailed information on the Safra trees in the states? */
  bool _detailed_states;

public:
  /** Constructor */
  NBA2DA(bool detailedStates=false) : _detailed_states(detailedStates) {
    ; // nop
  }

  ~NBA2DA() {
    ; //nop
  }

  /**
   * Generate a DA using the Algorithm
   * Throws LimitReachedException if a limit is set (>0) and
   * there are more states in the generated DA than the limit. 
   * @param algo the algorithm 
   * @param da_result the DA where the result is stored 
   *        (has to have same APSet as the nba)
   * @param limit a limit for the number of states (0 disables the limit).
   */
  void convert(Algorithm_t& algo, 
	       DA_t& da_result,
	       unsigned int limit=0) {
    StateMapper_t state_mapper;

    const APSet& ap_set=da_result.getAPSet();

    if (algo.checkEmpty()) {
      da_result.constructEmpty();
      return;
    }

    typedef typename DA_t::state_type da_state_t;
    typedef typename Algorithm_t::state_t algo_state_t;
    typedef typename Algorithm_t::result_t algo_result_t;

    algo.prepareAcceptance(da_result.acceptance());

    algo_state_t start=algo.getStartState();
    da_state_t* start_state=da_result.newState();
    start->generateAcceptance(start_state->acceptance());
    if (_detailed_states) {
      start_state->setDescription(start->toHTML());
    }

    state_mapper.add(start, start_state);
    da_result.setStartState(start_state);

    typedef std::pair<algo_state_t, da_state_t*> unprocessed_value;

    std::stack< unprocessed_value > unprocessed;
    unprocessed.push(unprocessed_value(start, start_state));

    while (!unprocessed.empty()) {
      unprocessed_value top=unprocessed.top();
      unprocessed.pop();      

      algo_state_t cur=top.first;
      da_state_t *from=top.second;

      for (APSet::element_iterator it_elem=ap_set.all_elements_begin();
	   it_elem!=ap_set.all_elements_end();
	   ++it_elem) {
	
	APElement elem=*it_elem;

	algo_result_t result=algo.delta(cur, elem);

	da_state_t* to=state_mapper.find(result);

	if (!to) {
	  to=da_result.newState();
	  result->getState()->generateAcceptance(to->acceptance());

	  if (_detailed_states) {
	    to->setDescription(result->getState()->toHTML());
	  }

	  state_mapper.add(result->getState(), to);
	  unprocessed.push(unprocessed_value(result->getState(), to));
	}
 
	from->edges().set(elem, to);
	
	if (limit!=0 && da_result.size()>limit) {
	  THROW_EXCEPTION(LimitReachedException, "");
	}
      }
    }

  }
};

#endif
