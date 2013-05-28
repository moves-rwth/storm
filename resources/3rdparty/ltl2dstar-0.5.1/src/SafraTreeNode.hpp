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


#ifndef SAFRATREENODE_H
#define SAFRATREENODE_H

/** @file
 * Provides class SafraTreeNode.
 */

#include <iostream>
#include <cassert>
#include <boost/iterator/iterator_facade.hpp>
#include "common/BitSet.hpp"

/**
 * A node in a SafraTree.
 * Each node has: <br>
 * - a name/id (integer)<br>
 * - a final flag (bool)<br>
 * - a parent (NULL for root node)<br>
 * - an older brother (NULL if node is oldest)<br>
 * - a younger brother (NULL if node is youngest)<br>
 * - an oldestChild (NULL if node has no children<br>
 * - a youngestChild (NULL if node has no children<br>
 * - a label (BitSet) for the powerset 
 */
class SafraTreeNode {
public:
  /** 
   * Constructor 
   * @param id the name of the node
   */
  SafraTreeNode(unsigned int id) : _id(id),
				   _final_flag(false),
				   _parent(0),
				   _olderBrother(0),
				   _youngerBrother(0),
				   _oldestChild(0),
				   _youngestChild(0),
				   _childCount(0) {
    ;
  }

  /** destructor */
  ~SafraTreeNode() {;}

  /** Get the name of the node*/
  unsigned int getID() const {return _id;}
  
  /** Get the final flag */
  bool hasFinalFlag() const {return _final_flag;}
  
  /** Get the number of children. */
  unsigned int getChildCount() const {return _childCount;}

  /** Get the labeling */
  BitSet& getLabeling() {return _labeling;}

  /** 
   * Get the older brother.
   * @return the older brother, or NULL if node is oldest child.
   */
  SafraTreeNode *getOlderBrother() {return _olderBrother;}

  /** 
   * Get the younger brother.
   * @return the younger brother, or NULL if node is youngest child.
   */
  SafraTreeNode *getYoungerBrother() {return _youngerBrother;}

  /** 
   * Get the oldest child.
   * @return the oldest child, or NULL if node has no children
   */
  SafraTreeNode *getOldestChild() {return _oldestChild;}

  /** 
   * Get the youngest child.
   * @return the youngest child, or NULL if node has no children
   */
  SafraTreeNode *getYoungestChild() {return _youngestChild;}

  /** 
   * Get the parent of the node.
   * @return the parent, or NULL if node has no parent (is root node)
   */
  SafraTreeNode *getParent() {return _parent;}

  // ... and the const versions...

  /** Get the labeling */
  const BitSet& getLabeling() const {return _labeling;}

  /** 
   * Get the older brother.
   * @return the older brother, or NULL if node is oldest child.
   */
  const SafraTreeNode *getOlderBrother() const {
    return _olderBrother;
  }

  /** 
   * Get the younger brother.
   * @return the younger brother, or NULL if node is youngest child.
   */
  const SafraTreeNode *getYoungerBrother() const {
    return _youngerBrother;
  }

  /** 
   * Get the oldest child.
   * @return the oldest child, or NULL if node has no children
   */
  const SafraTreeNode *getOldestChild() const {
    return _oldestChild;
  }

  /** 
   * Get the youngest child.
   * @return the youngest child, or NULL if node has no children
   */
  const SafraTreeNode *getYoungestChild() const {
    return _youngestChild;
  }

  /** 
   * Get the parent of the node.
   * @return the parent, or NULL if node has no parent (is root node)
   */
  const SafraTreeNode *getParent() const {
    return _parent;
  }

  /** 
   * Set the final flag.
   * @param finalFlag the value
   */
  void setFinalFlag(bool finalFlag=true) {_final_flag=finalFlag;}

#define NULL_OR_EQUALID(a,b) ((a==0 && b==0) || ((a!=0 && b!=0) && (a->getID()==b->getID())))

  /** Equality operator. Does not do a deep compare */
  bool operator==(const SafraTreeNode& other) {
    if (!(_id==other._id)) {return false;}
    if (!(_final_flag==other._final_flag)) {return false;}
    if (!(_childCount==other._childCount)) {return false;}
    if (!(_labeling==other._labeling)) {return false;}
    if (!NULL_OR_EQUALID(_parent, other._parent)) {return false;}
    if (!NULL_OR_EQUALID(_olderBrother, other._olderBrother)) {return false;}
    if (!NULL_OR_EQUALID(_youngerBrother, other._youngerBrother)) {return false;}
    if (!NULL_OR_EQUALID(_oldestChild, other._oldestChild)) {return false;}
    if (!NULL_OR_EQUALID(_youngestChild, other._youngestChild)) {return false;}

    return true;
  }

  /** 
   * Equality operator ignoring the name of the nodes, doing a deep compare
   * (checks that all children are also structurally equal.
   */
  bool structural_equal_to(const SafraTreeNode& other) const {
    if (!(_final_flag==other._final_flag)) {return false;}
    if (!(_childCount==other._childCount)) {return false;}
    if (!(_labeling==other._labeling)) {return false;}

    if (_childCount>0) {
      const SafraTreeNode* this_child=getOldestChild();
      const SafraTreeNode* other_child=other.getOldestChild();

      do {
	if (!this_child->structural_equal_to(*other_child)) {
	  return false;
	}

	this_child=this_child->getYoungerBrother();
	other_child=other_child->getYoungerBrother();
      } while (this_child!=0 && other_child!=0);

      assert(this_child==0 && other_child==0);
    }
    return true;
  }

  // LEG = LESS, EQUAL, GREATER
  enum LEG {LESS, EQUAL, GREATER};

#define CMP(a,b) ((a<b) ? LESS : (a==b ? EQUAL : GREATER))
#define ID(a) (a->getID())
#define CMP_ID(a,b) \
    if (a==0) {  \
       if (b!=0) {return true;} \
    } else { \
       if (b==0) {return false;} \
       if (ID(a)!=ID(b)) { \
	 return ID(a)<ID(b); \
       } \
    }

  /** Less-than operator. Does not do deep compare */
  bool operator<(const SafraTreeNode& other) const {
    enum LEG cmp;
    
    cmp=CMP(_id,other._id);
    if (cmp!=EQUAL) {return (cmp == LESS);}
    
    cmp=CMP(_final_flag,other._final_flag);
    if (cmp!=EQUAL) {return (cmp == LESS);}

    cmp=CMP(_childCount,other._childCount);
    if (cmp!=EQUAL) {return (cmp == LESS);}
    
    cmp=CMP(_labeling,other._labeling);
    if (cmp!=EQUAL) {return (cmp == LESS);}
    
    CMP_ID(_parent, other._parent);
    CMP_ID(_olderBrother, other._olderBrother);
    CMP_ID(_youngerBrother, other._youngerBrother);
    CMP_ID(_oldestChild, other._oldestChild);
    CMP_ID(_youngestChild, other._youngestChild);

    return false;
  }

  /** 
   * Less-than operator ignoring the name of the nodes, doing a deep compare
   * (applies recursively on the children).
   */
  bool structural_less_than(const SafraTreeNode& other) const {
    return (this->structural_cmp(other) == LESS);
  }

private:
  /** Do a structural comparison */
  LEG structural_cmp(const SafraTreeNode& other) const {
    LEG cmp;
    
    cmp=CMP(_final_flag,other._final_flag);
    if (cmp!=EQUAL) {return cmp;}

    cmp=CMP(_childCount,other._childCount);
    if (cmp!=EQUAL) {return cmp;}
    
    cmp=CMP(_labeling,other._labeling);
    if (cmp!=EQUAL) {return cmp;}

    // if we are here, this and other have the same number of children
    if (_childCount>0) {
      const SafraTreeNode* this_child=getOldestChild();
      const SafraTreeNode* other_child=other.getOldestChild();

      do {
	cmp=this_child->structural_cmp(*other_child);
	if (cmp!=EQUAL) {return cmp;}

	this_child=this_child->getYoungerBrother();
	other_child=other_child->getYoungerBrother();
      } while (this_child!=0 && other_child!=0);

      // assert that there was really the same number of children
      assert(this_child==0 && other_child==0);
    }

    // when we are here, all children were equal
    return EQUAL;
  }

public:
  /** Add a node as the youngest child */
  void addAsYoungestChild(SafraTreeNode *other) {
    assert(other->getParent()==0);
    assert(other->getOlderBrother()==0);
    assert(other->getYoungerBrother()==0);

    if (_youngestChild!=0) {
      assert(_youngestChild->_youngerBrother==0);
      _youngestChild->_youngerBrother=other;
      other->_olderBrother=_youngestChild;
    }

    other->_parent=this;
    _youngestChild=other;
    if (_oldestChild==0) {
      _oldestChild=other;
    }
    _childCount++;
  }

  /** Add a node as the oldest child */
  void addAsOldestChild(SafraTreeNode *other) {
    assert(other->getParent()==0);
    assert(other->getOlderBrother()==0);
    assert(other->getYoungerBrother()==0);

    if (_oldestChild!=0) {
      assert(_oldestChild->_olderBrother==0);
      _oldestChild->_olderBrother=other;
      other->_youngerBrother=_oldestChild;
    }

    other->_parent=this;
    _oldestChild=other;
    if (_youngestChild==0) {
      _youngestChild=other;
    }
    _childCount++;
  }

  /** Remove this node from the tree (relink siblings). The node is not allowed 
   * to have children! */
  void removeFromTree() {
    assert(_childCount==0);
    
    if (_parent==0) {
      // Root-Node or already removed from tree, nothing to do
      return;
    }

    // Relink siblings
    if (_olderBrother!=0) {
      _olderBrother->_youngerBrother=_youngerBrother;
    }
    if (_youngerBrother!=0) {
      _youngerBrother->_olderBrother=_olderBrother;
    }

    // Relink child-pointers in _parent
    if (_parent->_oldestChild==this) {
      // this node is oldest child
      _parent->_oldestChild=this->_youngerBrother;
    }

    if (_parent->_youngestChild==this) {
      // this node is youngest child
      _parent->_youngestChild=this->_olderBrother;
    }

    --_parent->_childCount;

    _youngerBrother=0;
    _olderBrother=0;
    _parent=0;
  }

  /**
   * Swap the places of two child nodes 
   */
  void swapChildren(SafraTreeNode *a,
		    SafraTreeNode *b) {
    assert(a->getParent()==b->getParent() &&
	   a->getParent()==this);

    if (a==b) {return;}

    if (_oldestChild==a) {
      _oldestChild=b;
    } else if (_oldestChild==b) {
      _oldestChild=a;
    }

    if (_youngestChild==a) {
      _youngestChild=b;
    } else if (_youngestChild==b) {
      _youngestChild=a;
    }

    SafraTreeNode 
      *a_left=a->_olderBrother,
      *b_left=b->_olderBrother,
      *a_right=a->_youngerBrother,
      *b_right=b->_youngerBrother;

    if (a_left!=0) {a_left->_youngerBrother=b;}
    if (b_left!=0) {b_left->_youngerBrother=a;}
    if (a_right!=0) {a_right->_olderBrother=b;}
    if (b_right!=0) {b_right->_olderBrother=a;}

    a->_olderBrother=b_left;
    a->_youngerBrother=b_right;
    b->_olderBrother=a_left;
    b->_youngerBrother=a_right;

    if (a_right==b) {
      // a & b are direct neighbours, a to the left of b
      a->_olderBrother=b;
      b->_youngerBrother=a;
    } else if (b_right==a) {
      // a & b are direct neighbours, b to the left of a
      a->_youngerBrother=b;
      b->_olderBrother=a;
    }
  }

  /** Calculate the height of the subtree rooted at this node. */
  unsigned int treeHeight() {
    unsigned int height=0;
    
    if (getChildCount()>0) {
      SafraTreeNode::child_iterator it=children_begin();

      while (it!=children_end()) {
	SafraTreeNode *cur_child=*it++;

	unsigned int child_height=cur_child->treeHeight();
	if (child_height>height) {
	  height=child_height;
	}
      }
    }

    return height+1;
  }

  /** Calculate the width of the subtree rooted at this node. */
  unsigned int treeWidth() {
    unsigned int width=0;
    
    if (getChildCount()>0) {
      SafraTreeNode::child_iterator it=children_begin();

      while (it!=children_end()) {
	SafraTreeNode *cur_child=*it++;

	width+=cur_child->treeWidth();
      }
    } else {
      width=1;
    }

    return width;
  }

  /** 
   * Calculate a hashvalue using HashFunction for this node.
   * @param hashfunction the HashFunction functor
   * @param only_structure Ignore naming of the nodes?
   */
  template <class HashFunction>
  void hashCode(HashFunction& hashfunction, 
		bool only_structure=false) {
    if (!only_structure) {
      hashfunction.hash(getID());
    }

    getLabeling().hashCode(hashfunction);
    hashfunction.hash(hasFinalFlag());
    
    if (getChildCount()>0) {
      for (child_iterator cit=children_begin();
	   cit!=children_end();
	   ++cit) {
	(*cit)->hashCode(hashfunction, only_structure);
      }
    }
  }

  /** Type of an iterator over the immediate children of a node. */
  class child_iterator 
    : public boost::iterator_facade< child_iterator,
				     SafraTreeNode *,
				     boost::forward_traversal_tag,
				     SafraTreeNode *>
  {
  public:
    child_iterator() 
      : _p(0) {}
    
    explicit child_iterator(SafraTreeNode *p) : _p(p) {}


  private:
    friend class boost::iterator_core_access;
    
    void increment() {
      _p=_p->getYoungerBrother();
    }

    bool equal(child_iterator const& other) const {
      return (this->_p == other._p);
    }

    SafraTreeNode *dereference() const {
      return _p;
    }

    SafraTreeNode *_p;
  };

  /** Returns an iterator over the immediate children of this node, 
   * pointing to the oldest child. */
  child_iterator children_begin() {return child_iterator(getOldestChild());}

  /** Returns an iterator over the immediate children of this node, 
   * pointing after the youngest child. */
  child_iterator children_end() {return child_iterator();}

  /**
   * Print node to output stream
   */
  friend std::ostream& operator<<(std::ostream& out, 
				  const SafraTreeNode& stn) {
    out << stn._id << " " << stn._labeling;
    if (stn._final_flag) {out << " !";}
    return out;
  }

  /** Print HTML version of this node to output stream */
  void toHTML(std::ostream& out) {
    out << "<TABLE><TR>";
    
    if (getChildCount()<=1) {
      out << "<TD>";
    } else {
      out << "<TD COLSPAN=\"";
      out << getChildCount();
      out << "\">";
    }

    out << getID();
    out << " ";
    out << _labeling;
    if (_final_flag) {out << "!";}
    out << "</TD></TR>";
    if (getChildCount()>0) {
      out << "<TR>";
      for (child_iterator it=children_begin();
	   it!=children_end();
	   ++it) {
	out << "<TD>";
	(*it)->toHTML(out);
	out << "</TD>";
      }
      out << "</TR>";
    }
    out << "</TABLE>";
  }
  
private:
  /** The node name */
  unsigned int _id;

  /** The label of the node (powerset) */
  BitSet _labeling;

  /** The final flag */
  bool _final_flag;

  /** The parent node */
  SafraTreeNode *_parent;

  /** The older brother */
  SafraTreeNode *_olderBrother;

  /** The younger brother */
  SafraTreeNode *_youngerBrother;

  /** The oldest child */
  SafraTreeNode *_oldestChild;

  /** The youngest child */
  SafraTreeNode *_youngestChild;

  /** The number of children */
  unsigned int _childCount;
};

#endif
