// -*- coding: utf-8 -*-
// Copyright (C) 2007-2019 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2003-2007 Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
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

#include "config.h"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <fstream>
#include <string>
#include <cstdlib>
#include <spot/tl/print.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/ltl2taa.hh>
#include <spot/twa/bddprint.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/twaalgos/lbtt.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twa/twaproduct.hh>
#include <spot/parseaut/public.hh>
#include <spot/twaalgos/minimize.hh>
#include <spot/taalgos/minimize.hh>
#include <spot/twaalgos/neverclaim.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/strength.hh>
#include <spot/twaalgos/gtec/gtec.hh>
#include <spot/misc/timer.hh>
#include <spot/twaalgos/stats.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/emptiness_stats.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/cycles.hh>
#include <spot/twaalgos/isweakscc.hh>
#include <spot/twaalgos/simulation.hh>
#include <spot/twaalgos/compsusp.hh>
#include <spot/twaalgos/powerset.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/remfin.hh>
#include <spot/twaalgos/complete.hh>
#include <spot/twaalgos/dtbasat.hh>
#include <spot/twaalgos/dtwasat.hh>
#include <spot/twaalgos/stutter.hh>
#include <spot/twaalgos/totgba.hh>

#include <spot/taalgos/tgba2ta.hh>
#include <spot/taalgos/dot.hh>
#include <spot/taalgos/stats.hh>

static void
syntax(char* prog)
{
  // Display the supplied name unless it appears to be a libtool wrapper.
  char* slash = strrchr(prog, '/');
  if (slash && (strncmp(slash + 1, "lt-", 3) == 0))
    prog = slash + 4;

  std::cerr <<
    "Usage: " << prog << " [-f|-l|-taa] [OPTIONS...] formula\n"
    "       " << prog << " [-f|-l|-taa] -F [OPTIONS...] file\n"
    "       " << prog << " -XH [OPTIONS...] file\n"
    "\n"
    "Translate an LTL formula into an automaton, or read the automaton from "
    "a file.\n"
    "Optionally multiply this automaton by another automaton read "
    "from a file.\n"
    "Output the result in various formats, or perform an emptiness check."
    "\n\n"
    "Input options:\n"
    "  -F    read the formula from a file, not from the command line\n"
    "  -XD   do not compute an automaton, read it from an ltl2dstar file\n"
    "  -XDB  like -XD, and convert it to TGBA\n"
    "  -XH   do not compute an automaton, read it from a HOA file\n"
    "  -XL   do not compute an automaton, read it from an LBTT file\n"
    "  -XN   do not compute an automaton, read it from a neverclaim file\n"
    "  -Pfile  multiply the formula automaton with the TGBA read"
    " from `file'\n"
    "  -KPfile multiply the formula automaton with the Kripke"
    " structure from `file'\n"
    "\n"
    "Translation algorithm:\n"
    "  -f    use Couvreur's FM algorithm for LTL (default)\n"
    "  -taa  use Tauriainen's TAA-based algorithm for LTL\n"
    "  -u    use Compositional translation\n"
    "\n"
    "Options for Couvreur's FM algorithm (-f):\n"
    "  -fr   reduce formula at each step of FM\n"
    "          as specified with the -r{1..7} options\n"
    "  -fu   build unambiguous automata\n"
    "  -L    fair-loop approximation (implies -f)\n"
    "  -p    branching postponement (implies -f)\n"
    "  -U[PROPS]  consider atomic properties of the formula as "
    "exclusive events, and\n"
    "        PROPS as unobservables events (implies -f)\n"
    "  -x    try to produce a more deterministic automaton "
    "(implies -f)\n"
    "  -y    do not merge states with same symbolic representation "
    "(implies -f)\n"
    "\n"
    "Options for Tauriainen's TAA-based algorithm (-taa):\n"
    "  -c    enable language containment checks (implies -taa)\n"
    "\n"
    "Formula simplification (before translation):\n"
    "  -r1   reduce formula using basic rewriting\n"
    "  -r2   reduce formula using class of eventuality and universality\n"
    "  -r3   reduce formula using implication between sub-formulae\n"
    "  -r4   reduce formula using all above rules\n"
    "  -r5   reduce formula using tau03\n"
    "  -r6   reduce formula using tau03+\n"
    "  -r7   reduce formula using tau03+ and -r4\n"
    "  -rd   display the reduced formula\n"
    "  -rD   dump statistics about the simplifier cache\n"
    "  -rL   disable basic rewritings producing larger formulas\n"
    "  -ru   lift formulae that are eventual and universal\n"
    "\n"
    "Automaton degeneralization (after translation):\n"
    "  -DT   degeneralize the automaton as a TBA\n"
    "  -DS   degeneralize the automaton as an SBA\n"
    "          (append z/Z, o/O, l/L: to turn on/off options "
    "(default: zol)\n "
    "          z: level resetting, o: adaptive order, "
    "l: level cache)\n"
    "\n"
    "Automaton simplifications (after translation):\n"
    "  -R3   use SCCs to remove useless states and acceptance sets\n"
    "  -R3f  clean more acceptance sets than -R3\n"
    "          "
    "(prefer -R3 over -R3f if you degeneralize with -D, -DS, or -N)\n"
    "  -RDS  reduce the automaton with direct simulation\n"
    "  -RRS  reduce the automaton with reverse simulation\n"
    "  -RIS  iterate both direct and reverse simulations\n"
    "  -Rm   attempt to WDBA-minimize the automaton\n"
    "  -RM   attempt to WDBA-minimize the automaton unless the "
    "result is bigger\n"
    "  -RQ   determinize a TGBA (assuming it's legal!)\n"
    "\n"
    "Automaton conversion:\n"
    "  -M    convert into a det. minimal monitor (implies -R3 or R3b)\n"
    "  -s    convert to explicit automaton, and number states in DFS order\n"
    "  -S    convert to explicit automaton, and number states in BFS order\n"
    "\n"
    "Conversion to Testing Automaton:\n"
    "  -TA   output a Generalized Testing Automaton (GTA),\n"
    "          or a Testing Automaton (TA) with -DS\n"
    "  -lv   add an artificial livelock state to obtain a Single-pass (G)TA\n"
    "  -sp   convert into a single-pass (G)TA without artificial "
    "livelock state\n"
    "  -in   do not use an artificial initial state\n"
    "  -TGTA output a Transition-based Generalized TA\n"
    "  -RT   reduce the (G)TA/TGTA using bisimulation.\n"
    "\n"
    "Options for performing emptiness checks (on TGBA):\n"
    "  -e[ALGO]  run emptiness check, expect and compute an "
    "accepting run\n"
    "  -E[ALGO]  run emptiness check, expect no accepting run\n"
    "  -C    compute an accepting run (Counterexample) if it exists\n"
    "  -CR   compute and replay an accepting run (implies -C)\n"
    "  -G    graph the accepting run seen as an automaton (requires -e)\n"
    "  -m    try to reduce accepting runs, in a second pass\n"
    "Where ALGO should be one of:\n"
    "  Cou99(OPTIONS) (the default)\n"
    "  CVWY90(OPTIONS)\n"
    "  GV04(OPTIONS)\n"
    "  SE05(OPTIONS)\n"
    "  Tau03(OPTIONS)\n"
    "  Tau03_opt(OPTIONS)\n"
    "\n"
    "If no emptiness check is run, the automaton will be output "
    "in dot format\nby default.  This can be "
    "changed with the following options.\n"
    "\n"
    "Output options (if no emptiness check):\n"
    "  -ks   display statistics on the automaton (size only)\n"
    "  -kt   display statistics on the automaton (size + subtransitions)\n"
    "  -K    dump the graph of SCCs in dot format\n"
    "  -KC   list cycles in automaton\n"
    "  -KW   list weak SCCs\n"
    "  -N    output the never clain for Spin (implies -DS)\n"
    "  -NN   output the never clain for Spin, with commented states"
    " (implies -DS)\n"
    "  -O    tell if a formula represents a safety, guarantee, "
    "or obligation property\n"
    "  -t    output automaton in LBTT's format\n"
    "\n"
    "Miscellaneous options:\n"
    "  -0    produce minimal output dedicated to the paper\n"
    "  -8    output UTF-8 formulae\n"
    "  -d    turn on traces during parsing\n"
    "  -T    time the different phases of the translation\n"
    "  -v    display the BDD variables used by the automaton\n";
  exit(2);
}

static int
to_int(const char* s)
{
  char* endptr;
  int res = strtol(s, &endptr, 10);
  if (*endptr)
    {
      std::cerr << "Failed to parse `" << s << "' as an integer.\n";
      exit(1);
    }
  return res;
}

static spot::twa_graph_ptr
ensure_digraph(const spot::twa_ptr& a)
{
  auto aa = std::dynamic_pointer_cast<spot::twa_graph>(a);
  if (aa)
    return aa;
  return spot::make_twa_graph(a, spot::twa::prop_set::all());
}

static int
checked_main(int argc, char** argv)
{
  int exit_code = 0;

  bool debug_opt = false;
  bool paper_opt = false;
  bool utf8_opt = false;
  enum { NoDegen, DegenTBA, DegenSBA } degeneralize_opt = NoDegen;
  enum { TransFM, TransTAA, TransCompo } translation = TransFM;
  bool fm_red = false;
  bool fm_exprop_opt = false;
  bool fm_symb_merge_opt = true;
  bool fm_unambiguous = false;
  bool file_opt = false;
  bool degen_reset = true;
  bool degen_order = false;
  bool degen_cache = true;
  int output = 0;
  int formula_index = 0;
  const char* echeck_algo = nullptr;
  spot::emptiness_check_instantiator_ptr echeck_inst = nullptr;
  bool dupexp = false;
  bool expect_counter_example = false;
  bool accepting_run = false;
  bool accepting_run_replay = false;
  bool from_file = false;
  bool nra2nba = false;
  bool scc_filter = false;
  bool simpltl = false;
  spot::tl_simplifier_options redopt(false, false, false, false,
                                           false, false, false);
  bool simpcache_stats = false;
  bool scc_filter_all = false;
  bool display_reduced_form = false;
  bool post_branching = false;
  bool fair_loop_approx = false;
  bool graph_run_tgba_opt = false;
  bool opt_reduce = false;
  bool opt_minimize = false;
  bool opt_determinize = false;
  unsigned opt_determinize_threshold = 0;
  unsigned opt_o_threshold = 0;
  bool opt_dtwacomp = false;
  bool reject_bigger = false;
  bool opt_monitor = false;
  bool containment = false;
  bool opt_closure = false;
  bool opt_stutterize = false;
  const char* opt_never = nullptr;
  const char* hoa_opt = nullptr;
  auto& env = spot::default_environment::instance();
  spot::atomic_prop_set* unobservables = nullptr;
  spot::twa_ptr system_aut = nullptr;
  auto dict = spot::make_bdd_dict();
  spot::timer_map tm;
  bool use_timer = false;
  bool reduction_dir_sim = false;
  bool reduction_rev_sim = false;
  bool reduction_iterated_sim = false;
  bool opt_bisim_ta = false;
  bool ta_opt = false;
  bool tgta_opt = false;
  bool opt_with_artificial_initial_state = true;
  bool opt_single_pass_emptiness_check = false;
  bool opt_with_artificial_livelock = false;
  bool cs_nowdba = true;
  bool cs_wdba_smaller = false;
  bool cs_nosimul = true;
  bool cs_early_start = false;
  bool cs_oblig = false;
  bool opt_complete = false;
  int opt_dtbasat = -1;
  int opt_dtwasat = -1;

  for (;;)
    {
      if (argc < formula_index + 2)
        syntax(argv[0]);

      ++formula_index;

      if (!strcmp(argv[formula_index], "-0"))
        {
          paper_opt = true;
        }
      else if (!strcmp(argv[formula_index], "-8"))
        {
          utf8_opt = true;
          spot::enable_utf8();
        }
      else if (!strcmp(argv[formula_index], "-c"))
        {
          containment = true;
          translation = TransTAA;
        }
      else if (!strcmp(argv[formula_index], "-C"))
        {
          accepting_run = true;
        }
      else if (!strcmp(argv[formula_index], "-CR"))
        {
          accepting_run = true;
          accepting_run_replay = true;
        }
      else if (!strcmp(argv[formula_index], "-d"))
        {
          debug_opt = true;
        }
      else if (!strcmp(argv[formula_index], "-D"))
        {
          std::cerr << "-D was renamed to -DT\n";
          abort();
        }
      else if (!strcmp(argv[formula_index], "-DC"))
        {
          opt_dtwacomp = true;
        }
      else if (!strncmp(argv[formula_index], "-DS", 3)
               || !strncmp(argv[formula_index], "-DT", 3))
        {
          degeneralize_opt =
            argv[formula_index][2] == 'S' ? DegenSBA : DegenTBA;
          const char* p = argv[formula_index] + 3;
          while (*p)
            {
              switch (*p++)
                {
                case 'o':
                  degen_order = true;
                  break;
                case 'O':
                  degen_order = false;
                  break;
                case 'z':
                  degen_reset = true;
                  break;
                case 'Z':
                  degen_reset = false;
                  break;
                case 'l':
                  degen_cache = true;
                  break;
                case 'L':
                  degen_cache = false;
                  break;
                }
            }
        }
      else if (!strncmp(argv[formula_index], "-e", 2))
        {
          echeck_algo = 2 + argv[formula_index];
          if (!*echeck_algo)
            echeck_algo = "Cou99";

          const char* err;
          echeck_inst =
            spot::make_emptiness_check_instantiator(echeck_algo, &err);
          if (!echeck_inst)
            {
              std::cerr << "Failed to parse argument of -e near `"
                        << err <<  "'\n";
              exit(2);
            }
          expect_counter_example = true;
          output = -1;
        }
      else if (!strncmp(argv[formula_index], "-E", 2))
        {
          const char* echeck_algo = 2 + argv[formula_index];
          if (!*echeck_algo)
            echeck_algo = "Cou99";

          const char* err;
          echeck_inst =
            spot::make_emptiness_check_instantiator(echeck_algo, &err);
          if (!echeck_inst)
            {
              std::cerr << "Failed to parse argument of -e near `"
                        << err << "'\n";
              exit(2);
            }
          expect_counter_example = false;
          output = -1;
        }
      else if (!strcmp(argv[formula_index], "-f"))
        {
          translation = TransFM;
        }
      else if (!strcmp(argv[formula_index], "-fr"))
        {
          fm_red = true;
          translation = TransFM;
        }
      else if (!strcmp(argv[formula_index], "-fu"))
        {
          fm_unambiguous = true;
          fm_exprop_opt = true;
          translation = TransFM;
        }
      else if (!strcmp(argv[formula_index], "-F"))
        {
          file_opt = true;
        }
      else if (!strcmp(argv[formula_index], "-G"))
        {
          accepting_run = true;
          graph_run_tgba_opt = true;
        }
      else if (!strncmp(argv[formula_index], "-H", 2))
        {
          output = 17;
          hoa_opt = argv[formula_index] + 2;
        }
      else if (!strcmp(argv[formula_index], "-ks"))
        {
          output = 12;
        }
      else if (!strcmp(argv[formula_index], "-kt"))
        {
          output = 13;
        }
      else if (!strcmp(argv[formula_index], "-K"))
        {
          output = 10;
        }
      else if (!strncmp(argv[formula_index], "-KP", 3))
        {
          tm.start("reading -KP's argument");
          spot::automaton_parser_options opts;
          opts.debug = debug_opt;
          opts.want_kripke = true;
          auto paut = spot::parse_aut(argv[formula_index] + 3, dict, env, opts);
          if (paut->format_errors(std::cerr))
            return 2;
          system_aut = paut->ks;
          tm.stop("reading -KP's argument");
        }
      else if (!strcmp(argv[formula_index], "-KC"))
        {
          output = 15;
        }
      else if (!strcmp(argv[formula_index], "-KW"))
        {
          output = 16;
        }
      else if (!strcmp(argv[formula_index], "-L"))
        {
          fair_loop_approx = true;
          translation = TransFM;
        }
      else if (!strcmp(argv[formula_index], "-m"))
        {
          opt_reduce = true;
        }
      else if (!strcmp(argv[formula_index], "-N"))
        {
          degeneralize_opt = DegenSBA;
          output = 8;
          opt_never = nullptr;
        }
      else if (!strcmp(argv[formula_index], "-NN"))
        {
          degeneralize_opt = DegenSBA;
          output = 8;
          opt_never = "c";
        }
      else if (!strncmp(argv[formula_index], "-O", 2))
        {
          output = 14;
          opt_minimize = true;
          if (argv[formula_index][2] != 0)
            opt_o_threshold = to_int(argv[formula_index] + 2);
        }
      else if (!strcmp(argv[formula_index], "-p"))
        {
          post_branching = true;
          translation = TransFM;
        }
      else if (!strncmp(argv[formula_index], "-P", 2))
        {
          tm.start("reading -P's argument");

          spot::automaton_parser_options opts;
          opts.debug = debug_opt;
          auto daut = spot::parse_aut(argv[formula_index] + 2, dict, env, opts);
          if (daut->format_errors(std::cerr))
            return 2;
          daut->aut->merge_edges();
          system_aut = daut->aut;
          tm.stop("reading -P's argument");
        }
      else if (!strcmp(argv[formula_index], "-r1"))
        {
          simpltl = true;
          redopt.reduce_basics = true;
        }
      else if (!strcmp(argv[formula_index], "-r2"))
        {
          simpltl = true;
          redopt.event_univ = true;
        }
      else if (!strcmp(argv[formula_index], "-r3"))
        {
          simpltl = true;
          redopt.synt_impl = true;
        }
      else if (!strcmp(argv[formula_index], "-r4"))
        {
          simpltl = true;
          redopt.reduce_basics = true;
          redopt.event_univ = true;
          redopt.synt_impl = true;
        }
      else if (!strcmp(argv[formula_index], "-r5"))
        {
          simpltl = true;
          redopt.containment_checks = true;
        }
      else if (!strcmp(argv[formula_index], "-r6"))
        {
          simpltl = true;
          redopt.containment_checks = true;
          redopt.containment_checks_stronger = true;
        }
      else if (!strcmp(argv[formula_index], "-r7"))
        {
          simpltl = true;
          redopt.reduce_basics = true;
          redopt.event_univ = true;
          redopt.synt_impl = true;
          redopt.containment_checks = true;
          redopt.containment_checks_stronger = true;
        }
      else if (!strcmp(argv[formula_index], "-R1q")
               || !strcmp(argv[formula_index], "-R1t")
               || !strcmp(argv[formula_index], "-R2q")
               || !strcmp(argv[formula_index], "-R2t"))
        {
          // For backward compatibility, make all these options
          // equal to -RDS.
          reduction_dir_sim = true;
        }
      else if (!strcmp(argv[formula_index], "-RRS"))
        {
          reduction_rev_sim = true;
        }
      else if (!strcmp(argv[formula_index], "-R3"))
        {
          scc_filter = true;
        }
      else if (!strcmp(argv[formula_index], "-R3f"))
        {
          scc_filter = true;
          scc_filter_all = true;
        }
      else if (!strcmp(argv[formula_index], "-rd"))
        {
          display_reduced_form = true;
        }
      else if (!strcmp(argv[formula_index], "-rD"))
        {
          simpcache_stats = true;
        }
      else if (!strcmp(argv[formula_index], "-RC"))
        {
          opt_complete = true;
        }
      else if (!strcmp(argv[formula_index], "-RDS"))
        {
          reduction_dir_sim = true;
        }
      else if (!strcmp(argv[formula_index], "-RIS"))
        {
          reduction_iterated_sim = true;
        }
      else if (!strcmp(argv[formula_index], "-rL"))
        {
          simpltl = true;
          redopt.reduce_basics = true;
          redopt.reduce_size_strictly = true;
        }
      else if (!strncmp(argv[formula_index], "-RG", 3))
        {
          if (argv[formula_index][3] != 0)
            opt_dtwasat = to_int(argv[formula_index] + 3);
          else
            opt_dtwasat = 0;
          //output = -1;
        }
      else if (!strcmp(argv[formula_index], "-Rm"))
        {
          opt_minimize = true;
        }
      else if (!strcmp(argv[formula_index], "-RM"))
        {
          opt_minimize = true;
          reject_bigger = true;
        }
      else if (!strncmp(argv[formula_index], "-RQ", 3))
        {
          opt_determinize = true;
          if (argv[formula_index][3] != 0)
            opt_determinize_threshold = to_int(argv[formula_index] + 3);
        }
      else if (!strncmp(argv[formula_index], "-RS", 3))
        {
          if (argv[formula_index][3] != 0)
            opt_dtbasat = to_int(argv[formula_index] + 3);
          else
            opt_dtbasat = 0;
          //output = -1;
        }
      else if (!strcmp(argv[formula_index], "-RT"))
        {
          opt_bisim_ta = true;
        }
      else if (!strcmp(argv[formula_index], "-ru"))
        {
          simpltl = true;
          redopt.event_univ = true;
          redopt.favor_event_univ = true;
        }
      else if (!strcmp(argv[formula_index], "-M"))
        {
          opt_monitor = true;
        }
      else if (!strcmp(argv[formula_index], "-S"))
        {
          dupexp = true;
        }
      else if (!strcmp(argv[formula_index], "-CL"))
        {
          opt_closure = true;
        }
      else if (!strcmp(argv[formula_index], "-ST"))
        {
          opt_stutterize = true;
        }
      else if (!strcmp(argv[formula_index], "-t"))
        {
          output = 6;
        }
      else if (!strcmp(argv[formula_index], "-T"))
        {
          use_timer = true;
        }
      else if (!strcmp(argv[formula_index], "-TA"))
        {
          ta_opt = true;
        }
      else if (!strcmp(argv[formula_index], "-TGTA"))
        {
          tgta_opt = true;
        }
      else if (!strcmp(argv[formula_index], "-lv"))
        {
          opt_with_artificial_livelock = true;
        }
      else if (!strcmp(argv[formula_index], "-sp"))
        {
          opt_single_pass_emptiness_check = true;
        }
      else if (!strcmp(argv[formula_index], "-in"))
        {
          opt_with_artificial_initial_state = false;
        }
      else if (!strcmp(argv[formula_index], "-taa"))
        {
          translation = TransTAA;
        }
      else if (!strncmp(argv[formula_index], "-U", 2))
        {
          unobservables = new spot::atomic_prop_set;
          translation = TransFM;
          // Parse -U's argument.
          const char* tok = strtok(argv[formula_index] + 2, ", \t;");
          while (tok)
            {
              unobservables->insert(env.require(tok));
              tok = strtok(nullptr, ", \t;");
            }
        }
      else if (!strncmp(argv[formula_index], "-u", 2))
        {
          translation = TransCompo;
          const char* c = argv[formula_index] + 2;
          while (*c != 0)
            {
              switch (*c)
                {
                case '2':
                  cs_nowdba = false;
                  cs_wdba_smaller = true;
                  break;
                case 'w':
                  cs_nowdba = false;
                  cs_wdba_smaller = false;
                  break;
                case 's':
                  cs_nosimul = false;
                  break;
                case 'e':
                  cs_early_start = true;
                  break;
                case 'W':
                  cs_nowdba = true;
                  break;
                case 'S':
                  cs_nosimul = true;
                  break;
                case 'E':
                  cs_early_start = false;
                  break;
                case 'o':
                  cs_oblig = true;
                  break;
                case 'O':
                  cs_oblig = false;
                  break;
                default:
                  std::cerr << "Unknown suboption `" << *c
                            << "' for option -u\n";
                }
              ++c;
            }
        }
      else if (!strcmp(argv[formula_index], "-v"))
        {
          output = 5;
        }
      else if (!strcmp(argv[formula_index], "-x"))
        {
          translation = TransFM;
          fm_exprop_opt = true;
        }
      else if (!strcmp(argv[formula_index], "-XD"))
        {
          from_file = true;
        }
      else if (!strcmp(argv[formula_index], "-XDB"))
        {
          from_file = true;
          nra2nba = true;
        }
      else if (!strcmp(argv[formula_index], "-XH"))
        {
          from_file = true;
        }
      else if (!strcmp(argv[formula_index], "-XL"))
        {
          from_file = true;
        }
      else if (!strcmp(argv[formula_index], "-XN")) // now synonym for -XH
        {
          from_file = true;
        }
      else if (!strcmp(argv[formula_index], "-y"))
        {
          translation = TransFM;
          fm_symb_merge_opt = false;
        }
      else
        {
          break;
        }
    }

  if ((graph_run_tgba_opt)
      && (!echeck_inst || !expect_counter_example))
    {
      std::cerr << argv[0] << ": error: -G requires -e.\n";
      exit(1);
    }

  std::string input;

  if (file_opt)
    {
      tm.start("reading formula");
      if (strcmp(argv[formula_index], "-"))
        {
          std::ifstream fin(argv[formula_index]);
          if (!fin)
            {
              std::cerr << "Cannot open " << argv[formula_index] << '\n';
              exit(2);
            }

          if (!std::getline(fin, input, '\0'))
            {
              std::cerr << "Cannot read " << argv[formula_index] << '\n';
              exit(2);
            }
        }
      else
        {
          std::getline(std::cin, input, '\0');
        }
      tm.stop("reading formula");
    }
  else
    {
      input = argv[formula_index];
    }

  spot::formula f = nullptr;
  if (!from_file) // Reading a formula, not reading an automaton from a file.
    {
      switch (translation)
        {
        case TransFM:
        case TransTAA:
        case TransCompo:
          {
            tm.start("parsing formula");
            auto pf = spot::parse_infix_psl(input, env, debug_opt);
            tm.stop("parsing formula");
            exit_code = pf.format_errors(std::cerr);
            f = pf.f;
          }
          break;
        }
    }

  if (f || from_file)
    {
      spot::twa_ptr a = nullptr;
      bool assume_sba = false;

      if (from_file)
        {
          tm.start("parsing hoa");
          spot::automaton_parser_options opts;
          opts.debug = debug_opt;
          auto daut = spot::parse_aut(input, dict, env, opts);
          tm.stop("parsing hoa");
          if (daut->format_errors(std::cerr))
            return 2;
          daut->aut->merge_edges();
          a = daut->aut;

          if (nra2nba)
            a = spot::to_generalized_buchi(daut->aut);
          assume_sba = a->is_sba().is_true();
        }
      else
        {
          spot::tl_simplifier* simp = nullptr;
          if (simpltl)
            simp = new spot::tl_simplifier(redopt, dict);

          if (simp)
            {
              tm.start("reducing formula");
              spot::formula t = simp->simplify(f);
              tm.stop("reducing formula");
              f = t;
              if (display_reduced_form)
                {
                  if (utf8_opt)
                    print_utf8_psl(std::cout, f) << '\n';
                  else
                    print_psl(std::cout, f) << '\n';
                }
              // This helps ltl_to_tgba_fm() to order BDD variables in
              // a more natural way.
              simp->clear_as_bdd_cache();
            }

          if (f.is_psl_formula()
              && !f.is_ltl_formula()
              && (translation != TransFM && translation != TransCompo))
            {
              std::cerr << "Only the FM algorithm can translate PSL formulae;"
                        << " I'm using it for this formula.\n";
              translation = TransFM;
            }

          tm.start("translating formula");
          switch (translation)
            {
            case TransFM:
              a = spot::ltl_to_tgba_fm(f, dict, fm_exprop_opt,
                                       fm_symb_merge_opt,
                                       post_branching,
                                       fair_loop_approx,
                                       unobservables,
                                       fm_red ? simp : nullptr,
                                       fm_unambiguous);
              break;
            case TransCompo:
              {
                a = spot::compsusp(f, dict,
                                   cs_nowdba, cs_nosimul, cs_early_start,
                                   false, cs_wdba_smaller, cs_oblig);
                break;
              }
            case TransTAA:
              a = spot::ltl_to_taa(f, dict, containment);
              break;
            }
          tm.stop("translating formula");

          if (simp && simpcache_stats)
            {
              simp->print_stats(std::cerr);
              bddStat s;
              bdd_stats(&s);
              std::cerr << "BDD produced: " << s.produced
                        << "\n    nodenum: " << s.nodenum
                        << "\n    maxnodenum: " << s.maxnodenum
                        << "\n    freenodes: " <<  s.freenodes
                        << "\n    minfreenodes: " << s.minfreenodes
                        << "\n    varnum: " <<  s.varnum
                        << "\n    cachesize: " << s.cachesize
                        << "\n    gbcnum: " << s.gbcnum
                        << '\n';
              bdd_fprintstat(stderr);
              dict->dump(std::cerr);
            }
          delete simp;
        }

      if (opt_monitor && !scc_filter)
        scc_filter = true;

      // Remove dead SCCs and useless acceptance conditions before
      // degeneralization.
      if (scc_filter)
        {
          tm.start("SCC-filter");
          if (a->prop_state_acc().is_true() & !scc_filter_all)
            a = spot::scc_filter_states(ensure_digraph(a));
          else
            a = spot::scc_filter(ensure_digraph(a), scc_filter_all);
          tm.stop("SCC-filter");
          assume_sba = false;
        }

      bool wdba_minimization_is_success = false;
      if (opt_minimize)
        {
          auto aa = ensure_digraph(a);
          tm.start("obligation minimization");
          auto minimized = minimize_obligation(aa, f, nullptr, reject_bigger);
          tm.stop("obligation minimization");

          if (!minimized)
            {
              // if (!f)
                {
                  std::cerr << "Error: Without a formula I cannot make "
                            << "sure that the automaton built with -Rm\n"
                            << "       is correct.\n";
                  exit(2);
                }
            }
          else if (minimized == aa)
            {
              minimized = nullptr;
            }
          else
            {
              a = minimized;
              wdba_minimization_is_success = true;
              // When the minimization succeed, simulation is useless.
              reduction_dir_sim = false;
              reduction_rev_sim = false;
              reduction_iterated_sim = false;
              assume_sba = true;
            }
        }

      if (reduction_dir_sim && !reduction_iterated_sim)
        {
          tm.start("direct simulation");
          a = spot::simulation(ensure_digraph(a));
          tm.stop("direct simulation");
          assume_sba = false;
        }

      if (reduction_rev_sim && !reduction_iterated_sim)
        {
          tm.start("reverse simulation");
          a = spot::cosimulation(ensure_digraph(a));
          tm.stop("reverse simulation");
          assume_sba = false;
        }


      if (reduction_iterated_sim)
        {
          tm.start("Reduction w/ iterated simulations");
          a = spot::iterated_simulations(ensure_digraph(a));
          tm.stop("Reduction w/ iterated simulations");
          assume_sba = false;
        }

      if (scc_filter && (reduction_dir_sim || reduction_rev_sim))
        {
          tm.start("SCC-filter post-sim");
          a = spot::scc_filter(ensure_digraph(a), scc_filter_all);
          tm.stop("SCC-filter post-sim");
        }

      unsigned int n_acc = a->acc().num_sets();
      if (echeck_inst
          && degeneralize_opt == NoDegen
          && n_acc > 1
          && echeck_inst->max_sets() < n_acc)
        {
          degeneralize_opt = DegenTBA;
          assume_sba = false;
        }

      if (!assume_sba && !opt_monitor)
        {
          if (degeneralize_opt == DegenTBA)
            {
              a = spot::degeneralize_tba(ensure_digraph(a),
                                         degen_reset, degen_order, degen_cache);
            }
          else if (degeneralize_opt == DegenSBA)
            {
              tm.start("degeneralization");
              a = spot::degeneralize(ensure_digraph(a),
                                     degen_reset, degen_order, degen_cache);
              tm.stop("degeneralization");
              assume_sba = true;
            }
        }

      if (opt_determinize && a->acc().num_sets() <= 1
          && (!f || f.is_syntactic_recurrence()))
        {
          tm.start("determinization 2");
          auto determinized = tba_determinize(ensure_digraph(a), 0,
                                              opt_determinize_threshold);
          tm.stop("determinization 2");
          if (determinized)
            a = determinized;
        }

      if (opt_monitor)
        {
          tm.start("Monitor minimization");
          a = minimize_monitor(ensure_digraph(a));
          tm.stop("Monitor minimization");
          assume_sba = false;         // All states are accepting, so double
                                // circles in the dot output are
                                // pointless.
        }

      if (degeneralize_opt != NoDegen || opt_determinize)
        {
          if (reduction_dir_sim && !reduction_iterated_sim)
            {
              tm.start("direct simulation 2");
              a = spot::simulation(ensure_digraph(a));
              tm.stop("direct simulation 2");
              assume_sba = false;
            }

          if (reduction_rev_sim && !reduction_iterated_sim)
            {
              tm.start("reverse simulation 2");
              a = spot::cosimulation(ensure_digraph(a));
              tm.stop("reverse simulation 2");
              assume_sba = false;
            }

          if (reduction_iterated_sim)
            {
              tm.start("Reduction w/ iterated simulations");
              a = spot::iterated_simulations(ensure_digraph(a));
              tm.stop("Reduction w/ iterated simulations");
              assume_sba = false;
            }
        }

      if (opt_complete)
        {
          tm.start("completion");
          a = complete(a);
          tm.stop("completion");
        }

      if (opt_dtbasat >= 0)
        {
          tm.start("dtbasat");
          auto satminimized =
            dtba_sat_synthetize(ensure_digraph(a), opt_dtbasat);
          tm.stop("dtbasat");
          if (satminimized)
            a = satminimized;
        }
      else if (opt_dtwasat >= 0)
        {
          tm.start("dtwasat");
          auto satminimized = dtwa_sat_minimize
            (ensure_digraph(a), opt_dtwasat,
             spot::acc_cond::acc_code::generalized_buchi(opt_dtwasat));
          tm.stop("dtwasat");
          if (satminimized)
            a = satminimized;
        }

      if (opt_dtwacomp)
        {
          tm.start("DTωA complement");
          a = remove_fin(dualize(ensure_digraph(a)));
          tm.stop("DTωA complement");
        }

      if (opt_determinize || opt_dtwacomp || opt_dtbasat >= 0
          || opt_dtwasat >= 0)
        {
          if (scc_filter && (reduction_dir_sim || reduction_rev_sim))
            {
              tm.start("SCC-filter post-sim");
              auto aa = spot::down_cast<spot::const_twa_graph_ptr>(a);
              // Do not filter_all for SBA
              a = spot::scc_filter(aa, assume_sba ?
                                   false : scc_filter_all);
              tm.stop("SCC-filter post-sim");
            }
        }

      if (opt_closure)
        {
          a = closure(ensure_digraph(a));
        }

      if (opt_stutterize)
        {
          a = sl(ensure_digraph(a));
        }

      if (opt_monitor)
        {
          tm.start("Monitor minimization");
          a = minimize_monitor(ensure_digraph(a));
          tm.stop("Monitor minimization");
          assume_sba = false;         // All states are accepting, so double
                                // circles in the dot output are
                                // pointless.
        }

      if (dupexp)
        a = make_twa_graph(a, spot::twa::prop_set::all());

      //TA, STA, GTA, SGTA and TGTA
      if (ta_opt || tgta_opt)
        {
          bdd atomic_props_set_bdd = atomic_prop_collect_as_bdd(f, a);

          if (ta_opt)
            {
              tm.start("conversion to TA");
              auto testing_automaton
                  = tgba_to_ta(a, atomic_props_set_bdd, degeneralize_opt
                      == DegenSBA, opt_with_artificial_initial_state,
                      opt_single_pass_emptiness_check,
                      opt_with_artificial_livelock);
              tm.stop("conversion to TA");

              if (opt_bisim_ta)
                {
                  tm.start("TA bisimulation");
                  testing_automaton = minimize_ta(testing_automaton);
                  tm.stop("TA bisimulation");
                }

              if (output != -1)
                {
                  tm.start("producing output");
                  switch (output)
                    {
                    case 0:
                      spot::print_dot(std::cout, testing_automaton);
                      break;
                    case 12:
                      stats_reachable(testing_automaton).dump(std::cout);
                      break;
                    default:
                      std::cerr << "unsupported output option\n";
                      exit(1);
                    }
                  tm.stop("producing output");
                }
              a = nullptr;
              output = -1;
            }
          if (tgta_opt)
            {
              auto tgta = tgba_to_tgta(a, atomic_props_set_bdd);
              if (opt_bisim_ta)
                {
                  tm.start("TA bisimulation");
                  a = minimize_tgta(tgta);
                  tm.stop("TA bisimulation");
                }
              else
                {
                  a = tgta;
                }

              if (output != -1)
                {
                  tm.start("producing output");
                  switch (output)
                    {
                    case 0:
                      spot::print_dot(std::cout, std::dynamic_pointer_cast
                                            <spot::tgta_explicit>(a)->get_ta());
                      break;
                    case 12:
                      stats_reachable(a).dump(std::cout);
                      break;
                    default:
                      std::cerr << "unsupported output option\n";
                      exit(1);
                    }
                  tm.stop("producing output");
                }
              output = -1;
            }
        }

      if (system_aut)
        {
          a = spot::otf_product(system_aut, a);

          assume_sba = false;

          unsigned int n_acc = a->acc().num_sets();
          if (echeck_inst
              && degeneralize_opt == NoDegen
              && n_acc > 1
              && echeck_inst->max_sets() < n_acc)
            degeneralize_opt = DegenTBA;
          if (degeneralize_opt == DegenTBA)
            {
              tm.start("degeneralize product");
              a = spot::degeneralize_tba(ensure_digraph(a),
                                         degen_reset,
                                         degen_order,
                                         degen_cache);
              tm.stop("degeneralize product");
            }
          else if (degeneralize_opt == DegenSBA)
            {
              tm.start("degeneralize product");
              a = spot::degeneralize(ensure_digraph(a),
                                     degen_reset,
                                     degen_order,
                                     degen_cache);
              tm.stop("degeneralize product");
              assume_sba = true;
            }
        }


      if (echeck_inst
          && (a->acc().num_sets() < echeck_inst->min_sets()))
        {
          if (!paper_opt)
            {
              std::cerr << echeck_algo << " requires at least "
                        << echeck_inst->min_sets()
                        << " acceptance sets.\n";
              exit(1);
            }
          else
            {
              std::cout << std::endl;
              exit(0);
            }
        }

      if (f)
        a->set_named_prop("automaton-name", new std::string(str_psl(f)));

      if (output != -1)
        {
          tm.start("producing output");
          switch (output)
            {
            case 0:
              spot::print_dot(std::cout, a);
              break;
            case 5:
              a->get_dict()->dump(std::cout);
              break;
            case 6:
              spot::print_lbtt(std::cout, a);
              break;
            case 8:
              {
                assert(degeneralize_opt == DegenSBA);
                if (assume_sba)
                  spot::print_never_claim(std::cout, a, opt_never);
                else
                  {
                    // It is possible that we have applied other
                    // operations to the automaton since its initial
                    // degeneralization.  Let's degeneralize again!
                    auto s = spot::degeneralize(ensure_digraph(a), degen_reset,
                                                degen_order, degen_cache);
                    spot::print_never_claim(std::cout, s, opt_never);
                  }
                break;
              }
            case 10:
              dump_scc_info_dot(std::cout, ensure_digraph(a));
              break;
            case 12:
              stats_reachable(a).dump(std::cout);
              break;
            case 13:
              sub_stats_reachable(a).dump(std::cout);
              std::cout << "nondeterministic states: "
                        << count_nondet_states(ensure_digraph(a)) << std::endl;
              break;
            case 14:
              if (!wdba_minimization_is_success)
                {
                  std::cout << "this is not an obligation property";
                  auto tmp = tba_determinize_check(ensure_digraph(a),
                                                   0, opt_o_threshold,
                                                   f, nullptr);
                  if (tmp && tmp != a)
                    std::cout << ", but it is a recurrence property";
                }
              else
                {
                  bool g = is_terminal_automaton(ensure_digraph(a),
                                                 nullptr, true);
                  bool s = is_safety_automaton(ensure_digraph(a));
                  if (g && !s)
                    {
                      std::cout << "this is a guarantee property (hence, "
                                << "an obligation property)";
                    }
                  else if (s && !g)
                    {
                      std::cout << "this is a safety property (hence, "
                                << "an obligation property)";
                    }
                  else if (s && g)
                    {
                      std::cout << "this is a guarantee and a safety property"
                                << " (and of course an obligation property)";
                    }
                  else
                    {
                      std::cout << "this is an obligation property that is "
                                << "neither a safety nor a guarantee";
                    }
                }
              std::cout << std::endl;

              break;
            case 15:
              {
                spot::scc_info m(ensure_digraph(a));
                spot::enumerate_cycles c(m);
                unsigned max = m.scc_count();
                for (unsigned n = 0; n < max; ++n)
                  {
                    std::cout << "Cycles in SCC #" << n << std::endl;
                    c.run(n);
                  }
                break;
              }
            case 16:
              {
                spot::scc_info m(ensure_digraph(a));
                unsigned max = m.scc_count();
                for (unsigned n = 0; n < max; ++n)
                  {
                    bool w = spot::is_weak_scc(m, n);
                    std::cout << "SCC #" << n
                              << (w ? " is weak" : " is not weak")
                              << std::endl;
                  }
                break;
              }
            case 17:
              {
                print_hoa(std::cout, a, hoa_opt) << '\n';
                break;
              }
            default:
              SPOT_UNREACHABLE();
            }
          tm.stop("producing output");
        }

      if (echeck_inst)
        {
          auto ec = echeck_inst->instantiate(a);
          bool search_many = echeck_inst->options().get("repeated");
          assert(ec);
          do
            {
              tm.start("running emptiness check");
              auto res = ec->check();
              tm.stop("running emptiness check");

              if (paper_opt)
                {
                  std::ios::fmtflags old = std::cout.flags();
                  std::cout << std::left << std::setw(25)
                            << echeck_algo << ", ";
                  spot::twa_statistics a_size =
                                        spot::stats_reachable(ec->automaton());
                  std::cout << std::right << std::setw(10)
                            << a_size.states << ", "
                            << std::right << std::setw(10)
                            << a_size.edges << ", ";
                  std::cout << ec->automaton()->acc().num_sets()
                            << ", ";
                  auto ecs = ec->emptiness_check_statistics();
                  if (ecs)
                    std::cout << std::right << std::setw(10)
                              << ecs->states() << ", "
                              << std::right << std::setw(10)
                              << ecs->transitions() << ", "
                              << std::right << std::setw(10)
                              << ecs->max_depth();
                  else
                    std::cout << "no stats, , ";
                  if (res)
                    std::cout << ", accepting run found";
                  else
                    std::cout << ", no accepting run found";
                  std::cout << std::endl;
                  std::cout << std::setiosflags(old);
                }
              else
                {
                  if (!graph_run_tgba_opt)
                    ec->print_stats(std::cout);
                  if (expect_counter_example != !!res &&
                      (!expect_counter_example || ec->safe()))
                    exit_code = 1;

                  if (!res)
                    {
                      std::cout << "no accepting run found";
                      if (!ec->safe() && expect_counter_example)
                        {
                          std::cout << " even if expected\n";
                          std::cout << "this may be due to the use of the bit"
                                    << " state hashing technique\n";
                          std::cout << "you can try to increase the heap size "
                                    << "or use an explicit storage"
                                    << std::endl;
                        }
                      std::cout << std::endl;
                      break;
                    }
                  else if (accepting_run)
                    {

                      tm.start("computing accepting run");
                      auto run = res->accepting_run();
                      tm.stop("computing accepting run");

                      if (!run)
                        {
                          std::cout << "an accepting run exists\n";
                        }
                      else
                        {
                          if (opt_reduce)
                            {
                              tm.start("reducing accepting run");
                              run = run->reduce();
                              tm.stop("reducing accepting run");
                            }
                          if (accepting_run_replay)
                            {
                              tm.start("replaying acc. run");
                              if (!run->replay(std::cout, true))
                                exit_code = 1;
                              tm.stop("replaying acc. run");
                            }
                          else
                            {
                              tm.start("printing accepting run");
                              if (graph_run_tgba_opt)
                                spot::print_dot(std::cout, run->as_twa());
                              else
                                std::cout << *run;
                              tm.stop("printing accepting run");
                            }
                        }
                    }
                  else
                    {
                      std::cout << "an accepting run exists "
                                << "(use -C to print it)\n";
                    }
                }
            }
          while (search_many);
        }
    }
  else
    {
      exit_code = 1;
    }

  if (use_timer)
    tm.print(std::cout);

  delete unobservables;
  return exit_code;
}


int
main(int argc, char** argv)
{
  int exit_code = checked_main(argc, argv);
  assert(spot::fnode::instances_check());
  return exit_code;
}
