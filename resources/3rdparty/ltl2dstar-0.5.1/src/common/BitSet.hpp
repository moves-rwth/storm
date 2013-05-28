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



#ifndef BITSET_H
#define BITSET_H

/** @file BitSet.hpp
 * Provides class BitSet.
 */

#include "common/Exceptions.hpp"
#include "common/SimpleBitSet.hpp"

#include <iostream>
#include <string>

/**
 * \brief Variable size bitset.
 *
 * A BitSet implements a variable sized vector of bits, 
 * each of which can be either <code>true</code> or 
 * <code>false</code>, indexed by a non-negative integer.
 * By default, all bits are <code>false</code>.
 * The vector grows as needed. 
 *
 * This implementation allows access to each individual
 * bit and provides logical operators that work with
 * two BitSets.
 */
class BitSet {
 public:
  /// Index type.
  typedef signed int index_t;

  BitSet();
  BitSet(const BitSet& other);
  BitSet(const SimpleBitSet& other);
  ~BitSet();

  BitSet& operator=(const BitSet& other);


  void And(const BitSet& other);
  void Or(const BitSet& other);
  void AndNot(const BitSet& other);

  /** Perform set operation Intersection */ 
  void Intersect(const BitSet& other) {this->And(other);}
  /** Perform set operation Union */ 
  void Union(const BitSet& other) {this->Or(other);}
  /** Perform set operation Minus */ 
  void Minus(const BitSet& other) {this->AndNot(other);}

    
  bool intersects(const BitSet& other) const;

  void clear();
  void clear(index_t bitIndex);
  void clear(index_t fromIndex, index_t toIndex);
  unsigned int cardinality() const;
  void flip(index_t bitIndex);
  void flip(index_t fromIndex, index_t toIndex);
  bool get(index_t bitIndex) const;
  bool isEmpty() const;
  index_t length() const;
  index_t nextClearBit(index_t fromIndex) const;
  index_t nextSetBit(index_t fromIndex) const;
  void set(index_t bitIndex);
  void set(index_t bitIndex, bool value);

  bool operator==(const BitSet& other) const;
  bool operator<(const BitSet& other) const;

  friend std::ostream& operator<<(std::ostream& out, const BitSet& bs);
  std::string toString() const;
 
  /** Calculate hash value using a HashFunction */
  template <class HashFunction>
  void hashCode(HashFunction& hashfunction) const {
    bool all_zero=true;
    for (index_t i=storage_size;
	 i>0;
	 --i) {
      if (all_zero) {
	if (storage[i-1]!=0) {
	  hashfunction.hash(storage[i-1]);
	  all_zero=false;
	}
      } else {
	hashfunction.hash(storage[i-1]);
      }
    }
  }

private:
  /** The underlying storage type */
  typedef unsigned int storage_t;

  /** The number of allocated storage units. */
  index_t storage_size;

  /** The storage area. */
  storage_t *storage;

  /** The number of bits per storage unit */
  static const index_t bits_per_storage=sizeof(storage_t)*8;

  void checkIndex(index_t index) const; 
  void grow(index_t maxIndex);

public:
  index_t getHighestSetBit(storage_t s) const;
  index_t getLowestSetBit(storage_t s) const;
};

#ifdef INLINE_BITSET
 // inline implementation
 #include "common/BitSet.cpp"
#endif

#endif
