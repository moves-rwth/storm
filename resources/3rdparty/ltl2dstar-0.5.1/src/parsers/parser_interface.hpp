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


#ifndef PARSER_INTERFACE_H
#define PARSER_INTERFACE_H

/** @file
 * Provides interface to the parsers generated with bison and flex.
 */

#include <cstdio>
#include "NBA_I.hpp"

namespace nba_parser_lbtt {
  /** Parse a file in LBTT format and save result in nba */
  int parse(FILE *, NBA_I *nba, bool debug=false);
};

namespace nba_parser_promela {
  /** Parse a file in PROMELA (SPIN) format and save result in nba */
  int parse(FILE *, NBA_I *nba, bool debug=false);
};



#endif
