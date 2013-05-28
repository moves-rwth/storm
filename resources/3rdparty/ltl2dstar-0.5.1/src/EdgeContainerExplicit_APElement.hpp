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


#ifndef EDGECONTAINEREXPLICITAPELEMENT_H
#define EDGECONTAINEREXPLICITAPELEMENT_H

/** @file
 * Provides an implementation of an EdgeContainer with APElements as labels,
 * which stores the edges in a vector, which is complete, ie there are 
 * always 2^AP edges. 
 */

#include "common/Exceptions.hpp"
#include "APSet.hpp"
#include "APElement.hpp"


#include <boost/iterator/iterator_facade.hpp>
#include <utility>

/**
 * An EdgeContainer, which stores
 * the edges (labeled by APElements) in a vector, which is complete, ie there are 
 * always 2^AP edges.
 * The template parameter To signifies the type of target for the edges,
 * it can e.g. be an int to store a state index or a BitSet to 
 * store multiple targets for a single edge.
 */
template <typename To>
class EdgeContainerExplicit_APElement {
 public:
  /** The type of this container class */
  typedef EdgeContainerExplicit_APElement<To> container_type;
  /** The type of the edges */
  typedef std::pair<APElement, To* > edge_type;

  /** 
   * Constructor.
   * @param sizeAP the number of atomic propositions in the APSet
   */
  EdgeContainerExplicit_APElement(unsigned int sizeAP=1) : _sizeAP(sizeAP) {
    _arraySize=1<<sizeAP;
    _storage=new To*[_arraySize];
    for (unsigned int i=0;i<_arraySize;i++) {
      _storage[i]=0;
    }
  };

  /** Destructor */
  ~EdgeContainerExplicit_APElement() {delete[] _storage;};

  /** Add an edge that doesn't already exist */
  void addEdge(APElement label, To &to) {addEdge(label, &to);}

  /** Add an edge that doesn't already exist */
  void addEdge(APElement label, To *to) {
    if (get(label)) {
      THROW_EXCEPTION(IllegalArgumentException, "Trying to add edge which already exists!");
    }

    set(label, to);
  }

  /** Remove an edge */
  void removeEdge(APElement label) {
    if (!get(label)) {
      THROW_EXCEPTION(IllegalArgumentException, "Trying to remove non-existing edge!");
    }

    set(label, 0);
  }

  /** Get the target of the edge labeled with label*/
  To* get(APElement label) {
    if (label.getBitSet()>=_arraySize) {
      THROW_EXCEPTION(IndexOutOfBoundsException, "");
    }
    
    return _storage[label.getBitSet()];
  };

  /** Set the target of an edge labeled with <i>label</i> to <i>to</i> */
  void set(APElement label, To& to) {
    set(label, &to);
  }
  
  /** Set the target of an edge labeled with <i>label</i> to <i>to</i> */
  void set(APElement label, To *to) {
    if (label.getBitSet()>=_arraySize) {
      THROW_EXCEPTION(IndexOutOfBoundsException, "");
    }
    
    _storage[label.getBitSet()]=to;
  };

  /** An iterator over all the edges. */
  class EdgeIterator 
    : public boost::iterator_facade< EdgeIterator,
				     edge_type,
				     boost::forward_traversal_tag,
				     edge_type>
  {
  public:
    EdgeIterator(container_type& container) 
      : _container(container), _i(0)
    {
      if (!_container.get(_i)) {
	// edge with index 0 doesn't exist, increment until we find the first edge
	increment();
      }
    };  
    
    explicit EdgeIterator(container_type& container,
			  unsigned int i) 
      : _container(container), _i(i)
    {
    };

  private:
    friend class boost::iterator_core_access;
    
    void increment() {
      while (++_i < _container._arraySize &&
	     _container.get(_i) == 0) 
	{ 
	  ; //nop
	}
    }

    bool equal(EdgeIterator const& other) const {
      return 
	(&this->_container == &other._container) 
	&& (this->_i == other._i);
    }

    edge_type dereference() const {
      To* to=_container.get(_i);
      return edge_type(APElement(_i), to);
    }

    container_type& _container;
    unsigned int _i;
  };

  /** 
   * Returns an iterator pointing to the first edge. 
   * This iterator iterates over all the 2^AP edges,
   * irrespective that some may not have a target
   * (dereferencing will return NULL for these).
   */
  EdgeIterator begin() {return EdgeIterator(*this);}
  /** Returns an iterator pointing after the last edge. */
  EdgeIterator end() {return EdgeIterator(*this,_arraySize);}

  /** Type of the iterator over all the edges */
  typedef EdgeIterator iterator;

 private:
  /** The size of the APSet */
  unsigned int _sizeAP;
  /** The number of edges */
  unsigned long _arraySize;
  /** The storage area for the edges */
  To** _storage;
};

#endif






