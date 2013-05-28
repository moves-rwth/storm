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


#ifndef LTLSAFETYAUTOMATA_H
#define LTLSAFETYAUTOMATA_H

/** @file
 * Provide wrapper to the scheck tool.
 */

#include "APSet.hpp"
#include "LTLFormula.hpp"
#include "DBA2DRA.hpp"
#include "common/RunProgram.hpp"
#include "common/TempFile.hpp"
#include "common/Exceptions.hpp"
#include "parsers/parser_interface.hpp"


#include <memory>

/**
 * Wrapper to the external scheck tool that can convert (co)safe LTL formulas
 * to determinstic Büchi automata.
 */
class LTLSafetyAutomata {
public:
  /** Constructor */
  LTLSafetyAutomata(bool only_syntactically_safe=true) 
    : _only_syn(only_syntactically_safe) {}

  /** 
   * Generate a DRA for an LTL formula using scheck
   * @param ltl the formula
   * @param scheck_path the path to the scheck executable
   * @return a shared_ptr to the generated DRA (on failure returns a ptr to 0)
   */
  template <class DRA>
  typename DRA::shared_ptr 
  ltl2dra(LTLFormula& ltl, const std::string& scheck_path) {
    LTLFormula_ptr ltl_;
    LTLFormula *ltl_for_scheck=0;
    
    bool safe=false;
    
    if (ltl.isSafe()) {
      safe=true;
      ltl_=ltl.negate();
      ltl_for_scheck=ltl_.get();
    } else if (ltl.isCoSafe()) {
      ltl_for_scheck=&ltl;
    } else {
      if (_only_syn) {
	// Not syntactically safe -> abort
	typename DRA::shared_ptr p;
	return p;
      }
    }

    //    std::cerr<< "Calling scheck with " 
    //	     <<ltl_for_scheck->toStringPrefix() << " : " << safe << std::endl;

    std::auto_ptr<NBA_t> nba(ltl2dba(*ltl_for_scheck, 
				     scheck_path,
				     _only_syn));
    
    if (nba.get()==0) {
      typename DRA::shared_ptr p;
      return p;
    }
    
    //    nba->print(std::cerr);
    
    // safe -> negate DRA
    return DBA2DRA::dba2dra<NBA_t,DRA>(*nba,safe);
    //    return dba2dra<DRA>(*nba, safe);
    // nba is auto-destructed
  }
  

  
private:
  /** Only check syntactically safe formulas */
  bool _only_syn;
  
  typedef NBA<APElement, EdgeContainerExplicit_APElement> NBA_t;

  /** 
   * Generate a DRA for an LTL formula using scheck
   * @param ltl the formula
   * @param scheck_path the path to the scheck executable
   * @param only_syn don't use check for pathological formulas
   * @return a shared_ptr to the generated DRA (on failure returns a ptr to 0)
   */
  NBA_t *ltl2dba(LTLFormula& ltl, 
		 const std::string& scheck_path,
		 bool only_syn=true) {
    AnonymousTempFile scheck_outfile;
    AnonymousTempFile scheck_infile;
    std::vector<std::string> arguments;
    arguments.push_back("-d");

    if (only_syn) {
      arguments.push_back("-s");
    } else {
      THROW_EXCEPTION(Exception, "Checking for pathological safety not yet supported!");
      //      arguments.push_back("-p");
      //      arguments.push_back( --path-to-lbtt-type-ltl2nba-translator-- );
    }


    // Create canonical APSet (with 'p0', 'p1', ... as AP)
    LTLFormula_ptr ltl_canonical=ltl.copy();
    APSet_cp canonical_apset(ltl.getAPSet()->createCanonical());
    ltl_canonical->switchAPSet(canonical_apset);
    
    std::ostream& so=scheck_infile.getOStream();
    so << ltl_canonical->toStringPrefix() << std::endl;
    so.flush();
    
    const char *program_path=scheck_path.c_str();
    
    RunProgram scheck(program_path,
		      arguments,
		      false,
		      &scheck_infile,
		      &scheck_outfile,
		      0);
    
    int rv=scheck.waitForTermination();
    if (rv==0) {
      FILE *f=scheck_outfile.getInFILEStream();
      if (f==NULL) {
	throw Exception("");
      }
      
      std::auto_ptr<NBA_t> 
	nba_result(new NBA_t(ltl_canonical->getAPSet()));
      
      int rc=nba_parser_lbtt::parse(f, nba_result.get());
      fclose(f);

      if (rc!=0) {
	THROW_EXCEPTION(Exception, "scheck: Couldn't parse LBTT file!");
      }
      
      // switch back to original APSet
      nba_result->switchAPSet(ltl.getAPSet());

      // release auto_ptr so the NBA is not deleted,
      // return pointer to it to calling function
      return nba_result.release();
    } else {
      std::cerr << "scheck error code: "<< rv << std::endl;
      return 0;
    }
  }
  
};

#endif
