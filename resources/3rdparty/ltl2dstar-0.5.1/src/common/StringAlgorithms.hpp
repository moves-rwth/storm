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


#ifndef STRINGALGORITHMS_H
#define STRINGALGORITHMS_H

/** @file
 * Provides some string algorithms.
 */

#include <string>

/**
 * Provides some string algorithms
 */
class StringAlgorithms {
public:
  /** 
   * Replace all occurrences (in place) of pattern in string s by replacement.
   */
  static void replace_all(std::string& s, 
			  const std::string& pattern,
			  const std::string& replacement) {
    std::string::size_type pos=0;

    if (pattern.length()==0) {
      return;
    }

    while (pos>=0 && pos<s.length()) {
      pos=s.find(pattern, pos);

      if (pos==std::string::npos) {
	break;
      }

      s.replace(pos, pattern.length(), replacement);

      // Before:
      //   [----PPP---]
      //    0123456789
      //        ^ pos
      // 
      // After:
      //   [----RR---]
      //    0123456789
      //          ^ pos + replacement.length()

      pos+=replacement.length();
    }
  }
};

#endif
