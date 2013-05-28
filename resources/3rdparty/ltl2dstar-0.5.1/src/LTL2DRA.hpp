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


#ifndef LTL2DRA_HPP
#define LTL2DRA_HPP

/** @file
 * Provides class LTL2DRA, which is used as a wrapper to convert from
 * LTL to NBA and from NBA to DRA.
 */

#include "LTL2NBA.hpp"

#include "APSet.hpp"
#include "APElement.hpp"

#include "NBA.hpp"
#include "NBA_State.hpp"

#include "EdgeContainerExplicit_APElement.hpp"

#include "DA.hpp"
#include "DRA.hpp"
#include "DA_State.hpp"

#include "Configuration.hpp"

#include "StutterSensitivenessInformation.hpp"

#include "LTLFormula.hpp"

typedef NBA<APElement, EdgeContainerExplicit_APElement> NBA_t;
typedef NBA_t::shared_ptr NBA_ptr;

typedef DRA<APElement, EdgeContainerExplicit_APElement> DRA_t;
typedef DRA_t::shared_ptr DRA_ptr;

/**
 * Wrapper to convert from LTL to NBA and from NBA to DRA.
 */
class LTL2DRA {
public:
  LTL2DRA(Options_Safra& safra_opt,
	  LTL2NBA<NBA_t> *ltl2nba);

  /** Destructor */
  ~LTL2DRA() {}
  
  DRA_ptr ltl2dra(LTLFormula& ltl, 
		  LTL2DSTAR_Options options=LTL2DSTAR_Options());

  NBA_ptr ltl2nba(LTLFormula& ltl, bool exception_on_failure=false);

  DRA_ptr nba2dra(NBA_t& nba, 
		  unsigned int limit=0,
		  bool detailedStates=false,
		  StutterSensitivenessInformation::ptr stutter_information=
		  StutterSensitivenessInformation::ptr());

  /** Get the options for Safra's algorithm */
  Options_Safra& getOptions() {
    return _safra_opt;
  }

private:
  /** The options for Safra's algorithm */
  Options_Safra& _safra_opt;

  /** The wrapper for the external LTL-to-Buechi translator */
  LTL2NBA<NBA_t>* _ltl2nba;
};

#endif
