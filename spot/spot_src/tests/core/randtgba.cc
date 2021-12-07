// -*- coding: utf-8 -*-
// Copyright (C) 2008-2012, 2014-2019 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2004, 2005 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
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
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <utility>
#include <set>
#include <vector>
#include <spot/tl/parse.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/print.hh>
#include <spot/tl/length.hh>
#include <spot/tl/simplify.hh>
#include <spot/twaalgos/randomgraph.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/stats.hh>
#include <spot/tl/defaultenv.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/misc/random.hh>
#include <spot/misc/optionmap.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/product.hh>
#include <spot/misc/timer.hh>

#include <spot/twaalgos/ltl2tgba_fm.hh>

#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/emptiness_stats.hh>

struct ec_algo
{
  std::string name;
  spot::emptiness_check_instantiator_ptr inst;
};

const char* default_algos[] = {
  "Cou99(!poprem)",
  "Cou99(!poprem shy !group)",
  "Cou99(!poprem shy group)",
  "Cou99(poprem)",
  "Cou99(poprem shy !group)",
  "Cou99(poprem shy group)",
  "Cou99new",
  "Cou99abs",
  "CVWY90",
  "CVWY90(bsh=4K)",
  "GV04",
  "SE05",
  "SE05(bsh=4K)",
  "Tau03",
  "Tau03_opt",
  "Tau03_opt(condstack)",
  "Tau03_opt(condstack ordering)",
  "Tau03_opt(condstack ordering !weights)",
  nullptr
};

std::vector<ec_algo> ec_algos;

static spot::emptiness_check_ptr
cons_emptiness_check(int num, spot::const_twa_graph_ptr a,
                     const spot::const_twa_graph_ptr& degen,
                     unsigned int n_acc)
{
  auto inst = ec_algos[num].inst;
  if (n_acc < inst->min_sets() || n_acc > inst->max_sets())
    a = degen;
  if (a)
    return inst->instantiate(a);
  return nullptr;
}

static void
syntax(char* prog)
{
  std::cerr
    << "Usage: " << prog
    << (" [OPTIONS...] PROPS...\n\n"
        "General Options:\n"
        "  -0      suppress default output, just generate the graph"
        " in memory\n"
        "  -1      produce minimal output (for our paper)\n"
        "  -g      output graph in dot format\n"
        "  -s N    seed for the random number generator\n"
        "  -z      display statistics about emptiness-check algorithms\n"
        "  -Z      like -z, but print extra statistics after the run"
        " of each algorithm\n\n"
        "Graph Generation Options:\n"
        "  -a N F  number of acceptance conditions and probability that"
        " one is true\n"
        "            [0 0.0]\n"
        "  -d F    density of the graph [0.2]\n"
        "  -n N    number of nodes of the graph [20]\n"
        "  -t F    probability of the atomic propositions to be true"
        " [0.5]\n"
        "  -det    generate a deterministic and complete graph [false]\n\n"
        "Emptiness-Check Options:\n"
        "  -A FILE use all algorithms listed in FILE\n"
        "  -D      degeneralize TGBA for emptiness-check algorithms that"
        " would\n"
        "            otherwise be skipped (implies -e)\n"
        "  -e N    compare result of all "
        "emptiness checks on N randomly generated graphs\n"
        "  -m      try to reduce runs, in a second pass (implies -r)\n"
        "  -R N    repeat each emptiness-check and accepting run "
        "computation N times\n"
        "  -r      compute and replay accepting runs (implies -e)\n"
        "  ar:MODE select the mode MODE for accepting runs computation "
        "(implies -r)\n\n"
        "Where:\n"
        "  F      are floats between 0.0 and 1.0 inclusive\n"
        "  E      are floating values\n"
        "  S      are `KEY=E, KEY=E, ...' strings\n"
        "  N      are positive integers\n"
        "  PROPS  are the atomic properties to use on transitions\n"
        "Use -dp to see the list of KEYs.\n\n"
        "When -i is used, a random graph a synchronized with"
        " each formula.\nIf -e N is additionally used"
        " N random graphs are generated for each formula.\n");
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

static int
to_int_pos(const char* s, const char* arg)
{
  int res = to_int(s);
  if (res <= 0)
    {
      std::cerr << "argument of " << arg
                << " (" << res << ") must be positive\n";
      exit(1);
    }
  return res;
}

static int
to_int_nonneg(const char* s, const char* arg)
{
  int res = to_int(s);
  if (res < 0)
    {
      std::cerr << "argument of " << arg
                << " (" << res << ") must be nonnegative\n";
      exit(1);
    }
  return res;
}

static float
to_float(const char* s)
{
  char* endptr;
  // Do not use strtof(), it does not exist on Solaris 9.
  float res = strtod(s, &endptr);
  if (*endptr)
    {
      std::cerr << "Failed to parse `" << s << "' as a float.\n";
      exit(1);
    }
  return res;
}

static float
to_float_nonneg(const char* s, const char* arg)
{
  float res = to_float(s);
  if (res < 0)
    {
      std::cerr << "argument of " << arg
                << " (" << res << ") must be nonnegative\n";
      exit(1);
    }
  return res;
}

// Convertors using for statistics:

template <typename T>
T
id(const char*, unsigned x)
{
  return static_cast<T>(x);
}

spot::twa_statistics prod_stats;

static float
prod_conv(const char* name, unsigned x)
{
  float y = static_cast<float>(x);
  if (!strcmp(name, "transitions"))
    return y / prod_stats.edges * 100.0;
  return y / prod_stats.states * 100.0;
}

template <typename T, T (*convertor)(const char*, unsigned) = id<T> >
struct stat_collector
{
  struct one_stat
  {
    T min;
    T max;
    T tot;
    unsigned n;

    one_stat()
      : n(0)
    {
    }

    void
    count(T val)
    {
      if (n++)
        {
          min = std::min(min, val);
          max = std::max(max, val);
          tot += val;
        }
      else
        {
          max = min = tot = val;
        }
    }
  };

  typedef std::map<std::string, one_stat> alg_1stat_map;
  typedef std::map<std::string, alg_1stat_map> stats_alg_map;
  stats_alg_map stats;

  bool
  empty()
  {
    return stats.empty();
  }

  void
  count(const std::string& algorithm, const spot::unsigned_statistics* s)
  {
    if (!s)
      return;
    spot::unsigned_statistics::stats_map::const_iterator i;
    for (i = s->stats.begin(); i != s->stats.end(); ++i)
      {
        auto u = (s->*i->second)();
        auto t = convertor(i->first, u);
        stats[i->first][algorithm].count(t);
      }
  }

  std::ostream&
  display(std::ostream& os,
          const alg_1stat_map& m, const std::string& title,
          bool total = true) const
  {
    std::ios::fmtflags old = os.flags();
    os << std::setw(25) << "" << " | "
       << std::setw(30) << std::left << title << std::right << '|' << std::endl
       << std::setw(25) << "algorithm"
       << " |   min   < mean  < max | total |  n"
       << std::endl
       << std::setw(64) << std::setfill('-') << "" << std::setfill(' ')
       << std::endl;
    os << std::right << std::fixed << std::setprecision(1);
    for (typename alg_1stat_map::const_iterator i = m.begin();
         i != m.end(); ++i)
      {
        os << std::setw(25) << i->first << " |"
           << std::setw(6) << i->second.min
           << ' '
           << std::setw(8)
           << static_cast<float>(i->second.tot) / i->second.n
           << ' '
           << std::setw(6) << i->second.max
           << " |";
        if (total)
          os << std::setw(6) << i->second.tot;
        else
          os << "      ";
        os << " |"
           << std::setw(4) << i->second.n
           << std::endl;
      }
    os << std::setw(64) << std::setfill('-') << "" << std::setfill(' ')
       << std::endl;
    os << std::setiosflags(old);
    return os;
  }

  std::ostream&
  display(std::ostream& os, bool total = true) const
  {
    typename stats_alg_map::const_iterator i;
    for (i = stats.begin(); i != stats.end(); ++i)
      display(os, i->second, i->first, total);
    return os;
  }


};

struct ar_stat
{
  int min_prefix;
  int max_prefix;
  int tot_prefix;
  int min_cycle;
  int max_cycle;
  int tot_cycle;
  int min_run;
  int max_run;
  int n;

  ar_stat()
    : n(0)
  {
  }

  void
  count(const spot::const_twa_run_ptr& run)
  {
    int p = run->prefix.size();
    int c = run->cycle.size();
    if (n++)
      {
        min_prefix = std::min(min_prefix, p);
        max_prefix = std::max(max_prefix, p);
        tot_prefix += p;
        min_cycle = std::min(min_cycle, c);
        max_cycle = std::max(max_cycle, c);
        tot_cycle += c;
        min_run = std::min(min_run, c + p);
        max_run = std::max(max_run, c + p);
      }
    else
      {
        min_prefix = max_prefix = tot_prefix = p;
        min_cycle = max_cycle = tot_cycle = c;
        min_run = max_run = c + p;
      }
  }
};

stat_collector<unsigned> sc_ec;
stat_collector<unsigned> sc_arc;

typedef stat_collector<float, prod_conv> ec_ratio_stat_type;
ec_ratio_stat_type glob_ec_ratio_stats;
typedef std::map<int, ec_ratio_stat_type > ec_ratio_stats_type;
ec_ratio_stats_type ec_ratio_stats;

ec_ratio_stat_type arc_ratio_stats;

typedef std::map<std::string, ar_stat> ar_stats_type;
ar_stats_type ar_stats;                // Statistics about accepting runs.
ar_stats_type mar_stats;        // ... about minimized accepting runs.


static void
print_ar_stats(ar_stats_type& ar_stats, const std::string& s)
{
  std::ios::fmtflags old = std::cout.flags();
  std::cout << std::endl << s << std::endl;
  std::cout << std::right << std::fixed << std::setprecision(1);

  std::cout << std::setw(25) << ""
            << " |         prefix        |         cycle         |"
            << std::endl
            << std::setw(25) << "algorithm"
            << " |   min   < mean  < max |   min   < mean  < max |   n"
            << std::endl
            << std::setw(79) << std::setfill('-') << "" << std::setfill(' ')
            << std::endl;
  for (ar_stats_type::const_iterator i = ar_stats.begin();
           i != ar_stats.end(); ++i)
    std::cout << std::setw(25) << i->first << " |"
              << std::setw(6) << i->second.min_prefix
              << ' '
              << std::setw(8)
              << static_cast<float>(i->second.tot_prefix) / i->second.n
              << ' '
              << std::setw(6) << i->second.max_prefix
              << " |"
              << std::setw(6) << i->second.min_cycle
              << ' '
              << std::setw(8)
              << static_cast<float>(i->second.tot_cycle) / i->second.n
              << ' '
              << std::setw(6) << i->second.max_cycle
              << " |"
              << std::setw(4) << i->second.n
              << std::endl;
  std::cout << std::setw(79) << std::setfill('-') << "" << std::setfill(' ')
            << std::endl
            << std::setw(25) << ""
            << " |          runs         |         total         |"
            << std::endl <<
        std::setw(25) << "algorithm"
            << " |   min   < mean  < max |  pre.   cyc.     runs |   n"
            << std::endl
            << std::setw(79) << std::setfill('-') << "" << std::setfill(' ')
            << std::endl;
  for (ar_stats_type::const_iterator i = ar_stats.begin();
           i != ar_stats.end(); ++i)
    std::cout << std::setw(25) << i->first << " |"
              << std::setw(6)
              << i->second.min_run
              << ' '
              << std::setw(8)
              << static_cast<float>(i->second.tot_prefix
                                    + i->second.tot_cycle) / i->second.n
              << ' '
              << std::setw(6)
              << i->second.max_run
              << " |"
              << std::setw(6) << i->second.tot_prefix
              << ' '
              << std::setw(6) << i->second.tot_cycle
              << ' '
              << std::setw(8) << i->second.tot_prefix + i->second.tot_cycle
              << " |"
              << std::setw(4) << i->second.n
              << std::endl;
  std::cout << std::setiosflags(old);
}

int
main(int argc, char** argv)
{
  bool opt_paper = false;

  int opt_n_acc = 0;
  float opt_a = 0.0;
  float opt_d = 0.2;
  int opt_n = 20;
  float opt_t = 0.5;
  bool opt_det = false;

  bool opt_0 = false;
  bool opt_z = false;
  bool opt_Z = false;

  int opt_R = 0;

  bool opt_dot = false;
  int opt_ec = 0;
  int opt_ec_seed = 0;
  bool opt_reduce = false;
  bool opt_replay = false;
  bool opt_degen = false;
  int argn = 0;

  int exit_code = 0;

  spot::twa_graph_ptr product = nullptr;

  spot::option_map options;

  auto& env = spot::default_environment::instance();
  spot::atomic_prop_set* ap = new spot::atomic_prop_set;
  auto dict = spot::make_bdd_dict();

  spot::tl_simplifier_options simpopt(true, true, true, true, true);
  spot::tl_simplifier simp(simpopt);

  if (argc <= 1)
    syntax(argv[0]);

  while (++argn < argc)
    {
      if (!strcmp(argv[argn], "-0"))
        {
          opt_0 = true;
        }
      else if (!strcmp(argv[argn], "-1"))
        {
          opt_paper = true;
          opt_z = true;
        }
      else if (!strcmp(argv[argn], "-a"))
        {
          if (argc < argn + 3)
            syntax(argv[0]);
          opt_n_acc = to_int_nonneg(argv[++argn], "-a");
          opt_a = to_float_nonneg(argv[++argn], "-a");
        }
      else if (!strcmp(argv[argn], "-A"))
        {
          if (argc < argn + 2)
            syntax(argv[0]);
          if (!opt_ec)
            opt_ec = 1;
          std::istream* in;
          if (strcmp(argv[++argn], "-"))
            {
              in = new std::ifstream(argv[argn]);
              if (!*in)
                {
                  delete in;
                  std::cerr << "Failed to open " << argv[argn] << '\n';
                  exit(2);
                }
            }
          else
            {
              in = &std::cin;
            }

          while (in->good())
            {
              std::string input;
              if (std::getline(*in, input, '\n').fail())
                break;
              else if (input == "")
                break;
              ec_algo a = { input, nullptr };
              ec_algos.push_back(a);
            }

          if (in != &std::cin)
            delete in;
        }
      else if (!strncmp(argv[argn], "ar:", 3))
        {
          if (options.parse_options(argv[argn]))
            {
              std::cerr << "Failed to parse " << argv[argn] << '\n';
              exit(2);
            }
        }
      else if (!strcmp(argv[argn], "-d"))
        {
          if (argc < argn + 2)
            syntax(argv[0]);
          opt_d = to_float_nonneg(argv[++argn], "-d");
        }
      else if (!strcmp(argv[argn], "-D"))
        {
          opt_degen = true;
          if (!opt_ec)
            opt_ec = 1;
        }
      else if (!strcmp(argv[argn], "-det"))
        {
          opt_det = true;
        }
      else if (!strcmp(argv[argn], "-e"))
        {
          if (argc < argn + 2)
            syntax(argv[0]);
          opt_ec = to_int_nonneg(argv[++argn], "-e");
        }
      else if (!strcmp(argv[argn], "-g"))
        {
          opt_dot = true;
        }
      else if (!strcmp(argv[argn], "-m"))
        {
          opt_reduce = true;
          opt_replay = true;
          if (!opt_ec)
            opt_ec = 1;
        }
      else if (!strcmp(argv[argn], "-n"))
        {
          if (argc < argn + 2)
            syntax(argv[0]);
          opt_n = to_int_pos(argv[++argn], "-n");
        }
      else if (!strcmp(argv[argn], "-r"))
        {
          opt_replay = true;
          if (!opt_ec)
            opt_ec = 1;
        }
      else if (!strcmp(argv[argn], "-R"))
        {
          if (argc < argn + 2)
            syntax(argv[0]);
          opt_R = to_int_pos(argv[++argn], "-R");
        }
      else if (!strcmp(argv[argn], "-s"))
        {
          if (argc < argn + 2)
            syntax(argv[0]);
          opt_ec_seed = to_int_nonneg(argv[++argn], "-s");
          spot::srand(opt_ec_seed);
        }
      else if (!strcmp(argv[argn], "-t"))
        {
          if (argc < argn + 2)
            syntax(argv[0]);
          opt_t = to_float_nonneg(argv[++argn], "-t");
        }
      else if (!strcmp(argv[argn], "-z"))
        {
          opt_z = true;
        }
      else if (!strcmp(argv[argn], "-Z"))
        {
          opt_Z = opt_z = true;
        }
      else
        {
          ap->insert(env.require(argv[argn]));
        }
    }

  if (ec_algos.empty())
    {
      const char** i = default_algos;
      while (*i)
        {
          ec_algo a = { *(i++), nullptr };
          ec_algos.push_back(a);
        }
    }

  spot::timer_map tm_ec;
  spot::timer_map tm_ar;
  std::set<int> failed_seeds;
  spot::atomic_prop_set* apf = new spot::atomic_prop_set;
  for (auto i: *ap)
    apf->insert(i);

  if (opt_ec)
    {
      for (unsigned i = 0; i < ec_algos.size(); ++i)
        {
          const char* err;
          ec_algos[i].inst =
            spot::make_emptiness_check_instantiator(ec_algos[i].name.c_str(),
                                                    &err);
          if (!ec_algos[i].inst)
            {
              std::cerr << "Parse error after `" << err << "'\n";
              exit(1);
            }
          ec_algos[i].inst->options().set(options);
        }
    }

  do
    {

      if (opt_ec && !opt_paper)
        std::cout << "seed: " << opt_ec_seed << std::endl;
      spot::srand(opt_ec_seed);


      spot::twa_graph_ptr a =
        spot::random_graph(opt_n, opt_d, apf, dict,
                           opt_n_acc, opt_a, opt_t, opt_det);

      int real_n_acc = a->acc().num_sets();

      if (opt_dot)
        {
          print_dot(std::cout, a);
        }
      if (!opt_ec)
        {
          if (!opt_0 && !opt_dot)
            print_hoa(std::cout, a, nullptr);
        }
      else
        {
          spot::twa_graph_ptr degen = nullptr;
          if (opt_degen && real_n_acc > 1)
            degen = degeneralize_tba(a);

          int n_alg = ec_algos.size();
          int n_ec = 0;
          int n_empty = 0;
          int n_non_empty = 0;
          int n_maybe_empty = 0;

          for (int i = 0; i < n_alg; ++i)
            {
              auto ec = cons_emptiness_check(i, a, degen, real_n_acc);
              if (!ec)
                continue;
              ++n_ec;
              const std::string algo = ec_algos[i].name;
              if (!opt_paper)
                {
                  std::cout.width(32);
                  std::cout << algo << ": ";
                }
              tm_ec.start(algo);
              spot::emptiness_check_result_ptr res;
              for (int count = opt_R;;)
                {
                  res = ec->check();
                  if (count-- <= 0)
                    break;
                  ec = cons_emptiness_check(i, a, degen, real_n_acc);
                }
              tm_ec.stop(algo);
              auto ecs = ec->statistics();
              if (opt_z && res)
                {
                  // Notice that ratios are computed w.r.t. the
                  // generalized automaton a.
                  prod_stats = spot::stats_reachable(a);
                }
              else
                {
                  // To trigger a division by 0 if used erroneously.
                  prod_stats.states = 0;
                  prod_stats.edges = 0;
                }

              if (opt_z && ecs)
                {
                  sc_ec.count(algo, ecs);
                  if (res)
                    {
                      ec_ratio_stats[real_n_acc].count(algo, ecs);
                      glob_ec_ratio_stats.count(algo, ecs);
                    }
                }

              if (res)
                {
                  if (!opt_paper)
                    std::cout << "acc. run";
                  ++n_non_empty;
                  if (opt_replay)
                    {
                      spot::twa_run_ptr run;
                      tm_ar.start(algo);
                      for (int count = opt_R;;)
                        {
                          run = res->accepting_run();
                          if (count-- <= 0 || !run)
                            break;
                        }
                      if (!run)
                        tm_ar.cancel(algo);
                      else
                        tm_ar.stop(algo);

                      const spot::unsigned_statistics* s
                        = res->statistics();
                      if (opt_z)
                        {
                          // Count only the last run (the
                          // other way would be to divide
                          // the stats by opt_R).
                          sc_arc.count(algo, s);
                          arc_ratio_stats.count(algo, s);
                        }
                      if (!run)
                        {
                          if (!opt_paper)
                            std::cout << " exists, not computed";
                        }
                      else
                        {
                          std::ostringstream s;
                          if (!run->replay(s))
                            {
                              if (!opt_paper)
                                std::cout << ", but could not replay "
                                          << "it (ERROR!)";
                              failed_seeds.insert(opt_ec_seed);
                            }
                          else
                            {
                              if (!opt_paper)
                                std::cout << ", computed";
                              if (opt_z)
                                ar_stats[algo].count(run);
                            }
                          if (opt_z && !opt_paper)
                            std::cout << " [" << run->prefix.size()
                                      << '+' << run->cycle.size()
                                      << ']';

                          if (opt_reduce)
                            {
                              auto redrun = run->reduce();
                              if (!redrun->replay(s))
                                {
                                  if (!opt_paper)
                                    std::cout
                                      << ", but could not replay "
                                      << "its minimization (ERROR!)";
                                  failed_seeds.insert(opt_ec_seed);
                                }
                              else
                                {
                                  if (!opt_paper)
                                    std::cout << ", reduced";
                                  if (opt_z)
                                    mar_stats[algo].count(redrun);
                                }
                              if (opt_z && !opt_paper)
                                {
                                  std::cout << " ["
                                            << redrun->prefix.size()
                                            << '+'
                                            << redrun->cycle.size()
                                            << ']';
                                }
                            }
                        }
                    }
                  if (!opt_paper)
                    std::cout << std::endl;
                }
              else
                {
                  if (ec->safe())
                    {
                      if (!opt_paper)
                        std::cout << "empty language" << std::endl;
                      ++n_empty;
                    }
                  else
                    {
                      if (!opt_paper)
                        std::cout << "maybe empty language"
                                  << std::endl;
                      ++n_maybe_empty;
                    }

                }

              if (opt_Z && !opt_paper)
                ec->print_stats(std::cout);
            }

          assert(n_empty + n_non_empty + n_maybe_empty == n_ec);

          if ((n_empty == 0 && (n_non_empty + n_maybe_empty) != n_ec)
              || (n_empty != 0 && n_non_empty != 0))
            {
              std::cout << "ERROR: not all algorithms agree"
                        << std::endl;
              failed_seeds.insert(opt_ec_seed);
            }
        }

      if (opt_ec)
        {
          --opt_ec;
          ++opt_ec_seed;
        }
    }
  while (opt_ec);
  apf->clear();

  if (!opt_paper && opt_z)
    {
      if (!sc_ec.empty())
        {
          std::cout << std::endl
                    << "Statistics about emptiness checks:"
                    << std::endl;
          sc_ec.display(std::cout);
        }
      if (!sc_arc.empty())
        {
          std::cout << std::endl
                    << "Statistics about accepting run computations:"
                    << std::endl;
          sc_arc.display(std::cout);
        }
      if (!glob_ec_ratio_stats.empty())
        {
          std::cout << std::endl
                    << "Emptiness check ratios for non-empty automata:"
                    << std::endl << "all tests"
                    << std::endl;
          glob_ec_ratio_stats.display(std::cout, false);
          if (ec_ratio_stats.size() > 1)
            for (ec_ratio_stats_type::const_iterator i = ec_ratio_stats.begin();
                 i != ec_ratio_stats.end(); ++i)
              {
                std::cout << "tests with " << i->first
                          << " acceptance conditions"
                          << std::endl;
                i->second.display(std::cout, false);
              }
        }
      if (!ar_stats.empty())
        print_ar_stats(ar_stats, "Statistics about accepting runs:");
      if (!mar_stats.empty())
        print_ar_stats(mar_stats, "Statistics about reduced accepting runs:");
      if (!arc_ratio_stats.empty())
        {
          std::cout << std::endl
                    << "Accepting run ratios:" << std::endl;
          arc_ratio_stats.display(std::cout, false);
        }
      if (!tm_ec.empty())
        {
          std::cout << std::endl
                    << "emptiness checks cumulated timings:" << std::endl;
          tm_ec.print(std::cout);
        }
      if (!tm_ar.empty())
        {
          std::cout << std::endl
                    << "accepting runs cumulated timings:" << std::endl;
          tm_ar.print(std::cout);
        }
    }
  else if (opt_paper)
    {
      std::cout << "Emptiness check ratios" << std::endl;
      std::cout << std::right << std::fixed << std::setprecision(1);
      ec_ratio_stat_type::stats_alg_map& stats = glob_ec_ratio_stats.stats;
      typedef ec_ratio_stat_type::alg_1stat_map::const_iterator ec_iter;

      for (unsigned ai = 0; ai < ec_algos.size(); ++ai)
        {
          const std::string algo = ec_algos[ai].name;

          int n = -1;

          std::cout << std::setw(25)  << algo << ' ' << std::setw(8);

          ec_iter i = stats["states"].find(algo);
          if (i != stats["states"].end())
            {
              std::cout << i->second.tot / i->second.n;
              n = i->second.n;
            }
          else
            std::cout << "";
          std::cout << ' ' << std::setw(8);

          i = stats["transitions"].find(algo);
          if (i != stats["transitions"].end())
            {
              std::cout << i->second.tot / i->second.n;
              n = i->second.n;
            }
          else
            std::cout << "";
          std::cout << ' ' << std::setw(8);

          i = stats["max. depth"].find(algo);
          if (i != stats["max. depth"].end())
            {
              std::cout << i->second.tot / i->second.n;
              n = i->second.n;
            }
          else
            std::cout << "";
          if (n >= 0)
            std::cout << ' ' << std::setw(8) << n;
          std::cout << std::endl;
        }

      std::cout << std::endl << "Accepting run ratios" << std::endl;
      std::cout << std::right << std::fixed << std::setprecision(1);
      ec_ratio_stat_type::stats_alg_map& stats2 = arc_ratio_stats.stats;

      for (unsigned ai = 0; ai < ec_algos.size(); ++ai)
        {
          const std::string algo = ec_algos[ai].name;

          std::cout << std::setw(25)  << algo << ' ' << std::setw(8);

          ec_iter i = stats2["search space states"].find(algo);
          if (i != stats2["search space states"].end())
            std::cout << i->second.tot / i->second.n;
          else
            std::cout << "";
          std::cout << ' ' << std::setw(8);

          i = stats2["(non unique) states for cycle"].find(algo);
          if (i != stats2["(non unique) states for cycle"].end())
            std::cout << i->second.tot / i->second.n;
          else
            std::cout << "";
          std::cout << std::endl;
      }
    }

  if (!failed_seeds.empty())
    {
      exit_code = 1;
      std::cout << "The check failed for the following seeds:";
      for (std::set<int>::const_iterator i = failed_seeds.begin();
           i != failed_seeds.end(); ++i)
        std::cout << ' ' << *i;
      std::cout << std::endl;
    }

  delete ap;
  delete apf;
  return exit_code;
}
