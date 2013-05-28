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



#ifndef STATEMAPPERFUZZY_H
#define STATEMAPPERFUZZY_H

/** @file
 * Provide class StateMapperFuzzy
 */

#include "common/hash_map.hpp"
#include "common/HashFunction.hpp"

#include <cassert>

/**
 * A mapping from KeyType to StateType, with ResultType as an alternative key type, which can be fuzzily matched
 * using CandidateMatcher.
 */
template <typename ResultType,
	  typename KeyType, 
	  typename StateType,
 	  typename CandidateMatcher>
class StateMapperFuzzy {
public:
  /** Constructor. */
  StateMapperFuzzy() :_count(0) {}
  
  /** Destructor. */
  ~StateMapperFuzzy() {
    clear();
  }

  /** Clear the mapping. */
  void clear() {
    // delete all Lists...
    for (typename map_type::iterator it=
	   _map.begin();
	 it!=_map.end();
	 ++it) {
      ValueList* list=(*it).second;
      if (list!=0) {
	list->destroyList();
	delete list;
      }
    }
    _map.clear();
    _count=0;
  }

  /** 
   * Search for a mapping, fuzzily
   * @param result the query
   * @return the corresponding state or NULL otherwise
   */
  StateType* find(const ResultType& result) {
    typename map_type::const_iterator it;

    AbstractedKeyType search_key(result->getState());
    
    it=_map.find(search_key);

    if (it!=_map.end()) {
      ValueList* list=(*it).second;

      unsigned int count=0;
      while (list!=0) {
	// check to see if we are compatible

	if (CandidateMatcher::isMatch(result, list->_key)) {
	  //std::cerr << "Found: "<< count << std::endl;
	  return list->_state;
	}

	//	std::cerr << "Tree: "<< *list->_tree;

	list=list->_next;
	count++;
      }
      //      std::cerr << "Not found: "<< count << std::endl;
    }
    
    // not found
    return 0;
  }


  /** 
   * Add a mapping
   * @param key the key
   * @param state the state
   */
  void add(const KeyType& key, StateType *state) {
    AbstractedKeyType akey(key);
    ValueList *list=new ValueList;
    
    list->_key=key;
    list->_state=state;
    list->_next=0;

    typename map_type::value_type  value(akey, list);

    std::pair<typename map_type::iterator, bool> result=
      _map.insert(value);

    if (result.second!=true) {
      // there is already an element with this structure
      // -> insert list into current list
      
      ValueList* head=result.first->second;
      list->_next=head->_next;
      head->_next=list;
    }

    _count++;
  }

  /** Get the number of trees */
  unsigned int size() {return _count;}

  /** Print statistics */
  void print_stats(std::ostream& out) {
    _map.print_stats(out);
  }

private:
  /** 
   * A structure that abstracts the Keytype to its abstracted properties
   */
  class AbstractedKeyType {
  public:
    AbstractedKeyType(KeyType key) : _key(key) {}
    
    bool operator==(const AbstractedKeyType& other) const {
      return CandidateMatcher::abstract_equal_to(_key, other._key);
    }
    
    bool operator<(const AbstractedKeyType& other) const {
      return CandidateMatcher::abstract_less_than(_key, other._key);
    }

    size_t hashCode() const {
      StdHashFunction hash;
      CandidateMatcher::abstract_hash_code(hash, _key);
      return hash.value();
    }

  private:
    KeyType _key;    
  };

  /** A functor to hash AbstracedKeyType */
  struct abstract_hash {
    size_t operator()(const AbstractedKeyType& x) const {
      return x.hashCode();
    }
  };
  
  /** A linked list of KeyTypes that are structurally equal */
  struct ValueList {
    ValueList() : _state(0), _next(0) {}

    void destroyList() {
      ValueList* list=_next;
      while (list!=0) {
	ValueList* cur=list;
	list=list->_next;
	delete cur;
      }
    }
    
    KeyType _key;
    StateType* _state;
    ValueList* _next;
  };

  /** The type of a hash map from StateType to MappedStateType */
  typedef myHashMap<AbstractedKeyType,
		    ValueList*,
		    abstract_hash > map_type;
  
  /** The hash map from StateType to MappedStateType */
  map_type _map;
  unsigned int _count;
};


#endif
