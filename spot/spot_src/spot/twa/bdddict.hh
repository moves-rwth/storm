// -*- coding: utf-8 -*-
// Copyright (C) 2011-2017 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2006 Laboratoire d'Informatique de Paris
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

#pragma once

#include <list>
#include <set>
#include <map>
#include <iosfwd>
#include <bddx.h>
#include <vector>
#include <memory>
#include <spot/tl/formula.hh>

namespace spot
{
  /// \brief Private data for bdd_dict.
  class bdd_dict_priv;

  /// \ingroup twa_essentials
  /// \brief Map BDD variables to formulae.
  ///
  /// The BDD library uses integers to designate Boolean variables in
  /// its decision diagrams.  This class is used to map such integers
  /// to objects actually used in Spot.  These objects are usually
  /// atomic propositions, but they can also be acceptance conditions.
  ///
  /// When a BDD variable is registered using a bdd_dict, it is always
  /// associated to a "user" (or "owner") object.  This is done by
  /// supplying the bdd_dict with a pointer to the intended user of
  /// the variable.  When the user object dies, it should release the
  /// BDD variables it was using by calling (for instance)
  /// unregister_all_my_variables(), giving the same pointer.
  /// Variables can also by unregistered one by one using
  /// unregister_variable().
  class SPOT_API bdd_dict
  {
    bdd_dict_priv* priv_;
  public:

    bdd_dict();

    /// \brief Destroy the BDD dict.
    ///
    /// This always calls assert_emptiness() to diagnose cases where
    /// variables have not been unregistered.
    ~bdd_dict();

    /// Formula-to-BDD-variable maps.
    typedef std::map<formula, int> fv_map;
    /// BDD-variable-to-formula maps.
    typedef std::map<int, formula> vf_map;

    fv_map var_map; ///< Maps atomic propositions to BDD variables
    fv_map acc_map; ///< Maps acceptance conditions to BDD variables

    /// BDD-variable reference counts.
    typedef std::set<const void*> ref_set;

    enum var_type { anon = 0, var, acc };
    struct bdd_info {
      bdd_info() : type(anon) {}
      var_type type;
      formula f;        // Used unless t==anon.
      ref_set refs;
    };
    typedef std::vector<bdd_info> bdd_info_map;
    // Map BDD variables to their meaning.
    bdd_info_map bdd_map;

    /// \brief Register an atomic proposition.
    ///
    /// Return (and maybe allocate) a BDD variable designating formula
    /// \a f.  The \a for_me argument should point to the object using
    /// this BDD variable, this is used for reference counting.  It is
    /// perfectly safe to call this function several time with the same
    /// arguments.
    ///
    /// \return The variable number.  Use bdd_ithvar() or bdd_nithvar()
    ///   to convert this to a BDD.
    /// @{
    int register_proposition(formula f, const void* for_me);

    template <typename T>
    int register_proposition(formula f, std::shared_ptr<T> for_me)
    {
      return register_proposition(f, for_me.get());
    }
    /// @}

    /// \brief whether a proposition has already been registered
    ///
    /// If \a f has been registered for \a me, this returns
    /// a non-negative value that is the BDD variable number.
    /// Otherwise this returns -1.
    /// @{
    int has_registered_proposition(formula f, const void* me);

    template <typename T>
    int has_registered_proposition(formula f, std::shared_ptr<T> for_me)
    {
      return has_registered_proposition(f, for_me.get());
    }
    /// @}

    // \brief return the BDD variable associated to a registered
    // proposition.
    //
    // Throws std::out_of_range if the \a is not a known proposition.
    int varnum(formula f)
    {
      return var_map.at(f);
    }

    /// \brief Register an acceptance variable.
    ///
    /// Return (and maybe allocate) a BDD variable designating an
    /// acceptance set associated to formula \a f.  The \a for_me
    /// argument should point to the object using this BDD variable,
    /// this is used for reference counting.  It is perfectly safe to
    /// call this function several time with the same arguments.
    ///
    /// \return The variable number.  Use bdd_ithvar() or bdd_nithvar()
    ///   to convert this to a BDD.
    /// @{
    int register_acceptance_variable(formula f, const void* for_me);

    template <typename T>
    int register_acceptance_variable(formula f, std::shared_ptr<T> for_me)
    {
      return register_acceptance_variable(f, for_me.get());
    }
    /// @}

    /// \brief Register anonymous BDD variables.
    ///
    /// Return (and maybe allocate) \a n consecutive BDD variables which
    /// will be used only by \a for_me.
    ///
    /// \return The variable number.  Use bdd_ithvar() or bdd_nithvar()
    ///   to convert this to a BDD.
    /// @{
    int register_anonymous_variables(int n, const void* for_me);

    template <typename T>
    int register_anonymous_variables(int n, std::shared_ptr<T> for_me)
    {
      return register_anonymous_variables(n, for_me.get());
    }
    /// @}

    /// \brief Duplicate the variable usage of another object.
    ///
    /// This tells this dictionary that the \a for_me object will be
    /// using the same BDD variables as the \a from_other objects.
    /// This ensures that the variables won't be freed when \a
    /// from_other is deleted if \a from_other is still alive.
    /// @{
    void register_all_variables_of(const void* from_other, const void* for_me);

    template <typename T>
    void register_all_variables_of(const void* from_other,
                                   std::shared_ptr<T> for_me)
    {
      register_all_variables_of(from_other, for_me.get());
    }

    template <typename T>
    void register_all_variables_of(std::shared_ptr<T> from_other,
                                   const void* for_me)
    {
      register_all_variables_of(from_other.get(), for_me);
    }

    template <typename T, typename U>
    void register_all_variables_of(std::shared_ptr<T> from_other,
                                   std::shared_ptr<U> for_me)
    {
      register_all_variables_of(from_other.get(), for_me.get());
    }
    /// @}

    /// \brief Release all variables used by an object.
    ///
    /// Usually called in the destructor if \a me.
    void unregister_all_my_variables(const void* me);

    /// \brief Release a variable used by \a me.
    /// @{
    void unregister_variable(int var, const void* me);

    template <typename T>
    void unregister_variable(int var, std::shared_ptr<T> me)
    {
      unregister_variable(var, me.get());
    }
    /// @}

    /// \brief Dump all variables for debugging.
    /// \param os The output stream.
    std::ostream& dump(std::ostream& os) const;

    /// \brief Make sure the dictionary is empty.
    ///
    /// This will print diagnostics if the dictionary is not empty.
    /// Use for debugging.  This is called automatically by the
    /// destructor.  When Spot is compiled in development mode (i.e.,
    /// with <code>./configure --enable-devel</code>), this function
    /// will abort if the dictionary is not empty.
    ///
    /// The errors detected by this function usually indicate missing
    /// calls to unregister_variable() or
    /// unregister_all_my_variables().
    void assert_emptiness() const;

  private:
    // Disallow copy.
    bdd_dict(const bdd_dict& other) = delete;
    bdd_dict& operator=(const bdd_dict& other) = delete;
  };

  typedef std::shared_ptr<bdd_dict> bdd_dict_ptr;

  inline bdd_dict_ptr make_bdd_dict()
  {
    return std::make_shared<bdd_dict>();
  }
}
