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


#ifndef APELEMENT_H
#define APELEMENT_H

/** @file 
 * Provides class APElement.
 */

#include "common/SimpleBitSet.hpp"
#include "APSet.hpp"
#include <string>

/**
 * Class representing one element of 2^AP. 
 * It is currently capable of handling APSets with up 32 Elements,
 * representing one APElement by a SimpleBitSet (=unsigned int).
 */

class APElement : public SimpleBitSet {
 public:
  /** Constructor */
  APElement(unsigned int value) : SimpleBitSet(value) {;}

  /** Copy constructor */
  APElement(SimpleBitSet& sbs) : SimpleBitSet(sbs) {;}

  /** 
   * Convert to a string representation.
   * @param ap_set The underlying APSet
   * @param spaces Print spaces in front of positive AP? 
   */
  std::string toString(const APSet& ap_set, bool spaces=true) const {
    if (ap_set.size()==0) {return std::string("true");}
    std::string r("");
    for (unsigned int i=0;i<ap_set.size();i++) {
      if (i>=1) {
	r.append("&");
      }
      if (!this->get(i)) {
	r.append("!");
      } else {
	if (spaces) {
	  r.append(" ");
	}
      }
      r.append(ap_set.getAP(i));
    }
    return r;
  }

  /** 
   * Generate a representation in LBTT format.
   * @param ap_set The underlying APSet
   */
  std::string toStringLBTT(const APSet& ap_set) const {
    if (ap_set.size()==0) {
      return std::string("t");
    }

    std::string r("");
    for (unsigned int i=0;i+1<ap_set.size();i++) {
      r+="& ";
    }

    for (unsigned int i=0;i<ap_set.size();i++) {
      if (!this->get(i)) {
	r+="! ";
      }
      r+=ap_set.getAP(i);
      r+=" ";
    }
    return r;
  }

  /**
   * Prefix increment, goes to next APElement.
   * @return an APElement representing the next APElement
   */
  APElement& operator++() {
    set(get()+1);
    return *this;
  }

  /**
   * Check for equality.
   * @param other the other APElement
   * @return <b>true</b> if this and the other APElement are equal.
   */
  bool operator==(const APElement& other) const{
    return this->getBitSet()==other.getBitSet();
  }

  /**
   * Get the internal representation.
   */
  operator unsigned int() {
    return getBitSet();
  }
};

#endif

