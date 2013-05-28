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


#ifndef SAFRATREETEMPLATE_H
#define SAFRATREETEMPLATE_H

/** @file
 * Provides the class SafraTreeTemplate
 */

#include "SafraTree.hpp"
#include "common/BitSet.hpp"

#include <boost/shared_ptr.hpp>

/**
 * A SafraTreeTemplate consists of a SafraTree and two BitSets,
 * one containing the names of nodes that may be renamed and on
 * containing the names that are not allowed in the tree.
 */
class SafraTreeTemplate {
public:
  /**
   * Constructor.
   * @param safraTree the SafraTree
   */
  SafraTreeTemplate(SafraTree_ptr& safraTree) :
    _safraTree(safraTree) {}

  /** Get the SafraTree */
  SafraTree_ptr& getSafraTree() {return _safraTree;}

  /** Get the SafraTree */
  SafraTree_ptr& getState() {return _safraTree;}

  /** Get the names of nodes that may be renamed. */
  BitSet& renameableNames() {return _renameableNames;}

  /** Get the names that can are not allowed to be used in the Safra tree */
  BitSet& restrictedNames() {return _restrictedNames;}

  /** Set the 'renameable' flag for a name */
  void setRenameable(unsigned int name, bool flag=true) { _renameableNames.set(name, flag); }

  /** Get the 'renameable' flag for a name */
  bool isRenameable(unsigned int name) { return _renameableNames.get(name); }

  /** Set the 'restricted' flag for a name */
  void setRestricted(unsigned int name, bool flag=true) { _restrictedNames.set(name, flag); }

  /** Get the 'restricted' flag for a name */
  bool isRestricted(unsigned int name) { return _restrictedNames.get(name); }


  /**
   * Return true if this tree (taking into account the renameableNames and the restrictedNames) 
   * can be renamed to match the SafraTree other.
   * Can only be called for trees that are structural_equal!!!
   */
  bool matches(const SafraTree& other) {
    const SafraTreeNode* this_root=_safraTree->getRootNode();
    const SafraTreeNode* other_root=other.getRootNode();

    if (this_root==0 || other_root==0) {
      assert(this_root==0 && other_root==0);
      return true;
    }

    return matches(this_root, other_root);
  }

private:
  /**
   * Compare two subtrees to see if they match (taking into account the renameableNames
   * and the restrictedNames).
   */
  bool matches(const SafraTreeNode* this_node, const SafraTreeNode* other_node) {
    assert(this_node!=0 && other_node!=0);

    if (this_node==0 || other_node==0) {
      return false;
    }

    if (!renameableNames().get(this_node->getID())) {
      // this is not a new node, so we require a perfect match..
      if (other_node->getID()!=this_node->getID()) {
	return false;
      }
    } else {
      // we are flexible with the id, as long as the id wasn't removed
      //  in the tree
      if (restrictedNames().get(other_node->getID())) {
	return false;
      }
    }

    assert(this_node->getLabeling()==other_node->getLabeling());
    assert(this_node->hasFinalFlag()==other_node->hasFinalFlag());

    // this node looks good, now the children
    const SafraTreeNode *this_child=this_node->getOldestChild();
    const SafraTreeNode *other_child=other_node->getOldestChild();
    
    while (this_child!=0 && other_child!=0) {
      if (!matches(this_child, other_child)) {
	return false;
      }

      this_child=this_child->getYoungerBrother();
      other_child=other_child->getYoungerBrother();
    }
    assert(this_child==0 && other_child==0);

    return true;
  }

  SafraTree_ptr _safraTree;
  BitSet _renameableNames;
  BitSet _restrictedNames;
};

/** A boost::shared_ptr for a SafraTreeTemplate */
typedef boost::shared_ptr<SafraTreeTemplate> SafraTreeTemplate_ptr;

#endif
