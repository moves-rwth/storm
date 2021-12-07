// -*- coding: utf-8 -*-
// Copyright (C) 2016, 2018-2019 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE).
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

#include <iostream>

namespace spot
{

  /// \ingroup misc_tools
  /// @{

  /// \brief A class implementing Kleene's three-valued logic.
  ///
  /// https://en.wikipedia.org/wiki/Three-valued_logic#Kleene_and_Priest_logics
  class trival
  {
  public:
    // We use repr_t instead of value_t in bitfields to avoid a warning from gcc
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51242
    typedef signed char repr_t;
    enum value_t : repr_t { no_value = -1, maybe_value = 0, yes_value = 1 };
  private:
    value_t val_;
  public:
    constexpr trival()
      : val_(maybe_value)
    {
    }

    constexpr trival(bool v)
      : val_(v ? yes_value : no_value)
    {
    }

#ifndef SWIG
    // This is needed internally by Spot to work around the bitfield
    // issue mentioned earlier, it makes no sense to use it in Python.
    static trival from_repr_t(repr_t v)
    {
      return trival(static_cast<value_t>(v));
    }
#endif

    constexpr explicit trival(value_t v)
      : val_(v)
    {
    }

    static constexpr trival maybe() noexcept
    {
      return trival();
    }

    /// Is true or false, but not maybe.
    constexpr bool is_known() const
    {
      return val_ != maybe_value;
    }

    constexpr bool is_maybe() const
    {
      return val_ == maybe_value;
    }

    constexpr bool is_true() const
    {
      return val_ == yes_value;
    }

    constexpr bool is_false() const
    {
      return val_ == no_value;
    }

    constexpr value_t val() const
    {
      return val_;
    }

#ifndef SWIG
    // constexpr explicit only supported in SWIG >= 3.0.4
    constexpr
#endif
    explicit operator bool() const
    {
      return val_ == yes_value;
    }

    constexpr trival operator!() const
    {
      return trival((val_ == yes_value) ? no_value :
                    (val_ == no_value) ? yes_value :
                    maybe_value);
    }
  };

  // We prefer a global version of the operator so that the left
  // argument can be promoted (think "bool == trival" being promoted
  // to "trival == trival").  However Swig's generated Python bindings
  // cannot deal with operators in the global namespace, so we use an
  // in-class version (coded in impl.i) in this case.  This will fail
  // on a "bool == trival" comparison in Python, but we usually write
  // "trival == bool" and that works.
#ifndef SWIG
  constexpr bool operator==(trival a, trival b)
  {
    return a.val() == b.val();
  }

  constexpr bool operator!=(trival a, trival b)
  {
    return !(a == b);
  }
#endif

  constexpr trival operator&&(trival a, trival b)
  {
    return
      (a.val() == trival::no_value || b.val() == trival::no_value)
      ? trival(false)
      : (a.val() == trival::maybe_value || b.val() == trival::maybe_value)
      ? trival::maybe()
      : trival(true);
  }

  constexpr trival operator&&(bool a, trival b)
  {
    return trival(a) && b;
  }

  constexpr trival operator&&(trival a, bool b)
  {
    return a && trival(b);
  }

  constexpr trival operator||(trival a, trival b)
  {
    return
      (a.val() == trival::yes_value || b.val() == trival::yes_value)
      ? trival(true)
      : (a.val() == trival::maybe_value || b.val() == trival::maybe_value)
      ? trival::maybe()
      : trival(false);
  }

  constexpr trival operator||(bool a, trival b)
  {
    return trival(a) || b;
  }

  constexpr trival operator||(trival a, bool b)
  {
    return a || trival(b);
  }

  inline std::ostream& operator<<(std::ostream& os, trival v)
  {
    return os << ((v.val() == trival::no_value) ? "no"
                  : (v.val() == trival::maybe_value) ? "maybe"
                  : "yes");
  }

  /// @}
}
