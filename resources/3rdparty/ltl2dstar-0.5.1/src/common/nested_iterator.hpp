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


#ifndef NESTED_ITERATOR_HPP
#define NESTED_ITERATOR_HPP

/** @file
 * Provides a template that allows nesting of two
 * iterators.
 */

#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include "common/Exceptions.hpp"
#include <memory>
#include <iterator>


/**
 * Provides a template that allows nesting of two
 * iterators.
 * The iterator iterates over InnerIterator::type objects.
 * Glue is a class providing two functions, 
 * begin(OuterIterator::type) and end(OuterIterator::type)
 * that return InnerIterators.
 */
template <class OuterIterator,
	  class InnerIterator,
	  class Glue>

class nested_iterator : 
  public boost::iterator_facade <
  nested_iterator<OuterIterator,
		  InnerIterator,
		  Glue>,
  typename boost::iterator_value<InnerIterator>::type,
  boost::forward_traversal_tag,
  typename boost::iterator_reference<InnerIterator>::type
  > {
public:
  /** Construct a nested iterator*/
  nested_iterator(OuterIterator const& outer_begin, 
		  OuterIterator const& outer_end) 
    : _outer(outer_begin), _outer_end(outer_end) {
    Glue glue;
    if (_outer!=_outer_end) {
      _inner.reset(new InnerIterator(glue.begin(*_outer)));
      while (*_inner == glue.end(*_outer) &&
	     (++_outer) != _outer_end) {
	_inner.reset(new InnerIterator(glue.begin(*_outer)));
      }
    }
  }

  /** Copy constructor. */
  nested_iterator(nested_iterator const& other) :
    _outer(other._outer), _outer_end(other._outer_end) {
    _inner.reset(new InnerIterator(*other._inner));
  }
  
private:
  friend class boost::iterator_core_access;

  void increment() {
    Glue glue;
    if ((*_inner)!=glue.end(*_outer)) {
      ++(*_inner);
    }

    while ((*_inner) == glue.end(*_outer) &&
	   (++_outer) != _outer_end) {
      _inner.reset(new InnerIterator(glue.begin(*_outer)));
    }
  }

  bool equal(nested_iterator const& other) const {
    if (! (_outer == other._outer)) {return false;}
    if (_outer==_outer_end) {return true;}
    
    if (_inner.get()!=0 && other._inner.get()!=0) {
      return (*_inner == *other._inner);    
    } else {
      THROW_EXCEPTION(Exception, "Nested-Iterator-Failure!");
    }
  }

  typename boost::iterator_reference<InnerIterator>::type
  dereference() const {
    return **_inner;
  }

  OuterIterator _outer, _outer_end;
  std::auto_ptr<InnerIterator> _inner;
};

#endif
