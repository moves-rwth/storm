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


#ifndef STATEMAPPER_HPP
#define STATEMAPPER_HPP

#include "common/hash_map.hpp"
#include "common/HashFunction.hpp"

/** @file
 * Provides StateMapper
 */

/** A mapper from KeyType to StateType, ResultType can be used as an alternative key type. */
template <typename ResultType,
	  typename KeyType, 
	  typename StateType>
class StateMapper {
public:
  /** Constructor. */
  StateMapper() :_count(0) {}
  /** Constructor. */
  ~StateMapper() {}

  /** Clear the mapping */
  void clear() {
    _map.clear();
    _count=0;
  }

  /** Add a mapping. 
   * @param key the key
   * @param state the state
   */
  StateType* add(const KeyType& key, StateType* state) {
    _map[key]=state;
    ++_count;
    return state;
  }


  /** Find a mapping. 
   * @param key the key
   * @return the state (or the NULL pointer if not found)
   */
  StateType* find(const KeyType& key) const {
    typename map_type::const_iterator it;
    it=_map.find(key);
    if (it == _map.end()) {
      return 0;
    } else {
      return (*it).second;
    }
  }

  /** Find a mapping using ResultType. 
   * @param result
   * @return the state (or the NULL pointer if not found)
   */
  StateType* find(const ResultType& result) const {
    return find(result->getState());
  }

  /** Get number of mappings.
   * @return the number of mappings
   */
  unsigned int size() {
    return _count;
  }


private:
  /** A functor using the standard hash function on boost::shared_ptr */
  template <typename K >
  struct std_hash {
    size_t operator()(const K& x) const {
#ifdef BADHASH
      return 1;
#else
      StdHashFunction hash;
      x->hashCode(hash);
      return hash.value();
#endif
    }
  };


  /** The type of a hash map from StateType to MappedStateType */
  typedef myHashMap<KeyType,
		    StateType*,
		    std_hash<KeyType>,
		    PtrComparator<KeyType> > map_type;
  
  /** The hash map from StateType to MappedStateType */
  map_type _map;
  /** The number of mappings */
  unsigned int _count;
};

#endif
