// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2013, 2015-2019 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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

#include <iostream>
#include <fstream>
#include <argp.h>
#include <cstdlib>
#include "error.h"
#include <vector>

#include "common_setup.hh"
#include "common_output.hh"
#include "common_range.hh"
#include "common_cout.hh"

#include <cassert>
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <cstdlib>
#include <cstring>
#include <spot/tl/formula.hh>
#include <spot/tl/relabel.hh>
#include <spot/gen/formulas.hh>

using namespace spot;

const char argp_program_doc[] ="\
Generate temporal logic formulas from predefined patterns.";

// We reuse the values from gen::ltl_pattern_id as option keys.
// Additional options should therefore start after gen::LTL_END.
enum {
  OPT_POSITIVE = gen::LTL_END,
  OPT_NEGATIVE,
};


#define OPT_ALIAS(o) { #o, 0, nullptr, OPTION_ALIAS, nullptr, 0 }

static const argp_option options[] =
  {
    /**************************************************/
    // Keep this alphabetically sorted (except for aliases).
    { nullptr, 0, nullptr, 0, "Pattern selection:", 1},
    // J. Geldenhuys and H. Hansen (Spin'06): Larger automata and less
    // work for LTL model checking.
    { "and-f", gen::LTL_AND_F, "RANGE", 0, "F(p1)&F(p2)&...&F(pn)", 0 },
    OPT_ALIAS(gh-e),
    { "and-fg", gen::LTL_AND_FG, "RANGE", 0, "FG(p1)&FG(p2)&...&FG(pn)", 0 },
    { "and-gf", gen::LTL_AND_GF, "RANGE", 0, "GF(p1)&GF(p2)&...&GF(pn)", 0 },
    OPT_ALIAS(ccj-phi),
    OPT_ALIAS(gh-c2),
    { "ccj-alpha", gen::LTL_CCJ_ALPHA, "RANGE", 0,
      "F(p1&F(p2&F(p3&...F(pn)))) & F(q1&F(q2&F(q3&...F(qn))))", 0 },
    { "ccj-beta", gen::LTL_CCJ_BETA, "RANGE", 0,
      "F(p&X(p&X(p&...X(p)))) & F(q&X(q&X(q&...X(q))))", 0 },
    { "ccj-beta-prime", gen::LTL_CCJ_BETA_PRIME, "RANGE", 0,
      "F(p&(Xp)&(XXp)&...(X...X(p))) & F(q&(Xq)&(XXq)&...(X...X(q)))", 0 },
    { "dac-patterns", gen::LTL_DAC_PATTERNS, "RANGE", OPTION_ARG_OPTIONAL,
      "Dwyer et al. [FMSP'98] Spec. Patterns for LTL "
      "(range should be included in 1..55)", 0 },
    OPT_ALIAS(spec-patterns),
    { "eh-patterns", gen::LTL_EH_PATTERNS, "RANGE", OPTION_ARG_OPTIONAL,
      "Etessami and Holzmann [Concur'00] patterns "
      "(range should be included in 1..12)", 0 },
    { "fxg-or", gen::LTL_FXG_OR, "RANGE", 0,
      "F(p0 | XG(p1 | XG(p2 | ... XG(pn))))", 0},
    { "gf-equiv", gen::LTL_GF_EQUIV, "RANGE", 0,
      "(GFa1 & GFa2 & ... & GFan) <-> GFz", 0},
    { "gf-equiv-xn", gen::LTL_GF_EQUIV_XN, "RANGE", 0,
      "GF(a <-> X^n(a))", 0},
    { "gf-implies", gen::LTL_GF_IMPLIES, "RANGE", 0,
      "(GFa1 & GFa2 & ... & GFan) -> GFz", 0},
    { "gf-implies-xn", gen::LTL_GF_IMPLIES_XN, "RANGE", 0,
      "GF(a -> X^n(a))", 0},
    { "gh-q", gen::LTL_GH_Q, "RANGE", 0,
      "(F(p1)|G(p2))&(F(p2)|G(p3))&...&(F(pn)|G(p{n+1}))", 0 },
    { "gh-r", gen::LTL_GH_R, "RANGE", 0,
      "(GF(p1)|FG(p2))&(GF(p2)|FG(p3))&... &(GF(pn)|FG(p{n+1}))", 0 },
    { "go-theta", gen::LTL_GO_THETA, "RANGE", 0,
      "!((GF(p1)&GF(p2)&...&GF(pn)) -> G(q->F(r)))", 0 },
    { "gxf-and", gen::LTL_GXF_AND, "RANGE", 0,
      "G(p0 & XF(p1 & XF(p2 & ... XF(pn))))", 0},
    { "hkrss-patterns", gen::LTL_HKRSS_PATTERNS,
      "RANGE", OPTION_ARG_OPTIONAL,
      "Holeček et al. patterns from the Liberouter project "
      "(range should be included in 1..55)", 0 },
    OPT_ALIAS(liberouter-patterns),
    { "kr-n", gen::LTL_KR_N, "RANGE", 0,
      "linear formula with doubly exponential DBA", 0 },
    { "kr-nlogn", gen::LTL_KR_NLOGN, "RANGE", 0,
      "quasilinear formula with doubly exponential DBA", 0 },
    { "kv-psi", gen::LTL_KV_PSI, "RANGE", 0,
      "quadratic formula with doubly exponential DBA", 0 },
    OPT_ALIAS(kr-n2),
    { "ms-example", gen::LTL_MS_EXAMPLE, "RANGE[,RANGE]", 0,
      "GF(a1&X(a2&X(a3&...Xan)))&F(b1&F(b2&F(b3&...&Xbm)))", 0 },
    { "ms-phi-h", gen::LTL_MS_PHI_H, "RANGE", 0,
      "FG(a|b)|FG(!a|Xb)|FG(a|XXb)|FG(!a|XXXb)|...", 0 },
    { "ms-phi-r", gen::LTL_MS_PHI_R, "RANGE", 0,
      "(FGa{n}&GFb{n})|((FGa{n-1}|GFb{n-1})&(...))", 0 },
    { "ms-phi-s", gen::LTL_MS_PHI_S, "RANGE", 0,
      "(FGa{n}|GFb{n})&((FGa{n-1}&GFb{n-1})|(...))", 0 },
    { "or-fg", gen::LTL_OR_FG, "RANGE", 0, "FG(p1)|FG(p2)|...|FG(pn)", 0 },
    OPT_ALIAS(ccj-xi),
    { "or-g", gen::LTL_OR_G, "RANGE", 0, "G(p1)|G(p2)|...|G(pn)", 0 },
    OPT_ALIAS(gh-s),
    { "or-gf", gen::LTL_OR_GF, "RANGE", 0, "GF(p1)|GF(p2)|...|GF(pn)", 0 },
    OPT_ALIAS(gh-c1),
    { "p-patterns", gen::LTL_P_PATTERNS, "RANGE", OPTION_ARG_OPTIONAL,
      "Pelánek [Spin'07] patterns from BEEM "
      "(range should be included in 1..20)", 0 },
    OPT_ALIAS(beem-patterns),
    OPT_ALIAS(p),
    { "pps-arbiter-standard", gen::LTL_PPS_ARBITER_STANDARD, "RANGE", 0,
      "Arbiter with n clients that sent requests (ri) and "
      "receive grants (gi).  Standard semantics.", 0 },
    { "pps-arbiter-strict", gen::LTL_PPS_ARBITER_STRICT, "RANGE", 0,
      "Arbiter with n clients that sent requests (ri) and "
      "receive grants (gi).  Strict semantics.", 0 },
    { "r-left", gen::LTL_R_LEFT, "RANGE", 0, "(((p1 R p2) R p3) ... R pn)", 0 },
    { "r-right", gen::LTL_R_RIGHT, "RANGE", 0, "(p1 R (p2 R (... R pn)))", 0 },
    { "rv-counter", gen::LTL_RV_COUNTER, "RANGE", 0, "n-bit counter", 0 },
    { "rv-counter-carry", gen::LTL_RV_COUNTER_CARRY, "RANGE", 0,
      "n-bit counter w/ carry", 0 },
    { "rv-counter-carry-linear", gen::LTL_RV_COUNTER_CARRY_LINEAR,
      "RANGE", 0, "n-bit counter w/ carry (linear size)", 0 },
    { "rv-counter-linear", gen::LTL_RV_COUNTER_LINEAR, "RANGE", 0,
      "n-bit counter (linear size)", 0 },
    { "sb-patterns", gen::LTL_SB_PATTERNS, "RANGE", OPTION_ARG_OPTIONAL,
      "Somenzi and Bloem [CAV'00] patterns "
      "(range should be included in 1..27)", 0 },
    { "sejk-f", gen::LTL_SEJK_F, "RANGE[,RANGE]", 0,
      "f(0,j)=(GFa0 U X^j(b)), f(i,j)=(GFai U G(f(i-1,j)))", 0 },
    { "sejk-j", gen::LTL_SEJK_J, "RANGE", 0,
      "(GFa1&...&GFan) -> (GFb1&...&GFbn)", 0 },
    { "sejk-k", gen::LTL_SEJK_K, "RANGE", 0,
      "(GFa1|FGb1)&...&(GFan|FGbn)", 0 },
    { "sejk-patterns", gen::LTL_SEJK_PATTERNS, "RANGE", OPTION_ARG_OPTIONAL,
      "φ₁,φ₂,φ₃ from Sikert et al's [CAV'16] paper "
      "(range should be included in 1..3)", 0 },
    { "tv-f1", gen::LTL_TV_F1, "RANGE", 0,
      "G(p -> (q | Xq | ... | XX...Xq)", 0 },
    { "tv-f2", gen::LTL_TV_F2, "RANGE", 0,
      "G(p -> (q | X(q | X(... | Xq)))", 0 },
    { "tv-g1", gen::LTL_TV_G1, "RANGE", 0,
      "G(p -> (q & Xq & ... & XX...Xq)", 0 },
    { "tv-g2", gen::LTL_TV_G2, "RANGE", 0,
      "G(p -> (q & X(q & X(... & Xq)))", 0 },
    { "tv-uu", gen::LTL_TV_UU, "RANGE", 0,
      "G(p1 -> (p1 U (p2 & (p2 U (p3 & (p3 U ...))))))", 0 },
    { "u-left", gen::LTL_U_LEFT, "RANGE", 0, "(((p1 U p2) U p3) ... U pn)", 0 },
    OPT_ALIAS(gh-u),
    { "u-right", gen::LTL_U_RIGHT, "RANGE", 0, "(p1 U (p2 U (... U pn)))", 0 },
    OPT_ALIAS(gh-u2),
    OPT_ALIAS(go-phi),
    RANGE_DOC,
  /**************************************************/
    { nullptr, 0, nullptr, 0, "Output options:", -20 },
    { "negative", OPT_NEGATIVE, nullptr, 0,
      "output the negated versions of all formulas", 0 },
    OPT_ALIAS(negated),
    { "positive", OPT_POSITIVE, nullptr, 0,
      "output the positive versions of all formulas (done by default, unless"
      " --negative is specified without --positive)", 0 },
    { nullptr, 0, nullptr, 0, "The FORMAT string passed to --format may use "
      "the following interpreted sequences:", -19 },
    { "%f", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the formula (in the selected syntax)", 0 },
    { "%F", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the name of the pattern", 0 },
    { "%L", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the argument of the pattern", 0 },
    { "%%", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "a single %", 0 },
    COMMON_LTL_OUTPUT_SPECS,
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Miscellaneous options:", -1 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

struct job
{
  gen::ltl_pattern_id pattern;
  struct range range;
  struct range range2;
};

typedef std::vector<job> jobs_t;
static jobs_t jobs;
bool opt_positive = false;
bool opt_negative = false;

const struct argp_child children[] =
  {
    { &output_argp, 0, nullptr, 0 },
    { &misc_argp, 0, nullptr, 0 },
    { nullptr, 0, nullptr, 0 }
  };

static void
enqueue_job(int pattern, const char* range_str = nullptr)
{
  job j;
  j.pattern = static_cast<gen::ltl_pattern_id>(pattern);
  j.range2.min = -1;
  j.range2.max = -1;
  if (range_str)
    {
      if (gen::ltl_pattern_argc(j.pattern) == 2)
        {
          const char* comma = strchr(range_str, ',');
          if (!comma)
            {
              j.range2 = j.range = parse_range(range_str);
            }
          else
            {
              std::string range1(range_str, comma);
              j.range = parse_range(range1.c_str());
              j.range2 = parse_range(comma + 1);
            }
        }
      else
        {
          j.range = parse_range(range_str);
        }
    }
  else
    {
      j.range.min = 1;
      j.range.max = gen::ltl_pattern_max(j.pattern);
      if (j.range.max == 0)
        error(2, 0, "missing range for %s",
              gen::ltl_pattern_name(j.pattern));
    }
  jobs.push_back(j);
}

static int
parse_opt(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  if (key >= gen::LTL_BEGIN && key < gen::LTL_END)
    {
      enqueue_job(key, arg);
      return 0;
    }
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case OPT_POSITIVE:
      opt_positive = true;
      break;
    case OPT_NEGATIVE:
      opt_negative = true;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}


static void
output_pattern(gen::ltl_pattern_id pattern, int n, int n2)
{
  formula f = gen::ltl_pattern(pattern, n, n2);

  // Make sure we use only "p42"-style of atomic propositions
  // in lbt's output.
  if (output_format == lbt_output && !f.has_lbt_atomic_props())
    f = relabel(f, Pnn);

  std::string args = std::to_string(n);
  if (n2 >= 0)
    args = args + ',' + std::to_string(n2);
  if (opt_positive || !opt_negative)
    {
      output_formula_checked(f, nullptr, gen::ltl_pattern_name(pattern),
                             args.c_str());
    }
  if (opt_negative)
    {
      std::string tmp = "!";
      tmp += gen::ltl_pattern_name(pattern);
      output_formula_checked(formula::Not(f), nullptr, tmp.c_str(),
                             args.c_str());
    }
}

static void
run_jobs()
{
  for (auto& j: jobs)
    {
      int inc = (j.range.max < j.range.min) ? -1 : 1;
      int n = j.range.min;
      for (;;)
        {
          int inc2 = (j.range2.max < j.range2.min) ? -1 : 1;
          int n2 = j.range2.min;
          for (;;)
            {
              output_pattern(j.pattern, n, n2);
              if (n2 == j.range2.max)
                break;
              n2 += inc2;
            }
          if (n == j.range.max)
            break;
          n += inc;
        }
    }
}


int
main(int argc, char** argv)
{
  return protected_main(argv, [&] {
      const argp ap = { options, parse_opt, nullptr, argp_program_doc,
                        children, nullptr, nullptr };

      if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
        exit(err);

      if (jobs.empty())
        error(1, 0, "Nothing to do.  Try '%s --help' for more information.",
              program_name);

      run_jobs();
      flush_cout();
      return 0;
    });
}
