// -*- coding: utf-8 -*-
// Copyright (C) 2014-2020 Laboratoire de Recherche et Développement
// de l'Epita.
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

#include <functional>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>

#include <spot/misc/_config.h>
#include <spot/misc/bitset.hh>
#include <spot/misc/trival.hh>

namespace spot
{
  namespace internal
  {
    class mark_container;

    template<bool>
    struct _32acc {};
    template<>
    struct _32acc<true>
    {
      SPOT_DEPRECATED("mark_t no longer relies on unsigned, stop using value_t")
      typedef unsigned value_t;
    };
  }

  /// \ingroup twa_essentials
  /// @{

  /// \brief An acceptance condition
  ///
  /// This represent an acceptance condition in the HOA sense, that
  /// is, an acceptance formula plus a number of acceptance sets.  The
  /// acceptance formula is expected to use a subset of the acceptance
  /// sets.  (It usually uses *all* sets, otherwise that means that
  /// some of the sets have no influence on the automaton language and
  /// could be removed.)
  class SPOT_API acc_cond
  {

  public:
    bool
    has_parity_prefix(acc_cond& new_acc, std::vector<unsigned>& colors) const;

#ifndef SWIG
  private:
    [[noreturn]] static void report_too_many_sets();
#endif
  public:

    /// \brief An acceptance mark
    ///
    /// This type is used to represent a set of acceptance sets.  It
    /// works (and is implemented) like a bit vector where bit at
    /// index i represents the membership to the i-th acceptance set.
    ///
    /// Typically, each transition of an automaton is labeled by a
    /// mark_t that represents the membership of the transition to
    /// each of the acceptance sets.
    ///
    /// For efficiency reason, the maximum number of acceptance sets
    /// (i.e., the size of the bit vector) supported is a compile-time
    /// constant.  It can be changed by passing an option to the
    /// configure script of Spot.
    struct mark_t :
      public internal::_32acc<SPOT_MAX_ACCSETS == 8*sizeof(unsigned)>
    {
    private:
      // configure guarantees that SPOT_MAX_ACCSETS % (8*sizeof(unsigned)) == 0
      typedef bitset<SPOT_MAX_ACCSETS / (8*sizeof(unsigned))> _value_t;
      _value_t id;

      mark_t(_value_t id) noexcept
        : id(id)
      {
      }

    public:
      /// Initialize an empty mark_t.
      mark_t() = default;

      mark_t
      apply_permutation(std::vector<unsigned> permut);


#ifndef SWIG
      /// Create a mark_t from a range of set numbers.
      template<class iterator>
      mark_t(const iterator& begin, const iterator& end)
        : mark_t(_value_t::zero())
      {
        for (iterator i = begin; i != end; ++i)
          if (SPOT_LIKELY(*i < SPOT_MAX_ACCSETS))
            set(*i);
          else
            report_too_many_sets();
      }

      /// Create a mark_t from a list of set numbers.
      mark_t(std::initializer_list<unsigned> vals)
        : mark_t(vals.begin(), vals.end())
      {
      }

      SPOT_DEPRECATED("use brace initialization instead")
      mark_t(unsigned i)
      {
        unsigned j = 0;
        while (i)
          {
            if (i & 1U)
              this->set(j);
            ++j;
            i >>= 1;
          }
      }
#endif

      /// \brief The maximum number of acceptance sets supported by
      /// this implementation.
      ///
      /// The value can be changed at compile-time using configure's
      /// --enable-max-accsets=N option.
      constexpr static unsigned max_accsets()
      {
        return SPOT_MAX_ACCSETS;
      }

      /// \brief A mark_t with all bits set to one.
      ///
      /// Beware that *all* bits are sets, not just the bits used in
      /// the acceptance condition.  This class is unaware of the
      /// acceptance condition.
      static mark_t all()
      {
        return mark_t(_value_t::mone());
      }

      size_t hash() const noexcept
      {
        std::hash<decltype(id)> h;
        return h(id);
      }

      SPOT_DEPRECATED("compare mark_t to mark_t, not to unsigned")
      bool operator==(unsigned o) const
      {
        SPOT_ASSERT(o == 0U);
        (void)o;
        return !id;
      }

      SPOT_DEPRECATED("compare mark_t to mark_t, not to unsigned")
      bool operator!=(unsigned o) const
      {
        SPOT_ASSERT(o == 0U);
        (void)o;
        return !!id;
      }

      bool operator==(mark_t o) const
      {
        return id == o.id;
      }

      bool operator!=(mark_t o) const
      {
        return id != o.id;
      }

      bool operator<(mark_t o) const
      {
        return id < o.id;
      }

      bool operator<=(mark_t o) const
      {
        return id <= o.id;
      }

      bool operator>(mark_t o) const
      {
        return id > o.id;
      }

      bool operator>=(mark_t o) const
      {
        return id >= o.id;
      }

      explicit operator bool() const
      {
        return !!id;
      }

      bool has(unsigned u) const
      {
        return !!this->operator&(mark_t({0}) << u);
      }

      void set(unsigned u)
      {
        id.set(u);
      }

      void clear(unsigned u)
      {
        id.clear(u);
      }

      mark_t& operator&=(mark_t r)
      {
        id &= r.id;
        return *this;
      }

      mark_t& operator|=(mark_t r)
      {
        id |= r.id;
        return *this;
      }

      mark_t& operator-=(mark_t r)
      {
        id &= ~r.id;
        return *this;
      }

      mark_t& operator^=(mark_t r)
      {
        id ^= r.id;
        return *this;
      }

      mark_t operator&(mark_t r) const
      {
        return id & r.id;
      }

      mark_t operator|(mark_t r) const
      {
        return id | r.id;
      }

      mark_t operator-(mark_t r) const
      {
        return id & ~r.id;
      }

      mark_t operator~() const
      {
        return ~id;
      }

      mark_t operator^(mark_t r) const
      {
        return id ^ r.id;
      }

#if SPOT_DEBUG || defined(SWIGPYTHON)
#  define SPOT_WRAP_OP(ins)                     \
        try                                     \
          {                                     \
            ins;                                \
          }                                     \
        catch (const std::runtime_error& e)     \
          {                                     \
            report_too_many_sets();             \
          }
#else
#  define SPOT_WRAP_OP(ins) ins;
#endif
      mark_t operator<<(unsigned i) const
      {
        SPOT_WRAP_OP(return id << i);
      }

      mark_t& operator<<=(unsigned i)
      {
        SPOT_WRAP_OP(id <<= i; return *this);
      }

      mark_t operator>>(unsigned i) const
      {
        SPOT_WRAP_OP(return id >> i);
      }

      mark_t& operator>>=(unsigned i)
      {
        SPOT_WRAP_OP(id >>= i; return *this);
      }
#undef SPOT_WRAP_OP

      mark_t strip(mark_t y) const
      {
        // strip every bit of id that is marked in y
        //       100101110100.strip(
        //       001011001000)
        //   ==  10 1  11 100
        //   ==      10111100

        auto xv = id;                // 100101110100
        auto yv = y.id;                // 001011001000

        while (yv && xv)
          {
            // Mask for everything after the last 1 in y
            auto rm = (~yv) & (yv - 1);        // 000000000111
            // Mask for everything before the last 1 in y
            auto lm = ~(yv ^ (yv - 1));        // 111111110000
            xv = ((xv & lm) >> 1) | (xv & rm);
            yv = (yv & lm) >> 1;
          }
        return xv;
      }

      /// \brief Whether the set of bits represented by *this is a
      /// subset of those represented by \a m.
      bool subset(mark_t m) const
      {
        return !((*this) - m);
      }

      /// \brief Whether the set of bits represented by *this is a
      /// proper subset of those represented by \a m.
      bool proper_subset(mark_t m) const
      {
        return *this != m && this->subset(m);
      }

      /// \brief Number of bits sets.
      unsigned count() const
      {
        return id.count();
      }

      /// \brief The number of the highest set used plus one.
      ///
      /// If no set is used, this returns 0,
      /// If the sets {1,3,8} are used, this returns 9.
      unsigned max_set() const
      {
        if (id)
          return id.highest()+1;
        else
          return 0;
      }

      /// \brief The number of the lowest set used plus one.
      ///
      /// If no set is used, this returns 0.
      /// If the sets {1,3,8} are used, this returns 2.
      unsigned min_set() const
      {
        if (id)
          return id.lowest()+1;
        else
          return 0;
      }

      /// \brief A mark_t where all bits have been removed except the
      /// lowest one.
      ///
      /// For instance if this contains {1,3,8}, the output is {1}.
      mark_t lowest() const
      {
        return id & -id;
      }

      /// \brief Whether the mark contains only one bit set.
      bool is_singleton() const
      {
#if __GNUC__
        /* With GCC and Clang, count() is implemented using popcount. */
        return count() == 1;
#else
        return id && !(id & (id - 1));
#endif
      }

      /// \brief Whether the mark contains at least two bits set.
      bool has_many() const
      {
#if __GNUC__
        /* With GCC and Clang, count() is implemented using popcount. */
        return count() > 1;
#else
        return !!(id & (id - 1));
#endif
      }

      /// \brief Remove n bits that where set.
      ///
      /// If there are less than n bits set, the output is empty.
      mark_t& remove_some(unsigned n)
      {
        while (n--)
          id &= id - 1;
        return *this;
      }

      /// \brief Fill a container with the indices of the bits that are set.
      template<class iterator>
      void fill(iterator here) const
      {
        auto a = *this;
        unsigned level = 0;
        while (a)
          {
            if (a.has(0))
              *here++ = level;
            ++level;
            a >>= 1;
          }
      }

      /// Returns some iterable object that contains the used sets.
      spot::internal::mark_container sets() const;

      SPOT_API
      friend std::ostream& operator<<(std::ostream& os, mark_t m);

      std::string as_string() const
      {
        std::ostringstream os;
        os << *this;
        return os.str();
      }
    };

    /// \brief Operators for acceptance formulas.
    enum class acc_op : unsigned short
    { Inf, Fin, InfNeg, FinNeg, And, Or };

    /// \brief A "node" in an acceptance formulas.
    ///
    /// Acceptance formulas are stored as a vector of acc_word in a
    /// kind of reverse polish notation.  Each acc_word is either an
    /// operator, or a set of acceptance sets.  Operators come with a
    /// size that represent the number of words in the subtree,
    /// current operator excluded.
    union acc_word
    {
      mark_t mark;
      struct {
        acc_op op;             // Operator
        unsigned short size; // Size of the subtree (number of acc_word),
                             // not counting this node.
      } sub;
    };

    /// \brief An acceptance formula.
    ///
    /// Acceptance formulas are stored as a vector of acc_word in a
    /// kind of reverse polish notation.  The motivation for this
    /// design was that we could evaluate the acceptance condition
    /// using a very simple stack-based interpreter; however it turned
    /// out that such a stack-based interpretation would prevent us
    /// from doing short-circuit evaluation, so we are not evaluating
    /// acceptance conditions this way, and maybe the implementation
    /// of acc_code could change in the future.  It's best not to rely
    /// on the fact that formulas are stored as vectors.  Use the
    /// provided methods instead.
    struct SPOT_API acc_code: public std::vector<acc_word>
    {
      acc_code
      unit_propagation();

      bool
      has_parity_prefix(acc_cond& new_cond,
        std::vector<unsigned>& colors) const;

      bool
      is_parity_max_equiv(std::vector<int>& permut,
                        unsigned new_color,
                        bool even) const;

     bool operator==(const acc_code& other) const
      {
        unsigned pos = size();
        if (other.size() != pos)
          return false;
        while (pos > 0)
          {
            auto op = (*this)[pos - 1].sub.op;
            auto sz = (*this)[pos - 1].sub.size;
            if (other[pos - 1].sub.op != op ||
                other[pos - 1].sub.size != sz)
              return false;
            switch (op)
              {
              case acc_cond::acc_op::And:
              case acc_cond::acc_op::Or:
                --pos;
                break;
              case acc_cond::acc_op::Inf:
              case acc_cond::acc_op::InfNeg:
              case acc_cond::acc_op::Fin:
              case acc_cond::acc_op::FinNeg:
                pos -= 2;
                if (other[pos].mark != (*this)[pos].mark)
                  return false;
                break;
              }
          }
        return true;
      };

      bool operator<(const acc_code& other) const
      {
        unsigned pos = size();
        auto osize = other.size();
        if (pos < osize)
          return true;
        if (pos > osize)
          return false;
        while (pos > 0)
          {
            auto op = (*this)[pos - 1].sub.op;
            auto oop = other[pos - 1].sub.op;
            if (op < oop)
              return true;
            if (op > oop)
              return false;
            auto sz = (*this)[pos - 1].sub.size;
            auto osz = other[pos - 1].sub.size;
            if (sz < osz)
              return true;
            if (sz > osz)
              return false;
            switch (op)
              {
              case acc_cond::acc_op::And:
              case acc_cond::acc_op::Or:
                --pos;
                break;
              case acc_cond::acc_op::Inf:
              case acc_cond::acc_op::InfNeg:
              case acc_cond::acc_op::Fin:
              case acc_cond::acc_op::FinNeg:
                {
                  pos -= 2;
                  auto m = (*this)[pos].mark;
                  auto om = other[pos].mark;
                  if (m < om)
                    return true;
                  if (m > om)
                    return false;
                  break;
                }
              }
          }
        return false;
      }

      bool operator>(const acc_code& other) const
      {
        return other < *this;
      }

      bool operator<=(const acc_code& other) const
      {
        return !(other < *this);
      }

      bool operator>=(const acc_code& other) const
      {
        return !(*this < other);
      }

      bool operator!=(const acc_code& other) const
      {
        return !(*this == other);
      }

      /// \brief Is this the "true" acceptance condition?
      ///
      /// This corresponds to "t" in the HOA format.  Under this
      /// acceptance condition, all runs are accepting.
      bool is_t() const
      {
        // We store "t" as an empty condition, or as Inf({}).
        unsigned s = size();
        return s == 0 || ((*this)[s - 1].sub.op == acc_op::Inf
                          && !((*this)[s - 2].mark));
      }

      /// \brief Is this the "false" acceptance condition?
      ///
      /// This corresponds to "f" in the HOA format.  Under this
      /// acceptance condition, no runs is accepting.  Obviously, this
      /// has very few practical application, except as neutral
      /// element in some construction.
      bool is_f() const
      {
        // We store "f" as Fin({}).
        unsigned s = size();
        return s > 1
          && (*this)[s - 1].sub.op == acc_op::Fin && !((*this)[s - 2].mark);
      }

      /// \brief Construct the "false" acceptance condition.
      ///
      /// This corresponds to "f" in the HOA format.  Under this
      /// acceptance condition, no runs is accepting.  Obviously, this
      /// has very few practical application, except as neutral
      /// element in some construction.
      static acc_code f()
      {
        acc_code res;
        res.resize(2);
        res[0].mark = {};
        res[1].sub.op = acc_op::Fin;
        res[1].sub.size = 1;
        return res;
      }

      /// \brief Construct the "true" acceptance condition.
      ///
      /// This corresponds to "t" in the HOA format.  Under this
      /// acceptance condition, all runs are accepting.
      static acc_code t()
      {
        return {};
      }

      /// \brief Construct a generalized co-Büchi acceptance
      ///
      /// For the input m={1,8,9}, this constructs Fin(1)|Fin(8)|Fin(9).
      ///
      /// Internally, such a formula is stored using a single word
      /// Fin({1,8,9}).
      /// @{
      static acc_code fin(mark_t m)
      {
        acc_code res;
        res.resize(2);
        res[0].mark = m;
        res[1].sub.op = acc_op::Fin;
        res[1].sub.size = 1;
        return res;
      }

      static acc_code fin(std::initializer_list<unsigned> vals)
      {
        return fin(mark_t(vals));
      }
      /// @}

      /// \brief Construct a generalized co-Büchi acceptance for
      /// complemented sets.
      ///
      /// For the input `m={1,8,9}`, this constructs
      /// `Fin(!1)|Fin(!8)|Fin(!9)`.
      ///
      /// Internally, such a formula is stored using a single word
      /// `FinNeg({1,8,9})`.
      ///
      /// Note that `FinNeg` formulas are not supported by most methods
      /// of this class, and not supported by algorithms in Spot.
      /// This is mostly used in the parser for HOA files: if the
      /// input file uses `Fin(!0)` as acceptance condition, the
      /// condition will eventually be rewritten as `Fin(0)` by toggling
      /// the membership to set 0 of each transition.
      /// @{
      static acc_code fin_neg(mark_t m)
      {
        acc_code res;
        res.resize(2);
        res[0].mark = m;
        res[1].sub.op = acc_op::FinNeg;
        res[1].sub.size = 1;
        return res;
      }

      static acc_code fin_neg(std::initializer_list<unsigned> vals)
      {
        return fin_neg(mark_t(vals));
      }
      /// @}

      /// \brief Construct a generalized Büchi acceptance
      ///
      /// For the input `m={1,8,9}`, this constructs
      /// `Inf(1)&Inf(8)&Inf(9)`.
      ///
      /// Internally, such a formula is stored using a single word
      /// `Inf({1,8,9})`.
      /// @{
      static acc_code inf(mark_t m)
      {
        acc_code res;
        res.resize(2);
        res[0].mark = m;
        res[1].sub.op = acc_op::Inf;
        res[1].sub.size = 1;
        return res;
      }

      static acc_code inf(std::initializer_list<unsigned> vals)
      {
        return inf(mark_t(vals));
      }
      /// @}

      /// \brief Construct a generalized Büchi acceptance for
      /// complemented sets.
      ///
      /// For the input `m={1,8,9}`, this constructs
      /// `Inf(!1)&Inf(!8)&Inf(!9)`.
      ///
      /// Internally, such a formula is stored using a single word
      /// `InfNeg({1,8,9})`.
      ///
      /// Note that `InfNeg` formulas are not supported by most methods
      /// of this class, and not supported by algorithms in Spot.
      /// This is mostly used in the parser for HOA files: if the
      /// input file uses `Inf(!0)` as acceptance condition, the
      /// condition will eventually be rewritten as `Inf(0)` by toggling
      /// the membership to set 0 of each transition.
      /// @{
      static acc_code inf_neg(mark_t m)
      {
        acc_code res;
        res.resize(2);
        res[0].mark = m;
        res[1].sub.op = acc_op::InfNeg;
        res[1].sub.size = 1;
        return res;
      }

      static acc_code inf_neg(std::initializer_list<unsigned> vals)
      {
        return inf_neg(mark_t(vals));
      }
      /// @}

      /// \brief Build a Büchi acceptance condition.
      ///
      /// This builds the formula `Inf(0)`.
      static acc_code buchi()
      {
        return inf({0});
      }

      /// \brief Build a co-Büchi acceptance condition.
      ///
      /// This builds the formula `Fin(0)`.
      static acc_code cobuchi()
      {
        return fin({0});
      }

      /// \brief Build a generalized-Büchi acceptance condition with n sets
      ///
      /// This builds the formula `Inf(0)&Inf(1)&...&Inf(n-1)`.
      ///
      /// When n is zero, the acceptance condition reduces to true.
      static acc_code generalized_buchi(unsigned n)
      {
        if (n == 0)
          return inf({});
        acc_cond::mark_t m = mark_t::all();
        m >>= mark_t::max_accsets() - n;
        return inf(m);
      }

      /// \brief Build a generalized-co-Büchi acceptance condition with n sets
      ///
      /// This builds the formula `Fin(0)|Fin(1)|...|Fin(n-1)`.
      ///
      /// When n is zero, the acceptance condition reduces to false.
      static acc_code generalized_co_buchi(unsigned n)
      {
        if (n == 0)
          return fin({});
        acc_cond::mark_t m = mark_t::all();
        m >>= mark_t::max_accsets() - n;
        return fin(m);
      }

      /// \brief Build a Rabin condition with n pairs.
      ///
      /// This builds the formula
      /// `(Fin(0)&Inf(1))|(Fin(2)&Inf(3))|...|(Fin(2n-2)&Inf(2n-1))`
      static acc_code rabin(unsigned n)
      {
        acc_cond::acc_code res = f();
        while (n > 0)
          {
            res |= inf({2*n - 1}) & fin({2*n - 2});
            --n;
          }
        return res;
      }

      /// \brief Build a Streett condition with n pairs.
      ///
      /// This builds the formula
      /// `(Fin(0)|Inf(1))&(Fin(2)|Inf(3))&...&(Fin(2n-2)|Inf(2n-1))`
      static acc_code streett(unsigned n)
      {
        acc_cond::acc_code res = t();
        while (n > 0)
          {
            res &= inf({2*n - 1}) | fin({2*n - 2});
            --n;
          }
        return res;
      }

      /// \brief Build a generalized Rabin condition.
      ///
      /// The two iterators should point to a range of integers, each
      /// integer being the number of Inf term in a generalized Rabin pair.
      ///
      /// For instance if the input is `[2,3,0]`, the output
      /// will have three clauses (=generalized pairs), with 2 Inf terms in
      /// the first clause, 3 in the second, and 0 in the last:
      ///   `(Fin(0)&Inf(1)&Inf(2))|Fin(3)&Inf(4)&Inf(5)&Inf(6)|Fin(7)`.
      ///
      /// Since set numbers are not reused, the number of sets used is
      /// the sum of all input integers plus their count.
      template<class Iterator>
      static acc_code generalized_rabin(Iterator begin, Iterator end)
      {
        acc_cond::acc_code res = f();
        unsigned n = 0;
        for (Iterator i = begin; i != end; ++i)
          {
            unsigned f = n++;
            acc_cond::mark_t m = {};
            for (unsigned ni = *i; ni > 0; --ni)
              m.set(n++);
            auto pair = inf(m) & fin({f});
            std::swap(pair, res);
            res |= std::move(pair);
          }
        return res;
      }

      /// \brief Build a parity acceptance condition
      ///
      /// In parity acceptance a run is accepting if the maximum (or
      /// minimum) set number that is seen infinitely often is odd (or
      /// even).  These functions will build a formula for that, as
      /// specified in the HOA format.
      /// @{
      static acc_code parity(bool is_max, bool is_odd, unsigned sets);
      static acc_code parity_max(bool is_odd, unsigned sets)
      {
        return parity(true, is_odd, sets);
      }
      static acc_code parity_max_odd(unsigned sets)
      {
        return parity_max(true, sets);
      }
      static acc_code parity_max_even(unsigned sets)
      {
        return parity_max(false, sets);
      }
      static acc_code parity_min(bool is_odd, unsigned sets)
      {
        return parity(false, is_odd, sets);
      }
      static acc_code parity_min_odd(unsigned sets)
      {
        return parity_min(true, sets);
      }
      static acc_code parity_min_even(unsigned sets)
      {
        return parity_min(false, sets);
      }
      /// @}

      /// \brief Build a random acceptance condition
      ///
      /// If \a n is 0, this will always generate the true acceptance,
      /// because working with false acceptance is somehow pointless.
      ///
      /// For \a n>0, we randomly create a term Fin(i) or Inf(i) for
      /// each set 0≤i<n.  If \a reuse>0.0, it gives the probability
      /// that a set i can generate more than one Fin(i)/Inf(i) term.
      /// Set i will be reused as long as our [0,1) random number
      /// generator gives a value ≤reuse.  (Do not set reuse≥1.0 as
      /// that will give an infinite loop.)
      ///
      /// All these Fin/Inf terms are the leaves of the tree we are
      /// building.  That tree is then build by picking two random
      /// subtrees, and joining them with & and | randomly, until we
      /// are left with a single tree.
      static acc_code random(unsigned n, double reuse = 0.0);

      /// \brief Conjunct the current condition in place with \a r.
      acc_code& operator&=(const acc_code& r)
      {
        if (is_t() || r.is_f())
          {
            *this = r;
            return *this;
          }
        if (is_f() || r.is_t())
          return *this;
        unsigned s = size() - 1;
        unsigned rs = r.size() - 1;
        // We want to group all Inf(x) operators:
        //   Inf(a) & Inf(b) = Inf(a & b)
        if (((*this)[s].sub.op == acc_op::Inf
             && r[rs].sub.op == acc_op::Inf)
            || ((*this)[s].sub.op == acc_op::InfNeg
                && r[rs].sub.op == acc_op::InfNeg))
          {
            (*this)[s - 1].mark |= r[rs - 1].mark;
            return *this;
          }

        // In the more complex scenarios, left and right may both
        // be conjunctions, and Inf(x) might be a member of each
        // side.  Find it if it exists.
        // left_inf points to the left Inf mark if any.
        // right_inf points to the right Inf mark if any.
        acc_word* left_inf = nullptr;
        if ((*this)[s].sub.op == acc_op::And)
          {
            auto start = &(*this)[s] - (*this)[s].sub.size;
            auto pos = &(*this)[s] - 1;
            pop_back();
            while (pos > start)
              {
                if (pos->sub.op == acc_op::Inf)
                  {
                    left_inf = pos - 1;
                    break;
                  }
                pos -= pos->sub.size + 1;
              }
          }
        else if ((*this)[s].sub.op == acc_op::Inf)
          {
            left_inf = &(*this)[s - 1];
          }

        const acc_word* right_inf = nullptr;
        auto right_end = &r.back();
        if (right_end->sub.op == acc_op::And)
          {
            auto start = &r[0];
            auto pos = --right_end;
            while (pos > start)
            {
              if (pos->sub.op == acc_op::Inf)
                {
                  right_inf = pos - 1;
                  break;
                }
              pos -= pos->sub.size + 1;
            }
          }
        else if (right_end->sub.op == acc_op::Inf)
          {
            right_inf = right_end - 1;
          }

        acc_cond::mark_t carry = {};
        if (left_inf && right_inf)
          {
            carry = left_inf->mark;
            auto pos = left_inf - &(*this)[0];
            erase(begin() + pos, begin() + pos + 2);
          }
        auto sz = size();
        insert(end(), &r[0], right_end + 1);
        if (carry)
          (*this)[sz + (right_inf - &r[0])].mark |= carry;

        acc_word w;
        w.sub.op = acc_op::And;
        w.sub.size = size();
        emplace_back(w);
        return *this;
      }

      /// \brief Conjunct the current condition with \a r.
      acc_code operator&(const acc_code& r) const
      {
        acc_code res = *this;
        res &= r;
        return res;
      }

      /// \brief Conjunct the current condition with \a r.
      acc_code operator&(acc_code&& r) const
      {
        acc_code res = *this;
        res &= r;
        return res;
      }

      /// \brief Disjunct the current condition in place with \a r.
      acc_code& operator|=(const acc_code& r)
      {
        if (is_t() || r.is_f())
          return *this;
        if (is_f() || r.is_t())
          {
            *this = r;
            return *this;
          }
        unsigned s = size() - 1;
        unsigned rs = r.size() - 1;
        // Fin(a) | Fin(b) = Fin(a | b)
        if (((*this)[s].sub.op == acc_op::Fin
             && r[rs].sub.op == acc_op::Fin)
            || ((*this)[s].sub.op == acc_op::FinNeg
                && r[rs].sub.op == acc_op::FinNeg))
          {
            (*this)[s - 1].mark |= r[rs - 1].mark;
            return *this;
          }

        // In the more complex scenarios, left and right may both
        // be disjunctions, and Fin(x) might be a member of each
        // side.  Find it if it exists.
        // left_inf points to the left Inf mark if any.
        // right_inf points to the right Inf mark if any.
        acc_word* left_fin = nullptr;
        if ((*this)[s].sub.op == acc_op::Or)
          {
            auto start = &(*this)[s] - (*this)[s].sub.size;
            auto pos = &(*this)[s] - 1;
            pop_back();
            while (pos > start)
              {
                if (pos->sub.op == acc_op::Fin)
                  {
                    left_fin = pos - 1;
                    break;
                  }
                pos -= pos->sub.size + 1;
              }
          }
        else if ((*this)[s].sub.op == acc_op::Fin)
          {
            left_fin = &(*this)[s - 1];
          }

        const acc_word* right_fin = nullptr;
        auto right_end = &r.back();
        if (right_end->sub.op == acc_op::Or)
          {
            auto start = &r[0];
            auto pos = --right_end;
            while (pos > start)
            {
              if (pos->sub.op == acc_op::Fin)
                {
                  right_fin = pos - 1;
                  break;
                }
              pos -= pos->sub.size + 1;
            }
          }
        else if (right_end->sub.op == acc_op::Fin)
          {
            right_fin = right_end - 1;
          }

        acc_cond::mark_t carry = {};
        if (left_fin && right_fin)
          {
            carry = left_fin->mark;
            auto pos = (left_fin - &(*this)[0]);
            this->erase(begin() + pos, begin() + pos + 2);
          }
        auto sz = size();
        insert(end(), &r[0], right_end + 1);
        if (carry)
          (*this)[sz + (right_fin - &r[0])].mark |= carry;
        acc_word w;
        w.sub.op = acc_op::Or;
        w.sub.size = size();
        emplace_back(w);
        return *this;
      }

      /// \brief Disjunct the current condition with \a r.
      acc_code operator|(acc_code&& r) const
      {
        acc_code res = *this;
        res |= r;
        return res;
      }

      /// \brief Disjunct the current condition with \a r.
      acc_code operator|(const acc_code& r) const
      {
        acc_code res = *this;
        res |= r;
        return res;
      }

      /// \brief Apply a left shift to all mark_t that appear in the condition.
      ///
      /// Shifting `Fin(0)&Inf(3)` by 2 will give `Fin(2)&Inf(5)`.
      ///
      /// The result is modified in place.
      acc_code& operator<<=(unsigned sets)
      {
        if (SPOT_UNLIKELY(sets >= mark_t::max_accsets()))
          report_too_many_sets();
        if (empty())
          return *this;
        unsigned pos = size();
        do
          {
            switch ((*this)[pos - 1].sub.op)
              {
              case acc_cond::acc_op::And:
              case acc_cond::acc_op::Or:
                --pos;
                break;
              case acc_cond::acc_op::Inf:
              case acc_cond::acc_op::InfNeg:
              case acc_cond::acc_op::Fin:
              case acc_cond::acc_op::FinNeg:
                pos -= 2;
                (*this)[pos].mark <<= sets;
                break;
              }
          }
        while (pos > 0);
        return *this;
      }

      /// \brief Apply a left shift to all mark_t that appear in the condition.
      ///
      /// Shifting `Fin(0)&Inf(3)` by 2 will give `Fin(2)&Inf(5)`.
      acc_code operator<<(unsigned sets) const
      {
        acc_code res = *this;
        res <<= sets;
        return res;
      }

      /// \brief Whether the acceptance formula is in disjunctive normal form.
      ///
      /// The formula is in DNF if it is either:
      /// - one of `t`, `f`, `Fin(i)`, `Inf(i)`
      /// - a conjunction of any of the above
      /// - a disjunction of any of the above (including the conjunctions).
      bool is_dnf() const;

      /// \brief Whether the acceptance formula is in conjunctive normal form.
      ///
      /// The formula is in DNF if it is either:
      /// - one of `t`, `f`, `Fin(i)`, `Inf(i)`
      /// - a disjunction of any of the above
      /// - a conjunction of any of the above (including the disjunctions).
      bool is_cnf() const;

      /// \brief Convert the acceptance formula into disjunctive normal form
      ///
      /// This works by distributing `&` over `|`, resulting in a formula that
      /// can be exponentially larger than the input.
      ///
      /// The implementation works by converting the formula into a
      /// BDD where `Inf(i)` is encoded by vᵢ and `Fin(i)` is encoded
      /// by !vᵢ, and then finding prime implicants to build an
      /// irredundant sum-of-products.  In practice, the results are
      /// usually better than what we would expect by hand.
      acc_code to_dnf() const;

      /// \brief Convert the acceptance formula into disjunctive normal form
      ///
      /// This works by distributing `|` over `&`, resulting in a formula that
      /// can be exponentially larger than the input.
      ///
      /// This implementation is the dual of `to_dnf()`.
      acc_code to_cnf() const;


      /// \brief Return the top-level disjuncts.
      ///
      /// For instance, if the formula is
      /// Fin(0)|Fin(1)|(Fin(2)&(Inf(3)|Fin(4))), this returns
      /// [Fin(0), Fin(1), Fin(2)&(Inf(3)|Fin(4))].
      ///
      /// If the formula is not a disjunction, this returns
      /// a vector with the formula as only element.
      std::vector<acc_code> top_disjuncts() const;

      /// \brief Return the top-level conjuncts.
      ///
      /// For instance, if the formula is
      /// Fin(0)|Fin(1)|(Fin(2)&(Inf(3)|Fin(4))), this returns
      /// [Fin(0), Fin(1), Fin(2)&(Inf(3)|Fin(4))].
      ///
      /// If the formula is not a conjunction, this returns
      /// a vector with the formula as only element.
      std::vector<acc_code> top_conjuncts() const;

      /// \brief Complement an acceptance formula.
      ///
      /// Also known as "dualizing the acceptance condition" since
      /// this replaces `Fin` ↔ `Inf`, and `&` ↔ `|`.
      ///
      /// Not that dualizing the acceptance condition on a
      /// deterministic automaton is enough to complement that
      /// automaton.  On a non-deterministic automaton, you should
      /// also replace existential choices by universal choices,
      /// as done by the dualize() function.
      acc_code complement() const;

      /// \brief Find a `Fin(i)` that is a unit clause.
      ///
      /// This return a mark_t `{i}` such that `Fin(i)` appears as a
      /// unit clause in the acceptance condition.  I.e., either the
      /// condition is exactly `Fin(i)`, or the condition has the form
      /// `...&Fin(i)&...`.  If there is no such `Fin(i)`, an empty
      /// mark_t is returned.
      ///
      /// If multiple unit-Fin appear as unit-clauses, the set of
      /// those will be returned.  For instance applied to
      /// `Fin(0)&Fin(1)&(Inf(2)|Fin(3))`, this will return `{0,1}`.
      mark_t fin_unit() const;

      /// \brief Return one acceptance set i that appear as `Fin(i)`
      /// in the condition.
      ///
      /// Return -1 if no such set exist.
      int fin_one() const;

      /// \brief Help closing accepting or rejecting cycle.
      ///
      /// Assuming you have a partial cycle visiting all acceptance
      /// sets in \a inf, this returns the combination of set you
      /// should see or avoid when closing the cycle to make it
      /// accepting or rejecting (as specified with \a accepting).
      ///
      /// The result is a vector of vectors of integers.
      /// A positive integer x denote a set that should be seen,
      /// a negative value x means the set -x-1 must be absent.
      /// The different inter vectors correspond to different
      /// solutions satisfying the \a accepting criterion.
      std::vector<std::vector<int>>
        missing(mark_t inf, bool accepting) const;

      /// \brief Check whether visiting *exactly* all sets \a inf
      /// infinitely often satisfies the acceptance condition.
      bool accepting(mark_t inf) const;

      /// \brief Assuming that we will visit at least all sets in \a
      /// inf, is there any chance that we will satisfy the condition?
      ///
      /// This return false only when it is sure that visiting more
      /// set will never make the condition satisfiable.
      bool inf_satisfiable(mark_t inf) const;

      /// \brief Check potential acceptance of an SCC.
      ///
      /// Assuming that an SCC intersects all sets in \a
      /// infinitely_often (i.e., for each set in \a infinetely_often,
      /// there exist one marked transition in the SCC), and is
      /// included in all sets in \a always_present (i.e., all
      /// transitions are marked with \a always_present), this returns
      /// one tree possible results:
      /// - trival::yes() the SCC is necessarily accepting,
      /// - trival::no() the SCC is necessarily rejecting,
      /// - trival::maybe() the SCC could contain an accepting cycle.
      trival maybe_accepting(mark_t infinitely_often,
                             mark_t always_present) const;

      /// \brief compute the symmetry class of the acceptance sets.
      ///
      /// Two sets x and y are symmetric if swapping them in the
      /// acceptance condition produces an equivalent formula.
      /// For instance 0 and 2 are symmetric in Inf(0)&Fin(1)&Inf(2).
      ///
      /// The returned vector is indexed by set numbers, and each
      /// entry points to the "root" (or representative element) of
      /// its symmetry class.  In the above example the result would
      /// be [0,1,0], showing that 0 and 2 are in the same class.
      std::vector<unsigned> symmetries() const;

      /// \brief Remove all the acceptance sets in \a rem.
      ///
      /// If \a missing is set, the acceptance sets are assumed to be
      /// missing from the automaton, and the acceptance is updated to
      /// reflect this.  For instance `(Inf(1)&Inf(2))|Fin(3)` will
      /// become `Fin(3)` if we remove `2` because it is missing from this
      /// automaton.  Indeed there is no way to fulfill `Inf(1)&Inf(2)`
      /// in this case.  So essentially `missing=true` causes Inf(rem) to
      /// become `f`, and `Fin(rem)` to become `t`.
      ///
      /// If \a missing is unset, `Inf(rem)` become `t` while
      /// `Fin(rem)` become `f`.  Removing `2` from
      /// `(Inf(1)&Inf(2))|Fin(3)` would then give `Inf(1)|Fin(3)`.
      acc_code remove(acc_cond::mark_t rem, bool missing) const;

      /// \brief Remove acceptance sets, and shift set numbers
      ///
      /// Same as remove, but also shift set numbers in the result so
      /// that all used set numbers are continuous.
      acc_code strip(acc_cond::mark_t rem, bool missing) const;
      /// \brief For all `x` in \a m, replaces `Fin(x)` by `false`.
      acc_code force_inf(mark_t m) const;

      /// \brief Return the set of sets appearing in the condition.
      acc_cond::mark_t used_sets() const;

      /// \brief Return the sets that appears only once in the
      /// acceptance.
      ///
      /// For instance if the condition is
      /// `Fin(0)&(Inf(1)|(Fin(1)&Inf(2)))`, this returns `{0,2}`,
      /// because set `1` is used more than once.
      mark_t used_once_sets() const;

      /// \brief  Return the sets used as Inf or Fin in the acceptance condition
      std::pair<acc_cond::mark_t, acc_cond::mark_t> used_inf_fin_sets() const;

      /// \brief Print the acceptance formula as HTML.
      ///
      /// The set_printer function can be used to customize the output
      /// of set numbers.
      std::ostream&
      to_html(std::ostream& os,
              std::function<void(std::ostream&, int)>
              set_printer = nullptr) const;

      /// \brief Print the acceptance formula as text.
      ///
      /// The set_printer function can be used to customize the output
      /// of set numbers.
      std::ostream&
      to_text(std::ostream& os,
              std::function<void(std::ostream&, int)>
              set_printer = nullptr) const;

      /// \brief Print the acceptance formula as LaTeX.
      ///
      /// The set_printer function can be used to customize the output
      /// of set numbers.
      std::ostream&
      to_latex(std::ostream& os,
               std::function<void(std::ostream&, int)>
               set_printer = nullptr) const;

      /// \brief Construct an acc_code from a string.
      ///
      /// The string should either follow the following grammar:
      ///
      /// <pre>
      ///   acc ::= "t"
      ///         | "f"
      ///         | "Inf" "(" num ")"
      ///         | "Fin" "(" num ")"
      ///         | "(" acc ")"
      ///         | acc "&" acc
      ///         | acc "|" acc
      /// </pre>
      ///
      /// Where num is an integer and "&" has priority over "|".  Note that
      /// "Fin(!x)" and "Inf(!x)" are not supported by this parser.
      ///
      /// Or the string could be the name of an acceptance condition, as
      /// speficied in the HOA format.  (E.g. "Rabin 2", "parity max odd 3",
      /// "generalized-Rabin 4 2 1", etc.).
      ///
      /// A spot::parse_error is thrown on syntax error.
      acc_code(const char* input);

      /// \brief Build an empty acceptance formula.
      ///
      /// This is the same as t().
      acc_code()
      {
      }

      /// \brief Copy a part of another acceptance formula
      acc_code(const acc_word* other)
        : std::vector<acc_word>(other - other->sub.size, other + 1)
      {
      }

      /// \brief prints the acceptance formula as text
      SPOT_API
      friend std::ostream& operator<<(std::ostream& os, const acc_code& code);
    };

    /// \brief Build an acceptance condition
    ///
    /// This takes a number of sets \a n_sets, and an acceptance
    /// formula (\a code) over those sets.
    ///
    /// The number of sets should be at least cover all the sets used
    /// in \a code.
    acc_cond(unsigned n_sets = 0, const acc_code& code = {})
      : num_(0U), all_({}), code_(code)
    {
      add_sets(n_sets);
      uses_fin_acceptance_ = check_fin_acceptance();
    }

    /// \brief Build an acceptance condition
    ///
    /// In this version, the number of sets is set the the smallest
    /// number necessary for \a code.
    acc_cond(const acc_code& code)
      : num_(0U), all_({}), code_(code)
    {
      add_sets(code.used_sets().max_set());
      uses_fin_acceptance_ = check_fin_acceptance();
    }

    /// \brief Copy an acceptance condition
    acc_cond(const acc_cond& o)
      : num_(o.num_), all_(o.all_), code_(o.code_),
        uses_fin_acceptance_(o.uses_fin_acceptance_)
    {
    }

    /// \brief Copy an acceptance condition
    acc_cond& operator=(const acc_cond& o)
    {
      num_ = o.num_;
      all_ = o.all_;
      code_ = o.code_;
      uses_fin_acceptance_ = o.uses_fin_acceptance_;
      return *this;
    }

    ~acc_cond()
    {
    }

    /// \brief Change the acceptance formula.
    ///
    /// Beware, this does not change the number of declared sets.
    void set_acceptance(const acc_code& code)
    {
      code_ = code;
      uses_fin_acceptance_ = check_fin_acceptance();
    }

    /// \brief Retrieve the acceptance formula
    const acc_code& get_acceptance() const
    {
      return code_;
    }

    /// \brief Retrieve the acceptance formula
    acc_code& get_acceptance()
    {
      return code_;
    }

    bool operator==(const acc_cond& other) const
    {
      return other.num_sets() == num_ && other.get_acceptance() == code_;
    }

    bool operator!=(const acc_cond& other) const
    {
      return !(*this == other);
    }

    /// \brief Whether the acceptance condition uses Fin terms
    bool uses_fin_acceptance() const
    {
      return uses_fin_acceptance_;
    }

    /// \brief Whether the acceptance formula is "t" (true)
    bool is_t() const
    {
      return code_.is_t();
    }

    /// \brief Whether the acceptance condition is "all"
    ///
    /// In the HOA format, the acceptance condition "all" correspond
    /// to the formula "t" with 0 declared acceptance sets.
    bool is_all() const
    {
      return num_ == 0 && is_t();
    }

    /// \brief Whether the acceptance formula is "f" (false)
    bool is_f() const
    {
      return code_.is_f();
    }

    /// \brief Whether the acceptance condition is "none"
    ///
    /// In the HOA format, the acceptance condition "all" correspond
    /// to the formula "f" with 0 declared acceptance sets.
    bool is_none() const
    {
      return num_ == 0 && is_f();
    }

    /// \brief Whether the acceptance condition is "Büchi"
    ///
    /// The acceptance condition is Büchi if its formula is Inf(0) and
    /// only 1 set is used.
    bool is_buchi() const
    {
      unsigned s = code_.size();
      return num_ == 1 &&
        s == 2 && code_[1].sub.op == acc_op::Inf && code_[0].mark == all_sets();
    }

    /// \brief Whether the acceptance condition is "co-Büchi"
    ///
    /// The acceptance condition is co-Büchi if its formula is Fin(0) and
    /// only 1 set is used.
    bool is_co_buchi() const
    {
      return num_ == 1 && is_generalized_co_buchi();
    }

    /// \brief Change the acceptance condition to generalized-Büchi,
    /// over all declared sets.
    void set_generalized_buchi()
    {
      set_acceptance(inf(all_sets()));
    }

    /// \brief Change the acceptance condition to
    /// generalized-co-Büchi, over all declared sets.
    void set_generalized_co_buchi()
    {
      set_acceptance(fin(all_sets()));
    }

    /// \brief Whether the acceptance condition is "generalized-Büchi"
    ///
    /// The acceptance condition with n sets is generalized-Büchi if its
    /// formula is Inf(0)&Inf(1)&...&Inf(n-1).
    bool is_generalized_buchi() const
    {
      unsigned s = code_.size();
      return (s == 0 && num_ == 0) || (s == 2 && code_[1].sub.op == acc_op::Inf
                                       && code_[0].mark == all_sets());
    }

    /// \brief Whether the acceptance condition is "generalized-co-Büchi"
    ///
    /// The acceptance condition with n sets is generalized-co-Büchi if its
    /// formula is Fin(0)|Fin(1)|...|Fin(n-1).
    bool is_generalized_co_buchi() const
    {
      unsigned s = code_.size();
      return (s == 2 &&
              code_[1].sub.op == acc_op::Fin && code_[0].mark == all_sets());
    }

    /// \brief Check if the acceptance condition matches the Rabin
    /// acceptance of the HOA format
    ///
    /// Rabin acceptance over 2n sets look like
    /// `(Fin(0)&Inf(1))|...|(Fin(2n-2)&Inf(2n-1))`; i.e., a
    /// disjunction of n pairs of the form `Fin(2i)&Inf(2i+1)`.
    ///
    /// `f` is a special kind of Rabin acceptance with 0 pairs.
    ///
    /// This function returns a number of pairs (>=0) or -1 if the
    /// acceptance condition is not Rabin.
    int is_rabin() const;

    /// \brief Check if the acceptance condition matches the Streett
    /// acceptance of the HOA format
    ///
    /// Streett acceptance over 2n sets look like
    /// `(Fin(0)|Inf(1))&...&(Fin(2n-2)|Inf(2n-1))`; i.e., a
    /// conjunction of n pairs of the form `Fin(2i)|Inf(2i+1)`.
    ///
    /// `t` is a special kind of Streett acceptance with 0 pairs.
    ///
    /// This function returns a number of pairs (>=0) or -1 if the
    /// acceptance condition is not Streett.
    int is_streett() const;

    /// \brief Rabin/streett pairs used by is_rabin_like and is_streett_like.
    ///
    /// These pairs hold two marks which can each contain one or no set number.
    ///
    /// For instance is_streett_like() rewrites  Inf(0)&(Inf(2)|Fin(1))&Fin(3)
    /// as three pairs: [(fin={},inf={0}),(fin={1},inf={2}),(fin={3},inf={})].
    ///
    /// Empty marks should be interpreted in a way that makes them
    /// false in Streett, and true in Rabin.
    struct SPOT_API rs_pair
    {
#ifndef SWIG
      rs_pair() = default;
      rs_pair(const rs_pair&) = default;
#endif

      rs_pair(acc_cond::mark_t fin, acc_cond::mark_t inf) noexcept:
        fin(fin),
        inf(inf)
        {}
      acc_cond::mark_t fin;
      acc_cond::mark_t inf;

      bool operator==(rs_pair o) const
      {
        return fin == o.fin && inf == o.inf;
      }
      bool operator!=(rs_pair o) const
      {
        return fin != o.fin || inf != o.inf;
      }
      bool operator<(rs_pair o) const
      {
        return fin < o.fin || (!(o.fin < fin) && inf < o.inf);
      }
      bool operator<=(rs_pair o) const
      {
        return !(o < *this);
      }
      bool operator>(rs_pair o) const
      {
        return o < *this;
      }
      bool operator>=(rs_pair o) const
      {
        return !(*this < o);
      }
    };
    /// \brief Test whether an acceptance condition is Streett-like
    ///  and returns each Streett pair in an std::vector<rs_pair>.
    ///
    /// An acceptance condition is Streett-like if it can be transformed into
    /// a Streett acceptance with little modification to its automaton.
    /// A Streett-like acceptance condition follows one of those rules:
    /// -It is a conjunction of disjunctive clauses containing at most one
    ///  Inf and at most one Fin.
    /// -It is true (with 0 pair)
    /// -It is false (1 pair [0U, 0U])
    bool is_streett_like(std::vector<rs_pair>& pairs) const;

    /// \brief Test whether an acceptance condition is Rabin-like
    ///  and returns each Rabin pair in an std::vector<rs_pair>.
    ///
    /// An acceptance condition is Rabin-like if it can be transformed into
    /// a Rabin acceptance with little modification to its automaton.
    /// A Rabin-like acceptance condition follows one of those rules:
    /// -It is a disjunction of conjunctive clauses containing at most one
    ///  Inf and at most one Fin.
    /// -It is true (1 pair [0U, 0U])
    /// -It is false (0 pairs)
    bool is_rabin_like(std::vector<rs_pair>& pairs) const;

    /// \brief Is the acceptance condition generalized-Rabin?
    ///
    /// Check if the condition follows the generalized-Rabin
    /// definition of the HOA format.  So one Fin should be in front
    /// of each generalized pair, and set numbers should all be used
    /// once.
    ///
    /// When true is returned, the \a pairs vector contains the number
    /// of `Inf` term in each pair.  Otherwise, \a pairs is emptied.
    bool is_generalized_rabin(std::vector<unsigned>& pairs) const;

    /// \brief Is the acceptance condition generalized-Streett?
    ///
    /// There is no definition of generalized Streett in HOA v1,
    /// so this uses the definition from the development version
    /// of the HOA format, that should eventually become HOA v1.1 or
    /// HOA v2.
    ///
    /// One Inf should be in front of each generalized pair, and
    /// set numbers should all be used once.
    ///
    /// When true is returned, the \a pairs vector contains the number
    /// of `Fin` term in each pair.  Otherwise, \a pairs is emptied.
    bool is_generalized_streett(std::vector<unsigned>& pairs) const;

    /// \brief check is the acceptance condition matches one of the
    /// four type of parity acceptance defined in the HOA format.
    ///
    /// On success, this return true and sets \a max, and \a odd to
    /// the type of parity acceptance detected.  By default \a equiv =
    /// false, and the parity acceptance should match exactly the
    /// order of operators given in the HOA format.  If \a equiv is
    /// set, any formula that this logically equivalent to one of the
    /// HOA format will be accepted.
    bool is_parity(bool& max, bool& odd, bool equiv = false) const;


    bool is_parity_max_equiv(std::vector<int>& permut, bool even) const;

    /// \brief check is the acceptance condition matches one of the
    /// four type of parity acceptance defined in the HOA format.
    bool is_parity() const
    {
      bool max;
      bool odd;
      return is_parity(max, odd);
    }

    /// \brief Remove superfluous Fin and Inf by unit propagation.
    ///
    /// For example in `Fin(0)|(Inf(0) & Fin(1))`, `Inf(0)` is true
    /// iff `Fin(0)` is false so we can rewrite it as `Fin(0)|Fin(1)`.
    ///
    /// The number of acceptance sets is not modified even if some do
    /// not appear in the acceptance condition anymore.
    acc_cond unit_propagation()
    {
      return acc_cond(num_, code_.unit_propagation());
    }

    // Return (true, m) if there exist some acceptance mark m that
    // does not satisfy the acceptance condition.  Return (false, 0U)
    // otherwise.
    std::pair<bool, acc_cond::mark_t> unsat_mark() const
    {
      return sat_unsat_mark(false);
    }
    // Return (true, m) if there exist some acceptance mark m that
    // does satisfy the acceptance condition.  Return (false, 0U)
    // otherwise.
    std::pair<bool, acc_cond::mark_t> sat_mark() const
    {
      return sat_unsat_mark(true);
    }

  protected:
    bool check_fin_acceptance() const;
    std::pair<bool, acc_cond::mark_t> sat_unsat_mark(bool) const;

  public:
    /// \brief Construct a generalized Büchi acceptance
    ///
    /// For the input `m={1,8,9}`, this constructs
    /// `Inf(1)&Inf(8)&Inf(9)`.
    ///
    /// Internally, such a formula is stored using a single word
    /// `Inf({1,8,9})`.
    /// @{
    static acc_code inf(mark_t mark)
    {
      return acc_code::inf(mark);
    }

    static acc_code inf(std::initializer_list<unsigned> vals)
    {
      return inf(mark_t(vals.begin(), vals.end()));
    }
    /// @}

    /// \brief Construct a generalized Büchi acceptance for
    /// complemented sets.
    ///
    /// For the input `m={1,8,9}`, this constructs
    /// `Inf(!1)&Inf(!8)&Inf(!9)`.
    ///
    /// Internally, such a formula is stored using a single word
    /// `InfNeg({1,8,9})`.
    ///
    /// Note that `InfNeg` formulas are not supported by most methods
    /// of this class, and not supported by algorithms in Spot.
    /// This is mostly used in the parser for HOA files: if the
    /// input file uses `Inf(!0)` as acceptance condition, the
    /// condition will eventually be rewritten as `Inf(0)` by toggling
    /// the membership to set 0 of each transition.
    /// @{
    static acc_code inf_neg(mark_t mark)
    {
      return acc_code::inf_neg(mark);
    }

    static acc_code inf_neg(std::initializer_list<unsigned> vals)
    {
      return inf_neg(mark_t(vals.begin(), vals.end()));
    }
    /// @}

    /// \brief Construct a generalized co-Büchi acceptance
    ///
    /// For the input m={1,8,9}, this constructs Fin(1)|Fin(8)|Fin(9).
    ///
    /// Internally, such a formula is stored using a single word
    /// Fin({1,8,9}).
    /// @{
    static acc_code fin(mark_t mark)
    {
      return acc_code::fin(mark);
    }

    static acc_code fin(std::initializer_list<unsigned> vals)
    {
      return fin(mark_t(vals.begin(), vals.end()));
    }
    /// @}

    /// \brief Construct a generalized co-Büchi acceptance for
    /// complemented sets.
    ///
    /// For the input `m={1,8,9}`, this constructs
    /// `Fin(!1)|Fin(!8)|Fin(!9)`.
    ///
    /// Internally, such a formula is stored using a single word
    /// `FinNeg({1,8,9})`.
    ///
    /// Note that `FinNeg` formulas are not supported by most methods
    /// of this class, and not supported by algorithms in Spot.
    /// This is mostly used in the parser for HOA files: if the
    /// input file uses `Fin(!0)` as acceptance condition, the
    /// condition will eventually be rewritten as `Fin(0)` by toggling
    /// the membership to set 0 of each transition.
    /// @{
    static acc_code fin_neg(mark_t mark)
    {
      return acc_code::fin_neg(mark);
    }

    static acc_code fin_neg(std::initializer_list<unsigned> vals)
    {
      return fin_neg(mark_t(vals.begin(), vals.end()));
    }
    /// @}

    /// \brief Add more sets to the acceptance condition.
    ///
    /// This simply augment the number of sets, without changing the
    /// acceptance formula.
    unsigned add_sets(unsigned num)
    {
      if (num == 0)
        return -1U;
      unsigned j = num_;
      num += j;
      if (num > mark_t::max_accsets())
        report_too_many_sets();
      // Make sure we do not update if we raised an exception.
      num_ = num;
      all_ = all_sets_();
      return j;
    }

    /// \brief Add a single set to the acceptance condition.
    ///
    /// This simply augment the number of sets, without changing the
    /// acceptance formula.
    unsigned add_set()
    {
      return add_sets(1);
    }

    /// \brief Build a mark_t with a single set
    mark_t mark(unsigned u) const
    {
      SPOT_ASSERT(u < num_sets());
      return mark_t({u});
    }

    /// \brief Complement a mark_t
    ///
    /// Complementation is done with respect to the number of sets
    /// declared.
    mark_t comp(const mark_t& l) const
    {
      return all_ ^ l;
    }

    /// \brief Construct a mark_t with all declared sets.
    mark_t all_sets() const
    {
      return all_;
    }

    acc_cond
    apply_permutation(std::vector<unsigned>permut)
    {
      return acc_cond(apply_permutation_aux(permut));
    }

    acc_code
    apply_permutation_aux(std::vector<unsigned>permut)
    {
      auto conj = top_conjuncts();
      auto disj = top_disjuncts();

      if (conj.size() > 1)
      {
        auto transformed = std::vector<acc_code>();
        for (auto elem : conj)
          transformed.push_back(elem.apply_permutation_aux(permut));
        std::sort(transformed.begin(), transformed.end());
        auto uniq = std::unique(transformed.begin(), transformed.end());
        auto result = std::accumulate(transformed.begin(), uniq, acc_code::t(),
          [](acc_code c1, acc_code c2)
              {
                return c1 & c2;
              });
        return result;
      }
      else if (disj.size() > 1)
      {
        auto transformed = std::vector<acc_code>();
        for (auto elem : disj)
          transformed.push_back(elem.apply_permutation_aux(permut));
        std::sort(transformed.begin(), transformed.end());
        auto uniq = std::unique(transformed.begin(), transformed.end());
        auto result = std::accumulate(transformed.begin(), uniq, acc_code::f(),
          [](acc_code c1, acc_code c2)
              {
                return c1 | c2;
              });
        return result;
      }
      else
      {
        if (code_.back().sub.op == acc_cond::acc_op::Fin)
          return fin(code_[0].mark.apply_permutation(permut));
        if (code_.back().sub.op == acc_cond::acc_op::Inf)
          return inf(code_[0].mark.apply_permutation(permut));
      }
      SPOT_ASSERT(false);
      return {};
    }

    /// \brief Check whether visiting *exactly* all sets \a inf
    /// infinitely often satisfies the acceptance condition.
    bool accepting(mark_t inf) const
    {
      return code_.accepting(inf);
    }

    /// \brief Assuming that we will visit at least all sets in \a
    /// inf, is there any chance that we will satisfy the condition?
    ///
    /// This return false only when it is sure that visiting more
    /// set will never make the condition satisfiable.
    bool inf_satisfiable(mark_t inf) const
    {
      return code_.inf_satisfiable(inf);
    }

    /// \brief Check potential acceptance of an SCC.
    ///
    /// Assuming that an SCC intersects all sets in \a
    /// infinitely_often (i.e., for each set in \a infinitely_often,
    /// there exist one marked transition in the SCC), and is
    /// included in all sets in \a always_present (i.e., all
    /// transitions are marked with \a always_present), this returns
    /// one tree possible results:
    /// - trival::yes() the SCC is necessarily accepting,
    /// - trival::no() the SCC is necessarily rejecting,
    /// - trival::maybe() the SCC could contain an accepting cycle.
    trival maybe_accepting(mark_t infinitely_often, mark_t always_present) const
    {
      return code_.maybe_accepting(infinitely_often, always_present);
    }

    /// \brief Return an accepting subset of \a inf
    ///
    /// This function works only on Fin-less acceptance, and returns a
    /// subset of \a inf that is enough to satisfy the acceptance
    /// condition.  This is typically used when an accepting SCC that
    /// visits all sets in \a inf has been found, and we want to find
    /// an accepting cycle: maybe it is not necessary for the accepting
    /// cycle to intersect all sets in \a inf.
    ///
    /// This returns `mark_t({})` if \a inf does not satisfies the
    /// acceptance condition, or if the acceptance condition is `t`.
    /// So usually you should only use this method in cases you know
    /// that the condition is satisfied.
    mark_t accepting_sets(mark_t inf) const;

    // Deprecated since Spot 2.8
    SPOT_DEPRECATED("Use operator<< instead.")
    std::ostream& format(std::ostream& os, mark_t m) const
    {
      if (!m)
        return os;
      return os << m;
    }

    // Deprecated since Spot 2.8
    SPOT_DEPRECATED("Use operator<< or mark_t::as_string() instead.")
    std::string format(mark_t m) const
    {
      std::ostringstream os;
      if (m)
        os << m;
      return os.str();
    }

    /// \brief The number of sets used in the acceptance condition.
    unsigned num_sets() const
    {
      return num_;
    }

    /// \brief Compute useless acceptance sets given a list of mark_t
    /// that occur in an SCC.
    ///
    /// Assuming that the condition is generalized Büchi using all
    /// declared sets, this scans all the mark_t between \a begin and
    /// \a end, and return the set of acceptance sets that are useless
    /// because they are always implied by other sets.
    template<class iterator>
    mark_t useless(iterator begin, iterator end) const
    {
      mark_t u = {};        // The set of useless sets
      for (unsigned x = 0; x < num_; ++x)
        {
          // Skip sets that are already known to be useless.
          if (u.has(x))
            continue;
          auto all = comp(u | mark_t({x}));
          // Iterate over all mark_t, and keep track of
          // set numbers that always appear with x.
          for (iterator y = begin; y != end; ++y)
            {
              const mark_t& v = *y;
              if (v.has(x))
                {
                  all &= v;
                  if (!all)
                    break;
                }
            }
          u |= all;
        }
      return u;
    }

    /// \brief Remove all the acceptance sets in \a rem.
    ///
    /// If \a missing is set, the acceptance sets are assumed to be
    /// missing from the automaton, and the acceptance is updated to
    /// reflect this.  For instance `(Inf(1)&Inf(2))|Fin(3)` will
    /// become `Fin(3)` if we remove `2` because it is missing from this
    /// automaton.  Indeed there is no way to fulfill `Inf(1)&Inf(2)`
    /// in this case.  So essentially `missing=true` causes Inf(rem) to
    /// become `f`, and `Fin(rem)` to become `t`.
    ///
    /// If \a missing is unset, `Inf(rem)` become `t` while
    /// `Fin(rem)` become `f`.  Removing `2` from
    /// `(Inf(1)&Inf(2))|Fin(3)` would then give `Inf(1)|Fin(3)`.
    acc_cond remove(mark_t rem, bool missing) const
    {
      return {num_sets(), code_.remove(rem, missing)};
    }

    /// \brief Remove acceptance sets, and shift set numbers
    ///
    /// Same as remove, but also shift set numbers in the result so
    /// that all used set numbers are continuous.
    acc_cond strip(mark_t rem, bool missing) const
    {
      return
        { num_sets() - (all_sets() & rem).count(), code_.strip(rem, missing) };
    }

    /// \brief For all `x` in \a m, replaces `Fin(x)` by `false`.
    acc_cond force_inf(mark_t m) const
    {
      return {num_sets(), code_.force_inf(m)};
    }

    /// \brief Restrict an acceptance condition to a subset of set
    /// numbers that are occurring at some point.
    acc_cond restrict_to(mark_t rem) const
    {
      return {num_sets(), code_.remove(all_sets() - rem, true)};
    }

    /// \brief Return the name of this acceptance condition, in the
    /// specified format.
    ///
    /// The empty string is returned if no name is known.
    ///
    /// \a fmt should be a combination of the following letters.  (0)
    /// no parameters, (a) accentuated, (b) abbreviated, (d) style
    /// used in dot output, (g) no generalized parameter, (l)
    /// recognize Street-like and Rabin-like, (m) no main parameter,
    /// (p) no parity parameter, (o) name unknown acceptance as
    /// 'other', (s) shorthand for 'lo0'.
    std::string name(const char* fmt = "alo") const;

    /// \brief Find a `Fin(i)` that is a unit clause.
    ///
    /// This return a mark_t `{i}` such that `Fin(i)` appears as a
    /// unit clause in the acceptance condition.  I.e., either the
    /// condition is exactly `Fin(i)`, or the condition has the form
    /// `...&Fin(i)&...`.  If there is no such `Fin(i)`, an empty
    /// mark_t is returned.
    ///
    /// If multiple unit-Fin appear as unit-clauses, the set of
    /// those will be returned.  For instance applied to
    /// `Fin(0)&Fin(1)&(Inf(2)|Fin(3))``, this will return `{0,1}`.
    mark_t fin_unit() const
    {
      return code_.fin_unit();
    }

    /// \brief Return one acceptance set i that appear as `Fin(i)`
    /// in the condition.
    ///
    /// Return -1 if no such set exist.
    int fin_one() const
    {
      return code_.fin_one();
    }

    /// \brief Return the top-level disjuncts.
    ///
    /// For instance, if the formula is
    /// (5, Fin(0)|Fin(1)|(Fin(2)&(Inf(3)|Fin(4)))), this returns
    /// [(5, Fin(0)), (5, Fin(1)), (5, Fin(2)&(Inf(3)|Fin(4)))].
    ///
    /// If the formula is not a disjunction, this returns
    /// a vector with the formula as only element.
    std::vector<acc_cond> top_disjuncts() const;

    /// \brief Return the top-level conjuncts.
    ///
    /// For instance, if the formula is
    /// (5, Fin(0)|Fin(1)|(Fin(2)&(Inf(3)|Fin(4)))), this returns
    /// [(5, Fin(0)), (5, Fin(1)), (5, Fin(2)&(Inf(3)|Fin(4)))].
    ///
    /// If the formula is not a conjunction, this returns
    /// a vector with the formula as only element.
    std::vector<acc_cond> top_conjuncts() const;

  protected:
    mark_t all_sets_() const
    {
      return mark_t::all() >> (spot::acc_cond::mark_t::max_accsets() - num_);
    }

    unsigned num_;
    mark_t all_;
    acc_code code_;
    bool uses_fin_acceptance_ = false;

  };

  struct rs_pairs_view {
    typedef std::vector<acc_cond::rs_pair> rs_pairs;

    // Creates view of pairs 'p' with restriction only to marks in 'm'
    explicit rs_pairs_view(const rs_pairs& p, const acc_cond::mark_t& m)
      : pairs_(p), view_marks_(m) {}

    // Creates view of pairs without restriction to marks
    explicit rs_pairs_view(const rs_pairs& p)
      : rs_pairs_view(p, acc_cond::mark_t::all()) {}

    acc_cond::mark_t infs() const
    {
      return do_view([&](const acc_cond::rs_pair& p)
        {
          return visible(p.inf) ? p.inf : acc_cond::mark_t({});
        });
    }

    acc_cond::mark_t fins() const
    {
      return do_view([&](const acc_cond::rs_pair& p)
        {
          return visible(p.fin) ? p.fin : acc_cond::mark_t({});
        });
    }

    acc_cond::mark_t fins_alone() const
    {
      return do_view([&](const acc_cond::rs_pair& p)
        {
          return !visible(p.inf) && visible(p.fin) ? p.fin
                                                   : acc_cond::mark_t({});
        });
    }

    acc_cond::mark_t infs_alone() const
    {
      return do_view([&](const acc_cond::rs_pair& p)
        {
          return !visible(p.fin) && visible(p.inf) ? p.inf
                                                   : acc_cond::mark_t({});
        });
    }

    acc_cond::mark_t paired_with_fin(unsigned mark) const
    {
      acc_cond::mark_t res = {};
      for (const auto& p: pairs_)
        if (p.fin.has(mark) && visible(p.fin) && visible(p.inf))
          res |= p.inf;
      return res;
    }

    const rs_pairs& pairs() const
    {
      return pairs_;
    }

  private:
    template<typename filter>
    acc_cond::mark_t do_view(const filter& filt) const
    {
      acc_cond::mark_t res = {};
      for (const auto& p: pairs_)
        res |= filt(p);
      return res;
    }

    bool visible(const acc_cond::mark_t& v) const
    {
      return !!(view_marks_ & v);
    }

    const rs_pairs& pairs_;
    acc_cond::mark_t view_marks_;
  };


  SPOT_API
  std::ostream& operator<<(std::ostream& os, const acc_cond& acc);

  /// @}

  namespace internal
  {
    class SPOT_API mark_iterator
    {
    public:
      typedef unsigned value_type;
      typedef const value_type& reference;
      typedef const value_type* pointer;
      typedef std::ptrdiff_t difference_type;
      typedef std::forward_iterator_tag iterator_category;

      mark_iterator() noexcept
        : m_({})
      {
      }

      mark_iterator(acc_cond::mark_t m) noexcept
        : m_(m)
      {
      }

      bool operator==(mark_iterator m) const
      {
        return m_ == m.m_;
      }

      bool operator!=(mark_iterator m) const
      {
        return m_ != m.m_;
      }

      value_type operator*() const
      {
        SPOT_ASSERT(m_);
        return m_.min_set() - 1;
      }

      mark_iterator& operator++()
      {
        m_.clear(this->operator*());
        return *this;
      }

      mark_iterator operator++(int)
      {
        mark_iterator it = *this;
        ++(*this);
        return it;
      }
    private:
      acc_cond::mark_t m_;
    };

    class SPOT_API mark_container
    {
    public:
      mark_container(spot::acc_cond::mark_t m) noexcept
        : m_(m)
      {
      }

      mark_iterator begin() const
      {
        return {m_};
      }
      mark_iterator end() const
      {
        return {};
      }
    private:
      spot::acc_cond::mark_t m_;
    };
  }

  inline spot::internal::mark_container acc_cond::mark_t::sets() const
  {
    return {*this};
  }

  inline acc_cond::mark_t
  acc_cond::mark_t::apply_permutation(std::vector<unsigned> permut)
  {
    mark_t result { };
    for (auto color : sets())
      if (color < permut.size())
        result.set(permut[color]);
    return result;
  }
}

namespace std
{
  template<>
  struct hash<spot::acc_cond::mark_t>
  {
    size_t operator()(spot::acc_cond::mark_t m) const noexcept
    {
      return m.hash();
    }
  };
}
