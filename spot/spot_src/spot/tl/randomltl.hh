// -*- coding: utf-8 -*-
// Copyright (C) 2010-2016, 2018 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
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

#include <spot/tl/apcollect.hh>
#include <iosfwd>

#include <unordered_set>
#include <spot/misc/optionmap.hh>
#include <spot/misc/hash.hh>
#include <spot/tl/simplify.hh>

namespace spot
{
  /// \ingroup tl_io
  /// \brief Base class for random formula generators
  class SPOT_API random_formula
  {
  public:
    random_formula(unsigned proba_size,
                   const atomic_prop_set* ap):
      proba_size_(proba_size), proba_(new op_proba[proba_size_]), ap_(ap)
    {
    }

    virtual ~random_formula()
    {
      delete[] proba_;
    }

    /// Return the set of atomic proposition used to build formulae.
    const atomic_prop_set*
      ap() const
    {
      return ap_;
    }

    /// \brief Generate a formula of size \a n.
    ///
    /// It is possible to obtain formulae that are smaller than \a
    /// n, because some simple simplifications are performed by the
    /// AST.  (For instance the formula <code>a | a</code> is
    /// automatically reduced to <code>a</code> by spot::multop.)
    formula generate(int n) const;

    /// \brief Print the priorities of each operator, constants,
    /// and atomic propositions.
    std::ostream& dump_priorities(std::ostream& os) const;

    /// \brief Update the priorities used to generate the formulae.
    ///
    /// \a options should be comma-separated list of KEY=VALUE
    /// assignments, using keys from the above list.
    /// For instance <code>"xor=0, F=3"</code> will prevent \c xor
    /// from being used, and will raise the relative probability of
    /// occurrences of the \c F operator.
    const char* parse_options(char* options);

  protected:
    void update_sums();

    struct op_proba
    {
      const char* name;
      int min_n;
      double proba;
      typedef formula (*builder)(const random_formula* rl, int n);
      builder build;
      void setup(const char* name, int min_n, builder build);
    };
    unsigned proba_size_;
    op_proba* proba_;
    double total_1_;
    op_proba* proba_2_;
    double total_2_;
    op_proba* proba_2_or_more_;
    double total_2_and_more_;
    const atomic_prop_set* ap_;
  };


  /// \ingroup tl_io
  /// \brief Generate random LTL formulae.
  ///
  /// This class recursively constructs LTL formulae of a given
  /// size.  The formulae will use the use atomic propositions from
  /// the set of propositions passed to the constructor, in addition
  /// to the constant and all LTL operators supported by Spot.
  ///
  /// By default each operator has equal chance to be selected.
  /// Also, each atomic proposition has as much chance as each
  /// constant (i.e., true and false) to be picked.  This can be
  /// tuned using parse_options().
  class SPOT_API random_ltl: public random_formula
  {
  public:
    /// Create a random LTL generator using atomic propositions from \a ap.
    ///
    /// The default priorities are defined as follows:
    ///
    /** \verbatim
        ap      n
        false   1
        true    1
        not     1
        F       1
        G       1
        X       1
        equiv   1
        implies 1
        xor     1
        R       1
        U       1
        W       1
        M       1
        and     1
        or      1
        \endverbatim */
    ///
    /// Where \c n is the number of atomic propositions in the
    /// set passed to the constructor.
    ///
    /// This means that each operator has equal chance to be
    /// selected.  Also, each atomic proposition has as much chance
    /// as each constant (i.e., true and false) to be picked.
    ///
    /// These priorities can be changed use the parse_options method.
    random_ltl(const atomic_prop_set* ap);

  protected:
    void setup_proba_();
    random_ltl(int size, const atomic_prop_set* ap);
  };

  /// \ingroup tl_io
  /// \brief Generate random Boolean formulae.
  ///
  /// This class recursively constructs Boolean formulae of a given size.
  /// The formulae will use the use atomic propositions from the
  /// set of propositions passed to the constructor, in addition to the
  /// constant and all Boolean operators supported by Spot.
  ///
  /// By default each operator has equal chance to be selected.
  class SPOT_API random_boolean final: public random_formula
  {
  public:
    /// Create a random Boolean formula generator using atomic
    /// propositions from \a ap.
    ///
    /// The default priorities are defined as follows:
    ///
    /** \verbatim
        ap      n
        false   1
        true    1
        not     1
        equiv   1
        implies 1
        xor     1
        and     1
        or      1
        \endverbatim */
    ///
    /// Where \c n is the number of atomic propositions in the
    /// set passed to the constructor.
    ///
    /// This means that each operator has equal chance to be
    /// selected.  Also, each atomic proposition has as much chance
    /// as each constant (i.e., true and false) to be picked.
    ///
    /// These priorities can be changed use the parse_options method.
    random_boolean(const atomic_prop_set* ap);
  };

  /// \ingroup tl_io
  /// \brief Generate random SERE.
  ///
  /// This class recursively constructs SERE of a given size.
  /// The formulae will use the use atomic propositions from the
  /// set of propositions passed to the constructor, in addition to the
  /// constant and all SERE operators supported by Spot.
  ///
  /// By default each operator has equal chance to be selected.
  class SPOT_API random_sere final: public random_formula
  {
  public:
    /// Create a random SERE genere using atomic propositions from \a ap.
    ///
    /// The default priorities are defined as follows:
    ///
    /** \verbatim
        eword    1
        boolform 1
        star     1
        star_b   1
        equal_b  1
        goto_b   1
        and      1
        andNLM   1
        or       1
        concat   1
        fusion   1
        \endverbatim */
    ///
    /// Where "boolfrom" designates a Boolean formula generated
    /// by random_boolean.
    ///
    /// These priorities can be changed use the parse_options method.
    ///
    /// In addition, you can set the properties of the Boolean
    /// formula generator used to build Boolean subformulae using
    /// the parse_options method of the \c rb attribute.
    random_sere(const atomic_prop_set* ap);

    random_boolean rb;
  };

  /// \ingroup tl_io
  /// \brief Generate random PSL formulae.
  ///
  /// This class recursively constructs PSL formulae of a given size.
  /// The formulae will use the use atomic propositions from the
  /// set of propositions passed to the constructor, in addition to the
  /// constant and all PSL operators supported by Spot.
  class SPOT_API random_psl: public random_ltl
  {
  public:
    /// Create a random PSL generator using atomic propositions from \a ap.
    ///
    /// PSL formulae are built by combining LTL operators, plus
    /// three operators (EConcat, UConcat, Closure) taking a SERE
    /// as parameter.
    ///
    /// The default priorities are defined as follows:
    ///
    /** \verbatim
        ap      n
        false   1
        true    1
        not     1
        F       1
        G       1
        X       1
        Closure 1
        equiv   1
        implies 1
        xor     1
        R       1
        U       1
        W       1
        M       1
        and     1
        or      1
        EConcat 1
        UConcat 1
        \endverbatim */
    ///
    /// Where \c n is the number of atomic propositions in the
    /// set passed to the constructor.
    ///
    /// This means that each operator has equal chance to be
    /// selected.  Also, each atomic proposition has as much chance
    /// as each constant (i.e., true and false) to be picked.
    ///
    /// These priorities can be changed use the parse_options method.
    ///
    /// In addition, you can set the properties of the SERE generator
    /// used to build SERE subformulae using the parse_options method
    /// of the \c rs attribute.
    random_psl(const atomic_prop_set* ap);

    /// The SERE generator used to generate SERE subformulae.
    random_sere rs;
  };

  class SPOT_API randltlgenerator
  {
    typedef std::unordered_set<formula> fset_t;


  public:
    enum output_type { Bool, LTL, SERE, PSL };
    static constexpr unsigned MAX_TRIALS = 100000U;

    randltlgenerator(int aprops_n, const option_map& opts,
                     char* opt_pL = nullptr,
                     char* opt_pS = nullptr,
                     char* opt_pB = nullptr);

    randltlgenerator(atomic_prop_set aprops, const option_map& opts,
                     char* opt_pL = nullptr,
                     char* opt_pS = nullptr,
                     char* opt_pB = nullptr);

    ~randltlgenerator();

    formula next();

    void dump_ltl_priorities(std::ostream& os);
    void dump_bool_priorities(std::ostream& os);
    void dump_psl_priorities(std::ostream& os);
    void dump_sere_priorities(std::ostream& os);
    void dump_sere_bool_priorities(std::ostream& os);
    void remove_some_props(atomic_prop_set& s);

    formula GF_n();

  private:
    fset_t unique_set_;
    atomic_prop_set aprops_;

    int opt_seed_;
    int opt_tree_size_min_;
    int opt_tree_size_max_;
    bool opt_unique_;
    bool opt_wf_;
    int opt_simpl_level_;
    tl_simplifier simpl_;

    int output_;

    random_formula* rf_ = nullptr;
    random_psl* rp_ = nullptr;
    random_sere* rs_ = nullptr;
  };
}
