// -*- coding: utf-8 -*-
// Copyright (C) 2015-2018, 2020 Laboratoire de Recherche et
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

#pragma once

#include "common_sys.hh"
#include <vector>
#include <atomic>
#include <argp.h>

#include <spot/misc/formater.hh>
#include <spot/misc/tmpfile.hh>
#include <spot/twa/twagraph.hh>


extern const struct argp trans_argp; // ltlcross, ltldo
extern const struct argp autproc_argp; // autcross

extern bool opt_relabel;

struct shorthands_t;

struct tool_spec
{
  // The translator command, as specified on the command-line.
  // If this has the form of
  //    {name}cmd
  // then it is split in two components.
  // Otherwise, spec=cmd=name.
  const char* spec;
  // actual shell command (or spec)
  const char* cmd;
  // name of the translator (or spec)
  const char* name;
  // Whether the tool is a reference.
  bool reference;

  tool_spec(const char* spec, shorthands_t* begin, shorthands_t* end,
            bool is_ref) noexcept;
  tool_spec(const tool_spec& other) noexcept;
  tool_spec& operator=(const tool_spec& other);
  ~tool_spec();
};

extern std::vector<tool_spec> tools;

void tools_push_trans(const char* trans, bool is_ref = false);
void tools_push_autproc(const char* proc, bool is_ref = false);

struct quoted_formula final: public spot::printable_value<spot::formula>
{
  using spot::printable_value<spot::formula>::operator=;
  void print(std::ostream& os, const char* pos) const override;
};

struct filed_formula final: public spot::printable
{
  filed_formula(const quoted_formula& ltl) : f_(ltl)
  {
  }

  void print(std::ostream& os, const char* pos) const override;

  void new_round(unsigned serial)
  {
    serial_ = serial;
  }

 private:
  const quoted_formula& f_;
  unsigned serial_;
};

struct filed_automaton final: public spot::printable
{
  filed_automaton()
  {
  }

  void print(std::ostream& os, const char* pos) const override;

  void new_round(spot::const_twa_graph_ptr aut, unsigned serial)
  {
    aut_ = aut;
    serial_ = serial;
  }

 private:
  spot::const_twa_graph_ptr aut_;
  unsigned serial_;
};

struct printable_result_filename final:
  public spot::printable_value<spot::temporary_file*>
{
  unsigned translator_num;

  printable_result_filename();
  ~printable_result_filename();
  void reset(unsigned n);
  void cleanup();

  void print(std::ostream& os, const char* pos) const override;
};


class translator_runner: protected spot::formater
{
protected:
  spot::bdd_dict_ptr dict;
  // Round-specific variables
  quoted_formula ltl_formula;
  filed_formula filename_formula = ltl_formula;
  // Run-specific variables
  printable_result_filename output;
public:
  using spot::formater::has;

  translator_runner(spot::bdd_dict_ptr dict,
                    // whether we accept the absence of output
                    // specifier
                    bool no_output_allowed = false);
  std::string formula() const;
  void round_formula(spot::formula f, unsigned serial);
};


class autproc_runner: protected spot::formater
{
protected:
  // Round-specific variables
  filed_automaton filename_automaton;
  // Run-specific variables
  printable_result_filename output;
public:
  using spot::formater::has;

  autproc_runner(// whether we accept the absence of output
                 // specifier
                 bool no_output_allowed = false);
  void round_automaton(spot::const_twa_graph_ptr aut, unsigned serial);
};


// Disable handling of timeout on systems that miss kill() or alarm().
// For instance MinGW.
#if HAVE_KILL && HAVE_ALARM
# define ENABLE_TIMEOUT 1
#else
# define ENABLE_TIMEOUT 0
#endif

extern std::atomic<bool> timed_out;
extern unsigned timeout_count;
#if ENABLE_TIMEOUT
void setup_sig_handler();
int exec_with_timeout(const char* cmd);
#else // !ENABLE_TIMEOUT
#define exec_with_timeout(cmd) system(cmd)
#define setup_sig_handler() while (0);
#endif // !ENABLE_TIMEOUT
