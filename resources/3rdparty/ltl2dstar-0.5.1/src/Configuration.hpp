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


#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/** @file
 * Provides Option classes.
 */

#include <string>
#include <vector>

/**
 * Options for Safra's algorithm
 */
class Options_Safra {
public:
  /** Optimize accepting true loops */
  bool opt_accloop; 

  /** Optimize all successor accepting */
  bool opt_accsucc; 

  /** Renaming optimization (templates) */
  bool opt_rename;  

  /** Try to reorder Safra trees */
  bool opt_reorder; 

  /** Use stuttering */
  bool stutter;

  /** Check for stutter insensitive */
  bool partial_stutter_check;

  /* Perform stutter closure on NBA before conversion */
  bool stutter_closure;

  /** Check for DBA */
  bool dba_check;

  /** Provide statistics */
  bool stat;

  /** Optimize accepting true loops in union construction */
  bool union_trueloop;

  /** Constructor */
  Options_Safra() {
    opt_none();
    
    dba_check=false;
    //    tree_verbose=false;
    stat=false;
    union_trueloop=true;

    stutter=false;
    partial_stutter_check=false;
    stutter_closure=false;
  }

  /** Enable all opt_ options */
  void opt_all() {
    opt_accloop
      =opt_accsucc
      =opt_rename
      =opt_reorder
      =true;
  }

  /** Disable all opt_ options */
  void opt_none() {
    opt_accloop
      =opt_accsucc
      =opt_rename
      =opt_reorder
      =false;
  }
};


/**
 * Options for the LTL2DSTAR scheduler.
 */
class LTL2DSTAR_Options {
public:
  /** Constructor */
  LTL2DSTAR_Options() {
    // Defaults...
    allow_union=true;
    recursive_union=true;
    
    optimizeAcceptance=true;
    
    bisim=false;
    recursive_bisim=true;
    
    safety=false;
    
    automata=RABIN;
    only_union=false;
    only_safety=false;

    detailed_states=false;
    verbose_scheduler=false;
  }
  
  /** Disable all options */
  void allFalse() {
    allow_union
      =recursive_union
      
      =safety
      
      =optimizeAcceptance
      =bisim
      =recursive_bisim
      
      =only_union
      =only_safety
      =detailed_states
      =verbose_scheduler
      =false;
  }
  
  /** Change options for next level of recursion */
  void recursion() {
    allow_union=allow_union && recursive_union;
    only_union=false;
    
    bisim=bisim && recursive_bisim;
  }

  /** Safra Options */
  Options_Safra opt_safra;
  
  /** Allow union construction */
  bool allow_union;

  /** Allow union construction on next levels */
  bool recursive_union;
  
  /** Allow using scheck for (co-)safety LTL formulas */
  bool safety;

  /** Allow optimization of acceptance conditions */
  bool optimizeAcceptance;

  /** Allow bisimulation */
  bool bisim;

  /** Allow bisimulation on all levels. */
  bool recursive_bisim;

  /** Provide detailed internal description in the states */
  bool detailed_states;
  
  /** Type of the automata that should be generated */
  enum automata_type {STREETT, RABIN, RABIN_AND_STREETT, ORIGINAL_NBA} automata;
  
  /** Use union construction exclusively */
  bool only_union;

  /** Use scheck exclusively */
  bool only_safety;
  
  /** Debug information from the scheduler */
  bool verbose_scheduler;

  /** Path to scheck */
  std::string scheck_path;
};




#endif
