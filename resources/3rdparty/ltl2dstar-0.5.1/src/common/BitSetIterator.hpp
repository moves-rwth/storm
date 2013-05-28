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



#ifndef BITSETITERATOR_H
#define BITSETITERATOR_H

/** @file
 * Provides class BitSetIterator.
 */

#include <boost/iterator/iterator_facade.hpp>
#include "common/BitSet.hpp"

/**
 * An iterator over the indexes of the set bits in a BitSet (of type BitSet::index_t).
 */
class BitSetIterator
  : public boost::iterator_facade< BitSetIterator, 
				   BitSet::index_t const, 
				   boost::forward_traversal_tag,
				   BitSet::index_t const
				   >
{
public:
  /** Constructor, generate iterator pointing to first set bit. */
  BitSetIterator(const BitSet& bitset)
    : _bitset(bitset), _i(bitset.nextSetBit(0))
  {}
  
  /** Constructor, generate iterator pointing a certain bit. */
  explicit BitSetIterator(const BitSet& bitset,
			  BitSet::index_t i)
    : _bitset(bitset), _i(i)
  {}

  /** Constructor, pointing after the last set bit. */
  static BitSetIterator end(const BitSet& bitset) {
    return BitSetIterator(bitset, -1);
  }
  
private:
  friend class boost::iterator_core_access;
  
  /** Increment iterator */
  void increment() {
    _i=_bitset.nextSetBit(_i+1);
  }
  
  /** Test iterator for equality */
  bool equal(BitSetIterator const& other) const
  {
    return this->_i == other._i;
  }
  
  /** Dereference */
  const BitSet::index_t dereference() const {
    return _i;
  }
  
  /** The underlying bitset. */
  const BitSet& _bitset;
  /** The current index. */
  BitSet::index_t _i;
};

#endif
