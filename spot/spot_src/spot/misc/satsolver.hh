// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2017-2018, 2020 Laboratoire de Recherche et
// Développement de l'Epita.
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

#include <spot/misc/common.hh>
#include <spot/misc/tmpfile.hh>
#include <vector>
#include <stdexcept>
#include <iosfwd>
#include <initializer_list>

struct PicoSAT; // forward

namespace spot
{
  class printable;

  /// \brief Interface with a given sat solver.
  ///
  /// When created, it checks if SPOT_SATSOLVER env var is set. If so,
  /// its value is parsed and saved internally. The env variable musb be set
  /// like this: "<satsolver> [its_options] %I > %O"
  /// where %I and %O are replaced by input and output files.
  ///
  /// The run method permits of course to run the given sat solver.
  class satsolver_command: formater
  {
  private:
    const char* satsolver;

  public:
    satsolver_command();

    /// \brief Return true if a satsolver is given, false otherwise.
    bool command_given();

    /// \brief Run the given satsolver.
    int run(printable* in, printable* out);

  };

  /// \brief Interface with a SAT solver.
  ///
  /// This class provides the necessary functions to add clauses, comments.
  /// Depending on SPOT_SATSOLVER, it will use either picosat solver (default)
  /// or the given satsolver.
  ///
  /// Now that spot is distributed with a satsolver (PicoSAT), it is used by
  /// default. But another satsolver can be configured via the
  /// <code>SPOT_SATSOLVER</code> environment variable. It must be set following
  /// this: "satsolver [options] %I > %O"
  /// where %I and %O are replaced by input and output files.
  class SPOT_API satsolver
  {
  public:
    /// \brief Construct the sat solver and itinialize variables.
    /// If no satsolver is provided through SPOT_SATSOLVER env var, a
    /// distributed version of PicoSAT will be used.
    satsolver();
    ~satsolver();

    /// \brief Adjust the number of variables used in the cnf formula.
    void adjust_nvars(int nvars);

    /// \brief Declare the number of vars reserved for assumptions.
    void set_nassumptions_vars(int nassumptions_vars);

    /// \brief Add a list of lit. to the current clause.
    void add(std::initializer_list<int> values);

    /// \brief Add a single lit. to the current clause.
    void add(int v);

    /// \breif Get the current number of clauses.
    int get_nb_clauses() const;

    /// \brief Get the current number of variables.
    int get_nb_vars() const;

    /// \brief Returns std::pair<nvars, nclauses>;
    std::pair<int, int> stats() const;

    /// \brief Add a comment.
    /// It should be used only in debug mode after providing a satsolver.
    template<typename T>
    void comment_rec(T single);

    /// \brief Add comments.
    /// It should be used only in debug mode after providing a satsolver.
    template<typename T, typename... Args>
    void comment_rec(T first, Args... args);

    /// \brief Add a comment. It will start with "c ".
    /// It should be used only in debug mode after providing a satsolver.
    template<typename T>
    void comment(T single);

    /// \brief Add comments. It will start with "c ".
    /// It should be used only in debug mode after providing a satsolver.
    template<typename T, typename... Args>
    void comment(T first, Args... args);

    /// \brief Assume a litteral value.
    /// Must only be used with distributed picolib.
    void assume(int lit);

    typedef std::vector<bool> solution;
    typedef std::pair<int, solution> solution_pair;

    /// \brief Return std::vector<solving_return_code, solution>.
    solution_pair get_solution();

  private:  // methods
    /// \brief Initialize cnf streams attributes.
    void start();

    /// \brief End the current clause and increment the counter.
    void end_clause();

    /// \brief Extract the solution of Picosat output.
    /// Must be called only if SPOT_SATSOLVER env variable is not set.
    satsolver::solution
    picosat_get_sol(int res);

    /// \brief Extract the solution of a SAT solver output.
    satsolver::solution
    satsolver_get_sol(const char* filename);

    /// \brief Check if <code>SPOT_XCNF</code> env var is set.
    bool
    xcnf_mode();

  private:  // variables
    /// \brief A satsolver_command. Check if SPOT_SATSOLVER is given.
    satsolver_command cmd_;

    // cnf streams and associated clause counter.
    // The next 2 pointers will be != nullptr if SPOT_SATSOLVER is given.
    temporary_file* cnf_tmp_;
    std::ostream* cnf_stream_;
    int nclauses_;
    int nvars_;
    int nassumptions_vars_; // Surplus of vars (for 'assume' algorithm).

    /// \brief Number of solutions to obtain from the satsolver
    /// (without assuming litterals).
    int nsols_;

    /// \brief Picosat satsolver instance.
    PicoSAT* psat_;

    // The next 2 pointers will be initialized if SPOT_XCNF env var
    // is set. This requires SPOT_SATSOLVER to be set as well.
    std::ofstream* xcnf_tmp_;
    std::ofstream* xcnf_stream_;
    std::string path_;
  };

  template<typename T>
  void
  satsolver::comment_rec(T single)
  {
    if (!psat_)
      *cnf_stream_ << ' ' << single;
  }

  template<typename T, typename... Args>
  void
  satsolver::comment_rec(T first, Args... args)
  {
    if (!psat_)
    {
      *cnf_stream_ << ' ' << first;
      comment_rec(args...);
    }
  }

  template<typename T>
  void
  satsolver::comment(T single)
  {
    if (!psat_)
      *cnf_stream_ << "c " << single;
  }

  template<typename T, typename... Args>
  void
  satsolver::comment(T first, Args... args)
  {
    if (!psat_)
    {
      *cnf_stream_ << "c " << first;
      comment_rec(args...);
    }
  }

}
