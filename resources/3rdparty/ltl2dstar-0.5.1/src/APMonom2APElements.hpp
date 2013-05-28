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


#ifndef APMONOM2APELEMENTS_H
#define APMONOM2APELEMENTS_H

/** @file
 * Provides APMonom2APElement iterators
 */

#include "APMonom.hpp"
#include "APElement.hpp"
#include "common/BitSet.hpp"

#include <boost/iterator/iterator_facade.hpp>

/**
 * Iterator over all APElements that are represented by an APMonom.
 * As every APMonom represents a subset of 2^AP, this allows access
 * to all of the members of this subset.<br>
 * Use the static begin(...) and end(...) to generate the iterators.<br>
 * Uses boost::iterator_facade as base for this iterator.
 */
class APMonom2APElements
  : public boost::iterator_facade< APMonom2APElements, 
				   APElement, 
				   boost::forward_traversal_tag,
				   APElement
				   >
{
public:
  /**
   * Constructor that generates an iterator pointing to the first
   * APElement.
   * @param ap_set the underlying APSet
   * @param m the APMonom over which we iterate
   */
  APMonom2APElements(const APSet& ap_set, APMonom& m) 
    : _ap_set(ap_set), _m(m), _cur_e(m.getValueBits()), _end_marker(false)
  {
    if (m.isFalse()) {
      _end_marker=true;
    }
  }
  
  /**
   * Constructor that generates an iterator pointing
   * to a specific APElement.
   * @param ap_set the underlying APSet
   * @param m the APMonom over which we iterate
   * @param cur_e the current APElement
   */
  explicit APMonom2APElements(const APSet& ap_set, 
			      APMonom& m, 
			      APElement cur_e) 
    : _ap_set(ap_set), _m(m), _cur_e(cur_e), _end_marker(false)
  {
    if (m.isFalse()) {
      _end_marker=true;
    }
  }

  /**
   * Provides an iterator pointing to the first APElement
   * in the subset represented by the APMonom <i>m</i>.
   * @param ap_set the underlying APSet
   * @param m the APMonom over which we iterate
   * @return the iterator.
   */
  static APMonom2APElements begin(const APSet& ap_set, APMonom& m) {
    return APMonom2APElements(ap_set, m);
  }

  /**
   * Provides an iterator pointing after the last APElement
   * in the subset represented by the APMonom <i>m</i>.
   * @param ap_set the underlying APSet
   * @param m the APMonom over which we iterate
   * @return the iterator.
   */
  static APMonom2APElements end(const APSet& ap_set, APMonom& m) {
    APMonom2APElements m2e(ap_set, m);
    m2e._end_marker=true;
    return m2e;
  }
  
private:
  friend class boost::iterator_core_access;

  /**
   * Increment the iterator (used by the boost iterator base class).
   */
  void increment() {
    BitSet set_mask(_m.getSetBits());
    unsigned int i=set_mask.nextClearBit(0);
    while (i<_ap_set.size()) {
      if (_cur_e.get(i)==false) {
	_cur_e.set(i, true);
	return;
      } else {
	_cur_e.set(i, false);
	i=set_mask.nextClearBit(i+1);
      }
    }

    // overflow -> end
    _end_marker=true;
  }

  /**
   * Checks iterators for equality (used by the boost iterator base class).
   */
  
  bool equal(const APMonom2APElements& other) const
  {
    return (this->_end_marker==other._end_marker) &&
      (this->_end_marker || this->_cur_e == other._cur_e);
  }
  
  /**
   * Dereferences the iterator (used by the boost iterator base class).
   */
  APElement dereference() const {
    return _cur_e;
  }


  /** The underlying APSet. */
  const APSet& _ap_set;    
  
  /** The underlying APMonom. */
  const APMonom& _m;       

  /** The current APElement. */
  APElement _cur_e;        

  /** Marker, true if end was reached */
  bool _end_marker;        
};

#endif
