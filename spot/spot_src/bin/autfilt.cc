// -*- coding: utf-8 -*-
// Copyright (C) 2013-2020 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "common_sys.hh"

#include <string>
#include <iostream>
#include <limits>
#include <set>
#include <memory>
#include <sys/stat.h>
#include <unistd.h>

#include <argp.h>
#include "error.h"
#include "argmatch.h"

#include "common_setup.hh"
#include "common_finput.hh"
#include "common_cout.hh"
#include "common_aoutput.hh"
#include "common_range.hh"
#include "common_post.hh"
#include "common_conv.hh"
#include "common_hoaread.hh"

#include <spot/misc/optionmap.hh>
#include <spot/misc/random.hh>
#include <spot/misc/timer.hh>
#include <spot/parseaut/public.hh>
#include <spot/tl/exclusive.hh>
#include <spot/twaalgos/are_isomorphic.hh>
#include <spot/twaalgos/canonicalize.hh>
#include <spot/twaalgos/cobuchi.hh>
#include <spot/twaalgos/cleanacc.hh>
#include <spot/twaalgos/complement.hh>
#include <spot/twaalgos/contains.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/dtwasat.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/gtec/gtec.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/iscolored.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/isunamb.hh>
#include <spot/twaalgos/isweakscc.hh>
#include <spot/twaalgos/langmap.hh>
#include <spot/twaalgos/mask.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/randomize.hh>
#include <spot/twaalgos/remfin.hh>
#include <spot/twaalgos/remprop.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/sepsets.hh>
#include <spot/twaalgos/split.hh>
#include <spot/twaalgos/strength.hh>
#include <spot/twaalgos/stripacc.hh>
#include <spot/twaalgos/stutter.hh>
#include <spot/twaalgos/sum.hh>
#include <spot/twaalgos/totgba.hh>

static const char argp_program_doc[] ="\
Convert, transform, and filter omega-automata.\v\
Exit status:\n\
  0  if some automata were output\n\
  1  if no automata were output (no match)\n\
  2  if any error has been reported";

// Keep this list sorted
enum {
  OPT_ACC_SCCS = 256,
  OPT_ACC_SETS,
  OPT_ACCEPT_WORD,
  OPT_ACCEPTANCE_IS,
  OPT_AP_N,
  OPT_ARE_ISOMORPHIC,
  OPT_CLEAN_ACC,
  OPT_CNF_ACC,
  OPT_COMPLEMENT,
  OPT_COMPLEMENT_ACC,
  OPT_COUNT,
  OPT_DCA,
  OPT_DECOMPOSE_SCC,
  OPT_DESTUT,
  OPT_DUALIZE,
  OPT_DNF_ACC,
  OPT_EDGES,
  OPT_EQUIVALENT_TO,
  OPT_EXCLUSIVE_AP,
  OPT_GENERALIZED_RABIN,
  OPT_GENERALIZED_STREETT,
  OPT_HAS_EXIST_BRANCHING,
  OPT_HAS_UNIV_BRANCHING,
  OPT_HIGHLIGHT_NONDET,
  OPT_HIGHLIGHT_NONDET_EDGES,
  OPT_HIGHLIGHT_NONDET_STATES,
  OPT_HIGHLIGHT_WORD,
  OPT_HIGHLIGHT_ACCEPTING_RUN,
  OPT_HIGHLIGHT_LANGUAGES,
  OPT_INSTUT,
  OPT_INCLUDED_IN,
  OPT_INHERENTLY_WEAK_SCCS,
  OPT_INTERSECT,
  OPT_IS_ALTERNATING,
  OPT_IS_COLORED,
  OPT_IS_COMPLETE,
  OPT_IS_DETERMINISTIC,
  OPT_IS_EMPTY,
  OPT_IS_INHERENTLY_WEAK,
  OPT_IS_SEMI_DETERMINISTIC,
  OPT_IS_STUTTER_INVARIANT,
  OPT_IS_TERMINAL,
  OPT_IS_UNAMBIGUOUS,
  OPT_IS_VERY_WEAK,
  OPT_IS_WEAK,
  OPT_KEEP_STATES,
  OPT_MASK_ACC,
  OPT_MERGE,
  OPT_NONDET_STATES,
  OPT_PARTIAL_DEGEN,
  OPT_PRODUCT_AND,
  OPT_PRODUCT_OR,
  OPT_RANDOMIZE,
  OPT_REJ_SCCS,
  OPT_REJECT_WORD,
  OPT_REM_AP,
  OPT_REM_DEAD,
  OPT_REM_UNREACH,
  OPT_REM_UNUSED_AP,
  OPT_REM_FIN,
  OPT_SAT_MINIMIZE,
  OPT_SCCS,
  OPT_SEED,
  OPT_SEP_SETS,
  OPT_SIMPL_ACC,
  OPT_SIMPLIFY_EXCLUSIVE_AP,
  OPT_SPLIT_EDGES,
  OPT_STATES,
  OPT_STREETT_LIKE,
  OPT_STRIPACC,
  OPT_SUM_OR,
  OPT_SUM_AND,
  OPT_TERMINAL_SCCS,
  OPT_TRIV_SCCS,
  OPT_USED_AP_N,
  OPT_UNUSED_AP_N,
  OPT_WEAK_SCCS,
};

#define DOC(NAME, TXT) NAME, 0, nullptr, OPTION_DOC | OPTION_NO_USAGE, TXT, 0

static const argp_option options[] =
  {
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Input:", 1 },
    { "file", 'F', "FILENAME", 0,
      "process the automaton in FILENAME", 0 },
    /**************************************************/
    { "count", 'c', nullptr, 0, "print only a count of matched automata", 3 },
    { "max-count", 'n', "NUM", 0, "output at most NUM automata", 3 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Filtering options:", 5 },
    { "ap", OPT_AP_N, "RANGE", 0,
      "match automata with a number of (declared) atomic propositions in RANGE",
      0 },
    { "used-ap", OPT_USED_AP_N, "RANGE", 0,
      "match automata with a number of used atomic propositions in RANGE", 0 },
    { "unused-ap", OPT_UNUSED_AP_N, "RANGE", 0,
      "match automata with a number of declared, but unused atomic "
      "propositions in RANGE", 0 },
    { "acceptance-is", OPT_ACCEPTANCE_IS, "NAME|FORMULA", 0,
      "match automata with given acceptance condition", 0 },
    { "are-isomorphic", OPT_ARE_ISOMORPHIC, "FILENAME", 0,
      "keep automata that are isomorphic to the automaton in FILENAME", 0 },
    { "isomorphic", 0, nullptr, OPTION_ALIAS | OPTION_HIDDEN, nullptr, 0 },
    { "unique", 'u', nullptr, 0,
      "do not output the same automaton twice (same in the sense that they "
      "are isomorphic)", 0 },
    { "has-exist-branching", OPT_HAS_EXIST_BRANCHING, nullptr, 0,
      "keep automata that use existential branching (i.e., make "
      "non-deterministic choices)", 0 },
    { "has-univ-branching", OPT_HAS_UNIV_BRANCHING, nullptr, 0,
      "keep alternating automata that use universal branching", 0 },
    { "is-colored", OPT_IS_COLORED, nullptr, 0,
      "keep colored automata (i.e., exactly one acceptance mark per "
      "transition or state)", 0 },
    { "is-complete", OPT_IS_COMPLETE, nullptr, 0,
      "keep complete automata", 0 },
    { "is-deterministic", OPT_IS_DETERMINISTIC, nullptr, 0,
      "keep deterministic automata", 0 },
    { "is-semi-deterministic", OPT_IS_SEMI_DETERMINISTIC, nullptr, 0,
      "keep semi-deterministic automata", 0 },
    { "is-empty", OPT_IS_EMPTY, nullptr, 0,
      "keep automata with an empty language", 0 },
    { "is-stutter-invariant", OPT_IS_STUTTER_INVARIANT, nullptr, 0,
      "keep automata representing stutter-invariant properties", 0 },
    { "is-terminal", OPT_IS_TERMINAL, nullptr, 0,
      "keep only terminal automata", 0 },
    { "is-unambiguous", OPT_IS_UNAMBIGUOUS, nullptr, 0,
      "keep only unambiguous automata", 0 },
    { "is-weak", OPT_IS_WEAK, nullptr, 0,
      "keep only weak automata", 0 },
    { "is-inherently-weak", OPT_IS_INHERENTLY_WEAK, nullptr, 0,
      "keep only inherently weak automata", 0 },
    { "is-very-weak", OPT_IS_VERY_WEAK, nullptr, 0,
      "keep only very-weak automata", 0 },
    { "is-alternating", OPT_IS_ALTERNATING, nullptr, 0,
      "keep only automata using universal branching", 0 },
    { "intersect", OPT_INTERSECT, "FILENAME", 0,
      "keep automata whose languages have an non-empty intersection with"
      " the automaton from FILENAME", 0 },
    { "included-in", OPT_INCLUDED_IN, "FILENAME", 0,
      "keep automata whose languages are included in that of the "
      "automaton from FILENAME", 0 },
    { "equivalent-to", OPT_EQUIVALENT_TO, "FILENAME", 0,
      "keep automata that are equivalent (language-wise) to the automaton "
      "in FILENAME", 0 },
    { "invert-match", 'v', nullptr, 0, "select non-matching automata", 0 },
    { "states", OPT_STATES, "RANGE", 0,
      "keep automata whose number of states is in RANGE", 0 },
    { "edges", OPT_EDGES, "RANGE", 0,
      "keep automata whose number of edges is in RANGE", 0 },
    { "nondet-states", OPT_NONDET_STATES, "RANGE", 0,
      "keep automata whose number of nondeterministic states is in RANGE", 0 },
    { "acc-sets", OPT_ACC_SETS, "RANGE", 0,
      "keep automata whose number of acceptance sets is in RANGE", 0 },
    { "sccs", OPT_SCCS, "RANGE", 0,
      "keep automata whose number of SCCs is in RANGE", 0 },
    { "acc-sccs", OPT_ACC_SCCS, "RANGE", 0,
      "keep automata whose number of non-trivial accepting SCCs is in RANGE",
      0 },
    { "accepting-sccs", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "rej-sccs", OPT_REJ_SCCS, "RANGE", 0,
      "keep automata whose number of non-trivial rejecting SCCs is in RANGE",
      0 },
    { "rejecting-sccs", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "triv-sccs", OPT_TRIV_SCCS, "RANGE", 0,
      "keep automata whose number of trivial SCCs is in RANGE", 0 },
    { "trivial-sccs", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "inherently-weak-sccs", OPT_INHERENTLY_WEAK_SCCS, "RANGE", 0,
      "keep automata whose number of accepting inherently-weak SCCs is in "
      "RANGE.  An accepting SCC is inherently weak if it does not have a "
      "rejecting cycle.", 0 },
    { "weak-sccs", OPT_WEAK_SCCS, "RANGE", 0,
      "keep automata whose number of accepting weak SCCs is in RANGE.  "
      "In a weak SCC, all transitions belong to the same acceptance sets.", 0 },
    { "terminal-sccs", OPT_TERMINAL_SCCS, "RANGE", 0,
      "keep automata whose number of accepting terminal SCCs is in RANGE.  "
      "Terminal SCCs are weak and complete.", 0 },
    { "accept-word", OPT_ACCEPT_WORD, "WORD", 0,
      "keep automata that accept WORD", 0 },
    { "reject-word", OPT_REJECT_WORD, "WORD", 0,
      "keep automata that reject WORD", 0 },
    { "nth", 'N', "RANGE", 0,
      "assuming input automata are numbered from 1, keep only those in RANGE",
      0 },
    /**************************************************/
    RANGE_DOC_FULL,
    WORD_DOC,
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Transformations:", 7 },
    { "merge-transitions", OPT_MERGE, nullptr, 0,
      "merge transitions with same destination and acceptance", 0 },
    { "product", OPT_PRODUCT_AND, "FILENAME", 0,
      "build the product with the automaton in FILENAME "
      "to intersect languages", 0 },
    { "product-and", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "product-or", OPT_PRODUCT_OR, "FILENAME", 0,
      "build the product with the automaton in FILENAME "
      "to sum languages", 0 },
    { "randomize", OPT_RANDOMIZE, "s|t", OPTION_ARG_OPTIONAL,
      "randomize states and transitions (specify 's' or 't' to "
      "randomize only states or transitions)", 0 },
    { "instut", OPT_INSTUT, "1|2", OPTION_ARG_OPTIONAL,
      "allow more stuttering (two possible algorithms)", 0 },
    { "destut", OPT_DESTUT, nullptr, 0, "allow less stuttering", 0 },
    { "mask-acc", OPT_MASK_ACC, "NUM[,NUM...]", 0,
      "remove all transitions in specified acceptance sets", 0 },
    { "strip-acceptance", OPT_STRIPACC, nullptr, 0,
      "remove the acceptance condition and all acceptance sets", 0 },
    { "keep-states", OPT_KEEP_STATES, "NUM[,NUM...]", 0,
      "only keep specified states.  The first state will be the new "\
      "initial state.  Implies --remove-unreachable-states.", 0 },
    { "dnf-acceptance", OPT_DNF_ACC, nullptr, 0,
      "put the acceptance condition in Disjunctive Normal Form", 0 },
    { "streett-like", OPT_STREETT_LIKE, nullptr, 0,
      "convert to an automaton with Streett-like acceptance. Works only with "
      "acceptance condition in DNF", 0 },
    { "cnf-acceptance", OPT_CNF_ACC, nullptr, 0,
      "put the acceptance condition in Conjunctive Normal Form", 0 },
    { "remove-fin", OPT_REM_FIN, nullptr, 0,
      "rewrite the automaton without using Fin acceptance", 0 },
    { "generalized-rabin", OPT_GENERALIZED_RABIN,
      "unique-inf|share-inf", OPTION_ARG_OPTIONAL,
      "rewrite the acceptance condition as generalized Rabin; the default "
      "\"unique-inf\" option uses the generalized Rabin definition from the "
      "HOA format; the \"share-inf\" option allows clauses to share Inf sets, "
      "therefore reducing the number of sets", 0 },
    { "gra", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "generalized-streett", OPT_GENERALIZED_STREETT,
      "unique-fin|share-fin", OPTION_ARG_OPTIONAL,
      "rewrite the acceptance condition as generalized Streett;"
      " the \"share-fin\" option allows clauses to share Fin sets,"
      " therefore reducing the number of sets; the default"
      " \"unique-fin\" does not", 0 },
    { "gsa", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "cleanup-acceptance", OPT_CLEAN_ACC, nullptr, 0,
      "remove unused acceptance sets from the automaton", 0 },
    { "complement", OPT_COMPLEMENT, nullptr, 0,
      "complement each automaton (different strategies are used)", 0 },
    { "complement-acceptance", OPT_COMPLEMENT_ACC, nullptr, 0,
      "complement the acceptance condition (without touching the automaton)",
      0 },
    { "decompose-scc", OPT_DECOMPOSE_SCC, "t|w|s|N|aN", 0,
      "extract the (t) terminal, (w) weak, or (s) strong part of an automaton"
      " or (N) the subautomaton leading to the Nth SCC, or (aN) to the Nth "
      "accepting SCC (option can be combined with commas to extract multiple "
      "parts)", 0 },
    { "decompose-strength", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "dualize", OPT_DUALIZE, nullptr, 0,
      "dualize each automaton", 0 },
    { "exclusive-ap", OPT_EXCLUSIVE_AP, "AP,AP,...", 0,
      "if any of those APs occur in the automaton, restrict all edges to "
      "ensure two of them may not be true at the same time.  Use this option "
      "multiple times to declare independent groups of exclusive "
      "propositions.", 0 },
    { "simplify-exclusive-ap", OPT_SIMPLIFY_EXCLUSIVE_AP, nullptr, 0,
      "if --exclusive-ap is used, assume those AP groups are actually exclusive"
      " in the system to simplify the expression of transition labels (implies "
      "--merge-transitions)", 0 },
    { "remove-ap", OPT_REM_AP, "AP[=0|=1][,AP...]", 0,
      "remove atomic propositions either by existential quantification, or "
      "by assigning them 0 or 1", 0 },
    { "remove-unused-ap", OPT_REM_UNUSED_AP, nullptr, 0,
      "remove declared atomic propositions that are not used", 0 },
    { "remove-unreachable-states", OPT_REM_UNREACH, nullptr, 0,
      "remove states that are unreachable from the initial state", 0 },
    { "remove-dead-states", OPT_REM_DEAD, nullptr, 0,
      "remove states that are unreachable, or that cannot belong to an "
      "infinite path", 0 },
    { "simplify-acceptance", OPT_SIMPL_ACC, nullptr, 0,
      "simplify the acceptance condition by merging identical acceptance sets "
      "and by simplifying some terms containing complementary sets", 0 },
    { "split-edges", OPT_SPLIT_EDGES, nullptr, 0,
      "split edges into transitions labeled by conjunctions of all atomic "
      "propositions, so they can be read as letters", 0 },
    { "sum", OPT_SUM_OR, "FILENAME", 0,
      "build the sum with the automaton in FILENAME "
      "to sum languages", 0 },
    { "sum-or", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "sum-and", OPT_SUM_AND, "FILENAME", 0,
      "build the sum with the automaton in FILENAME "
      "to intersect languages", 0 },
    { "partial-degeneralize", OPT_PARTIAL_DEGEN, "NUM1,NUM2,...",
      OPTION_ARG_OPTIONAL, "Degeneralize automata according to sets "
      "NUM1,NUM2,... If no sets are given, partial degeneralization "
      "is performed for all conjunctions of Inf and disjunctions of Fin.", 0 },
    { "separate-sets", OPT_SEP_SETS, nullptr, 0,
      "if both Inf(x) and Fin(x) appear in the acceptance condition, replace "
      "Fin(x) by a new Fin(y) and adjust the automaton", 0 },
    { "sat-minimize", OPT_SAT_MINIMIZE, "options", OPTION_ARG_OPTIONAL,
      "minimize the automaton using a SAT solver (only works for deterministic"
      " automata). Supported options are acc=STRING, states=N, max-states=N, "
      "sat-incr=N, sat-incr-steps=N, sat-langmap, sat-naive, colored, preproc=N"
      ". Spot uses by default its PicoSAT distribution but an external SAT"
      "solver can be set thanks to the SPOT_SATSOLVER environment variable"
      "(see spot-x)."
      , 0 },
    { nullptr, 0, nullptr, 0, "Decorations (for -d and -H1.1 output):", 9 },
    { "highlight-accepting-run", OPT_HIGHLIGHT_ACCEPTING_RUN, "NUM",
      OPTION_ARG_OPTIONAL, "highlight one accepting run using color NUM", 0},
    { "highlight-nondet-states", OPT_HIGHLIGHT_NONDET_STATES, "NUM",
      OPTION_ARG_OPTIONAL, "highlight nondeterministic states with color NUM",
      0 },
    { "highlight-nondet-edges", OPT_HIGHLIGHT_NONDET_EDGES, "NUM",
      OPTION_ARG_OPTIONAL, "highlight nondeterministic edges with color NUM",
      0 },
    { "highlight-nondet", OPT_HIGHLIGHT_NONDET, "NUM",
      OPTION_ARG_OPTIONAL,
      "highlight nondeterministic states and edges with color NUM", 0},
    { "highlight-word", OPT_HIGHLIGHT_WORD, "[NUM,]WORD", 0,
      "highlight one run matching WORD using color NUM", 0},
    { "highlight-languages", OPT_HIGHLIGHT_LANGUAGES, nullptr, 0 ,
      "highlight states that recognize identical languages", 0},
    /**************************************************/
    { nullptr, 0, nullptr, 0,
      "If any option among --small, --deterministic, or --any is given, "
      "then the simplification level defaults to --high unless specified "
      "otherwise.  If any option among --low, --medium, or --high is given, "
      "then the simplification goal defaults to --small unless specified "
      "otherwise.  If none of those options are specified, then autfilt "
      "acts as is --any --low were given: these actually disable the "
      "simplification routines.", 22 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Miscellaneous options:", -1 },
    { "extra-options", 'x', "OPTS", 0,
      "fine-tuning options (see spot-x (7))", 0 },
    { "seed", OPT_SEED, "INT", 0,
      "seed for the random number generator (0)", 0 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

static const struct argp_child children[] =
  {
    { &hoaread_argp, 0, nullptr, 0 },
    { &aoutput_argp, 0, nullptr, 0 },
    { &aoutput_io_format_argp, 0, nullptr, 4 },
    { &post_argp_disabled, 0, nullptr, 0 },
    { &misc_argp, 0, nullptr, -1 },
    { nullptr, 0, nullptr, 0 }
  };


struct canon_aut
{
  typedef spot::twa_graph::graph_t::edge_storage_t tr_t;
  unsigned num_states;
  std::vector<tr_t> edges;
  std::string acc;

  canon_aut(const spot::const_twa_graph_ptr& aut)
    : num_states(aut->num_states())
    , edges(aut->edge_vector().begin() + 1,
            aut->edge_vector().end())
  {
    std::ostringstream os;
    aut->get_acceptance().to_text(os);
    acc = os.str();
  }

  bool operator<(const canon_aut& o) const
  {
    return std::tie(num_states, edges, acc)
      < std::tie(o.num_states, o.edges, o.acc);
  };
};

typedef std::set<canon_aut> unique_aut_t;
static long int match_count = 0;
static spot::option_map extra_options;
static bool randomize_st = false;
static bool randomize_tr = false;
static int opt_seed = 0;

enum gra_type { GRA_NO = 0, GRA_SHARE_INF = 1, GRA_UNIQUE_INF = 2 };
static gra_type opt_gra = GRA_NO;
static char const *const gra_args[] =
{
  "default", "share-inf", "hoa", "unique-inf", nullptr
};
static gra_type const gra_types[] =
{
  GRA_UNIQUE_INF, GRA_SHARE_INF, GRA_UNIQUE_INF, GRA_UNIQUE_INF
};
ARGMATCH_VERIFY(gra_args, gra_types);

enum acc_type {
  ACC_Any = 0,
  ACC_Given,
  ACC_tt,
  ACC_ff,
  ACC_Buchi,
  ACC_GenBuchi,
  ACC_CoBuchi,
  ACC_GenCoBuchi,
  ACC_Rabin,
  ACC_Streett,
  ACC_GenRabin,
  ACC_GenStreett,
  ACC_RabinLike,
  ACC_StreettLike,
  ACC_Parity,
  ACC_ParityMin,
  ACC_ParityMax,
  ACC_ParityOdd,
  ACC_ParityEven,
  ACC_ParityMinOdd,
  ACC_ParityMaxOdd,
  ACC_ParityMinEven,
  ACC_ParityMaxEven,
  ACC_FinLess,
};
static acc_type opt_acceptance_is = ACC_Any;
spot::acc_cond opt_acceptance_is_given;
static char const *const acc_is_args[] =
  {
    "any",
    "t", "all",
    "f", "none",
    "Buchi", "Büchi",
    "generalized-Buchi", "generalized-Büchi",
    "genBuchi", "genBüchi", "gen-Buchi", "gen-Büchi",
    "coBuchi", "coBüchi", "co-Buchi", "co-Büchi",
    "generalized-coBuchi", "generalized-coBüchi",
    "generalized-co-Buchi", "generalized-co-Büchi",
    "gen-coBuchi", "gen-coBüchi",
    "gen-co-Buchi", "gen-co-Büchi",
    "gencoBuchi", "gencoBüchi",
    "Rabin",
    "Streett",
    "generalized-Rabin", "gen-Rabin", "genRabin",
    "generalized-Streett", "gen-Streett", "genStreett",
    "Streett-like",
    "Rabin-like",
    "parity",
    "parity min", "parity-min",
    "parity max", "parity-max",
    "parity odd", "parity-odd",
    "parity even", "parity-even",
    "parity min even", "parity-min-even",
    "parity min odd", "parity-min-odd",
    "parity max even", "parity-max-even",
    "parity max odd", "parity-max-odd",
    "Fin-less", "Finless",
    nullptr,
  };
static acc_type const acc_is_types[] =
  {
    ACC_Any,
    ACC_tt, ACC_tt,
    ACC_ff, ACC_ff,
    ACC_Buchi, ACC_Buchi,
    ACC_GenBuchi, ACC_GenBuchi,
    ACC_GenBuchi, ACC_GenBuchi, ACC_GenBuchi, ACC_GenBuchi,
    ACC_CoBuchi, ACC_CoBuchi, ACC_CoBuchi, ACC_CoBuchi,
    ACC_GenCoBuchi, ACC_GenCoBuchi,
    ACC_GenCoBuchi, ACC_GenCoBuchi,
    ACC_GenCoBuchi, ACC_GenCoBuchi,
    ACC_GenCoBuchi, ACC_GenCoBuchi,
    ACC_GenCoBuchi, ACC_GenCoBuchi,
    ACC_Rabin,
    ACC_Streett,
    ACC_GenRabin, ACC_GenRabin, ACC_GenRabin,
    ACC_GenStreett, ACC_GenStreett, ACC_GenStreett,
    ACC_StreettLike,
    ACC_RabinLike,
    ACC_Parity,
    ACC_ParityMin, ACC_ParityMin,
    ACC_ParityMax, ACC_ParityMax,
    ACC_ParityOdd, ACC_ParityOdd,
    ACC_ParityEven, ACC_ParityEven,
    ACC_ParityMinEven, ACC_ParityMinEven,
    ACC_ParityMinOdd, ACC_ParityMinOdd,
    ACC_ParityMaxEven, ACC_ParityMaxEven,
    ACC_ParityMaxOdd, ACC_ParityMaxOdd,
    ACC_FinLess, ACC_FinLess,
  };
ARGMATCH_VERIFY(acc_is_args, acc_is_types);

enum gsa_type { GSA_NO = 0, GSA_SHARE_FIN = 1, GSA_UNIQUE_FIN = 2 };
static gsa_type opt_gsa = GSA_NO;
static char const *const gsa_args[] =
{
  "default", "share-fin", "unique-fin", nullptr
};
static gsa_type const gsa_types[] =
{
  GSA_UNIQUE_FIN, GSA_SHARE_FIN, GSA_UNIQUE_FIN
};
ARGMATCH_VERIFY(gsa_args, gsa_types);

// We want all these variables to be destroyed when we exit main, to
// make sure it happens before all other global variables (like the
// atomic propositions maps) are destroyed.  Otherwise we risk
// accessing deleted stuff.
static struct opt_t
{
  spot::bdd_dict_ptr dict = spot::make_bdd_dict();
  spot::twa_graph_ptr product_and = nullptr;
  spot::twa_graph_ptr product_or = nullptr;
  spot::twa_graph_ptr sum_and = nullptr;
  spot::twa_graph_ptr sum_or = nullptr;
  spot::twa_graph_ptr intersect = nullptr;
  spot::twa_graph_ptr included_in = nullptr;
  spot::twa_graph_ptr equivalent_pos = nullptr;
  spot::twa_graph_ptr equivalent_neg = nullptr;
  spot::twa_graph_ptr are_isomorphic = nullptr;
  std::unique_ptr<spot::isomorphism_checker>
                         isomorphism_checker = nullptr;
  std::unique_ptr<unique_aut_t> uniq = nullptr;
  spot::exclusive_ap excl_ap;
  spot::remove_ap rem_ap;
  std::vector<spot::twa_graph_ptr> acc_words;
  std::vector<spot::twa_graph_ptr> rej_words;
  std::vector<std::pair<spot::twa_graph_ptr, unsigned>> hl_words;
}* opt;

static bool opt_merge = false;
static bool opt_has_univ_branching = false;
static bool opt_has_exist_branching = false;
static bool opt_is_alternating = false;
static bool opt_is_colored = false;
static bool opt_is_complete = false;
static bool opt_is_deterministic = false;
static bool opt_is_semi_deterministic = false;
static bool opt_is_unambiguous = false;
static bool opt_is_terminal = false;
static bool opt_is_weak = false;
static bool opt_is_inherently_weak = false;
static bool opt_is_very_weak = false;
static bool opt_is_stutter_invariant = false;
static bool opt_invert = false;
static range opt_states = { 0, std::numeric_limits<int>::max() };
static range opt_edges = { 0, std::numeric_limits<int>::max() };
static range opt_accsets = { 0, std::numeric_limits<int>::max() };
static range opt_ap_n = { 0, std::numeric_limits<int>::max() };
static range opt_used_ap_n = { 0, std::numeric_limits<int>::max() };
static range opt_unused_ap_n = { 0, std::numeric_limits<int>::max() };
static bool need_unused_ap_count = false;
static range opt_sccs = { 0, std::numeric_limits<int>::max() };
static range opt_acc_sccs = { 0, std::numeric_limits<int>::max() };
static range opt_rej_sccs = { 0, std::numeric_limits<int>::max() };
static range opt_triv_sccs = { 0, std::numeric_limits<int>::max() };
static bool opt_sccs_set = false;
static bool opt_art_sccs_set = false; // need to classify SCCs as Acc/Rej/Triv.
static range opt_inhweak_sccs = { 0, std::numeric_limits<int>::max() };
static bool opt_inhweak_sccs_set = false;
static range opt_weak_sccs = { 0, std::numeric_limits<int>::max() };
static bool opt_weak_sccs_set = false;
static range opt_terminal_sccs = { 0, std::numeric_limits<int>::max() };
static bool opt_terminal_sccs_set = false;
static range opt_nondet_states = { 0, std::numeric_limits<int>::max() };
static bool opt_nondet_states_set = false;
static int opt_max_count = -1;
static range opt_nth = { 0, std::numeric_limits<int>::max() };
static bool opt_destut = false;
static char opt_instut = 0;
static bool opt_is_empty = false;
static bool opt_stripacc = false;
static bool opt_dnf_acc = false;
static bool opt_cnf_acc = false;
static bool opt_rem_fin = false;
static bool opt_clean_acc = false;
static bool opt_complement = false;
static bool opt_complement_acc = false;
static char* opt_decompose_scc = nullptr;
static bool opt_dualize = false;
static bool opt_partial_degen_set = false;
static spot::acc_cond::mark_t opt_partial_degen = {};
static spot::acc_cond::mark_t opt_mask_acc = {};
static std::vector<bool> opt_keep_states = {};
static unsigned int opt_keep_states_initial = 0;
static bool opt_simpl_acc = false;
static bool opt_simplify_exclusive_ap = false;
static bool opt_rem_dead = false;
static bool opt_rem_unreach = false;
static bool opt_rem_unused_ap = false;
static bool opt_sep_sets = false;
static bool opt_split_edges = false;
static const char* opt_sat_minimize = nullptr;
static int opt_highlight_nondet_states = -1;
static int opt_highlight_nondet_edges = -1;
static int opt_highlight_accepting_run = -1;
static bool opt_highlight_languages = false;
static bool opt_dca = false;
static bool opt_streett_like = false;

static spot::twa_graph_ptr
ensure_deterministic(const spot::twa_graph_ptr& aut, bool nonalt = false)
{
  if ((!nonalt || aut->is_existential()) && spot::is_universal(aut))
    return aut;
  spot::postprocessor p;
  p.set_type(spot::postprocessor::Generic);
  p.set_pref(spot::postprocessor::Deterministic);
  p.set_level(level);
  return p.run(aut);
}

static spot::twa_graph_ptr ensure_tba(spot::twa_graph_ptr aut)
{
  spot::postprocessor p;
  p.set_type(spot::postprocessor::TGBA);
  p.set_pref(spot::postprocessor::Any);
  p.set_level(spot::postprocessor::Low);
  return spot::degeneralize_tba(p.run(aut));

}

static spot::twa_graph_ptr
product(spot::twa_graph_ptr left, spot::twa_graph_ptr right)
{
  if ((type == spot::postprocessor::BA)
      && (left->num_sets() + right->num_sets() >
          spot::acc_cond::mark_t::max_accsets()))
    {
      left = ensure_tba(left);
      right = ensure_tba(right);
    }
  return spot::product(left, right);
}

static spot::twa_graph_ptr
product_or(spot::twa_graph_ptr left, spot::twa_graph_ptr right)
{
  if ((type == spot::postprocessor::BA)
      && (left->num_sets() + right->num_sets() >
          spot::acc_cond::mark_t::max_accsets()))
    {
      left = ensure_tba(left);
      right = ensure_tba(right);
    }
  return spot::product_or(left, right);
}

static int
parse_opt(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case 'c':
      automaton_format = Count;
      break;
    case 'F':
      jobs.emplace_back(arg, true);
      break;
    case 'n':
      opt_max_count = to_pos_int(arg, "-n/--max-count");
      break;
    case 'N':
      opt_nth = parse_range(arg, 0, std::numeric_limits<int>::max());
      break;
    case 'u':
      opt->uniq = std::unique_ptr<unique_aut_t>(new std::set<canon_aut>());
      break;
    case 'v':
      opt_invert = true;
      break;
    case 'x':
      {
        const char* opt = extra_options.parse_options(arg);
        if (opt)
          error(2, 0, "failed to parse --options near '%s'", opt);
      }
      break;
    case OPT_AP_N:
      opt_ap_n = parse_range(arg, 0, std::numeric_limits<int>::max());
      break;
    case OPT_ACC_SETS:
      opt_accsets = parse_range(arg, 0, std::numeric_limits<int>::max());
      break;
    case OPT_ACC_SCCS:
      opt_acc_sccs = parse_range(arg, 0, std::numeric_limits<int>::max());
      opt_art_sccs_set = true;
      break;
    case OPT_ACCEPT_WORD:
      try
        {
          opt->acc_words.push_back(spot::parse_word(arg, opt->dict)
                                   ->as_automaton());
        }
      catch (const spot::parse_error& e)
        {
          error(2, 0, "failed to parse the argument of --accept-word:\n%s",
                e.what());
        }
      break;
    case OPT_ACCEPTANCE_IS:
      {
        auto res = argmatch(arg, acc_is_args,
                            (char const*) acc_is_types, sizeof(acc_type));
        if (res >= 0)
          opt_acceptance_is = acc_is_types[res];
        else
          {
            try
              {
                opt_acceptance_is_given =
                  spot::acc_cond(spot::acc_cond::acc_code(arg));
                opt_acceptance_is = ACC_Given;
              }
            catch (const spot::parse_error& err)
              {
                std::cerr << program_name << ": failed to parse '" << arg
                          << ("' as an acceptance formula for "
                              "--acceptance-is\n\t")
                          << err.what()
                          << ("\nIf you do not want to supply a formula, the "
                              "following names are recognized:");
                acc_type last_val = ACC_Any;
                for (int i = 0; acc_is_args[i]; i++)
                  if ((i == 0) || last_val != acc_is_types[i])
                    {
                      std::cerr << "\n  - '" << acc_is_args[i] << '\'';
                      last_val = acc_is_types[i];
                    }
                  else
                    {
                      std::cerr << ", '" << acc_is_args[i] << '\'';
                    }
                std::cerr << '\n';
                exit(2);
              }
          }
      }
      break;
    case OPT_ARE_ISOMORPHIC:
      opt->are_isomorphic = read_automaton(arg, opt->dict);
      break;
    case OPT_CLEAN_ACC:
      opt_clean_acc = true;
      break;
    case OPT_CNF_ACC:
      opt_dnf_acc = false;
      opt_cnf_acc = true;
      break;
    case OPT_COMPLEMENT:
      if (opt_dualize)
        error(2, 0, "either --complement or --dualize options"
                    " can be given, not both");
      opt_complement = true;
      break;
    case OPT_COMPLEMENT_ACC:
      opt_complement_acc = true;
      break;
     case OPT_DCA:
      opt_dca = true;
      break;
    case OPT_DECOMPOSE_SCC:
      opt_decompose_scc = arg;
      break;
    case OPT_DESTUT:
      opt_destut = true;
      break;
    case OPT_DUALIZE:
      if (opt_complement)
        error(2, 0, "either --complement or --dualize options"
                    " can be given, not both");
      opt_dualize = true;
      break;
    case OPT_DNF_ACC:
      opt_dnf_acc = true;
      opt_cnf_acc = false;
      break;
    case OPT_STREETT_LIKE:
      opt_streett_like = true;
      break;
    case OPT_EDGES:
      opt_edges = parse_range(arg, 0, std::numeric_limits<int>::max());
      break;
    case OPT_EXCLUSIVE_AP:
      opt->excl_ap.add_group(arg);
      break;
    case OPT_EQUIVALENT_TO:
      if (opt->equivalent_pos)
        error(2, 0, "only one --equivalent-to option can be given");
      opt->equivalent_pos = read_automaton(arg, opt->dict);
      opt->equivalent_neg =
        spot::dualize(ensure_deterministic(opt->equivalent_pos, true));
      break;
    case OPT_GENERALIZED_RABIN:
      if (arg)
        opt_gra = XARGMATCH("--generalized-rabin", arg, gra_args, gra_types);
      else
        opt_gra = GRA_UNIQUE_INF;
      opt_gsa = GSA_NO;
      break;
    case OPT_GENERALIZED_STREETT:
      if (arg)
        opt_gsa = XARGMATCH("--generalized-streett", arg, gsa_args, gsa_types);
      else
        opt_gsa = GSA_UNIQUE_FIN;
      opt_gra = GRA_NO;
      break;
    case OPT_HAS_EXIST_BRANCHING:
      opt_has_exist_branching = true;
      break;
    case OPT_HAS_UNIV_BRANCHING:
      opt_has_univ_branching = true;
      break;
    case OPT_HIGHLIGHT_ACCEPTING_RUN:
      opt_highlight_accepting_run =
        arg ? to_pos_int(arg, "--highlight-accepting-run") : 1;
      break;
    case OPT_HIGHLIGHT_NONDET:
      {
        int v = arg ? to_pos_int(arg, "--highlight-nondet") : 1;
        opt_highlight_nondet_edges = opt_highlight_nondet_states = v;
        break;
      }
    case OPT_HIGHLIGHT_NONDET_STATES:
      opt_highlight_nondet_states =
        arg ? to_pos_int(arg, "--highlight-nondet-states") : 1;
      break;
    case OPT_HIGHLIGHT_NONDET_EDGES:
      opt_highlight_nondet_edges =
        arg ? to_pos_int(arg, "--highlight-nondet-edges") : 1;
      break;
    case OPT_HIGHLIGHT_WORD:
      {
        char* endptr;
        int res = strtol(arg, &endptr, 10);
        if (endptr == arg)
          {
            res = 1;
          }
        else
          {
            if (res < 0)
              error(2, 0, "failed to parse the argument of --highlight-word: "
                    "%d is not positive", res);
            while (std::isspace(*endptr))
              ++endptr;
            if (*endptr != ',')
              error(2, 0, "failed to parse the argument of --highlight-word: "
                    "%d should be followed by a comma and WORD", res);
            arg = endptr + 1;
          }
        try
          {
            opt->hl_words.emplace_back(spot::parse_word(arg, opt->dict)
                                       ->as_automaton(), res);
          }
        catch (const spot::parse_error& e)
          {
            error(2, 0, "failed to parse the argument of --highlight-word:\n%s",
                  e.what());
          }
      }
      break;
    case OPT_HIGHLIGHT_LANGUAGES:
      opt_highlight_languages = true;
      break;
    case OPT_INSTUT:
      if (!arg || (arg[0] == '1' && arg[1] == 0))
        opt_instut = 1;
      else if (arg[0] == '2' && arg[1] == 0)
        opt_instut = 2;
      else
        error(2, 0, "unknown argument for --instut: %s", arg);
      break;
    case OPT_INCLUDED_IN:
      {
        auto aut = ensure_deterministic(read_automaton(arg, opt->dict), true);
        aut = spot::dualize(aut);
        if (!opt->included_in)
          opt->included_in = aut;
        else
          opt->included_in = spot::product_or(opt->included_in, aut);
      }
      break;
    case OPT_INHERENTLY_WEAK_SCCS:
      opt_inhweak_sccs = parse_range(arg, 0, std::numeric_limits<int>::max());
      opt_inhweak_sccs_set = true;
      opt_art_sccs_set = true;
      break;
    case OPT_INTERSECT:
      opt->intersect = read_automaton(arg, opt->dict);
      break;
    case OPT_IS_ALTERNATING:
      opt_is_alternating = true;
      break;
    case OPT_IS_COLORED:
      opt_is_colored = true;
      break;
    case OPT_IS_COMPLETE:
      opt_is_complete = true;
      break;
    case OPT_IS_DETERMINISTIC:
      opt_is_deterministic = true;
      break;
    case OPT_IS_EMPTY:
      opt_is_empty = true;
      break;
    case OPT_IS_INHERENTLY_WEAK:
      opt_is_inherently_weak = true;
      break;
    case OPT_IS_VERY_WEAK:
      opt_is_very_weak = true;
      break;
    case OPT_IS_SEMI_DETERMINISTIC:
      opt_is_semi_deterministic = true;
      break;
    case OPT_IS_STUTTER_INVARIANT:
      opt_is_stutter_invariant = true;
      break;
    case OPT_IS_TERMINAL:
      opt_is_terminal = true;
      break;
    case OPT_IS_UNAMBIGUOUS:
      opt_is_unambiguous = true;
      break;
    case OPT_IS_WEAK:
      opt_is_weak = true;
      break;
    case OPT_KEEP_STATES:
      {
        std::vector<long> values = to_longs(arg);
        if (!values.empty())
          opt_keep_states_initial = values[0];
        for (auto res : values)
          {
            if (res < 0)
              error(2, 0, "state ids should be non-negative:"
                    " --mask-acc=%ld", res);
            // We don't know yet how many states the automata contain.
            if (opt_keep_states.size() <= static_cast<unsigned long>(res))
              opt_keep_states.resize(res + 1, false);
            opt_keep_states[res] = true;
          }
        break;
      }
    case OPT_MERGE:
      opt_merge = true;
      break;
    case OPT_MASK_ACC:
      {
        for (auto res : to_longs(arg))
          {
            if (res < 0)
              error(2, 0, "acceptance sets should be non-negative:"
                    " --mask-acc=%ld", res);
            if (static_cast<unsigned long>(res) >=
                spot::acc_cond::mark_t::max_accsets())
              error(2, 0, "this implementation does not support that many"
                    " acceptance sets: --mask-acc=%ld", res);
            opt_mask_acc.set(res);
          }
        break;
      }
    case OPT_NONDET_STATES:
      opt_nondet_states = parse_range(arg, 0, std::numeric_limits<int>::max());
      opt_nondet_states_set = true;
      break;
    case OPT_PARTIAL_DEGEN:
      {
        opt_partial_degen_set = true;
        if (arg)
          for (auto res : to_longs(arg))
            {
              if (res < 0)
                error(2, 0, "acceptance sets should be non-negative:"
                      " --partial-degeneralize=%ld", res);
              if (static_cast<unsigned long>(res) >=
                  spot::acc_cond::mark_t::max_accsets())
                error(2, 0, "this implementation does not support that many"
                      " acceptance sets: --partial-degeneralize=%ld", res);
              opt_partial_degen.set(res);
            }
        break;
      }
    case OPT_PRODUCT_AND:
      {
        auto a = read_automaton(arg, opt->dict);
        if (!opt->product_and)
          opt->product_and = std::move(a);
        else
          opt->product_and = ::product(std::move(opt->product_and),
                                       std::move(a));
      }
      break;
    case OPT_PRODUCT_OR:
      {
        auto a = read_automaton(arg, opt->dict);
        if (!opt->product_or)
          opt->product_or = std::move(a);
        else
          opt->product_or = ::product_or(std::move(opt->product_or),
                                         std::move(a));
      }
      break;
    case OPT_RANDOMIZE:
      if (arg)
        {
          for (auto p = arg; *p; ++p)
            switch (*p)
              {
              case 's':
                randomize_st = true;
                break;
              case 't':
                randomize_tr = true;
                break;
              default:
                error(2, 0, "unknown argument for --randomize: '%c'", *p);
              }
        }
      else
        {
          randomize_tr = true;
          randomize_st = true;
        }
      break;
    case OPT_REJ_SCCS:
      opt_rej_sccs = parse_range(arg, 0, std::numeric_limits<int>::max());
      opt_art_sccs_set = true;
      break;
    case OPT_REJECT_WORD:
      try
        {
          opt->rej_words.push_back(spot::parse_word(arg, opt->dict)
                                   ->as_automaton());
        }
      catch (const spot::parse_error& e)
        {
          error(2, 0, "failed to parse the argument of --reject-word:\n%s",
                e.what());
        }
      break;
    case OPT_REM_AP:
      opt->rem_ap.add_ap(arg);
      break;
    case OPT_REM_DEAD:
      opt_rem_dead = true;
      break;
    case OPT_REM_FIN:
      opt_rem_fin = true;
      break;
    case OPT_REM_UNREACH:
      opt_rem_unreach = true;
      break;
    case OPT_REM_UNUSED_AP:
      opt_rem_unused_ap = true;
      break;
    case OPT_SAT_MINIMIZE:
      opt_sat_minimize = arg ? arg : "";
      break;
    case OPT_SCCS:
      opt_sccs_set = true;
      opt_sccs = parse_range(arg, 0, std::numeric_limits<int>::max());
      break;
    case OPT_SEED:
      opt_seed = to_int(arg, "--seed");
      break;
    case OPT_SEP_SETS:
      opt_sep_sets = true;
      break;
    case OPT_SIMPL_ACC:
      opt_simpl_acc = true;
      break;
    case OPT_SIMPLIFY_EXCLUSIVE_AP:
      opt_simplify_exclusive_ap = true;
      opt_merge = true;
      break;
    case OPT_SPLIT_EDGES:
      opt_split_edges = true;
      break;
    case OPT_STATES:
      opt_states = parse_range(arg, 0, std::numeric_limits<int>::max());
      break;
    case OPT_STRIPACC:
      opt_stripacc = true;
      break;
    case OPT_SUM_OR:
      {
        auto a = read_automaton(arg, opt->dict);
        if (!opt->sum_or)
          opt->sum_or = std::move(a);
        else
          opt->sum_or = spot::sum(std::move(opt->sum_or),
                                  std::move(a));
      }
      break;
    case OPT_SUM_AND:
      {
        auto a = read_automaton(arg, opt->dict);
        if (!opt->sum_and)
          opt->sum_and = std::move(a);
        else
          opt->sum_and = spot::sum_and(std::move(opt->sum_and),
                                       std::move(a));
      }
      break;
    case OPT_TERMINAL_SCCS:
      opt_terminal_sccs = parse_range(arg, 0, std::numeric_limits<int>::max());
      opt_terminal_sccs_set = true;
      opt_art_sccs_set = true;
      break;
    case OPT_TRIV_SCCS:
      opt_triv_sccs = parse_range(arg, 0, std::numeric_limits<int>::max());
      opt_art_sccs_set = true;
      break;
    case OPT_USED_AP_N:
      opt_used_ap_n = parse_range(arg, 0, std::numeric_limits<int>::max());
      need_unused_ap_count = true;
      break;
    case OPT_UNUSED_AP_N:
      opt_unused_ap_n = parse_range(arg, 0, std::numeric_limits<int>::max());
      need_unused_ap_count = true;
      break;
    case OPT_WEAK_SCCS:
      opt_weak_sccs = parse_range(arg, 0, std::numeric_limits<int>::max());
      opt_weak_sccs_set = true;
      opt_art_sccs_set = true;
      break;
    case ARGP_KEY_ARG:
      jobs.emplace_back(arg, true);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}

static int unused_ap(const spot::const_twa_graph_ptr& aut)
{
  bdd all = aut->ap_vars();
  for (auto& e: aut->edges())
    {
      all = bdd_exist(all, bdd_support(e.cond));
      if (all == bddtrue)    // All APs are used.
        return 0;
    }
  int count = 0;
  while (all != bddtrue)
    {
      ++count;
      all = bdd_high(all);
    }
  return count;
}

namespace
{
  static
  bool match_acceptance(spot::twa_graph_ptr aut)
  {
    auto& acc = aut->acc();
    switch (opt_acceptance_is)
      {
      case ACC_Any:
        return true;
      case ACC_Given:
        return acc == opt_acceptance_is_given;
      case ACC_tt:
        return acc.is_t();
      case ACC_ff:
        return acc.is_f();
      case ACC_Buchi:
        return acc.is_buchi();
      case ACC_GenBuchi:
        return acc.is_generalized_buchi();
      case ACC_CoBuchi:
        return acc.is_co_buchi();
      case ACC_GenCoBuchi:
        return acc.is_generalized_co_buchi();
      case ACC_Rabin:
        return acc.is_rabin() >= 0;
      case ACC_Streett:
        return acc.is_streett() >= 0;
      case ACC_GenRabin:
        {
          std::vector<unsigned> p;
          return acc.is_generalized_rabin(p);
        }
      case ACC_GenStreett:
        {
          std::vector<unsigned> p;
          return acc.is_generalized_streett(p);
        }
      case ACC_RabinLike:
        {
          std::vector<spot::acc_cond::rs_pair> p;
          return acc.is_rabin_like(p);
        }
      case ACC_StreettLike:
        {
          std::vector<spot::acc_cond::rs_pair> p;
          return acc.is_streett_like(p);
        }
      case ACC_Parity:
      case ACC_ParityMin:
      case ACC_ParityMax:
      case ACC_ParityOdd:
      case ACC_ParityEven:
      case ACC_ParityMinOdd:
      case ACC_ParityMaxOdd:
      case ACC_ParityMinEven:
      case ACC_ParityMaxEven:
        {
          bool max;
          bool odd;
          bool is_p = acc.is_parity(max, odd, true);
          if (!is_p)
            return false;
          switch (opt_acceptance_is)
            {
            case ACC_Parity:
              return true;
            case ACC_ParityMin:
              return !max;
            case ACC_ParityMax:
              return max;
            case ACC_ParityOdd:
              return odd;
            case ACC_ParityEven:
              return !odd;
            case ACC_ParityMinOdd:
              return !max && odd;
            case ACC_ParityMaxOdd:
              return max && odd;
            case ACC_ParityMinEven:
              return !max && !odd;
            case ACC_ParityMaxEven:
              return max && !odd;
            default:
              SPOT_UNREACHABLE();
            }
        }
      case ACC_FinLess:
        return !acc.uses_fin_acceptance() && !acc.is_f();
      }
    SPOT_UNREACHABLE();
  }


  struct autfilt_processor: hoa_processor
  {
  private:
    spot::postprocessor& post;
    automaton_printer printer;
  public:
    autfilt_processor(spot::postprocessor& post, spot::bdd_dict_ptr dict)
      : hoa_processor(dict), post(post), printer(aut_input)
    {
    }

    int
    process_automaton(const spot::const_parsed_aut_ptr& haut) override
    {
      static unsigned order = 0;
      ++order;

      spot::process_timer timer;
      timer.start();

      // If --stats or --name is used, duplicate the automaton so we
      // never modify the original automaton (e.g. with
      // merge_edges()) and the statistics about it make sense.
      auto aut = ((automaton_format == Stats) || opt_name)
        ? spot::make_twa_graph(haut->aut, spot::twa::prop_set::all())
        : haut->aut;

      // Preprocessing.

      if (opt_stripacc)
        spot::strip_acceptance_here(aut);
      if (opt_merge)
        aut->merge_edges();

      if (opt_simpl_acc)
        simplify_acceptance_here(aut);
      else if (opt_clean_acc)
        cleanup_acceptance_here(aut);

      if (opt_sep_sets)
        separate_sets_here(aut);
      if (opt_complement_acc)
        aut->set_acceptance(aut->acc().num_sets(),
                            aut->get_acceptance().complement());
      if (opt_rem_fin)
        aut = remove_fin(aut);
      if (opt_dnf_acc)
        aut->set_acceptance(aut->acc().num_sets(),
                            aut->get_acceptance().to_dnf());
      if (opt_cnf_acc)
        aut->set_acceptance(aut->acc().num_sets(),
                            aut->get_acceptance().to_cnf());

      // Filters.

      bool matched = true;

      matched &= opt_nth.contains(order);
      matched &= opt_states.contains(aut->num_states());
      matched &= opt_edges.contains(aut->num_edges());
      matched &= opt_accsets.contains(aut->acc().num_sets());
      matched &= opt_ap_n.contains(aut->ap().size());
      if (matched && need_unused_ap_count)
        {
          int unused = unused_ap(aut);
          matched &= opt_unused_ap_n.contains(unused);
          matched &= opt_used_ap_n.contains(aut->ap().size() - unused);
        }
      if (matched && opt_is_alternating)
        matched &= !aut->is_existential();
      if (matched && opt_is_colored)
        matched &= is_colored(aut);
      if (matched && opt_is_complete)
        matched &= is_complete(aut);

      if (matched && opt_acceptance_is)
        matched = match_acceptance(aut);

      if (matched && (opt_sccs_set | opt_art_sccs_set))
        {
          spot::scc_info si(aut);
          unsigned n = si.scc_count();
          matched = opt_sccs.contains(n);

          if (opt_art_sccs_set && matched)
            {
              si.determine_unknown_acceptance();
              unsigned triv = 0;
              unsigned acc = 0;
              unsigned rej = 0;
              unsigned inhweak = 0;
              unsigned weak = 0;
              unsigned terminal = 0;
              for (unsigned s = 0; s < n; ++s)
                if (si.is_trivial(s))
                  {
                    ++triv;
                  }
                else if (si.is_rejecting_scc(s))
                  {
                    ++rej;
                  }
                else
                  {
                    ++acc;
                    if (opt_inhweak_sccs_set)
                      inhweak += is_inherently_weak_scc(si, s);
                    if (opt_weak_sccs_set)
                      weak += is_weak_scc(si, s);
                    if (opt_terminal_sccs_set)
                      terminal += is_terminal_scc(si, s);
                  }
              matched &= opt_acc_sccs.contains(acc);
              matched &= opt_rej_sccs.contains(rej);
              matched &= opt_triv_sccs.contains(triv);
              matched &= opt_inhweak_sccs.contains(inhweak);
              matched &= opt_weak_sccs.contains(weak);
              matched &= opt_terminal_sccs.contains(terminal);
            }
        }
      if (opt_nondet_states_set)
        matched &= opt_nondet_states.contains(spot::count_nondet_states(aut));
      if (opt_has_univ_branching)
        matched &= !aut->is_existential();
      if (opt_has_exist_branching)
        matched &= !is_universal(aut);
      if (opt_is_deterministic)
        {
          matched &= is_deterministic(aut);
        }
      else
        {
          if (opt_is_unambiguous)
            matched &= is_unambiguous(aut);
          if (opt_is_semi_deterministic)
            matched &= is_semi_deterministic(aut);
        }
      if (opt_is_terminal)
        matched &= is_terminal_automaton(aut);
      else if (opt_is_very_weak)
        matched &= is_very_weak_automaton(aut);
      else if (opt_is_weak)
        matched &= is_weak_automaton(aut);
      else if (opt_is_inherently_weak)
        matched &= is_inherently_weak_automaton(aut);
      if (opt->are_isomorphic)
        matched &= opt->isomorphism_checker->is_isomorphic(aut);
      if (opt_is_empty)
        matched &= aut->is_empty();
      if (opt->intersect)
        matched &= aut->intersects(opt->intersect);
      if (opt->included_in)
        matched &= !aut->intersects(opt->included_in);
      if (opt->equivalent_pos)
        matched &= !aut->intersects(opt->equivalent_neg)
          && spot::contains(aut, opt->equivalent_pos);

      if (matched && !opt->acc_words.empty())
        for (auto& word_aut: opt->acc_words)
          if (spot::product(aut, word_aut)->is_empty())
            {
              matched = false;
              break;
            }
      if (matched && !opt->rej_words.empty())
        for (auto& word_aut: opt->rej_words)
          if (!spot::product(aut, word_aut)->is_empty())
            {
              matched = false;
              break;
            }
      if (opt_is_stutter_invariant)
        {
          check_stutter_invariance(aut);
          assert(aut->prop_stutter_invariant().is_known());
          matched &= aut->prop_stutter_invariant().is_true();
        }

      // Drop or keep matched automata depending on the --invert option
      if (matched == opt_invert)
        return 0;

      // Postprocessing.

      if (opt_mask_acc)
        aut = mask_acc_sets(aut, opt_mask_acc & aut->acc().all_sets());

      if (!opt->rem_ap.empty())
        aut = opt->rem_ap.strip(aut);

      // opt_simplify_exclusive_ap is handled only after
      // post-processing.
      if (!opt->excl_ap.empty())
        aut = opt->excl_ap.constrain(aut, false);

      if (opt_destut)
        aut = spot::closure_inplace(std::move(aut));
      if (opt_instut == 1)
        aut = spot::sl(std::move(aut));
      else if (opt_instut == 2)
        aut = spot::sl2_inplace(std::move(aut));

      if (!opt_keep_states.empty())
        aut = mask_keep_accessible_states(aut, opt_keep_states,
                                          opt_keep_states_initial);
      if (opt_rem_dead)
        aut->purge_dead_states();
      else if (opt_rem_unreach)
        aut->purge_unreachable_states();

      if (opt_partial_degen_set)
        {
          if (opt_partial_degen)
            {
              auto sets = opt_partial_degen & aut->acc().all_sets();
              aut = spot::partial_degeneralize(aut, sets);
            }
          else
            {
              aut = spot::partial_degeneralize(aut);
            }
        }

      if (opt->product_and)
        aut = ::product(std::move(aut), opt->product_and);
      if (opt->product_or)
        aut = ::product_or(std::move(aut), opt->product_or);

      if (opt->sum_or)
        aut = spot::sum(std::move(aut), opt->sum_or);
      if (opt->sum_and)
        aut = spot::sum_and(std::move(aut), opt->sum_and);

      if (opt_decompose_scc)
        {
          aut = decompose_scc(aut, opt_decompose_scc);
          if (!aut)
            return 0;
        }

      if (opt_sat_minimize)
        {
          aut = spot::sat_minimize(aut, opt_sat_minimize, sbacc);
          if (!aut)
            return 0;
        }

      if (opt_complement)
        {
          aut = spot::complement(aut);
          aut->merge_edges();
        }

      if (opt_dualize)
        aut = spot::dualize(aut);

      aut = post.run(aut, nullptr);

      if (opt_gra)
        aut = spot::to_generalized_rabin(aut, opt_gra == GRA_SHARE_INF);
      if (opt_gsa)
        aut = spot::to_generalized_streett(aut, opt_gsa == GSA_SHARE_FIN);

      if (opt_streett_like)
        aut = spot::dnf_to_streett(aut);

      if (opt_simplify_exclusive_ap && !opt->excl_ap.empty())
        aut = opt->excl_ap.constrain(aut, true);
      else if (opt_rem_unused_ap) // constrain(aut, true) already does that
        aut->remove_unused_ap();

      if (opt_split_edges)
        aut = spot::split_edges(aut);

      if (randomize_st || randomize_tr)
        spot::randomize(aut, randomize_st, randomize_tr);

      if (opt_highlight_nondet_states >= 0)
        spot::highlight_nondet_states(aut, opt_highlight_nondet_states);
      if (opt_highlight_nondet_edges >= 0)
        spot::highlight_nondet_edges(aut, opt_highlight_nondet_edges);

      if (opt_highlight_languages)
        spot::highlight_languages(aut);

      if (opt_highlight_accepting_run >= 0)
        aut->accepting_run()->highlight(opt_highlight_accepting_run);

      if (!opt->hl_words.empty())
        for (auto& word_aut: opt->hl_words)
          {
            if (aut->acc().uses_fin_acceptance())
              error(2, 0,
                    "--highlight-word does not yet work with Fin acceptance");
            if (auto run = spot::product(aut, word_aut.first)->accepting_run())
              run->project(aut)->highlight(word_aut.second);
          }

      timer.stop();
      if (opt->uniq)
        {
          auto tmp =
            spot::canonicalize(make_twa_graph(aut,
                                                 spot::twa::prop_set::all()));
          if (!opt->uniq->emplace(tmp).second)
            return 0;
        }

      ++match_count;

      printer.print(aut, timer, nullptr, haut->filename.c_str(), -1,
                    haut, prefix, suffix);

      if (opt_max_count >= 0 && match_count >= opt_max_count)
        abort_run = true;

      return 0;
    }
  };
}

int
main(int argc, char** argv)
{
  return protected_main(argv, [&] {
      const argp ap = { options, parse_opt, "[FILENAME[/COL]...]",
                        argp_program_doc, children, nullptr, nullptr };

      // This will ensure that all objects stored in this struct are
      // destroyed before global variables.
      opt_t o;
      opt = &o;

      // Disable post-processing as much as possible by default.
      level = spot::postprocessor::Low;
      pref = spot::postprocessor::Any;
      type = spot::postprocessor::Generic;
      if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
        exit(err);

      if (level_set && !pref_set)
        pref = spot::postprocessor::Small;
      if (pref_set && !level_set)
        level = spot::postprocessor::High;

      check_no_automaton();

      if (opt->are_isomorphic)
        {
          if (opt_merge)
            opt->are_isomorphic->merge_edges();
          opt->isomorphism_checker = std::unique_ptr<spot::isomorphism_checker>
            (new spot::isomorphism_checker(opt->are_isomorphic));
        }


      spot::srand(opt_seed);

      spot::postprocessor post(&extra_options);
      post.set_type(type);
      post.set_pref(pref | comp | sbacc | colored);
      post.set_level(level);

      autfilt_processor processor(post, o.dict);
      if (processor.run())
        return 2;

      // Diagnose unused -x options
      extra_options.report_unused_options();

      if (automaton_format == Count)
        std::cout << match_count << std::endl;

      check_cout();
      return match_count ? 0 : 1;
    });
}
