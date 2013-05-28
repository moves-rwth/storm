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


#ifndef INDEX_H
#define INDEX_H

/** @file 
 * Providing an Index, a vector where contained objects know their index,
 * which allows efficient acces.
 */

#include "common/Exceptions.hpp"
  //include "common/Indexable.h"

#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>

#include "common/BitSet.hpp"
#include "common/BitSetIterator.hpp"

#include <vector>
#include <utility>
#include <functional>

//class Indexable;

/**
 * An Index, a vector where the contained objects (which should define the
 * interface Indexable) know their index, which allows efficient acces.
 * <p>
 * If an object is removed from the index, the space is not reclaimed
 * automatically and the Index doesn't change size. Use compact()
 * to compact the Index. This will invalidate iterators and can change
 * the indizes of the contained objects.
 */
template <typename Indexable>
class Index {
public:
  Index();
  ~Index();
  
  unsigned int add(Indexable *obj);
  void remove(unsigned int index);
  unsigned int size() const;
  Indexable *get(unsigned int index);
  unsigned int get_index(const Indexable *obj) const;

  /** []-operator, get the object at index */
  Indexable *operator[](unsigned int index) {return get(index);}

  /** Helper functor that checks if an object* is not null */
  struct is_not_null{ bool operator()(Indexable *x) { return x!=0; }};
  /** Type of an iterator over the object pointers in the Index that are not null. */
  typedef typename boost::filter_iterator<is_not_null,
					  typename std::vector<Indexable*>::iterator> ptr_iterator;

  /** Type of an iterator over the references of all contained objects. */
  typedef typename boost::indirect_iterator<ptr_iterator> ref_iterator;
  
  /** ptr_iterator that points to the first object pointer */
  ptr_iterator begin_ptr() {return ptr_iterator(storage.begin(), storage.end());}

  /** ptr_iterator that points after the last object pointer */
  ptr_iterator end_ptr() {return ptr_iterator(storage.end(), storage.end());}

  /** ref_iterator that points to the first object */
  ref_iterator begin_ref() {return ref_iterator(begin_ptr());}

  /** ref_iterator that points after the last object */
  ref_iterator end_ref() {return ref_iterator(end_ptr());}

  /**
   * Helper class for the IndexBitsetIterator
   */
  struct index2indexed {
  public:
    typedef unsigned int argument_type;
    typedef Indexable * result_type;

    index2indexed(Index<Indexable> *index) : _index(index) {};
    Indexable *operator()(unsigned int i) const { return _index->get(i); };
  private:
    Index<Indexable> *_index;
  };

  /**
   * Iterator that takes a BitSet and iterates over the 
   * objects on the indizes that are set in the bit set.
   */
  typedef typename boost::transform_iterator<index2indexed, 
					     BitSetIterator> IndexBitSetIterator;

  /** IndexBitSetIterator that points to the first object */
  IndexBitSetIterator begin(BitSet& bs) {
    return IndexBitSetIterator(BitSetIterator(bs), index2indexed(this));
  }
  
  /** IndexBitSetIterator that points after the last object */
  IndexBitSetIterator end(BitSet& bs) {
    return IndexBitSetIterator(BitSetIterator::end(bs), index2indexed(this));
  }
  
  std::pair<bool, std::vector<unsigned int> > compact();

private:

  /** The storage */
  std::vector<Indexable *> storage;
};


/**
 * Constructor.
 */
template <typename Indexable>
Index<Indexable>::Index() {

}


/**
 * Destructor.
 */
template <typename Indexable>
Index<Indexable>::~Index() {
  typedef typename std::vector<Indexable *>::iterator iterator;
  for (iterator it=storage.begin();
       it!=storage.end();
       ++it) {
    if (*it) {
      (*it)->idx_clearIndex(this);
    }
  }
}

/**
 * Add a new object to the index.
 * @return the new index of the object
 */
template <typename Indexable>
unsigned int Index<Indexable>::add(Indexable *obj) {
  if (obj->idx_hasIndex(this)) {
    THROW_EXCEPTION(IllegalArgumentException, "Object already in index!");
  }
  int new_index=storage.size();
  storage.insert(storage.end(), obj);
  obj->idx_setIndex(this, new_index);
  return new_index;
}

/**
 * Removes the object with index from the Index. The resulting free 
 * space is not reclaimed, a subsequent 'get(index)' will return 0
 */
template <typename Indexable>
void Index<Indexable>::remove(unsigned int index) {
  if (index>=size()) {
    THROW_EXCEPTION(IndexOutOfBoundsException, "");
  }

  storage[index]->idx_clearIndex(this);
  storage[index]=0;
}

/**
 * Get size of the index (ie the highest used index-1)
 */
template <typename Indexable>
unsigned int Index<Indexable>::size() const {
  return storage.size();
}

/**
 * Get the object at index.
 * @return a pointer to the object, or NULL if the object was removed
 */
template <typename Indexable>
Indexable *Index<Indexable>::get(unsigned int index) {
  if (index>=size()) {
    THROW_EXCEPTION(IndexOutOfBoundsException, "");
  }

  return storage[index];
}

/** 
 * Get the index of an object contained in the index
 */
template <typename Indexable>
unsigned int Index<Indexable>::get_index(const Indexable *obj) const {
  return obj->idx_getIndex(this);
}

/**
 * Compact the Index.
 */
template <typename Indexable>
std::pair<bool, std::vector<unsigned int> > Index<Indexable>::compact() {
  // Create mapping with enough room for the re-indexing
  std::vector<unsigned int> mapping(size());

  bool moved=false;
  unsigned int j=0;
  for (unsigned int i=0;i<size();i++) {
    if (storage[i]!=0) {
      mapping[i]=j;

      if (i==j) {
	// nothing to do...
      } else {
	moved=true;

	// change index for object
	storage[i]->idx_setIndex(this, j);

	// move object in storage
	storage[j]=storage[i];

	// clean old storage place
	storage[i]=0;
      }

      ++j; // increment
    }
  }

  // remove unneeded elements at the end
  storage.resize(j);

  // return the mapping
  std::pair<bool, std::vector<unsigned int> > result(moved, mapping);

  return result;
}


#endif


