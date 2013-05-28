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


#ifndef SAFRATREEWALKER_H
#define SAFRATREEWALKER_H

/** @file 
 * Provides class SafraTreeWalker  */

#include "SafraTree.hpp"


/** 
 * Walk a SafraTree and invoke the function 
 * <code>void visit(SafraTree& tree, SafraTreeNode *node)</code>
 * of the SafraTreeVisitor on each node of the tree.
 */
template <class SafraTreeVisitor>
class SafraTreeWalker {
public:
  /** Constructor.
   *  @param visitor the visitor functor
   */
  SafraTreeWalker(SafraTreeVisitor& visitor) : _visitor(visitor) {
    ;
  }

  /** Destructor */
  ~SafraTreeWalker() {;}

  /** 
   * Walk the tree post-order and call visit() on each node.
   * @param tree the SafraTree
   */
  void walkTreePostOrder(SafraTree& tree) {
    if (tree.getRootNode()==0) {return;}
    walkSubTreePostOrder(tree, tree.getRootNode());
  }

  /** 
   * Walk the subtree rooted at *top post-order and call visit() on each node.
   * @param tree the SafraTree
   * @param top the current subroot 
   * @param visit_top if true, *top is visited too
   */
  void walkSubTreePostOrder(SafraTree& tree, 
			    SafraTreeNode *top,
			    bool visit_top=true) {
    if (top->getChildCount()>0) {
      SafraTreeNode::child_iterator it=top->children_begin();

      while (it!=top->children_end()) {
	SafraTreeNode *cur_child=*it;
	
	// Increment iterator *before* recursion & visit to account
	// for possible deletion of this child
	++it; 	     
	walkSubTreePostOrder(tree, cur_child, true);
      }
    }

    if (visit_top) {
      _visitor.visit(tree, top);
    }
  }

private:
  /** The SafraTreeVisitor */
  SafraTreeVisitor& _visitor;
};

#endif
