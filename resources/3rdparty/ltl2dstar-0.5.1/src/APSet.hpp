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


#ifndef APSET_H
#define APSET_H

/** @file 
 *Provides class APSet 
 */

#include <cstring>
#include <string>
#include "common/Exceptions.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/shared_ptr.hpp>

typedef unsigned int ap_index_t;

/**
 * Class representing a set of atomic propsitions (AP).
 * This class currently supports a maximum of 31 APs per set!
 */
class APSet {
public:

  /**
   * Constructor.
   */
  explicit APSet() {
    array=new std::string[MAX_AP];
    array_size=0;
  }

  /**
   * Destructor.
   */
  ~APSet() { if (array!=0) {delete[] array;}}

  /**
   * Adds a new AP to the set.
   * @param name the name of the AP
   * @return the index of the added AP
   */
  ap_index_t addAP(const std::string& name) {
    ap_index_t new_index=array_size;
  
    if (new_index>=MAX_AP) {
      THROW_EXCEPTION(IllegalArgumentException, "Can't add AP, APSet is full");
    }

    array[new_index]=name;  // make copy of name

    array_size=new_index+1;
    return new_index;
  }

  /**
   * Gets the name of a certain AP.
   * @param index index of the AP
   * @return string-ref with the name
   */
  const std::string& getAP(ap_index_t index) const {
    if (index<0 || index >= array_size) {
      THROW_EXCEPTION(IndexOutOfBoundsException, "Index out of bounds!");
    }

    return array[index];
  }

  /**
   * Searches for an existing AP in the APSet and returns the index.
   * @return the index of the AP, or -1 if not found.
   */
  int find(const std::string& s) const {
    for (ap_index_t i=0;
	 i<size();
	 i++) {
      if (array[i]==s) {
	return i;
      }
    }
    return -1;
  }

  /**
   * Get the size of this set
   * @return the number of APs in this set.
   */
  ap_index_t size() const {
    return array_size;
  }

  /**
   * Get the size of the powerset 2^APSet
   * @return the size of 2^AP
   */
  ap_index_t powersetSize() const {
    return (1<<size());
  }

  /** An iterator over all the APElements in 2^APSet. */
  typedef boost::counting_iterator<unsigned int> element_iterator;

  /** Return iterator pointing to the first APElement. */
  element_iterator all_elements_begin() const {
    return element_iterator(0);
  }
  
  /** Return iterator pointing to the last APElement. */
  element_iterator all_elements_end() const {
    return element_iterator(1<<size());
  }

  /** 
   * Equality check.
   * @param other the other APSet
   * @return <b>true</b> if this and the other APSet are equal
   */
  bool operator==(const APSet& other) const {
    if (this->size()!=other.size()) {
      return false;
    }

    for (unsigned int i=0;i<this->size();i++) {
      if (!(this->getAP(i) == other.getAP(i))) {
	return false;
      }
    }
    
    return true;
  }

  /**
   * Create a new APSet with the same number of 
   * atomic propositions, but named 'p0', 'p1', 'p2', ...
   * The caller takes ownership of the memory of the created APSet.
   * @return APSet* to newly created APSet
   */
  APSet *createCanonical() const {
    APSet *canonical=new APSet();
    
    for (unsigned int i=0;
	 i<size();
	 i++) {
      
      canonical->addAP(std::string("p")+
		       boost::lexical_cast<std::string>(i));
    }

    return canonical;
  }

  
  /** Maximum size of the APSet */
  static const ap_index_t MAX_AP=31;
private:
  /** Dummy constructor to prevent copying. */
  APSet(APSet const& other);
  /** Dummy operator= to prevent copying. */
  APSet& operator=(APSet const& other);

  /** The storage for the atomic propositions. */
  std::string* array;
  /** The size of the APSet. */
  ap_index_t array_size;
};

/** A reference counted pointer (boost::shared_ptr) to an APSet. */
typedef boost::shared_ptr<APSet> APSet_p;
/** A reference counted pointer (boost::shared_ptr) to a const APSet. */
typedef boost::shared_ptr<const APSet> APSet_cp;

#endif

