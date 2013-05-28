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


#ifndef BITSET_CPP
#define BITSET_CPP

/** @file BitSet.cpp
 * Provides implementation of class BitSet.
 * If INLINE_BITSET is defined, the functions are inlined
 */

#include "common/BitSet.hpp"
#include "common/Exceptions.hpp"

#include <cassert>
#include <sstream>
#include <algorithm>

//#define BITSET_CONSERVATIVE 1

#ifdef INLINE_BITSET
 #define _BITSET_INLINE inline
#else
 #define _BITSET_INLINE
#endif

/**
 * Constructor.
 */
_BITSET_INLINE BitSet::BitSet() {
  storage=new storage_t[1];
  storage[0]=0;
  storage_size=1;
  //  bits_per_storage=sizeof(storage_t)*8;
}

/**
 * Copy-Constructor.
 */
_BITSET_INLINE BitSet::BitSet(const BitSet& other) :
  storage_size(other.storage_size)
  // ,  bits_per_storage(other.bits_per_storage)
{
  this->storage=new storage_t[storage_size];
  for (index_t i=0;i<storage_size;i++) {
    storage[i]=other.storage[i];
  }
}

/**
 * Constructor from SimpleBitSet.
 */
_BITSET_INLINE BitSet::BitSet(const SimpleBitSet& other) {
  storage=new storage_t[1];
  storage[0]=other.getBitSet();
  storage_size=1;
  //  bits_per_storage=sizeof(storage_t)*8;
}


/**
 * Destructor.
 */
_BITSET_INLINE BitSet::~BitSet() {
  delete[] storage;
}


/**
 * Assignment operator.
 */
_BITSET_INLINE BitSet& BitSet::operator=(const BitSet& other) {
  // Handle self-assignment
  if (&other==this) {return *this;}

  delete[] this->storage;
  this->storage_size=other.storage_size;
  //  this->bits_per_storage=other.bits_per_storage;
  this->storage=new storage_t[storage_size];
  for (index_t i=0;i<storage_size;i++) {
    storage[i]=other.storage[i];
  }

  return *this;
}


/**
 * Compares two BitSets for equality.
 * Equality is defined as: <br>
 * For all indexes <i>i</i>, <code>set1.get(<i>i</i>)==set2.get(<i>i</i>)</code>.
 */
_BITSET_INLINE bool BitSet::operator==(const BitSet& other) const {
  index_t i;
  for (i=0;i<storage_size && i<other.storage_size;i++) {
    if (storage[i]!=other.storage[i]) {
      return false;
    }
  }

  for (index_t j=i;j<storage_size;j++) {
    if (storage[j]!=0) {return false;}
  }

  for (index_t j=i;j<other.storage_size;j++) {
    if (other.storage[j]!=0) {return false;}
  }

  return true;
}

/**
 * Less-than operator.
 * Provides an (arbitrary) partial order on the BitSets.
 */
_BITSET_INLINE bool BitSet::operator<(const BitSet& other) const {
  index_t i;
  if (storage_size > other.storage_size) {
    for (i = other.storage_size;
	 i < storage_size;
	 i++) {
      if (storage[i]!=0) {
	return false;
      }
    }
  } else if (storage_size < other.storage_size) {
    for (i = storage_size;
	 i < other.storage_size;
	 i++) {
      if (other.storage[i]!=0) {
	return true;
      }
    }
  }

  for (i=std::min(storage_size, other.storage_size);
       i>0;
       --i) {
    if (storage[i-1] > other.storage[i-1]) {
      return false;
    } else if (storage[i-1] < other.storage[i-1]) {
      return true;
    }
  }

  // we are here if both bitsets are equal
  return false;
}


/**
 * Performs logical And operation with another BitSet.
 * @param other the other BitSet.
 */
_BITSET_INLINE void BitSet::And(const BitSet& other) {
  for (index_t i=0;i<storage_size;i++) {
    if (i>=other.storage_size) {
      storage[i]=0;
    } else {
      storage[i]&=other.storage[i];
    }
  }
}


/**
 * Performs intersection test with other BitSet.
 * @param other the other BitSet.
 * @return <b>true</b> if there is an index <i>i</i> such that bit 
 *         <i>i</i> is set in both BitSets.
 */
_BITSET_INLINE bool BitSet::intersects(const BitSet& other) const {
  for (index_t i=0;
       i<storage_size && i<other.storage_size;
       i++) {
    if ((storage[i] & other.storage[i])!=0) {
      return true;
    }
  }
  return false;
}

/**
 * Performs logical Or operation with another BitSet.
 * @param other the other BitSet.
 */
_BITSET_INLINE void BitSet::Or(const BitSet& other) {
  grow(other.storage_size*bits_per_storage-1);
  for (index_t i=0;i<other.storage_size;i++) {
    storage[i]|=other.storage[i];
  }
}


/**
 * Performs logical And Not operation with another BitSet.
 * @param other the other BitSet.
 */
_BITSET_INLINE void BitSet::AndNot(const BitSet& other) {
  for (index_t i=0;
       i<storage_size && i<other.storage_size;
       i++) {
    storage[i]=storage[i] & ~other.storage[i];
  }
}


/**
 * Clears the BitSet. All bits are set to <code>false</code>.
 */
_BITSET_INLINE void BitSet::clear() {
  delete[] storage;
  storage=new storage_t[1];
  storage[0]=0;
  storage_size=1;
}


/**
 * Set the bit at the specified index to <code>false</code>.
 * @param bitIndex the index
 */
_BITSET_INLINE void BitSet::clear(index_t bitIndex) {
  set(bitIndex, false);
}

/**
 * Set the bits between the specified indexes (inclusive) to
 * <code>false</code>.
 * @param fromIndex the lower index
 * @param toIndex the upper index
 */
_BITSET_INLINE void BitSet::clear(index_t fromIndex, index_t toIndex) {
  checkIndex(fromIndex);
  checkIndex(toIndex);

  if (toIndex<fromIndex) {
    THROW_EXCEPTION(IllegalArgumentException, "toIndex < fromIndex");
  }

  for (index_t i=fromIndex;i<=toIndex;i++) {
    clear(i);
  }
}


/**
 * Calculate the cardinality of the BitSet.
 * @return the number of set bits in the BitSet.
 */
_BITSET_INLINE unsigned int BitSet::cardinality() const {
  unsigned int c=0;
  for (index_t i=0;i<storage_size*bits_per_storage;i++) {
    if (get(i)) {
      c++;
    }
  }
  return c;
}

/**
 * Flip bit at index.
 * @param bitIndex the index.
 */
_BITSET_INLINE void BitSet::flip(index_t bitIndex) {
  checkIndex(bitIndex);

  set(bitIndex, !get(bitIndex));
}


/**
 * Flip the bits between the specified indexes (inclusive).
 * @param fromIndex the lower index
 * @param toIndex the upper index
 */
_BITSET_INLINE void BitSet::flip(index_t fromIndex, index_t toIndex) {
  checkIndex(fromIndex);
  checkIndex(toIndex);

  if (fromIndex > toIndex) {
    THROW_EXCEPTION(IllegalArgumentException, "toIndex < fromIndex");
  }

  for (index_t i=fromIndex;i<=toIndex;i++) {
    flip(i);
  }
}


/**
 * Is the bit at bitIndex set?
 * @param bitIndex the index
 * @return <b>true</b> if the bit at bitIndex is set.
 */
_BITSET_INLINE bool BitSet::get(index_t bitIndex) const {
  checkIndex(bitIndex);

  if (bitIndex >= bits_per_storage*storage_size) {
    // Requested bit is outside of storage, defaults to false
    return false;
  }

  index_t storage_index=bitIndex / bits_per_storage;
  index_t index_inside_storage=bitIndex % bits_per_storage;

  return (storage[storage_index] & (1 << index_inside_storage)) != 0;
}


/**
 * Checks if the BitSet is empty.
 * @return <b>true</b> if the cardinality of the BitSet is 0
 */
_BITSET_INLINE bool BitSet::isEmpty() const {
  for (index_t i=0;i<storage_size;i++) {
    if (storage[i]!=0) {
      return false;
    }
  }
  return true;
}

/**
 * Get the length (the highest-set bit) of the BitSet.
 * @return the length
 */
_BITSET_INLINE BitSet::index_t 
BitSet::length() const {
  for (unsigned int i=storage_size; i>0; i--) {
    if (storage[i-1]!=0) {
      for (unsigned int j=bits_per_storage; j>0; j--) {
	if (get((i-1)*bits_per_storage+(j-1))) {
	  return (i-1)*bits_per_storage+j;
	}
      }
      // Implementation Failure!
      THROW_EXCEPTION(Exception, "Implementation failure!");
    }
  }
  return 0;
}

/**
 * Return the index of the next clear bit at an index >= fromIndex.
 * @return the index of the next clear bit
 */
_BITSET_INLINE BitSet::index_t 
BitSet::nextClearBit(index_t fromIndex) const {
  if (fromIndex < 0) {
    return -1;
  }

  if (fromIndex >= bits_per_storage*storage_size) {
    return fromIndex;
  }
  
#ifdef BITSET_CONSERVATIVE
  index_t i=fromIndex;
  while (get(i)) {
    ++i;
  }
  return i;
#else
  index_t storage_index=fromIndex / bits_per_storage;
  index_t index_inside_storage=fromIndex % bits_per_storage;
  storage_t tmp=storage[storage_index] >> index_inside_storage;
  while (storage_index<storage_size) {
    if (tmp==0) {
      return storage_index*bits_per_storage+index_inside_storage;
    }
    tmp=~tmp;
    index_t i=getLowestSetBit(tmp);
    if (i!=-1) {
      return storage_index*bits_per_storage+index_inside_storage+i;
    } else {
      index_inside_storage=0;
      ++storage_index;
      if (storage_index>=storage_size) {
	break;
      }
      tmp=storage[storage_index];
    }
  }

  // the bits outside of storage are always 0
  return storage_size*bits_per_storage;
  
#endif
}


/**
 * Return the index of the next set bit at an index >= fromIndex.
 * @return the index of the next set bit (or -1 if there is no set bit).
 */
_BITSET_INLINE BitSet::index_t 
BitSet::nextSetBit(index_t fromIndex) const {
  if (fromIndex < 0) {
    return -1;
  }

  if (fromIndex >= bits_per_storage*storage_size) {
    return -1;
  }

  #ifdef BITSET_CONSERVATIVE
  for (index_t i=fromIndex;i<bits_per_storage*storage_size;i++) {
    if (get(i)) {
      return i;
    }
  }
  return -1;
  #else
  index_t storage_index=fromIndex / bits_per_storage;
  index_t index_inside_storage=fromIndex % bits_per_storage;
  storage_t tmp=storage[storage_index] >> index_inside_storage;
  while (storage_index<storage_size) {
    index_t i=getLowestSetBit(tmp);
    if (i!=-1) {
      return storage_index*bits_per_storage+index_inside_storage+i;
    } else {
      index_inside_storage=0;
      ++storage_index;
      if (storage_index>=storage_size) {
	return -1;
      }
      tmp=storage[storage_index];
    }
  }
  return -1;
  #endif

}

/**
 * Set the bit at index bitIndex.
 * @param bitIndex the index.
 */
_BITSET_INLINE void BitSet::set(index_t bitIndex) {
  set(bitIndex, true);
}


/**
 * Set the bit at index bitIndex to a certain value
 * @param bitIndex the index.
 * @param value the value
 */
_BITSET_INLINE void BitSet::set(index_t bitIndex, bool value) {
  checkIndex(bitIndex);

  if (bitIndex >= bits_per_storage*storage_size) {
    if (value == false) {
      // Bit not in storage, defaults to false: Nothing to do
      return;
    }

    // We need to grow storage
    grow(bitIndex);
  }

  index_t storage_index=bitIndex / bits_per_storage;
  index_t index_inside_storage=bitIndex % bits_per_storage;
  
  if (value) {
    storage[storage_index]|=(1L << index_inside_storage);
  } else {
    storage[storage_index]&=~(1L << index_inside_storage);
  }
}


/**
 * Grow the underlying representation of the BitSet to
 * accommodate at least maxIndex bits.
 */
_BITSET_INLINE void BitSet::grow(index_t maxIndex) {
  // Check to see, if we have enough storage...
  if (maxIndex >= bits_per_storage*storage_size) {
    // We need to grow it...
    unsigned int new_size=maxIndex/bits_per_storage+1;
    storage_t *new_storage=new storage_t[new_size];
    for (index_t i=0;i<storage_size;i++) {
      new_storage[i]=storage[i];
    }

    for (unsigned int j=storage_size;j<new_size;j++) {
      new_storage[j]=0;
    }

    delete[] storage;
    storage=new_storage;
    storage_size=new_size;
  }
}

/**
 * Check index for validity.
 */
_BITSET_INLINE void BitSet::checkIndex(index_t index) const {
  assert(index>=0);
}

/**
 * Get the highest set bit in a single storage unit.
 * @param s the storage unit
 * @return the index of the highest set bit (or -1 if there are none).
 */
_BITSET_INLINE BitSet::index_t 
BitSet::getHighestSetBit(storage_t s) const {
  if (s==0) {return -1;}
  
  unsigned int r_i=0, size=bits_per_storage/2;
  storage_t tmp;
  while (size) {
    tmp=s & (((1<<size)-1) << size);
    if (tmp!=0) {
      s=tmp >> size;
      r_i+=size;
    } else {
      s=s & ((1<<size)-1);
    }
    size=size/2;
  }
  return r_i;
}


/**
 * Get the lowest set bit in a single storage unit.
 * @param s the storage unit
 * @return the index of the lowest set bit (or -1 if there are none).
 */
_BITSET_INLINE BitSet::index_t
BitSet::getLowestSetBit(storage_t s) const {
  if (s==0) {return -1;}

#if (__GNUC__ > 3 || (__GNUC__==3 && (__GNUC_MINOR__ >= 4)))
  return __builtin_ctz(s);
#else
  unsigned int r_i=0, size=bits_per_storage/2;
  storage_t tmp;
  while (size) {
    tmp=s & ((1<<size)-1);
    if (tmp!=0) {
      s=tmp;
    } else {
      s=(s >> size) & ((1<<size)-1);
      r_i+=size;
    }
    size=size/2;
  }
  return r_i;
#endif
}


/**
 * Output string representation of the BitSet to an outstream.
 */
_BITSET_INLINE std::ostream& operator<<(std::ostream& out, const BitSet& bs) {
  out << "{";

  bool first=true;
  int i=bs.nextSetBit(0);
  while (i!=-1) {
    if (!first) {
      out << ",";
    } else {
      first=false;
    }
    
    out << i;

    // next set bit
    i=bs.nextSetBit(i+1);
  }

  out << "}";

  return out;
}

/**
 * Generate a string representing this BitSet.
 */
_BITSET_INLINE std::string BitSet::toString() const {
  std::ostringstream buf;
  buf <<*this;

  return buf.str();
}

#endif
