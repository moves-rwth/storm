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


#ifndef HASH_MAP_H
#define HASH_MAP_H


/** @file
 * This header file provides a compiler independant 
 * interface to the different hash_map implementations.
 *
 * It provides a class myHashMap<Key,Value,HashFunction>.
 * It is assumed that for class Key, the functors 
 * std::equal_to<> and std::less<> exist and are 
 * well-defined.
 */




#ifdef __STL_PORT__
 #include <hash_map>
 namespace HashMap = std;

 #define HASH_MAP(key_t,value_t,hash_function,comparator)	\
     HashMap::hash_map<key_t,value_t,hash_function, typename comparator::equal_to>

#else
 #ifdef __GNUC__
  #if __GNUC__ < 3
   #include <hash_map.h>
   namespace HashMap { using ::hash_map; }; // inherit globals
  #else
   #include <ext/hash_map>
   #if (__GNUC__ ==3 && __GNUC_MINOR__ == 0)
      namespace HashMap = std;               // GCC 3.0
   #else
      namespace HashMap = ::__gnu_cxx;       // GCC 3.1 and later
   #endif
  #endif

 #define HASH_MAP(key_t,value_t,hash_function,comparator)	\
     HashMap::hash_map<key_t,value_t,hash_function, typename comparator::equal_to>

 #else  
  #ifdef _MSC_VER   // ok, Microsoft Visual C++...
   #include <hash_map>
   namespace HashMap = stdext;

// Microsoft Visual C++ uses hash_compare<>, 
// which needs to be specialized
template <class Key, class HashFunction, class Comparator> 
   class msvc_hash_compare : public HashMap::hash_compare<Key> {
   public:
     HashFunction _hash_function;
     typename Comparator::less _comparator;

     size_t operator() (const Key& key) const {
       return _hash_function(key);
     }

     bool operator() (const Key& k1, const Key& k2) const {
       return _comparator(k1,k2);
     }
   };
   #undef HASH_MAP
   #define HASH_MAP(key_t,value_t,hash_function,comparator)	\
     HashMap::hash_map<key_t, \
                       value_t, \
	               msvc_hash_compare<key_t,hash_function,comparator> >

  #else
    // ...  there are other compilers, right?
    #include <ext/hash_map>
    namespace HashMap = std;
  #endif
 #endif
#endif


/** Provides a standard comparator for type Key, using std::equal_to and
 * std::less for comparison.
 */
template <class Key>
struct StdComparator {
  typedef std::equal_to<Key> equal_to;
  typedef std::less<Key> less;
};


/** Provides a comparator for pointers to type Key,
 * using std::equal_to and std::less for comparison on
 * the dereferenced Keys.
 */
template <class KeyPtr>
struct PtrComparator {
  typedef typename KeyPtr::element_type value_type;
  typedef std::shared_ptr<value_type> ptr_type;
  
  /** equal_to functor */
  struct equal_to {
    /** Returns std::equal_to(*a, *b) */
    bool operator()(const ptr_type& a,
		    const ptr_type& b) const {
      std::equal_to<value_type> eq;
      return eq(*a,*b);
    }
  };
  
  /** less functor */
  struct less {
    /** Returns std::less(*a, *b) */
    bool operator()(const ptr_type& a,
		    const ptr_type& b) const {
      std::less<value_type> less;
      return less(*a,*b);
    }
  };

};

/**
 * Template class myHashMap<Key,Value,HashFunction> for a hash map.
 * It is assumed that for class Key, the functors 
 * std::equal_to<> and std::less<> exist and are 
 * well-defined.
 * HashFunction is a functor. To obtain a hash value for 
 * a Key object, operator(Key& key) is called.
 */
template <class Key, class Value, class HashFunction, class Comparator=StdComparator<Key> >
class myHashMap : public HASH_MAP(Key,Value,HashFunction,Comparator) 
{
 public:
  /** The type of the STL hash_map */
  typedef HASH_MAP(Key,Value,HashFunction,Comparator) _base_map;
  
  /** Print statistics */
  void print_stats(std::ostream& out) {
    for (typename _base_map::iterator it=this->begin();
	 it!=this->end();
	 ++it) {
      typename _base_map::value_type& entry=*it;

      size_t hash_value=(this->hash_funct())(entry.first);

      out << hash_value << std::endl;
    }
  }

};
							       

#endif
