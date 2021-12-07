// -*- coding: utf-8 -*-
// Copyright (C) 2015-2020 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
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

/// \file tl/formula.hh
/// \brief LTL/PSL formula interface
#pragma once

/// \defgroup tl Temporal Logic
///
/// Spot supports the future-time fragment of LTL, and the linear-time
/// fragment of and PSL formulas.  The former is included in the
/// latter.  Both types of formulas are represented by instances of
/// the spot::formula class.

/// \addtogroup tl_essentials Essential Temporal Logic Types
/// \ingroup tl

/// \addtogroup tl_io Input and Output of Formulas
/// \ingroup tl

/// \addtogroup tl_rewriting Rewriting Algorithms for Formulas
/// \ingroup tl

/// \addtogroup tl_hier Algorithms related to the temporal hierarchy
/// \ingroup tl

/// \addtogroup tl_misc Miscellaneous Algorithms for Formulas
/// \ingroup tl

#include <spot/misc/common.hh>
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <cassert>
#include <vector>
#include <string>
#include <iterator>
#include <iosfwd>
#include <sstream>
#include <list>
#include <cstddef>
#include <limits>

// The strong_X operator was introduced in Spot 2.8.2 to fix an issue
// with from_ltlf().  As adding a new operator is a backward
// incompatibility, causing new warnings from the compiler.
#if defined(SPOT_BUILD) or defined(SPOT_USES_STRONG_X)
// Use #if SPOT_HAS_STRONG_X in code that need to be backward
// compatible with older Spot versions.
#  define SPOT_HAS_STRONG_X 1
// You me #define SPOT_WANT_STRONG_X yourself before including
// this file to force the use of STRONG_X
#  define SPOT_WANT_STRONG_X 1
#endif

namespace spot
{


  /// \ingroup tl_essentials
  /// \brief Operator types
  enum class op: uint8_t
  {
    ff,                        ///< False
    tt,                        ///< True
    eword,                     ///< Empty word
    ap,                        ///< Atomic proposition
    // unary operators
    Not,                       ///< Negation
    X,                         ///< Next
    F,                         ///< Eventually
    G,                         ///< Globally
    Closure,                   ///< PSL Closure
    NegClosure,                ///< Negated PSL Closure
    NegClosureMarked,          ///< marked version of the Negated PSL Closure
    // binary operators
    Xor,                       ///< Exclusive Or
    Implies,                   ///< Implication
    Equiv,                     ///< Equivalence
    U,                         ///< until
    R,                         ///< release (dual of until)
    W,                         ///< weak until
    M,                         ///< strong release (dual of weak until)
    EConcat,                   ///< Seq
    EConcatMarked,             ///< Seq, Marked
    UConcat,                   ///< Triggers
    // n-ary operators
    Or,                        ///< (omega-Rational) Or
    OrRat,                     ///< Rational Or
    And,                       ///< (omega-Rational) And
    AndRat,                    ///< Rational And
    AndNLM,                    ///< Non-Length-Matching Rational-And
    Concat,                    ///< Concatenation
    Fusion,                    ///< Fusion
    // star-like operators
    Star,                      ///< Star
    FStar,                     ///< Fustion Star
    first_match,               ///< first_match(sere)
#ifdef SPOT_WANT_STRONG_X
    strong_X,                  ///< strong Next
#endif
  };

#ifndef SWIG
  /// \brief Actual storage for formula nodes.
  ///
  /// spot::formula objects contain references to instances of this
  /// class, and delegate most of their methods.  Because
  /// spot::formula is meant to be the public interface, most of the
  /// methods are documented there, rather than here.
  class SPOT_API fnode final
  {
    public:
      /// \brief Clone an fnode.
      ///
      /// This simply increment the reference counter.  If the counter
      /// saturates, the fnode will stay permanently allocated.
      const fnode* clone() const
      {
        // Saturate.
        ++refs_;
        if (SPOT_UNLIKELY(!refs_))
          saturated_ = 1;
        return this;
      }

      /// \brief Dereference an fnode.
      ///
      /// This decrement the reference counter (unless the counter is
      /// saturated), and actually deallocate the fnode when the
      /// counder reaches 0 (unless the fnode denotes a constant).
      void destroy() const
      {
        if (SPOT_LIKELY(refs_))
          --refs_;
        else if (SPOT_LIKELY(id_ > 2) && SPOT_LIKELY(!saturated_))
          // last reference to a node that is not a constant
          destroy_aux();
      }

      /// \see formula::unbounded
      static constexpr uint8_t unbounded()
      {
        return UINT8_MAX;
      }

      /// \see formula::ap
      static const fnode* ap(const std::string& name);
      /// \see formula::unop
      static const fnode* unop(op o, const fnode* f);
      /// \see formula::binop
      static const fnode* binop(op o, const fnode* f, const fnode* g);
      /// \see formula::multop
      static const fnode* multop(op o, std::vector<const fnode*> l);
      /// \see formula::bunop
      static const fnode* bunop(op o, const fnode* f,
          uint8_t min, uint8_t max = unbounded());

      /// \see formula::nested_unop_range
      static const fnode* nested_unop_range(op uo, op bo, unsigned min,
                                            unsigned max, const fnode* f);

      /// \see formula::kind
      op kind() const
      {
        return op_;
      }

      /// \see formula::kindstr
      std::string kindstr() const;

      /// \see formula::is
      /// @{
      bool is(op o) const
      {
        return op_ == o;
      }

      bool is(op o1, op o2) const
      {
        return op_ == o1 || op_ == o2;
      }

      bool is(op o1, op o2, op o3) const
      {
        return op_ == o1 || op_ == o2 || op_ == o3;
      }

      bool is(op o1, op o2, op o3, op o4) const
      {
        return op_ == o1 || op_ == o2 || op_ == o3 || op_ == o4;
      }

      bool is(std::initializer_list<op> l) const
      {
        const fnode* n = this;
        for (auto o: l)
        {
          if (!n->is(o))
            return false;
          n = n->nth(0);
        }
        return true;
      }
      /// @}

      /// \see formula::get_child_of
      const fnode* get_child_of(op o) const
      {
        if (op_ != o)
          return nullptr;
        if (SPOT_UNLIKELY(size_ != 1))
          report_get_child_of_expecting_single_child_node();
        return nth(0);
      }

      /// \see formula::get_child_of
      const fnode* get_child_of(std::initializer_list<op> l) const
      {
        auto c = this;
        for (auto o: l)
        {
          c = c->get_child_of(o);
          if (c == nullptr)
            return c;
        }
        return c;
      }

      /// \see formula::min
      unsigned min() const
      {
        if (SPOT_UNLIKELY(op_ != op::FStar && op_ != op::Star))
          report_min_invalid_arg();
        return min_;
      }

      /// \see formula::max
      unsigned max() const
      {
        if (SPOT_UNLIKELY(op_ != op::FStar && op_ != op::Star))
          report_max_invalid_arg();
        return max_;
      }

      /// \see formula::size
      unsigned size() const
      {
        return size_;
      }

      /// \see formula::is_leaf
      bool is_leaf() const
      {
        return size_ == 0;
      }

      /// \see formula::id
      size_t id() const
      {
        return id_;
      }

      /// \see formula::begin
      const fnode*const* begin() const
      {
        return children;
      }

      /// \see formula::end
      const fnode*const* end() const
      {
        return children + size();
      }

      /// \see formula::nth
      const fnode* nth(unsigned i) const
      {
        if (SPOT_UNLIKELY(i >= size()))
          report_non_existing_child();
        return children[i];
      }

      /// \see formula::ff
      static const fnode* ff()
      {
        return ff_;
      }

      /// \see formula::is_ff
      bool is_ff() const
      {
        return op_ == op::ff;
      }

      /// \see formula::tt
      static const fnode* tt()
      {
        return tt_;
      }

      /// \see formula::is_tt
      bool is_tt() const
      {
        return op_ == op::tt;
      }

      /// \see formula::eword
      static const fnode* eword()
      {
        return ew_;
      }

      /// \see formula::is_eword
      bool is_eword() const
      {
        return op_ == op::eword;
      }

      /// \see formula::is_constant
      bool is_constant() const
      {
        return op_ == op::ff || op_ == op::tt || op_ == op::eword;
      }

      /// \see formula::is_Kleene_star
      bool is_Kleene_star() const
      {
        if (op_ != op::Star)
          return false;
        return min_ == 0 && max_ == unbounded();
      }

      /// \see formula::one_star
      static const fnode* one_star()
      {
        if (!one_star_)
          one_star_ = bunop(op::Star, tt(), 0);
        return one_star_;
      }

      /// \see formula::ap_name
      const std::string& ap_name() const;

      /// \see formula::dump
      std::ostream& dump(std::ostream& os) const;

      /// \see formula::all_but
      const fnode* all_but(unsigned i) const;

      /// \see formula::boolean_count
      unsigned boolean_count() const
      {
        unsigned pos = 0;
        unsigned s = size();
        while (pos < s && children[pos]->is_boolean())
          ++pos;
        return pos;
      }

      /// \see formula::boolean_operands
      const fnode* boolean_operands(unsigned* width = nullptr) const;

      /// \brief safety check for the reference counters
      ///
      /// \return true iff the unicity map contains only the globally
      /// pre-allocated formulas.
      ///
      /// This is meant to be used as
      /// \code
      /// assert(spot::fnode::instances_check());
      /// \endcode
      /// at the end of a program.
      static bool instances_check();

      ////////////////
      // Properties //
      ////////////////

      /// \see formula::is_boolean
      bool is_boolean() const
      {
        return is_.boolean;
      }

      /// \see formula::is_sugar_free_boolean
      bool is_sugar_free_boolean() const
      {
        return is_.sugar_free_boolean;
      }

      /// \see formula::is_in_nenoform
      bool is_in_nenoform() const
      {
        return is_.in_nenoform;
      }

      /// \see formula::is_syntactic_stutter_invariant
      bool is_syntactic_stutter_invariant() const
      {
        return is_.syntactic_si;
      }

      /// \see formula::is_sugar_free_ltl
      bool is_sugar_free_ltl() const
      {
        return is_.sugar_free_ltl;
      }

      /// \see formula::is_ltl_formula
      bool is_ltl_formula() const
      {
        return is_.ltl_formula;
      }

      /// \see formula::is_psl_formula
      bool is_psl_formula() const
      {
        return is_.psl_formula;
      }

      /// \see formula::is_sere_formula
      bool is_sere_formula() const
      {
        return is_.sere_formula;
      }

      /// \see formula::is_finite
      bool is_finite() const
      {
        return is_.finite;
      }

      /// \see formula::is_eventual
      bool is_eventual() const
      {
        return is_.eventual;
      }

      /// \see formula::is_universal
      bool is_universal() const
      {
        return is_.universal;
      }

      /// \see formula::is_syntactic_safety
      bool is_syntactic_safety() const
      {
        return is_.syntactic_safety;
      }

      /// \see formula::is_syntactic_guarantee
      bool is_syntactic_guarantee() const
      {
        return is_.syntactic_guarantee;
      }

      /// \see formula::is_syntactic_obligation
      bool is_syntactic_obligation() const
      {
        return is_.syntactic_obligation;
      }

      /// \see formula::is_syntactic_recurrence
      bool is_syntactic_recurrence() const
      {
        return is_.syntactic_recurrence;
      }

      /// \see formula::is_syntactic_persistence
      bool is_syntactic_persistence() const
      {
        return is_.syntactic_persistence;
      }

      /// \see formula::is_marked
      bool is_marked() const
      {
        return !is_.not_marked;
      }

      /// \see formula::accepts_eword
      bool accepts_eword() const
      {
        return is_.accepting_eword;
      }

      /// \see formula::has_lbt_atomic_props
      bool has_lbt_atomic_props() const
      {
        return is_.lbt_atomic_props;
      }

      /// \see formula::has_spin_atomic_props
      bool has_spin_atomic_props() const
      {
        return is_.spin_atomic_props;
      }

    private:
      static size_t bump_next_id();
      void setup_props(op o);
      void destroy_aux() const;

      [[noreturn]] static void report_non_existing_child();
      [[noreturn]] static void report_too_many_children();
      [[noreturn]] static void
        report_get_child_of_expecting_single_child_node();
      [[noreturn]] static void report_min_invalid_arg();
      [[noreturn]] static void report_max_invalid_arg();

      static const fnode* unique(fnode*);

      // Destruction may only happen via destroy().
      ~fnode() = default;
      // Disallow copies.
      fnode(const fnode&) = delete;
      fnode& operator=(const fnode&) = delete;



      template<class iter>
      fnode(op o, iter begin, iter end)
        // Clang has some optimization where is it able to combine the
        // 4 movb initializing op_,min_,max_,saturated_ into a single
        // movl.  Also it can optimize the three byte-comparisons of
        // is_Kleene_star() into a single masked 32-bit comparison.
        // The latter optimization triggers warnings from valgrind if
        // min_ and max_ are not initialized.  So to benefit from the
        // initialization optimization and the is_Kleene_star()
        // optimization in Clang, we always initialize min_ and max_
        // with this compiler.  Do not do it the rest of the time,
        // since the optimization is not done.
        : op_(o),
#if __llvm__
         min_(0), max_(0),
#endif
         saturated_(0)
      {
        size_t s = std::distance(begin, end);
        if (SPOT_UNLIKELY(s > (size_t) UINT16_MAX))
          report_too_many_children();
        size_ = s;
        auto pos = children;
        for (auto i = begin; i != end; ++i)
          *pos++ = *i;
        setup_props(o);
      }

      fnode(op o, std::initializer_list<const fnode*> l)
        : fnode(o, l.begin(), l.end())
      {
      }

      fnode(op o, const fnode* f, uint8_t min, uint8_t max)
        : op_(o), min_(min), max_(max), saturated_(0), size_(1)
      {
        children[0] = f;
        setup_props(o);
      }

      static const fnode* ff_;
      static const fnode* tt_;
      static const fnode* ew_;
      static const fnode* one_star_;

      op op_;                      // operator
      uint8_t min_;                // range minimum (for star-like operators)
      uint8_t max_;                // range maximum;
      mutable uint8_t saturated_;
      uint16_t size_;              // number of children
      mutable uint16_t refs_ = 0;  // reference count - 1;
      size_t id_;                  // Also used as hash.
      static size_t next_id_;

      struct ltl_prop
      {
        // All properties here should be expressed in such a a way
        // that property(f && g) is just property(f)&property(g).
        // This allows us to compute all properties of a compound
        // formula in one operation.
        //
        // For instance we do not use a property that says "has
        // temporal operator", because it would require an OR between
        // the two arguments.  Instead we have a property that
        // says "no temporal operator", and that one is computed
        // with an AND between the arguments.
        //
        // Also choose a name that makes sense when prefixed with
        // "the formula is".
        bool boolean:1;                // No temporal operators.
        bool sugar_free_boolean:1;     // Only AND, OR, and NOT operators.
        bool in_nenoform:1;            // Negative Normal Form.
        bool syntactic_si:1;           // LTL-X or siPSL
        bool sugar_free_ltl:1;         // No F and G operators.
        bool ltl_formula:1;            // Only LTL operators.
        bool psl_formula:1;            // Only PSL operators.
        bool sere_formula:1;           // Only SERE operators.
        bool finite:1;                 // Finite SERE formulae, or Bool+X forms.
        bool eventual:1;               // Purely eventual formula.
        bool universal:1;              // Purely universal formula.
        bool syntactic_safety:1;       // Syntactic Safety Property.
        bool syntactic_guarantee:1;    // Syntactic Guarantee Property.
        bool syntactic_obligation:1;   // Syntactic Obligation Property.
        bool syntactic_recurrence:1;   // Syntactic Recurrence Property.
        bool syntactic_persistence:1;  // Syntactic Persistence Property.
        bool not_marked:1;             // No occurrence of EConcatMarked.
        bool accepting_eword:1;        // Accepts the empty word.
        bool lbt_atomic_props:1;       // Use only atomic propositions like p42.
        bool spin_atomic_props:1;      // Use only spin-compatible atomic props.
      };
      union
      {
        // Use an unsigned for fast computation of all properties.
        unsigned props;
        ltl_prop is_;
      };

      const fnode* children[1];
  };

  /// Order two atomic propositions.
  SPOT_API
    int atomic_prop_cmp(const fnode* f, const fnode* g);

  struct formula_ptr_less_than_bool_first
  {
    bool
      operator()(const fnode* left, const fnode* right) const
      {
        SPOT_ASSERT(left);
        SPOT_ASSERT(right);
        if (left == right)
          return false;

        // We want Boolean formulae first.
        bool lib = left->is_boolean();
        if (lib != right->is_boolean())
          return lib;

        // We have two Boolean formulae
        if (lib)
        {
          bool lconst = left->is_constant();
          if (lconst != right->is_constant())
            return lconst;
          if (!lconst)
          {
            auto get_literal = [](const fnode* f) -> const fnode*
            {
              if (f->is(op::Not))
                f = f->nth(0);
              if (f->is(op::ap))
                return f;
              return nullptr;
            };
            // Literals should come first
            const fnode* litl = get_literal(left);
            const fnode* litr = get_literal(right);
            if (!litl != !litr)
              return litl;
            if (litl)
            {
              // And they should be sorted alphabetically
              int cmp = atomic_prop_cmp(litl, litr);
              if (cmp)
                return cmp < 0;
            }
          }
        }

        size_t l = left->id();
        size_t r = right->id();
        if (l != r)
          return l < r;
        // Because the hash code assigned to each formula is the
        // number of formulae constructed so far, it is very unlikely
        // that we will ever reach a case were two different formulae
        // have the same hash.  This will happen only ever with have
        // produced 256**sizeof(size_t) formulae (i.e. max_count has
        // looped back to 0 and started over).  In that case we can
        // order two formulas by looking at their text representation.
        // We could be more efficient and look at their AST, but it's
        // not worth the burden.  (Also ordering pointers is ruled out
        // because it breaks the determinism of the implementation.)
        std::ostringstream old;
        left->dump(old);
        std::ostringstream ord;
        right->dump(ord);
        return old.str() < ord.str();
      }
  };

#endif // SWIG

  /// \ingroup tl_essentials
  /// \brief Main class for temporal logic formula
  class SPOT_API formula final
  {
    const fnode* ptr_;
    public:
    /// \brief Create a formula from an fnode.
    ///
    /// This constructor is mainly for internal use, as spot::fnode
    /// object should usually not be manipulated from user code.
    explicit formula(const fnode* f) noexcept
      : ptr_(f)
      {
      }

    /// \brief Create a null formula.
    ///
    /// This could be used to default-initialize a formula, however
    /// null formula should be short lived: most algorithms and member
    /// functions assume that formulas should not be null.
    formula(std::nullptr_t) noexcept
      : ptr_(nullptr)
      {
      }

    /// \brief Default initialize a formula to nullptr.
    formula() noexcept
      : ptr_(nullptr)
      {
      }

    /// Clone a formula.
    formula(const formula& f) noexcept
      : ptr_(f.ptr_)
      {
        if (ptr_)
          ptr_->clone();
      }

    /// Move-construct a formula.
    formula(formula&& f) noexcept
      : ptr_(f.ptr_)
      {
        f.ptr_ = nullptr;
      }

    /// Destroy a formula.
    ~formula()
    {
      if (ptr_)
        ptr_->destroy();
    }

    /// \brief Reset a formula to null
    ///
    /// Note that null formula should be short lived: most algorithms
    /// and member function assume that formulas should not be null.
    /// Assigning nullptr to a formula can be useful when cleaning an
    /// array of formula using multiple passes and marking some
    /// formula as nullptr before actually erasing them.
    const formula& operator=(std::nullptr_t)
    {
      this->~formula();
      ptr_ = nullptr;
      return *this;
    }

    const formula& operator=(const formula& f)
    {
      this->~formula();
      if ((ptr_ = f.ptr_))
        ptr_->clone();
      return *this;
    }

    const formula& operator=(formula&& f) noexcept
    {
      std::swap(f.ptr_, ptr_);
      return *this;
    }

    bool operator<(const formula& other) const noexcept
    {
      if (SPOT_UNLIKELY(!other.ptr_))
        return false;
      if (SPOT_UNLIKELY(!ptr_))
        return true;
      if (id() < other.id())
        return true;
      if (id() > other.id())
        return false;
      // The case where id()==other.id() but ptr_ != other.ptr_ is
      // very unlikely (we would need to build more than UINT_MAX
      // formulas), so let's just compare pointers, and ignore the
      // fact that it may introduce some nondeterminism.
      return ptr_ < other.ptr_;
    }

    bool operator<=(const formula& other) const noexcept
    {
      return *this == other || *this < other;
    }

    bool operator>(const formula& other) const noexcept
    {
      return !(*this <= other);
    }

    bool operator>=(const formula& other) const noexcept
    {
      return !(*this < other);
    }

    bool operator==(const formula& other) const noexcept
    {
      return other.ptr_ == ptr_;
    }

    bool operator==(std::nullptr_t) const noexcept
    {
      return ptr_ == nullptr;
    }

    bool operator!=(const formula& other) const noexcept
    {
      return other.ptr_ != ptr_;
    }

    bool operator!=(std::nullptr_t) const noexcept
    {
      return ptr_ != nullptr;
    }

    operator bool() const noexcept
    {
      return ptr_ != nullptr;
    }

    /////////////////////////
    // Forwarded functions //
    /////////////////////////

    /// Unbounded constant to use as end of range for bounded operators.
    static constexpr uint8_t unbounded()
    {
      return fnode::unbounded();
    }

    /// Build an atomic proposition.
    static formula ap(const std::string& name)
    {
      return formula(fnode::ap(name));
    }

    /// \brief Build an atomic proposition from... an atomic proposition.
    ///
    /// The only practical interest of this methods is for the Python
    /// bindings, where ap() can therefore work both from string or
    /// atomic propositions.
    static formula ap(const formula& a)
    {
      if (SPOT_UNLIKELY(a.kind() != op::ap))
        report_ap_invalid_arg();
      return a;
    }

    /// \brief Build a unary operator.
    /// \pre \a o should be one of op::Not, op::X, op::F, op::G,
    /// op::Closure, op::NegClosure, op::NegClosureMarked.
    /// @{
    static formula unop(op o, const formula& f)
    {
      return formula(fnode::unop(o, f.ptr_->clone()));
    }

#ifndef SWIG
    static formula unop(op o, formula&& f)
    {
      return formula(fnode::unop(o, f.to_node_()));
    }
#endif // !SWIG
    /// @}

#ifdef SWIG
#define SPOT_DEF_UNOP(Name)                          \
    static formula Name(const formula& f)            \
    {                                                \
      return unop(op::Name, f);                      \
    }
#else // !SWIG
#define SPOT_DEF_UNOP(Name)                          \
    static formula Name(const formula& f)            \
    {                                                \
      return unop(op::Name, f);                      \
    }                                                \
    static formula Name(formula&& f)                 \
    {                                                \
      return unop(op::Name, std::move(f));           \
    }
#endif // !SWIG
    /// \brief Construct a negation
    /// @{
    SPOT_DEF_UNOP(Not);
    /// @}

    /// \brief Construct an X
    /// @{
    SPOT_DEF_UNOP(X);
    /// @}

    /// \brief Construct an X[n]
    ///
    /// X[3]f = XXXf
    static formula X(unsigned level, const formula& f)
    {
      return nested_unop_range(op::X, op::Or /* unused */, level, level, f);
    }

#if SPOT_WANT_STRONG_X
    /// \brief Construct a strong_X
    /// @{
    SPOT_DEF_UNOP(strong_X);
    /// @}

    /// \brief Construct a strong_X[n]
    ///
    /// strong_X[3]f = strong_X strong_X strong_X f
    static formula strong_X(unsigned level, const formula& f)
    {
      return nested_unop_range(op::strong_X, op::Or /* unused */,
                               level, level, f);
    }
#endif

    /// \brief Construct an F
    /// @{
    SPOT_DEF_UNOP(F);
    /// @}

    /// \brief Construct F[n:m]
    ///
    /// F[2:3]a = XX(a | Xa)
    /// F[2:$]a = XXFa
    ///
    /// This syntax is from TSLF; the operator is called next_e![n..m] in PSL.
    static formula F(unsigned min_level, unsigned max_level, const formula& f)
    {
      return nested_unop_range(op::X, op::Or, min_level, max_level, f);
    }

    /// \brief Construct G[n:m]
    ///
    /// G[2:3]a = XX(a & Xa)
    /// G[2:$]a = XXGa
    ///
    /// This syntax is from TSLF; the operator is called next_a![n..m] in PSL.
    static formula G(unsigned min_level, unsigned max_level, const formula& f)
    {
      return nested_unop_range(op::X, op::And, min_level, max_level, f);
    }

    /// \brief Construct a G
    /// @{
    SPOT_DEF_UNOP(G);
    /// @}

    /// \brief Construct a PSL Closure
    /// @{
    SPOT_DEF_UNOP(Closure);
    /// @}

    /// \brief Construct a negated PSL Closure
    /// @{
    SPOT_DEF_UNOP(NegClosure);
    /// @}

    /// \brief Construct a marked negated PSL Closure
    /// @{
    SPOT_DEF_UNOP(NegClosureMarked);
    /// @}

    /// \brief Construct first_match(sere)
    /// @{
    SPOT_DEF_UNOP(first_match);
    /// @}
#undef SPOT_DEF_UNOP

    /// @{
    /// \brief Construct a binary operator
    /// \pre \a o should be one of op::Xor, op::Implies, op::Equiv,
    /// op::U, op::R, op::W, op::M, op::EConcat, op::EConcatMarked,
    /// or op::UConcat.
    static formula binop(op o, const formula& f, const formula& g)
    {
      return formula(fnode::binop(o, f.ptr_->clone(), g.ptr_->clone()));
    }

#ifndef SWIG
    static formula binop(op o, const formula& f, formula&& g)
    {
      return formula(fnode::binop(o, f.ptr_->clone(), g.to_node_()));
    }

    static formula binop(op o, formula&& f, const formula& g)
    {
      return formula(fnode::binop(o, f.to_node_(), g.ptr_->clone()));
    }

    static formula binop(op o, formula&& f, formula&& g)
    {
      return formula(fnode::binop(o, f.to_node_(), g.to_node_()));
    }
    ///@}

#endif //SWIG

#ifdef SWIG
#define SPOT_DEF_BINOP(Name)                                         \
    static formula Name(const formula& f, const formula& g)          \
    {                                                                \
      return binop(op::Name, f, g);                                  \
    }
#else // !SWIG
#define SPOT_DEF_BINOP(Name)                                         \
    static formula Name(const formula& f, const formula& g)          \
    {                                                                \
      return binop(op::Name, f, g);                                  \
    }                                                                \
    static formula Name(const formula& f, formula&& g)               \
    {                                                                \
      return binop(op::Name, f, std::move(g));                       \
    }                                                                \
    static formula Name(formula&& f, const formula& g)               \
    {                                                                \
      return binop(op::Name, std::move(f), g);                       \
    }                                                                \
    static formula Name(formula&& f, formula&& g)                    \
    {                                                                \
      return binop(op::Name, std::move(f), std::move(g));            \
    }
#endif // !SWIG
    /// \brief Construct an `Xor` formula
    /// @{
    SPOT_DEF_BINOP(Xor);
    /// @}

    /// \brief Construct an `->` formula
    /// @{
    SPOT_DEF_BINOP(Implies);
    /// @}

    /// \brief Construct an `<->` formula
    /// @{
    SPOT_DEF_BINOP(Equiv);
    /// @}

    /// \brief Construct a `U` formula
    /// @{
    SPOT_DEF_BINOP(U);
    /// @}

    /// \brief Construct an `R` formula
    /// @{
    SPOT_DEF_BINOP(R);
    /// @}

    /// \brief Construct a `W` formula
    /// @{
    SPOT_DEF_BINOP(W);
    /// @}

    /// \brief Construct an `M` formula
    /// @{
    SPOT_DEF_BINOP(M);
    /// @}

    /// \brief Construct a `<>->` PSL formula
    /// @{
    SPOT_DEF_BINOP(EConcat);
    /// @}

    /// \brief Construct a marked `<>->` PSL formula
    /// @{
    SPOT_DEF_BINOP(EConcatMarked);
    /// @}

    /// \brief Construct a `[]->` PSL formula
    /// @{
    SPOT_DEF_BINOP(UConcat);
    /// @}
#undef SPOT_DEF_BINOP

    /// \brief Construct an n-ary operator
    ///
    /// \pre \a o should be one of op::Or, op::OrRat, op::And,
    /// op::AndRat, op::AndNLM, op::Concat, op::Fusion.
    /// @{
    static formula multop(op o, const std::vector<formula>& l)
    {
      std::vector<const fnode*> tmp;
      tmp.reserve(l.size());
      for (auto f: l)
        if (f.ptr_)
          tmp.emplace_back(f.ptr_->clone());
      return formula(fnode::multop(o, std::move(tmp)));
    }

#ifndef SWIG
    static formula multop(op o, std::vector<formula>&& l)
    {
      std::vector<const fnode*> tmp;
      tmp.reserve(l.size());
      for (auto f: l)
        if (f.ptr_)
          tmp.emplace_back(f.to_node_());
      return formula(fnode::multop(o, std::move(tmp)));
    }
#endif // !SWIG
    /// @}

#ifdef SWIG
#define SPOT_DEF_MULTOP(Name)                                \
    static formula Name(const std::vector<formula>& l)       \
    {                                                        \
      return multop(op::Name, l);                            \
    }
#else // !SWIG
#define SPOT_DEF_MULTOP(Name)                                \
    static formula Name(const std::vector<formula>& l)       \
    {                                                        \
      return multop(op::Name, l);                            \
    }                                                        \
    \
    static formula Name(std::vector<formula>&& l)            \
    {                                                        \
      return multop(op::Name, std::move(l));                 \
    }
#endif // !SWIG
    /// \brief Construct an Or formula.
    /// @{
    SPOT_DEF_MULTOP(Or);
    /// @}

    /// \brief Construct an Or SERE.
    /// @{
    SPOT_DEF_MULTOP(OrRat);
    /// @}

    /// \brief Construct an And formula.
    /// @{
    SPOT_DEF_MULTOP(And);
    /// @}

    /// \brief Construct an And SERE.
    /// @{
    SPOT_DEF_MULTOP(AndRat);
    /// @}

    /// \brief Construct a non-length-matching And SERE.
    /// @{
    SPOT_DEF_MULTOP(AndNLM);
    /// @}

    /// \brief Construct a Concatenation SERE.
    /// @{
    SPOT_DEF_MULTOP(Concat);
    /// @}

    /// \brief Construct a Fusion SERE.
    /// @{
    SPOT_DEF_MULTOP(Fusion);
    /// @}
#undef SPOT_DEF_MULTOP

    /// \brief Define a bounded unary-operator (i.e. star-like)
    ///
    /// \pre \a o should be op::Star or op::FStar.
    /// @{
    static formula bunop(op o, const formula& f,
        uint8_t min = 0U,
        uint8_t max = unbounded())
    {
      return formula(fnode::bunop(o, f.ptr_->clone(), min, max));
    }

#ifndef SWIG
    static formula bunop(op o, formula&& f,
        uint8_t min = 0U,
        uint8_t max = unbounded())
    {
      return formula(fnode::bunop(o, f.to_node_(), min, max));
    }
#endif // !SWIG
    ///@}

#if SWIG
#define SPOT_DEF_BUNOP(Name)                                \
    static formula Name(const formula& f,                   \
        uint8_t min = 0U,                                   \
        uint8_t max = unbounded())                          \
    {                                                       \
      return bunop(op::Name, f, min, max);                  \
    }
#else // !SWIG
#define SPOT_DEF_BUNOP(Name)                                \
    static formula Name(const formula& f,                   \
        uint8_t min = 0U,                                   \
        uint8_t max = unbounded())                          \
    {                                                       \
      return bunop(op::Name, f, min, max);                  \
    }                                                       \
    static formula Name(formula&& f,                        \
        uint8_t min = 0U,                                   \
        uint8_t max = unbounded())                          \
    {                                                       \
      return bunop(op::Name, std::move(f), min, max);       \
    }
#endif
    /// \brief Create SERE for f[*min..max]
    /// @{
    SPOT_DEF_BUNOP(Star);
    /// @}

    /// \brief Create SERE for f[:*min..max]
    ///
    /// This operator is a generalization of the (+) operator
    /// defined by Dax et al. \cite dax.09.atva
    /// @{
    SPOT_DEF_BUNOP(FStar);
    /// @}
#undef SPOT_DEF_BUNOP

    /// \brief Nested operator construction (syntactic sugar).
    ///
    /// Build between min and max nested uo, and chose between the
    /// different numbers with bo.
    ///
    /// For instance nested_unup_range(op::X, op::Or, 2, 4, a) returns
    /// XX(a | X(a | Xa)).
    ///
    /// For `max==unbounded()`, \a uo is repeated \a min times, and
    /// its child is set to `F(a)` if \a bo is `op::Or` or to `G(a)`
    /// otherwise.
    static const formula nested_unop_range(op uo, op bo, unsigned min,
                                           unsigned max, formula f)
    {
      return formula(fnode::nested_unop_range(uo, bo, min, max,
                                              f.ptr_->clone()));
    }

    /// \brief Create a SERE equivalent to b[->min..max]
    ///
    /// The operator does not exist: it is handled as syntactic sugar
    /// by the parser and the printer.  This function is used by the
    /// parser to create the equivalent SERE.
    static formula sugar_goto(const formula& b, uint8_t min, uint8_t max);

    /// Create the SERE b[=min..max]
    ///
    /// The operator does not exist: it is handled as syntactic sugar
    /// by the parser and the printer.  This function is used by the
    /// parser to create the equivalent SERE.
    static formula sugar_equal(const formula& b, uint8_t min, uint8_t max);

    /// Create the SERE a ##[n:m] b
    ///
    /// This ##[n:m] operator comes from SVA.  When n=m, it is simply
    /// written ##n.
    ///
    /// The operator does not exist in Spot it is handled as syntactic
    /// sugar by the parser.  This function is used by the parser to
    /// create the equivalent SERE using PSL operators.
    ///
    /// The rewriting rules depends on the values of a, n, and b.
    /// If n≥1 `a ##[n:m] b` is encoded as `a;1[*n-1,m-1];b`.
    /// Otherwise:
    /// * `a ##[0:0] b` is encoded as `a:b`,
    /// * For m>0, `a ##[0:m] b` is encoded as
    ///   - `a:(1[*0:m];b)` is `a` rejects `[*0]`,
    ///   - `(a;1[*0:m]):b` is `b` rejects `[*0]`,
    ///   - `(a:b) | (a;1[*0:m-1];b)` is `a` and `b` accept `[*0]`.
    ///
    /// The left operand can also be missing, in which case
    /// `##[n:m] b` is rewritten as `1[*n:m];b`.
    /// @{
    static formula sugar_delay(const formula& a, const formula& b,
                               unsigned min, unsigned max);
    static formula sugar_delay(const formula& b,
                               unsigned min, unsigned max);
    /// @}

#ifndef SWIG
    /// \brief Return the underlying pointer to the formula.
    ///
    /// It is not recommended to call this function, which is
    /// mostly meant for internal use.
    ///
    /// By calling this function you take ownership of the fnode
    /// instance pointed by this formula instance, and should take
    /// care of calling its destroy() methods once you are done with
    /// it.  Otherwise the fnode will be leaked.
    const fnode* to_node_()
    {
      auto tmp = ptr_;
      ptr_ = nullptr;
      return tmp;
    }
#endif

    /// Return top-most operator.
    op kind() const
    {
      return ptr_->kind();
    }

    /// Return the name of the top-most operator.
    std::string kindstr() const
    {
      return ptr_->kindstr();
    }

    /// Return true if the formula is of kind \a o.
    bool is(op o) const
    {
      return ptr_->is(o);
    }

#ifndef SWIG
    /// Return true if the formula is of kind \a o1 or \a o2.
    bool is(op o1, op o2) const
    {
      return ptr_->is(o1, o2);
    }

    /// Return true if the formula is of kind \a o1 or \a o2 or \a o3
    bool is(op o1, op o2, op o3) const
    {
      return ptr_->is(o1, o2, o3);
    }

    /// Return true if the formula is of kind \a o1 or \a o2 or \a o3
    /// or \a a4.
    bool is(op o1, op o2, op o3, op o4) const
    {
      return ptr_->is(o1, o2, o3, o4);
    }

    /// Return true if the formulas nests all the operators in \a l.
    bool is(std::initializer_list<op> l) const
    {
      return ptr_->is(l);
    }
#endif

    /// \brief Remove operator \a o and return the child.
    ///
    /// This works only for unary operators.
    formula get_child_of(op o) const
    {
      auto f = ptr_->get_child_of(o);
      if (f)
        f->clone();
      return formula(f);
    }

#ifndef SWIG
    /// \brief Remove all operators in \a l and return the child.
    ///
    /// This works only for a list of unary operators.
    /// For instance if \c f  is a formula for XG(a U b),
    /// then <code>f.get_child_of({op::X, op::G})</code>
    /// will return the subformula a U b.
    formula get_child_of(std::initializer_list<op> l) const
    {
      auto f = ptr_->get_child_of(l);
      if (f)
        f->clone();
      return formula(f);
    }
#endif

    /// \brief Return start of the range for star-like operators.
    ///
    /// \pre The formula should have kind op::Star or op::FStar.
    unsigned min() const
    {
      return ptr_->min();
    }

    /// \brief Return end of the range for star-like operators.
    ///
    /// \pre The formula should have kind op::Star or op::FStar.
    unsigned max() const
    {
      return ptr_->max();
    }

    /// Return the number of children.
    unsigned size() const
    {
      return ptr_->size();
    }

    /// \brief Whether the formula is a leaf.
    ///
    /// Leaves are formulas without children.  They are either
    /// constants (true, false, empty word) or atomic propositions.
    bool is_leaf() const
    {
      return ptr_->is_leaf();
    }

    /// \brief Return the id of a formula.
    ///
    /// Can be used as a hash number.
    ///
    /// The id is almost unique as it is an unsigned number
    /// incremented for each formula construction, and the number may
    /// wrap around zero.  If this is used for ordering, make sure to
    /// deal with equality
    size_t id() const
    {
      return ptr_->id();
    }

#ifndef SWIG
    /// Allow iterating over children
    class SPOT_API formula_child_iterator final
    {
      const fnode*const* ptr_;
    public:
      formula_child_iterator()
        : ptr_(nullptr)
      {
      }

      formula_child_iterator(const fnode*const* f)
        : ptr_(f)
      {
      }

      bool operator==(formula_child_iterator o)
      {
        return ptr_ == o.ptr_;
      }

      bool operator!=(formula_child_iterator o)
      {
        return ptr_ != o.ptr_;
      }

      formula operator*()
      {
        return formula((*ptr_)->clone());
      }

      formula_child_iterator operator++()
      {
        ++ptr_;
        return *this;
      }

      formula_child_iterator operator++(int)
      {
        auto tmp = *this;
        ++ptr_;
        return tmp;
      }
    };

    /// Allow iterating over children
    formula_child_iterator begin() const
    {
      return ptr_->begin();
    }

    /// Allow iterating over children
    formula_child_iterator end() const
    {
      return ptr_->end();
    }

    /// Return children number \a i
    formula operator[](unsigned i) const
    {
      return formula(ptr_->nth(i)->clone());
    }
#endif

    /// Return the false constant.
    static formula ff()
    {
      return formula(fnode::ff());
    }

    /// Whether the formula is the false constant.
    bool is_ff() const
    {
      return ptr_->is_ff();
    }

    /// Return the true constant.
    static formula tt()
    {
      return formula(fnode::tt());
    }

    /// Whether the formula is the true constant.
    bool is_tt() const
    {
      return ptr_->is_tt();
    }

    /// Return the empty word constant.
    static formula eword()
    {
      return formula(fnode::eword());
    }

    /// Whether the formula is the empty word constant.
    bool is_eword() const
    {
      return ptr_->is_eword();
    }

    /// Whether the formula is op::ff, op::tt, or op::eword.
    bool is_constant() const
    {
      return ptr_->is_constant();
    }

    /// \brief Test whether the formula represent a Kleene star
    ///
    /// That is, it should be of kind op::Star, with min=0 and
    /// max=unbounded().
    bool is_Kleene_star() const
    {
      return ptr_->is_Kleene_star();
    }

    /// \brief Return a copy of the formula 1[*].
    static formula one_star()
    {
      return formula(fnode::one_star()->clone());
    }

    /// \brief Whether the formula is an atomic proposition or its
    /// negation.
    bool is_literal()
    {
      return (is(op::ap) ||
          // If f is in nenoform, Not can only occur in front of
          // an atomic proposition.  So this way we do not have
          // to check the type of the child.
          (is(op::Not) && is_boolean() && is_in_nenoform()));
    }

    /// \brief Print the name of an atomic proposition.
    ///
    /// \pre the formula should be of kind op::ap.
    const std::string& ap_name() const
    {
      return ptr_->ap_name();
    }

    /// \brief Print the formula for debugging
    ///
    /// In addition to the operator and children, this also display
    /// the formula's unique id, and its reference count.
    std::ostream& dump(std::ostream& os) const
    {
      return ptr_->dump(os);
    }

    /// \brief clone this formula, omitting child \a i
    ///
    /// \pre The current node should be an n-ary operator such as
    /// op::And, op::AndRat, op::AndNLM, op::Or, op::OrRat,
    /// op::Concat, or op::Fusion.
    formula all_but(unsigned i) const
    {
      return formula(ptr_->all_but(i));
    }

    /// \brief number of Boolean children
    ///
    /// \pre The current node should be an n-ary operator such as
    /// op::And, op::AndRat, op::AndNLM, op::Or, or op::OrRat.
    ///
    /// Note that the children of an n-ary operator are always sorted
    /// when the node is constructed, and such that Boolean children
    /// appear at the beginning. This function therefore return the
    /// number of the first non-Boolean child if it exists.
    unsigned boolean_count() const
    {
      return ptr_->boolean_count();
    }

    /// \brief return a clone of the current node, restricted to its
    /// Boolean children
    ///
    /// \pre The current node should be an n-ary operator such as
    /// op::And, op::AndRat, op::AndNLM, op::Or, or op::OrRat.
    ///
    /// On a formula such as And({a,b,c,d,F(e),G(f)}), this returns
    /// And({a,b,c,d}).  If \a width is not nullptr, it is set the the
    /// number of Boolean children gathered.  Note that the children
    /// of an n-ary operator are always sorted when the node is
    /// constructed, and such that Boolean children appear at the
    /// beginning. \a width would therefore give the number of the
    /// first non-Boolean child if it exists.
    formula boolean_operands(unsigned* width = nullptr) const
    {
      return formula(ptr_->boolean_operands(width));
    }

#define SPOT_DEF_PROP(Name)                        \
    bool Name() const                              \
    {                                              \
      return ptr_->Name();                         \
    }
    ////////////////
    // Properties //
    ////////////////

    /// Whether the formula use only boolean operators.
    SPOT_DEF_PROP(is_boolean);
    /// Whether the formula use only AND, OR, and NOT operators.
    SPOT_DEF_PROP(is_sugar_free_boolean);
    /// \brief Whether the formula is in negative normal form.
    ///
    /// A formula is in negative normal form if the not operators
    /// occur only in front of atomic propositions.
    SPOT_DEF_PROP(is_in_nenoform);
    /// Whether the formula is syntactically stutter_invariant
    SPOT_DEF_PROP(is_syntactic_stutter_invariant);
    /// Whether the formula avoids the F and G operators.
    SPOT_DEF_PROP(is_sugar_free_ltl);
    /// Whether the formula uses only LTL operators.
    SPOT_DEF_PROP(is_ltl_formula);
    /// Whether the formula uses only PSL operators.
    SPOT_DEF_PROP(is_psl_formula);
    /// Whether the formula uses only SERE operators.
    SPOT_DEF_PROP(is_sere_formula);
    /// \brief Whether a SERE describes a finite language, or an LTL
    /// formula uses no temporal operator but X.
    SPOT_DEF_PROP(is_finite);
    /// \brief Whether the formula is purely eventual.
    ///
    /// Pure eventuality formulae are defined in
    ///
    /// A word that satisfies a pure eventuality can be prefixed by
    /// anything and still satisfies the formula.
    /// \cite etessami.00.concur
    SPOT_DEF_PROP(is_eventual);
    /// \brief Whether a formula is purely universal.
    ///
    /// Purely universal formulae are defined in
    ///
    /// Any (non-empty) suffix of a word that satisfies a purely
    /// universal formula also satisfies the formula.
    /// \cite etessami.00.concur
    SPOT_DEF_PROP(is_universal);
    /// Whether a PSL/LTL formula is syntactic safety property.
    SPOT_DEF_PROP(is_syntactic_safety);
    /// Whether a PSL/LTL formula is syntactic guarantee property.
    SPOT_DEF_PROP(is_syntactic_guarantee);
    /// Whether a PSL/LTL formula is syntactic obligation property.
    SPOT_DEF_PROP(is_syntactic_obligation);
    /// Whether a PSL/LTL formula is syntactic recurrence property.
    SPOT_DEF_PROP(is_syntactic_recurrence);
    /// Whether a PSL/LTL formula is syntactic persistence property.
    SPOT_DEF_PROP(is_syntactic_persistence);
    /// \brief Whether the formula has an occurrence of EConcatMarked
    /// or NegClosureMarked
    SPOT_DEF_PROP(is_marked);
    /// Whether the formula accepts [*0].
    SPOT_DEF_PROP(accepts_eword);
    /// \brief Whether the formula has only LBT-compatible atomic
    /// propositions.
    ///
    /// LBT only supports atomic propositions of the form p1, p12,
    /// etc.
    SPOT_DEF_PROP(has_lbt_atomic_props);
    /// \brief Whether the formula has spin-compatible atomic
    /// propositions.
    ///
    /// In Spin 5 (and hence ltl2ba, ltl3ba, ltl3dra), atomic
    /// propositions should start with a lowercase letter, and can
    /// then consist solely of alphanumeric characters and underscores.
    ///
    /// \see spot::is_spin_ap()
    SPOT_DEF_PROP(has_spin_atomic_props);
#undef SPOT_DEF_PROP

    /// \brief Clone this node after applying \a trans to its children.
    ///
    /// Any additional argument is passed to trans.
    template<typename Trans, typename... Args>
      formula map(Trans trans, Args&&... args)
      {
        switch (op o = kind())
        {
          case op::ff:
          case op::tt:
          case op::eword:
          case op::ap:
            return *this;
          case op::Not:
          case op::X:
#if SPOT_HAS_STRONG_X
          case op::strong_X:
#endif
          case op::F:
          case op::G:
          case op::Closure:
          case op::NegClosure:
          case op::NegClosureMarked:
          case op::first_match:
            return unop(o, trans((*this)[0], std::forward<Args>(args)...));
          case op::Xor:
          case op::Implies:
          case op::Equiv:
          case op::U:
          case op::R:
          case op::W:
          case op::M:
          case op::EConcat:
          case op::EConcatMarked:
          case op::UConcat:
            {
              formula tmp = trans((*this)[0], std::forward<Args>(args)...);
              return binop(o, tmp,
                           trans((*this)[1], std::forward<Args>(args)...));
            }
          case op::Or:
          case op::OrRat:
          case op::And:
          case op::AndRat:
          case op::AndNLM:
          case op::Concat:
          case op::Fusion:
            {
              std::vector<formula> tmp;
              tmp.reserve(size());
              for (auto f: *this)
                tmp.emplace_back(trans(f, std::forward<Args>(args)...));
              return multop(o, std::move(tmp));
            }
          case op::Star:
          case op::FStar:
            return bunop(o, trans((*this)[0], std::forward<Args>(args)...),
                         min(), max());
        }
        SPOT_UNREACHABLE();
      }

    /// \brief Apply \a func to each subformula.
    ///
    /// This does a simple DFS without checking for duplicate
    /// subformulas.  If \a func returns true, the children of the
    /// current node are skipped.
    ///
    /// Any additional argument is passed to \a func when it is
    /// invoked.
    template<typename Func, typename... Args>
      void traverse(Func func, Args&&... args)
      {
        if (func(*this, std::forward<Args>(args)...))
          return;
        for (auto f: *this)
          f.traverse(func, std::forward<Args>(args)...);
      }

  private:
#ifndef SWIG
    [[noreturn]] static void report_ap_invalid_arg();
#endif
  };

  /// Print the properties of formula \a f on stream \a out.
  SPOT_API
    std::ostream& print_formula_props(std::ostream& out, const formula& f,
        bool abbreviated = false);

  /// List the properties of formula \a f.
  SPOT_API
    std::list<std::string> list_formula_props(const formula& f);

  /// Print a formula.
  SPOT_API
    std::ostream& operator<<(std::ostream& os, const formula& f);
}

#ifndef SWIG
namespace std
{
  template <>
    struct hash<spot::formula>
    {
      size_t operator()(const spot::formula& x) const noexcept
      {
        return x.id();
      }
    };
}
#endif
