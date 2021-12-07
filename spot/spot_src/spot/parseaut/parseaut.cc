// A Bison parser, made by GNU Bison 3.3.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2019 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.


// Take the name prefix into account.
#define yylex   hoayylex



#include "parseaut.hh"


// Unqualified %code blocks.
#line 214 "parseaut.yy" // lalr1.cc:435

#include <sstream>

  /* parseaut.hh and parsedecl.hh include each other recursively.
   We must ensure that YYSTYPE is declared (by the above %union)
   before parsedecl.hh uses it. */
#include <spot/parseaut/parsedecl.hh>

  static void fill_guards(result_& res);

#line 58 "parseaut.cc" // lalr1.cc:435


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if HOAYYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !HOAYYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !HOAYYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace hoayy {
#line 153 "parseaut.cc" // lalr1.cc:510

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  parser::parser (void* scanner_yyarg, result_& res_yyarg, spot::location initial_loc_yyarg)
    :
#if HOAYYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg),
      res (res_yyarg),
      initial_loc (initial_loc_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | Symbol types.  |
  `---------------*/

  // basic_symbol.
#if 201103L <= YY_CPLUSPLUS
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (basic_symbol&& that)
    : Base (std::move (that))
    , value (std::move (that.value))
    , location (std::move (that.location))
  {}
#endif

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
    , location (that.location)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_MOVE_REF (location_type) l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (semantic_type) v, YY_RVREF (location_type) l)
    : Base (t)
    , value (YY_MOVE (v))
    , location (YY_MOVE (l))
  {}

  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
    location = YY_MOVE (s.location);
  }

  // by_type.
  parser::by_type::by_type ()
    : type (empty_symbol)
  {}

#if 201103L <= YY_CPLUSPLUS
  parser::by_type::by_type (by_type&& that)
    : type (that.type)
  {
    that.clear ();
  }
#endif

  parser::by_type::by_type (const by_type& that)
    : type (that.type)
  {}

  parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  void
  parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  void
  parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  int
  parser::by_type::type_get () const YY_NOEXCEPT
  {
    return type;
  }


  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_number_type
  parser::by_state::type_get () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value), YY_MOVE (that.location))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value), YY_MOVE (that.location))
  {
    // that is emptied.
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.type_get ())
    {
      case 18: // "identifier"
#line 302 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 388 "parseaut.cc" // lalr1.cc:652
        break;

      case 19: // "header name"
#line 302 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 394 "parseaut.cc" // lalr1.cc:652
        break;

      case 20: // "alias name"
#line 302 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 400 "parseaut.cc" // lalr1.cc:652
        break;

      case 21: // "string"
#line 302 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 406 "parseaut.cc" // lalr1.cc:652
        break;

      case 44: // "boolean formula"
#line 302 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 412 "parseaut.cc" // lalr1.cc:652
        break;

      case 69: // string_opt
#line 302 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 418 "parseaut.cc" // lalr1.cc:652
        break;

      case 90: // state-conj-2
#line 306 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.states); }
#line 424 "parseaut.cc" // lalr1.cc:652
        break;

      case 91: // init-state-conj-2
#line 306 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.states); }
#line 430 "parseaut.cc" // lalr1.cc:652
        break;

      case 92: // label-expr
#line 303 "parseaut.yy" // lalr1.cc:652
        { bdd_delref((yysym.value.b)); }
#line 436 "parseaut.cc" // lalr1.cc:652
        break;

      case 94: // acceptance-cond
#line 305 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.code); }
#line 442 "parseaut.cc" // lalr1.cc:652
        break;

      case 112: // state-conj-checked
#line 306 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.states); }
#line 448 "parseaut.cc" // lalr1.cc:652
        break;

      case 129: // nc-one-ident
#line 302 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 454 "parseaut.cc" // lalr1.cc:652
        break;

      case 130: // nc-ident-list
#line 302 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 460 "parseaut.cc" // lalr1.cc:652
        break;

      case 131: // nc-transition-block
#line 321 "parseaut.yy" // lalr1.cc:652
        {
  for (auto i = (yysym.value.list)->begin(); i != (yysym.value.list)->end(); ++i)
  {
    bdd_delref(i->first);
    delete i->second;
  }
  delete (yysym.value.list);
  }
#line 473 "parseaut.cc" // lalr1.cc:652
        break;

      case 133: // nc-transitions
#line 321 "parseaut.yy" // lalr1.cc:652
        {
  for (auto i = (yysym.value.list)->begin(); i != (yysym.value.list)->end(); ++i)
  {
    bdd_delref(i->first);
    delete i->second;
  }
  delete (yysym.value.list);
  }
#line 486 "parseaut.cc" // lalr1.cc:652
        break;

      case 134: // nc-formula-or-ident
#line 302 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 492 "parseaut.cc" // lalr1.cc:652
        break;

      case 135: // nc-formula
#line 303 "parseaut.yy" // lalr1.cc:652
        { bdd_delref((yysym.value.b)); }
#line 498 "parseaut.cc" // lalr1.cc:652
        break;

      case 136: // nc-opt-dest
#line 302 "parseaut.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 504 "parseaut.cc" // lalr1.cc:652
        break;

      case 137: // nc-src-dest
#line 304 "parseaut.yy" // lalr1.cc:652
        { bdd_delref((yysym.value.p)->first); delete (yysym.value.p)->second; delete (yysym.value.p); }
#line 510 "parseaut.cc" // lalr1.cc:652
        break;

      case 138: // nc-transition
#line 304 "parseaut.yy" // lalr1.cc:652
        { bdd_delref((yysym.value.p)->first); delete (yysym.value.p)->second; delete (yysym.value.p); }
#line 516 "parseaut.cc" // lalr1.cc:652
        break;

      default:
        break;
    }
  }

#if HOAYYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
#if defined __GNUC__ && ! defined __clang__ && ! defined __ICC && __GNUC__ * 100 + __GNUC_MINOR__ <= 408
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
#endif
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    switch (yytype)
    {
      case 18: // "identifier"
#line 329 "parseaut.yy" // lalr1.cc:676
        {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 551 "parseaut.cc" // lalr1.cc:676
        break;

      case 19: // "header name"
#line 329 "parseaut.yy" // lalr1.cc:676
        {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 561 "parseaut.cc" // lalr1.cc:676
        break;

      case 20: // "alias name"
#line 329 "parseaut.yy" // lalr1.cc:676
        {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 571 "parseaut.cc" // lalr1.cc:676
        break;

      case 21: // "string"
#line 329 "parseaut.yy" // lalr1.cc:676
        {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 581 "parseaut.cc" // lalr1.cc:676
        break;

      case 22: // "integer"
#line 334 "parseaut.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 587 "parseaut.cc" // lalr1.cc:676
        break;

      case 44: // "boolean formula"
#line 329 "parseaut.yy" // lalr1.cc:676
        {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 597 "parseaut.cc" // lalr1.cc:676
        break;

      case 47: // "LBTT header"
#line 334 "parseaut.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 603 "parseaut.cc" // lalr1.cc:676
        break;

      case 48: // "state acceptance"
#line 334 "parseaut.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 609 "parseaut.cc" // lalr1.cc:676
        break;

      case 49: // "acceptance sets for empty automaton"
#line 334 "parseaut.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 615 "parseaut.cc" // lalr1.cc:676
        break;

      case 50: // "acceptance set"
#line 334 "parseaut.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 621 "parseaut.cc" // lalr1.cc:676
        break;

      case 51: // "state number"
#line 334 "parseaut.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 627 "parseaut.cc" // lalr1.cc:676
        break;

      case 52: // "destination number"
#line 334 "parseaut.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 633 "parseaut.cc" // lalr1.cc:676
        break;

      case 69: // string_opt
#line 329 "parseaut.yy" // lalr1.cc:676
        {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 643 "parseaut.cc" // lalr1.cc:676
        break;

      case 90: // state-conj-2
#line 307 "parseaut.yy" // lalr1.cc:676
        {
  auto& os = debug_stream();
  os << '{';
  bool notfirst = false;
  for (auto i: *(yysym.value.states))
  {
    if (notfirst)
      os << ", ";
    else
      notfirst = true;
    os << i;
  }
  os << '}';
}
#line 662 "parseaut.cc" // lalr1.cc:676
        break;

      case 91: // init-state-conj-2
#line 307 "parseaut.yy" // lalr1.cc:676
        {
  auto& os = debug_stream();
  os << '{';
  bool notfirst = false;
  for (auto i: *(yysym.value.states))
  {
    if (notfirst)
      os << ", ";
    else
      notfirst = true;
    os << i;
  }
  os << '}';
}
#line 681 "parseaut.cc" // lalr1.cc:676
        break;

      case 93: // acc-set
#line 334 "parseaut.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 687 "parseaut.cc" // lalr1.cc:676
        break;

      case 96: // state-num
#line 334 "parseaut.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 693 "parseaut.cc" // lalr1.cc:676
        break;

      case 97: // checked-state-num
#line 334 "parseaut.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 699 "parseaut.cc" // lalr1.cc:676
        break;

      case 112: // state-conj-checked
#line 307 "parseaut.yy" // lalr1.cc:676
        {
  auto& os = debug_stream();
  os << '{';
  bool notfirst = false;
  for (auto i: *(yysym.value.states))
  {
    if (notfirst)
      os << ", ";
    else
      notfirst = true;
    os << i;
  }
  os << '}';
}
#line 718 "parseaut.cc" // lalr1.cc:676
        break;

      case 121: // sign
#line 334 "parseaut.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 724 "parseaut.cc" // lalr1.cc:676
        break;

      case 129: // nc-one-ident
#line 329 "parseaut.yy" // lalr1.cc:676
        {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 734 "parseaut.cc" // lalr1.cc:676
        break;

      case 130: // nc-ident-list
#line 329 "parseaut.yy" // lalr1.cc:676
        {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 744 "parseaut.cc" // lalr1.cc:676
        break;

      case 134: // nc-formula-or-ident
#line 329 "parseaut.yy" // lalr1.cc:676
        {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 754 "parseaut.cc" // lalr1.cc:676
        break;

      case 136: // nc-opt-dest
#line 329 "parseaut.yy" // lalr1.cc:676
        {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 764 "parseaut.cc" // lalr1.cc:676
        break;

      default:
        break;
    }
    yyo << ')';
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if HOAYYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // HOAYYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    // User initialization code.
#line 199 "parseaut.yy" // lalr1.cc:788
{ yyla.location = res.h->loc = initial_loc; }

#line 886 "parseaut.cc" // lalr1.cc:788

    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location, scanner, PARSE_ERROR_LIST));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2:
#line 337 "parseaut.yy" // lalr1.cc:919
    { res.h->loc = yylhs.location; YYACCEPT; }
#line 1010 "parseaut.cc" // lalr1.cc:919
    break;

  case 3:
#line 338 "parseaut.yy" // lalr1.cc:919
    { YYABORT; }
#line 1016 "parseaut.cc" // lalr1.cc:919
    break;

  case 4:
#line 339 "parseaut.yy" // lalr1.cc:919
    { YYABORT; }
#line 1022 "parseaut.cc" // lalr1.cc:919
    break;

  case 5:
#line 341 "parseaut.yy" // lalr1.cc:919
    {
       error(yystack_[1].location, "leading garbage was ignored");
       res.h->loc = yystack_[0].location;
       YYACCEPT;
     }
#line 1032 "parseaut.cc" // lalr1.cc:919
    break;

  case 6:
#line 347 "parseaut.yy" // lalr1.cc:919
    { res.h->type = spot::parsed_aut_type::HOA; }
#line 1038 "parseaut.cc" // lalr1.cc:919
    break;

  case 7:
#line 348 "parseaut.yy" // lalr1.cc:919
    { res.h->type = spot::parsed_aut_type::NeverClaim; }
#line 1044 "parseaut.cc" // lalr1.cc:919
    break;

  case 8:
#line 349 "parseaut.yy" // lalr1.cc:919
    { res.h->type = spot::parsed_aut_type::LBTT; }
#line 1050 "parseaut.cc" // lalr1.cc:919
    break;

  case 12:
#line 359 "parseaut.yy" // lalr1.cc:919
    { (yylhs.value.str) = nullptr; }
#line 1056 "parseaut.cc" // lalr1.cc:919
    break;

  case 13:
#line 360 "parseaut.yy" // lalr1.cc:919
    { (yylhs.value.str) = (yystack_[0].value.str); }
#line 1062 "parseaut.cc" // lalr1.cc:919
    break;

  case 16:
#line 364 "parseaut.yy" // lalr1.cc:919
    {
          bool v1plus = strverscmp("v1", res.format_version.c_str()) < 0;
	  // Preallocate the states if we know their number.
	  if (res.states >= 0)
	    {
	      unsigned states = res.states;
	      for (auto& p : res.start)
                for (unsigned s: p.second)
                  if ((unsigned) res.states <= s)
                    {
                      error(p.first, "initial state number is larger "
                            "than state count...");
                      error(res.states_loc, "... declared here.");
                      states = std::max(states, s + 1);
                    }
	      if (res.opts.want_kripke)
		res.h->ks->new_states(states, bddfalse);
	      else
		res.h->aut->new_states(states);
	      res.info_states.resize(states);
	    }
	  if (res.accset < 0)
	    {
	      error(yylhs.location, "missing 'Acceptance:' header");
	      res.ignore_acc = true;
	    }
          if (res.unknown_ap_max >= 0 && !res.ignore_more_ap)
            {
              error(res.unknown_ap_max_location,
                    "atomic proposition used in Alias without AP declaration");
              for (auto& p: res.alias)
                p.second = bddtrue;
            }
	  // Process properties.
	  {
	    auto explicit_labels = res.prop_is_true("explicit-labels");
	    auto implicit_labels = res.prop_is_true("implicit-labels");

	    if (implicit_labels)
	      {
		if (res.opts.want_kripke)
		  error(implicit_labels.loc,
			"Kripke structure may not use implicit labels");

		if (explicit_labels)
		  {
		    error(implicit_labels.loc,
			  "'properties: implicit-labels' is incompatible "
			  "with...");
		    error(explicit_labels.loc,
			  "... 'properties: explicit-labels'.");
		  }
		else
		  {
		    res.label_style = Implicit_Labels;
		  }
	      }

	    auto trans_labels = res.prop_is_true("trans-labels");
	    auto state_labels = res.prop_is_true("state-labels");

	    if (trans_labels)
	      {
		if (res.opts.want_kripke)
		  error(trans_labels.loc,
			"Kripke structures may not use transition labels");

		if (state_labels)
		  {
		    error(trans_labels.loc,
			  "'properties: trans-labels' is incompatible with...");
		    error(state_labels.loc,
			  "... 'properties: state-labels'.");
		  }
		else
		  {
		    if (res.label_style != Implicit_Labels)
		      res.label_style = Trans_Labels;
		  }
	      }
	    else if (state_labels)
	      {
		if (res.label_style != Implicit_Labels)
		  {
		    res.label_style = State_Labels;
		  }
		else
		  {
		    error(state_labels.loc,
			  "'properties: state-labels' is incompatible with...");
		    error(implicit_labels.loc,
			  "... 'properties: implicit-labels'.");
		  }
	      }

	    if (res.opts.want_kripke && res.label_style != State_Labels)
	      error(yylhs.location,
		    "Kripke structures should use 'properties: state-labels'");

	    auto state_acc = res.prop_is_true("state-acc");
	    auto trans_acc = res.prop_is_true("trans-acc");
	    if (trans_acc)
	      {
		if (state_acc)
		  {
		    error(trans_acc.loc,
			  "'properties: trans-acc' is incompatible with...");
		    error(state_acc.loc,
			  "... 'properties: state-acc'.");
		  }
		else
		  {
		    res.acc_style = Trans_Acc;
		  }
	      }
	    else if (state_acc)
	      {
		res.acc_style = State_Acc;
	      }

            if (auto univ_branch = res.prop_is_true("univ-branch"))
              if (res.opts.want_kripke)
                error(univ_branch.loc,
                    "Kripke structures may not use 'properties: univ-branch'");
          }
	  {
	    unsigned ss = res.start.size();
	    auto det = res.prop_is_true("deterministic");
	    auto no_exist = res.prop_is_false("exist-branch");
	    if (ss > 1)
	      {
		if (det)
		  {
		    error(det.loc,
			  "deterministic automata should have at most "
			  "one initial state");
                    res.universal = spot::trival::maybe();
		  }
                else if (no_exist)
                  {
		    error(no_exist.loc,
			  "universal automata should have at most "
			  "one initial state");
                    res.universal = spot::trival::maybe();
                  }
	      }
	    else
	      {
		// Assume the automaton is deterministic until proven
		// wrong, or unless we are building a Kripke structure.
                if (!res.opts.want_kripke)
                  {
                    res.universal = true;
                    res.existential = true;
                  }
	      }
            for (auto& ss: res.start)
              {
                if (ss.second.size() > 1)
                  {
                    if (auto no_univ = res.prop_is_false("univ-branch"))
                      {
                        error(ss.first,
                              "conjunct initial state despite...");
                        error(no_univ.loc, "... property: !univ-branch");
                      }
                    else if (v1plus)
                      if (auto det = res.prop_is_true("deterministic"))
                        {
                          error(ss.first,
                                "conjunct initial state despite...");
                          error(det.loc, "... property: deterministic");
                        }
                    res.existential = false;
                  }
              }
	    auto complete = res.prop_is_true("complete");
	    if (ss < 1)
	      {
		if (complete)
		  {
		    error(complete.loc,
			  "complete automata should have at least "
			  "one initial state");
		  }
		res.complete = false;
	      }
	    else
	      {
		// Assume the automaton is complete until proven
		// wrong.
                if (!res.opts.want_kripke)
                  res.complete = true;
	      }
	    // if ap_count == 0, then a Kripke structure could be
	    // declared complete, although that probably doesn't
	    // matter.
	    if (res.opts.want_kripke && complete && res.ap_count > 0)
	      error(complete.loc,
		    "Kripke structure may not be complete");
	  }
	  if (res.opts.trust_hoa)
	    {
	      auto& a = res.aut_or_ks;
	      auto& p = res.props;
	      auto e = p.end();
	      auto si = p.find("stutter-invariant");
	      if (si != e)
		{
		  a->prop_stutter_invariant(si->second.val);
		  auto i = p.find("stutter-sensitive");
		  if (i != e && si->second.val == i->second.val)
		    error(i->second.loc,
			  "automaton cannot be both stutter-invariant"
			  "and stutter-sensitive");
		}
	      else
		{
		  auto ss = p.find("stutter-sensitive");
		  if (ss != e)
		    a->prop_stutter_invariant(!ss->second.val);
		}
	      auto iw = p.find("inherently-weak");
	      auto vw = p.find("very-weak");
	      auto w = p.find("weak");
	      auto t = p.find("terminal");
              if (vw != e)
                {
                  a->prop_very_weak(vw->second.val);
                  if (w != e && !w->second.val && vw->second.val)
                    {
		      error(w->second.loc,
                            "'properties: !weak' contradicts...");
		      error(vw->second.loc,
			    "... 'properties: very-weak' given here");
                    }
                  if (iw != e && !iw->second.val && vw->second.val)
                    {
		      error(iw->second.loc,
                            "'properties: !inherently-weak' contradicts...");
		      error(vw->second.loc,
			    "... 'properties: very-weak' given here");
                    }
                }
	      if (iw != e)
		{
		  a->prop_inherently_weak(iw->second.val);
		  if (w != e && !iw->second.val && w->second.val)
		    {
		      error(w->second.loc, "'properties: weak' contradicts...");
		      error(iw->second.loc,
			    "... 'properties: !inherently-weak' given here");
		    }
		  if (t != e && !iw->second.val && t->second.val)
		    {
		      error(t->second.loc,
			    "'properties: terminal' contradicts...");
		      error(iw->second.loc,
			    "... 'properties: !inherently-weak' given here");
		    }
		}
	      if (w != e)
		{
		  a->prop_weak(w->second.val);
		  if (t != e && !w->second.val && t->second.val)
		    {
		      error(t->second.loc,
			    "'properties: terminal' contradicts...");
		      error(w->second.loc,
			    "... 'properties: !weak' given here");
		    }
		}
	      if (t != e)
		a->prop_terminal(t->second.val);
	      auto u = p.find("unambiguous");
	      if (u != e)
		{
		  a->prop_unambiguous(u->second.val);
		  auto d = p.find("deterministic");
		  if (d != e && !u->second.val && d->second.val)
		    {
		      error(d->second.loc,
			    "'properties: deterministic' contradicts...");
		      error(u->second.loc,
			    "... 'properties: !unambiguous' given here");
		    }
		}
	      auto sd = p.find("semi-deterministic");
	      if (sd != e)
		{
		  a->prop_semi_deterministic(sd->second.val);
		  auto d = p.find("deterministic");
		  if (d != e && !sd->second.val && d->second.val)
		    {
		      error(d->second.loc,
			    "'properties: deterministic' contradicts...");
		      error(sd->second.loc,
			    "... 'properties: !semi-deterministic' given here");
		    }
		}
	    }
	}
#line 1369 "parseaut.cc" // lalr1.cc:919
    break;

  case 17:
#line 668 "parseaut.yy" // lalr1.cc:919
    {
	   res.format_version = *(yystack_[0].value.str);
	   res.format_version_loc = yystack_[0].location;
	   delete (yystack_[0].value.str);
	 }
#line 1379 "parseaut.cc" // lalr1.cc:919
    break;

  case 18:
#line 674 "parseaut.yy" // lalr1.cc:919
    { res.h->loc = yystack_[0].location; }
#line 1385 "parseaut.cc" // lalr1.cc:919
    break;

  case 20:
#line 677 "parseaut.yy" // lalr1.cc:919
    {
       if (res.ignore_more_ap)
	 {
	   error(yystack_[1].location, "ignoring this redeclaration of APs...");
	   error(res.ap_loc, "... previously declared here.");
	 }
       else
	 {
	   res.ap_count = (yystack_[0].value.num);
	   res.ap.reserve((yystack_[0].value.num));
	 }
     }
#line 1402 "parseaut.cc" // lalr1.cc:919
    break;

  case 21:
#line 690 "parseaut.yy" // lalr1.cc:919
    {
       if (!res.ignore_more_ap)
	 {
	   res.ap_loc = yystack_[3].location + yystack_[2].location;
	   if ((int) res.ap.size() != res.ap_count)
	     {
	       std::ostringstream out;
	       out << "found " << res.ap.size()
		   << " atomic propositions instead of the "
		   << res.ap_count << " announced";
	       error(yylhs.location, out.str());
	     }
	   res.ignore_more_ap = true;
           // If we have seen Alias: before AP: we have some variable
           // renaming to perform.
           if (res.unknown_ap_max >= 0)
             {
               int apsize = res.ap.size();
               if (apsize <= res.unknown_ap_max)
                 {
                   error(res.unknown_ap_max_location,
                         "AP number is larger than the number of APs...");
                   error(yystack_[3].location, "... declared here");
                 }
               bddPair* pair = bdd_newpair();
               int max = std::min(res.unknown_ap_max, apsize - 1);
               for (int i = 0; i <= max; ++i)
                 if (i != res.ap[i])
                   bdd_setbddpair(pair, i, res.ap[i]);
               bdd extra = bddtrue;
               for (int i = apsize; i <= res.unknown_ap_max; ++i)
                 extra &= bdd_ithvar(i);
               for (auto& p: res.alias)
                 p.second = bdd_restrict(bdd_replace(p.second, pair), extra);
               bdd_freepair(pair);
             }
	 }
     }
#line 1445 "parseaut.cc" // lalr1.cc:919
    break;

  case 24:
#line 732 "parseaut.yy" // lalr1.cc:919
    {
	     if (res.states >= 0)
	       {
		 error(yylhs.location, "redefinition of the number of states...");
		 error(res.states_loc, "... previously defined here.");
	       }
	     else
	       {
		 res.states_loc = yylhs.location;
	       }
	     if (((int) (yystack_[0].value.num)) < 0)
	       {
		 error(yylhs.location, "too many states for this implementation");
		 YYABORT;
	       }
	     res.states = std::max(res.states, (int) (yystack_[0].value.num));
	   }
#line 1467 "parseaut.cc" // lalr1.cc:919
    break;

  case 25:
#line 750 "parseaut.yy" // lalr1.cc:919
    {
               res.start.emplace_back(yylhs.location, *(yystack_[0].value.states));
               delete (yystack_[0].value.states);
	     }
#line 1476 "parseaut.cc" // lalr1.cc:919
    break;

  case 26:
#line 755 "parseaut.yy" // lalr1.cc:919
    {
	       res.start.emplace_back(yylhs.location, std::vector<unsigned>{(yystack_[0].value.num)});
	     }
#line 1484 "parseaut.cc" // lalr1.cc:919
    break;

  case 28:
#line 759 "parseaut.yy" // lalr1.cc:919
    { res.in_alias=true; }
#line 1490 "parseaut.cc" // lalr1.cc:919
    break;

  case 29:
#line 760 "parseaut.yy" // lalr1.cc:919
    {
               res.in_alias = false;
	       if (!res.alias.emplace(*(yystack_[2].value.str), bdd_from_int((yystack_[0].value.b))).second)
		 {
		   std::ostringstream o;
		   o << "ignoring redefinition of alias @" << *(yystack_[2].value.str);
		   error(yylhs.location, o.str());
		 }
	       delete (yystack_[2].value.str);
	       bdd_delref((yystack_[0].value.b));
	     }
#line 1506 "parseaut.cc" // lalr1.cc:919
    break;

  case 30:
#line 772 "parseaut.yy" // lalr1.cc:919
    {
		if (res.ignore_more_acc)
		  {
		    error(yystack_[1].location + yystack_[0].location, "ignoring this redefinition of the "
			  "acceptance condition...");
		    error(res.accset_loc, "... previously defined here.");
		  }
		else if ((yystack_[0].value.num) > SPOT_MAX_ACCSETS)
		  {
		    error(yystack_[1].location + yystack_[0].location,
			  "this implementation cannot support such a large "
			  "number of acceptance sets");
		    YYABORT;
		  }
		else
		  {
		    res.aut_or_ks->acc().add_sets((yystack_[0].value.num));
		    res.accset = (yystack_[0].value.num);
		    res.accset_loc = yystack_[1].location + yystack_[0].location;
		  }
	     }
#line 1532 "parseaut.cc" // lalr1.cc:919
    break;

  case 31:
#line 794 "parseaut.yy" // lalr1.cc:919
    {
	       res.ignore_more_acc = true;
	       // Not setting the acceptance in case of error will
	       // force it to be true.
	       if (res.opts.want_kripke && (!(yystack_[0].value.code)->is_t() || (yystack_[2].value.num) > 0))
		 error(yystack_[2].location + yystack_[0].location,
		       "the acceptance for Kripke structure must be '0 t'");
	       else
		 res.aut_or_ks->set_acceptance((yystack_[2].value.num), *(yystack_[0].value.code));
	       delete (yystack_[0].value.code);
	     }
#line 1548 "parseaut.cc" // lalr1.cc:919
    break;

  case 32:
#line 806 "parseaut.yy" // lalr1.cc:919
    {
	       delete (yystack_[1].value.str);
	     }
#line 1556 "parseaut.cc" // lalr1.cc:919
    break;

  case 33:
#line 810 "parseaut.yy" // lalr1.cc:919
    {
	       delete (yystack_[1].value.str);
	       delete (yystack_[0].value.str);
	     }
#line 1565 "parseaut.cc" // lalr1.cc:919
    break;

  case 34:
#line 815 "parseaut.yy" // lalr1.cc:919
    {
	       res.aut_or_ks->set_named_prop("automaton-name", (yystack_[0].value.str));
	     }
#line 1573 "parseaut.cc" // lalr1.cc:919
    break;

  case 36:
#line 820 "parseaut.yy" // lalr1.cc:919
    { res.highlight_edges = new std::map<unsigned, unsigned>; }
#line 1579 "parseaut.cc" // lalr1.cc:919
    break;

  case 38:
#line 823 "parseaut.yy" // lalr1.cc:919
    { res.highlight_states = new std::map<unsigned, unsigned>; }
#line 1585 "parseaut.cc" // lalr1.cc:919
    break;

  case 40:
#line 826 "parseaut.yy" // lalr1.cc:919
    {
	       char c = (*(yystack_[1].value.str))[0];
	       if (c >= 'A' && c <= 'Z')
		 error(yylhs.location, "ignoring unsupported header \"" + *(yystack_[1].value.str) + ":\"\n\t"
		       "(but the capital indicates information that should not"
		       " be ignored)");
	       delete (yystack_[1].value.str);
	     }
#line 1598 "parseaut.cc" // lalr1.cc:919
    break;

  case 44:
#line 839 "parseaut.yy" // lalr1.cc:919
    {
	   if (!res.ignore_more_ap)
	     {
	       auto f = res.env->require(*(yystack_[0].value.str));
	       int b = 0;
	       if (f == nullptr)
		 {
		   std::ostringstream out;
		   out << "unknown atomic proposition \"" << *(yystack_[0].value.str) << "\"";
		   error(yystack_[0].location, out.str());
		   b = res.aut_or_ks->register_ap("$unknown$");
		 }
	       else
		 {
		   b = res.aut_or_ks->register_ap(f);
		   if (!res.ap_set.emplace(b).second)
		     {
		       std::ostringstream out;
		       out << "duplicate atomic proposition \"" << *(yystack_[0].value.str) << "\"";
		       error(yystack_[0].location, out.str());
		     }
		 }
	       res.ap.push_back(b);
	     }
	   delete (yystack_[0].value.str);
	 }
#line 1629 "parseaut.cc" // lalr1.cc:919
    break;

  case 48:
#line 870 "parseaut.yy" // lalr1.cc:919
    {
	      delete (yystack_[0].value.str);
	    }
#line 1637 "parseaut.cc" // lalr1.cc:919
    break;

  case 50:
#line 875 "parseaut.yy" // lalr1.cc:919
    {
                bool val = true;
                // no-univ-branch was replaced by !univ-branch in HOA 1.1
                if (*(yystack_[0].value.str) == "no-univ-branch")
                  {
                    *(yystack_[0].value.str) = "univ-branch";
                    val = false;
                  }
		auto pos = res.props.emplace(*(yystack_[0].value.str), result_::prop_info{yystack_[0].location, val});
		if (pos.first->second.val != val)
		  {
		    std::ostringstream out(std::ios_base::ate);
		    error(yystack_[0].location, "'properties: "s + (val ? "" : "!")
                          + *(yystack_[0].value.str) + "' contradicts...");
		    error(pos.first->second.loc,
			  "... 'properties: "s + (val ? "!" : "") + *(yystack_[0].value.str)
			  + "' previously given here.");
		  }
		delete (yystack_[0].value.str);
	      }
#line 1662 "parseaut.cc" // lalr1.cc:919
    break;

  case 51:
#line 896 "parseaut.yy" // lalr1.cc:919
    {
		auto loc = yystack_[1].location + yystack_[0].location;
		auto pos =
		  res.props.emplace(*(yystack_[0].value.str), result_::prop_info{loc, false});
		if (pos.first->second.val)
		  {
		    std::ostringstream out(std::ios_base::ate);
		    error(loc, "'properties: !"s + *(yystack_[0].value.str) + "' contradicts...");
		    error(pos.first->second.loc, "... 'properties: "s + *(yystack_[0].value.str)
                          + "' previously given here.");
		  }
		delete (yystack_[0].value.str);
	      }
#line 1680 "parseaut.cc" // lalr1.cc:919
    break;

  case 53:
#line 912 "parseaut.yy" // lalr1.cc:919
    {
		res.highlight_edges->emplace((yystack_[1].value.num), (yystack_[0].value.num));
	      }
#line 1688 "parseaut.cc" // lalr1.cc:919
    break;

  case 55:
#line 917 "parseaut.yy" // lalr1.cc:919
    {
		res.highlight_states->emplace((yystack_[1].value.num), (yystack_[0].value.num));
	      }
#line 1696 "parseaut.cc" // lalr1.cc:919
    break;

  case 59:
#line 925 "parseaut.yy" // lalr1.cc:919
    {
		 delete (yystack_[0].value.str);
	       }
#line 1704 "parseaut.cc" // lalr1.cc:919
    break;

  case 60:
#line 929 "parseaut.yy" // lalr1.cc:919
    {
		 delete (yystack_[0].value.str);
	       }
#line 1712 "parseaut.cc" // lalr1.cc:919
    break;

  case 61:
#line 934 "parseaut.yy" // lalr1.cc:919
    {
              (yylhs.value.states) = new std::vector<unsigned>{(yystack_[2].value.num), (yystack_[0].value.num)};
            }
#line 1720 "parseaut.cc" // lalr1.cc:919
    break;

  case 62:
#line 938 "parseaut.yy" // lalr1.cc:919
    {
              (yylhs.value.states) = (yystack_[2].value.states);
              (yylhs.value.states)->emplace_back((yystack_[0].value.num));
            }
#line 1729 "parseaut.cc" // lalr1.cc:919
    break;

  case 63:
#line 946 "parseaut.yy" // lalr1.cc:919
    {
              (yylhs.value.states) = new std::vector<unsigned>{(yystack_[2].value.num), (yystack_[0].value.num)};
            }
#line 1737 "parseaut.cc" // lalr1.cc:919
    break;

  case 64:
#line 950 "parseaut.yy" // lalr1.cc:919
    {
              (yylhs.value.states) = (yystack_[2].value.states);
              (yylhs.value.states)->emplace_back((yystack_[0].value.num));
            }
#line 1746 "parseaut.cc" // lalr1.cc:919
    break;

  case 65:
#line 956 "parseaut.yy" // lalr1.cc:919
    {
	      (yylhs.value.b) = bddtrue.id();
	    }
#line 1754 "parseaut.cc" // lalr1.cc:919
    break;

  case 66:
#line 960 "parseaut.yy" // lalr1.cc:919
    {
	      (yylhs.value.b) = bddfalse.id();
	    }
#line 1762 "parseaut.cc" // lalr1.cc:919
    break;

  case 67:
#line 964 "parseaut.yy" // lalr1.cc:919
    {
              if (res.in_alias && !res.ignore_more_ap)
                {
                  // We are reading Alias: before AP: has been given.
                  // Use $1 as temporary variable number.  We will relabel
                  // everything once AP: is known.
                  if (res.unknown_ap_max < (int)(yystack_[0].value.num))
                    {
                      res.unknown_ap_max = (yystack_[0].value.num);
                      res.unknown_ap_max_location = yystack_[0].location;
                      int missing_vars = 1 + bdd_varnum() - (yystack_[0].value.num);
                      if (missing_vars > 0)
                        bdd_extvarnum(missing_vars);
                    }
                  (yylhs.value.b) = bdd_ithvar((yystack_[0].value.num)).id();
                }
	      else if ((yystack_[0].value.num) >= res.ap.size())
		{
		  error(yystack_[0].location, "AP number is larger than the number of APs...");
		  error(res.ap_loc, "... declared here");
		  (yylhs.value.b) = bddtrue.id();
		}
	      else
		{
		  (yylhs.value.b) = bdd_ithvar(res.ap[(yystack_[0].value.num)]).id();
		  bdd_addref((yylhs.value.b));
		}
	    }
#line 1795 "parseaut.cc" // lalr1.cc:919
    break;

  case 68:
#line 993 "parseaut.yy" // lalr1.cc:919
    {
	      auto i = res.alias.find(*(yystack_[0].value.str));
	      if (i == res.alias.end())
		{
		  error(yylhs.location, "unknown alias @" + *(yystack_[0].value.str));
		  (yylhs.value.b) = 1;
		}
	      else
		{
		  (yylhs.value.b) = i->second.id();
		  bdd_addref((yylhs.value.b));
		}
	      delete (yystack_[0].value.str);
	    }
#line 1814 "parseaut.cc" // lalr1.cc:919
    break;

  case 69:
#line 1008 "parseaut.yy" // lalr1.cc:919
    {
              (yylhs.value.b) = bdd_not((yystack_[0].value.b));
              bdd_delref((yystack_[0].value.b));
              bdd_addref((yylhs.value.b));
            }
#line 1824 "parseaut.cc" // lalr1.cc:919
    break;

  case 70:
#line 1014 "parseaut.yy" // lalr1.cc:919
    {
              (yylhs.value.b) = bdd_and((yystack_[2].value.b), (yystack_[0].value.b));
              bdd_delref((yystack_[2].value.b));
              bdd_delref((yystack_[0].value.b));
              bdd_addref((yylhs.value.b));
            }
#line 1835 "parseaut.cc" // lalr1.cc:919
    break;

  case 71:
#line 1021 "parseaut.yy" // lalr1.cc:919
    {
              (yylhs.value.b) = bdd_or((yystack_[2].value.b), (yystack_[0].value.b));
              bdd_delref((yystack_[2].value.b));
              bdd_delref((yystack_[0].value.b));
              bdd_addref((yylhs.value.b));
            }
#line 1846 "parseaut.cc" // lalr1.cc:919
    break;

  case 72:
#line 1028 "parseaut.yy" // lalr1.cc:919
    {
	    (yylhs.value.b) = (yystack_[1].value.b);
	  }
#line 1854 "parseaut.cc" // lalr1.cc:919
    break;

  case 73:
#line 1034 "parseaut.yy" // lalr1.cc:919
    {
	      if ((int) (yystack_[0].value.num) >= res.accset)
		{
		  if (!res.ignore_acc)
		    {
		      error(yystack_[0].location, "number is larger than the count "
			    "of acceptance sets...");
		      error(res.accset_loc, "... declared here.");
		    }
		  (yylhs.value.num) = -1U;
		}
	      else
		{
		  (yylhs.value.num) = (yystack_[0].value.num);
		}
	    }
#line 1875 "parseaut.cc" // lalr1.cc:919
    break;

  case 74:
#line 1052 "parseaut.yy" // lalr1.cc:919
    {
		   if ((yystack_[1].value.num) != -1U)
		     {
		       res.pos_acc_sets |= res.aut_or_ks->acc().mark((yystack_[1].value.num));
		       if (*(yystack_[3].value.str) == "Inf")
                         {
                           (yylhs.value.code) = new spot::acc_cond::acc_code
                             (res.aut_or_ks->acc().inf({(yystack_[1].value.num)}));
                         }
		       else if (*(yystack_[3].value.str) == "Fin")
                         {
                           (yylhs.value.code) = new spot::acc_cond::acc_code
                             (res.aut_or_ks->acc().fin({(yystack_[1].value.num)}));
                         }
                       else
                         {
                           error(yystack_[3].location, "unknown acceptance '"s + *(yystack_[3].value.str)
                                 + "', expected Fin or Inf");
                           (yylhs.value.code) = new spot::acc_cond::acc_code;
                         }
		     }
		   else
		     {
		       (yylhs.value.code) = new spot::acc_cond::acc_code;
		     }
		   delete (yystack_[3].value.str);
		 }
#line 1907 "parseaut.cc" // lalr1.cc:919
    break;

  case 75:
#line 1080 "parseaut.yy" // lalr1.cc:919
    {
		   if ((yystack_[1].value.num) != -1U)
		     {
		       res.neg_acc_sets |= res.aut_or_ks->acc().mark((yystack_[1].value.num));
		       if (*(yystack_[4].value.str) == "Inf")
			 (yylhs.value.code) = new spot::acc_cond::acc_code
			   (res.aut_or_ks->acc().inf_neg({(yystack_[1].value.num)}));
		       else
			 (yylhs.value.code) = new spot::acc_cond::acc_code
			   (res.aut_or_ks->acc().fin_neg({(yystack_[1].value.num)}));
		     }
		   else
		     {
		       (yylhs.value.code) = new spot::acc_cond::acc_code;
		     }
		   delete (yystack_[4].value.str);
		 }
#line 1929 "parseaut.cc" // lalr1.cc:919
    break;

  case 76:
#line 1098 "parseaut.yy" // lalr1.cc:919
    {
		   (yylhs.value.code) = (yystack_[1].value.code);
		 }
#line 1937 "parseaut.cc" // lalr1.cc:919
    break;

  case 77:
#line 1102 "parseaut.yy" // lalr1.cc:919
    {
		   *(yystack_[0].value.code) &= std::move(*(yystack_[2].value.code));
		   (yylhs.value.code) = (yystack_[0].value.code);
		   delete (yystack_[2].value.code);
		 }
#line 1947 "parseaut.cc" // lalr1.cc:919
    break;

  case 78:
#line 1108 "parseaut.yy" // lalr1.cc:919
    {
		   *(yystack_[0].value.code) |= std::move(*(yystack_[2].value.code));
		   (yylhs.value.code) = (yystack_[0].value.code);
		   delete (yystack_[2].value.code);
		 }
#line 1957 "parseaut.cc" // lalr1.cc:919
    break;

  case 79:
#line 1114 "parseaut.yy" // lalr1.cc:919
    {
		   (yylhs.value.code) = new spot::acc_cond::acc_code;
		 }
#line 1965 "parseaut.cc" // lalr1.cc:919
    break;

  case 80:
#line 1118 "parseaut.yy" // lalr1.cc:919
    {
	         {
		   (yylhs.value.code) = new spot::acc_cond::acc_code
		     (res.aut_or_ks->acc().fin({}));
		 }
	       }
#line 1976 "parseaut.cc" // lalr1.cc:919
    break;

  case 81:
#line 1127 "parseaut.yy" // lalr1.cc:919
    {
	for (auto& p: res.start)
          for (unsigned s: p.second)
            if (s >= res.info_states.size() || !res.info_states[s].declared)
              {
                error(p.first, "initial state " + std::to_string(s) +
                      " has no definition");
                // Pretend that the state is declared so we do not
                // mention it in the next loop.
                if (s < res.info_states.size())
                  res.info_states[s].declared = true;
                res.complete = spot::trival::maybe();
              }
	unsigned n = res.info_states.size();
	// States with number above res.states have already caused a
	// diagnostic, so let not add another one.
	if (res.states >= 0)
	  n = res.states;
	for (unsigned i = 0; i < n; ++i)
	  {
	    auto& p = res.info_states[i];
            if (!p.declared)
              {
                if (p.used)
                  error(p.used_loc,
                        "state " + std::to_string(i) + " has no definition");
                if (!p.used && res.complete)
                  if (auto p = res.prop_is_true("complete"))
                    {
                      error(res.states_loc,
                            "state " + std::to_string(i) +
                            " has no definition...");
                      error(p.loc, "... despite 'properties: complete'");
                    }
                res.complete = false;
              }
	  }
        if (res.complete)
          if (auto p = res.prop_is_false("complete"))
            {
              error(yystack_[0].location, "automaton is complete...");
              error(p.loc, "... despite 'properties: !complete'");
            }
        bool det_warned = false;
        if (res.universal && res.existential)
          if (auto p = res.prop_is_false("deterministic"))
            {
              error(yystack_[0].location, "automaton is deterministic...");
              error(p.loc, "... despite 'properties: !deterministic'");
              det_warned = true;
            }
        static bool tolerant = getenv("SPOT_HOA_TOLERANT");
        if (res.universal.is_true() && !det_warned && !tolerant)
          if (auto p = res.prop_is_true("exist-branch"))
            {
              error(yystack_[0].location, "automaton has no existential branching...");
              error(p.loc, "... despite 'properties: exist-branch'\n"
                    "note: If this is an issue you cannot fix, you may disable "
                    "this diagnostic\n      by defining the SPOT_HOA_TOLERANT "
                    "environment variable.");
              det_warned = true;
            }
        if (res.existential.is_true() && !det_warned && !tolerant)
          if (auto p = res.prop_is_true("univ-branch"))
            {
              error(yystack_[0].location, "automaton is has no universal branching...");
              error(p.loc, "... despite 'properties: univ-branch'\n"
                    "note: If this is an issue you cannot fix, you may disable "
                    "this diagnostic\n      by defining the SPOT_HOA_TOLERANT "
                    "environment variable.");
              det_warned = true;
            }
      }
#line 2054 "parseaut.cc" // lalr1.cc:919
    break;

  case 82:
#line 1201 "parseaut.yy" // lalr1.cc:919
    {
	     if (((int) (yystack_[0].value.num)) < 0)
	       {
		 error(yystack_[0].location, "state number is too large for this implementation");
		 YYABORT;
	       }
	     (yylhs.value.num) = (yystack_[0].value.num);
	   }
#line 2067 "parseaut.cc" // lalr1.cc:919
    break;

  case 83:
#line 1211 "parseaut.yy" // lalr1.cc:919
    {
		     if ((int) (yystack_[0].value.num) >= res.states)
		       {
			 if (res.states >= 0)
			   {
			     error(yystack_[0].location, "state number is larger than state "
				   "count...");
			     error(res.states_loc, "... declared here.");
			   }
			 if (res.opts.want_kripke)
			   {
			     int missing =
			       ((int) (yystack_[0].value.num)) - res.h->ks->num_states() + 1;
			     if (missing >= 0)
			       {
				 res.h->ks->new_states(missing, bddfalse);
				 res.info_states.resize
				   (res.info_states.size() + missing);
			       }
			   }
			 else
			   {
			     int missing =
			       ((int) (yystack_[0].value.num)) - res.h->aut->num_states() + 1;
			     if (missing >= 0)
			       {
				 res.h->aut->new_states(missing);
				 res.info_states.resize
				   (res.info_states.size() + missing);
			       }
			   }
		       }
		     // Remember the first place were a state is the
		     // destination of a transition.
		     if (!res.info_states[(yystack_[0].value.num)].used)
		       {
			 res.info_states[(yystack_[0].value.num)].used = true;
			 res.info_states[(yystack_[0].value.num)].used_loc = yystack_[0].location;
		       }
		     (yylhs.value.num) = (yystack_[0].value.num);
		   }
#line 2113 "parseaut.cc" // lalr1.cc:919
    break;

  case 85:
#line 1255 "parseaut.yy" // lalr1.cc:919
    {
	  if ((res.universal.is_true() || res.complete.is_true()))
	    {
	      bdd available = bddtrue;
	      bool det = true;
	      for (auto& t: res.h->aut->out(res.cur_state))
		{
		  if (det && !bdd_implies(t.cond, available))
		    det = false;
		  available -= t.cond;
		}
	      if (res.universal.is_true() && !det)
		{
		  res.universal = false;
		  if (auto p = res.prop_is_true("deterministic"))
		    {
		      error(yystack_[0].location, "automaton is not deterministic...");
		      error(p.loc,
			    "... despite 'properties: deterministic'");
		    }
		  else if (auto p = res.prop_is_false("exist-branch"))
		    {
		      error(yystack_[0].location, "automaton has existential branching...");
		      error(p.loc,
			    "... despite 'properties: !exist-branch'");
		    }
		}
	      if (res.complete.is_true() && available != bddfalse)
		{
		  res.complete = false;
		  if (auto p = res.prop_is_true("complete"))
		    {
		      error(yystack_[0].location, "automaton is not complete...");
		      error(p.loc, "... despite 'properties: complete'");
		    }
		}
	    }
	}
#line 2156 "parseaut.cc" // lalr1.cc:919
    break;

  case 87:
#line 1295 "parseaut.yy" // lalr1.cc:919
    {
	 if (!res.has_state_label) // Implicit labels
	   {
	     if (res.cur_guard != res.guards.end())
	       error(yylhs.location, "not enough transitions for this state");

	     if (res.label_style == State_Labels)
	       {
		 error(yystack_[0].location, "these transitions have implicit labels but the"
		       " automaton is...");
		 error(res.props["state-labels"].loc, "... declared with "
		       "'properties: state-labels'");
		 // Do not repeat this message.
		 res.label_style = Mixed_Labels;
	       }
	     res.cur_guard = res.guards.begin();
	   }
	 else if (res.opts.want_kripke)
	   {
	     res.h->ks->state_from_number(res.cur_state)->cond(res.state_label);
	   }

       }
#line 2184 "parseaut.cc" // lalr1.cc:919
    break;

  case 88:
#line 1319 "parseaut.yy" // lalr1.cc:919
    {
	 // Assume the worse.  This skips the tests about determinism
	 // we might perform on the state.
	 res.universal = spot::trival::maybe();
	 res.existential = spot::trival::maybe();
	 res.complete = spot::trival::maybe();
       }
#line 2196 "parseaut.cc" // lalr1.cc:919
    break;

  case 89:
#line 1329 "parseaut.yy" // lalr1.cc:919
    {
	    res.cur_state = (yystack_[2].value.num);
	    if (res.info_states[(yystack_[2].value.num)].declared)
	      {
		std::ostringstream o;
		o << "redeclaration of state " << (yystack_[2].value.num);
		error(yystack_[4].location + yystack_[2].location, o.str());
                // The additional transitions from extra states might
                // led us to believe that the automaton is complete
                // while it is not if we ignore them.
                if (res.complete.is_true())
                  res.complete = spot::trival::maybe();
	      }
	    res.info_states[(yystack_[2].value.num)].declared = true;
	    res.acc_state = (yystack_[0].value.mark);
	    if ((yystack_[1].value.str))
	      {
		if (!res.state_names)
		  res.state_names =
		    new std::vector<std::string>(res.states > 0 ?
						 res.states : 0);
		if (res.state_names->size() < (yystack_[2].value.num) + 1)
		  res.state_names->resize((yystack_[2].value.num) + 1);
		(*res.state_names)[(yystack_[2].value.num)] = std::move(*(yystack_[1].value.str));
		delete (yystack_[1].value.str);
	      }
	    if (res.opts.want_kripke && !res.has_state_label)
	      error(yylhs.location, "Kripke structures should have labeled states");
	  }
#line 2230 "parseaut.cc" // lalr1.cc:919
    break;

  case 90:
#line 1359 "parseaut.yy" // lalr1.cc:919
    {
             res.cur_label = bdd_from_int((yystack_[1].value.b));
             bdd_delref((yystack_[1].value.b));
	   }
#line 2239 "parseaut.cc" // lalr1.cc:919
    break;

  case 91:
#line 1364 "parseaut.yy" // lalr1.cc:919
    {
	     error(yylhs.location, "ignoring this invalid label");
	     res.cur_label = bddtrue;
	   }
#line 2248 "parseaut.cc" // lalr1.cc:919
    break;

  case 92:
#line 1368 "parseaut.yy" // lalr1.cc:919
    { res.has_state_label = false; }
#line 2254 "parseaut.cc" // lalr1.cc:919
    break;

  case 93:
#line 1370 "parseaut.yy" // lalr1.cc:919
    {
		 res.has_state_label = true;
		 res.state_label_loc = yystack_[0].location;
		 res.state_label = res.cur_label;
		 if (res.label_style == Trans_Labels
		     || res.label_style == Implicit_Labels)
		   {
		     error(yylhs.location,
			   "state label used although the automaton was...");
		     if (res.label_style == Trans_Labels)
		       error(res.props["trans-labels"].loc,
			     "... declared with 'properties: trans-labels'"
			     " here");
		     else
		       error(res.props["implicit-labels"].loc,
			     "... declared with 'properties: implicit-labels'"
			     " here");
		     // Do not show this error anymore.
		     res.label_style = Mixed_Labels;
		   }
	       }
#line 2280 "parseaut.cc" // lalr1.cc:919
    break;

  case 94:
#line 1392 "parseaut.yy" // lalr1.cc:919
    {
		   if (res.has_state_label)
		     {
		       error(yystack_[0].location, "cannot label this edge because...");
		       error(res.state_label_loc,
			     "... the state is already labeled.");
		       res.cur_label = res.state_label;
		     }
		   if (res.label_style == State_Labels
		       || res.label_style == Implicit_Labels)
		     {
		       error(yylhs.location, "transition label used although the "
			     "automaton was...");
		       if (res.label_style == State_Labels)
			 error(res.props["state-labels"].loc,
			       "... declared with 'properties: state-labels' "
			       "here");
		       else
			 error(res.props["implicit-labels"].loc,
			       "... declared with 'properties: implicit-labels'"
			       " here");
		       // Do not show this error anymore.
		       res.label_style = Mixed_Labels;
		     }
		 }
#line 2310 "parseaut.cc" // lalr1.cc:919
    break;

  case 95:
#line 1419 "parseaut.yy" // lalr1.cc:919
    {
	       (yylhs.value.mark) = (yystack_[1].value.mark);
	       if (res.ignore_acc && !res.ignore_acc_silent)
		 {
		   error(yylhs.location, "ignoring acceptance sets because of "
			 "missing acceptance condition");
		   // Emit this message only once.
		   res.ignore_acc_silent = true;
		 }
	     }
#line 2325 "parseaut.cc" // lalr1.cc:919
    break;

  case 96:
#line 1430 "parseaut.yy" // lalr1.cc:919
    {
	       error(yylhs.location, "ignoring this invalid acceptance set");
	     }
#line 2333 "parseaut.cc" // lalr1.cc:919
    break;

  case 97:
#line 1434 "parseaut.yy" // lalr1.cc:919
    {
	    (yylhs.value.mark) = spot::acc_cond::mark_t({});
	  }
#line 2341 "parseaut.cc" // lalr1.cc:919
    break;

  case 98:
#line 1438 "parseaut.yy" // lalr1.cc:919
    {
	    if (res.ignore_acc || (yystack_[0].value.num) == -1U)
	      (yylhs.value.mark) = spot::acc_cond::mark_t({});
	    else
	      (yylhs.value.mark) = (yystack_[1].value.mark) | res.aut_or_ks->acc().mark((yystack_[0].value.num));
	  }
#line 2352 "parseaut.cc" // lalr1.cc:919
    break;

  case 99:
#line 1446 "parseaut.yy" // lalr1.cc:919
    {
                 (yylhs.value.mark) = spot::acc_cond::mark_t({});
               }
#line 2360 "parseaut.cc" // lalr1.cc:919
    break;

  case 100:
#line 1450 "parseaut.yy" // lalr1.cc:919
    {
		 (yylhs.value.mark) = (yystack_[0].value.mark);
		 if (res.acc_style == Trans_Acc)
		   {
		     error(yylhs.location, "state-based acceptance used despite...");
		     error(res.props["trans-acc"].loc,
			   "... declaration of transition-based acceptance.");
		     res.acc_style = Mixed_Acc;
		   }
	       }
#line 2375 "parseaut.cc" // lalr1.cc:919
    break;

  case 101:
#line 1461 "parseaut.yy" // lalr1.cc:919
    {
                 (yylhs.value.mark) = spot::acc_cond::mark_t({});
               }
#line 2383 "parseaut.cc" // lalr1.cc:919
    break;

  case 102:
#line 1465 "parseaut.yy" // lalr1.cc:919
    {
		 (yylhs.value.mark) = (yystack_[0].value.mark);
		 res.trans_acc_seen = true;
		 if (res.acc_style == State_Acc)
		   {
		     error(yylhs.location, "trans-based acceptance used despite...");
		     error(res.props["state-acc"].loc,
			   "... declaration of state-based acceptance.");
		     res.acc_style = Mixed_Acc;
		   }
	       }
#line 2399 "parseaut.cc" // lalr1.cc:919
    break;

  case 108:
#line 1484 "parseaut.yy" // lalr1.cc:919
    {
			      bdd cond = bddtrue;
			      if (!res.has_state_label)
				error(yylhs.location, "missing label for this edge "
				      "(previous edge is labeled)");
			      else
				cond = res.state_label;
			      if (cond != bddfalse)
				{
				  if (res.opts.want_kripke)
				    res.h->ks->new_edge(res.cur_state, (yystack_[1].value.num));
				  else
				    res.h->aut->new_edge(res.cur_state, (yystack_[1].value.num),
							 cond,
							 (yystack_[0].value.mark) | res.acc_state);
				}
			    }
#line 2421 "parseaut.cc" // lalr1.cc:919
    break;

  case 109:
#line 1502 "parseaut.yy" // lalr1.cc:919
    {
		if (res.cur_label != bddfalse)
		  {
		    if (res.opts.want_kripke)
		      res.h->ks->new_edge(res.cur_state, (yystack_[1].value.num));
		    else
		      res.h->aut->new_edge(res.cur_state, (yystack_[1].value.num),
					   res.cur_label, (yystack_[0].value.mark) | res.acc_state);
		  }
	      }
#line 2436 "parseaut.cc" // lalr1.cc:919
    break;

  case 110:
#line 1513 "parseaut.yy" // lalr1.cc:919
    {
                if (res.cur_label != bddfalse)
                  {
                    assert(!res.opts.want_kripke);
                    res.h->aut->new_univ_edge(res.cur_state,
                                              (yystack_[1].value.states)->begin(), (yystack_[1].value.states)->end(),
                                              res.cur_label,
                                              (yystack_[0].value.mark) | res.acc_state);
                  }
                delete (yystack_[1].value.states);
	      }
#line 2452 "parseaut.cc" // lalr1.cc:919
    break;

  case 111:
#line 1526 "parseaut.yy" // lalr1.cc:919
    {
                (yylhs.value.states) = (yystack_[0].value.states);
                if (auto ub = res.prop_is_false("univ-branch"))
                  {
                    error(yystack_[0].location, "universal branch used despite"
                          " previous declaration...");
                    error(ub.loc, "... here");
                  }
                res.existential = false;
              }
#line 2467 "parseaut.cc" // lalr1.cc:919
    break;

  case 115:
#line 1544 "parseaut.yy" // lalr1.cc:919
    {
		  bdd cond;
		  if (res.has_state_label)
		    {
		      cond = res.state_label;
		    }
		  else
		    {
		      if (res.guards.empty())
			fill_guards(res);
		      if (res.cur_guard == res.guards.end())
			{
			  error(yylhs.location, "too many transitions for this state, "
				"ignoring this one");
			  cond = bddfalse;
			}
		      else
			{
			  cond = *res.cur_guard++;
			}
		    }
		  if (cond != bddfalse)
		    {
		      if (res.opts.want_kripke)
			res.h->ks->new_edge(res.cur_state, (yystack_[1].value.num));
		      else
			res.h->aut->new_edge(res.cur_state, (yystack_[1].value.num),
					     cond, (yystack_[0].value.mark) | res.acc_state);
		    }
		}
#line 2502 "parseaut.cc" // lalr1.cc:919
    break;

  case 116:
#line 1575 "parseaut.yy" // lalr1.cc:919
    {
		  bdd cond;
		  if (res.has_state_label)
		    {
		      cond = res.state_label;
		    }
		  else
		    {
		      if (res.guards.empty())
			fill_guards(res);
		      if (res.cur_guard == res.guards.end())
			{
			  error(yylhs.location, "too many transitions for this state, "
				"ignoring this one");
			  cond = bddfalse;
			}
		      else
			{
			  cond = *res.cur_guard++;
			}
		    }
		  if (cond != bddfalse)
		    {
		      assert(!res.opts.want_kripke);
                      res.h->aut->new_univ_edge(res.cur_state,
                                                (yystack_[1].value.states)->begin(), (yystack_[1].value.states)->end(),
                                                cond, (yystack_[0].value.mark) | res.acc_state);
		    }
                  delete (yystack_[1].value.states);
		}
#line 2537 "parseaut.cc" // lalr1.cc:919
    break;

  case 117:
#line 1606 "parseaut.yy" // lalr1.cc:919
    {
			    error(yystack_[1].location, "ignoring this label, because previous"
				  " edge has no label");
                          }
#line 2546 "parseaut.cc" // lalr1.cc:919
    break;

  case 119:
#line 1618 "parseaut.yy" // lalr1.cc:919
    {
	 error(yylhs.location, "failed to parse this as an ltl2dstar automaton");
       }
#line 2554 "parseaut.cc" // lalr1.cc:919
    break;

  case 120:
#line 1623 "parseaut.yy" // lalr1.cc:919
    {
         res.h->type = spot::parsed_aut_type::DRA;
         res.plus = 1;
         res.minus = 0;
	 if (res.opts.want_kripke)
	   {
	     error(yylhs.location,
		   "cannot read a Kripke structure out of a DSTAR automaton");
	     YYABORT;
	   }
       }
#line 2570 "parseaut.cc" // lalr1.cc:919
    break;

  case 121:
#line 1635 "parseaut.yy" // lalr1.cc:919
    {
	 res.h->type = spot::parsed_aut_type::DSA;
         res.plus = 0;
         res.minus = 1;
	 if (res.opts.want_kripke)
	   {
	     error(yylhs.location,
		   "cannot read a Kripke structure out of a DSTAR automaton");
	     YYABORT;
	   }
       }
#line 2586 "parseaut.cc" // lalr1.cc:919
    break;

  case 122:
#line 1648 "parseaut.yy" // lalr1.cc:919
    {
    if (res.states < 0)
      error(yystack_[0].location, "missing state count");
    if (!res.ignore_more_acc)
      error(yystack_[0].location, "missing acceptance-pair count");
    if (res.start.empty())
      error(yystack_[0].location, "missing start-state number");
    if (!res.ignore_more_ap)
      error(yystack_[0].location, "missing atomic propositions definition");

    if (res.states > 0)
      {
	res.h->aut->new_states(res.states);;
	res.info_states.resize(res.states);
      }
    res.acc_style = State_Acc;
    res.universal = true;
    res.existential = true;
    res.complete = true;
    fill_guards(res);
    res.cur_guard = res.guards.end();
  }
#line 2613 "parseaut.cc" // lalr1.cc:919
    break;

  case 125:
#line 1674 "parseaut.yy" // lalr1.cc:919
    {
    if (res.ignore_more_acc)
      {
	error(yystack_[2].location + yystack_[1].location, "ignoring this redefinition of the "
	      "acceptance pairs...");
	error(res.accset_loc, "... previously defined here.");
      }
    else{
      res.accset = (yystack_[0].value.num);
      res.h->aut->set_acceptance(2 * (yystack_[0].value.num),
				 res.h->type == spot::parsed_aut_type::DRA
				 ? spot::acc_cond::acc_code::rabin((yystack_[0].value.num))
				 : spot::acc_cond::acc_code::streett((yystack_[0].value.num)));
      res.accset_loc = yystack_[0].location;
      res.ignore_more_acc = true;
    }
  }
#line 2635 "parseaut.cc" // lalr1.cc:919
    break;

  case 126:
#line 1692 "parseaut.yy" // lalr1.cc:919
    {
    if (res.states < 0)
      {
	res.states = (yystack_[0].value.num);
      }
    else
      {
	error(yylhs.location, "redeclaration of state count");
	if ((unsigned) res.states < (yystack_[0].value.num))
	  res.states = (yystack_[0].value.num);
      }
  }
#line 2652 "parseaut.cc" // lalr1.cc:919
    break;

  case 127:
#line 1705 "parseaut.yy" // lalr1.cc:919
    {
    res.start.emplace_back(yystack_[0].location, std::vector<unsigned>{(yystack_[0].value.num)});
  }
#line 2660 "parseaut.cc" // lalr1.cc:919
    break;

  case 129:
#line 1711 "parseaut.yy" // lalr1.cc:919
    {
    if (res.cur_guard != res.guards.end())
      error(yystack_[2].location, "not enough transitions for previous state");
    if (res.states < 0 || (yystack_[1].value.num) >= (unsigned) res.states)
      {
	std::ostringstream o;
	if (res.states > 0)
	  {
	    o << "state numbers should be in the range [0.."
	      << res.states - 1 << "]";
	  }
	else
	  {
	    o << "no states have been declared";
	  }
	error(yystack_[1].location, o.str());
      }
    else
      {
	res.info_states[(yystack_[1].value.num)].declared = true;

	if ((yystack_[0].value.str))
	  {
	    if (!res.state_names)
	      res.state_names =
		new std::vector<std::string>(res.states > 0 ?
					     res.states : 0);
	    if (res.state_names->size() < (yystack_[1].value.num) + 1)
	      res.state_names->resize((yystack_[1].value.num) + 1);
	    (*res.state_names)[(yystack_[1].value.num)] = std::move(*(yystack_[0].value.str));
	    delete (yystack_[0].value.str);
	  }
      }

    res.cur_guard = res.guards.begin();
    res.dest_map.clear();
    res.cur_state = (yystack_[1].value.num);
  }
#line 2703 "parseaut.cc" // lalr1.cc:919
    break;

  case 130:
#line 1750 "parseaut.yy" // lalr1.cc:919
    { (yylhs.value.num) = res.plus; }
#line 2709 "parseaut.cc" // lalr1.cc:919
    break;

  case 131:
#line 1751 "parseaut.yy" // lalr1.cc:919
    { (yylhs.value.num) = res.minus; }
#line 2715 "parseaut.cc" // lalr1.cc:919
    break;

  case 132:
#line 1755 "parseaut.yy" // lalr1.cc:919
    {
    (yylhs.value.mark) = spot::acc_cond::mark_t({});
  }
#line 2723 "parseaut.cc" // lalr1.cc:919
    break;

  case 133:
#line 1759 "parseaut.yy" // lalr1.cc:919
    {
    if (res.states < 0 || res.cur_state >= (unsigned) res.states)
      break;
    if (res.accset > 0 && (yystack_[0].value.num) < (unsigned) res.accset)
      {
	(yylhs.value.mark) = (yystack_[2].value.mark);
	(yylhs.value.mark).set((yystack_[0].value.num) * 2 + (yystack_[1].value.num));
      }
    else
      {
	std::ostringstream o;
	if (res.accset > 0)
	  {
	    o << "acceptance pairs should be in the range [0.."
	      << res.accset - 1 << "]";
	  }
	else
	  {
	    o << "no acceptance pairs have been declared";
	  }
	error(yystack_[0].location, o.str());
      }
  }
#line 2751 "parseaut.cc" // lalr1.cc:919
    break;

  case 134:
#line 1783 "parseaut.yy" // lalr1.cc:919
    { (yylhs.value.mark) = (yystack_[0].value.mark); }
#line 2757 "parseaut.cc" // lalr1.cc:919
    break;

  case 136:
#line 1787 "parseaut.yy" // lalr1.cc:919
    {
    std::pair<map_t::iterator, bool> i =
      res.dest_map.emplace((yystack_[0].value.num), *res.cur_guard);
    if (!i.second)
      i.first->second |= *res.cur_guard;
    ++res.cur_guard;
  }
#line 2769 "parseaut.cc" // lalr1.cc:919
    break;

  case 139:
#line 1798 "parseaut.yy" // lalr1.cc:919
    {
    for (auto i: res.dest_map)
      res.h->aut->new_edge(res.cur_state, i.first, i.second, (yystack_[1].value.mark));
  }
#line 2778 "parseaut.cc" // lalr1.cc:919
    break;

  case 140:
#line 1808 "parseaut.yy" // lalr1.cc:919
    {
	 if (res.opts.want_kripke)
	   {
	     error(yylhs.location, "cannot read a Kripke structure out of a never claim.");
	     YYABORT;
	   }
	 res.namer = res.h->aut->create_namer<std::string>();
	 res.h->aut->set_buchi();
	 res.acc_style = State_Acc;
	 res.pos_acc_sets = res.h->aut->acc().all_sets();
       }
#line 2794 "parseaut.cc" // lalr1.cc:919
    break;

  case 141:
#line 1820 "parseaut.yy" // lalr1.cc:919
    {
	 // Add an accept_all state if needed.
	 if (res.accept_all_needed && !res.accept_all_seen)
	   {
	     unsigned n = res.namer->new_state("accept_all");
	     res.h->aut->new_acc_edge(n, n, bddtrue);
	   }
	 // If we aliased existing state, we have some unreachable
	 // states to remove.
	 if (res.aliased_states)
	   res.h->aut->purge_unreachable_states();
	 res.info_states.resize(res.h->aut->num_states());
	 // Pretend that we have declared all states.
	 for (auto& p: res.info_states)
	   p.declared = true;
         res.h->aut->register_aps_from_dict();
       }
#line 2816 "parseaut.cc" // lalr1.cc:919
    break;

  case 146:
#line 1844 "parseaut.yy" // lalr1.cc:919
    {
      auto r = res.labels.insert(std::make_pair(*(yystack_[1].value.str), yystack_[1].location));
      if (!r.second)
	{
	  error(yystack_[1].location, "redefinition of "s + *(yystack_[1].value.str) + "...");
	  error(r.first->second, "... "s + *(yystack_[1].value.str) + " previously defined here");
	}
      (yylhs.value.str) = (yystack_[1].value.str);
    }
#line 2830 "parseaut.cc" // lalr1.cc:919
    break;

  case 147:
#line 1855 "parseaut.yy" // lalr1.cc:919
    {
      unsigned n = res.namer->new_state(*(yystack_[0].value.str));
      if (res.start.empty())
	{
	  // The first state is initial.
	  res.start.emplace_back(yylhs.location, std::vector<unsigned>{n});
	}
      (yylhs.value.str) = (yystack_[0].value.str);
    }
#line 2844 "parseaut.cc" // lalr1.cc:919
    break;

  case 148:
#line 1865 "parseaut.yy" // lalr1.cc:919
    {
      res.aliased_states |=
	res.namer->alias_state(res.namer->get_state(*(yystack_[1].value.str)), *(yystack_[0].value.str));
      // Keep any identifier that starts with accept.
      if (strncmp("accept", (yystack_[1].value.str)->c_str(), 6))
        {
          delete (yystack_[1].value.str);
          (yylhs.value.str) = (yystack_[0].value.str);
        }
      else
        {
	  delete (yystack_[0].value.str);
	  (yylhs.value.str) = (yystack_[1].value.str);
        }
    }
#line 2864 "parseaut.cc" // lalr1.cc:919
    break;

  case 149:
#line 1883 "parseaut.yy" // lalr1.cc:919
    {
      (yylhs.value.list) = (yystack_[1].value.list);
    }
#line 2872 "parseaut.cc" // lalr1.cc:919
    break;

  case 150:
#line 1887 "parseaut.yy" // lalr1.cc:919
    {
      (yylhs.value.list) = (yystack_[1].value.list);
    }
#line 2880 "parseaut.cc" // lalr1.cc:919
    break;

  case 151:
#line 1893 "parseaut.yy" // lalr1.cc:919
    {
      if (*(yystack_[1].value.str) == "accept_all")
	res.accept_all_seen = true;

      auto acc = !strncmp("accept", (yystack_[1].value.str)->c_str(), 6) ?
	res.h->aut->acc().all_sets() : spot::acc_cond::mark_t({});
      res.namer->new_edge(*(yystack_[1].value.str), *(yystack_[1].value.str), bddtrue, acc);
      delete (yystack_[1].value.str);
    }
#line 2894 "parseaut.cc" // lalr1.cc:919
    break;

  case 152:
#line 1902 "parseaut.yy" // lalr1.cc:919
    { delete (yystack_[0].value.str); }
#line 2900 "parseaut.cc" // lalr1.cc:919
    break;

  case 153:
#line 1903 "parseaut.yy" // lalr1.cc:919
    { delete (yystack_[1].value.str); }
#line 2906 "parseaut.cc" // lalr1.cc:919
    break;

  case 154:
#line 1905 "parseaut.yy" // lalr1.cc:919
    {
      auto acc = !strncmp("accept", (yystack_[1].value.str)->c_str(), 6) ?
	res.h->aut->acc().all_sets() : spot::acc_cond::mark_t({});
      for (auto& p: *(yystack_[0].value.list))
	{
	  bdd c = bdd_from_int(p.first);
	  bdd_delref(p.first);
	  res.namer->new_edge(*(yystack_[1].value.str), *p.second, c, acc);
	  delete p.second;
	}
      delete (yystack_[1].value.str);
      delete (yystack_[0].value.list);
    }
#line 2924 "parseaut.cc" // lalr1.cc:919
    break;

  case 155:
#line 1920 "parseaut.yy" // lalr1.cc:919
    { (yylhs.value.list) = new std::list<pair>; }
#line 2930 "parseaut.cc" // lalr1.cc:919
    break;

  case 156:
#line 1922 "parseaut.yy" // lalr1.cc:919
    {
      if ((yystack_[0].value.p))
	{
	  (yystack_[1].value.list)->push_back(*(yystack_[0].value.p));
	  delete (yystack_[0].value.p);
	}
      (yylhs.value.list) = (yystack_[1].value.list);
    }
#line 2943 "parseaut.cc" // lalr1.cc:919
    break;

  case 157:
#line 1931 "parseaut.yy" // lalr1.cc:919
    { (yylhs.value.str) = (yystack_[0].value.str); }
#line 2949 "parseaut.cc" // lalr1.cc:919
    break;

  case 158:
#line 1931 "parseaut.yy" // lalr1.cc:919
    { (yylhs.value.str) = (yystack_[0].value.str); }
#line 2955 "parseaut.cc" // lalr1.cc:919
    break;

  case 159:
#line 1934 "parseaut.yy" // lalr1.cc:919
    {
       auto i = res.fcache.find(*(yystack_[0].value.str));
       if (i == res.fcache.end())
	 {
	   auto pf = spot::parse_infix_boolean(*(yystack_[0].value.str), *res.env, debug_level(),
					       true);
	   for (auto& j: pf.errors)
	     {
	       // Adjust the diagnostic to the current position.
	       spot::location here = yystack_[0].location;
	       here.end.line = here.begin.line + j.first.end.line - 1;
	       here.end.column = here.begin.column + j.first.end.column - 1;
	       here.begin.line += j.first.begin.line - 1;
	       here.begin.column += j.first.begin.column - 1;
	       res.h->errors.emplace_back(here, j.second);
	     }
           bdd cond = bddfalse;
	   if (pf.f)
	     cond = spot::formula_to_bdd(pf.f,
					 res.h->aut->get_dict(), res.h->aut);
	   (yylhs.value.b) = (res.fcache[*(yystack_[0].value.str)] = cond).id();
	 }
       else
	 {
	   (yylhs.value.b) = i->second.id();
	 }
       bdd_addref((yylhs.value.b));
       delete (yystack_[0].value.str);
     }
#line 2989 "parseaut.cc" // lalr1.cc:919
    break;

  case 160:
#line 1964 "parseaut.yy" // lalr1.cc:919
    {
       (yylhs.value.b) = 0;
     }
#line 2997 "parseaut.cc" // lalr1.cc:919
    break;

  case 161:
#line 1970 "parseaut.yy" // lalr1.cc:919
    {
      (yylhs.value.str) = nullptr;
    }
#line 3005 "parseaut.cc" // lalr1.cc:919
    break;

  case 162:
#line 1974 "parseaut.yy" // lalr1.cc:919
    {
      (yylhs.value.str) = (yystack_[0].value.str);
    }
#line 3013 "parseaut.cc" // lalr1.cc:919
    break;

  case 163:
#line 1978 "parseaut.yy" // lalr1.cc:919
    {
      delete (yystack_[0].value.str);
      (yylhs.value.str) = new std::string("accept_all");
      res.accept_all_needed = true;
    }
#line 3023 "parseaut.cc" // lalr1.cc:919
    break;

  case 164:
#line 1985 "parseaut.yy" // lalr1.cc:919
    {
      // If there is no destination, do ignore the transition.
      // This happens for instance with
      //   if
      //   :: false
      //   fi
      if (!(yystack_[0].value.str))
	{
	  (yylhs.value.p) = nullptr;
	}
      else
	{
	  (yylhs.value.p) = new pair((yystack_[1].value.b), (yystack_[0].value.str));
	  res.namer->new_state(*(yystack_[0].value.str));
	}
    }
#line 3044 "parseaut.cc" // lalr1.cc:919
    break;

  case 165:
#line 2004 "parseaut.yy" // lalr1.cc:919
    {
      (yylhs.value.p) = (yystack_[1].value.p);
    }
#line 3052 "parseaut.cc" // lalr1.cc:919
    break;

  case 166:
#line 2008 "parseaut.yy" // lalr1.cc:919
    {
      (yylhs.value.p) = (yystack_[0].value.p);
    }
#line 3060 "parseaut.cc" // lalr1.cc:919
    break;

  case 167:
#line 2017 "parseaut.yy" // lalr1.cc:919
    {
	auto& acc = res.h->aut->acc();
	unsigned num = acc.num_sets();
	res.h->aut->set_generalized_buchi(num);
	res.pos_acc_sets = acc.all_sets();
	assert(!res.states_map.empty());
	auto n = res.states_map.size();
	if (n != (unsigned) res.states)
	  {
	    std::ostringstream err;
	    err << res.states << " states have been declared, but "
		<< n << " different state numbers have been used";
	    error(yylhs.location, err.str());
	  }
	if (res.states_map.rbegin()->first > (unsigned) res.states)
	  {
	    // We have seen numbers larger that the total number of
	    // states in the automaton.  Usually this happens when the
	    // states are numbered from 1 instead of 0, but the LBTT
	    // documentation actually allow any number to be used.
	    // What we have done is to map all input state numbers 0
	    // <= .. < n to the digraph states with the same number,
	    // and any time we saw a number larger than n, we mapped
	    // it to a new state.  The correspondence is given by
	    // res.states_map.  Now we just need to remove the useless
	    // states we allocated.
	    std::vector<unsigned> rename(res.h->aut->num_states(), -1U);
	    unsigned s = 0;
	    for (auto& i: res.states_map)
	      rename[i.second] = s++;
	    assert(s == (unsigned) res.states);
	    for (auto& i: res.start)
	      i.second.front() = rename[i.second.front()];
	    res.h->aut->get_graph().defrag_states(std::move(rename), s);
	  }
	 res.info_states.resize(res.h->aut->num_states());
	 for (auto& s: res.info_states)
	   s.declared = true;
         res.h->aut->register_aps_from_dict();
      }
#line 3105 "parseaut.cc" // lalr1.cc:919
    break;

  case 168:
#line 2058 "parseaut.yy" // lalr1.cc:919
    {
        res.h->aut->set_generalized_buchi((yystack_[0].value.num));
	res.pos_acc_sets = res.h->aut->acc().all_sets();
      }
#line 3114 "parseaut.cc" // lalr1.cc:919
    break;

  case 169:
#line 2064 "parseaut.yy" // lalr1.cc:919
    {
		    if (res.opts.want_kripke)
		      {
			error(yylhs.location,
			      "cannot read a Kripke structure out of "
			      "an LBTT automaton");
			YYABORT;
		      }
		    res.states = (yystack_[0].value.num);
		    res.states_loc = yystack_[0].location;
		    res.h->aut->new_states((yystack_[0].value.num));
		  }
#line 3131 "parseaut.cc" // lalr1.cc:919
    break;

  case 170:
#line 2077 "parseaut.yy" // lalr1.cc:919
    {
	     res.acc_mapper = new spot::acc_mapper_int(res.h->aut, (yystack_[0].value.num));
	     res.acc_style = State_Acc;
	   }
#line 3140 "parseaut.cc" // lalr1.cc:919
    break;

  case 171:
#line 2082 "parseaut.yy" // lalr1.cc:919
    {
	     res.acc_mapper = new spot::acc_mapper_int(res.h->aut, (yystack_[0].value.num));
	     res.trans_acc_seen = true;
	   }
#line 3149 "parseaut.cc" // lalr1.cc:919
    break;

  case 175:
#line 2091 "parseaut.yy" // lalr1.cc:919
    {
	    if ((yystack_[2].value.num) >= (unsigned) res.states)
	      {
		auto p = res.states_map.emplace((yystack_[2].value.num), 0);
		if (p.second)
		  p.first->second = res.h->aut->new_state();
		res.cur_state = p.first->second;
	      }
	    else
	      {
		res.states_map.emplace((yystack_[2].value.num), (yystack_[2].value.num));
		res.cur_state = (yystack_[2].value.num);
	      }
	    if ((yystack_[1].value.num))
	      res.start.emplace_back(yystack_[2].location + yystack_[1].location,
                                     std::vector<unsigned>{res.cur_state});
	    res.acc_state = (yystack_[0].value.mark);
	  }
#line 3172 "parseaut.cc" // lalr1.cc:919
    break;

  case 176:
#line 2109 "parseaut.yy" // lalr1.cc:919
    { (yylhs.value.mark) = spot::acc_cond::mark_t({}); }
#line 3178 "parseaut.cc" // lalr1.cc:919
    break;

  case 177:
#line 2111 "parseaut.yy" // lalr1.cc:919
    {
	  (yylhs.value.mark)  = (yystack_[1].value.mark);
	  auto p = res.acc_mapper->lookup((yystack_[0].value.num));
	  if (p.first)
	    (yylhs.value.mark) |= p.second;
	  else
	    error(yystack_[0].location, "more acceptance sets used than declared");
	}
#line 3191 "parseaut.cc" // lalr1.cc:919
    break;

  case 178:
#line 2120 "parseaut.yy" // lalr1.cc:919
    {
	    auto pf = spot::parse_prefix_ltl(*(yystack_[0].value.str), *res.env);
	    if (!pf.f || !pf.errors.empty())
	      {
		std::string s = "failed to parse guard: ";
		s += *(yystack_[0].value.str);
		error(yylhs.location, s);
	      }
	    if (!pf.errors.empty())
	      for (auto& j: pf.errors)
		{
		  // Adjust the diagnostic to the current position.
		  spot::location here = yystack_[0].location;
		  here.end.line = here.begin.line + j.first.end.line - 1;
		  here.end.column = here.begin.column + j.first.end.column - 1;
		  here.begin.line += j.first.begin.line - 1;
		  here.begin.column += j.first.begin.column - 1;
		  res.h->errors.emplace_back(here, j.second);
		}
	    if (!pf.f)
	      {
		res.cur_label = bddtrue;
	      }
	    else
	      {
		if (!pf.f.is_boolean())
		  {
		    error(yylhs.location,
			  "non-Boolean transition label (replaced by true)");
		    res.cur_label = bddtrue;
		  }
		else
		  {
		    res.cur_label =
		      formula_to_bdd(pf.f, res.h->aut->get_dict(), res.h->aut);
		  }
	      }
	    delete (yystack_[0].value.str);
	  }
#line 3235 "parseaut.cc" // lalr1.cc:919
    break;

  case 180:
#line 2161 "parseaut.yy" // lalr1.cc:919
    {
		  unsigned dst = (yystack_[2].value.num);
		  if (dst >= (unsigned) res.states)
		    {
		      auto p = res.states_map.emplace(dst, 0);
		      if (p.second)
			p.first->second = res.h->aut->new_state();
		      dst = p.first->second;
		    }
		  else
		    {
		      res.states_map.emplace(dst, dst);
		    }
		  res.h->aut->new_edge(res.cur_state, dst,
				       res.cur_label,
				       res.acc_state | (yystack_[1].value.mark));
		}
#line 3257 "parseaut.cc" // lalr1.cc:919
    break;


#line 3261 "parseaut.cc" // lalr1.cc:919
            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const short parser::yypact_ninf_ = -165;

  const signed char parser::yytable_ninf_ = -123;

  const short
  parser::yypact_[] =
  {
      17,  -165,    42,    14,  -165,  -165,  -165,  -165,    68,  -165,
    -165,     8,  -165,  -165,     6,  -165,  -165,    52,  -165,  -165,
    -165,    69,    70,    58,  -165,  -165,   130,    75,    97,  -165,
    -165,  -165,    87,    79,  -165,  -165,  -165,   139,   144,   105,
    -165,   137,   138,   140,   141,   142,   145,   146,   147,  -165,
    -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,   143,  -165,
     102,    85,  -165,    92,  -165,  -165,  -165,   112,  -165,    -3,
    -165,  -165,   148,   149,  -165,  -165,  -165,  -165,   150,  -165,
      -5,  -165,  -165,    16,   152,    89,  -165,   118,  -165,  -165,
     139,  -165,  -165,  -165,  -165,  -165,  -165,     3,  -165,   138,
     151,  -165,   -20,  -165,   138,  -165,    -3,  -165,   113,    -3,
    -165,   138,   138,  -165,    60,    -6,    55,  -165,  -165,  -165,
     155,   153,   154,  -165,  -165,  -165,  -165,  -165,  -165,  -165,
    -165,   156,   161,   162,  -165,   124,  -165,  -165,    27,    47,
     119,  -165,  -165,    60,  -165,  -165,    60,    66,   150,   138,
     138,     2,  -165,  -165,   -20,   113,   113,  -165,  -165,  -165,
     138,  -165,  -165,  -165,  -165,   164,    -2,   131,  -165,  -165,
      -6,   121,  -165,  -165,  -165,  -165,   165,   166,    21,  -165,
    -165,  -165,  -165,     5,  -165,   125,  -165,  -165,  -165,  -165,
      56,    60,    60,  -165,   113,  -165,  -165,   132,   -14,  -165,
    -165,  -165,  -165,  -165,  -165,   122,    72,    -6,    -6,  -165,
    -165,  -165,   168,  -165,   163,  -165,  -165,    63,  -165,   167,
    -165,  -165,  -165,  -165,  -165,  -165,  -165,   171,   157,  -165,
     169,  -165,   150,  -165,  -165,  -165,  -165,   135,  -165,  -165,
     158,  -165,   159,  -165,  -165,    94,   173,    81,   110,  -165,
    -165,  -165,  -165,   174,  -165,   160,   181,   170,  -165,  -165,
    -165,  -165
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,     3,     0,     0,   120,   121,   140,   169,     0,     2,
       6,     0,    22,     9,     0,     7,     8,     0,   173,     4,
       5,     0,     0,     0,     1,    84,     0,     0,     0,   171,
     170,   168,     0,   172,    11,    17,    19,   142,     0,     0,
      41,     0,     0,     0,     0,     0,     0,     0,     0,    49,
      36,    38,    56,    27,    23,   119,   123,   167,     0,   179,
       0,     0,   147,   152,   143,    10,    88,    92,    85,   103,
      24,    82,    25,    26,    20,    28,    30,    45,    12,    34,
      35,    52,    54,    40,     0,     0,   176,   174,   146,   141,
     145,   151,   155,   155,   153,   148,   154,     0,    93,     0,
     111,    83,   101,    94,     0,    86,   104,   105,   101,    87,
     112,     0,     0,    42,     0,     0,    32,    13,    33,    50,
       0,    37,    39,    60,    59,    58,    14,    15,    57,   137,
     124,     0,     0,     0,   128,   175,   176,   144,     0,     0,
       0,    68,    67,     0,    65,    66,     0,     0,    12,     0,
       0,     0,   102,   115,   101,   101,   101,   107,   106,   116,
       0,   113,   114,    64,    63,    21,    29,     0,    79,    80,
       0,    31,    48,    47,    46,    51,     0,     0,     0,   126,
     127,   125,   177,     0,   149,     0,   156,   150,    91,    69,
       0,     0,     0,    90,    99,    62,    61,     0,     0,   109,
     110,   108,   117,    44,    43,     0,     0,     0,     0,    53,
      55,   138,     0,   118,     0,   178,   180,     0,    72,    71,
      70,   100,    89,    96,    73,    95,    98,     0,     0,    76,
      78,    77,    12,   132,   135,   158,   160,     0,   157,   159,
     161,   166,     0,    74,   129,   134,   139,     0,     0,   164,
      75,   130,   131,     0,   136,     0,     0,     0,   133,   165,
     162,   163
  };

  const short
  parser::yypgoto_[] =
  {
    -165,  -165,   199,  -165,  -148,    86,  -165,  -165,  -165,  -165,
     120,  -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,
    -165,  -165,  -165,  -165,  -165,  -165,  -165,  -113,  -129,  -164,
    -165,   -40,   -90,  -165,  -165,  -165,   136,  -165,    95,    12,
    -165,  -165,  -103,  -165,  -165,  -165,   101,   104,  -165,   -99,
    -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,
    -165,  -165,  -165,  -165,   172,  -165,  -165,   126,   116,  -165,
    -165,  -165,   -37,  -165,  -165,  -165,  -165,  -165,  -165,  -165,
      76,  -165,  -165
  };

  const short
  parser::yydefgoto_[] =
  {
      -1,     8,     9,    10,   118,   128,    11,    36,    12,    22,
      53,   113,    26,    54,   114,   115,    81,    82,   165,   204,
     116,    80,   121,   122,    83,   100,    72,   147,   226,   171,
      38,   101,   102,    39,    68,    69,   103,    99,   104,   152,
     198,   222,   153,   105,   106,   157,   107,   108,   109,   110,
     162,    13,    14,    84,    85,   214,   253,   245,   234,   246,
     178,    15,    23,    61,    62,    63,    96,    64,   138,   239,
     240,   249,   241,   186,    16,    17,    18,    32,    33,    59,
     135,   216,    87
  };

  const short
  parser::yytable_[] =
  {
     194,   166,    73,   197,   140,   159,   206,    27,   224,   148,
     161,   150,   167,   119,   154,    21,   156,     1,     2,    71,
       3,    25,   211,   141,   -97,   142,   215,   120,   191,   192,
     189,    28,   -18,   190,   123,   143,   212,   124,   125,   151,
       4,     5,    19,   230,   231,     3,   225,   168,   169,   170,
       6,   199,   200,   201,    97,   182,   144,   145,   146,   195,
     196,   202,   -97,   184,     7,     4,     5,   213,    24,   126,
     127,   163,   164,   172,    29,     6,   228,   173,   219,   220,
     141,   235,   142,    34,   244,   187,   191,   192,    35,     7,
     130,   185,   143,   131,   132,    43,   191,   192,   242,   235,
      30,    31,   207,   208,   236,   237,    66,   238,   126,   127,
      60,   185,   218,   144,   145,   146,   133,    37,  -122,   -81,
      67,    55,   236,    56,   193,   238,    91,    92,   229,    93,
      58,    40,    57,    94,    41,    42,    43,    44,    45,    46,
      47,    48,    49,   -16,   224,    89,    50,    51,    90,    52,
     256,   207,   208,   257,   227,   251,   252,    60,    65,    70,
      71,    75,    74,    77,    76,    86,    88,    78,    79,    97,
     136,   117,   151,   175,   182,   176,   177,   188,   179,   111,
     112,   129,   149,   180,   181,   203,   205,   209,   210,   217,
     232,   233,   223,   224,   247,   254,   258,   248,   192,   260,
     208,    20,   174,    98,   160,   134,   221,   158,   155,   139,
     255,     0,   183,   243,   261,   250,   137,     0,     0,     0,
     259,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    95
  };

  const short
  parser::yycheck_[] =
  {
     148,   114,    42,     1,     1,   108,   170,     1,    22,    99,
     109,    31,    18,    18,   104,     1,   106,     0,     1,    22,
       3,    13,     1,    20,    22,    22,    21,    32,    30,    31,
     143,    25,    18,   146,    18,    32,    15,    21,    22,    59,
      23,    24,     0,   207,   208,     3,    60,    53,    54,    55,
      33,   154,   155,   156,    57,    50,    53,    54,    55,   149,
     150,   160,    60,    36,    47,    23,    24,    46,     0,    53,
      54,   111,   112,    18,    22,    33,   205,    22,   191,   192,
      20,    18,    22,    14,   232,    38,    30,    31,    18,    47,
       1,    64,    32,     4,     5,     6,    30,    31,   227,    18,
      48,    49,    30,    31,    41,    42,     1,    44,    53,    54,
      18,    64,    56,    53,    54,    55,    27,    59,    29,    14,
      15,    46,    41,    26,    58,    44,    34,    35,    56,    37,
      51,     1,    45,    41,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    22,    60,    16,    17,    63,    19,
      40,    30,    31,    43,    32,    61,    62,    18,    14,    22,
      22,    20,    22,    18,    22,    22,    64,    21,    21,    57,
      52,    21,    59,    18,    50,    22,    22,    58,    22,    31,
      31,    29,    31,    22,    22,    21,    55,    22,    22,    64,
      22,    28,    60,    22,    59,    22,    22,    39,    31,    18,
      31,     2,   116,    67,   109,    85,   194,   106,   104,    93,
     247,    -1,   136,    56,    44,    56,    90,    -1,    -1,    -1,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    63
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,     0,     1,     3,    23,    24,    33,    47,    66,    67,
      68,    71,    73,   116,   117,   126,   139,   140,   141,     0,
      67,     1,    74,   127,     0,    13,    77,     1,    25,    22,
      48,    49,   142,   143,    14,    18,    72,    59,    95,    98,
       1,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      16,    17,    19,    75,    78,    46,    26,    45,    51,   144,
      18,   128,   129,   130,   132,    14,     1,    15,    99,   100,
      22,    22,    91,    96,    22,    20,    22,    18,    21,    21,
      86,    81,    82,    89,   118,   119,    22,   147,    64,    60,
      63,    34,    35,    37,    41,   129,   131,    57,   101,   102,
      90,    96,    97,   101,   103,   108,   109,   111,   112,   113,
     114,    31,    31,    76,    79,    80,    85,    21,    69,    18,
      32,    87,    88,    18,    21,    22,    53,    54,    70,    29,
       1,     4,     5,    27,    75,   145,    52,   132,   133,   133,
       1,    20,    22,    32,    53,    54,    55,    92,    97,    31,
      31,    59,   104,   107,    97,   112,    97,   110,   111,   107,
     103,   114,   115,    96,    96,    83,    92,    18,    53,    54,
      55,    94,    18,    22,    70,    18,    22,    22,   125,    22,
      22,    22,    50,   145,    36,    64,   138,    38,    58,    92,
      92,    30,    31,    58,    69,    97,    97,     1,   105,   107,
     107,   107,   114,    21,    84,    55,    94,    30,    31,    22,
      22,     1,    15,    46,   120,    21,   146,    64,    56,    92,
      92,   104,   106,    60,    22,    60,    93,    32,    93,    56,
      94,    94,    22,    28,   123,    18,    41,    42,    44,   134,
     135,   137,    93,    56,    69,   122,   124,    59,    39,   136,
      56,    61,    62,   121,    22,   137,    40,    43,    22,    60,
      18,    44
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    65,    66,    66,    66,    66,    67,    67,    67,    67,
      68,    68,    69,    69,    70,    70,    71,    72,    74,    73,
      76,    75,    77,    77,    78,    78,    78,    78,    79,    78,
      80,    78,    78,    78,    78,    78,    81,    78,    82,    78,
      78,    78,    83,    83,    84,    85,    85,    85,    85,    86,
      86,    86,    87,    87,    88,    88,    89,    89,    89,    89,
      89,    90,    90,    91,    91,    92,    92,    92,    92,    92,
      92,    92,    92,    93,    94,    94,    94,    94,    94,    94,
      94,    95,    96,    97,    98,    98,    99,    99,    99,   100,
     101,   101,   102,   102,   103,   104,   104,   105,   105,   106,
     106,   107,   107,   108,   108,   109,   109,   109,   110,   111,
     111,   112,   113,   113,   113,   114,   114,   115,   116,   116,
     117,   117,   118,   119,   119,   119,   119,   119,   119,   120,
     121,   121,   122,   122,   123,   124,   124,   125,   125,   125,
     127,   126,   128,   128,   128,   128,   129,   130,   130,   131,
     131,   132,   132,   132,   132,   133,   133,   134,   134,   135,
     135,   136,   136,   136,   137,   138,   138,   139,   139,   140,
     141,   141,   142,   143,   143,   144,   145,   145,   146,   147,
     147
  };

  const unsigned char
  parser::yyr2_[] =
  {
       0,     2,     1,     1,     2,     2,     1,     1,     1,     1,
       4,     3,     0,     1,     1,     1,     2,     1,     0,     3,
       0,     4,     0,     2,     2,     2,     2,     1,     0,     4,
       0,     4,     3,     3,     2,     2,     0,     3,     0,     3,
       2,     1,     0,     2,     1,     0,     2,     2,     2,     0,
       2,     3,     0,     3,     0,     3,     0,     2,     2,     2,
       2,     3,     3,     3,     3,     1,     1,     1,     1,     2,
       3,     3,     3,     1,     4,     5,     3,     3,     3,     1,
       1,     1,     1,     1,     0,     2,     2,     2,     1,     5,
       3,     3,     0,     1,     1,     3,     3,     0,     2,     0,
       1,     0,     1,     0,     1,     1,     2,     2,     2,     3,
       3,     1,     1,     2,     2,     2,     2,     2,     7,     3,
       1,     1,     1,     0,     2,     3,     3,     3,     2,     3,
       1,     1,     0,     3,     2,     0,     2,     0,     2,     4,
       0,     5,     0,     1,     3,     2,     2,     1,     2,     3,
       3,     2,     1,     2,     2,     0,     2,     1,     1,     1,
       1,     0,     3,     3,     2,     6,     3,     3,     2,     1,
       2,     2,     1,     0,     3,     3,     0,     2,     1,     0,
       4
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "\"HOA:\"", "\"States:\"",
  "\"Start:\"", "\"AP:\"", "\"Alias:\"", "\"Acceptance:\"",
  "\"acc-name:\"", "\"tool:\"", "\"name:\"", "\"properties:\"",
  "\"--BODY--\"", "\"--END--\"", "\"State:\"", "\"spot.highlight.edges:\"",
  "\"spot.highlight.states:\"", "\"identifier\"", "\"header name\"",
  "\"alias name\"", "\"string\"", "\"integer\"", "\"DRA\"", "\"DSA\"",
  "\"v2\"", "\"explicit\"", "\"Acceptance-Pairs:\"", "\"Acc-Sig:\"",
  "\"---\"", "'|'", "'&'", "'!'", "\"never\"", "\"skip\"", "\"if\"",
  "\"fi\"", "\"do\"", "\"od\"", "\"->\"", "\"goto\"", "\"false\"",
  "\"atomic\"", "\"assert\"", "\"boolean formula\"", "\"-1\"",
  "\"end of DSTAR automaton\"", "\"LBTT header\"", "\"state acceptance\"",
  "\"acceptance sets for empty automaton\"", "\"acceptance set\"",
  "\"state number\"", "\"destination number\"", "'t'", "'f'", "'('", "')'",
  "'['", "']'", "'{'", "'}'", "'+'", "'-'", "';'", "':'", "$accept", "aut",
  "aut-1", "hoa", "string_opt", "BOOLEAN", "header", "version",
  "format-version", "$@1", "aps", "$@2", "header-items", "header-item",
  "$@3", "$@4", "$@5", "$@6", "ap-names", "ap-name", "acc-spec",
  "properties", "highlight-edges", "highlight-states", "header-spec",
  "state-conj-2", "init-state-conj-2", "label-expr", "acc-set",
  "acceptance-cond", "body", "state-num", "checked-state-num", "states",
  "state", "state-name", "label", "state-label_opt", "trans-label",
  "acc-sig", "acc-sets", "state-acc_opt", "trans-acc_opt", "labeled-edges",
  "some-labeled-edges", "incorrectly-unlabeled-edge", "labeled-edge",
  "state-conj-checked", "unlabeled-edges", "unlabeled-edge",
  "incorrectly-labeled-edge", "dstar", "dstar_type", "dstar_header",
  "dstar_sizes", "dstar_state_id", "sign", "dstar_accsigs",
  "dstar_state_accsig", "dstar_transitions", "dstar_states", "never",
  "$@7", "nc-states", "nc-one-ident", "nc-ident-list",
  "nc-transition-block", "nc-state", "nc-transitions",
  "nc-formula-or-ident", "nc-formula", "nc-opt-dest", "nc-src-dest",
  "nc-transition", "lbtt", "lbtt-header-states", "lbtt-header",
  "lbtt-body", "lbtt-states", "lbtt-state", "lbtt-acc", "lbtt-guard",
  "lbtt-transitions", YY_NULLPTR
  };

#if HOAYYDEBUG
  const unsigned short
  parser::yyrline_[] =
  {
       0,   337,   337,   338,   339,   340,   347,   348,   349,   350,
     356,   357,   359,   360,   361,   361,   363,   667,   674,   674,
     677,   676,   729,   730,   731,   749,   754,   758,   759,   759,
     772,   771,   805,   809,   814,   818,   820,   819,   823,   822,
     825,   834,   836,   837,   838,   866,   867,   868,   869,   873,
     874,   895,   910,   911,   915,   916,   921,   922,   923,   924,
     928,   933,   937,   945,   949,   955,   959,   963,   992,  1007,
    1013,  1020,  1027,  1033,  1051,  1079,  1097,  1101,  1107,  1113,
    1117,  1126,  1200,  1210,  1253,  1254,  1293,  1294,  1318,  1328,
    1358,  1363,  1368,  1369,  1391,  1418,  1429,  1433,  1437,  1445,
    1449,  1460,  1464,  1478,  1479,  1480,  1481,  1482,  1483,  1501,
    1512,  1525,  1540,  1541,  1542,  1543,  1574,  1605,  1616,  1617,
    1622,  1634,  1647,  1671,  1672,  1673,  1691,  1704,  1708,  1710,
    1750,  1751,  1754,  1758,  1783,  1785,  1786,  1795,  1796,  1797,
    1808,  1807,  1838,  1839,  1840,  1841,  1843,  1854,  1864,  1882,
    1886,  1892,  1902,  1903,  1904,  1920,  1921,  1931,  1931,  1933,
    1963,  1969,  1973,  1977,  1984,  2003,  2007,  2016,  2057,  2063,
    2076,  2081,  2086,  2087,  2088,  2090,  2109,  2110,  2119,  2159,
    2160
  };

  // Print the state stack on the debug stream.
  void
  parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  parser::yy_reduce_print_ (int yyrule)
  {
    unsigned yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // HOAYYDEBUG

  parser::token_number_type
  parser::yytranslate_ (int t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const token_number_type
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    32,     2,     2,     2,     2,    31,     2,
      55,    56,     2,    61,     2,    62,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    64,    63,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    57,     2,    58,     2,     2,     2,     2,     2,     2,
       2,     2,    54,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    53,     2,     2,     2,
       2,     2,     2,    59,    30,    60,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52
    };
    const unsigned user_token_number_max_ = 304;
    const token_number_type undef_token_ = 2;

    if (static_cast<int> (t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // hoayy
#line 3915 "parseaut.cc" // lalr1.cc:1242
#line 2179 "parseaut.yy" // lalr1.cc:1243


static void fill_guards(result_& r)
{
  unsigned nap = r.ap.size();

  int* vars = new int[nap];
  for (unsigned i = 0; i < nap; ++i)
    vars[i] = r.ap[nap - 1 - i];

  // build the 2^nap possible guards
  r.guards.reserve(1U << nap);
  for (size_t i = 0; i < (1U << nap); ++i)
    r.guards.push_back(bdd_ibuildcube(i, nap, vars));
  r.cur_guard = r.guards.begin();

  delete[] vars;
}

void
hoayy::parser::error(const location_type& location,
		     const std::string& message)
{
  res.h->errors.emplace_back(location, message);
}

static spot::acc_cond::acc_code
fix_acceptance_aux(spot::acc_cond& acc,
		   spot::acc_cond::acc_code in, unsigned pos,
		   spot::acc_cond::mark_t onlyneg,
		   spot::acc_cond::mark_t both,
		   unsigned base)
{
  auto& w = in[pos];
  switch (w.sub.op)
    {
    case spot::acc_cond::acc_op::And:
      {
	unsigned sub = pos - w.sub.size;
	--pos;
	auto c = fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	pos -= in[pos].sub.size;
	while (sub < pos)
	  {
	    --pos;
	    c &= fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	    pos -= in[pos].sub.size;
	  }
	return c;
      }
    case spot::acc_cond::acc_op::Or:
      {
	unsigned sub = pos - w.sub.size;
	--pos;
	auto c = fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	pos -= in[pos].sub.size;
	while (sub < pos)
	  {
	    --pos;
	    c |= fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	    pos -= in[pos].sub.size;
	  }
	return c;
      }
    case spot::acc_cond::acc_op::Inf:
      return acc.inf(in[pos - 1].mark);
    case spot::acc_cond::acc_op::Fin:
      return acc.fin(in[pos - 1].mark);
    case spot::acc_cond::acc_op::FinNeg:
      {
	auto m = in[pos - 1].mark;
	auto c = acc.fin(onlyneg & m);
	spot::acc_cond::mark_t tmp = {};
	for (auto i: both.sets())
	  {
	    if (m.has(i))
	      tmp.set(base);
	    ++base;
	  }
	if (tmp)
	  c |= acc.fin(tmp);
	return c;
      }
    case spot::acc_cond::acc_op::InfNeg:
      {
	auto m = in[pos - 1].mark;
	auto c = acc.inf(onlyneg & m);
	spot::acc_cond::mark_t tmp = {};
	for (auto i: both.sets())
	  {
	    if (m.has(i))
	      tmp.set(base);
	    ++base;
	  }
	if (tmp)
	  c &= acc.inf(tmp);
	return c;
      }
    }
  SPOT_UNREACHABLE();
  return {};
}

static void fix_acceptance(result_& r)
{
  if (r.opts.want_kripke)
    return;
  auto& acc = r.h->aut->acc();

  // If a set x appears only as Inf(!x), we can complement it so that
  // we work with Inf(x) instead.
  auto onlyneg = r.neg_acc_sets - r.pos_acc_sets;
  if (onlyneg)
    {
      for (auto& t: r.h->aut->edge_vector())
	t.acc ^= onlyneg;
    }

  // However if set x is used elsewhere, for instance in
  //   Inf(!x) & Inf(x)
  // complementing x would be wrong.  We need to create a
  // new set, y, that is the complement of x, and rewrite
  // this as Inf(y) & Inf(x).
  auto both = r.neg_acc_sets & r.pos_acc_sets;
  unsigned base = 0;
  if (both)
    {
      base = acc.add_sets(both.count());
      for (auto& t: r.h->aut->edge_vector())
        {
          unsigned i = 0;
	  if ((t.acc & both) != both)
            for (unsigned v : both.sets())
              {
                if (!t.acc.has(v))
                  t.acc |= acc.mark(base + i);
                i++;
              }
        }
    }

  if (onlyneg || both)
    {
      auto& acc = r.h->aut->acc();
      auto code = acc.get_acceptance();
      r.h->aut->set_acceptance(acc.num_sets(),
			       fix_acceptance_aux(acc, code, code.size() - 1,
						  onlyneg, both, base));
    }
}

// Spot only supports a single initial state.
//
// If the input file does not declare any initial state (this is valid
// in the HOA format) add one nonetheless.
//
// If the input file has multiple initial states we have to merge
// them.
//
//   1) In the non-alternating case, this is as simple as making a new
//   initial state using the union of all the outgoing transitions of
//   the declared initial states.  Note that if one of the original
//   initial state has no incoming transition, then we can use it as
//   initial state, avoiding the creation of a new state.
//
//   2) In the alternating case, the input may have several initial
//   states that are conjuncts.  We have to reduce the conjuncts to a
//   single state first.

static void fix_initial_state(result_& r)
{
  std::vector<std::vector<unsigned>> start;
  start.reserve(r.start.size());
  unsigned ssz = r.info_states.size();
  for (auto& p : r.start)
    {
      std::vector<unsigned> v;
      v.reserve(p.second.size());
      for (unsigned s: p.second)
        // Ignore initial states without declaration
        // (They have been diagnosed already.)
        if (s < ssz && r.info_states[s].declared)
          v.emplace_back(s);
      if (!v.empty())
        start.push_back(v);
    }

  // If no initial state has been declared, add one.
  if (start.empty())
    {
      if (r.opts.want_kripke)
	r.h->ks->set_init_state(r.h->ks->new_state(bddfalse));
      else
	r.h->aut->set_init_state(r.h->aut->new_state());
      return;
    }

  // Remove any duplicate initial state.
  std::sort(start.begin(), start.end());
  auto res = std::unique(start.begin(), start.end());
  start.resize(std::distance(start.begin(), res));

  assert(start.size() >= 1);
  if (start.size() == 1)
    {
      if (r.opts.want_kripke)
	r.h->ks->set_init_state(start.front().front());
      else
	r.h->aut->set_univ_init_state(start.front().begin(),
                                      start.front().end());
    }
  else
    {
      if (r.opts.want_kripke)
	{
	  r.h->errors.emplace_front(r.start.front().first,
				    "Kripke structure only support "
				    "a single initial state");
	  return;
	}
      // Fiddling with initial state may turn an incomplete automaton
      // into a complete one.
      if (r.complete.is_false())
        r.complete = spot::trival::maybe();
      // Multiple initial states.  We might need to add a fake one,
      // unless one of the actual initial state has no incoming edge.
      auto& aut = r.h->aut;
      std::vector<unsigned char> has_incoming(aut->num_states(), 0);
      for (auto& t: aut->edges())
        for (unsigned ud: aut->univ_dests(t))
          has_incoming[ud] = 1;

      bool found = false;
      unsigned init = 0;
      bool init_alternation = false;
      for (auto& pp: start)
        if (pp.size() == 1)
          {
            unsigned p = pp.front();
            if (!has_incoming[p])
              {
                init = p;
                found = true;
              }
          }
        else
          {
            init_alternation = true;
            break;
          }

      if (!found || init_alternation)
	// We do need a fake initial state
	init = aut->new_state();
      aut->set_init_state(init);

      // The non-alternating case is the easiest, we simply declare
      // the outgoing transitions of all "original initial states"
      // into the only one initial state.
      if (!init_alternation)
        {
          for (auto& pp: start)
            {
              unsigned p = pp.front();
              if (p != init)
                for (auto& t: aut->out(p))
                  aut->new_edge(init, t.dst, t.cond);
            }
        }
      else
        {
          // In the alternating case, we merge outgoing transition of
          // the universal destination of conjunct initial states.
          // (Note that this loop would work for the non-alternating
          // case too, but it is more expansive, so we avoid it if we
          // can.)
          spot::outedge_combiner combiner(aut);
          bdd comb_or = bddfalse;
          for (auto& pp: start)
            {
              bdd comb_and = bddtrue;
              for (unsigned d: pp)
                comb_and &= combiner(d);
              comb_or |= comb_and;
            }
          combiner.new_dests(init, comb_or);
        }
    }
}

static void fix_properties(result_& r)
{
  r.aut_or_ks->prop_universal(r.universal);
  r.aut_or_ks->prop_complete(r.complete);
  if (r.acc_style == State_Acc ||
      (r.acc_style == Mixed_Acc && !r.trans_acc_seen))
    r.aut_or_ks->prop_state_acc(true);
}

static void check_version(const result_& r)
{
  if (r.h->type != spot::parsed_aut_type::HOA)
    return;
  auto& v = r.format_version;
  if (v.size() < 2 || v[0] != 'v' || v[1] < '1' || v[1] > '9')
    {
      r.h->errors.emplace_front(r.format_version_loc, "unknown HOA version");
      return;
    }
  const char* beg = &v[1];
  char* end = nullptr;
  long int vers = strtol(beg, &end, 10);
  if (vers != 1)
    {
      r.h->errors.emplace_front(r.format_version_loc,
				  "unsupported HOA version");
      return;
    }
  constexpr const char supported[] = "1";
  if (strverscmp(supported, beg) < 0 && !r.h->errors.empty())
    {
      std::ostringstream s;
      s << "we can read HOA v" << supported
	<< " but this file uses " << v << "; this might "
	"cause the following errors";
      r.h->errors.emplace_front(r.format_version_loc, s.str());
      return;
    }
}

namespace spot
{
  automaton_stream_parser::automaton_stream_parser(const std::string& name,
						   automaton_parser_options opt)
  try
    : filename_(name), opts_(opt)
  {
    if (hoayyopen(name, &scanner_))
      throw std::runtime_error("Cannot open file "s + name);
  }
  catch (...)
  {
    hoayyclose(scanner_);
    throw;
  }

  automaton_stream_parser::automaton_stream_parser(int fd,
						   const std::string& name,
						   automaton_parser_options opt)
  try
    : filename_(name), opts_(opt)
  {
    if (hoayyopen(fd, &scanner_))
      throw std::runtime_error("Cannot open file "s + name);
  }
  catch (...)
  {
    hoayyclose(scanner_);
    throw;
  }

  automaton_stream_parser::automaton_stream_parser(const char* data,
						   const std::string& filename,
						   automaton_parser_options opt)
  try
    : filename_(filename), opts_(opt)
  {
    hoayystring(data, &scanner_);
  }
  catch (...)
  {
    hoayyclose(scanner_);
    throw;
  }

  automaton_stream_parser::~automaton_stream_parser()
  {
    hoayyclose(scanner_);
  }

  static void raise_parse_error(const parsed_aut_ptr& pa)
  {
    if (pa->aborted)
      pa->errors.emplace_back(pa->loc, "parsing aborted");
    if (!pa->errors.empty())
      {
	std::ostringstream s;
	if (pa->format_errors(s))
	  throw parse_error(s.str());
      }
    // It is possible that pa->aut == nullptr if we reach the end of a
    // stream.  It is not necessarily an error.
  }

  parsed_aut_ptr
  automaton_stream_parser::parse(const bdd_dict_ptr& dict,
				 environment& env)
  {
  restart:
    result_ r;
    r.opts = opts_;
    r.h = std::make_shared<spot::parsed_aut>(filename_);
    if (opts_.want_kripke)
      r.aut_or_ks = r.h->ks = make_kripke_graph(dict);
    else
      r.aut_or_ks = r.h->aut = make_twa_graph(dict);
    r.env = &env;
    hoayy::parser parser(scanner_, r, last_loc);
    static bool env_debug = !!getenv("SPOT_DEBUG_PARSER");
    parser.set_debug_level(opts_.debug || env_debug);
    hoayyreset(scanner_);
    try
      {
	if (parser.parse())
	  {
	    r.h->aut = nullptr;
	    r.h->ks = nullptr;
	    r.aut_or_ks = nullptr;
	  }
      }
    catch (const spot::hoa_abort& e)
      {
	r.h->aborted = true;
	// Bison 3.0.2 lacks a += operator for locations.
	r.h->loc = r.h->loc + e.pos;
      }
    check_version(r);
    last_loc = r.h->loc;
    last_loc.step();
    if (r.h->aborted)
      {
	if (opts_.ignore_abort)
	  goto restart;
	return r.h;
      }
    if (opts_.raise_errors)
      raise_parse_error(r.h);
    if (!r.aut_or_ks)
      return r.h;
    if (r.state_names)
      r.aut_or_ks->set_named_prop("state-names", r.state_names);
    if (r.highlight_edges)
      r.aut_or_ks->set_named_prop("highlight-edges", r.highlight_edges);
    if (r.highlight_states)
      r.aut_or_ks->set_named_prop("highlight-states", r.highlight_states);
    fix_acceptance(r);
    fix_initial_state(r);
    fix_properties(r);
    if (r.h->aut && !r.h->aut->is_existential())
      r.h->aut->merge_univ_dests();
    return r.h;
  }

  parsed_aut_ptr
  parse_aut(const std::string& filename, const bdd_dict_ptr& dict,
	    environment& env, automaton_parser_options opts)
  {
    auto localopts = opts;
    localopts.raise_errors = false;
    parsed_aut_ptr pa;
    try
      {
	automaton_stream_parser p(filename, localopts);
	pa = p.parse(dict, env);
      }
    catch (const std::runtime_error& e)
      {
	if (opts.raise_errors)
	  throw;
	parsed_aut_ptr pa = std::make_shared<spot::parsed_aut>(filename);
	pa->errors.emplace_back(spot::location(), e.what());
	return pa;
      }
    if (!pa->aut && !pa->ks && pa->errors.empty())
      pa->errors.emplace_back(pa->loc, "no automaton read (empty input?)");
    if (opts.raise_errors)
      raise_parse_error(pa);
    return pa;
  }


}

// Local Variables:
// mode: c++
// End:
