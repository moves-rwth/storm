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


#ifndef NBA2DRA_H
#define NBA2DRA_H

/** @file
 * Provides class NBA2DRA, which converts an NBA to a DRA 
 * using Safra's algorithm.
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

#include "NBAAnalysis.hpp"

#include "StateMapper.hpp"
#include "StateMapperFuzzy.hpp"
#include "NBA2DA.hpp"
#include "StutteredNBA2DA.hpp"

#include <vector>
#include <stack>
#include <memory>

#include <fstream>

#include "Configuration.hpp"

/**
 * Convert an NBA to a DRA using Safra's algorithm
 */
class NBA2DRA {
private:
  /** The options */
  Options_Safra _options;
  /** Save detailed information on the Safra trees in the states? */
  bool _detailed_states;

  // stuttering
  StutterSensitivenessInformation::ptr _stutter_information;

public:
  /** Constructor */
  NBA2DRA() : _detailed_states(false) {
    ; // nop
  }

  /** Constructor.
   * @param options Options_Safra specifying whether to stutter, etc...
   * @param detailedStates generate detailed descriptions for the states?
   * @param stutter_information Information about the symbols that may be stuttered
   */
  NBA2DRA(Options_Safra& options, 
	  bool detailedStates=false, 
	  StutterSensitivenessInformation::ptr stutter_information=StutterSensitivenessInformation::ptr()) : 
    _options(options), _detailed_states(detailedStates),
    _stutter_information(stutter_information) {
    ; // nop
  }

  ~NBA2DRA() {
    
  }


  /**
   * Convert an NBA to an DRA (having APElements as edge labels).
   * Throws LimitReachedException if a limit is set (>0) and
   * there are more states in the generated DRA than the limit. 
   * @param nba the NBA
   * @param dra_result the DRA where the result is stored 
   *        (has to have same APSet as the nba)
   * @param limit a limit for the number of states (0 disables the limit).
   */
  template < typename NBA_t, typename DRA_t>
  void convert(NBA_t& nba,
	       DRA_t& dra_result,
	       unsigned int limit=0) {

    if (nba.size()==0 ||
	nba.getStartState()==0) {
      // the NBA is empty -> construct DRA that is empty
      
      dra_result.constructEmpty();
      return;
    }

    if (_options.dba_check && nba.isDeterministic()) {
      DBA2DRA::dba2dra(nba, dra_result);
      return;
    }

    if (_options.stutter_closure) {
      if (_stutter_information && 
	  !_stutter_information->isCompletelyInsensitive()) {
	std::cerr << "WARNING: NBA might not be 100% stutter insensitive, applying stutter closure can create invalid results!" << std::endl;
      }
      
      boost::shared_ptr<NBA_t> nba_closed=
	NBAStutterClosure::stutter_closure(nba);
      
      if (can_stutter()) {      
	convert_safra_stuttered(*nba_closed, dra_result, limit);
	return;
      }
      
      convert_safra(*nba_closed,dra_result,limit);
      return;
    }


    if (can_stutter()) {      
      convert_safra_stuttered(nba, dra_result, limit);
      return;
    }

    convert_safra(nba,dra_result,limit);

    return;

  }
  
  /** 
   * Is stuttering allowed?
   */
  bool can_stutter() {
    if (!_stutter_information) {
      return false;
    }

    if (_options.stutter && _stutter_information->isCompletelyInsensitive()) {
      return true;
    }

    if (_options.stutter && _stutter_information->isPartiallyInsensitive()) {
      return true;
    }
    
    return false;
  }


  /**
   * Provides CandidateMatcher for SafraTrees
   */
  class SafraTreeCandidateMatcher {
  public:
    static bool isMatch(const SafraTreeTemplate_ptr temp, const SafraTree_ptr tree) {
      return temp->matches(*tree);
    };
    
    static bool abstract_equal_to(const SafraTree_ptr t1, const SafraTree_ptr t2) {
      return t1->structural_equal_to(*t2);
    }
    
    static bool abstract_less_than(const SafraTree_ptr t1, const SafraTree_ptr t2) {
      return t1->structural_less_than(*t2);
    }
    
    template <typename HashFunction>
    static void abstract_hash_code(HashFunction& hash, SafraTree_ptr t) {
      t->hashCode(hash, true);
    }
  };



  /**
   * Convert the NBA to a DRA using Safra's algorithm
   * @param nba the NBA
   * @param dra_result the result DRA
   * @param limit limit for the size of the DRA
   */
  template < typename NBA_t,
	     typename DRA_t>
  void convert_safra(NBA_t& nba,
		     DRA_t& dra_result,
		     unsigned int limit=0) {
    typedef SafrasAlgorithm<NBA_t> safra_t;
    safra_t safras_algo(nba, _options);
    
    if (!_options.opt_rename) {
      NBA2DA<safra_t, DRA_t> 
	nba2da(_detailed_states);
      
      nba2da.convert(safras_algo, dra_result, limit);
    } else {
      typedef typename SafrasAlgorithm<NBA_t>::result_t result_t;
      typedef typename SafrasAlgorithm<NBA_t>::state_t key_t;
      
      NBA2DA<safra_t,
	DRA_t, 
	StateMapperFuzzy<result_t, key_t, typename DRA_t::state_type, SafraTreeCandidateMatcher> >
	nba2da_fuzzy(_detailed_states);
      
      nba2da_fuzzy.convert(safras_algo, dra_result, limit);
    }
  }


  /**
   * Convert the NBA to a DRA using Safra's algorithm, using stuttering
   * @param nba the NBA
   * @param dra_result the result DRA
   * @param limit limit for the size of the DRA
   */  
  template < typename NBA_t,
	     typename DRA_t >
  void convert_safra_stuttered(NBA_t& nba, DRA_t& dra_result, unsigned int limit=0) {
    typedef SafrasAlgorithm<NBA_t> safra_t;
    safra_t safras_algo(nba, _options);
    
    StutteredNBA2DA<safra_t, DRA_t> 
      nba2dra_stuttered(_detailed_states, _stutter_information);
    
    nba2dra_stuttered.convert(safras_algo, dra_result, limit);
  }
  
};


#endif
