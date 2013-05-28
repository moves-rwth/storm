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



#ifndef SAFRATREE_H
#define SAFRATREE_H

/** @file
 * Provides class SafraTree.
 */

#include "common/BitSet.hpp"
#include "SafraTreeNode.hpp"
#include "RabinAcceptance.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <memory>
#include <functional>

#include <boost/shared_ptr.hpp>

// forward declaration
template <class SafraTreeVisitor> class SafraTreeWalker;

/** 
 * A Safra tree, an ordered tree of SafraTreeNodes.
 */
class SafraTree {
public:
  /** 
   * Constructor.
   * @param N the maximum number of nodes.
   */
  SafraTree(unsigned int N) {
    if (N==0) {N=1;}

    MAX_NODES=N;
    _nodes=new SafraTreeNode*[MAX_NODES];
    for (unsigned int i=0;i<MAX_NODES;i++) {
      _nodes[i]=0;
    }
    
    // create root-node
    newNode(0);
  }

  /** Copy constructor. */
  SafraTree(const SafraTree& other) {
    MAX_NODES=other.MAX_NODES;

    _nodes=new SafraTreeNode*[MAX_NODES];
    for (unsigned int i=0;i<MAX_NODES;i++) {
      _nodes[i]=0;
      if (other._nodes[i]!=0) {
	_nodes[i]=newNode(i);
	_nodes[i]->getLabeling()=other._nodes[i]->getLabeling();
	_nodes[i]->setFinalFlag(other._nodes[i]->hasFinalFlag());
      }
    }
    
    copySubTree(_nodes[0], other._nodes[0]);
  }

  /** Destructor. */
  ~SafraTree() {
    for (unsigned int i=0;i<MAX_NODES;i++) {
      delete _nodes[i];
    }
    delete[] _nodes;
  }
  
  /** Get the root node of the tree. */
  SafraTreeNode* getRootNode() {return _nodes[0];}
  /** Get the root node of the tree. */
  const SafraTreeNode* getRootNode() const {return _nodes[0];}

  /** Create a new node. The name is the next free node name. */
  SafraTreeNode* newNode() {
    for (unsigned int i=0;i<MAX_NODES;i++) {
      if (_nodes[i]==0) {
	return newNode(i);
      }
    }
    return 0;
  }

  /** Create a new node with name <i>id</i>. */
  SafraTreeNode* newNode(unsigned int id) {
    assert(id<MAX_NODES);
    assert(_nodes[id]==0);

    _nodes[id]=new SafraTreeNode(id);

    return _nodes[id];
  }

  /** 
   * Remove a SafraTreeNode from the tree, 
   * the node can have no children.
   */
  void remove(SafraTreeNode *node) {
    assert(_nodes[node->getID()]==node);
    remove(node->getID());
  }

  /** 
   * Remove the SafraTreeNode <i>id</i> from the tree,
   * the node can have no children.
   */
  void remove(unsigned int id) {
    assert(id<MAX_NODES);

    _nodes[id]->removeFromTree();
    delete _nodes[id];
    _nodes[id]=0;
  }


  /**
   * Remove all children of the SafraTreeNode <i>id</i>.
   */
  void removeAllChildren(unsigned int id) {
    assert(id<MAX_NODES);
    
    SafraTreeNode *n=_nodes[id];
    SafraTreeNode *child;
    while ((child=n->getOldestChild())!=0) {
      removeAllChildren(child->getID());
      remove(child->getID());
    }
  }

  /** 
   * Walk the tree post-order, calling the function 
   * void visit(SafraTree& tree, SafraTreeNode *node) 
   * in the SafraTreeVisitor on each node.
   */
  template <class SafraTreeVisitor>
  void walkTreePostOrder(SafraTreeVisitor& visitor) {
    std::auto_ptr<SafraTreeWalker<SafraTreeVisitor> >
      stw(new SafraTreeWalker<SafraTreeVisitor>(visitor));
    stw->walkTreePostOrder(*this);
  }


  /** 
   * Walk the subtree rooted under node *top post-order, 
   * calling the function void visit(SafraTree& tree, SafraTreeNode *node) 
   * in the SafraTreeVisitor on each node.
   */
  template <class SafraTreeVisitor>
  void walkSubTreePostOrder(SafraTreeVisitor& visitor, 
			    SafraTreeNode *top) {
    std::auto_ptr<SafraTreeWalker<SafraTreeVisitor> >
      stw(new SafraTreeWalker<SafraTreeVisitor>(visitor));
    stw->walkSubTreePostOrder(*this, top);
  }

  /** 
   * Walk the subtree rooted under node *top (only the children, not *top itself) 
   * post-order, calling the function void visit(SafraTree& tree, SafraTreeNode *node) 
   * in the SafraTreeVisitor on each node.
   */
  template <class SafraTreeVisitor>
  void walkChildrenPostOrder(SafraTreeVisitor& visitor, 
			     SafraTreeNode *top) {
    std::auto_ptr<SafraTreeWalker<SafraTreeVisitor> >
      stw(new SafraTreeWalker<SafraTreeVisitor>(visitor));

    stw->walkSubTreePostOrder(*this, 
			      top, 
			      false // = don't visit top
			      );
  }


  /**
   * Calculate the height of the tree.
   */
  unsigned int treeHeight() {
    if (getRootNode()!=0) {
      return getRootNode()->treeHeight();
    }

    return 0;
  }


  /**
   * Calculate the width of the tree.
   */
  unsigned int treeWidth() {
    if (getRootNode()!=0) {
      return getRootNode()->treeWidth();
    }

    return 0;
  }

  /**
   * Equality operator.
   */
  bool operator==(const SafraTree& other) const{
    if (other.MAX_NODES!=MAX_NODES) {return false;}

    for (unsigned int i=0;i<MAX_NODES;i++) {
      if (_nodes[i]==0) {
	if (other._nodes[i]!=0) {
	  return false;
	}
      } else {
	if (other._nodes[i]==0) {return false;}

	if (!(*_nodes[i]==*other._nodes[i])) {
	  return false;
	}
      }
    }
    return true;
  }

  /**
   * Checks equality when ignoring the node names.
   */
  bool structural_equal_to(const SafraTree& other) const {
    if (other.MAX_NODES!=MAX_NODES) {return false;}

    const SafraTreeNode* this_root=this->getRootNode();
    const SafraTreeNode* other_root=other.getRootNode();

    if (this_root==0 || other_root==0) {
      // return true if both are 0
      return (this_root==other_root);
    }

    return this_root->structural_equal_to(*other_root);
  }

  /**
   * Less-than operator when ignoring the node names.
   */
  bool structural_less_than(const SafraTree& other) const {
    if (other.MAX_NODES<MAX_NODES) {return true;}

    const SafraTreeNode* this_root=this->getRootNode();
    const SafraTreeNode* other_root=other.getRootNode();

    if (this_root==0) {
      if (other_root!=0) {
	return true;
      } else {
	return false;
      }
    } else { // this_root !=0 
      if (other_root==0) {return false;}

      return this_root->structural_less_than(*other_root);
    }
  }

  /**
   * Less-than operator
   */
  bool operator<(const SafraTree& other) const{
    if (MAX_NODES < other.MAX_NODES) {return true;}

    for (unsigned int i=0;i<MAX_NODES;i++) {
      if (_nodes[i]==0 && other._nodes[i]==0) {
	;
      } else if (_nodes[i]==0) {
	return true;
      } else if (other._nodes[i]==0) {
	return false;
      } else {
	if (*_nodes[i]<*other._nodes[i]) {
	  return true;
	} else if (*_nodes[i] == *other._nodes[i]) {
	  ;
	} else {
	  return false;
	}
      }
    }
    return false;
  }

  /** Get the maximum number of nodes. */
  unsigned int getNodeMax() const {return MAX_NODES;}

  /** Get SafraTreeNode with index <i>i</i>*/
  SafraTreeNode* operator[](unsigned int i) {
    return _nodes[i];
  }

  const SafraTreeNode* operator[](unsigned int i) const {
    return _nodes[i];
  }

  /** Print the SafraTree on an output stream. */
  friend std::ostream& operator<<(std::ostream& out, 
				  SafraTree& st) {
    if (st.getRootNode()==0) {
      out << "<empty>" << std::endl;
    } else {
      st.printSubTree(out, 0, st.getRootNode());
    }
    return out;
  }

  /** Returns a string representation of the SafraTree */
  std::string toString() {
    std::ostringstream buf;
    buf << *this;
    return buf.str();
  }

  /** Returns a string representation in HTML of the SafraTree */
  std::string toHTML() {
    if (getRootNode()==0) {
      return std::string("<TABLE><TR><TD>[empty]</TD></TR></TABLE>");
    } else {
      std::ostringstream buf;
      getRootNode()->toHTML(buf);
      return buf.str();
    }
  }
  
  /**
   * Calculate a hash value using HashFunction
   * @param hashfunction the HashFunction
   * @param only_structure ignore the nameing of the nodes
   */
  template <class HashFunction>
  void hashCode(HashFunction& hashfunction,
		bool only_structure=false) {
    SafraTreeNode* root=getRootNode();

    if (root!=0) {
      root->hashCode(hashfunction, only_structure);
    }
  }

  /**
   * Generate the appropriate acceptance signature for Rabin Acceptance for this tree  
   */
  void generateAcceptance(RabinAcceptance::AcceptanceForState acceptance) const {
    for (unsigned int i=0;i<getNodeMax();i++) {
      const SafraTreeNode *stn=(*this)[i];
      if (stn==0) {
	acceptance.addTo_U(i);
      } else {
	if (stn->hasFinalFlag()) {
	  acceptance.addTo_L(i);
	}
      }
    }
  }

  void generateAcceptance(RabinAcceptance::RabinSignature& acceptance) const {
    acceptance.setSize(getNodeMax());
    for (unsigned int i=0;i<getNodeMax();i++) {
      const SafraTreeNode *stn=(*this)[i];
      if (stn==0) {
	acceptance.setColor(i, RabinAcceptance::RABIN_RED);
      } else {
	if (stn->hasFinalFlag()) {
	  acceptance.setColor(i, RabinAcceptance::RABIN_GREEN);
	} else {
	  acceptance.setColor(i, RabinAcceptance::RABIN_WHITE);
	}
      }
    }
  }

  RabinAcceptance::RabinSignature generateAcceptance() const {
    RabinAcceptance::RabinSignature s(getNodeMax());
    generateAcceptance(s);
    return s;
  }

private:
  /** The maximum number of nodex */
  unsigned int MAX_NODES;
  /** An array to store the nodes */
  SafraTreeNode **_nodes;

  /**
   * Copy the subtree (the children) of *other
   * to *top, becoming the children of *top
   */
  void copySubTree(SafraTreeNode *top, SafraTreeNode *other) {
    if (other==0) {return;}

    for (SafraTreeNode::child_iterator it=other->children_begin();
	 it!=other->children_end();
	 ++it) {
      SafraTreeNode *n=_nodes[(*it)->getID()], *n_o=*it;
      top->addAsYoungestChild(n);
      copySubTree(n, n_o);
    }
  }

  /**
   * Print the subtree rooted at node *top to the output stream
   * @param out the output stream
   * @param prefix the number of spaces ' ' in front of each node
   * @param top the current tree sub root
   */
  void printSubTree(std::ostream& out,
		    unsigned int prefix, 
		    SafraTreeNode* top) {
    for (unsigned int i=0;i<prefix;i++) {
      out << " ";
    }
    out << *top << std::endl;

    for (SafraTreeNode::child_iterator it=top->children_begin();
	 it!=top->children_end();
	 ++it) {
      printSubTree(out, prefix+1, *it);
    }
  }
};

/** a boost::shared_ptr for SafraTree */
typedef boost::shared_ptr<SafraTree> SafraTree_ptr;

namespace std {

/** overload equal_to for SafraTree_ptr to compare the actual trees */
template <> 
inline bool 
std::equal_to<SafraTree_ptr>::operator()
  (SafraTree_ptr const& x, SafraTree_ptr const& y) const {
  return (*x)==(*y);
}

/** overload less for SafraTree_ptr to compare the actual trees */
template <>
inline bool
std::less<SafraTree_ptr>::operator()
  (SafraTree_ptr const& x, SafraTree_ptr const& y) const {
  return (*x)<(*y);
}

};


// include SafraTreeWalker after declaration, because it 
// depends on SafraTree... 
#include "SafraTreeWalker.hpp"


#endif
