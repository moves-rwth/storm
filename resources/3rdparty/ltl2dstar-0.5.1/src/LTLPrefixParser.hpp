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


#ifndef LTLPREFIXPARSER_HPP
#define LTLPREFIXPARSER_HPP

/** @file
 * Provides a parser for LTL formulas.
 */

#include "LTLFormula.hpp"
#include "common/Exceptions.hpp"

#include <string>
#include <cassert>


#include <boost/tokenizer.hpp>

/** 
 * A parser for LTL formulas
 */
class LTLPrefixParser {
public:

  /**
   * Functor for boost::tokenizer
   */
  class ltl_seperator {
  public:
    void reset() {};
    
    template <typename InputIterator, typename Token>
    bool operator()(InputIterator& next, 
		    InputIterator end, 
		    Token& tok) {
      
      tok.clear();
      
      while (next != end &&
	     *next == ' ') {
	++next;  // skip whitespace
      }

      if (next == end) {return false;}
      
      if (*next=='"') {
	tok+=*next;
	++next;

	// start of quoted string
	while (next != end &&
	       *next != '"') {
	  tok+=*next;
	  ++next;
	}

	if (next == end) {
	  // no matching end of quote!
	  THROW_EXCEPTION(Exception, "Missing final quote!");
	}

	tok+=*next; // eat final "
	++next; 
	return true;
      } else {
	// read until end or first whitespace
	while (next != end &&
	       *next != ' ') {
	  tok+=*next;
	  ++next;
	}

        return true;
      }
    }
  };

  /** The tokenizer */
  typedef boost::tokenizer<ltl_seperator> tokenizer;

  /**
   * Parse the formula and return an LTLFormula_ptr
   * @param formula the formula string
   * @param predefined_apset if specified, use this APSet and don't 
   *                         add new atomic propositions
   * @return the LTLFormula_ptr
   */
  static LTLFormula_ptr parse(std::string const& formula,
			  APSet_cp predefined_apset=APSet_cp((APSet*)0)) {
    //    boost::char_separator<char> sep(" ");
    ltl_seperator sep;
    tokenizer tokens(formula, sep);

    APSet_p apset(new APSet());

    tokenizer::iterator it=tokens.begin();

    LTLNode_p ltl=parse(it,
			tokens,
			*apset,
			predefined_apset.get());

    if (it!=tokens.end()) {
      THROW_EXCEPTION(Exception, "Unexpected character(s) at end of LTL formula: '"+*it+"'");
    }

    APSet_cp apset_=(predefined_apset.get()!=0)
      ? predefined_apset : 
      boost::const_pointer_cast<APSet const>(apset);


    return LTLFormula_ptr(new LTLFormula(ltl, apset_));
  }
  
private:
  static LTLNode_p parse(tokenizer::iterator& iterator,
			 tokenizer& tokenizer,
			 APSet& apset,
			 APSet const* predefined_apset) {
    if (iterator==tokenizer.end()) {
      THROW_EXCEPTION(Exception, "LTL-Parse-Exception!");
    }

    std::string s=*iterator;
    ++iterator;

    if (s.length()==0) {
      THROW_EXCEPTION(Exception, "LTL-Parse-Exception!");
    }

    //    std::cerr << s << " ";

    unsigned char ch=s[0];
    if (s.length()==1) { 
     switch (ch) {
      case 't':
	return LTLNode_p(new LTLNode(LTLNode::T_TRUE, 
				     LTLNode_p(),
				     LTLNode_p()));
      case 'f':
	return LTLNode_p(new LTLNode(LTLNode::T_FALSE, 
				     LTLNode_p(), 
				     LTLNode_p()));
      case '!': {
	LTLNode_p left=parse(iterator, tokenizer, apset,
			     predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_NOT, left));
      }			

      case '|': {
	LTLNode_p left=parse(iterator, tokenizer, apset, predefined_apset);
	LTLNode_p right=parse(iterator, tokenizer, apset, predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_OR, left, right));
      }			

      case '&': {
	LTLNode_p left=parse(iterator, tokenizer, apset, predefined_apset);
	LTLNode_p right=parse(iterator, tokenizer, apset, predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_AND, left, right));
      }			

      case '^': {
	LTLNode_p left=parse(iterator, tokenizer, apset, predefined_apset);
	LTLNode_p right=parse(iterator, tokenizer, apset, predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_XOR, left, right));
      }			

      case 'i': {
	LTLNode_p left=parse(iterator, tokenizer, apset, predefined_apset);
	LTLNode_p right=parse(iterator, tokenizer, apset, predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_IMPLICATE, left, right));
      }			

      case 'e': {
	LTLNode_p left=parse(iterator, tokenizer, apset, predefined_apset);
	LTLNode_p right=parse(iterator, tokenizer, apset, predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_EQUIV, left, right));
      }			

      case 'X': {
	LTLNode_p left=parse(iterator, tokenizer, apset, predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_NEXTSTEP, left));
      }			

      case 'G': {
	LTLNode_p left=parse(iterator, tokenizer, apset, predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_GLOBALLY, left));
      }			

      case 'F': {
	LTLNode_p left=parse(iterator, tokenizer, apset, predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_FINALLY, left));
      }			

      case 'U': {
	LTLNode_p left=parse(iterator, tokenizer, apset, predefined_apset);
	LTLNode_p right=parse(iterator, tokenizer, apset, predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_UNTIL, left, right));
      }			

      case 'V': {
	LTLNode_p left=parse(iterator, tokenizer, apset, predefined_apset);
	LTLNode_p right=parse(iterator, tokenizer, apset, predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_RELEASE, left, right));
      }			

      case 'W': {
	LTLNode_p left=parse(iterator, tokenizer, apset, predefined_apset);
	LTLNode_p right=parse(iterator, tokenizer, apset, predefined_apset);
	return LTLNode_p(new LTLNode(LTLNode::T_WEAKUNTIL, left, right));
      }			
      }
    }

    // no operator =>
    // possible atomic proposition
    
    std::string& ap=s;
    
    if (ch=='"') {
      //	std::cerr << ap << std::endl;
      assert(ap[ap.size()-1]=='"'); // last char is "
      
      if (ap.size()<=2) {
	// empty ap!
	THROW_EXCEPTION(Exception, "LTL-Parse-Error: empty quoted string");
      }

      
      ap=ap.substr(1,ap.size()-2); // cut quotes
    } else if ((ch>='a' && ch<='z') ||
	       (ch>='A' && ch<='Z')) {
      ;  // nop
    } else {
      THROW_EXCEPTION(Exception, "LTL-Parse-Error");
    }
    
    int ap_i; // the AP index
    
    if (predefined_apset!=0) {
      if ((ap_i=predefined_apset->find(ap))!=-1) {
	return LTLNode_p(new LTLNode(ap_i));
      } else {
	// not found in predefined APSet!
	std::cerr << "[" << (int)s[2] << "]" << std::endl;
	THROW_EXCEPTION(Exception, "Can't parse formula with this APSet!");
      }
    } else {
      if ((ap_i=apset.find(ap))!=-1) {
	// AP exists already
	return LTLNode_p(new LTLNode(ap_i));
      } else {
	// create new AP
	ap_i=apset.addAP(ap);
	return LTLNode_p(new LTLNode(ap_i));
      }
    }
    assert(0);  // Unreachable
  }

};


#endif
