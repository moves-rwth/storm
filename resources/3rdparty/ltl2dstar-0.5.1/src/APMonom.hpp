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


#ifndef APMONOM_H
#define APMONOM_H

/** @file
 * Provides class APMonom.
 */

#include "common/Exceptions.hpp"
#include "common/SimpleBitSet.hpp"
#include <cassert>
#include <iostream>

// As TRUE and FALSE are defined to equal true and false on Windows, they need to be undefined.
#undef TRUE
#undef FALSE

/**
 * Class representing a subset of 2^AP where AP is the set of 
 * atomic propositions (APSet). It stores two bits per AP: 
 * One bit to determine if the value of this AP is set, 
 * the second bit to store the value.<br>
 * Additionally, the APMonom can have the special values
 * TRUE or FALSE.<br>
 * Currently, the APSet can only have 31 members.
 */
class APMonom {
public:
  /**
   * Constructor.
   */
  APMonom() {*this=TRUE;}

  /**
   * Constructor.
   * @param set_bits integer representation of the bits which are set
   * @param value_bits integer representation of the value bits
   */
  APMonom(unsigned int set_bits, unsigned int value_bits) {
    bits_set.set(set_bits);
    bits_value.set(value_bits);
  }



  /**
   * Is the AP set?
   * @param index index of AP
   * @return <b>true</b> if AP is set
   */
  bool isSet(unsigned int index) const {
    if (!isNormal()) {
      THROW_EXCEPTION(IllegalArgumentException, "Can't get AP, is either TRUE/FALSE!");
    }
    return bits_set.get(index);
  }

  /**
   * Gets the value for this AP. You can't get the value if the AP is not set.
   * @param index index of AP
   * @return <b>true</b> if AP is true
   */
  bool getValue(unsigned int index) const {
    if (!isNormal()) {
      THROW_EXCEPTION(IllegalArgumentException, "Can't get AP, is either TRUE/FALSE!");
    }

    if (!bits_set.get(index)) {
      THROW_EXCEPTION(IllegalArgumentException, "Can't get value: AP not set!");
    }

    return bits_value.get(index);
  }

  /**
   * Sets the value for this AP. Implicitly, it also sets the AP to 'set'.
   * @param index index of AP
   * @param value value of AP
   */
  void setValue(unsigned int index, bool value) {
    bits_set.set(index, true);
    bits_value.set(index, value);

    #ifdef APMONOM_DEBUG
    assert(isNormalized());
    #endif
  }


  /**
   * Perform a logical AND operation of this APMonom with a single AP.
   * @param index index index of AP
   * @param value value of AP
   */
  void andAP(unsigned int index, bool value) {
    if (isFalse()) {return;}

    if (!isTrue()) {
      if (isSet(index) && getValue(index)!=value) {
	// contradiction
	*this=FALSE;
	return;
      }
    }

    setValue(index, value);
  }


  /**
   * Unsets this AP.
   * @param index index of AP
   */
  void unset(unsigned int index) {
    bits_value.set(index,false);
    bits_set.set(index, false);

    #ifdef APMONOM_DEBUG
    assert(isNormalized());
    #endif
  }


  /**
   * Checks if this APMonom is equivalent to TRUE.
   * @return <b>true</b> if this APMonom is TRUE
   */
  bool isTrue() const {
    return *this==TRUE;
  }

  /**
   * Checks if this APMonom is equivalent to FALSE.
   * @return <b>true</b> if this APMonom is FALSE
   */
  bool isFalse() const {
    return *this==FALSE;
  }

  /**
   * Checks if this APMonom is a normal APMonon (not equivalent to TRUE or FALSE).
   * @return <b>true</b> if this APMonom is normal (not TRUE/FALSE).
   */
  bool isNormal() const {
    return bits_set!=0;
  }

  /**
   * Provides access to the underlying bitset representing the
   * value (AP occurs in positive or negative form).
   * @return the SimpleBitSet of the values
   */
  SimpleBitSet getValueBits() const {
    #ifdef APMONOM_DEBUG
    assert(isNormalized());
    #endif
    return bits_value;
  }

  /**
   * Provides access to the underlying bitset representing the
   * bits that are set (AP occurs).
   * @return the SimpleBitSet of the occuring APs
   */
  SimpleBitSet getSetBits() const {
    #ifdef APMONOM_DEBUG
    assert(isNormalized());
    #endif
    return bits_set;
  };


  /**
   * Performs an intersection check.
   * @param m1 the first APMonom
   * @param m2 the second APMonom
   * @return <b>true</b> if the intersection of <i>m1</i> and <i>m2</i> is empty.
   */
  static bool isIntersectionEmpty(const APMonom& m1,
				  const APMonom& m2) {
    // check if there are contradicting values 
    unsigned int set_in_both=
      m1.getSetBits() & m2.getSetBits();

    if ((m1.getValueBits() & set_in_both) !=
	(m2.getValueBits() & set_in_both)) {
      // contradiction 
      return true;
    } else {
      return false;
    }
  }

  /**
   * Perform logical conjunction with other APMonom.
   * @param other the other APMonom
   */
  APMonom operator&(const APMonom& other) const {
    if (this->isFalse() || other.isFalse()) {
      return FALSE;
    }

    if (this->isTrue()) {return other;}
    if (other.isTrue()) {return *this;}

    // both are not TRUE/FALSE:

    if (isIntersectionEmpty(*this, other)) {
      //  return APMonom equivalent to false
      return FALSE;
    }

    // both Monoms are not contradicting...
    unsigned int result_set=
      this->getSetBits() | other.getSetBits();

    unsigned int result_value=
      this->getValueBits() | other.getValueBits();

    return APMonom(result_set, result_value);
  }

  /**
   * Perform 'minus' operation (equal to *this & !other).
   * @param other the other APMonom
   */
  APMonom operator-(const APMonom& other) const {
    if (this->isFalse()) {
      // false & anything == false
      return FALSE;
    }
    
    if (other.isFalse()) {
      // *this & !(false) == *this & true == *this
      return *this;
    }

    if (other.isTrue()) {
      // *this & !(true) == *this & false == false
      return FALSE;
    }

    // the result will be false, if there are two set bits
    // with equal value
    unsigned int set_in_both=
      this->getSetBits() & other.getSetBits();

    if ((this->getValueBits() & set_in_both) !=
        ((~other.getValueBits()) & set_in_both)) {
      // return false;
      return FALSE;
    }

    unsigned int result_set=
      this->getSetBits() | other.getSetBits();

    unsigned int result_value=
      this->getValueBits() & (~(other.getValueBits()));

    return APMonom(result_set, result_value);
  }


  /**
   * Checks for equality.
   * @param other the other APMonom
   * @return <b>true</b> if this and the other APMonom are equal
   */
  bool operator==(const APMonom& other) const {
    return (this->getValueBits()==other.getValueBits()) &&
      (this->getSetBits()==other.getSetBits());
  }


  /**
   * Output APMonom on an ostream.
   * @param out the output stream
   * @param m the APMonom
   */
  friend std::ostream& operator<<(std::ostream& out,
				  const APMonom& m) {
    if (m.isTrue()) {
      out << "true";
    } else if (m.isFalse()) {
      out << "false";
    } else {
      for (unsigned int i=0;i<32;i++) {
	if (m.isSet(i)) {
	  if (m.getValue(i)) {
	    out << "+" << i;
	  } else {
	    out << "-" << i;
	  }
	}
      }
    }

    return out;
  }

  /** An APMonom representing TRUE */
  static const APMonom TRUE;
  /** An APMonom representing FALSE */
  static const APMonom FALSE;

private:
  /** The bitset for the values */
  SimpleBitSet bits_value;
  /** The bitset for the occurence */
  SimpleBitSet bits_set;
  
  /** Checks to see if the bitset representation is normalized. */
  bool isNormalized() {
    if (isTrue() || isFalse()) {
      return true;
    }

    return (bits_value.getBitSet() & ~(bits_set.getBitSet())) == 0;
  }
};


#endif

