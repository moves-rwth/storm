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


#ifndef STUTTERSENSITIVENESSINFORMATION_H
#define STUTTERSENSITIVENESSINFORMATION_H

/** @file
 * Provide class StutterSensitivenessInformation
 */

#include "NBAStutterClosure.hpp"
#include "NBAAnalysis.hpp"
#include "LTLFormula.hpp"

#include "common/TimeKeeper.hpp"

/** Calculate and store information about which symbols from 2^APSet
 *  are insensitive to stuttering.
 */
class StutterSensitivenessInformation {
public:
  /** Constructor */
  StutterSensitivenessInformation() :
    _completelyStutterInsensitive(false),
    _partiallyStutterInsensitive(false),
    _hasCheckedLTL(false),
    _hasCheckedNBAs(false) {}

  /** Check the LTLFormula syntacticly for
   *  stutter insensitiveness */
  void checkLTL(LTLFormula_ptr& ltl) {
    if (hasTimekeeping()) {getTimeKeeper().start();}
    if (ltl->hasNextStep()) {
      _completelyStutterInsensitive=false;
      _partiallyStutterInsensitive=false;
    } else {
      _completelyStutterInsensitive=true;
      _partiallyStutterInsensitive=false;
    }

    _hasCheckedLTL=true;

    if (hasTimekeeping()) {getTimeKeeper().stop();}
  }
  

  /** Check for partial stutter insensitiveness using 
   *  the nba and the complement nba */
  template <class NBA_t>
  void checkNBAs(NBA_t& nba,
		 NBA_t& nba_complement) {
    if (hasTimekeeping()) {getTimeKeeper().start();}
    APSet_cp apset=nba.getAPSet_cp();    

    bool nba_is_smaller=(nba.size() < nba_complement.size());

    if (_printInfo) {
      std::cerr << "Checking for insensitiveness" << std::endl;
    }
    bool one_insensitive=false;
    bool all_insensitive=true;
    for (APSet::element_iterator it=apset->all_elements_begin();
	 it!=apset->all_elements_end();
	 ++it) {
      APElement elem(*it);

      if (_partiallyInsensitive.get(elem)) {
	// don't recheck something we already now is stutter insensitive
	one_insensitive=true;
	continue;
      }

      if (_printInfo) {
	std::cerr << "Checking " << elem.toString(*apset) << ": ";
	std::cerr.flush();
      }
      
      bool insensitive;
      if (nba_is_smaller) {
	insensitive=is_stutter_insensitive(nba, nba_complement, elem);
      } else {
	insensitive=is_stutter_insensitive(nba_complement, nba, elem);
      }
      if (insensitive) {
	_partiallyInsensitive.set(elem);
	one_insensitive=true;
	if (_printInfo) {
	  std::cerr << "+" << std::endl;
	}
      } else {
	all_insensitive=false;
	if (_printInfo) {
	  std::cerr << "-" << std::endl;
	}
      }
    }

    _hasCheckedNBAs=true;
    _partiallyStutterInsensitive=one_insensitive;

    if (hasTimekeeping()) {getTimeKeeper().stop();}
  }

  /** Check for partial stutter insensitiveness for a LTL formula.
   *  @param ltl the LTL formula
   *  @param llt2nba the LTL2NBA translator, has to provide function ltl2nba(ltl)
   */
  template <class LTL2NBA_Calc>
  void checkPartial(LTLFormula& ltl, 
		    LTL2NBA_Calc& ltl2nba) {
    if (hasTimekeeping()) {getTimeKeeper().start();}
    checkNBAs(*ltl2nba.ltl2nba(ltl, true), 
	      *ltl2nba.ltl2nba(*ltl.negate()->toPNF(), true));
    if (hasTimekeeping()) {getTimeKeeper().stop();}
  }

  /** Check for partial stutter insensitiveness for a LTL formula, using an 
   *  already calculated NBA.
   *  @param nba an NBA for the positive formula
   *  @param ltl_neg the negated LTL formula (in PNF)
   *  @param llt2nba the LTL2NBA translator, has to provide function ltl2nba(ltl)
   */  
  template <class NBA_t, class LTL2NBA_Calc>
  void checkPartial(NBA_t& nba, 
		    LTLFormula& ltl_neg, 
		    LTL2NBA_Calc& ltl2nba) {
    if (hasTimekeeping()) {getTimeKeeper().start();}
    checkNBAs(nba, *ltl2nba.ltl2nba(ltl_neg));
    if (hasTimekeeping()) {getTimeKeeper().stop();}
  }


  
  /** Return true iff all symbols are completely stutter
   * insensitive. */
  bool isCompletelyInsensitive() const {
    return _completelyStutterInsensitive;
  }

  /** Return true iff some (or all) symbols are stutter insensitive,
   *  result is undefined if isCompletelyInsensitive()==true
   */
  bool isPartiallyInsensitive() const {
    return _partiallyStutterInsensitive;
  }

  /** Get the set of the stutter insensitive symbols from 2^APSet */
  const BitSet& getPartiallyInsensitiveSymbols() const {
    return _partiallyInsensitive;
  }

  /** shared_ptr */
  typedef std::shared_ptr<StutterSensitivenessInformation> ptr;

  /** Static, do Timekeeping? */
  static bool hasTimekeeping() {
    return _enableTimekeeping;
  }

  /** Static, enable Timekeeping? */
  static void enableTimekeeping() {
    _enableTimekeeping=true;
  }

  /** Static, enable printing of verbose information */
  static void enablePrintInfo() {
    _printInfo=true;
  }

  /** Get the timekeeper object */
  static TimeKeeper& getTimeKeeper() {
    return _timekeeper;
  }


private:
  bool _completelyStutterInsensitive;
  bool _partiallyStutterInsensitive;

  bool _hasCheckedLTL;
  bool _hasCheckedNBAs;
  BitSet _partiallyInsensitive;

  static bool _enableTimekeeping;
  static bool _printInfo;
  static TimeKeeper _timekeeper;


  // -- private member functions

  /** Check that symbol label is stutter insensitive, 
   *  using nba and complement_nba */
  template <class NBA_t>
  bool is_stutter_insensitive(NBA_t& nba, NBA_t& nba_complement, APElement label) {
    typedef std::shared_ptr<NBA_t> NBA_ptr;
    
    NBA_ptr stutter_closed_nba=NBAStutterClosure::stutter_closure(nba, label);
    
    NBA_ptr product=NBA_t::product_automaton(*stutter_closed_nba, nba_complement);

    NBAAnalysis<NBA_t> analysis_product(*product);
    bool empty=analysis_product.emptinessCheck();
    //  std::cerr << "NBA is " << (empty ? "empty" : "not empty") << std::endl;
    
    return empty;
  }
};


#endif
