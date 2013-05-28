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


#ifndef LTLFORMULA_HPP
#define LTLFORMULA_HPP

/** @file
 * Provides classes for the handling of LTL formulas
 */

#include "APSet.hpp"
#include "APMonom.hpp"
#include "common/Exceptions.hpp"

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <sstream>

// forward declaration
class LTLNode;
/** Type of a shared_ptr to an LTLNode */
typedef boost::shared_ptr<LTLNode> LTLNode_p;

/** A node in the syntax tree of an LTL formula. Memory management is
 * automatic using boost::shared_ptr */
class LTLNode {
public:
  enum type_t {T_AP,
	       
	       T_TRUE,
	       T_FALSE,
	       
	       T_NOT,
	       T_AND,
	       T_OR,
	       T_XOR,
	       T_IMPLICATE,
	       T_EQUIV,
	       
	       T_NEXTSTEP,
	       
	       T_GLOBALLY,
	       T_FINALLY,
	       
	       T_UNTIL,
	       T_WEAKUNTIL,
	       
	       T_RELEASE,
	       T_WEAKRELEASE,
	       
	       T_BEFORE};
  
  /** Constructor for a node containing an atomic proposition (index into APSet) */
  LTLNode(unsigned int ap) : 
    _type(T_AP), _left(), _right(), _ap(ap) {}
  
  /** Constructor for an operator of type */
  LTLNode(type_t type,
	  LTLNode_p left,
	  LTLNode_p right=LTLNode_p()) :
    _type(type), _left(left), _right(right) {}
  
  /** Get the type for the node */
  type_t getType() {return _type;}

  /** Get the left child node */
  LTLNode_p getLeft() {return _left;}

  /** Get the right child node */
  LTLNode_p getRight() {return _right;}

  /** Get the index of the AP (when type==T_AP)*/
  unsigned int getAP() {return _ap;}

  /** Deep copy of the node */
  LTLNode_p copy() {
    if (_type==T_AP) {
      return ptr(new LTLNode(_ap));
    } else if (_type==T_TRUE || _type==T_FALSE) {
      return ptr(new LTLNode(_type, ptr(0), ptr(0)));
    } else if (_right.get()==0) {
      return ptr(new LTLNode(_type,
			     _left->copy()));
    } else {
      return ptr(new LTLNode(_type,
			     _left->copy(),
			     _right->copy()));
    }
  }

  /**
   * Generate formula in positive normal form,
   * with only TRUE, FALSE, AND, OR, X, U, V, G, F as operators
   */
  LTLNode_p toPNF(bool negate=false) {
    switch (_type) {
    case T_XOR:
      THROW_EXCEPTION(Exception, "XOR not yet supported!");
    case T_IMPLICATE: {
      if (negate) {
	// ! (a->b) = !(!a | b) = (a & ! b)
	return ptr(new LTLNode(T_AND,
			       getLeft()->toPNF(),
			       getRight()->toPNF(true)));
      } else {
	// (a->b) = (!a | b)
	return ptr(new LTLNode(T_OR,
			       getLeft()->toPNF(true),
			       getRight()->toPNF()));
      }
      break;
    }      
    case T_EQUIV: {
      if (negate) {
	// ! (a<->b) == (a & !b) || (!a & b)
	return ptr(new LTLNode(T_OR,
			       ptr(new LTLNode(T_AND, 
					       getLeft()->toPNF(),
					       getRight()->toPNF(true))),
			       ptr(new LTLNode(T_AND,
					       getLeft()->toPNF(true),
					       getRight()->toPNF()))));
      } else {
	// (a<->b) = (!a && !b) || (a && b)
	return ptr(new LTLNode(T_OR,
			       ptr(new LTLNode(T_AND, 
					       getLeft()->toPNF(true),
					       getRight()->toPNF(true))),
			       ptr(new LTLNode(T_AND,
					       getLeft()->toPNF(),
					       getRight()->toPNF()))));
      }
    }
    case T_NOT:
      if (negate) {
	// double negation
	return getLeft()->toPNF();
      } else {
	if (getLeft()->getType()==T_AP) {
	  // we are in front of an AP, negation is ok
	  return ptr(new LTLNode(T_NOT, getLeft()));
	} else {
	  return getLeft()->toPNF(true);
	}
      }
    case T_AP:
      if (negate) {
	return ptr(new LTLNode(T_NOT, ptr(new LTLNode(_ap))));
      } else {
	return ptr(new LTLNode(_ap));
      }
    case T_TRUE:
      if (negate) {
	return ptr(new LTLNode(T_FALSE, ptr(0)));
      } else {
	return ptr(new LTLNode(T_TRUE, ptr(0)));
      }
    case T_FALSE:
      if (negate) {
	return ptr(new LTLNode(T_TRUE, ptr(0)));
      } else {
	return ptr(new LTLNode(T_FALSE, ptr(0)));
      }
    case T_AND:
	// ! (a & b) = (!a | !b)
      if (negate) {
	return ptr(new LTLNode(T_OR, 
			       getLeft()->toPNF(true),
			       getRight()->toPNF(true)));
      } else {
	return ptr(new LTLNode(T_AND, 
			       getLeft()->toPNF(),
			       getRight()->toPNF()));
      }
    case T_OR:
      if (negate) {
	// ! (a | b) = (!a & !b)
	return ptr(new LTLNode(T_AND, 
			       getLeft()->toPNF(true),
			       getRight()->toPNF(true)));
      } else {
	return ptr(new LTLNode(T_OR, 
			       getLeft()->toPNF(),
			       getRight()->toPNF()));
      }
    case T_NEXTSTEP:
      // ! (X a) = X (!a)
      if (negate) {
	return ptr(new LTLNode(T_NEXTSTEP, 
			       getLeft()->toPNF(true)));
      } else {
	return ptr(new LTLNode(T_NEXTSTEP, 
			       getLeft()->toPNF()));
      }
    case T_FINALLY:
      // ! (F a) = G (!a) = f V !a
      if (negate) {
	return ptr(new LTLNode(T_RELEASE,
			       ptr(new LTLNode(T_FALSE, ptr(0))),
			       getLeft()->toPNF(true)));
      } else { // F a = t U a
	return ptr(new LTLNode(T_UNTIL,
			       ptr(new LTLNode(T_TRUE, ptr(0))),
			       getLeft()->toPNF()));
      }
    case T_GLOBALLY:
      // ! (G a) = F (!a) = t U ! a
      if (negate) {
	return ptr(new LTLNode(T_UNTIL,
			       ptr(new LTLNode(T_TRUE, ptr(0))),
			       getLeft()->toPNF(true)));
      } else { // f V a
	return ptr(new LTLNode(T_RELEASE,
			       ptr(new LTLNode(T_FALSE, ptr(0))),
			       getLeft()->toPNF()));
      }
    case T_UNTIL:
	// ! (a U b) = (!a V !b)
      if (negate) {
	return ptr(new LTLNode(T_RELEASE, 
			       getLeft()->toPNF(true),
			       getRight()->toPNF(true)));
      } else {
	return ptr(new LTLNode(T_UNTIL, 
			       getLeft()->toPNF(),
			       getRight()->toPNF()));
      }
    case T_RELEASE:
      // ! (a R b) = (!a U !b)
      if (negate) {
	return ptr(new LTLNode(T_UNTIL, 
			       getLeft()->toPNF(true),
			       getRight()->toPNF(true)));
      } else {
	return ptr(new LTLNode(T_RELEASE, 
			       getLeft()->toPNF(),
			       getRight()->toPNF()));
      }
    case T_WEAKUNTIL: {
      LTLNode_p weak_until(new LTLNode(T_UNTIL,
				       getLeft(),
				       ptr(new LTLNode(T_OR,
						       getRight(),
						       ptr(new LTLNode(T_GLOBALLY, getLeft(), ptr(0)))))));
      return weak_until->toPNF(negate);
    }
    case T_WEAKRELEASE:
      THROW_EXCEPTION(Exception, "Operator WEAKRELEASE not yet supported");
    case T_BEFORE:
      THROW_EXCEPTION(Exception, "Operator BEFORE not yet supported");
    }

    THROW_EXCEPTION(Exception, "Implementation error");
  }

  /** 
   * Check if the formula rooted in this node is 
   * syntactically safe. Formula has to be in PNF.
   */
  bool isSafe() {
    switch (getType()) {
    case T_UNTIL:
    case T_FINALLY:
      return false;
    case T_WEAKUNTIL:
      THROW_EXCEPTION(Exception, "Operator WEAKUNTIL not yet supported");
    case T_WEAKRELEASE:
      THROW_EXCEPTION(Exception, "Operator WEAKRELEASE not yet supported");
    case T_BEFORE:
      THROW_EXCEPTION(Exception, "Operator BEFORE not yet supported");
    default:
      // this level ok, check next level
      break;
    }

    if (getLeft().get()!=0) {
      if (!getLeft()->isSafe()) {
	return false;
      }
    }

    if (getRight().get()!=0) {
      if (!getRight()->isSafe()) {
	return false;
      }
    }

    return true;
  }


  /** 
   * Check if the formula rooted in this node is 
   * syntactically co-safe. Formula has to be in PNF.
   */
  bool isCoSafe() {
    switch (getType()) {
    case T_RELEASE:
    case T_GLOBALLY:
      return false;
    case T_WEAKUNTIL:
      THROW_EXCEPTION(Exception, "Operator WEAKUNTIL not yet supported");
    case T_WEAKRELEASE:
      THROW_EXCEPTION(Exception, "Operator WEAKRELEASE not yet supported");
    case T_BEFORE:
      THROW_EXCEPTION(Exception, "Operator BEFORE not yet supported");
    default:
      // this level ok, check next level
      break;
    }

    if (getLeft().get()!=0) {
      if (!getLeft()->isCoSafe()) {
	return false;
      }
    }

    if (getRight().get()!=0) {
      if (!getRight()->isCoSafe()) {
	return false;
      }
    }

    return true;
  }


  /**
   * Recursively if the formula rooted at the current node is
   * NextStep-free.
   */
  bool hasNextStep() {
    switch (getType()) {
    case T_NEXTSTEP:
      return true;
    default:
      // this level ok, check next level
      break;
    }

    if (getLeft().get()!=0) {
      if (getLeft()->hasNextStep()) {
	return true;
      }
    }

    if (getRight().get()!=0) {
      if (getRight()->hasNextStep()) {
	return true;
      }
    }

    return false;
  }


  /**
   * Generate formula in DNF. Formula has to have only 
   * AND, OR, TRUE, FALSE, ! as operators, and has to
   * be in PNF
   */
  LTLNode_p toDNF() {
    switch (_type) {
    case T_TRUE:
      return ptr(new LTLNode(T_TRUE, ptr(0)));
    case T_FALSE:
      return ptr(new LTLNode(T_FALSE, ptr(0)));
    case T_NOT:
      return ptr(new LTLNode(T_NOT, getLeft()->toDNF()));
    case T_AP:
      return ptr(new LTLNode(getAP()));
    case T_OR:
      return ptr(new LTLNode(T_OR,
			     getLeft()->toDNF(),
			     getRight()->toDNF()));
    case T_AND: {
      LTLNode_p left=getLeft()->toDNF();
      LTLNode_p right=getRight()->toDNF();

      if (left->getType()==T_OR) {
	LTLNode_p a, b;
	a=left->getLeft();
	b=left->getRight();

	if (right->getType()==T_OR) {
	  LTLNode_p c, d;
	  c=right->getLeft();
	  d=right->getRight();
	  
	  LTLNode_p 
	    a_c(new LTLNode(T_AND, a, c)),
	    b_c(new LTLNode(T_AND, b, c)),
	    a_d(new LTLNode(T_AND, a, d)),
	    b_d(new LTLNode(T_AND, b, d));
	  
	  return ptr(new LTLNode(T_OR,
				 ptr(new LTLNode(T_OR, a_c, b_c))->toDNF(),
				 ptr(new LTLNode(T_OR, a_d, b_d))->toDNF()));	  
	} else {
	  LTLNode_p 
	    a_c(new LTLNode(T_AND, a, right)),
	    b_c(new LTLNode(T_AND, b, right));
	  
	  return ptr(new LTLNode(T_OR, 
				 a_c->toDNF(), 
				 b_c->toDNF()));
	}
      } else if (right->getType()==T_OR) {
	LTLNode_p a, b;
	a=right->getLeft();
	b=right->getRight();
	
	LTLNode_p 
	  a_c(new LTLNode(T_AND, left, a)),
	  b_c(new LTLNode(T_AND, left, b));
	
	return ptr(new LTLNode(T_OR, a_c->toDNF(), b_c->toDNF()));
      } else {
	return ptr(new LTLNode(T_AND, left, right));
      }
    }
    default:
      THROW_EXCEPTION(Exception, "Illegal operator for DNF!");
    }
  }


  /**
   * Calls Functor::operator(APMonom& m) for each monom of the formula. 
   * Formula has to be in DNF!
   */
  template <class Functor>
  void forEachMonom(Functor& f) {
    if (getType()==T_OR) {
      getLeft()->forEachMonom(f);
      getRight()->forEachMonom(f);
    } else {
      APMonom m=this->toMonom();
      f(m);
    }
  }

  /** Returns an APMonom representing the formula rooted at
   * this node. Formula has to be in DNF. */
  APMonom toMonom() {
    APMonom result=APMonom::TRUE;

    switch (getType()) {
    case T_AND: {
      APMonom left=getLeft()->toMonom();
      APMonom right=getRight()->toMonom();
      
      result=left & right;
      return result;
    }
    case T_NOT:
      switch (getLeft()->getType()) {
      case T_AP:
	result.setValue(getLeft()->getAP(), false);
	return result;
      case T_FALSE:
	result=APMonom::TRUE;
	return result;
      case T_TRUE:
	result=APMonom::FALSE;
	return result;	
      default:
	THROW_EXCEPTION(Exception, "Formula not in DNF!");
      }	  
    case T_AP:
      result.setValue(getAP(), true);
      return result;
    case T_FALSE:
      result=APMonom::FALSE;
      return result;
    case T_TRUE:
      result=APMonom::TRUE;
      return result;	
    default:
      THROW_EXCEPTION(Exception, "Formula not in DNF!");
    }
  }

private:
  /** Operator type */
  type_t _type;

  /** The left child */
  LTLNode_p _left;

  /** The right child */
  LTLNode_p _right;

  /** Index of AP (when type==T_AP)*/
  unsigned int _ap;

  /** Helper function to get LTLNode_p from LTLNode* */
  LTLNode_p ptr(LTLNode *p) {
    return LTLNode_p(p);
  }
};


// Forward declaration
class LTLFormula;
/** Type of boost::shared_ptr for LTLFormula */
typedef boost::shared_ptr<LTLFormula> LTLFormula_ptr;

/** An LTLFormula is a tree of LTLNode_p and an underlying APSet*/
class LTLFormula {
public:
  /**
   * Constructor
   * @param root the root node
   * @param apset the underlying APSet
   */
  LTLFormula(LTLNode_p root, APSet_cp apset) :
    _root(root), _apset(apset) {}

  /** Copy constructor (not deep) */
  LTLFormula(const LTLFormula& other) : 
    _root(other._root), _apset(other._apset) {}

  /** Destructor */
  ~LTLFormula() {}

  /** Deep copy */
  LTLFormula_ptr copy() const {
    LTLFormula_ptr copy_p(new LTLFormula(_root->copy(), _apset));
    return copy_p;
  }

  /** Get root node */
  LTLNode_p getRootNode() {return _root;}

  /** Get APSet */
  APSet_cp getAPSet() {return _apset;}

  /**
   * Switch the APSet to another with the same number of APs.
   */
  void switchAPSet(APSet_cp new_apset) {
    if (new_apset->size()!=_apset->size()) {
      THROW_EXCEPTION(IllegalArgumentException, "New APSet has to have the same size as the old APSet!");
    }

    _apset=new_apset;
  }
  

  /**
   * Get a LTLFormula_ptr for the subformula rooted at subroot
   */
  LTLFormula_ptr getSubFormula(LTLNode_p subroot) {
    LTLFormula_ptr sub(new LTLFormula(subroot, _apset));
    return sub;		     
  }

  /** Return an LTLFormula_ptr of the negation of this formula */
  LTLFormula_ptr negate() {
    LTLNode_p new_root(new LTLNode(LTLNode::T_NOT, 
				   getRootNode()));
    return LTLFormula_ptr(new LTLFormula(new_root, _apset));
  }

  // TODO:
  //bool isInPNF() {

  /** Return true if the formula is syntactically safe (has to be in PNF) */
  bool isSafe() {
    return getRootNode()->isSafe();
  }

  /** Return true if the formula is syntactically co-safe (has to be in PNF) */
  bool isCoSafe() {
    return getRootNode()->isCoSafe();
  }

  /** Return true if the formula has at least one NextStep operator */
  bool hasNextStep() {
    return getRootNode()->hasNextStep();
  }

  /** Return this formula in PNF */
  LTLFormula_ptr toPNF() {
    return LTLFormula_ptr(new LTLFormula(getRootNode()->toPNF(),
					 _apset));
  }
  
  /** Return this formula in DNF (no temporal operators allowed) */
  LTLFormula_ptr toDNF() {
    return LTLFormula_ptr(new LTLFormula(getRootNode()->toPNF()->toDNF(), _apset));
  }

  /**
   * Calls Functor::operator(APMonom& m) for each monom of the formula. 
   * Formula has to be in DNF!
   */
  template <class Functor>
  void forEachMonom(Functor& f) {
    getRootNode()->forEachMonom(f);
  }

  /** Print this formula in infix format (SPIN) to out */
  void printInfix(std::ostream &out) {
    printInfix(getRootNode(), out);
  }

  /** Print this formula in prefix format to out */
  void printPrefix(std::ostream &out) {
    printPrefix(getRootNode(), out);
  }

  /** Get a string with this formula in infix format (SPIN) */
  std::string toStringInfix() {
    std::stringstream sstream;
    printInfix(sstream);
    return sstream.str();
  }

  /** Get a string with this formula in prefix format*/
  std::string toStringPrefix() {
    std::stringstream sstream;
    printPrefix(sstream);
    return sstream.str();
  }

private:
  /** The root node */
  LTLNode_p _root;
  /** The underlying APSet */
  APSet_cp _apset;

  /** Print in prefix format */
  void printPrefix(LTLNode_p ltl,
		    std::ostream& out) {
    switch (ltl->getType()) {
    case LTLNode::T_AP:
      out << _apset->getAP(ltl->getAP());
      break;
    case LTLNode::T_TRUE:
      out << "t";
      break;
    case LTLNode::T_FALSE:
      out << "f";
      break;
    case LTLNode::T_NOT:
      out << "! ";
      printPrefix(ltl->getLeft(), out);
      break;
    case LTLNode::T_AND:
      out << "& ";
      printPrefix(ltl->getLeft(), out);
      printPrefix(ltl->getRight(), out);
      break;
    case LTLNode::T_OR:
      out << "| ";
      printPrefix(ltl->getLeft(), out);
      printPrefix(ltl->getRight(), out);
      break;
    case LTLNode::T_XOR:
      out << "^ ";
      printPrefix(ltl->getLeft(), out);
      printPrefix(ltl->getRight(), out);
      break;
    case LTLNode::T_IMPLICATE:
      out << "i ";
      printPrefix(ltl->getLeft(), out);
      printPrefix(ltl->getRight(), out);
      break;
    case LTLNode::T_EQUIV:
      out << "e ";
      printPrefix(ltl->getLeft(), out);
      printPrefix(ltl->getRight(), out);
      break;
    case LTLNode::T_NEXTSTEP:
      out << "X ";
      printPrefix(ltl->getLeft(), out);
      break;
    case LTLNode::T_GLOBALLY:
      out << "G ";
      printPrefix(ltl->getLeft(), out);
      break;
    case LTLNode::T_FINALLY:
      out << "F ";
      printPrefix(ltl->getLeft(), out);
      break;
    case LTLNode::T_UNTIL:
      out << "U ";
      printPrefix(ltl->getLeft(), out);
      printPrefix(ltl->getRight(), out);
      break;
    case LTLNode::T_RELEASE:
      out << "V ";
      printPrefix(ltl->getLeft(), out);
      printPrefix(ltl->getRight(), out);
      break;
    case LTLNode::T_WEAKUNTIL:
      out << "| U ";
      printPrefix(ltl->getLeft(), out);
      printPrefix(ltl->getRight(), out);
      out << "G ";
      printPrefix(ltl->getLeft(), out);
      break;
    case LTLNode::T_WEAKRELEASE:
    case LTLNode::T_BEFORE:
      THROW_EXCEPTION(Exception, "Not yet implemented");      
    default:
      THROW_EXCEPTION(Exception, "Illegal operator");
    }
    out << " ";
  }

  /** Print in infix (SPIN) format */
  void printInfix(LTLNode_p ltl,
		  std::ostream& out) {
    switch (ltl->getType()) {
    case LTLNode::T_AP:
      out << _apset->getAP(ltl->getAP());
      break;
    case LTLNode::T_TRUE:
      out << "true";
      break;
    case LTLNode::T_FALSE:
      out << "false";
      break;
    case LTLNode::T_NOT:
      out << "! (";
      printInfix(ltl->getLeft(), out);
      out << ")";
      break;
    case LTLNode::T_AND:
      out << "(";
      printInfix(ltl->getLeft(), out);
      out << ") && (";
      printInfix(ltl->getRight(), out);
      out << ")";
      break;
    case LTLNode::T_OR:
      out << "(";
      printInfix(ltl->getLeft(), out);
      out << ") || (";
      printInfix(ltl->getRight(), out);
      out << ")";
      break;
    case LTLNode::T_XOR:
      out << "(";
      printInfix(ltl->getLeft(), out);
      out << ") ^ (";
      printInfix(ltl->getRight(), out);
      out << ")";
      break;
    case LTLNode::T_IMPLICATE:
      out << "(";
      printInfix(ltl->getLeft(), out);
      out << ") -> (";
      printInfix(ltl->getRight(), out);
      out << ")";
      break;
    case LTLNode::T_EQUIV:
      out << "(";
      printInfix(ltl->getLeft(), out);
      out << ") <-> (";
      printInfix(ltl->getRight(), out);
      out << ")";
      break;
    case LTLNode::T_NEXTSTEP:
      out << "X (";
      printInfix(ltl->getLeft(), out);
      out << ")";
      break;
    case LTLNode::T_GLOBALLY:
      out << "[] (";
      printInfix(ltl->getLeft(), out);
      out << ")";
      break;
    case LTLNode::T_FINALLY:
      out << "<> (";
      printInfix(ltl->getLeft(), out);
      out << ")";
      break;
    case LTLNode::T_UNTIL:
      out << "(";
      printInfix(ltl->getLeft(), out);
      out << ") U (";
      printInfix(ltl->getRight(), out);
      out << ")";
      break;

    case LTLNode::T_RELEASE:
      out << "(";
      printInfix(ltl->getLeft(), out);
      out << ") V (";
      printInfix(ltl->getRight(), out);
      out << ")";
      break;
    case LTLNode::T_WEAKUNTIL:
      out << "(";
      out << "(";
      printInfix(ltl->getLeft(), out);
      out << ") U ((";
      printInfix(ltl->getRight(), out);
      out << ") || ([] (";
      printInfix(ltl->getLeft(), out);
      out << "))))";
      break;
    case LTLNode::T_WEAKRELEASE:
    case LTLNode::T_BEFORE:
      // TODO: implement
      THROW_EXCEPTION(Exception, "Not yet implemented");      
    default:
      THROW_EXCEPTION(Exception, "Illegal operator");
    }
  }
};





#endif
