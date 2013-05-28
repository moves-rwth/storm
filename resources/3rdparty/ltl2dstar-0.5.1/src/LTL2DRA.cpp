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


/** @file
 * Provides implementations for class LTL2DRA
 */


#include "LTL2DRA.hpp"

#include "DAUnionAlgorithm.hpp"
#include "DRAOptimizations.hpp"

#include "NBA2DRA.hpp"
#include "DRA2NBA.hpp"

#include "LTLSafetyAutomata.hpp"

#include "common/TempFile.hpp"
#include "common/RunProgram.hpp"
#include "common/Exceptions.hpp"


#include <typeinfo>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

/**
 * Constructor.
 * @param safra_opt The options for Safra's algorithm
 * @param ltl2nba The LTL2NBA translator wrapper
 */
LTL2DRA::LTL2DRA(Options_Safra& safra_opt,
		 LTL2NBA<NBA_t> *ltl2nba) :
  _safra_opt(safra_opt),
  _ltl2nba(ltl2nba) {}

/**
 * Convert an LTL formula to a DRA.
 * @param ltl the LTL formula
 * @param options which operators are allowed
 * @return a shared_ptr to the DRA
 */
DRA_ptr LTL2DRA::ltl2dra(LTLFormula& ltl,
			 LTL2DSTAR_Options options) {
  APSet_cp ap_set=ltl.getAPSet();

  LTLFormula_ptr ltl_pnf=ltl.toPNF();
  
  if (options.allow_union &&
      ltl_pnf->getRootNode()->getType() == LTLNode::T_OR) {
    LTLFormula_ptr ltl_left=
      ltl_pnf->getSubFormula(ltl_pnf->getRootNode()->getLeft());
    
    LTLFormula_ptr ltl_right=
      ltl_pnf->getSubFormula(ltl_pnf->getRootNode()->getRight());
   
    LTL2DSTAR_Options rec_opt=options;
    rec_opt.recursion();

    DRA_ptr dra_left=ltl2dra(*ltl_left, rec_opt);
    DRA_ptr dra_right=ltl2dra(*ltl_right, rec_opt);
    
    return DRA_t::calculateUnion(*dra_left,
				 *dra_right,
				 _safra_opt.union_trueloop);
  }

  if (options.safety) {
    LTLSafetyAutomata lsa;
    
    DRA_ptr safety_dra(lsa.ltl2dra<DRA_t>(ltl,
					  options.scheck_path));
    
    if (safety_dra.get()!=0) {
      return safety_dra;
    }
  }
  
  DRA_ptr dra(new DRA_t(ap_set));
  
  NBA_ptr nba=ltl2nba(*ltl_pnf);
  
  if (nba.get()==0) {
    THROW_EXCEPTION(Exception, "Couldn't create NBA from LTL formula");
  };
  
  NBA2DRA nba2dra(_safra_opt);
  
  nba2dra.convert(*nba, *dra);
  
  if (options.optimizeAcceptance) {
    dra->optimizeAcceptanceCondition();
  }
  
  if (options.bisim) {
    DRAOptimizations<DRA_t> dra_optimizer;
    dra=dra_optimizer.optimizeBisimulation(*dra);
  }
  
  return dra;
}


/**
 * Convert an LTL formula to an NBA using the specified LTL2NBA translator
 * @param ltl the formula
 * @param exception_on_failure if false, on error a null pointer is returned
 * @return a shared_ptr to the created NBA
 */
NBA_ptr LTL2DRA::ltl2nba(LTLFormula& ltl, bool exception_on_failure) {
  assert(_ltl2nba!=0);
  
  NBA_ptr nba(_ltl2nba->ltl2nba(ltl));

  if (exception_on_failure && !nba) {
    THROW_EXCEPTION(Exception, "Couldn't generate NBA from LTL formula!");
  }

  return nba;
}


/**
 * Convert an NBA to a DRA using Safra's algorithm.
 * If limit is specified (>0), the conversion is 
 * aborted with LimitReachedException when the number of 
 * states exceeds the limit.
 * @param nba the formula
 * @param limit a limit on the number of states (0 for no limit)
 * @param detailedStates save detailed interal information (Safra trees) 
 *                       in the generated states
 * @param stutter_information Information about the symbols that can be stuttered
 * @return a shared_ptr to the created DRA
 */
DRA_ptr LTL2DRA::nba2dra(NBA_t& nba, 
			 unsigned int limit,
			 bool detailedStates,
			 StutterSensitivenessInformation::ptr stutter_information) {
  DRA_ptr dra(new DRA_t(nba.getAPSet_cp()));
  
  NBA2DRA nba2dra(_safra_opt, detailedStates, stutter_information);
  
  try {
    nba2dra.convert(nba, *dra, limit);
  } catch (LimitReachedException& e) {
    dra.reset();
    // rethrow to notify caller
    throw;
  }

  return dra;  
}
