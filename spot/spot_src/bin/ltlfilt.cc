// -*- coding: utf-8 -*-
// Copyright (C) 2012-2020 Laboratoire de Recherche et Développement
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

#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <argp.h>
#include <cstring>
#include "error.h"

#include "common_setup.hh"
#include "common_finput.hh"
#include "common_output.hh"
#include "common_cout.hh"
#include "common_conv.hh"
#include "common_r.hh"
#include "common_range.hh"

#include <spot/misc/hash.hh>
#include <spot/misc/timer.hh>
#include <spot/tl/simplify.hh>
#include <spot/tl/length.hh>
#include <spot/tl/relabel.hh>
#include <spot/tl/unabbrev.hh>
#include <spot/tl/remove_x.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/exclusive.hh>
#include <spot/tl/ltlf.hh>
#include <spot/tl/print.hh>
#include <spot/tl/hierarchy.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/minimize.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/remfin.hh>
#include <spot/twaalgos/strength.hh>
#include <spot/twaalgos/stutter.hh>
#include <spot/twaalgos/totgba.hh>
#include <spot/twaalgos/word.hh>

const char argp_program_doc[] ="\
Read a list of formulas and output them back after some optional processing.\v\
Exit status:\n\
  0  if some formulas were output (skipped syntax errors do not count)\n\
  1  if no formulas were output (no match)\n\
  2  if any error has been reported";

enum {
  OPT_ACCEPT_WORD = 256,
  OPT_AP_N,
  OPT_BOOLEAN,
  OPT_BOOLEAN_TO_ISOP,
  OPT_BSIZE,
  OPT_BSIZE_MAX,
  OPT_BSIZE_MIN,
  OPT_DEFINE,
  OPT_DROP_ERRORS,
  OPT_EQUIVALENT_TO,
  OPT_EXCLUSIVE_AP,
  OPT_EVENTUAL,
  OPT_FROM_LTLF,
  OPT_GUARANTEE,
  OPT_IGNORE_ERRORS,
  OPT_IMPLIED_BY,
  OPT_IMPLY,
  OPT_LIVENESS,
  OPT_LTL,
  OPT_NEGATE,
  OPT_NNF,
  OPT_OBLIGATION,
  OPT_PERSISTENCE,
  OPT_RECURRENCE,
  OPT_REJECT_WORD,
  OPT_RELABEL,
  OPT_RELABEL_BOOL,
  OPT_REMOVE_WM,
  OPT_REMOVE_X,
  OPT_SAFETY,
  OPT_SIZE,
  OPT_SIZE_MAX,
  OPT_SIZE_MIN,
  OPT_SKIP_ERRORS,
  OPT_STUTTER_INSENSITIVE,
  OPT_SUSPENDABLE,
  OPT_SYNTACTIC_GUARANTEE,
  OPT_SYNTACTIC_OBLIGATION,
  OPT_SYNTACTIC_PERSISTENCE,
  OPT_SYNTACTIC_RECURRENCE,
  OPT_SYNTACTIC_SAFETY,
  OPT_SYNTACTIC_SI,
  OPT_UNABBREVIATE,
  OPT_UNIVERSAL,
};

static const argp_option options[] =
  {
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Error handling:", 2 },
    { "skip-errors", OPT_SKIP_ERRORS, nullptr, 0,
      "output erroneous lines as-is without processing", 0 },
    { "drop-errors", OPT_DROP_ERRORS, nullptr, 0,
      "discard erroneous lines (default)", 0 },
    { "ignore-errors", OPT_IGNORE_ERRORS, nullptr, 0,
      "do not report syntax errors", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Transformation options:", 3 },
    { "negate", OPT_NEGATE, nullptr, 0, "negate each formula", 0 },
    { "nnf", OPT_NNF, nullptr, 0,
      "rewrite formulas in negative normal form", 0 },
    { "relabel", OPT_RELABEL, "abc|pnn", OPTION_ARG_OPTIONAL,
      "relabel all atomic propositions, alphabetically unless " \
      "specified otherwise", 0 },
    { "relabel-bool", OPT_RELABEL_BOOL, "abc|pnn", OPTION_ARG_OPTIONAL,
      "relabel Boolean subexpressions, alphabetically unless " \
      "specified otherwise", 0 },
    { "define", OPT_DEFINE, "FILENAME", OPTION_ARG_OPTIONAL,
      "when used with --relabel or --relabel-bool, output the relabeling map "
      "using #define statements", 0 },
    { "remove-wm", OPT_REMOVE_WM, nullptr, 0,
      "rewrite operators W and M using U and R (this is an alias for "
      "--unabbreviate=WM)", 0 },
    { "boolean-to-isop", OPT_BOOLEAN_TO_ISOP, nullptr, 0,
      "rewrite Boolean subformulas as irredundant sum of products "
      "(implies at least -r1)", 0 },
    { "remove-x", OPT_REMOVE_X, nullptr, 0,
      "remove X operators (valid only for stutter-insensitive properties)",
      0 },
    { "unabbreviate", OPT_UNABBREVIATE, "STR", OPTION_ARG_OPTIONAL,
      "remove all occurrences of the operators specified by STR, which "
      "must be a substring of \"eFGiMRW^\", where 'e', 'i', and '^' stand "
      "respectively for <->, ->, and xor.  If no argument is passed, "
      "the subset \"eFGiMW^\" is used.", 0 },
    { "exclusive-ap", OPT_EXCLUSIVE_AP, "AP,AP,...", 0,
      "if any of those APs occur in the formula, add a term ensuring "
      "two of them may not be true at the same time.  Use this option "
      "multiple times to declare independent groups of exclusive "
      "propositions.", 0 },
    { "from-ltlf", OPT_FROM_LTLF, "alive", OPTION_ARG_OPTIONAL,
      "transform LTLf (finite LTL) to LTL by introducing some 'alive'"
      " proposition", 0 },
    DECLARE_OPT_R,
    LEVEL_DOC(4),
    /**************************************************/
    { nullptr, 0, nullptr, 0,
      "Filtering options (matching is done after transformation):", 5 },
    { "ltl", OPT_LTL, nullptr, 0,
      "match only LTL formulas (no PSL operator)", 0 },
    { "boolean", OPT_BOOLEAN, nullptr, 0, "match Boolean formulas", 0 },
    { "eventual", OPT_EVENTUAL, nullptr, 0, "match pure eventualities", 0 },
    { "universal", OPT_UNIVERSAL, nullptr, 0,
      "match purely universal formulas", 0 },
    { "suspendable", OPT_SUSPENDABLE, nullptr, 0,
      "synonym for --universal --eventual", 0 },
    { "syntactic-safety", OPT_SYNTACTIC_SAFETY, nullptr, 0,
      "match syntactic-safety formulas", 0 },
    { "syntactic-guarantee", OPT_SYNTACTIC_GUARANTEE, nullptr, 0,
      "match syntactic-guarantee formulas", 0 },
    { "syntactic-obligation", OPT_SYNTACTIC_OBLIGATION, nullptr, 0,
      "match syntactic-obligation formulas", 0 },
    { "syntactic-recurrence", OPT_SYNTACTIC_RECURRENCE, nullptr, 0,
      "match syntactic-recurrence formulas", 0 },
    { "syntactic-persistence", OPT_SYNTACTIC_PERSISTENCE, nullptr, 0,
      "match syntactic-persistence formulas", 0 },
    { "syntactic-stutter-invariant", OPT_SYNTACTIC_SI, nullptr, 0,
      "match stutter-invariant formulas syntactically (LTL-X or siPSL)", 0 },
    { "nox", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "liveness", OPT_LIVENESS, nullptr, 0,
      "match liveness properties", 0 },
    { "safety", OPT_SAFETY, nullptr, 0,
      "match safety formulas (even pathological)", 0 },
    { "guarantee", OPT_GUARANTEE, nullptr, 0,
      "match guarantee formulas (even pathological)", 0 },
    { "obligation", OPT_OBLIGATION, nullptr, 0,
      "match obligation formulas (even pathological)", 0 },
    { "persistence", OPT_PERSISTENCE, nullptr, 0,
      "match persistence formulas (even pathological)", 0 },
    { "recurrence", OPT_RECURRENCE, nullptr, 0,
      "match recurrence formulas (even pathological)", 0 },
    { "size", OPT_SIZE, "RANGE", 0,
      "match formulas with size in RANGE", 0},
    // backward compatibility
    { "size-max", OPT_SIZE_MAX, "INT", OPTION_HIDDEN,
      "match formulas with size <= INT", 0 },
    // backward compatibility
    { "size-min", OPT_SIZE_MIN, "INT", OPTION_HIDDEN,
      "match formulas with size >= INT", 0 },
    { "bsize", OPT_BSIZE, "RANGE", 0,
      "match formulas with Boolean size in RANGE", 0 },
    // backward compatibility
    { "bsize-max", OPT_BSIZE_MAX, "INT", OPTION_HIDDEN,
      "match formulas with Boolean size <= INT", 0 },
    // backward compatibility
    { "bsize-min", OPT_BSIZE_MIN, "INT", OPTION_HIDDEN,
      "match formulas with Boolean size >= INT", 0 },
    { "implied-by", OPT_IMPLIED_BY, "FORMULA", 0,
      "match formulas implied by FORMULA", 0 },
    { "imply", OPT_IMPLY, "FORMULA", 0,
      "match formulas implying FORMULA", 0 },
    { "equivalent-to", OPT_EQUIVALENT_TO, "FORMULA", 0,
      "match formulas equivalent to FORMULA", 0 },
    { "stutter-insensitive", OPT_STUTTER_INSENSITIVE, nullptr, 0,
      "match stutter-insensitive LTL formulas", 0 },
    { "stutter-invariant", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "ap", OPT_AP_N, "RANGE", 0,
      "match formulas with a number of atomic propositions in RANGE", 0 },
    { "nth", 'N', "RANGE", 0,
      "assuming input formulas are numbered from 1, keep only those in RANGE",
      0 },
    { "invert-match", 'v', nullptr, 0, "select non-matching formulas", 0},
    { "unique", 'u', nullptr, 0,
      "drop formulas that have already been output (not affected by -v)", 0 },
    { "accept-word", OPT_ACCEPT_WORD, "WORD", 0,
      "keep formulas that accept WORD", 0 },
    { "reject-word", OPT_REJECT_WORD, "WORD", 0,
      "keep formulas that reject WORD", 0 },
    RANGE_DOC_FULL,
    WORD_DOC,
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Output options:", -20 },
    { "count", 'c', nullptr, 0, "print only a count of matched formulas", 0 },
    { "quiet", 'q', nullptr, 0, "suppress all normal output", 0 },
    { "max-count", 'n', "NUM", 0, "output at most NUM formulas", 0 },
    { nullptr, 0, nullptr, 0, "The FORMAT string passed to --format may use "\
      "the following interpreted sequences:", -19 },
    { "%f", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the formula (in the selected syntax)", 0 },
    { "%F", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the name of the input file", 0 },
    { "%L", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the original line number in the input file", 0 },
    { "%r", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "wall-clock time elapsed in seconds (excluding parsing)", 0 },
    { "%R, %[LETTERS]R", 0, nullptr,
      OPTION_DOC | OPTION_NO_USAGE,
      "CPU time (excluding parsing), in seconds; Add LETTERS to restrict to"
      "(u) user time, (s) system time, (p) parent process, "
      "or (c) children processes.", 0 },
    { "%<", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the part of the line before the formula if it "
      "comes from a column extracted from a CSV file", 0 },
    { "%>", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the part of the line after the formula if it "
      "comes from a column extracted from a CSV file", 0 },
    { "%%", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "a single %", 0 },
    COMMON_LTL_OUTPUT_SPECS,
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Miscellaneous options:", -1 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

const struct argp_child children[] =
  {
    { &finput_argp, 0, nullptr, 1 },
    { &output_argp, 0, nullptr, 0 },
    { &misc_argp, 0, nullptr, 0 },
    { nullptr, 0, nullptr, 0 }
  };

static bool one_match = false;

enum error_style_t { drop_errors, skip_errors };
static error_style_t error_style = drop_errors;
static bool ignore_errors = false;
static bool nnf = false;
static bool negate = false;
static bool boolean_to_isop = false;
static bool unique = false;
static bool psl = false;
static bool liveness = false;
static bool ltl = false;
static bool invert = false;
static bool boolean = false;
static bool universal = false;
static bool eventual = false;
static bool syntactic_safety = false;
static bool syntactic_guarantee = false;
static bool syntactic_obligation = false;
static bool syntactic_recurrence = false;
static bool syntactic_persistence = false;
static bool syntactic_si = false;
static bool safety = false;
static bool guarantee = false;
static bool obligation = false;
static bool recurrence = false;
static bool persistence = false;
static range size = { -1, -1 };
static range bsize = { -1, -1 };
enum relabeling_mode { NoRelabeling = 0, ApRelabeling, BseRelabeling };
static relabeling_mode relabeling = NoRelabeling;
static spot::relabeling_style style = spot::Abc;
static bool remove_x = false;
static bool stutter_insensitive = false;
static range ap_n = { -1, -1 };
static range opt_nth = { 0, std::numeric_limits<int>::max() };
static int opt_max_count = -1;
static long int match_count = 0;
static const char* from_ltlf = nullptr;


// We want all these variables to be destroyed when we exit main, to
// make sure it happens before all other global variables (like the
// atomic propositions maps) are destroyed.  Otherwise we risk
// accessing deleted stuff.
static struct opt_t
{
  spot::bdd_dict_ptr dict = spot::make_bdd_dict();
  spot::exclusive_ap excl_ap;
  std::unique_ptr<output_file> output_define = nullptr;
  spot::formula implied_by = nullptr;
  spot::formula imply = nullptr;
  spot::formula equivalent_to = nullptr;
  std::vector<spot::twa_graph_ptr> acc_words;
  std::vector<spot::twa_graph_ptr> rej_words;
}* opt;

static std::string unabbreviate;


static spot::formula
parse_formula_arg(const std::string& input)
{
  spot::parsed_formula pf = parse_formula(input);
  if (pf.format_errors(std::cerr))
    error(2, 0, "parse error when parsing an argument");
  return pf.f;
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
      output_format = count_output;
      break;
    case 'n':
      opt_max_count = to_pos_int(arg, "-n/--max-count");
      break;
    case 'N':
      opt_nth = parse_range(arg, 0, std::numeric_limits<int>::max());
      break;
    case 'q':
      output_format = quiet_output;
      break;
    case OPT_R:
      parse_r(arg);
      break;
    case 'u':
      unique = true;
      break;
    case 'v':
      invert = true;
      break;
    case ARGP_KEY_ARG:
      // FIXME: use stat() to distinguish filename from string?
      jobs.emplace_back(arg, true);
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
    case OPT_BOOLEAN:
      boolean = true;
      break;
    case OPT_BOOLEAN_TO_ISOP:
      boolean_to_isop = true;
      break;
    case OPT_BSIZE:
      bsize = parse_range(arg, 0, std::numeric_limits<int>::max());
      break;
    case OPT_BSIZE_MIN:
      bsize.min = to_int(arg, "--bsize-min");
      break;
    case OPT_BSIZE_MAX:
      bsize.max = to_int(arg, "--bsize-max");
      break;
    case OPT_DEFINE:
      opt->output_define.reset(new output_file(arg ? arg : "-"));
      break;
    case OPT_DROP_ERRORS:
      error_style = drop_errors;
      break;
    case OPT_EVENTUAL:
      eventual = true;
      break;
    case OPT_EQUIVALENT_TO:
      {
        if (opt->equivalent_to)
          error(2, 0, "only one --equivalent-to option can be given");
        opt->equivalent_to = parse_formula_arg(arg);
        break;
      }
    case OPT_EXCLUSIVE_AP:
      opt->excl_ap.add_group(arg);
      break;
    case OPT_GUARANTEE:
      guarantee = obligation = true;
      break;
    case OPT_IGNORE_ERRORS:
      ignore_errors = true;
      break;
    case OPT_IMPLIED_BY:
      {
        spot::formula i = parse_formula_arg(arg);
        // a→c∧b→c ≡ (a∨b)→c
        opt->implied_by = spot::formula::Or({opt->implied_by, i});
        break;
      }
    case OPT_IMPLY:
      {
        // a→b∧a→c ≡ a→(b∧c)
        spot::formula i = parse_formula_arg(arg);
        opt->imply = spot::formula::And({opt->imply, i});
        break;
      }
    case OPT_LIVENESS:
      liveness = true;
      break;
    case OPT_LTL:
      ltl = true;
      break;
    case OPT_FROM_LTLF:
      from_ltlf = arg ? arg : "alive";
      break;
    case OPT_NEGATE:
      negate = true;
      break;
    case OPT_NNF:
      nnf = true;
      break;
    case OPT_OBLIGATION:
      obligation = true;
      break;
    case OPT_PERSISTENCE:
      persistence = true;
      break;
    case OPT_RECURRENCE:
      recurrence = true;
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
    case OPT_RELABEL:
    case OPT_RELABEL_BOOL:
      relabeling = (key == OPT_RELABEL_BOOL ? BseRelabeling : ApRelabeling);
      if (!arg || !strncasecmp(arg, "abc", 6))
        style = spot::Abc;
      else if (!strncasecmp(arg, "pnn", 4))
        style = spot::Pnn;
      else
        error(2, 0, "invalid argument for --relabel%s: '%s'",
              (key == OPT_RELABEL_BOOL ? "-bool" : ""),
              arg);
      break;
    case OPT_REMOVE_WM:
      unabbreviate += "MW";
      break;
    case OPT_REMOVE_X:
      remove_x = true;
      break;
    case OPT_SAFETY:
      safety = obligation = true;
      break;
    case OPT_SIZE:
      size = parse_range(arg, 0, std::numeric_limits<int>::max());
      break;
    case OPT_SIZE_MIN:
      size.min = to_int(arg, "--size-min");
      break;
    case OPT_SIZE_MAX:
      size.max = to_int(arg, "--size-max");
      break;
    case OPT_SKIP_ERRORS:
      error_style = skip_errors;
      break;
    case OPT_STUTTER_INSENSITIVE:
      stutter_insensitive = true;
      break;
    case OPT_UNABBREVIATE:
      if (arg)
        unabbreviate += arg;
      else
        unabbreviate += spot::default_unabbrev_string;
      break;
    case OPT_AP_N:
      ap_n = parse_range(arg, 0, std::numeric_limits<int>::max());
      break;
    case OPT_SUSPENDABLE:
      universal = true;
      eventual = true;
      break;
    case OPT_SYNTACTIC_SAFETY:
      syntactic_safety = true;
      break;
    case OPT_SYNTACTIC_GUARANTEE:
      syntactic_guarantee = true;
      break;
    case OPT_SYNTACTIC_OBLIGATION:
      syntactic_obligation = true;
      break;
    case OPT_SYNTACTIC_RECURRENCE:
      syntactic_recurrence = true;
      break;
    case OPT_SYNTACTIC_PERSISTENCE:
      syntactic_persistence = true;
      break;
    case OPT_SYNTACTIC_SI:
      syntactic_si = true;
      break;
    case OPT_UNIVERSAL:
      universal = true;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}

typedef
std::unordered_set<spot::formula> fset_t;

namespace
{
  class ltl_processor final: public job_processor
  {
  public:
    spot::tl_simplifier& simpl;
    fset_t unique_set;
    spot::relabeling_map relmap;

    ltl_processor(spot::tl_simplifier& simpl)
      : simpl(simpl)
    {
    }

    int
    process_string(const std::string& input,
                   const char* filename = nullptr, int linenum = 0) override
    {
      spot::parsed_formula pf = parse_formula(input);

      if (!pf.f || !pf.errors.empty())
          {
            if (!ignore_errors)
              {
                if (filename)
                  error_at_line(0, 0, filename, linenum, "parse error:");
                pf.format_errors(std::cerr);
              }

            if (error_style == skip_errors)
              std::cout << input << '\n';
            else
              assert(error_style == drop_errors);
            check_cout();
            return !ignore_errors;
          }
      try
        {
          return process_formula(pf.f, filename, linenum);
        }
      catch (const std::runtime_error& e)
        {
          error_at_line(2, 0, filename, linenum, "%s", e.what());
          SPOT_UNREACHABLE();
        }
    }

    int
    process_formula(spot::formula f,
                    const char* filename = nullptr, int linenum = 0) override
    {
      static unsigned order = 0;
      ++order;
      spot::process_timer timer;
      timer.start();


      if (opt_max_count >= 0 && match_count >= opt_max_count)
        {
          abort_run = true;
          return 0;
        }

      if (negate)
        f = spot::formula::Not(f);

      bool matched = opt_nth.contains(order);

      if (from_ltlf)
        {
          matched &= !ltl || f.is_ltl_formula();
          if (matched)
            f = spot::from_ltlf(f, from_ltlf);
        }

      if (remove_x)
        {
          // If simplification are enabled, we do them before and after.
          if (simplification_level)
            f = simpl.simplify(f);
          f = spot::remove_x(f);
        }

      if (simplification_level || boolean_to_isop)
        f = simpl.simplify(f);

      if (nnf)
        f = simpl.negative_normal_form(f);

      switch (relabeling)
        {
        case ApRelabeling:
          {
            relmap.clear();
            f = spot::relabel(f, style, &relmap);
            break;
          }
        case BseRelabeling:
          {
            relmap.clear();
            f = spot::relabel_bse(f, style, &relmap);
            break;
          }
        case NoRelabeling:
          break;
        }

      if (!unabbreviate.empty())
        f = spot::unabbreviate(f, unabbreviate.c_str());

      if (!opt->excl_ap.empty())
        f = opt->excl_ap.constrain(f);

      matched &= !ltl || f.is_ltl_formula();
      matched &= !psl || f.is_psl_formula();
      matched &= !boolean || f.is_boolean();
      matched &= !universal || f.is_universal();
      matched &= !eventual || f.is_eventual();
      matched &= !syntactic_safety || f.is_syntactic_safety();
      matched &= !syntactic_guarantee || f.is_syntactic_guarantee();
      matched &= !syntactic_obligation || f.is_syntactic_obligation();
      matched &= !syntactic_recurrence || f.is_syntactic_recurrence();
      matched &= !syntactic_persistence || f.is_syntactic_persistence();
      matched &= !syntactic_si || f.is_syntactic_stutter_invariant();
      if (matched && (ap_n.min > 0 || ap_n.max >= 0))
        {
          auto s = atomic_prop_collect(f);
          int n = s->size();
          delete s;
          matched &= (ap_n.min <= 0) || (n >= ap_n.min);
          matched &= (ap_n.max < 0) || (n <= ap_n.max);
        }

      if (matched && (size.min > 0 || size.max >= 0))
        {
          int l = spot::length(f);
          matched &= (size.min <= 0) || (l >= size.min);
          matched &= (size.max < 0) || (l <= size.max);
        }

      if (matched && (bsize.min > 0 || bsize.max >= 0))
        {
          int l = spot::length_boolone(f);
          matched &= (bsize.min <= 0) || (l >= bsize.min);
          matched &= (bsize.max < 0) || (l <= bsize.max);
        }

      matched &= !opt->implied_by || simpl.implication(opt->implied_by, f);
      matched &= !opt->imply || simpl.implication(f, opt->imply);
      matched &= !opt->equivalent_to
        || simpl.are_equivalent(f, opt->equivalent_to);
      matched &= !stutter_insensitive || spot::is_stutter_invariant(f);

      if (matched && (obligation || recurrence || persistence
                      || !opt->acc_words.empty()
                      || !opt->rej_words.empty()
                      || liveness))
        {
          spot::twa_graph_ptr aut = nullptr;

          if (!opt->acc_words.empty() || !opt->rej_words.empty() || liveness)
            {
              aut = ltl_to_tgba_fm(f, simpl.get_dict(), true);

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

              matched &= !liveness || is_liveness_automaton(aut);
            }

          // Some combinations of options can be simplified.
          if (recurrence && persistence)
            obligation = true;
          if (obligation && recurrence)
            recurrence = false;
          if (obligation && persistence)
            persistence = false;

          // Try a syntactic match before looking at the automata.
          if (matched &&
              !((!persistence || f.is_syntactic_persistence())
                && (!recurrence || f.is_syntactic_recurrence())
                && (!guarantee || f.is_syntactic_guarantee())
                && (!safety || f.is_syntactic_safety())
                && (!obligation || f.is_syntactic_obligation())))
            {
              // Match obligations and subclasses using WDBA minimization.
              // Because this is costly, we compute it later, so that we don't
              // have to compute it on formulas that have been discarded for
              // other reasons.
              if (obligation && (safety || guarantee))
                {
                  if (!aut)
                    aut = ltl_to_tgba_fm(f, simpl.get_dict(), true);

                  auto min = minimize_obligation(aut, f);
                  assert(min);
                  if (aut == min)
                    {
                      // Not an obligation
                      matched = false;
                    }
                  else
                    {
                      spot::scc_info si(min);
                      matched &= !guarantee
                        || is_terminal_automaton(min, &si, true);
                      matched &= !safety || is_safety_automaton(min, &si);
                    }
                }
              else if (obligation) // just obligation, not safety or guarantee
                  matched &= is_obligation(f, aut);
              else if (persistence)
                matched &= spot::is_persistence(f);
              else if (recurrence)
                matched &= spot::is_recurrence(f, aut);
            }
        }

      matched ^= invert;

      if (unique && !unique_set.insert(f).second)
        matched = false;

      timer.stop();

      if (matched)
        {
          if (opt->output_define
              && output_format != count_output
              && output_format != quiet_output)
            {
              // Sort the formulas alphabetically.
              std::map<std::string, spot::formula> m;
              for (auto& p: relmap)
                m.emplace(str_psl(p.first), p.second);
              for (auto& p: m)
                stream_formula(opt->output_define->ostream()
                               << "#define " << p.first << " (",
                               p.second, filename,
                               std::to_string(linenum).c_str()) << ")\n";
            }
          one_match = true;
          output_formula_checked(f, &timer, filename, linenum, prefix, suffix);
          ++match_count;
        }
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

      if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
        exit(err);

      if (jobs.empty())
        jobs.emplace_back("-", 1);

      if (boolean_to_isop && simplification_level == 0)
        simplification_level = 1;
      spot::tl_simplifier_options tlopt(simplification_level);
      tlopt.boolean_to_isop = boolean_to_isop;
      spot::tl_simplifier simpl(tlopt, opt->dict);

      ltl_processor processor(simpl);
      if (processor.run())
        return 2;

      if (output_format == count_output)
        std::cout << match_count << '\n';
      flush_cout();
      return one_match ? 0 : 1;
    });
}
