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


#ifndef LTL2NBA_HPP
#define LTL2NBA_HPP

/** @file
 * Provides wrapper classes for external LTL-to-Buechi translators.
 */

#include "NBA.hpp"
#include "LTLFormula.hpp"
#include "common/RunProgram.hpp"
#include "parsers/parser_interface.hpp"
#include <cstdio>

/**
 * Virtual base class for wrappers to external LTL-to-Buechi translators.
 */
template <class NBA_t>
class LTL2NBA {
public:
  /** Constructor */
  LTL2NBA() {}
  /** Destructor */
  virtual ~LTL2NBA() {}

  /** Convert an LTL formula to an NBA */
  virtual NBA_t *ltl2nba(LTLFormula& ltl) = 0;
};

/**
 * Wrapper for external LTL-to-Buechi translators using the SPIN interface.
 */
template <class NBA_t>
class LTL2NBA_SPIN : public LTL2NBA<NBA_t> {
public:
  /**
   * Constructor
   * @param path path to the executable
   * @param arguments vector of command line arguments to be passed to the external translator
   */
  LTL2NBA_SPIN(std::string path,
		 std::vector<std::string> arguments=std::vector<std::string>()) :
    _path(path), _arguments(arguments)  {}

  /** Destructor */
  virtual ~LTL2NBA_SPIN() {}

  /** 
   * Convert an LTL formula to an NBA
   * @param ltl
   * @return a pointer to the created NBA (caller gets ownership).
   */
  virtual
  NBA_t *ltl2nba(LTLFormula& ltl) {

    // Create canonical APSet (with 'p0', 'p1', ... as AP)
    LTLFormula_ptr ltl_canonical=ltl.copy();
    APSet_cp canonical_apset(ltl.getAPSet()->createCanonical());
    ltl_canonical->switchAPSet(canonical_apset);

    AnonymousTempFile spin_outfile;
    std::vector<std::string> arguments;
    arguments.push_back("-f");
    arguments.push_back(ltl_canonical->toStringInfix());

    arguments.insert(arguments.end(),
		     _arguments.begin(),
		     _arguments.end());
    
    const char *program_path=_path.c_str();
    
    RunProgram spin(program_path,
		    arguments,
		    false,
		    0,
		    &spin_outfile,
		    0);
    
    int rv=spin.waitForTermination();
    if (rv==0) {
      NBA_t *result_nba(new NBA_t(canonical_apset));
      
      FILE *f=spin_outfile.getInFILEStream();
      if (f==NULL) {
	throw Exception("");
      }

      int rc=nba_parser_promela::parse(f, result_nba);
      fclose(f);

      if (rc!=0) {
	throw Exception("Couldn't parse PROMELA file!");
      }
      
      // switch back to original APSet
      result_nba->switchAPSet(ltl.getAPSet());

      return result_nba;
    } else {
      // There was an error, return null ptr
      return (NBA_t *)0;
    }
  }

private:
  /** The path */
  std::string _path;

  /** The arguments */
  std::vector<std::string> _arguments;
};


/**
 * Wrapper for external LTL-to-Buechi translators using the LBTT interface.
 */
template <class NBA_t>
class LTL2NBA_LBTT : public LTL2NBA<NBA_t> {
public:
  /**
   * Constructor
   * @param path path to the executable
   * @param arguments vector of command line arguments to be passed to the external translator
   */
  LTL2NBA_LBTT(std::string path,
	       std::vector<std::string> arguments=std::vector<std::string>()) :
    _path(path), _arguments(arguments)  {}

  /** Destructor */
  virtual ~LTL2NBA_LBTT() {}

  /** 
   * Convert an LTL formula to an NBA
   * @param ltl
   * @return a pointer to the created NBA (caller gets ownership).
   */
  virtual NBA_t *ltl2nba(LTLFormula& ltl) {
    // Create canonical APSet (with 'p0', 'p1', ... as AP)
    LTLFormula_ptr ltl_canonical=ltl.copy();
    APSet_cp canonical_apset(ltl.getAPSet()->createCanonical());
    ltl_canonical->switchAPSet(canonical_apset);




    NamedTempFile infile(true);
    NamedTempFile outfile(true);

    std::ostream& o=infile.getOStream();
    o << ltl_canonical->toStringPrefix() << std::endl;
    o.flush();

    std::vector<std::string> arguments(_arguments);
    arguments.push_back(infile.getFileName());
    arguments.push_back(outfile.getFileName());
    
    const char *program_path=_path.c_str();
    
    RunProgram ltl2nba_lbtt(program_path,
			    arguments,
			    false,
			    0,
			    0,
			    0);
    
    int rv=ltl2nba_lbtt.waitForTermination();
    if (rv==0) {
      NBA_t *result_nba=new NBA_t(ltl_canonical->getAPSet());


      FILE *f=outfile.getInFILEStream();
      if (f==NULL) {
	throw Exception("");
      }
      int rc=nba_parser_lbtt::parse(f, result_nba);
      fclose(f);

      if (rc!=0) {
	throw Exception("Couldn't parse LBTT file!");
      }

      // result_nba->print(std::cerr);

      // switch back to original APSet
      result_nba->switchAPSet(ltl.getAPSet());

      return result_nba;
    } else {
      // There was an error, return null ptr
      return (NBA_t *)0;
    }
  }

private:
  /** The path */
  std::string _path;

  /** The arguments */
  std::vector<std::string> _arguments;
};




#endif
