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


#ifndef SIMPLEBITSET_H
#define SIMPLEBITSET_H

/** @file
 * Provides class SimpleBitSet, capable of storing up to 32 bits.
 */

#include "common/Exceptions.hpp"


// TODO: What if unsigned int is not >32bit?

/**
 * Class representing a bitset capable of storing up to 32 bits.
 */
class SimpleBitSet {
public:
  /**
   * Constructor
   */
  SimpleBitSet(unsigned int value=0) : bitset(value) {;};


  /**
   * Returns the bit at index.
   */  
  bool get(unsigned int index) const {
    // Argument check
    if (index >= 32) {
      THROW_EXCEPTION(IllegalArgumentException, "Index out of range!");
    }
    
    unsigned int mask=1<<index;
    return (bitset&mask)!=0;
  };


  unsigned int get() const {
    return bitset;
  };


  /**
   * Sets the bit at index to value.
   */
  void set(unsigned int index, bool value) {
    // Argument check
    if (index >= 32) {
      THROW_EXCEPTION(IllegalArgumentException, "Index out of range!");
    }
    
    unsigned int mask=1<<index;
    mask=~mask; // negate
    bitset&=mask; // set bit at index to 0
    if (value) {
      bitset|=1<<index;
    }
  };

  /**
   * Sets the value of this bitset
   * @param values integer representation of the bitset
   */
  void set(unsigned int values) {
    bitset=values;
  }

  
  /** Get the integer representation of this bitset */
  unsigned int getBitSet() const {return bitset;};

  operator unsigned int() const{
    return getBitSet();
  }

  bool operator==(const SimpleBitSet& other) const {
    return (this->bitset == other.bitset);
  }
private:
  unsigned int bitset;
};

#endif



