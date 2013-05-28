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


#ifndef LTL2DSTAR_Scheduler_HPP
#define LTL2DSTAR_Scheduler_HPP

/** @file
 * Provides class LTL2DSTAR_Scheduler, which allows translation of LTL to DRA/DSA
 * in multiple ways and combines the basic building blocks to choose the most
 * efficient.
 */

#include "LTL2DRA.hpp"
#include "Configuration.hpp"

/**
 * Allows translation of LTL to DRA/DSA
 * in multiple ways and combines the basic building blocks to choose the most
 * efficient.
 */
class LTL2DSTAR_Scheduler {
public:

  LTL2DSTAR_Scheduler(LTL2DRA& ltl2dra, bool opt_limits=false,
		      double alpha=1.0);

  DRA_ptr calculate(LTLFormula& ltl, LTL2DSTAR_Options ltl_opt);

  LTL2DRA& getLTL2DRA();

  bool flagOptLimits() {return _opt_limits;}

  unsigned int calcLimit(unsigned int limit);

  std::string getTimingInformation();

  /** Get the state of the StatNBA flag */
  bool flagStatNBA() {return _stat_NBA;}

  /** Set the value of the StatNBA flag */
  void flagStatNBA(bool value) {_stat_NBA=value;}
private:
  /** The LTL2DRA wrapper for Safra's algorithm and the external LTL->NBA translator */
  LTL2DRA& _ltl2dra;

  /** Use limiting? */
  bool _opt_limits;

  /** The limiting factor */
  double _alpha;

  /** Print stats on the NBA? */
  bool _stat_NBA;
};

#endif
