/* -*- coding: utf-8 -*-
** Copyright (C) 2014-2019 Laboratoire de Recherche et Développement
** de l'Epita (LRDE).
**
** This file is part of Spot, a model checking library.
**
** Spot is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** Spot is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
** License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
%require "3.0"
%language "C++"
%locations
%defines
%expect 0 // No shift/reduce
%define api.prefix {hoayy}
%debug
%define parse.error verbose
%parse-param {void* scanner}
%lex-param {void* scanner} { PARSE_ERROR_LIST }
%define api.location.type {spot::location}

%code requires
{
#include "config.h"
#include <spot/misc/common.hh>
#include <string>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <spot/twa/formula2bdd.hh>
#include <spot/parseaut/public.hh>
#include "spot/priv/accmap.hh"
#include <spot/tl/parse.hh>
#include <spot/twaalgos/alternation.hh>

using namespace std::string_literals;

#ifndef HAVE_STRVERSCMP
// If the libc does not have this, a version is compiled in lib/.
extern "C" int strverscmp(const char *s1, const char *s2);
#endif

// Work around Bison not letting us write
//  %lex-param { res.h->errors }
#define PARSE_ERROR_LIST res.h->errors

  inline namespace hoayy_support
  {
    typedef std::map<int, bdd> map_t;

    /* Cache parsed formulae.  Labels on arcs are frequently identical
       and it would be a waste of time to parse them to formula
       over and over, and to register all their atomic_propositions in
       the bdd_dict.  Keep the bdd result around so we can reuse
       it.  */
    typedef std::map<std::string, bdd> formula_cache;

    typedef std::pair<int, std::string*> pair;
    typedef spot::twa_graph::namer<std::string> named_tgba_t;

    // Note: because this parser is meant to be used on a stream of
    // automata, it tries hard to recover from errors, so that we get a
    // chance to reach the end of the current automaton in order to
    // process the next one.  Several variables below are used to keep
    // track of various error conditions.
    enum label_style_t { Mixed_Labels, State_Labels, Trans_Labels,
			 Implicit_Labels };
    enum acc_style_t { Mixed_Acc, State_Acc, Trans_Acc };

    struct result_
    {
      struct state_info
      {
	bool declared = false;
	bool used = false;
	spot::location used_loc;
      };
      spot::parsed_aut_ptr h;
      spot::twa_ptr aut_or_ks;
      spot::automaton_parser_options opts;
      std::string format_version;
      spot::location format_version_loc;
      spot::environment* env;
      formula_cache fcache;
      named_tgba_t* namer = nullptr;
      spot::acc_mapper_int* acc_mapper = nullptr;
      std::vector<int> ap;
      std::vector<bdd> guards;
      std::vector<bdd>::const_iterator cur_guard;
      // If "Alias: ..." occurs before "AP: ..." in the HOA format we
      // are in trouble because the parser would like to turn each
      // alias into a BDD, yet the atomic proposition have not been
      // declared yet.  We solve that by using arbitrary BDD variables
      // numbers (in fact we use the same number given in the Alias:
      // definition) and keeping track of the highest variable number
      // we have seen (unknown_ap_max).  Once AP: is encountered,
      // we can remap everything.  If AP: is never encountered an
      // unknown_ap_max is non-negative, then we can signal an error.
      int unknown_ap_max = -1;
      spot::location unknown_ap_max_location;
      bool in_alias = false;
      map_t dest_map;
      std::vector<state_info> info_states; // States declared and used.
      std::vector<std::pair<spot::location,
                            std::vector<unsigned>>> start; // Initial states;
      std::unordered_map<std::string, bdd> alias;
      struct prop_info
      {
	spot::location loc;
	bool val;
	operator bool() const
	{
	  return val;
	};
      };
      std::unordered_map<std::string, prop_info> props;
      spot::location states_loc;
      spot::location ap_loc;
      spot::location state_label_loc;
      spot::location accset_loc;
      spot::acc_cond::mark_t acc_state;
      spot::acc_cond::mark_t neg_acc_sets = {};
      spot::acc_cond::mark_t pos_acc_sets = {};
      int plus;
      int minus;
      std::vector<std::string>* state_names = nullptr;
      std::map<unsigned, unsigned>* highlight_edges = nullptr;
      std::map<unsigned, unsigned>* highlight_states = nullptr;
      std::map<unsigned, unsigned> states_map;
      std::set<int> ap_set;
      unsigned cur_state;
      int states = -1;
      int ap_count = -1;
      int accset = -1;
      bdd state_label;
      bdd cur_label;
      bool has_state_label = false;
      bool ignore_more_ap = false; // Set to true after the first "AP:"
      // line has been read.
      bool ignore_acc = false;	// Set to true in case of missing
				// Acceptance: lines.
      bool ignore_acc_silent = false;
      bool ignore_more_acc = false; // Set to true after the first
				// "Acceptance:" line has been read.

      label_style_t label_style = Mixed_Labels;
      acc_style_t acc_style = Mixed_Acc;

      bool accept_all_needed = false;
      bool accept_all_seen = false;
      bool aliased_states = false;

      spot::trival universal = spot::trival::maybe();
      spot::trival existential = spot::trival::maybe();
      spot::trival complete = spot::trival::maybe();
      bool trans_acc_seen = false;

      std::map<std::string, spot::location> labels;

      prop_info prop_is_true(const std::string& p)
      {
	auto i = props.find(p);
	if (i == props.end())
	  return prop_info{spot::location(), false};
	return i->second;
      }

      prop_info prop_is_false(const std::string& p)
      {
	auto i = props.find(p);
	if (i == props.end())
	  return prop_info{spot::location(), false};
	return prop_info{i->second.loc, !i->second.val};
      }

      ~result_()
      {
	delete namer;
	delete acc_mapper;
      }
    };
  }
}

%parse-param {result_& res}
%parse-param {spot::location initial_loc}

%initial-action { @$ = res.h->loc = initial_loc; }

%union
{
  std::string* str;
  unsigned int num;
  int b;
  spot::acc_cond::mark_t mark;
  pair* p;
  std::list<pair>* list;
  spot::acc_cond::acc_code* code;
  std::vector<unsigned>* states;
}

%code
{
#include <sstream>

  /* parseaut.hh and parsedecl.hh include each other recursively.
   We must ensure that YYSTYPE is declared (by the above %union)
   before parsedecl.hh uses it. */
#include <spot/parseaut/parsedecl.hh>

  static void fill_guards(result_& res);
}

/**** HOA tokens ****/
%token HOA "HOA:"
%token STATES "States:"
%token START "Start:"
%token AP "AP:"
%token ALIAS "Alias:"
%token ACCEPTANCE "Acceptance:"
%token ACCNAME "acc-name:"
%token TOOL "tool:"
%token NAME "name:"
%token PROPERTIES "properties:"
%token BODY "--BODY--"
%token END "--END--"
%token STATE "State:";
%token SPOT_HIGHLIGHT_EDGES "spot.highlight.edges:";
%token SPOT_HIGHLIGHT_STATES "spot.highlight.states:";
%token <str> IDENTIFIER "identifier";  // also used by neverclaim
%token <str> HEADERNAME "header name";
%token <str> ANAME "alias name";
%token <str> STRING "string";
%token <num> INT "integer";
%token ENDOFFILE 0 "end of file"

%token DRA "DRA"
%token DSA "DSA"
%token V2 "v2"
%token EXPLICIT "explicit"
%token ACCPAIRS "Acceptance-Pairs:"
%token ACCSIG "Acc-Sig:";
%token ENDOFHEADER "---";


%left '|'
%left '&'
%precedence '!'

%type <states> init-state-conj-2 state-conj-2 state-conj-checked
%type <num> checked-state-num state-num acc-set sign
%type <b> label-expr
%type <mark> acc-sig acc-sets trans-acc_opt state-acc_opt
             dstar_accsigs dstar_state_accsig
%type <str> string_opt

/**** NEVERCLAIM tokens ****/

%token NEVER "never"
%token SKIP "skip"
%token IF "if"
%token FI "fi"
%token DO "do"
%token OD "od"
%token ARROW "->"
%token GOTO "goto"
%token FALSE "false"
%token ATOMIC "atomic"
%token ASSERT "assert"
%token <str> FORMULA "boolean formula"

%type <b> nc-formula
%type <str> nc-opt-dest nc-formula-or-ident
%type <p> nc-transition nc-src-dest
%type <list> nc-transitions nc-transition-block
%type <str> nc-one-ident nc-ident-list
%type <code> acceptance-cond

/**** LBTT tokens *****/
 // Also using INT, STRING
%token ENDAUT "-1"
%token ENDDSTAR "end of DSTAR automaton"
%token <num> LBTT "LBTT header"
%token <num> INT_S "state acceptance"
%token <num> LBTT_EMPTY "acceptance sets for empty automaton"
%token <num> ACC "acceptance set"
%token <num> STATE_NUM "state number"
%token <num> DEST_NUM "destination number"
%type <mark> lbtt-acc

%destructor { delete $$; } <str>
%destructor { bdd_delref($$); } <b>
%destructor { bdd_delref($$->first); delete $$->second; delete $$; } <p>
%destructor { delete $$; } <code>
%destructor { delete $$; } <states>
%printer {
  auto& os = debug_stream();
  os << '{';
  bool notfirst = false;
  for (auto i: *$$)
  {
    if (notfirst)
      os << ", ";
    else
      notfirst = true;
    os << i;
  }
  os << '}';
} <states>
%destructor {
  for (auto i = $$->begin(); i != $$->end(); ++i)
  {
    bdd_delref(i->first);
    delete i->second;
  }
  delete $$;
  } <list>
%printer {
    if ($$)
      debug_stream() << *$$;
    else
      debug_stream() << "\"\""; } <str>
%printer { debug_stream() << $$; } <num>

%%
aut: aut-1     { res.h->loc = @$; YYACCEPT; }
   | ENDOFFILE { YYABORT; }
   | error ENDOFFILE { YYABORT; }
   | error aut-1
     {
       error(@1, "leading garbage was ignored");
       res.h->loc = @2;
       YYACCEPT;
     }

aut-1: hoa   { res.h->type = spot::parsed_aut_type::HOA; }
     | never { res.h->type = spot::parsed_aut_type::NeverClaim; }
     | lbtt  { res.h->type = spot::parsed_aut_type::LBTT; }
     | dstar /* will set type as DSA or DRA while parsing first line */

/**********************************************************************/
/*                          Rules for HOA                             */
/**********************************************************************/

hoa: header "--BODY--" body "--END--"
   | "HOA:" error "--END--"

string_opt: %empty { $$ = nullptr; }
          | STRING { $$ = $1; }
BOOLEAN: 't' | 'f'

header: format-version header-items
        {
          bool v1plus = strverscmp("v1", res.format_version.c_str()) < 0;
	  // Preallocate the states if we know their number.
	  if (res.states >= 0)
	    {
	      unsigned states = res.states;
	      for (auto& p : res.start)
                for (unsigned s: p.second)
                  if ((unsigned) res.states <= s)
                    {
                      error(p.first, "initial state number is larger "
                            "than state count...");
                      error(res.states_loc, "... declared here.");
                      states = std::max(states, s + 1);
                    }
	      if (res.opts.want_kripke)
		res.h->ks->new_states(states, bddfalse);
	      else
		res.h->aut->new_states(states);
	      res.info_states.resize(states);
	    }
	  if (res.accset < 0)
	    {
	      error(@$, "missing 'Acceptance:' header");
	      res.ignore_acc = true;
	    }
          if (res.unknown_ap_max >= 0 && !res.ignore_more_ap)
            {
              error(res.unknown_ap_max_location,
                    "atomic proposition used in Alias without AP declaration");
              for (auto& p: res.alias)
                p.second = bddtrue;
            }
	  // Process properties.
	  {
	    auto explicit_labels = res.prop_is_true("explicit-labels");
	    auto implicit_labels = res.prop_is_true("implicit-labels");

	    if (implicit_labels)
	      {
		if (res.opts.want_kripke)
		  error(implicit_labels.loc,
			"Kripke structure may not use implicit labels");

		if (explicit_labels)
		  {
		    error(implicit_labels.loc,
			  "'properties: implicit-labels' is incompatible "
			  "with...");
		    error(explicit_labels.loc,
			  "... 'properties: explicit-labels'.");
		  }
		else
		  {
		    res.label_style = Implicit_Labels;
		  }
	      }

	    auto trans_labels = res.prop_is_true("trans-labels");
	    auto state_labels = res.prop_is_true("state-labels");

	    if (trans_labels)
	      {
		if (res.opts.want_kripke)
		  error(trans_labels.loc,
			"Kripke structures may not use transition labels");

		if (state_labels)
		  {
		    error(trans_labels.loc,
			  "'properties: trans-labels' is incompatible with...");
		    error(state_labels.loc,
			  "... 'properties: state-labels'.");
		  }
		else
		  {
		    if (res.label_style != Implicit_Labels)
		      res.label_style = Trans_Labels;
		  }
	      }
	    else if (state_labels)
	      {
		if (res.label_style != Implicit_Labels)
		  {
		    res.label_style = State_Labels;
		  }
		else
		  {
		    error(state_labels.loc,
			  "'properties: state-labels' is incompatible with...");
		    error(implicit_labels.loc,
			  "... 'properties: implicit-labels'.");
		  }
	      }

	    if (res.opts.want_kripke && res.label_style != State_Labels)
	      error(@$,
		    "Kripke structures should use 'properties: state-labels'");

	    auto state_acc = res.prop_is_true("state-acc");
	    auto trans_acc = res.prop_is_true("trans-acc");
	    if (trans_acc)
	      {
		if (state_acc)
		  {
		    error(trans_acc.loc,
			  "'properties: trans-acc' is incompatible with...");
		    error(state_acc.loc,
			  "... 'properties: state-acc'.");
		  }
		else
		  {
		    res.acc_style = Trans_Acc;
		  }
	      }
	    else if (state_acc)
	      {
		res.acc_style = State_Acc;
	      }

            if (auto univ_branch = res.prop_is_true("univ-branch"))
              if (res.opts.want_kripke)
                error(univ_branch.loc,
                    "Kripke structures may not use 'properties: univ-branch'");
          }
	  {
	    unsigned ss = res.start.size();
	    auto det = res.prop_is_true("deterministic");
	    auto no_exist = res.prop_is_false("exist-branch");
	    if (ss > 1)
	      {
		if (det)
		  {
		    error(det.loc,
			  "deterministic automata should have at most "
			  "one initial state");
                    res.universal = spot::trival::maybe();
		  }
                else if (no_exist)
                  {
		    error(no_exist.loc,
			  "universal automata should have at most "
			  "one initial state");
                    res.universal = spot::trival::maybe();
                  }
	      }
	    else
	      {
		// Assume the automaton is deterministic until proven
		// wrong, or unless we are building a Kripke structure.
                if (!res.opts.want_kripke)
                  {
                    res.universal = true;
                    res.existential = true;
                  }
	      }
            for (auto& ss: res.start)
              {
                if (ss.second.size() > 1)
                  {
                    if (auto no_univ = res.prop_is_false("univ-branch"))
                      {
                        error(ss.first,
                              "conjunct initial state despite...");
                        error(no_univ.loc, "... property: !univ-branch");
                      }
                    else if (v1plus)
                      if (auto det = res.prop_is_true("deterministic"))
                        {
                          error(ss.first,
                                "conjunct initial state despite...");
                          error(det.loc, "... property: deterministic");
                        }
                    res.existential = false;
                  }
              }
	    auto complete = res.prop_is_true("complete");
	    if (ss < 1)
	      {
		if (complete)
		  {
		    error(complete.loc,
			  "complete automata should have at least "
			  "one initial state");
		  }
		res.complete = false;
	      }
	    else
	      {
		// Assume the automaton is complete until proven
		// wrong.
                if (!res.opts.want_kripke)
                  res.complete = true;
	      }
	    // if ap_count == 0, then a Kripke structure could be
	    // declared complete, although that probably doesn't
	    // matter.
	    if (res.opts.want_kripke && complete && res.ap_count > 0)
	      error(complete.loc,
		    "Kripke structure may not be complete");
	  }
	  if (res.opts.trust_hoa)
	    {
	      auto& a = res.aut_or_ks;
	      auto& p = res.props;
	      auto e = p.end();
	      auto si = p.find("stutter-invariant");
	      if (si != e)
		{
		  a->prop_stutter_invariant(si->second.val);
		  auto i = p.find("stutter-sensitive");
		  if (i != e && si->second.val == i->second.val)
		    error(i->second.loc,
			  "automaton cannot be both stutter-invariant"
			  "and stutter-sensitive");
		}
	      else
		{
		  auto ss = p.find("stutter-sensitive");
		  if (ss != e)
		    a->prop_stutter_invariant(!ss->second.val);
		}
	      auto iw = p.find("inherently-weak");
	      auto vw = p.find("very-weak");
	      auto w = p.find("weak");
	      auto t = p.find("terminal");
              if (vw != e)
                {
                  a->prop_very_weak(vw->second.val);
                  if (w != e && !w->second.val && vw->second.val)
                    {
		      error(w->second.loc,
                            "'properties: !weak' contradicts...");
		      error(vw->second.loc,
			    "... 'properties: very-weak' given here");
                    }
                  if (iw != e && !iw->second.val && vw->second.val)
                    {
		      error(iw->second.loc,
                            "'properties: !inherently-weak' contradicts...");
		      error(vw->second.loc,
			    "... 'properties: very-weak' given here");
                    }
                }
	      if (iw != e)
		{
		  a->prop_inherently_weak(iw->second.val);
		  if (w != e && !iw->second.val && w->second.val)
		    {
		      error(w->second.loc, "'properties: weak' contradicts...");
		      error(iw->second.loc,
			    "... 'properties: !inherently-weak' given here");
		    }
		  if (t != e && !iw->second.val && t->second.val)
		    {
		      error(t->second.loc,
			    "'properties: terminal' contradicts...");
		      error(iw->second.loc,
			    "... 'properties: !inherently-weak' given here");
		    }
		}
	      if (w != e)
		{
		  a->prop_weak(w->second.val);
		  if (t != e && !w->second.val && t->second.val)
		    {
		      error(t->second.loc,
			    "'properties: terminal' contradicts...");
		      error(w->second.loc,
			    "... 'properties: !weak' given here");
		    }
		}
	      if (t != e)
		a->prop_terminal(t->second.val);
	      auto u = p.find("unambiguous");
	      if (u != e)
		{
		  a->prop_unambiguous(u->second.val);
		  auto d = p.find("deterministic");
		  if (d != e && !u->second.val && d->second.val)
		    {
		      error(d->second.loc,
			    "'properties: deterministic' contradicts...");
		      error(u->second.loc,
			    "... 'properties: !unambiguous' given here");
		    }
		}
	      auto sd = p.find("semi-deterministic");
	      if (sd != e)
		{
		  a->prop_semi_deterministic(sd->second.val);
		  auto d = p.find("deterministic");
		  if (d != e && !sd->second.val && d->second.val)
		    {
		      error(d->second.loc,
			    "'properties: deterministic' contradicts...");
		      error(sd->second.loc,
			    "... 'properties: !semi-deterministic' given here");
		    }
		}
	    }
	}

version: IDENTIFIER
         {
	   res.format_version = *$1;
	   res.format_version_loc = @1;
	   delete $1;
	 }

format-version: "HOA:" { res.h->loc = @1; } version

aps: "AP:" INT
     {
       if (res.ignore_more_ap)
	 {
	   error(@1, "ignoring this redeclaration of APs...");
	   error(res.ap_loc, "... previously declared here.");
	 }
       else
	 {
	   res.ap_count = $2;
	   res.ap.reserve($2);
	 }
     }
     ap-names
     {
       if (!res.ignore_more_ap)
	 {
	   res.ap_loc = @1 + @2;
	   if ((int) res.ap.size() != res.ap_count)
	     {
	       std::ostringstream out;
	       out << "found " << res.ap.size()
		   << " atomic propositions instead of the "
		   << res.ap_count << " announced";
	       error(@$, out.str());
	     }
	   res.ignore_more_ap = true;
           // If we have seen Alias: before AP: we have some variable
           // renaming to perform.
           if (res.unknown_ap_max >= 0)
             {
               int apsize = res.ap.size();
               if (apsize <= res.unknown_ap_max)
                 {
                   error(res.unknown_ap_max_location,
                         "AP number is larger than the number of APs...");
                   error(@1, "... declared here");
                 }
               bddPair* pair = bdd_newpair();
               int max = std::min(res.unknown_ap_max, apsize - 1);
               for (int i = 0; i <= max; ++i)
                 if (i != res.ap[i])
                   bdd_setbddpair(pair, i, res.ap[i]);
               bdd extra = bddtrue;
               for (int i = apsize; i <= res.unknown_ap_max; ++i)
                 extra &= bdd_ithvar(i);
               for (auto& p: res.alias)
                 p.second = bdd_restrict(bdd_replace(p.second, pair), extra);
               bdd_freepair(pair);
             }
	 }
     }

header-items: %empty
            | header-items header-item
header-item: "States:" INT
           {
	     if (res.states >= 0)
	       {
		 error(@$, "redefinition of the number of states...");
		 error(res.states_loc, "... previously defined here.");
	       }
	     else
	       {
		 res.states_loc = @$;
	       }
	     if (((int) $2) < 0)
	       {
		 error(@$, "too many states for this implementation");
		 YYABORT;
	       }
	     res.states = std::max(res.states, (int) $2);
	   }
           | "Start:" init-state-conj-2
	     {
               res.start.emplace_back(@$, *$2);
               delete $2;
	     }
           | "Start:" state-num
	     {
	       res.start.emplace_back(@$, std::vector<unsigned>{$2});
	     }
           | aps
           | "Alias:" ANAME { res.in_alias=true; } label-expr
             {
               res.in_alias = false;
	       if (!res.alias.emplace(*$2, bdd_from_int($4)).second)
		 {
		   std::ostringstream o;
		   o << "ignoring redefinition of alias @" << *$2;
		   error(@$, o.str());
		 }
	       delete $2;
	       bdd_delref($4);
	     }
           | "Acceptance:" INT
	      {
		if (res.ignore_more_acc)
		  {
		    error(@1 + @2, "ignoring this redefinition of the "
			  "acceptance condition...");
		    error(res.accset_loc, "... previously defined here.");
		  }
		else if ($2 > SPOT_MAX_ACCSETS)
		  {
		    error(@1 + @2,
			  "this implementation cannot support such a large "
			  "number of acceptance sets");
		    YYABORT;
		  }
		else
		  {
		    res.aut_or_ks->acc().add_sets($2);
		    res.accset = $2;
		    res.accset_loc = @1 + @2;
		  }
	     }
             acceptance-cond
	     {
	       res.ignore_more_acc = true;
	       // Not setting the acceptance in case of error will
	       // force it to be true.
	       if (res.opts.want_kripke && (!$4->is_t() || $2 > 0))
		 error(@2 + @4,
		       "the acceptance for Kripke structure must be '0 t'");
	       else
		 res.aut_or_ks->set_acceptance($2, *$4);
	       delete $4;
	     }
           | "acc-name:" IDENTIFIER acc-spec
             {
	       delete $2;
	     }
           | "tool:" STRING string_opt
             {
	       delete $2;
	       delete $3;
	     }
           | "name:" STRING
             {
	       res.aut_or_ks->set_named_prop("automaton-name", $2);
	     }
           | "properties:" properties
	   | "spot.highlight.edges:"
	     { res.highlight_edges = new std::map<unsigned, unsigned>; }
             highlight-edges
	   | "spot.highlight.states:"
	     { res.highlight_states = new std::map<unsigned, unsigned>; }
             highlight-states
           | HEADERNAME header-spec
	     {
	       char c = (*$1)[0];
	       if (c >= 'A' && c <= 'Z')
		 error(@$, "ignoring unsupported header \"" + *$1 + ":\"\n\t"
		       "(but the capital indicates information that should not"
		       " be ignored)");
	       delete $1;
	     }
           | error

ap-names: %empty
        | ap-names ap-name
ap-name: STRING
	 {
	   if (!res.ignore_more_ap)
	     {
	       auto f = res.env->require(*$1);
	       int b = 0;
	       if (f == nullptr)
		 {
		   std::ostringstream out;
		   out << "unknown atomic proposition \"" << *$1 << "\"";
		   error(@1, out.str());
		   b = res.aut_or_ks->register_ap("$unknown$");
		 }
	       else
		 {
		   b = res.aut_or_ks->register_ap(f);
		   if (!res.ap_set.emplace(b).second)
		     {
		       std::ostringstream out;
		       out << "duplicate atomic proposition \"" << *$1 << "\"";
		       error(@1, out.str());
		     }
		 }
	       res.ap.push_back(b);
	     }
	   delete $1;
	 }

acc-spec: %empty
          | acc-spec BOOLEAN
	  | acc-spec INT
	  | acc-spec IDENTIFIER
            {
	      delete $2;
	    }
properties: %empty
            | properties IDENTIFIER
	      {
                bool val = true;
                // no-univ-branch was replaced by !univ-branch in HOA 1.1
                if (*$2 == "no-univ-branch")
                  {
                    *$2 = "univ-branch";
                    val = false;
                  }
		auto pos = res.props.emplace(*$2, result_::prop_info{@2, val});
		if (pos.first->second.val != val)
		  {
		    std::ostringstream out(std::ios_base::ate);
		    error(@2, "'properties: "s + (val ? "" : "!")
                          + *$2 + "' contradicts...");
		    error(pos.first->second.loc,
			  "... 'properties: "s + (val ? "!" : "") + *$2
			  + "' previously given here.");
		  }
		delete $2;
	      }
            | properties '!' IDENTIFIER
	      {
		auto loc = @2 + @3;
		auto pos =
		  res.props.emplace(*$3, result_::prop_info{loc, false});
		if (pos.first->second.val)
		  {
		    std::ostringstream out(std::ios_base::ate);
		    error(loc, "'properties: !"s + *$3 + "' contradicts...");
		    error(pos.first->second.loc, "... 'properties: "s + *$3
                          + "' previously given here.");
		  }
		delete $3;
	      }

highlight-edges: %empty
               | highlight-edges INT INT
              {
		res.highlight_edges->emplace($2, $3);
	      }
highlight-states: %empty
                | highlight-states INT INT
              {
		res.highlight_states->emplace($2, $3);
	      }

header-spec: %empty
             | header-spec BOOLEAN
             | header-spec INT
             | header-spec STRING
	       {
		 delete $2;
	       }
             | header-spec IDENTIFIER
	       {
		 delete $2;
	       }

state-conj-2: checked-state-num '&' checked-state-num
            {
              $$ = new std::vector<unsigned>{$1, $3};
            }
            | state-conj-2 '&' checked-state-num
            {
              $$ = $1;
              $$->emplace_back($3);
            }

// Same as state-conj-2 except we cannot check the state numbers
// against a number of states that may not have been declared yet.
init-state-conj-2: state-num '&' state-num
            {
              $$ = new std::vector<unsigned>{$1, $3};
            }
            | init-state-conj-2 '&' state-num
            {
              $$ = $1;
              $$->emplace_back($3);
            }

label-expr: 't'
	    {
	      $$ = bddtrue.id();
	    }
          | 'f'
	    {
	      $$ = bddfalse.id();
	    }
	  | INT
	    {
              if (res.in_alias && !res.ignore_more_ap)
                {
                  // We are reading Alias: before AP: has been given.
                  // Use $1 as temporary variable number.  We will relabel
                  // everything once AP: is known.
                  if (res.unknown_ap_max < (int)$1)
                    {
                      res.unknown_ap_max = $1;
                      res.unknown_ap_max_location = @1;
                      int missing_vars = 1 + bdd_varnum() - $1;
                      if (missing_vars > 0)
                        bdd_extvarnum(missing_vars);
                    }
                  $$ = bdd_ithvar($1).id();
                }
	      else if ($1 >= res.ap.size())
		{
		  error(@1, "AP number is larger than the number of APs...");
		  error(res.ap_loc, "... declared here");
		  $$ = bddtrue.id();
		}
	      else
		{
		  $$ = bdd_ithvar(res.ap[$1]).id();
		  bdd_addref($$);
		}
	    }
          | ANAME
	    {
	      auto i = res.alias.find(*$1);
	      if (i == res.alias.end())
		{
		  error(@$, "unknown alias @" + *$1);
		  $$ = 1;
		}
	      else
		{
		  $$ = i->second.id();
		  bdd_addref($$);
		}
	      delete $1;
	    }
          | '!' label-expr
	    {
              $$ = bdd_not($2);
              bdd_delref($2);
              bdd_addref($$);
            }
          | label-expr '&' label-expr
	    {
              $$ = bdd_and($1, $3);
              bdd_delref($1);
              bdd_delref($3);
              bdd_addref($$);
            }
          | label-expr '|' label-expr
	    {
              $$ = bdd_or($1, $3);
              bdd_delref($1);
              bdd_delref($3);
              bdd_addref($$);
            }
          | '(' label-expr ')'
	  {
	    $$ = $2;
	  }


acc-set: INT
            {
	      if ((int) $1 >= res.accset)
		{
		  if (!res.ignore_acc)
		    {
		      error(@1, "number is larger than the count "
			    "of acceptance sets...");
		      error(res.accset_loc, "... declared here.");
		    }
		  $$ = -1U;
		}
	      else
		{
		  $$ = $1;
		}
	    }

acceptance-cond: IDENTIFIER '(' acc-set ')'
		 {
		   if ($3 != -1U)
		     {
		       res.pos_acc_sets |= res.aut_or_ks->acc().mark($3);
		       if (*$1 == "Inf")
                         {
                           $$ = new spot::acc_cond::acc_code
                             (res.aut_or_ks->acc().inf({$3}));
                         }
		       else if (*$1 == "Fin")
                         {
                           $$ = new spot::acc_cond::acc_code
                             (res.aut_or_ks->acc().fin({$3}));
                         }
                       else
                         {
                           error(@1, "unknown acceptance '"s + *$1
                                 + "', expected Fin or Inf");
                           $$ = new spot::acc_cond::acc_code;
                         }
		     }
		   else
		     {
		       $$ = new spot::acc_cond::acc_code;
		     }
		   delete $1;
		 }
               | IDENTIFIER '(' '!' acc-set ')'
		 {
		   if ($4 != -1U)
		     {
		       res.neg_acc_sets |= res.aut_or_ks->acc().mark($4);
		       if (*$1 == "Inf")
			 $$ = new spot::acc_cond::acc_code
			   (res.aut_or_ks->acc().inf_neg({$4}));
		       else
			 $$ = new spot::acc_cond::acc_code
			   (res.aut_or_ks->acc().fin_neg({$4}));
		     }
		   else
		     {
		       $$ = new spot::acc_cond::acc_code;
		     }
		   delete $1;
		 }
               | '(' acceptance-cond ')'
	         {
		   $$ = $2;
		 }
               | acceptance-cond '&' acceptance-cond
	         {
		   *$3 &= std::move(*$1);
		   $$ = $3;
		   delete $1;
		 }
               | acceptance-cond '|' acceptance-cond
	         {
		   *$3 |= std::move(*$1);
		   $$ = $3;
		   delete $1;
		 }
               | 't'
	         {
		   $$ = new spot::acc_cond::acc_code;
		 }
	       | 'f'
	       {
	         {
		   $$ = new spot::acc_cond::acc_code
		     (res.aut_or_ks->acc().fin({}));
		 }
	       }


body: states
      {
	for (auto& p: res.start)
          for (unsigned s: p.second)
            if (s >= res.info_states.size() || !res.info_states[s].declared)
              {
                error(p.first, "initial state " + std::to_string(s) +
                      " has no definition");
                // Pretend that the state is declared so we do not
                // mention it in the next loop.
                if (s < res.info_states.size())
                  res.info_states[s].declared = true;
                res.complete = spot::trival::maybe();
              }
	unsigned n = res.info_states.size();
	// States with number above res.states have already caused a
	// diagnostic, so let not add another one.
	if (res.states >= 0)
	  n = res.states;
	for (unsigned i = 0; i < n; ++i)
	  {
	    auto& p = res.info_states[i];
            if (!p.declared)
              {
                if (p.used)
                  error(p.used_loc,
                        "state " + std::to_string(i) + " has no definition");
                if (!p.used && res.complete)
                  if (auto p = res.prop_is_true("complete"))
                    {
                      error(res.states_loc,
                            "state " + std::to_string(i) +
                            " has no definition...");
                      error(p.loc, "... despite 'properties: complete'");
                    }
                res.complete = false;
              }
	  }
        if (res.complete)
          if (auto p = res.prop_is_false("complete"))
            {
              error(@1, "automaton is complete...");
              error(p.loc, "... despite 'properties: !complete'");
            }
        bool det_warned = false;
        if (res.universal && res.existential)
          if (auto p = res.prop_is_false("deterministic"))
            {
              error(@1, "automaton is deterministic...");
              error(p.loc, "... despite 'properties: !deterministic'");
              det_warned = true;
            }
        static bool tolerant = getenv("SPOT_HOA_TOLERANT");
        if (res.universal.is_true() && !det_warned && !tolerant)
          if (auto p = res.prop_is_true("exist-branch"))
            {
              error(@1, "automaton has no existential branching...");
              error(p.loc, "... despite 'properties: exist-branch'\n"
                    "note: If this is an issue you cannot fix, you may disable "
                    "this diagnostic\n      by defining the SPOT_HOA_TOLERANT "
                    "environment variable.");
              det_warned = true;
            }
        if (res.existential.is_true() && !det_warned && !tolerant)
          if (auto p = res.prop_is_true("univ-branch"))
            {
              error(@1, "automaton is has no universal branching...");
              error(p.loc, "... despite 'properties: univ-branch'\n"
                    "note: If this is an issue you cannot fix, you may disable "
                    "this diagnostic\n      by defining the SPOT_HOA_TOLERANT "
                    "environment variable.");
              det_warned = true;
            }
      }
state-num: INT
	   {
	     if (((int) $1) < 0)
	       {
		 error(@1, "state number is too large for this implementation");
		 YYABORT;
	       }
	     $$ = $1;
	   }

checked-state-num: state-num
		   {
		     if ((int) $1 >= res.states)
		       {
			 if (res.states >= 0)
			   {
			     error(@1, "state number is larger than state "
				   "count...");
			     error(res.states_loc, "... declared here.");
			   }
			 if (res.opts.want_kripke)
			   {
			     int missing =
			       ((int) $1) - res.h->ks->num_states() + 1;
			     if (missing >= 0)
			       {
				 res.h->ks->new_states(missing, bddfalse);
				 res.info_states.resize
				   (res.info_states.size() + missing);
			       }
			   }
			 else
			   {
			     int missing =
			       ((int) $1) - res.h->aut->num_states() + 1;
			     if (missing >= 0)
			       {
				 res.h->aut->new_states(missing);
				 res.info_states.resize
				   (res.info_states.size() + missing);
			       }
			   }
		       }
		     // Remember the first place were a state is the
		     // destination of a transition.
		     if (!res.info_states[$1].used)
		       {
			 res.info_states[$1].used = true;
			 res.info_states[$1].used_loc = @1;
		       }
		     $$ = $1;
		   }

states: %empty
        | states state
        {
	  if ((res.universal.is_true() || res.complete.is_true()))
	    {
	      bdd available = bddtrue;
	      bool det = true;
	      for (auto& t: res.h->aut->out(res.cur_state))
		{
		  if (det && !bdd_implies(t.cond, available))
		    det = false;
		  available -= t.cond;
		}
	      if (res.universal.is_true() && !det)
		{
		  res.universal = false;
		  if (auto p = res.prop_is_true("deterministic"))
		    {
		      error(@2, "automaton is not deterministic...");
		      error(p.loc,
			    "... despite 'properties: deterministic'");
		    }
		  else if (auto p = res.prop_is_false("exist-branch"))
		    {
		      error(@2, "automaton has existential branching...");
		      error(p.loc,
			    "... despite 'properties: !exist-branch'");
		    }
		}
	      if (res.complete.is_true() && available != bddfalse)
		{
		  res.complete = false;
		  if (auto p = res.prop_is_true("complete"))
		    {
		      error(@2, "automaton is not complete...");
		      error(p.loc, "... despite 'properties: complete'");
		    }
		}
	    }
	}
state: state-name labeled-edges
     | state-name unlabeled-edges
       {
	 if (!res.has_state_label) // Implicit labels
	   {
	     if (res.cur_guard != res.guards.end())
	       error(@$, "not enough transitions for this state");

	     if (res.label_style == State_Labels)
	       {
		 error(@2, "these transitions have implicit labels but the"
		       " automaton is...");
		 error(res.props["state-labels"].loc, "... declared with "
		       "'properties: state-labels'");
		 // Do not repeat this message.
		 res.label_style = Mixed_Labels;
	       }
	     res.cur_guard = res.guards.begin();
	   }
	 else if (res.opts.want_kripke)
	   {
	     res.h->ks->state_from_number(res.cur_state)->cond(res.state_label);
	   }

       }
     | error
       {
	 // Assume the worse.  This skips the tests about determinism
	 // we might perform on the state.
	 res.universal = spot::trival::maybe();
	 res.existential = spot::trival::maybe();
	 res.complete = spot::trival::maybe();
       }


state-name: "State:" state-label_opt checked-state-num string_opt state-acc_opt
	  {
	    res.cur_state = $3;
	    if (res.info_states[$3].declared)
	      {
		std::ostringstream o;
		o << "redeclaration of state " << $3;
		error(@1 + @3, o.str());
                // The additional transitions from extra states might
                // led us to believe that the automaton is complete
                // while it is not if we ignore them.
                if (res.complete.is_true())
                  res.complete = spot::trival::maybe();
	      }
	    res.info_states[$3].declared = true;
	    res.acc_state = $5;
	    if ($4)
	      {
		if (!res.state_names)
		  res.state_names =
		    new std::vector<std::string>(res.states > 0 ?
						 res.states : 0);
		if (res.state_names->size() < $3 + 1)
		  res.state_names->resize($3 + 1);
		(*res.state_names)[$3] = std::move(*$4);
		delete $4;
	      }
	    if (res.opts.want_kripke && !res.has_state_label)
	      error(@$, "Kripke structures should have labeled states");
	  }
label: '[' label-expr ']'
	   {
             res.cur_label = bdd_from_int($2);
             bdd_delref($2);
	   }
     | '[' error ']'
           {
	     error(@$, "ignoring this invalid label");
	     res.cur_label = bddtrue;
	   }
state-label_opt: %empty { res.has_state_label = false; }
               | label
	       {
		 res.has_state_label = true;
		 res.state_label_loc = @1;
		 res.state_label = res.cur_label;
		 if (res.label_style == Trans_Labels
		     || res.label_style == Implicit_Labels)
		   {
		     error(@$,
			   "state label used although the automaton was...");
		     if (res.label_style == Trans_Labels)
		       error(res.props["trans-labels"].loc,
			     "... declared with 'properties: trans-labels'"
			     " here");
		     else
		       error(res.props["implicit-labels"].loc,
			     "... declared with 'properties: implicit-labels'"
			     " here");
		     // Do not show this error anymore.
		     res.label_style = Mixed_Labels;
		   }
	       }
trans-label: label
	         {
		   if (res.has_state_label)
		     {
		       error(@1, "cannot label this edge because...");
		       error(res.state_label_loc,
			     "... the state is already labeled.");
		       res.cur_label = res.state_label;
		     }
		   if (res.label_style == State_Labels
		       || res.label_style == Implicit_Labels)
		     {
		       error(@$, "transition label used although the "
			     "automaton was...");
		       if (res.label_style == State_Labels)
			 error(res.props["state-labels"].loc,
			       "... declared with 'properties: state-labels' "
			       "here");
		       else
			 error(res.props["implicit-labels"].loc,
			       "... declared with 'properties: implicit-labels'"
			       " here");
		       // Do not show this error anymore.
		       res.label_style = Mixed_Labels;
		     }
		 }

acc-sig: '{' acc-sets '}'
	     {
	       $$ = $2;
	       if (res.ignore_acc && !res.ignore_acc_silent)
		 {
		   error(@$, "ignoring acceptance sets because of "
			 "missing acceptance condition");
		   // Emit this message only once.
		   res.ignore_acc_silent = true;
		 }
	     }
           | '{' error '}'
	     {
	       error(@$, "ignoring this invalid acceptance set");
	     }
acc-sets: %empty
          {
	    $$ = spot::acc_cond::mark_t({});
	  }
        | acc-sets acc-set
	  {
	    if (res.ignore_acc || $2 == -1U)
	      $$ = spot::acc_cond::mark_t({});
	    else
	      $$ = $1 | res.aut_or_ks->acc().mark($2);
	  }

state-acc_opt: %empty
               {
                 $$ = spot::acc_cond::mark_t({});
               }
             | acc-sig
               {
		 $$ = $1;
		 if (res.acc_style == Trans_Acc)
		   {
		     error(@$, "state-based acceptance used despite...");
		     error(res.props["trans-acc"].loc,
			   "... declaration of transition-based acceptance.");
		     res.acc_style = Mixed_Acc;
		   }
	       }
trans-acc_opt: %empty
               {
                 $$ = spot::acc_cond::mark_t({});
               }
             | acc-sig
               {
		 $$ = $1;
		 res.trans_acc_seen = true;
		 if (res.acc_style == State_Acc)
		   {
		     error(@$, "trans-based acceptance used despite...");
		     error(res.props["state-acc"].loc,
			   "... declaration of state-based acceptance.");
		     res.acc_style = Mixed_Acc;
		   }
	       }

/* block of labeled-edges, with occasional (incorrect) unlabeled edge */
labeled-edges: %empty
               | some-labeled-edges
some-labeled-edges: labeled-edge
                  | some-labeled-edges labeled-edge
                  | some-labeled-edges incorrectly-unlabeled-edge
incorrectly-unlabeled-edge: checked-state-num trans-acc_opt
                            {
			      bdd cond = bddtrue;
			      if (!res.has_state_label)
				error(@$, "missing label for this edge "
				      "(previous edge is labeled)");
			      else
				cond = res.state_label;
			      if (cond != bddfalse)
				{
				  if (res.opts.want_kripke)
				    res.h->ks->new_edge(res.cur_state, $1);
				  else
				    res.h->aut->new_edge(res.cur_state, $1,
							 cond,
							 $2 | res.acc_state);
				}
			    }
labeled-edge: trans-label checked-state-num trans-acc_opt
	      {
		if (res.cur_label != bddfalse)
		  {
		    if (res.opts.want_kripke)
		      res.h->ks->new_edge(res.cur_state, $2);
		    else
		      res.h->aut->new_edge(res.cur_state, $2,
					   res.cur_label, $3 | res.acc_state);
		  }
	      }
	    | trans-label state-conj-checked trans-acc_opt
	      {
                if (res.cur_label != bddfalse)
                  {
                    assert(!res.opts.want_kripke);
                    res.h->aut->new_univ_edge(res.cur_state,
                                              $2->begin(), $2->end(),
                                              res.cur_label,
                                              $3 | res.acc_state);
                  }
                delete $2;
	      }

state-conj-checked: state-conj-2
              {
                $$ = $1;
                if (auto ub = res.prop_is_false("univ-branch"))
                  {
                    error(@1, "universal branch used despite"
                          " previous declaration...");
                    error(ub.loc, "... here");
                  }
                res.existential = false;
              }

/* Block of unlabeled edge, with occasional (incorrect) labeled
   edge. We never have zero unlabeled edges, these are considered as
   zero labeled edges. */
unlabeled-edges: unlabeled-edge
               | unlabeled-edges unlabeled-edge
               | unlabeled-edges incorrectly-labeled-edge
unlabeled-edge: checked-state-num trans-acc_opt
		{
		  bdd cond;
		  if (res.has_state_label)
		    {
		      cond = res.state_label;
		    }
		  else
		    {
		      if (res.guards.empty())
			fill_guards(res);
		      if (res.cur_guard == res.guards.end())
			{
			  error(@$, "too many transitions for this state, "
				"ignoring this one");
			  cond = bddfalse;
			}
		      else
			{
			  cond = *res.cur_guard++;
			}
		    }
		  if (cond != bddfalse)
		    {
		      if (res.opts.want_kripke)
			res.h->ks->new_edge(res.cur_state, $1);
		      else
			res.h->aut->new_edge(res.cur_state, $1,
					     cond, $2 | res.acc_state);
		    }
		}
	      | state-conj-checked trans-acc_opt
		{
		  bdd cond;
		  if (res.has_state_label)
		    {
		      cond = res.state_label;
		    }
		  else
		    {
		      if (res.guards.empty())
			fill_guards(res);
		      if (res.cur_guard == res.guards.end())
			{
			  error(@$, "too many transitions for this state, "
				"ignoring this one");
			  cond = bddfalse;
			}
		      else
			{
			  cond = *res.cur_guard++;
			}
		    }
		  if (cond != bddfalse)
		    {
		      assert(!res.opts.want_kripke);
                      res.h->aut->new_univ_edge(res.cur_state,
                                                $1->begin(), $1->end(),
                                                cond, $2 | res.acc_state);
		    }
                  delete $1;
		}
incorrectly-labeled-edge: trans-label unlabeled-edge
                          {
			    error(@1, "ignoring this label, because previous"
				  " edge has no label");
                          }


/**********************************************************************/
/*                   Rules for LTL2DSTAR's format                     */
/**********************************************************************/

dstar: dstar_type "v2" "explicit" dstar_header "---" dstar_states ENDDSTAR
     | dstar_type error ENDDSTAR
       {
	 error(@$, "failed to parse this as an ltl2dstar automaton");
       }

dstar_type: "DRA"
       {
         res.h->type = spot::parsed_aut_type::DRA;
         res.plus = 1;
         res.minus = 0;
	 if (res.opts.want_kripke)
	   {
	     error(@$,
		   "cannot read a Kripke structure out of a DSTAR automaton");
	     YYABORT;
	   }
       }
       | "DSA"
       {
	 res.h->type = spot::parsed_aut_type::DSA;
         res.plus = 0;
         res.minus = 1;
	 if (res.opts.want_kripke)
	   {
	     error(@$,
		   "cannot read a Kripke structure out of a DSTAR automaton");
	     YYABORT;
	   }
       }

dstar_header: dstar_sizes
  {
    if (res.states < 0)
      error(@1, "missing state count");
    if (!res.ignore_more_acc)
      error(@1, "missing acceptance-pair count");
    if (res.start.empty())
      error(@1, "missing start-state number");
    if (!res.ignore_more_ap)
      error(@1, "missing atomic propositions definition");

    if (res.states > 0)
      {
	res.h->aut->new_states(res.states);;
	res.info_states.resize(res.states);
      }
    res.acc_style = State_Acc;
    res.universal = true;
    res.existential = true;
    res.complete = true;
    fill_guards(res);
    res.cur_guard = res.guards.end();
  }

dstar_sizes: %empty
  | dstar_sizes error
  | dstar_sizes "Acceptance-Pairs:" INT
  {
    if (res.ignore_more_acc)
      {
	error(@1 + @2, "ignoring this redefinition of the "
	      "acceptance pairs...");
	error(res.accset_loc, "... previously defined here.");
      }
    else{
      res.accset = $3;
      res.h->aut->set_acceptance(2 * $3,
				 res.h->type == spot::parsed_aut_type::DRA
				 ? spot::acc_cond::acc_code::rabin($3)
				 : spot::acc_cond::acc_code::streett($3));
      res.accset_loc = @3;
      res.ignore_more_acc = true;
    }
  }
  | dstar_sizes "States:" INT
  {
    if (res.states < 0)
      {
	res.states = $3;
      }
    else
      {
	error(@$, "redeclaration of state count");
	if ((unsigned) res.states < $3)
	  res.states = $3;
      }
  }
  | dstar_sizes "Start:" INT
  {
    res.start.emplace_back(@3, std::vector<unsigned>{$3});
  }
  | dstar_sizes aps

dstar_state_id: "State:" INT string_opt
  {
    if (res.cur_guard != res.guards.end())
      error(@1, "not enough transitions for previous state");
    if (res.states < 0 || $2 >= (unsigned) res.states)
      {
	std::ostringstream o;
	if (res.states > 0)
	  {
	    o << "state numbers should be in the range [0.."
	      << res.states - 1 << "]";
	  }
	else
	  {
	    o << "no states have been declared";
	  }
	error(@2, o.str());
      }
    else
      {
	res.info_states[$2].declared = true;

	if ($3)
	  {
	    if (!res.state_names)
	      res.state_names =
		new std::vector<std::string>(res.states > 0 ?
					     res.states : 0);
	    if (res.state_names->size() < $2 + 1)
	      res.state_names->resize($2 + 1);
	    (*res.state_names)[$2] = std::move(*$3);
	    delete $3;
	  }
      }

    res.cur_guard = res.guards.begin();
    res.dest_map.clear();
    res.cur_state = $2;
  }

sign: '+' { $$ = res.plus; }
  |   '-' { $$ = res.minus; }

// Membership to a pair is represented as (+NUM,-NUM)
dstar_accsigs: %empty
  {
    $$ = spot::acc_cond::mark_t({});
  }
  | dstar_accsigs sign INT
  {
    if (res.states < 0 || res.cur_state >= (unsigned) res.states)
      break;
    if (res.accset > 0 && $3 < (unsigned) res.accset)
      {
	$$ = $1;
	$$.set($3 * 2 + $2);
      }
    else
      {
	std::ostringstream o;
	if (res.accset > 0)
	  {
	    o << "acceptance pairs should be in the range [0.."
	      << res.accset - 1 << "]";
	  }
	else
	  {
	    o << "no acceptance pairs have been declared";
	  }
	error(@3, o.str());
      }
  }

dstar_state_accsig: "Acc-Sig:" dstar_accsigs { $$ = $2; }

dstar_transitions: %empty
  | dstar_transitions INT
  {
    std::pair<map_t::iterator, bool> i =
      res.dest_map.emplace($2, *res.cur_guard);
    if (!i.second)
      i.first->second |= *res.cur_guard;
    ++res.cur_guard;
  }

dstar_states: %empty
  | dstar_states error
  | dstar_states dstar_state_id dstar_state_accsig dstar_transitions
  {
    for (auto i: res.dest_map)
      res.h->aut->new_edge(res.cur_state, i.first, i.second, $3);
  }

/**********************************************************************/
/*                      Rules for neverclaims                         */
/**********************************************************************/

never: "never"
       {
	 if (res.opts.want_kripke)
	   {
	     error(@$, "cannot read a Kripke structure out of a never claim.");
	     YYABORT;
	   }
	 res.namer = res.h->aut->create_namer<std::string>();
	 res.h->aut->set_buchi();
	 res.acc_style = State_Acc;
	 res.pos_acc_sets = res.h->aut->acc().all_sets();
       }
       '{' nc-states '}'
       {
	 // Add an accept_all state if needed.
	 if (res.accept_all_needed && !res.accept_all_seen)
	   {
	     unsigned n = res.namer->new_state("accept_all");
	     res.h->aut->new_acc_edge(n, n, bddtrue);
	   }
	 // If we aliased existing state, we have some unreachable
	 // states to remove.
	 if (res.aliased_states)
	   res.h->aut->purge_unreachable_states();
	 res.info_states.resize(res.h->aut->num_states());
	 // Pretend that we have declared all states.
	 for (auto& p: res.info_states)
	   p.declared = true;
         res.h->aut->register_aps_from_dict();
       }

nc-states: %empty
  | nc-state
  | nc-states ';' nc-state
  | nc-states ';'

nc-one-ident: IDENTIFIER ':'
    {
      auto r = res.labels.insert(std::make_pair(*$1, @1));
      if (!r.second)
	{
	  error(@1, "redefinition of "s + *$1 + "...");
	  error(r.first->second, "... "s + *$1 + " previously defined here");
	}
      $$ = $1;
    }

nc-ident-list: nc-one-ident
    {
      unsigned n = res.namer->new_state(*$1);
      if (res.start.empty())
	{
	  // The first state is initial.
	  res.start.emplace_back(@$, std::vector<unsigned>{n});
	}
      $$ = $1;
    }
  | nc-ident-list nc-one-ident
    {
      res.aliased_states |=
	res.namer->alias_state(res.namer->get_state(*$1), *$2);
      // Keep any identifier that starts with accept.
      if (strncmp("accept", $1->c_str(), 6))
        {
          delete $1;
          $$ = $2;
        }
      else
        {
	  delete $2;
	  $$ = $1;
        }
    }

nc-transition-block:
  "if" nc-transitions "fi"
    {
      $$ = $2;
    }
  | "do" nc-transitions "od"
    {
      $$ = $2;
    }

nc-state:
  nc-ident-list "skip"
    {
      if (*$1 == "accept_all")
	res.accept_all_seen = true;

      auto acc = !strncmp("accept", $1->c_str(), 6) ?
	res.h->aut->acc().all_sets() : spot::acc_cond::mark_t({});
      res.namer->new_edge(*$1, *$1, bddtrue, acc);
      delete $1;
    }
  | nc-ident-list { delete $1; }
  | nc-ident-list "false" { delete $1; }
  | nc-ident-list nc-transition-block
    {
      auto acc = !strncmp("accept", $1->c_str(), 6) ?
	res.h->aut->acc().all_sets() : spot::acc_cond::mark_t({});
      for (auto& p: *$2)
	{
	  bdd c = bdd_from_int(p.first);
	  bdd_delref(p.first);
	  res.namer->new_edge(*$1, *p.second, c, acc);
	  delete p.second;
	}
      delete $1;
      delete $2;
    }

nc-transitions:
  %empty { $$ = new std::list<pair>; }
  | nc-transitions nc-transition
    {
      if ($2)
	{
	  $1->push_back(*$2);
	  delete $2;
	}
      $$ = $1;
    }

nc-formula-or-ident: FORMULA | IDENTIFIER

nc-formula: nc-formula-or-ident
     {
       auto i = res.fcache.find(*$1);
       if (i == res.fcache.end())
	 {
	   auto pf = spot::parse_infix_boolean(*$1, *res.env, debug_level(),
					       true);
	   for (auto& j: pf.errors)
	     {
	       // Adjust the diagnostic to the current position.
	       spot::location here = @1;
	       here.end.line = here.begin.line + j.first.end.line - 1;
	       here.end.column = here.begin.column + j.first.end.column - 1;
	       here.begin.line += j.first.begin.line - 1;
	       here.begin.column += j.first.begin.column - 1;
	       res.h->errors.emplace_back(here, j.second);
	     }
           bdd cond = bddfalse;
	   if (pf.f)
	     cond = spot::formula_to_bdd(pf.f,
					 res.h->aut->get_dict(), res.h->aut);
	   $$ = (res.fcache[*$1] = cond).id();
	 }
       else
	 {
	   $$ = i->second.id();
	 }
       bdd_addref($$);
       delete $1;
     }
   | "false"
     {
       $$ = 0;
     }

nc-opt-dest:
  %empty
    {
      $$ = nullptr;
    }
  | "->" "goto" IDENTIFIER
    {
      $$ = $3;
    }
  | "->" "assert" FORMULA
    {
      delete $3;
      $$ = new std::string("accept_all");
      res.accept_all_needed = true;
    }

nc-src-dest: nc-formula nc-opt-dest
    {
      // If there is no destination, do ignore the transition.
      // This happens for instance with
      //   if
      //   :: false
      //   fi
      if (!$2)
	{
	  $$ = nullptr;
	}
      else
	{
	  $$ = new pair($1, $2);
	  res.namer->new_state(*$2);
	}
    }

nc-transition:
  ':' ':' "atomic" '{' nc-src-dest '}'
    {
      $$ = $5;
    }
  | ':' ':' nc-src-dest
    {
      $$ = $3;
    }

/**********************************************************************/
/*                         Rules for LBTT                             */
/**********************************************************************/

lbtt: lbtt-header lbtt-body ENDAUT
      {
	auto& acc = res.h->aut->acc();
	unsigned num = acc.num_sets();
	res.h->aut->set_generalized_buchi(num);
	res.pos_acc_sets = acc.all_sets();
	assert(!res.states_map.empty());
	auto n = res.states_map.size();
	if (n != (unsigned) res.states)
	  {
	    std::ostringstream err;
	    err << res.states << " states have been declared, but "
		<< n << " different state numbers have been used";
	    error(@$, err.str());
	  }
	if (res.states_map.rbegin()->first > (unsigned) res.states)
	  {
	    // We have seen numbers larger that the total number of
	    // states in the automaton.  Usually this happens when the
	    // states are numbered from 1 instead of 0, but the LBTT
	    // documentation actually allow any number to be used.
	    // What we have done is to map all input state numbers 0
	    // <= .. < n to the digraph states with the same number,
	    // and any time we saw a number larger than n, we mapped
	    // it to a new state.  The correspondence is given by
	    // res.states_map.  Now we just need to remove the useless
	    // states we allocated.
	    std::vector<unsigned> rename(res.h->aut->num_states(), -1U);
	    unsigned s = 0;
	    for (auto& i: res.states_map)
	      rename[i.second] = s++;
	    assert(s == (unsigned) res.states);
	    for (auto& i: res.start)
	      i.second.front() = rename[i.second.front()];
	    res.h->aut->get_graph().defrag_states(std::move(rename), s);
	  }
	 res.info_states.resize(res.h->aut->num_states());
	 for (auto& s: res.info_states)
	   s.declared = true;
         res.h->aut->register_aps_from_dict();
      }
    | lbtt-header-states LBTT_EMPTY
      {
        res.h->aut->set_generalized_buchi($2);
	res.pos_acc_sets = res.h->aut->acc().all_sets();
      }

lbtt-header-states: LBTT
                  {
		    if (res.opts.want_kripke)
		      {
			error(@$,
			      "cannot read a Kripke structure out of "
			      "an LBTT automaton");
			YYABORT;
		      }
		    res.states = $1;
		    res.states_loc = @1;
		    res.h->aut->new_states($1);
		  }
lbtt-header: lbtt-header-states INT_S
           {
	     res.acc_mapper = new spot::acc_mapper_int(res.h->aut, $2);
	     res.acc_style = State_Acc;
	   }
           | lbtt-header-states INT
           {
	     res.acc_mapper = new spot::acc_mapper_int(res.h->aut, $2);
	     res.trans_acc_seen = true;
	   }
lbtt-body: lbtt-states
lbtt-states: %empty
           | lbtt-states lbtt-state lbtt-transitions

lbtt-state: STATE_NUM INT lbtt-acc
          {
	    if ($1 >= (unsigned) res.states)
	      {
		auto p = res.states_map.emplace($1, 0);
		if (p.second)
		  p.first->second = res.h->aut->new_state();
		res.cur_state = p.first->second;
	      }
	    else
	      {
		res.states_map.emplace($1, $1);
		res.cur_state = $1;
	      }
	    if ($2)
	      res.start.emplace_back(@1 + @2,
                                     std::vector<unsigned>{res.cur_state});
	    res.acc_state = $3;
	  }
lbtt-acc: %empty { $$ = spot::acc_cond::mark_t({}); }
        | lbtt-acc ACC
	{
	  $$  = $1;
	  auto p = res.acc_mapper->lookup($2);
	  if (p.first)
	    $$ |= p.second;
	  else
	    error(@2, "more acceptance sets used than declared");
	}
lbtt-guard: STRING
          {
	    auto pf = spot::parse_prefix_ltl(*$1, *res.env);
	    if (!pf.f || !pf.errors.empty())
	      {
		std::string s = "failed to parse guard: ";
		s += *$1;
		error(@$, s);
	      }
	    if (!pf.errors.empty())
	      for (auto& j: pf.errors)
		{
		  // Adjust the diagnostic to the current position.
		  spot::location here = @1;
		  here.end.line = here.begin.line + j.first.end.line - 1;
		  here.end.column = here.begin.column + j.first.end.column - 1;
		  here.begin.line += j.first.begin.line - 1;
		  here.begin.column += j.first.begin.column - 1;
		  res.h->errors.emplace_back(here, j.second);
		}
	    if (!pf.f)
	      {
		res.cur_label = bddtrue;
	      }
	    else
	      {
		if (!pf.f.is_boolean())
		  {
		    error(@$,
			  "non-Boolean transition label (replaced by true)");
		    res.cur_label = bddtrue;
		  }
		else
		  {
		    res.cur_label =
		      formula_to_bdd(pf.f, res.h->aut->get_dict(), res.h->aut);
		  }
	      }
	    delete $1;
	  }
lbtt-transitions: %empty
                | lbtt-transitions DEST_NUM lbtt-acc lbtt-guard
                {
		  unsigned dst = $2;
		  if (dst >= (unsigned) res.states)
		    {
		      auto p = res.states_map.emplace(dst, 0);
		      if (p.second)
			p.first->second = res.h->aut->new_state();
		      dst = p.first->second;
		    }
		  else
		    {
		      res.states_map.emplace(dst, dst);
		    }
		  res.h->aut->new_edge(res.cur_state, dst,
				       res.cur_label,
				       res.acc_state | $3);
		}

%%

static void fill_guards(result_& r)
{
  unsigned nap = r.ap.size();

  int* vars = new int[nap];
  for (unsigned i = 0; i < nap; ++i)
    vars[i] = r.ap[nap - 1 - i];

  // build the 2^nap possible guards
  r.guards.reserve(1U << nap);
  for (size_t i = 0; i < (1U << nap); ++i)
    r.guards.push_back(bdd_ibuildcube(i, nap, vars));
  r.cur_guard = r.guards.begin();

  delete[] vars;
}

void
hoayy::parser::error(const location_type& location,
		     const std::string& message)
{
  res.h->errors.emplace_back(location, message);
}

static spot::acc_cond::acc_code
fix_acceptance_aux(spot::acc_cond& acc,
		   spot::acc_cond::acc_code in, unsigned pos,
		   spot::acc_cond::mark_t onlyneg,
		   spot::acc_cond::mark_t both,
		   unsigned base)
{
  auto& w = in[pos];
  switch (w.sub.op)
    {
    case spot::acc_cond::acc_op::And:
      {
	unsigned sub = pos - w.sub.size;
	--pos;
	auto c = fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	pos -= in[pos].sub.size;
	while (sub < pos)
	  {
	    --pos;
	    c &= fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	    pos -= in[pos].sub.size;
	  }
	return c;
      }
    case spot::acc_cond::acc_op::Or:
      {
	unsigned sub = pos - w.sub.size;
	--pos;
	auto c = fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	pos -= in[pos].sub.size;
	while (sub < pos)
	  {
	    --pos;
	    c |= fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	    pos -= in[pos].sub.size;
	  }
	return c;
      }
    case spot::acc_cond::acc_op::Inf:
      return acc.inf(in[pos - 1].mark);
    case spot::acc_cond::acc_op::Fin:
      return acc.fin(in[pos - 1].mark);
    case spot::acc_cond::acc_op::FinNeg:
      {
	auto m = in[pos - 1].mark;
	auto c = acc.fin(onlyneg & m);
	spot::acc_cond::mark_t tmp = {};
	for (auto i: both.sets())
	  {
	    if (m.has(i))
	      tmp.set(base);
	    ++base;
	  }
	if (tmp)
	  c |= acc.fin(tmp);
	return c;
      }
    case spot::acc_cond::acc_op::InfNeg:
      {
	auto m = in[pos - 1].mark;
	auto c = acc.inf(onlyneg & m);
	spot::acc_cond::mark_t tmp = {};
	for (auto i: both.sets())
	  {
	    if (m.has(i))
	      tmp.set(base);
	    ++base;
	  }
	if (tmp)
	  c &= acc.inf(tmp);
	return c;
      }
    }
  SPOT_UNREACHABLE();
  return {};
}

static void fix_acceptance(result_& r)
{
  if (r.opts.want_kripke)
    return;
  auto& acc = r.h->aut->acc();

  // If a set x appears only as Inf(!x), we can complement it so that
  // we work with Inf(x) instead.
  auto onlyneg = r.neg_acc_sets - r.pos_acc_sets;
  if (onlyneg)
    {
      for (auto& t: r.h->aut->edge_vector())
	t.acc ^= onlyneg;
    }

  // However if set x is used elsewhere, for instance in
  //   Inf(!x) & Inf(x)
  // complementing x would be wrong.  We need to create a
  // new set, y, that is the complement of x, and rewrite
  // this as Inf(y) & Inf(x).
  auto both = r.neg_acc_sets & r.pos_acc_sets;
  unsigned base = 0;
  if (both)
    {
      base = acc.add_sets(both.count());
      for (auto& t: r.h->aut->edge_vector())
        {
          unsigned i = 0;
	  if ((t.acc & both) != both)
            for (unsigned v : both.sets())
              {
                if (!t.acc.has(v))
                  t.acc |= acc.mark(base + i);
                i++;
              }
        }
    }

  if (onlyneg || both)
    {
      auto& acc = r.h->aut->acc();
      auto code = acc.get_acceptance();
      r.h->aut->set_acceptance(acc.num_sets(),
			       fix_acceptance_aux(acc, code, code.size() - 1,
						  onlyneg, both, base));
    }
}

// Spot only supports a single initial state.
//
// If the input file does not declare any initial state (this is valid
// in the HOA format) add one nonetheless.
//
// If the input file has multiple initial states we have to merge
// them.
//
//   1) In the non-alternating case, this is as simple as making a new
//   initial state using the union of all the outgoing transitions of
//   the declared initial states.  Note that if one of the original
//   initial state has no incoming transition, then we can use it as
//   initial state, avoiding the creation of a new state.
//
//   2) In the alternating case, the input may have several initial
//   states that are conjuncts.  We have to reduce the conjuncts to a
//   single state first.

static void fix_initial_state(result_& r)
{
  std::vector<std::vector<unsigned>> start;
  start.reserve(r.start.size());
  unsigned ssz = r.info_states.size();
  for (auto& p : r.start)
    {
      std::vector<unsigned> v;
      v.reserve(p.second.size());
      for (unsigned s: p.second)
        // Ignore initial states without declaration
        // (They have been diagnosed already.)
        if (s < ssz && r.info_states[s].declared)
          v.emplace_back(s);
      if (!v.empty())
        start.push_back(v);
    }

  // If no initial state has been declared, add one.
  if (start.empty())
    {
      if (r.opts.want_kripke)
	r.h->ks->set_init_state(r.h->ks->new_state(bddfalse));
      else
	r.h->aut->set_init_state(r.h->aut->new_state());
      return;
    }

  // Remove any duplicate initial state.
  std::sort(start.begin(), start.end());
  auto res = std::unique(start.begin(), start.end());
  start.resize(std::distance(start.begin(), res));

  assert(start.size() >= 1);
  if (start.size() == 1)
    {
      if (r.opts.want_kripke)
	r.h->ks->set_init_state(start.front().front());
      else
	r.h->aut->set_univ_init_state(start.front().begin(),
                                      start.front().end());
    }
  else
    {
      if (r.opts.want_kripke)
	{
	  r.h->errors.emplace_front(r.start.front().first,
				    "Kripke structure only support "
				    "a single initial state");
	  return;
	}
      // Fiddling with initial state may turn an incomplete automaton
      // into a complete one.
      if (r.complete.is_false())
        r.complete = spot::trival::maybe();
      // Multiple initial states.  We might need to add a fake one,
      // unless one of the actual initial state has no incoming edge.
      auto& aut = r.h->aut;
      std::vector<unsigned char> has_incoming(aut->num_states(), 0);
      for (auto& t: aut->edges())
        for (unsigned ud: aut->univ_dests(t))
          has_incoming[ud] = 1;

      bool found = false;
      unsigned init = 0;
      bool init_alternation = false;
      for (auto& pp: start)
        if (pp.size() == 1)
          {
            unsigned p = pp.front();
            if (!has_incoming[p])
              {
                init = p;
                found = true;
              }
          }
        else
          {
            init_alternation = true;
            break;
          }

      if (!found || init_alternation)
	// We do need a fake initial state
	init = aut->new_state();
      aut->set_init_state(init);

      // The non-alternating case is the easiest, we simply declare
      // the outgoing transitions of all "original initial states"
      // into the only one initial state.
      if (!init_alternation)
        {
          for (auto& pp: start)
            {
              unsigned p = pp.front();
              if (p != init)
                for (auto& t: aut->out(p))
                  aut->new_edge(init, t.dst, t.cond);
            }
        }
      else
        {
          // In the alternating case, we merge outgoing transition of
          // the universal destination of conjunct initial states.
          // (Note that this loop would work for the non-alternating
          // case too, but it is more expansive, so we avoid it if we
          // can.)
          spot::outedge_combiner combiner(aut);
          bdd comb_or = bddfalse;
          for (auto& pp: start)
            {
              bdd comb_and = bddtrue;
              for (unsigned d: pp)
                comb_and &= combiner(d);
              comb_or |= comb_and;
            }
          combiner.new_dests(init, comb_or);
        }
    }
}

static void fix_properties(result_& r)
{
  r.aut_or_ks->prop_universal(r.universal);
  r.aut_or_ks->prop_complete(r.complete);
  if (r.acc_style == State_Acc ||
      (r.acc_style == Mixed_Acc && !r.trans_acc_seen))
    r.aut_or_ks->prop_state_acc(true);
}

static void check_version(const result_& r)
{
  if (r.h->type != spot::parsed_aut_type::HOA)
    return;
  auto& v = r.format_version;
  if (v.size() < 2 || v[0] != 'v' || v[1] < '1' || v[1] > '9')
    {
      r.h->errors.emplace_front(r.format_version_loc, "unknown HOA version");
      return;
    }
  const char* beg = &v[1];
  char* end = nullptr;
  long int vers = strtol(beg, &end, 10);
  if (vers != 1)
    {
      r.h->errors.emplace_front(r.format_version_loc,
				  "unsupported HOA version");
      return;
    }
  constexpr const char supported[] = "1";
  if (strverscmp(supported, beg) < 0 && !r.h->errors.empty())
    {
      std::ostringstream s;
      s << "we can read HOA v" << supported
	<< " but this file uses " << v << "; this might "
	"cause the following errors";
      r.h->errors.emplace_front(r.format_version_loc, s.str());
      return;
    }
}

namespace spot
{
  automaton_stream_parser::automaton_stream_parser(const std::string& name,
						   automaton_parser_options opt)
  try
    : filename_(name), opts_(opt)
  {
    if (hoayyopen(name, &scanner_))
      throw std::runtime_error("Cannot open file "s + name);
  }
  catch (...)
  {
    hoayyclose(scanner_);
    throw;
  }

  automaton_stream_parser::automaton_stream_parser(int fd,
						   const std::string& name,
						   automaton_parser_options opt)
  try
    : filename_(name), opts_(opt)
  {
    if (hoayyopen(fd, &scanner_))
      throw std::runtime_error("Cannot open file "s + name);
  }
  catch (...)
  {
    hoayyclose(scanner_);
    throw;
  }

  automaton_stream_parser::automaton_stream_parser(const char* data,
						   const std::string& filename,
						   automaton_parser_options opt)
  try
    : filename_(filename), opts_(opt)
  {
    hoayystring(data, &scanner_);
  }
  catch (...)
  {
    hoayyclose(scanner_);
    throw;
  }

  automaton_stream_parser::~automaton_stream_parser()
  {
    hoayyclose(scanner_);
  }

  static void raise_parse_error(const parsed_aut_ptr& pa)
  {
    if (pa->aborted)
      pa->errors.emplace_back(pa->loc, "parsing aborted");
    if (!pa->errors.empty())
      {
	std::ostringstream s;
	if (pa->format_errors(s))
	  throw parse_error(s.str());
      }
    // It is possible that pa->aut == nullptr if we reach the end of a
    // stream.  It is not necessarily an error.
  }

  parsed_aut_ptr
  automaton_stream_parser::parse(const bdd_dict_ptr& dict,
				 environment& env)
  {
  restart:
    result_ r;
    r.opts = opts_;
    r.h = std::make_shared<spot::parsed_aut>(filename_);
    if (opts_.want_kripke)
      r.aut_or_ks = r.h->ks = make_kripke_graph(dict);
    else
      r.aut_or_ks = r.h->aut = make_twa_graph(dict);
    r.env = &env;
    hoayy::parser parser(scanner_, r, last_loc);
    static bool env_debug = !!getenv("SPOT_DEBUG_PARSER");
    parser.set_debug_level(opts_.debug || env_debug);
    hoayyreset(scanner_);
    try
      {
	if (parser.parse())
	  {
	    r.h->aut = nullptr;
	    r.h->ks = nullptr;
	    r.aut_or_ks = nullptr;
	  }
      }
    catch (const spot::hoa_abort& e)
      {
	r.h->aborted = true;
	// Bison 3.0.2 lacks a += operator for locations.
	r.h->loc = r.h->loc + e.pos;
      }
    check_version(r);
    last_loc = r.h->loc;
    last_loc.step();
    if (r.h->aborted)
      {
	if (opts_.ignore_abort)
	  goto restart;
	return r.h;
      }
    if (opts_.raise_errors)
      raise_parse_error(r.h);
    if (!r.aut_or_ks)
      return r.h;
    if (r.state_names)
      r.aut_or_ks->set_named_prop("state-names", r.state_names);
    if (r.highlight_edges)
      r.aut_or_ks->set_named_prop("highlight-edges", r.highlight_edges);
    if (r.highlight_states)
      r.aut_or_ks->set_named_prop("highlight-states", r.highlight_states);
    fix_acceptance(r);
    fix_initial_state(r);
    fix_properties(r);
    if (r.h->aut && !r.h->aut->is_existential())
      r.h->aut->merge_univ_dests();
    return r.h;
  }

  parsed_aut_ptr
  parse_aut(const std::string& filename, const bdd_dict_ptr& dict,
	    environment& env, automaton_parser_options opts)
  {
    auto localopts = opts;
    localopts.raise_errors = false;
    parsed_aut_ptr pa;
    try
      {
	automaton_stream_parser p(filename, localopts);
	pa = p.parse(dict, env);
      }
    catch (const std::runtime_error& e)
      {
	if (opts.raise_errors)
	  throw;
	parsed_aut_ptr pa = std::make_shared<spot::parsed_aut>(filename);
	pa->errors.emplace_back(spot::location(), e.what());
	return pa;
      }
    if (!pa->aut && !pa->ks && pa->errors.empty())
      pa->errors.emplace_back(pa->loc, "no automaton read (empty input?)");
    if (opts.raise_errors)
      raise_parse_error(pa);
    return pa;
  }


}

// Local Variables:
// mode: c++
// End:
