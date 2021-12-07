// -*- coding: utf-8 -*-
// Copyright (C) 2011-2019 Laboratoire de Recherche et Developpement
// de l'Epita (LRDE)
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
#include <spot/ltsmin/ltsmin.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/tl/defaultenv.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twa/twaproduct.hh>
#include <spot/misc/timer.hh>
#include <spot/misc/memusage.hh>
#include <cstring>
#include <spot/kripke/kripkegraph.hh>
#include <spot/twaalgos/hoa.hh>

static void
syntax(char* prog)
{
  // Display the supplied name unless it appears to be a libtool wrapper.
  char* slash = strrchr(prog, '/');
  if (slash && (strncmp(slash + 1, "lt-", 3) == 0))
    prog = slash + 4;

  std::cerr << "usage: " << prog << " [options] model formula\n\
\n\
Options:\n\
  -dDEAD use DEAD as property for marking DEAD states\n\
          (by default DEAD = true)\n\
  -e[ALGO]  run emptiness check, expect an accepting run\n\
  -E[ALGO]  run emptiness check, expect no accepting run\n\
  -C     compute an accepting run (Counterexample) if it exists\n\
  -D     favor a deterministic translation over a small transition\n\
  -gf    output the automaton of the formula in dot format\n\
  -gm    output the model state-space in dot format\n\
  -gK    output the model state-space in Kripke format\n\
  -gp    output the product state-space in dot format\n\
  -T     time the different phases of the execution\n\
  -z     compress states to handle larger models\n\
  -Z     compress states (faster) assuming all values in [0 .. 2^28-1]\n\
";
  exit(1);
}

static int
checked_main(int argc, char **argv)
{
  spot::timer_map tm;

  bool use_timer = false;

  enum { DotFormula, DotModel, DotProduct, EmptinessCheck, Kripke }
  output = EmptinessCheck;
  bool accepting_run = false;
  bool expect_counter_example = false;
  bool deterministic = false;
  char *dead = nullptr;
  int compress_states = 0;

  const char* echeck_algo = "Cou99";

  int dest = 1;
  int n = argc;
  for (int i = 1; i < n; ++i)
    {
      char* opt = argv[i];
      if (*opt == '-')
        {
          switch (*++opt)
            {
            case 'C':
              accepting_run = true;
              break;
            case 'd':
              dead = opt + 1;
              break;
            case 'D':
              deterministic = true;
              break;
            case 'e':
            case 'E':
              {
                echeck_algo = opt + 1;
                if (!*echeck_algo)
                  echeck_algo = "Cou99";

                expect_counter_example = (*opt == 'e');
                output = EmptinessCheck;
                break;
              }
            case 'g':
              switch (opt[1])
                {
                case 'm':
                  output = DotModel;
                  break;
                case 'p':
                  output = DotProduct;
                  break;
                case 'f':
                  output = DotFormula;
                  break;
                case 'K':
                  output = Kripke;
                  break;
                default:
                  goto error;
                }
              break;
            case 'T':
              use_timer = true;
              break;
            case 'z':
              compress_states = 1;
              break;
            case 'Z':
              compress_states = 2;
              break;
            default:
            error:
              std::cerr << "Unknown option `" << argv[i] << "'.\n";
              exit(1);
            }
          --argc;
        }
      else
        {
          argv[dest++] = argv[i];
        }
    }

  if (argc != 3)
    syntax(argv[0]);

  spot::default_environment& env =
    spot::default_environment::instance();


  spot::atomic_prop_set ap;
  auto dict = spot::make_bdd_dict();
  spot::const_kripke_ptr model = nullptr;
  spot::const_twa_ptr prop = nullptr;
  spot::const_twa_ptr product = nullptr;
  spot::emptiness_check_instantiator_ptr echeck_inst = nullptr;
  int exit_code = 0;
  spot::postprocessor post;
  spot::formula deadf = nullptr;
  spot::formula f = nullptr;

  if (!dead || !strcasecmp(dead, "true"))
    {
      deadf = spot::formula::tt();
    }
  else if (!strcasecmp(dead, "false"))
    {
      deadf = spot::formula::ff();
    }
  else
    {
      deadf = env.require(dead);
    }

  if (output == EmptinessCheck)
    {
      const char* err;
      echeck_inst = spot::make_emptiness_check_instantiator(echeck_algo, &err);
      if (!echeck_inst)
        {
          std::cerr << "Failed to parse argument of -e/-E near `"
                    << err <<  "'\n";
          exit_code = 1;
          goto safe_exit;
        }
    }

  tm.start("parsing formula");
  {
    auto pf = spot::parse_infix_psl(argv[2], env, false);
    exit_code = pf.format_errors(std::cerr);
    f = pf.f;
  }
  tm.stop("parsing formula");

  if (exit_code)
    goto safe_exit;

  tm.start("translating formula");
  {
    spot::translator trans(dict);
    if (deterministic)
      trans.set_pref(spot::postprocessor::Deterministic);

    prop = trans.run(&f);
  }
  tm.stop("translating formula");

  atomic_prop_collect(f, &ap);

  if (output != DotFormula)
    {
      tm.start("loading ltsmin model");
      try
        {
          model = spot::ltsmin_model::load(argv[1]).kripke(&ap, dict, deadf,
                                                           compress_states);
        }
      catch (const std::runtime_error& e)
        {
          std::cerr << e.what() << '\n';
        }
      tm.stop("loading ltsmin model");

      if (!model)
        {
          exit_code = 1;
          goto safe_exit;
        }

      if (output == DotModel)
        {
          tm.start("dot output");
          spot::print_dot(std::cout, model);
          tm.stop("dot output");
          goto safe_exit;
        }
      if (output == Kripke)
      {
        tm.start("kripke output");
        spot::print_hoa(std::cout, model);
        tm.stop("kripke output");
        goto safe_exit;
      }
    }

  if (output == DotFormula)
    {
      tm.start("dot output");
      spot::print_dot(std::cout, prop);
      tm.stop("dot output");
      goto safe_exit;
    }

  product = spot::otf_product(model, prop);

  if (output == DotProduct)
    {
      tm.start("dot output");
      spot::print_dot(std::cout, product);
      tm.stop("dot output");
      goto safe_exit;
    }

  assert(echeck_inst);

  {
    auto ec = echeck_inst->instantiate(product);
    bool search_many = echeck_inst->options().get("repeated");
    assert(ec);
    do
      {
        int memused = spot::memusage();
        tm.start("running emptiness check");
        spot::emptiness_check_result_ptr res;
        try
          {
            res = ec->check();
          }
        catch (const std::bad_alloc&)
          {
            std::cerr << "Out of memory during emptiness check.\n";
            if (!compress_states)
              std::cerr << "Try option -z for state compression.\n";
            exit_code = 2;
            exit(exit_code);
          }
        tm.stop("running emptiness check");
        memused = spot::memusage() - memused;

        ec->print_stats(std::cout);
        std::cout << memused << " pages allocated for emptiness check"
                  << std::endl;

        if (expect_counter_example == !res &&
            (!expect_counter_example || ec->safe()))
          exit_code = 1;

        if (!res)
          {
            std::cout << "no accepting run found";
            if (!ec->safe() && expect_counter_example)
              {
                std::cout << " even if expected" << std::endl;
                std::cout << "this may be due to the use of the bit"
                          << " state hashing technique" << std::endl;
                std::cout << "you can try to increase the heap size "
                          << "or use an explicit storage"
                          << std::endl;
              }
            std::cout << std::endl;
            break;
          }
        else if (accepting_run)
          {

            spot::twa_run_ptr run;
            tm.start("computing accepting run");
            try
              {
                run = res->accepting_run();
              }
            catch (const std::bad_alloc&)
              {
                std::cerr
                  << "Out of memory while looking for counterexample.\n";
                exit_code = 2;
                exit(exit_code);
              }
            tm.stop("computing accepting run");

            if (!run)
              {
                std::cout << "an accepting run exists" << std::endl;
              }
            else
              {
                tm.start("reducing accepting run");
                run = run->reduce();
                tm.stop("reducing accepting run");
                tm.start("printing accepting run");
                std::cout << *run;
                tm.stop("printing accepting run");
              }
          }
        else
          {
            std::cout << "an accepting run exists "
                      << "(use -C to print it)" << std::endl;
          }
      }
    while (search_many);
  }

 safe_exit:
  if (use_timer)
    tm.print(std::cout);
  tm.reset_all();                // This helps valgrind.
  return exit_code;
}

int
main(int argc, char **argv)
{
  auto exit_code = checked_main(argc, argv);

  // Additional checks to debug reference counts in formulas.
  assert(spot::fnode::instances_check());
  exit(exit_code);
}
