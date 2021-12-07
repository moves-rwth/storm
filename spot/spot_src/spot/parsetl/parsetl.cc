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
#define yylex   tlyylex



#include "parsetl.hh"


// Unqualified %code blocks.
#line 59 "parsetl.yy" // lalr1.cc:435

/* parsetl.hh and parsedecl.hh include each other recursively.
   We mut ensure that YYSTYPE is declared (by the above %union)
   before parsedecl.hh uses it. */
#include <spot/parsetl/parsedecl.hh>
using namespace spot;

#define missing_right_op_msg(op, str)		\
  error_list.emplace_back(op,			\
    "missing right operand for \"" str "\"");

#define missing_right_op(res, op, str)		\
  do						\
    {						\
      missing_right_op_msg(op, str);		\
      res = fnode::ff();		\
    }						\
  while (0);

// right is missing, so complain and use left.
#define missing_right_binop(res, left, op, str)	\
  do						\
    {						\
      missing_right_op_msg(op, str);		\
      res = left;				\
    }						\
  while (0);

// right is missing, so complain and use false.
#define missing_right_binop_hard(res, left, op, str)	\
  do							\
    {							\
      left->destroy();					\
      missing_right_op(res, op, str);			\
    }							\
  while (0);

  static bool
  sere_ensure_bool(const fnode* f, const spot::location& loc,
                   const char* oper, spot::parse_error_list& error_list)
  {
    if (f->is_boolean())
      return true;
    std::string s;
    s.reserve(80);
    s = "not a Boolean expression: in a SERE ";
    s += oper;
    s += " can only be applied to a Boolean expression";
    error_list.emplace_back(loc, s);
    return false;
  }

  static const fnode*
  error_false_block(const spot::location& loc,
                    spot::parse_error_list& error_list)
  {
    error_list.emplace_back(loc, "treating this block as false");
    return fnode::ff();
  }

  static const fnode*
  parse_ap(const std::string& str,
           const spot::location& location,
           spot::environment& env,
           spot::parse_error_list& error_list)
  {
    auto res = env.require(str);
    if (!res)
      {
        std::string s;
        s.reserve(64);
        s = "unknown atomic proposition `";
        s += str;
        s += "' in ";
        s += env.name();
        error_list.emplace_back(location, s);
      }
    return res.to_node_();
  }

  enum parser_type { parser_ltl, parser_bool, parser_sere };

  static const fnode*
  try_recursive_parse(const std::string& str,
		      const spot::location& location,
		      spot::environment& env,
		      bool debug,
		      parser_type type,
		      spot::parse_error_list& error_list)
    {
      // We want to parse a U (b U c) as two until operators applied
      // to the atomic propositions a, b, and c.  We also want to
      // parse a U (b == c) as one until operator applied to the
      // atomic propositions "a" and "b == c".  The only problem is
      // that we do not know anything about "==" or in general about
      // the syntax of atomic proposition of our users.
      //
      // To support that, the lexer will return "b U c" and "b == c"
      // as PAR_BLOCK tokens.  We then try to parse such tokens
      // recursively.  If, as in the case of "b U c", the block is
      // successfully parsed as a formula, we return this formula.
      // Otherwise, we convert the string into an atomic proposition
      // (it's up to the environment to check the syntax of this
      // proposition, and maybe reject it).

      if (str.empty())
	{
	  error_list.emplace_back(location, "unexpected empty block");
	  return nullptr;
	}

      spot::parsed_formula pf;
      switch (type)
	{
	case parser_sere:
	  pf = spot::parse_infix_sere(str, env, debug, true);
	  break;
	case parser_bool:
	  pf = spot::parse_infix_boolean(str, env, debug, true);
	  break;
	case parser_ltl:
	  pf = spot::parse_infix_psl(str, env, debug, true);
	  break;
	}

      if (pf.errors.empty())
	return pf.f.to_node_();
      return parse_ap(str, location, env, error_list);
    }


#line 179 "parsetl.cc" // lalr1.cc:435


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
#if TLYYDEBUG

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

#else // !TLYYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !TLYYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace tlyy {
#line 274 "parsetl.cc" // lalr1.cc:510

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
  parser::parser (spot::parse_error_list &error_list_yyarg, spot::environment &parse_environment_yyarg, spot::formula &result_yyarg)
    :
#if TLYYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      error_list (error_list_yyarg),
      parse_environment (parse_environment_yyarg),
      result (result_yyarg)
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
      case 9: // "(...) block"
#line 283 "parsetl.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 509 "parsetl.cc" // lalr1.cc:652
        break;

      case 10: // "{...} block"
#line 283 "parsetl.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 515 "parsetl.cc" // lalr1.cc:652
        break;

      case 11: // "{...}! block"
#line 283 "parsetl.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 521 "parsetl.cc" // lalr1.cc:652
        break;

      case 52: // "atomic proposition"
#line 283 "parsetl.yy" // lalr1.cc:652
        { delete (yysym.value.str); }
#line 527 "parsetl.cc" // lalr1.cc:652
        break;

      case 94: // atomprop
#line 284 "parsetl.yy" // lalr1.cc:652
        { (yysym.value.ltl)->destroy(); }
#line 533 "parsetl.cc" // lalr1.cc:652
        break;

      case 95: // booleanatom
#line 284 "parsetl.yy" // lalr1.cc:652
        { (yysym.value.ltl)->destroy(); }
#line 539 "parsetl.cc" // lalr1.cc:652
        break;

      case 96: // sere
#line 284 "parsetl.yy" // lalr1.cc:652
        { (yysym.value.ltl)->destroy(); }
#line 545 "parsetl.cc" // lalr1.cc:652
        break;

      case 97: // bracedsere
#line 284 "parsetl.yy" // lalr1.cc:652
        { (yysym.value.ltl)->destroy(); }
#line 551 "parsetl.cc" // lalr1.cc:652
        break;

      case 98: // parenthesedsubformula
#line 284 "parsetl.yy" // lalr1.cc:652
        { (yysym.value.ltl)->destroy(); }
#line 557 "parsetl.cc" // lalr1.cc:652
        break;

      case 99: // boolformula
#line 284 "parsetl.yy" // lalr1.cc:652
        { (yysym.value.ltl)->destroy(); }
#line 563 "parsetl.cc" // lalr1.cc:652
        break;

      case 100: // subformula
#line 284 "parsetl.yy" // lalr1.cc:652
        { (yysym.value.ltl)->destroy(); }
#line 569 "parsetl.cc" // lalr1.cc:652
        break;

      case 101: // lbtformula
#line 284 "parsetl.yy" // lalr1.cc:652
        { (yysym.value.ltl)->destroy(); }
#line 575 "parsetl.cc" // lalr1.cc:652
        break;

      default:
        break;
    }
  }

#if TLYYDEBUG
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
      case 9: // "(...) block"
#line 286 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << *(yysym.value.str); }
#line 606 "parsetl.cc" // lalr1.cc:676
        break;

      case 10: // "{...} block"
#line 286 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << *(yysym.value.str); }
#line 612 "parsetl.cc" // lalr1.cc:676
        break;

      case 11: // "{...}! block"
#line 286 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << *(yysym.value.str); }
#line 618 "parsetl.cc" // lalr1.cc:676
        break;

      case 44: // "number for square bracket operator"
#line 289 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 624 "parsetl.cc" // lalr1.cc:676
        break;

      case 52: // "atomic proposition"
#line 286 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << *(yysym.value.str); }
#line 630 "parsetl.cc" // lalr1.cc:676
        break;

      case 60: // "SVA delay operator"
#line 289 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.num); }
#line 636 "parsetl.cc" // lalr1.cc:676
        break;

      case 87: // sqbracketargs
#line 290 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.minmax).min << ".." << (yysym.value.minmax).max; }
#line 642 "parsetl.cc" // lalr1.cc:676
        break;

      case 88: // gotoargs
#line 290 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.minmax).min << ".." << (yysym.value.minmax).max; }
#line 648 "parsetl.cc" // lalr1.cc:676
        break;

      case 90: // starargs
#line 290 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.minmax).min << ".." << (yysym.value.minmax).max; }
#line 654 "parsetl.cc" // lalr1.cc:676
        break;

      case 91: // fstarargs
#line 290 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.minmax).min << ".." << (yysym.value.minmax).max; }
#line 660 "parsetl.cc" // lalr1.cc:676
        break;

      case 92: // equalargs
#line 290 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.minmax).min << ".." << (yysym.value.minmax).max; }
#line 666 "parsetl.cc" // lalr1.cc:676
        break;

      case 93: // delayargs
#line 290 "parsetl.yy" // lalr1.cc:676
        { debug_stream() << (yysym.value.minmax).min << ".." << (yysym.value.minmax).max; }
#line 672 "parsetl.cc" // lalr1.cc:676
        break;

      case 94: // atomprop
#line 287 "parsetl.yy" // lalr1.cc:676
        { print_psl(debug_stream(), formula((yysym.value.ltl)->clone())); }
#line 678 "parsetl.cc" // lalr1.cc:676
        break;

      case 95: // booleanatom
#line 287 "parsetl.yy" // lalr1.cc:676
        { print_psl(debug_stream(), formula((yysym.value.ltl)->clone())); }
#line 684 "parsetl.cc" // lalr1.cc:676
        break;

      case 96: // sere
#line 288 "parsetl.yy" // lalr1.cc:676
        { print_sere(debug_stream(), formula((yysym.value.ltl)->clone())); }
#line 690 "parsetl.cc" // lalr1.cc:676
        break;

      case 97: // bracedsere
#line 288 "parsetl.yy" // lalr1.cc:676
        { print_sere(debug_stream(), formula((yysym.value.ltl)->clone())); }
#line 696 "parsetl.cc" // lalr1.cc:676
        break;

      case 98: // parenthesedsubformula
#line 287 "parsetl.yy" // lalr1.cc:676
        { print_psl(debug_stream(), formula((yysym.value.ltl)->clone())); }
#line 702 "parsetl.cc" // lalr1.cc:676
        break;

      case 99: // boolformula
#line 287 "parsetl.yy" // lalr1.cc:676
        { print_psl(debug_stream(), formula((yysym.value.ltl)->clone())); }
#line 708 "parsetl.cc" // lalr1.cc:676
        break;

      case 100: // subformula
#line 287 "parsetl.yy" // lalr1.cc:676
        { print_psl(debug_stream(), formula((yysym.value.ltl)->clone())); }
#line 714 "parsetl.cc" // lalr1.cc:676
        break;

      case 101: // lbtformula
#line 287 "parsetl.yy" // lalr1.cc:676
        { print_psl(debug_stream(), formula((yysym.value.ltl)->clone())); }
#line 720 "parsetl.cc" // lalr1.cc:676
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

#if TLYYDEBUG
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
#endif // TLYYDEBUG

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
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location, error_list));
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
#line 294 "parsetl.yy" // lalr1.cc:919
    {
		result = formula((yystack_[1].value.ltl));
		YYACCEPT;
	      }
#line 963 "parsetl.cc" // lalr1.cc:919
    break;

  case 3:
#line 299 "parsetl.yy" // lalr1.cc:919
    {
		result = nullptr;
		YYABORT;
	      }
#line 972 "parsetl.cc" // lalr1.cc:919
    break;

  case 4:
#line 304 "parsetl.yy" // lalr1.cc:919
    {
		result = formula((yystack_[1].value.ltl));
		YYACCEPT;
	      }
#line 981 "parsetl.cc" // lalr1.cc:919
    break;

  case 5:
#line 309 "parsetl.yy" // lalr1.cc:919
    { YYABORT; }
#line 987 "parsetl.cc" // lalr1.cc:919
    break;

  case 6:
#line 311 "parsetl.yy" // lalr1.cc:919
    {
		result = formula((yystack_[1].value.ltl));
		YYACCEPT;
	      }
#line 996 "parsetl.cc" // lalr1.cc:919
    break;

  case 7:
#line 316 "parsetl.yy" // lalr1.cc:919
    {
		result = nullptr;
		YYABORT;
	      }
#line 1005 "parsetl.cc" // lalr1.cc:919
    break;

  case 8:
#line 321 "parsetl.yy" // lalr1.cc:919
    {
		result = formula((yystack_[1].value.ltl));
		YYACCEPT;
	      }
#line 1014 "parsetl.cc" // lalr1.cc:919
    break;

  case 9:
#line 326 "parsetl.yy" // lalr1.cc:919
    { YYABORT; }
#line 1020 "parsetl.cc" // lalr1.cc:919
    break;

  case 10:
#line 328 "parsetl.yy" // lalr1.cc:919
    {
		result = formula((yystack_[1].value.ltl));
		YYACCEPT;
	      }
#line 1029 "parsetl.cc" // lalr1.cc:919
    break;

  case 11:
#line 333 "parsetl.yy" // lalr1.cc:919
    {
		result = nullptr;
		YYABORT;
	      }
#line 1038 "parsetl.cc" // lalr1.cc:919
    break;

  case 12:
#line 338 "parsetl.yy" // lalr1.cc:919
    {
		result = formula((yystack_[1].value.ltl));
		YYACCEPT;
	      }
#line 1047 "parsetl.cc" // lalr1.cc:919
    break;

  case 13:
#line 343 "parsetl.yy" // lalr1.cc:919
    { YYABORT; }
#line 1053 "parsetl.cc" // lalr1.cc:919
    break;

  case 14:
#line 345 "parsetl.yy" // lalr1.cc:919
    {
		result = formula((yystack_[1].value.ltl));
		YYACCEPT;
	      }
#line 1062 "parsetl.cc" // lalr1.cc:919
    break;

  case 15:
#line 350 "parsetl.yy" // lalr1.cc:919
    {
		result = nullptr;
		YYABORT;
	      }
#line 1071 "parsetl.cc" // lalr1.cc:919
    break;

  case 16:
#line 355 "parsetl.yy" // lalr1.cc:919
    {
		result = formula((yystack_[1].value.ltl));
		YYACCEPT;
	      }
#line 1080 "parsetl.cc" // lalr1.cc:919
    break;

  case 17:
#line 360 "parsetl.yy" // lalr1.cc:919
    { YYABORT; }
#line 1086 "parsetl.cc" // lalr1.cc:919
    break;

  case 18:
#line 363 "parsetl.yy" // lalr1.cc:919
    {
		error_list.emplace_back(yylhs.location, "empty input");
		result = nullptr;
	      }
#line 1095 "parsetl.cc" // lalr1.cc:919
    break;

  case 19:
#line 369 "parsetl.yy" // lalr1.cc:919
    {
		error_list.emplace_back(yystack_[1].location, "ignoring trailing garbage");
	      }
#line 1103 "parsetl.cc" // lalr1.cc:919
    break;

  case 26:
#line 382 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = (yystack_[3].value.num); (yylhs.value.minmax).max = (yystack_[1].value.num); }
#line 1109 "parsetl.cc" // lalr1.cc:919
    break;

  case 27:
#line 384 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = (yystack_[2].value.num); (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1115 "parsetl.cc" // lalr1.cc:919
    break;

  case 28:
#line 386 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = 0U; (yylhs.value.minmax).max = (yystack_[1].value.num); }
#line 1121 "parsetl.cc" // lalr1.cc:919
    break;

  case 29:
#line 388 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = 0U; (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1127 "parsetl.cc" // lalr1.cc:919
    break;

  case 30:
#line 390 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = (yylhs.value.minmax).max = (yystack_[1].value.num); }
#line 1133 "parsetl.cc" // lalr1.cc:919
    break;

  case 31:
#line 394 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = (yystack_[3].value.num); (yylhs.value.minmax).max = (yystack_[1].value.num); }
#line 1139 "parsetl.cc" // lalr1.cc:919
    break;

  case 32:
#line 396 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = (yystack_[2].value.num); (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1145 "parsetl.cc" // lalr1.cc:919
    break;

  case 33:
#line 398 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = 1U; (yylhs.value.minmax).max = (yystack_[1].value.num); }
#line 1151 "parsetl.cc" // lalr1.cc:919
    break;

  case 34:
#line 400 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = 1U; (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1157 "parsetl.cc" // lalr1.cc:919
    break;

  case 35:
#line 402 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = (yylhs.value.minmax).max = 1U; }
#line 1163 "parsetl.cc" // lalr1.cc:919
    break;

  case 36:
#line 404 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = (yylhs.value.minmax).max = (yystack_[1].value.num); }
#line 1169 "parsetl.cc" // lalr1.cc:919
    break;

  case 37:
#line 406 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location, "treating this goto block as [->]");
             (yylhs.value.minmax).min = (yylhs.value.minmax).max = 1U; }
#line 1176 "parsetl.cc" // lalr1.cc:919
    break;

  case 38:
#line 409 "parsetl.yy" // lalr1.cc:919
    { error_list.
	       emplace_back(yylhs.location, "missing closing bracket for goto operator");
	     (yylhs.value.minmax).min = (yylhs.value.minmax).max = 0U; }
#line 1184 "parsetl.cc" // lalr1.cc:919
    break;

  case 41:
#line 416 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = 0U; (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1190 "parsetl.cc" // lalr1.cc:919
    break;

  case 42:
#line 418 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = 1U; (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1196 "parsetl.cc" // lalr1.cc:919
    break;

  case 43:
#line 420 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax) = (yystack_[0].value.minmax); }
#line 1202 "parsetl.cc" // lalr1.cc:919
    break;

  case 44:
#line 422 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location, "treating this star block as [*]");
              (yylhs.value.minmax).min = 0U; (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1209 "parsetl.cc" // lalr1.cc:919
    break;

  case 45:
#line 425 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location, "missing closing bracket for star");
	      (yylhs.value.minmax).min = (yylhs.value.minmax).max = 0U; }
#line 1216 "parsetl.cc" // lalr1.cc:919
    break;

  case 46:
#line 429 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = 0U; (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1222 "parsetl.cc" // lalr1.cc:919
    break;

  case 47:
#line 431 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = 1U; (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1228 "parsetl.cc" // lalr1.cc:919
    break;

  case 48:
#line 433 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax) = (yystack_[0].value.minmax); }
#line 1234 "parsetl.cc" // lalr1.cc:919
    break;

  case 49:
#line 435 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back
		(yylhs.location, "treating this fusion-star block as [:*]");
              (yylhs.value.minmax).min = 0U; (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1242 "parsetl.cc" // lalr1.cc:919
    break;

  case 50:
#line 439 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back
		(yylhs.location, "missing closing bracket for fusion-star");
	      (yylhs.value.minmax).min = (yylhs.value.minmax).max = 0U; }
#line 1250 "parsetl.cc" // lalr1.cc:919
    break;

  case 51:
#line 444 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax) = (yystack_[0].value.minmax); }
#line 1256 "parsetl.cc" // lalr1.cc:919
    break;

  case 52:
#line 446 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location, "treating this equal block as [=]");
              (yylhs.value.minmax).min = 0U; (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1263 "parsetl.cc" // lalr1.cc:919
    break;

  case 53:
#line 449 "parsetl.yy" // lalr1.cc:919
    { error_list.
		emplace_back(yylhs.location, "missing closing bracket for equal operator");
	      (yylhs.value.minmax).min = (yylhs.value.minmax).max = 0U; }
#line 1271 "parsetl.cc" // lalr1.cc:919
    break;

  case 54:
#line 454 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax) = (yystack_[0].value.minmax); }
#line 1277 "parsetl.cc" // lalr1.cc:919
    break;

  case 55:
#line 456 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location, "treating this delay block as ##1");
              (yylhs.value.minmax).min = (yylhs.value.minmax).max = 1U; }
#line 1284 "parsetl.cc" // lalr1.cc:919
    break;

  case 56:
#line 459 "parsetl.yy" // lalr1.cc:919
    { error_list.
		emplace_back(yylhs.location, "missing closing bracket for ##[");
	      (yylhs.value.minmax).min = (yylhs.value.minmax).max = 1U; }
#line 1292 "parsetl.cc" // lalr1.cc:919
    break;

  case 57:
#line 463 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = 1; (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1298 "parsetl.cc" // lalr1.cc:919
    break;

  case 58:
#line 465 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.minmax).min = 0; (yylhs.value.minmax).max = fnode::unbounded(); }
#line 1304 "parsetl.cc" // lalr1.cc:919
    break;

  case 59:
#line 468 "parsetl.yy" // lalr1.cc:919
    {
            (yylhs.value.ltl) = parse_ap(*(yystack_[0].value.str), yystack_[0].location, parse_environment, error_list);
            delete (yystack_[0].value.str);
            if (!(yylhs.value.ltl))
              YYERROR;
          }
#line 1315 "parsetl.cc" // lalr1.cc:919
    break;

  case 60:
#line 475 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[0].value.ltl); }
#line 1321 "parsetl.cc" // lalr1.cc:919
    break;

  case 61:
#line 476 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[1].value.ltl); }
#line 1327 "parsetl.cc" // lalr1.cc:919
    break;

  case 62:
#line 478 "parsetl.yy" // lalr1.cc:919
    {
		(yylhs.value.ltl) = fnode::unop(op::Not, (yystack_[1].value.ltl));
	      }
#line 1335 "parsetl.cc" // lalr1.cc:919
    break;

  case 63:
#line 482 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::tt(); }
#line 1341 "parsetl.cc" // lalr1.cc:919
    break;

  case 64:
#line 484 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::ff(); }
#line 1347 "parsetl.cc" // lalr1.cc:919
    break;

  case 65:
#line 486 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[0].value.ltl); }
#line 1353 "parsetl.cc" // lalr1.cc:919
    break;

  case 66:
#line 488 "parsetl.yy" // lalr1.cc:919
    {
		if (sere_ensure_bool((yystack_[0].value.ltl), yystack_[0].location, "`!'", error_list))
		  {
		    (yylhs.value.ltl) = fnode::unop(op::Not, (yystack_[0].value.ltl));
		  }
		else
		  {
		    (yystack_[0].value.ltl)->destroy();
		    (yylhs.value.ltl) = error_false_block(yylhs.location, error_list);
		  }
	      }
#line 1369 "parsetl.cc" // lalr1.cc:919
    break;

  case 67:
#line 499 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[0].value.ltl); }
#line 1375 "parsetl.cc" // lalr1.cc:919
    break;

  case 68:
#line 501 "parsetl.yy" // lalr1.cc:919
    {
		(yylhs.value.ltl) =
		  try_recursive_parse(*(yystack_[0].value.str), yystack_[0].location, parse_environment,
				      debug_level(), parser_sere, error_list);
		delete (yystack_[0].value.str);
		if (!(yylhs.value.ltl))
		  YYERROR;
	      }
#line 1388 "parsetl.cc" // lalr1.cc:919
    break;

  case 69:
#line 510 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[1].value.ltl); }
#line 1394 "parsetl.cc" // lalr1.cc:919
    break;

  case 70:
#line 512 "parsetl.yy" // lalr1.cc:919
    { error_list.
		  emplace_back(yylhs.location,
			       "treating this parenthetical block as false");
		(yylhs.value.ltl) = fnode::ff();
	      }
#line 1404 "parsetl.cc" // lalr1.cc:919
    break;

  case 71:
#line 518 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[2].location + yystack_[1].location, "missing closing parenthesis");
		(yylhs.value.ltl) = (yystack_[1].value.ltl);
	      }
#line 1412 "parsetl.cc" // lalr1.cc:919
    break;

  case 72:
#line 522 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location,
                    "missing closing parenthesis, "
		    "treating this parenthetical block as false");
		(yylhs.value.ltl) = fnode::ff();
	      }
#line 1422 "parsetl.cc" // lalr1.cc:919
    break;

  case 73:
#line 528 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::AndRat, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 1428 "parsetl.cc" // lalr1.cc:919
    break;

  case 74:
#line 530 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location,
				    "length-matching and operator"); }
#line 1435 "parsetl.cc" // lalr1.cc:919
    break;

  case 75:
#line 533 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::AndNLM, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 1441 "parsetl.cc" // lalr1.cc:919
    break;

  case 76:
#line 535 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location,
                                    "non-length-matching and operator"); }
#line 1448 "parsetl.cc" // lalr1.cc:919
    break;

  case 77:
#line 538 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::OrRat, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 1454 "parsetl.cc" // lalr1.cc:919
    break;

  case 78:
#line 540 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "or operator"); }
#line 1460 "parsetl.cc" // lalr1.cc:919
    break;

  case 79:
#line 542 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::Concat, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 1466 "parsetl.cc" // lalr1.cc:919
    break;

  case 80:
#line 544 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "concat operator"); }
#line 1472 "parsetl.cc" // lalr1.cc:919
    break;

  case 81:
#line 546 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::Fusion, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 1478 "parsetl.cc" // lalr1.cc:919
    break;

  case 82:
#line 548 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "fusion operator"); }
#line 1484 "parsetl.cc" // lalr1.cc:919
    break;

  case 83:
#line 550 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = formula::sugar_delay(formula((yystack_[0].value.ltl)), (yystack_[1].value.num), (yystack_[1].value.num)).to_node_(); }
#line 1490 "parsetl.cc" // lalr1.cc:919
    break;

  case 84:
#line 552 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), fnode::tt(), yystack_[1].location, "SVA delay operator"); }
#line 1496 "parsetl.cc" // lalr1.cc:919
    break;

  case 85:
#line 554 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = formula::sugar_delay(formula((yystack_[2].value.ltl)), formula((yystack_[0].value.ltl)),
                                          (yystack_[1].value.num), (yystack_[1].value.num)).to_node_(); }
#line 1503 "parsetl.cc" // lalr1.cc:919
    break;

  case 86:
#line 557 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "SVA delay operator"); }
#line 1509 "parsetl.cc" // lalr1.cc:919
    break;

  case 87:
#line 559 "parsetl.yy" // lalr1.cc:919
    {
		if ((yystack_[1].value.minmax).max < (yystack_[1].value.minmax).min)
		  {
		    error_list.emplace_back(yystack_[1].location, "reversed range");
		    std::swap((yystack_[1].value.minmax).max, (yystack_[1].value.minmax).min);
		  }
                (yylhs.value.ltl) = formula::sugar_delay(formula((yystack_[0].value.ltl)),
                                          (yystack_[1].value.minmax).min, (yystack_[1].value.minmax).max).to_node_();
              }
#line 1523 "parsetl.cc" // lalr1.cc:919
    break;

  case 88:
#line 569 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), fnode::tt(), yystack_[1].location, "SVA delay operator"); }
#line 1529 "parsetl.cc" // lalr1.cc:919
    break;

  case 89:
#line 571 "parsetl.yy" // lalr1.cc:919
    {
		if ((yystack_[1].value.minmax).max < (yystack_[1].value.minmax).min)
		  {
		    error_list.emplace_back(yystack_[2].location, "reversed range");
		    std::swap((yystack_[1].value.minmax).max, (yystack_[1].value.minmax).min);
		  }
                (yylhs.value.ltl) = formula::sugar_delay(formula((yystack_[2].value.ltl)), formula((yystack_[0].value.ltl)),
                                          (yystack_[1].value.minmax).min, (yystack_[1].value.minmax).max).to_node_();
              }
#line 1543 "parsetl.cc" // lalr1.cc:919
    break;

  case 90:
#line 581 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "SVA delay operator"); }
#line 1549 "parsetl.cc" // lalr1.cc:919
    break;

  case 91:
#line 583 "parsetl.yy" // lalr1.cc:919
    {
		if ((yystack_[0].value.minmax).max < (yystack_[0].value.minmax).min)
		  {
		    error_list.emplace_back(yystack_[0].location, "reversed range");
		    std::swap((yystack_[0].value.minmax).max, (yystack_[0].value.minmax).min);
		  }
		(yylhs.value.ltl) = fnode::bunop(op::Star, fnode::tt(), (yystack_[0].value.minmax).min, (yystack_[0].value.minmax).max);
	      }
#line 1562 "parsetl.cc" // lalr1.cc:919
    break;

  case 92:
#line 592 "parsetl.yy" // lalr1.cc:919
    {
		if ((yystack_[0].value.minmax).max < (yystack_[0].value.minmax).min)
		  {
		    error_list.emplace_back(yystack_[0].location, "reversed range");
		    std::swap((yystack_[0].value.minmax).max, (yystack_[0].value.minmax).min);
		  }
		(yylhs.value.ltl) = fnode::bunop(op::Star, (yystack_[1].value.ltl), (yystack_[0].value.minmax).min, (yystack_[0].value.minmax).max);
	      }
#line 1575 "parsetl.cc" // lalr1.cc:919
    break;

  case 93:
#line 601 "parsetl.yy" // lalr1.cc:919
    {
		if ((yystack_[0].value.minmax).max < (yystack_[0].value.minmax).min)
		  {
		    error_list.emplace_back(yystack_[0].location, "reversed range");
		    std::swap((yystack_[0].value.minmax).max, (yystack_[0].value.minmax).min);
		  }
		(yylhs.value.ltl) = fnode::bunop(op::FStar, (yystack_[1].value.ltl), (yystack_[0].value.minmax).min, (yystack_[0].value.minmax).max);
	      }
#line 1588 "parsetl.cc" // lalr1.cc:919
    break;

  case 94:
#line 610 "parsetl.yy" // lalr1.cc:919
    {
		if ((yystack_[0].value.minmax).max < (yystack_[0].value.minmax).min)
		  {
		    error_list.emplace_back(yystack_[0].location, "reversed range");
		    std::swap((yystack_[0].value.minmax).max, (yystack_[0].value.minmax).min);
		  }
		if (sere_ensure_bool((yystack_[1].value.ltl), yystack_[1].location, "[=...]", error_list))
		  {
		    (yylhs.value.ltl) = formula::sugar_equal(formula((yystack_[1].value.ltl)),
					      (yystack_[0].value.minmax).min, (yystack_[0].value.minmax).max).to_node_();
		  }
		else
		  {
		    (yystack_[1].value.ltl)->destroy();
		    (yylhs.value.ltl) = error_false_block(yylhs.location, error_list);
		  }
	      }
#line 1610 "parsetl.cc" // lalr1.cc:919
    break;

  case 95:
#line 628 "parsetl.yy" // lalr1.cc:919
    {
		if ((yystack_[0].value.minmax).max < (yystack_[0].value.minmax).min)
		  {
		    error_list.emplace_back(yystack_[0].location, "reversed range");
		    std::swap((yystack_[0].value.minmax).max, (yystack_[0].value.minmax).min);
		  }
		if (sere_ensure_bool((yystack_[1].value.ltl), yystack_[1].location, "[->...]", error_list))
		  {
		    (yylhs.value.ltl) = formula::sugar_goto(formula((yystack_[1].value.ltl)),
					     (yystack_[0].value.minmax).min, (yystack_[0].value.minmax).max).to_node_();
		  }
		else
		  {
		    (yystack_[1].value.ltl)->destroy();
		    (yylhs.value.ltl) = error_false_block(yylhs.location, error_list);
		  }
	      }
#line 1632 "parsetl.cc" // lalr1.cc:919
    break;

  case 96:
#line 646 "parsetl.yy" // lalr1.cc:919
    {
		if (sere_ensure_bool((yystack_[2].value.ltl), yystack_[2].location, "`^'", error_list)
                    && sere_ensure_bool((yystack_[0].value.ltl), yystack_[0].location, "`^'", error_list))
		  {
		    (yylhs.value.ltl) = fnode::binop(op::Xor, (yystack_[2].value.ltl), (yystack_[0].value.ltl));
		  }
		else
		  {
		    (yystack_[2].value.ltl)->destroy();
		    (yystack_[0].value.ltl)->destroy();
		    (yylhs.value.ltl) = error_false_block(yylhs.location, error_list);
		  }
	      }
#line 1650 "parsetl.cc" // lalr1.cc:919
    break;

  case 97:
#line 660 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "xor operator"); }
#line 1656 "parsetl.cc" // lalr1.cc:919
    break;

  case 98:
#line 662 "parsetl.yy" // lalr1.cc:919
    {
		if (sere_ensure_bool((yystack_[2].value.ltl), yystack_[2].location, "`->'", error_list))
		  {
		    (yylhs.value.ltl) = fnode::binop(op::Implies, (yystack_[2].value.ltl), (yystack_[0].value.ltl));
		  }
		else
		  {
		    (yystack_[2].value.ltl)->destroy();
		    (yystack_[0].value.ltl)->destroy();
		    (yylhs.value.ltl) = error_false_block(yylhs.location, error_list);
		  }
	      }
#line 1673 "parsetl.cc" // lalr1.cc:919
    break;

  case 99:
#line 675 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "implication operator"); }
#line 1679 "parsetl.cc" // lalr1.cc:919
    break;

  case 100:
#line 677 "parsetl.yy" // lalr1.cc:919
    {
		if (sere_ensure_bool((yystack_[2].value.ltl), yystack_[2].location, "`<->'", error_list)
                    && sere_ensure_bool((yystack_[0].value.ltl), yystack_[0].location, "`<->'", error_list))
		  {
		    (yylhs.value.ltl) = fnode::binop(op::Equiv, (yystack_[2].value.ltl), (yystack_[0].value.ltl));
		  }
		else
		  {
		    (yystack_[2].value.ltl)->destroy();
		    (yystack_[0].value.ltl)->destroy();
		    (yylhs.value.ltl) = error_false_block(yylhs.location, error_list);
		  }
	      }
#line 1697 "parsetl.cc" // lalr1.cc:919
    break;

  case 101:
#line 691 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "equivalent operator"); }
#line 1703 "parsetl.cc" // lalr1.cc:919
    break;

  case 102:
#line 693 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::first_match, (yystack_[1].value.ltl)); }
#line 1709 "parsetl.cc" // lalr1.cc:919
    break;

  case 103:
#line 696 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[1].value.ltl); }
#line 1715 "parsetl.cc" // lalr1.cc:919
    break;

  case 104:
#line 698 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[1].location, "ignoring this");
		(yylhs.value.ltl) = (yystack_[2].value.ltl);
	      }
#line 1723 "parsetl.cc" // lalr1.cc:919
    break;

  case 105:
#line 702 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location,
					"treating this brace block as false");
		(yylhs.value.ltl) = fnode::ff();
	      }
#line 1732 "parsetl.cc" // lalr1.cc:919
    break;

  case 106:
#line 707 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[2].location + yystack_[1].location,
					"missing closing brace");
		(yylhs.value.ltl) = (yystack_[1].value.ltl);
	      }
#line 1741 "parsetl.cc" // lalr1.cc:919
    break;

  case 107:
#line 712 "parsetl.yy" // lalr1.cc:919
    { error_list. emplace_back(yystack_[1].location,
                  "ignoring trailing garbage and missing closing brace");
		(yylhs.value.ltl) = (yystack_[2].value.ltl);
	      }
#line 1750 "parsetl.cc" // lalr1.cc:919
    break;

  case 108:
#line 717 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location,
                    "missing closing brace, "
		    "treating this brace block as false");
		(yylhs.value.ltl) = fnode::ff();
	      }
#line 1760 "parsetl.cc" // lalr1.cc:919
    break;

  case 109:
#line 723 "parsetl.yy" // lalr1.cc:919
    {
		(yylhs.value.ltl) = try_recursive_parse(*(yystack_[0].value.str), yystack_[0].location, parse_environment,
					 debug_level(),
                                         parser_sere, error_list);
		delete (yystack_[0].value.str);
		if (!(yylhs.value.ltl))
		  YYERROR;
	      }
#line 1773 "parsetl.cc" // lalr1.cc:919
    break;

  case 110:
#line 733 "parsetl.yy" // lalr1.cc:919
    {
		(yylhs.value.ltl) = try_recursive_parse(*(yystack_[0].value.str), yystack_[0].location, parse_environment,
					 debug_level(), parser_ltl, error_list);
		delete (yystack_[0].value.str);
		if (!(yylhs.value.ltl))
		  YYERROR;
	      }
#line 1785 "parsetl.cc" // lalr1.cc:919
    break;

  case 111:
#line 741 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[1].value.ltl); }
#line 1791 "parsetl.cc" // lalr1.cc:919
    break;

  case 112:
#line 743 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[1].location, "ignoring this");
		(yylhs.value.ltl) = (yystack_[2].value.ltl);
	      }
#line 1799 "parsetl.cc" // lalr1.cc:919
    break;

  case 113:
#line 747 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location,
		 "treating this parenthetical block as false");
		(yylhs.value.ltl) = fnode::ff();
	      }
#line 1808 "parsetl.cc" // lalr1.cc:919
    break;

  case 114:
#line 752 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[2].location + yystack_[1].location, "missing closing parenthesis");
		(yylhs.value.ltl) = (yystack_[1].value.ltl);
	      }
#line 1816 "parsetl.cc" // lalr1.cc:919
    break;

  case 115:
#line 756 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[1].location,
                "ignoring trailing garbage and missing closing parenthesis");
		(yylhs.value.ltl) = (yystack_[2].value.ltl);
	      }
#line 1825 "parsetl.cc" // lalr1.cc:919
    break;

  case 116:
#line 761 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location,
                    "missing closing parenthesis, "
		    "treating this parenthetical block as false");
		(yylhs.value.ltl) = fnode::ff();
	      }
#line 1835 "parsetl.cc" // lalr1.cc:919
    break;

  case 117:
#line 768 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[0].value.ltl); }
#line 1841 "parsetl.cc" // lalr1.cc:919
    break;

  case 118:
#line 770 "parsetl.yy" // lalr1.cc:919
    {
		(yylhs.value.ltl) = try_recursive_parse(*(yystack_[0].value.str), yystack_[0].location, parse_environment,
					 debug_level(),
                                         parser_bool, error_list);
		delete (yystack_[0].value.str);
		if (!(yylhs.value.ltl))
		  YYERROR;
	      }
#line 1854 "parsetl.cc" // lalr1.cc:919
    break;

  case 119:
#line 779 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[1].value.ltl); }
#line 1860 "parsetl.cc" // lalr1.cc:919
    break;

  case 120:
#line 781 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[1].location, "ignoring this");
		(yylhs.value.ltl) = (yystack_[2].value.ltl);
	      }
#line 1868 "parsetl.cc" // lalr1.cc:919
    break;

  case 121:
#line 785 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location,
		 "treating this parenthetical block as false");
		(yylhs.value.ltl) = fnode::ff();
	      }
#line 1877 "parsetl.cc" // lalr1.cc:919
    break;

  case 122:
#line 790 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[2].location + yystack_[1].location,
					"missing closing parenthesis");
		(yylhs.value.ltl) = (yystack_[1].value.ltl);
	      }
#line 1886 "parsetl.cc" // lalr1.cc:919
    break;

  case 123:
#line 795 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[1].location,
                "ignoring trailing garbage and missing closing parenthesis");
		(yylhs.value.ltl) = (yystack_[2].value.ltl);
	      }
#line 1895 "parsetl.cc" // lalr1.cc:919
    break;

  case 124:
#line 800 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location,
                    "missing closing parenthesis, "
		    "treating this parenthetical block as false");
		(yylhs.value.ltl) = fnode::ff();
	      }
#line 1905 "parsetl.cc" // lalr1.cc:919
    break;

  case 125:
#line 806 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::And, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 1911 "parsetl.cc" // lalr1.cc:919
    break;

  case 126:
#line 808 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "and operator"); }
#line 1917 "parsetl.cc" // lalr1.cc:919
    break;

  case 127:
#line 810 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::And, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 1923 "parsetl.cc" // lalr1.cc:919
    break;

  case 128:
#line 812 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "and operator"); }
#line 1929 "parsetl.cc" // lalr1.cc:919
    break;

  case 129:
#line 814 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::And, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 1935 "parsetl.cc" // lalr1.cc:919
    break;

  case 130:
#line 816 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "and operator"); }
#line 1941 "parsetl.cc" // lalr1.cc:919
    break;

  case 131:
#line 818 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::Or, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 1947 "parsetl.cc" // lalr1.cc:919
    break;

  case 132:
#line 820 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "or operator"); }
#line 1953 "parsetl.cc" // lalr1.cc:919
    break;

  case 133:
#line 822 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::Xor, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 1959 "parsetl.cc" // lalr1.cc:919
    break;

  case 134:
#line 824 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "xor operator"); }
#line 1965 "parsetl.cc" // lalr1.cc:919
    break;

  case 135:
#line 826 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::Implies, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 1971 "parsetl.cc" // lalr1.cc:919
    break;

  case 136:
#line 828 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "implication operator"); }
#line 1977 "parsetl.cc" // lalr1.cc:919
    break;

  case 137:
#line 830 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::Equiv, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 1983 "parsetl.cc" // lalr1.cc:919
    break;

  case 138:
#line 832 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "equivalent operator"); }
#line 1989 "parsetl.cc" // lalr1.cc:919
    break;

  case 139:
#line 834 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::Not, (yystack_[0].value.ltl)); }
#line 1995 "parsetl.cc" // lalr1.cc:919
    break;

  case 140:
#line 836 "parsetl.yy" // lalr1.cc:919
    { missing_right_op((yylhs.value.ltl), yystack_[1].location, "not operator"); }
#line 2001 "parsetl.cc" // lalr1.cc:919
    break;

  case 141:
#line 838 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[0].value.ltl); }
#line 2007 "parsetl.cc" // lalr1.cc:919
    break;

  case 142:
#line 839 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[0].value.ltl); }
#line 2013 "parsetl.cc" // lalr1.cc:919
    break;

  case 143:
#line 841 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::And, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 2019 "parsetl.cc" // lalr1.cc:919
    break;

  case 144:
#line 843 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "and operator"); }
#line 2025 "parsetl.cc" // lalr1.cc:919
    break;

  case 145:
#line 845 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::And, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 2031 "parsetl.cc" // lalr1.cc:919
    break;

  case 146:
#line 847 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "and operator"); }
#line 2037 "parsetl.cc" // lalr1.cc:919
    break;

  case 147:
#line 849 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::And, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 2043 "parsetl.cc" // lalr1.cc:919
    break;

  case 148:
#line 851 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "and operator"); }
#line 2049 "parsetl.cc" // lalr1.cc:919
    break;

  case 149:
#line 853 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::Or, {(yystack_[2].value.ltl), (yystack_[0].value.ltl)}); }
#line 2055 "parsetl.cc" // lalr1.cc:919
    break;

  case 150:
#line 855 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "or operator"); }
#line 2061 "parsetl.cc" // lalr1.cc:919
    break;

  case 151:
#line 857 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::Xor, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 2067 "parsetl.cc" // lalr1.cc:919
    break;

  case 152:
#line 859 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "xor operator"); }
#line 2073 "parsetl.cc" // lalr1.cc:919
    break;

  case 153:
#line 861 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::Implies, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 2079 "parsetl.cc" // lalr1.cc:919
    break;

  case 154:
#line 863 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "implication operator"); }
#line 2085 "parsetl.cc" // lalr1.cc:919
    break;

  case 155:
#line 865 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::Equiv, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 2091 "parsetl.cc" // lalr1.cc:919
    break;

  case 156:
#line 867 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "equivalent operator"); }
#line 2097 "parsetl.cc" // lalr1.cc:919
    break;

  case 157:
#line 869 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::U, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 2103 "parsetl.cc" // lalr1.cc:919
    break;

  case 158:
#line 871 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "until operator"); }
#line 2109 "parsetl.cc" // lalr1.cc:919
    break;

  case 159:
#line 873 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::R, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 2115 "parsetl.cc" // lalr1.cc:919
    break;

  case 160:
#line 875 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "release operator"); }
#line 2121 "parsetl.cc" // lalr1.cc:919
    break;

  case 161:
#line 877 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::W, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 2127 "parsetl.cc" // lalr1.cc:919
    break;

  case 162:
#line 879 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "weak until operator"); }
#line 2133 "parsetl.cc" // lalr1.cc:919
    break;

  case 163:
#line 881 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::M, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 2139 "parsetl.cc" // lalr1.cc:919
    break;

  case 164:
#line 883 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location, "strong release operator"); }
#line 2145 "parsetl.cc" // lalr1.cc:919
    break;

  case 165:
#line 885 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::F, (yystack_[0].value.ltl)); }
#line 2151 "parsetl.cc" // lalr1.cc:919
    break;

  case 166:
#line 887 "parsetl.yy" // lalr1.cc:919
    { missing_right_op((yylhs.value.ltl), yystack_[1].location, "sometimes operator"); }
#line 2157 "parsetl.cc" // lalr1.cc:919
    break;

  case 167:
#line 889 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::X, op::Or, (yystack_[2].value.num), (yystack_[2].value.num), (yystack_[0].value.ltl));
                error_list.emplace_back(yystack_[3].location + yystack_[1].location,
                                        "F[n:m] expects two parameters");
              }
#line 2166 "parsetl.cc" // lalr1.cc:919
    break;

  case 168:
#line 895 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::strong_X, op::Or, (yystack_[2].value.num), (yystack_[2].value.num), (yystack_[0].value.ltl));
                error_list.emplace_back(yystack_[3].location + yystack_[1].location,
                                        "F[n:m!] expects two parameters");
              }
#line 2175 "parsetl.cc" // lalr1.cc:919
    break;

  case 169:
#line 901 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::X, op::Or, (yystack_[4].value.num), (yystack_[2].value.num), (yystack_[0].value.ltl)); }
#line 2181 "parsetl.cc" // lalr1.cc:919
    break;

  case 170:
#line 904 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::strong_X,
                                              op::Or, (yystack_[4].value.num), (yystack_[2].value.num), (yystack_[0].value.ltl)); }
#line 2188 "parsetl.cc" // lalr1.cc:919
    break;

  case 171:
#line 908 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::X, op::Or, (yystack_[3].value.num),
                                            fnode::unbounded(), (yystack_[0].value.ltl)); }
#line 2195 "parsetl.cc" // lalr1.cc:919
    break;

  case 172:
#line 912 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::strong_X, op::Or, (yystack_[3].value.num),
                                            fnode::unbounded(), (yystack_[0].value.ltl)); }
#line 2202 "parsetl.cc" // lalr1.cc:919
    break;

  case 173:
#line 916 "parsetl.yy" // lalr1.cc:919
    { missing_right_op((yylhs.value.ltl), yystack_[5].location + yystack_[1].location, "F[.] operator"); }
#line 2208 "parsetl.cc" // lalr1.cc:919
    break;

  case 174:
#line 919 "parsetl.yy" // lalr1.cc:919
    { missing_right_op((yylhs.value.ltl), yystack_[5].location + yystack_[1].location, "F[.!] operator"); }
#line 2214 "parsetl.cc" // lalr1.cc:919
    break;

  case 175:
#line 921 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location, "missing closing bracket for F[.]");
                (yylhs.value.ltl) = fnode::ff(); }
#line 2221 "parsetl.cc" // lalr1.cc:919
    break;

  case 176:
#line 924 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[3].location + yystack_[1].location,
                                        "treating this F[.] as a simple F");
                (yylhs.value.ltl) = fnode::unop(op::F, (yystack_[0].value.ltl)); }
#line 2229 "parsetl.cc" // lalr1.cc:919
    break;

  case 177:
#line 928 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[3].location + yystack_[1].location,
                                        "treating this F[.!] as a simple F");
                (yylhs.value.ltl) = fnode::unop(op::F, (yystack_[0].value.ltl)); }
#line 2237 "parsetl.cc" // lalr1.cc:919
    break;

  case 178:
#line 932 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::G, (yystack_[0].value.ltl)); }
#line 2243 "parsetl.cc" // lalr1.cc:919
    break;

  case 179:
#line 934 "parsetl.yy" // lalr1.cc:919
    { missing_right_op((yylhs.value.ltl), yystack_[1].location, "always operator"); }
#line 2249 "parsetl.cc" // lalr1.cc:919
    break;

  case 180:
#line 937 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::X, op::And, (yystack_[4].value.num), (yystack_[2].value.num), (yystack_[0].value.ltl)); }
#line 2255 "parsetl.cc" // lalr1.cc:919
    break;

  case 181:
#line 940 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::strong_X, op::And,
                                              (yystack_[4].value.num), (yystack_[2].value.num), (yystack_[0].value.ltl)); }
#line 2262 "parsetl.cc" // lalr1.cc:919
    break;

  case 182:
#line 944 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::X, op::And, (yystack_[3].value.num),
                                            fnode::unbounded(), (yystack_[0].value.ltl)); }
#line 2269 "parsetl.cc" // lalr1.cc:919
    break;

  case 183:
#line 948 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::strong_X, op::And, (yystack_[3].value.num),
                                            fnode::unbounded(), (yystack_[0].value.ltl)); }
#line 2276 "parsetl.cc" // lalr1.cc:919
    break;

  case 184:
#line 951 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::X, op::And, (yystack_[2].value.num), (yystack_[2].value.num), (yystack_[0].value.ltl));
                error_list.emplace_back(yystack_[3].location + yystack_[1].location,
                                        "G[n:m] expects two parameters");
              }
#line 2285 "parsetl.cc" // lalr1.cc:919
    break;

  case 185:
#line 957 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::strong_X, op::And,
                                              (yystack_[2].value.num), (yystack_[2].value.num), (yystack_[0].value.ltl));
                error_list.emplace_back(yystack_[3].location + yystack_[1].location,
                                        "G[n:m!] expects two parameters");
              }
#line 2295 "parsetl.cc" // lalr1.cc:919
    break;

  case 186:
#line 964 "parsetl.yy" // lalr1.cc:919
    { missing_right_op((yylhs.value.ltl), yystack_[5].location + yystack_[1].location, "G[.] operator"); }
#line 2301 "parsetl.cc" // lalr1.cc:919
    break;

  case 187:
#line 967 "parsetl.yy" // lalr1.cc:919
    { missing_right_op((yylhs.value.ltl), yystack_[5].location + yystack_[1].location, "G[.!] operator"); }
#line 2307 "parsetl.cc" // lalr1.cc:919
    break;

  case 188:
#line 969 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location, "missing closing bracket for G[.]");
                (yylhs.value.ltl) = fnode::ff(); }
#line 2314 "parsetl.cc" // lalr1.cc:919
    break;

  case 189:
#line 972 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[3].location + yystack_[1].location,
                                        "treating this G[.] as a simple G");
                (yylhs.value.ltl) = fnode::unop(op::F, (yystack_[0].value.ltl)); }
#line 2322 "parsetl.cc" // lalr1.cc:919
    break;

  case 190:
#line 976 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yystack_[3].location + yystack_[1].location,
                                        "treating this G[.!] as a simple G");
                (yylhs.value.ltl) = fnode::unop(op::F, (yystack_[0].value.ltl)); }
#line 2330 "parsetl.cc" // lalr1.cc:919
    break;

  case 191:
#line 980 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::X, (yystack_[0].value.ltl)); }
#line 2336 "parsetl.cc" // lalr1.cc:919
    break;

  case 192:
#line 982 "parsetl.yy" // lalr1.cc:919
    { missing_right_op((yylhs.value.ltl), yystack_[1].location, "next operator"); }
#line 2342 "parsetl.cc" // lalr1.cc:919
    break;

  case 193:
#line 984 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::strong_X, (yystack_[0].value.ltl)); }
#line 2348 "parsetl.cc" // lalr1.cc:919
    break;

  case 194:
#line 986 "parsetl.yy" // lalr1.cc:919
    { missing_right_op((yylhs.value.ltl), yystack_[1].location, "strong next operator"); }
#line 2354 "parsetl.cc" // lalr1.cc:919
    break;

  case 195:
#line 988 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::X, op::Or, (yystack_[2].value.num), (yystack_[2].value.num), (yystack_[0].value.ltl)); }
#line 2360 "parsetl.cc" // lalr1.cc:919
    break;

  case 196:
#line 990 "parsetl.yy" // lalr1.cc:919
    { missing_right_op((yylhs.value.ltl), yystack_[3].location + yystack_[1].location, "X[.] operator"); }
#line 2366 "parsetl.cc" // lalr1.cc:919
    break;

  case 197:
#line 992 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location, "treating this X[.] as a simple X");
                (yylhs.value.ltl) = fnode::unop(op::X, (yystack_[0].value.ltl)); }
#line 2373 "parsetl.cc" // lalr1.cc:919
    break;

  case 198:
#line 995 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::strong_X, (yystack_[0].value.ltl)); }
#line 2379 "parsetl.cc" // lalr1.cc:919
    break;

  case 199:
#line 998 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::nested_unop_range(op::strong_X,
                                              op::Or, (yystack_[2].value.num), (yystack_[2].value.num), (yystack_[0].value.ltl)); }
#line 2386 "parsetl.cc" // lalr1.cc:919
    break;

  case 200:
#line 1001 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location, "treating this X[.!] as a simple X[!]");
                (yylhs.value.ltl) = fnode::unop(op::strong_X, (yystack_[0].value.ltl)); }
#line 2393 "parsetl.cc" // lalr1.cc:919
    break;

  case 201:
#line 1004 "parsetl.yy" // lalr1.cc:919
    { error_list.emplace_back(yylhs.location, "missing closing bracket for X[.]");
                (yylhs.value.ltl) = fnode::ff(); }
#line 2400 "parsetl.cc" // lalr1.cc:919
    break;

  case 202:
#line 1007 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::Not, (yystack_[0].value.ltl)); }
#line 2406 "parsetl.cc" // lalr1.cc:919
    break;

  case 203:
#line 1009 "parsetl.yy" // lalr1.cc:919
    { missing_right_op((yylhs.value.ltl), yystack_[1].location, "not operator"); }
#line 2412 "parsetl.cc" // lalr1.cc:919
    break;

  case 204:
#line 1011 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::Closure, (yystack_[0].value.ltl)); }
#line 2418 "parsetl.cc" // lalr1.cc:919
    break;

  case 205:
#line 1013 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::UConcat, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 2424 "parsetl.cc" // lalr1.cc:919
    break;

  case 206:
#line 1015 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::UConcat, (yystack_[1].value.ltl), (yystack_[0].value.ltl)); }
#line 2430 "parsetl.cc" // lalr1.cc:919
    break;

  case 207:
#line 1017 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop_hard((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location,
				    "universal overlapping concat operator"); }
#line 2437 "parsetl.cc" // lalr1.cc:919
    break;

  case 208:
#line 1020 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::EConcat, (yystack_[2].value.ltl), (yystack_[0].value.ltl)); }
#line 2443 "parsetl.cc" // lalr1.cc:919
    break;

  case 209:
#line 1022 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop_hard((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location,
				    "existential overlapping concat operator");
	      }
#line 2451 "parsetl.cc" // lalr1.cc:919
    break;

  case 210:
#line 1027 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::UConcat,
				  fnode::multop(op::Concat, {(yystack_[2].value.ltl), fnode::tt()}),
				  (yystack_[0].value.ltl)); }
#line 2459 "parsetl.cc" // lalr1.cc:919
    break;

  case 211:
#line 1031 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop_hard((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location,
				  "universal non-overlapping concat operator");
	      }
#line 2467 "parsetl.cc" // lalr1.cc:919
    break;

  case 212:
#line 1036 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::EConcat,
				  fnode::multop(op::Concat, {(yystack_[2].value.ltl), fnode::tt()}),
				  (yystack_[0].value.ltl)); }
#line 2475 "parsetl.cc" // lalr1.cc:919
    break;

  case 213:
#line 1040 "parsetl.yy" // lalr1.cc:919
    { missing_right_binop_hard((yylhs.value.ltl), (yystack_[2].value.ltl), yystack_[1].location,
				"existential non-overlapping concat operator");
	      }
#line 2483 "parsetl.cc" // lalr1.cc:919
    break;

  case 214:
#line 1045 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::EConcat, (yystack_[1].value.ltl), fnode::tt()); }
#line 2489 "parsetl.cc" // lalr1.cc:919
    break;

  case 215:
#line 1047 "parsetl.yy" // lalr1.cc:919
    {
		(yylhs.value.ltl) = try_recursive_parse(*(yystack_[0].value.str), yystack_[0].location, parse_environment,
					 debug_level(),
                                         parser_sere, error_list);
		delete (yystack_[0].value.str);
		if (!(yylhs.value.ltl))
		  YYERROR;
		(yylhs.value.ltl) = fnode::binop(op::EConcat, (yylhs.value.ltl), fnode::tt());
	      }
#line 2503 "parsetl.cc" // lalr1.cc:919
    break;

  case 216:
#line 1057 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = (yystack_[0].value.ltl); }
#line 2509 "parsetl.cc" // lalr1.cc:919
    break;

  case 217:
#line 1059 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::Not, (yystack_[0].value.ltl)); }
#line 2515 "parsetl.cc" // lalr1.cc:919
    break;

  case 218:
#line 1061 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::And, {(yystack_[1].value.ltl), (yystack_[0].value.ltl)}); }
#line 2521 "parsetl.cc" // lalr1.cc:919
    break;

  case 219:
#line 1063 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::multop(op::Or, {(yystack_[1].value.ltl), (yystack_[0].value.ltl)}); }
#line 2527 "parsetl.cc" // lalr1.cc:919
    break;

  case 220:
#line 1065 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::Xor, (yystack_[1].value.ltl), (yystack_[0].value.ltl)); }
#line 2533 "parsetl.cc" // lalr1.cc:919
    break;

  case 221:
#line 1067 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::Implies, (yystack_[1].value.ltl), (yystack_[0].value.ltl)); }
#line 2539 "parsetl.cc" // lalr1.cc:919
    break;

  case 222:
#line 1069 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::Equiv, (yystack_[1].value.ltl), (yystack_[0].value.ltl)); }
#line 2545 "parsetl.cc" // lalr1.cc:919
    break;

  case 223:
#line 1071 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::X, (yystack_[0].value.ltl)); }
#line 2551 "parsetl.cc" // lalr1.cc:919
    break;

  case 224:
#line 1073 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::F, (yystack_[0].value.ltl)); }
#line 2557 "parsetl.cc" // lalr1.cc:919
    break;

  case 225:
#line 1075 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::unop(op::G, (yystack_[0].value.ltl)); }
#line 2563 "parsetl.cc" // lalr1.cc:919
    break;

  case 226:
#line 1077 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::U, (yystack_[1].value.ltl), (yystack_[0].value.ltl)); }
#line 2569 "parsetl.cc" // lalr1.cc:919
    break;

  case 227:
#line 1079 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::R, (yystack_[1].value.ltl), (yystack_[0].value.ltl)); }
#line 2575 "parsetl.cc" // lalr1.cc:919
    break;

  case 228:
#line 1081 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::R, (yystack_[1].value.ltl), (yystack_[0].value.ltl)); }
#line 2581 "parsetl.cc" // lalr1.cc:919
    break;

  case 229:
#line 1083 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::W, (yystack_[1].value.ltl), (yystack_[0].value.ltl)); }
#line 2587 "parsetl.cc" // lalr1.cc:919
    break;

  case 230:
#line 1085 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::binop(op::M, (yystack_[1].value.ltl), (yystack_[0].value.ltl)); }
#line 2593 "parsetl.cc" // lalr1.cc:919
    break;

  case 231:
#line 1087 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::tt(); }
#line 2599 "parsetl.cc" // lalr1.cc:919
    break;

  case 232:
#line 1089 "parsetl.yy" // lalr1.cc:919
    { (yylhs.value.ltl) = fnode::ff(); }
#line 2605 "parsetl.cc" // lalr1.cc:919
    break;


#line 2609 "parsetl.cc" // lalr1.cc:919
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


  const signed char parser::yypact_ninf_ = -86;

  const signed char parser::yytable_ninf_ = -25;

  const short
  parser::yypact_[] =
  {
     237,   571,   407,   608,   256,    38,   -56,  1298,   -86,   -86,
     -86,   644,  1335,  1367,  1399,  1431,  1463,    21,    47,   125,
     -86,   -86,   -86,   -86,   -86,   -86,   -42,   -86,   146,   -86,
      13,  2258,  2258,  2258,  2258,  2258,  2258,  2258,  2258,  2258,
    2258,  2258,  2258,  2258,  2258,   -86,   -86,   -86,   -86,   -86,
       1,   680,   -86,   644,  1147,   -86,   -86,   -86,    89,    43,
     716,   124,   -86,   -86,   -86,   -86,   -86,   -86,   752,   -86,
     551,   -86,   440,   -86,  1097,   -86,   -86,   -86,   217,   -86,
     -86,    31,  1196,    -1,   486,   -86,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,    -2,  2228,    20,    -5,   166,
      81,    14,   177,   225,    40,   -86,   -86,  1495,  1527,  1559,
    1591,   -86,  1623,  1655,  1687,  1719,  1751,  1783,  1815,  1847,
    1879,  1911,  1943,   -86,   -86,   -86,  2258,  2258,  2258,  2258,
    2258,   -86,   -86,   -86,  2258,  2258,  2258,  2258,  2258,   -86,
     -86,    35,  1228,   497,   -86,    15,    86,   210,   -86,    74,
      88,   -86,  1147,   -86,  2329,   109,    97,   -86,   -86,  2329,
     788,   824,   860,   896,   932,   968,   -86,   -86,   182,   202,
     343,  1004,  1040,   -86,  1076,   -86,   -86,   -86,   -86,   -86,
    1112,    36,  1222,   -86,   -86,  2110,  2115,  2139,  2144,  2168,
    2173,  2178,   -86,   -86,   -86,   -86,    41,   -86,   -86,   -86,
     -86,    29,   -86,   -86,   -86,  2228,  2228,   -86,  1975,  2228,
     -86,  2228,  2228,  2228,  2228,   272,   277,   -86,  2228,  2228,
    2228,  2228,   292,   298,   -86,   -86,   415,   -86,   415,   -86,
     415,   -86,   415,   -86,   282,   -86,   126,   -86,   268,   -86,
     268,   -86,   415,   -86,   415,   -86,   254,   -86,   254,   -86,
     254,   -86,   254,   -86,   254,   -86,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,
     -86,   317,   114,   142,   -86,   -86,   -86,  1278,   -86,   -86,
     -86,  1153,   -86,  2175,   -86,  1237,   -86,  1237,   -86,  2329,
     -86,  2329,   150,   110,   -86,   155,   143,   -86,   168,   -86,
     133,   322,   174,   181,   -86,  2231,   -86,  2280,   -86,  2329,
     -86,  2329,   -86,   -86,    65,   -86,   -86,   -86,    63,   -86,
     140,   -86,    22,   -86,    22,   -86,   614,   -86,   614,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   300,  2228,  2228,   -86,   -86,   -86,
     -86,   330,  2228,  2228,   227,   -86,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   331,   239,   241,   -86,   -86,   -86,
     -86,  2007,  2039,   -86,   -86,  2071,  2103,   -86,   -86,   -86,
     246,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,
     -86,   -86
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     0,     0,     0,   110,   109,
     215,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      59,    63,    64,    18,     5,     3,    60,   141,   204,   142,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   231,   232,    17,    15,   216,
       0,     0,    68,     0,     0,    39,    40,    42,     0,     0,
       0,     0,    57,    58,    13,    11,    41,    91,     0,    65,
       0,    67,     0,   118,     0,     9,     7,   117,     0,     1,
      19,     0,     0,     0,     0,   166,   165,   179,   178,   192,
     191,   194,   193,   203,   202,    25,     0,     0,     0,    25,
       0,     0,    25,     0,     0,    62,    61,     0,     0,     0,
       0,   206,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     2,     4,   217,     0,     0,     0,     0,
       0,   223,   224,   225,     0,     0,     0,     0,     0,    14,
      16,     0,     0,     0,    66,    25,     0,    20,    23,     0,
       0,    43,     0,    84,    83,    25,     0,    54,    88,    87,
       0,     0,     0,     0,     0,     0,    46,    47,     0,     0,
       0,     0,     0,    10,     0,    12,    95,    92,    93,    94,
       0,     0,     0,   140,   139,     0,     0,     0,     0,     0,
       0,     0,     6,     8,   113,   116,     0,   111,   114,   105,
     108,     0,   103,   214,   106,     0,     0,   198,     0,     0,
     201,     0,     0,     0,     0,    20,     0,   175,     0,     0,
       0,     0,    20,     0,   188,   207,   205,   209,   208,   211,
     210,   213,   212,   150,   149,   152,   151,   144,   143,   146,
     145,   154,   153,   156,   155,   158,   157,   160,   159,   162,
     161,   164,   163,   148,   147,   218,   219,   220,   221,   222,
     226,   227,   228,   229,   230,    70,    72,    69,    71,    44,
      30,    20,     0,     0,    21,    29,    45,     0,    55,    56,
      78,    77,    97,    96,    74,    73,    76,    75,    99,    98,
     101,   100,    25,     0,    48,    25,     0,    51,    25,    35,
       0,    20,     0,     0,    80,    79,    82,    81,    86,    85,
      90,    89,   121,   124,     0,   119,   122,   132,   131,   134,
     133,   126,   125,   128,   127,   136,   135,   138,   137,   130,
     129,   112,   115,   104,   107,   197,   200,   196,   195,   199,
     176,   177,   167,   168,     0,     0,     0,   189,   190,   184,
     185,     0,     0,     0,     0,    27,    28,   102,    49,    50,
      52,    53,    37,    36,    20,     0,     0,    34,    38,   120,
     123,     0,     0,   171,   172,     0,     0,   182,   183,    26,
       0,    32,    33,   173,   169,   174,   170,   186,   180,   187,
     181,    31
  };

  const short
  parser::yypgoto_[] =
  {
     -86,   -86,    73,    17,   -85,   -86,     8,   -48,   -86,   -86,
     -60,   -86,   -86,   -25,   316,     0,   242,   219,   102,   -49,
      -7,   291
  };

  const short
  parser::yydefgoto_[] =
  {
      -1,     5,    24,    25,   148,   149,    98,   151,   176,    66,
      67,   178,   179,    68,    26,    27,    70,    28,    29,    78,
      30,    50
  };

  const short
  parser::yytable_[] =
  {
      82,    80,     6,    69,    77,    86,    88,    90,    92,    94,
     177,    69,   199,   157,     6,   216,   105,   106,   223,    48,
      65,    76,    95,   182,   177,   184,   101,   104,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    79,   194,
     205,   206,   333,   265,   312,   180,   122,   124,    99,   331,
     152,    69,   210,    69,    69,   191,   200,   269,   139,   180,
      69,   272,   208,   209,    96,    97,   150,   140,    69,   156,
     123,   217,    77,   369,    77,    47,    64,    75,   -24,   186,
     187,   188,   177,   177,   177,   302,   334,   175,   195,   207,
     145,   100,   266,   313,   177,   193,   191,   224,   332,   177,
     226,   228,   230,   232,   -24,   234,   236,   238,   240,   242,
     244,   246,   248,   250,   252,   254,   275,   180,   180,   180,
     294,   297,   370,   213,   214,   155,   102,   215,   270,   180,
     111,   -22,   271,   146,   180,   147,   318,   320,   322,   324,
     326,   328,   330,   114,   115,   276,   -24,   118,   119,   120,
     121,   278,    69,     7,   279,     8,   355,   187,   188,   122,
      69,    69,    69,    69,    69,    69,   -22,   359,   146,   103,
     147,    69,    69,   191,    69,   363,   293,   296,   303,   364,
      69,   -24,   -24,   292,   356,    77,    77,    77,    77,    77,
      77,    77,   358,   107,   108,   109,   110,   360,   335,   336,
     361,   338,   339,   295,   340,   341,   342,   343,   211,   212,
     362,   347,   348,   349,   350,   365,   367,   177,     6,   218,
     219,   177,    71,   177,   -22,   177,   146,   177,   147,   177,
      71,   177,   185,   186,   187,   188,   189,   190,   368,   -24,
       1,     2,     3,     4,   -22,   177,   146,   177,   147,   177,
     191,   177,   180,    84,   273,   274,   180,     6,   180,   -24,
     180,     0,   180,    72,   180,    73,   180,   220,   221,   379,
      71,   222,    71,    71,   192,   118,   119,   120,   121,    71,
     180,   381,   180,   382,   180,    74,   180,    71,   391,   118,
     119,   120,   121,   142,     0,   143,   144,     0,   113,   114,
     115,   122,   154,   118,   119,   120,   121,     0,    20,     0,
     159,    21,    22,    23,     0,   122,   344,   274,    49,   345,
     346,     0,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   351,   274,   373,   374,
     352,   353,   371,   372,   298,   377,   378,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      49,   354,   274,     0,   384,   386,   366,   274,   388,   390,
       0,    71,   375,   376,     0,   380,   274,     0,     0,    71,
      71,    71,    71,    71,    71,   299,     0,   300,     0,   301,
      71,    71,     0,    71,   277,     0,     0,     0,     0,    71,
     -24,     0,   281,   283,   285,   287,   289,   291,     6,     0,
       0,     0,     0,   305,   307,     0,   309,   255,   256,   257,
     258,   259,   311,     0,     0,   260,   261,   262,   263,   264,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
       0,   181,    49,    49,    49,    49,    49,    72,   122,    73,
      49,    49,    49,    49,    49,     0,     0,     0,     0,    20,
       0,     0,     0,     0,    23,     0,     0,     0,     0,    74,
       0,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,   201,     0,     0,
       0,     0,    20,     0,     0,    21,    22,     0,   201,   202,
     203,   160,   161,   162,   163,   164,   165,     0,     0,     0,
     202,     0,   160,   161,   162,   163,   164,   165,     0,    55,
      56,   166,    57,   167,    58,   168,   169,   170,     0,     0,
      55,    56,   166,    57,   167,    58,   168,   169,   170,   171,
     172,     0,     0,   204,     0,     0,   174,    61,    62,    63,
     171,   172,     6,     0,   204,     0,     0,   174,    61,    62,
      63,     0,     0,     0,     0,     0,   160,   161,   162,   163,
     164,   165,     6,     0,     0,     0,     0,     0,     7,     0,
       8,     9,    10,    11,    55,    56,   166,    57,   167,    58,
     168,   169,   170,     0,     0,     0,    12,    13,    14,    15,
      16,    17,    18,    19,   171,   172,     0,     0,   173,     6,
       0,   174,    61,    62,    63,    51,     0,    52,     9,     0,
      53,     0,     0,    20,     0,     0,    21,    22,    23,   185,
     186,   187,   188,   189,   190,     0,     0,    54,     0,     0,
       0,    55,    56,     0,    57,    83,    58,   191,     0,     0,
       0,    51,     0,    52,     9,     0,    53,     0,     0,    59,
      20,     0,     0,    21,    22,    23,     0,     0,    60,    61,
      62,    63,     0,    54,     0,     0,     0,    55,    56,     0,
      57,   141,    58,     0,     0,     0,     0,    51,     0,    52,
       9,     0,    53,     0,     0,    59,    20,     0,     0,    21,
      22,     0,     0,     0,    60,    61,    62,    63,     0,    54,
       0,     0,     0,    55,    56,     0,    57,   153,    58,     0,
       0,     0,     0,    51,     0,    52,     9,     0,    53,     0,
       0,    59,    20,     0,     0,    21,    22,     0,     0,     0,
      60,    61,    62,    63,     0,    54,     0,     0,     0,    55,
      56,     0,    57,   158,    58,     0,     0,     0,     0,    51,
       0,    52,     9,     0,    53,     0,     0,    59,    20,     0,
       0,    21,    22,     0,     0,     0,    60,    61,    62,    63,
       0,    54,     0,     0,     0,    55,    56,     0,    57,   280,
      58,     0,     0,     0,     0,    51,     0,    52,     9,     0,
      53,     0,     0,    59,    20,     0,     0,    21,    22,     0,
       0,     0,    60,    61,    62,    63,     0,    54,     0,     0,
       0,    55,    56,     0,    57,   282,    58,     0,     0,     0,
       0,    51,     0,    52,     9,     0,    53,     0,     0,    59,
      20,     0,     0,    21,    22,     0,     0,     0,    60,    61,
      62,    63,     0,    54,     0,     0,     0,    55,    56,     0,
      57,   284,    58,     0,     0,     0,     0,    51,     0,    52,
       9,     0,    53,     0,     0,    59,    20,     0,     0,    21,
      22,     0,     0,     0,    60,    61,    62,    63,     0,    54,
       0,     0,     0,    55,    56,     0,    57,   286,    58,     0,
       0,     0,     0,    51,     0,    52,     9,     0,    53,     0,
       0,    59,    20,     0,     0,    21,    22,     0,     0,     0,
      60,    61,    62,    63,     0,    54,     0,     0,     0,    55,
      56,     0,    57,   288,    58,     0,     0,     0,     0,    51,
       0,    52,     9,     0,    53,     0,     0,    59,    20,     0,
       0,    21,    22,     0,     0,     0,    60,    61,    62,    63,
       0,    54,     0,     0,     0,    55,    56,     0,    57,   290,
      58,     0,     0,     0,     0,    51,     0,    52,     9,     0,
      53,     0,     0,    59,    20,     0,     0,    21,    22,     0,
       0,     0,    60,    61,    62,    63,     0,    54,     0,     0,
       0,    55,    56,     0,    57,   304,    58,     0,     0,     0,
       0,    51,     0,    52,     9,     0,    53,     0,     0,    59,
      20,     0,     0,    21,    22,     0,     0,     0,    60,    61,
      62,    63,     0,    54,     0,     0,     0,    55,    56,     0,
      57,   306,    58,     0,     0,     0,     0,    51,     0,    52,
       9,     0,    53,     0,     0,    59,    20,     0,     0,    21,
      22,     0,     0,     0,    60,    61,    62,    63,     0,    54,
       0,     0,     0,    55,    56,     0,    57,   308,    58,     0,
       0,     0,     0,    51,     0,    52,     9,     0,    53,     0,
       0,    59,    20,     0,     0,    21,    22,     0,   183,     0,
      60,    61,    62,    63,    72,    54,    73,     0,     0,    55,
      56,     0,    57,   310,    58,     0,     0,     0,     0,    51,
       0,    52,     9,     0,    53,     0,    74,    59,    20,     0,
       0,    21,    22,     0,     0,     0,    60,    61,    62,    63,
       0,    54,     0,     0,     0,    55,    56,     0,    57,    20,
      58,     0,    21,    22,    51,     0,    52,     9,     0,    53,
       0,     0,     0,    59,    20,     0,     0,    21,    22,   161,
     162,   163,    60,    61,    62,    63,    54,     0,     0,     0,
      55,    56,     0,    57,     0,    58,    55,    56,   166,    57,
     167,    58,   168,   169,   170,     0,     0,   196,    59,    20,
       0,     0,    21,    22,   197,     0,     0,    60,    61,    62,
      63,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,     0,     0,   314,     0,     0,     0,     0,     0,   122,
     315,     0,     0,     0,     0,     0,   267,   185,   186,   187,
     188,   189,   190,   160,   161,   162,   163,   164,   165,     0,
       0,     0,     0,   198,     0,   191,     0,     0,     0,     0,
       0,    55,    56,   166,    57,   167,    58,   168,   169,   170,
      55,    56,   166,    57,   167,    58,   168,   169,   170,   316,
       0,   171,   172,     0,     0,   268,   357,     0,   174,    61,
      62,    63,     0,   160,   161,   162,   163,   164,   165,    81,
       0,     0,     0,     0,     0,     7,     0,     8,     9,    10,
      11,    55,    56,   166,    57,   167,    58,   168,   169,   170,
       0,     0,     0,    12,    13,    14,    15,    16,    17,    18,
      19,   171,   172,     0,     0,     0,    85,     0,   174,    61,
      62,    63,     7,     0,     8,     9,    10,    11,     0,     0,
      20,     0,     0,    21,    22,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    87,     0,
       0,     0,     0,     0,     7,     0,     8,     9,    10,    11,
       0,     0,     0,     0,     0,     0,     0,    20,     0,     0,
      21,    22,    12,    13,    14,    15,    16,    17,    18,    19,
      89,     0,     0,     0,     0,     0,     7,     0,     8,     9,
      10,    11,     0,     0,     0,     0,     0,     0,     0,    20,
       0,     0,    21,    22,    12,    13,    14,    15,    16,    17,
      18,    19,    91,     0,     0,     0,     0,     0,     7,     0,
       8,     9,    10,    11,     0,     0,     0,     0,     0,     0,
       0,    20,     0,     0,    21,    22,    12,    13,    14,    15,
      16,    17,    18,    19,    93,     0,     0,     0,     0,     0,
       7,     0,     8,     9,    10,    11,     0,     0,     0,     0,
       0,     0,     0,    20,     0,     0,    21,    22,    12,    13,
      14,    15,    16,    17,    18,    19,   225,     0,     0,     0,
       0,     0,     7,     0,     8,     9,    10,    11,     0,     0,
       0,     0,     0,     0,     0,    20,     0,     0,    21,    22,
      12,    13,    14,    15,    16,    17,    18,    19,   227,     0,
       0,     0,     0,     0,     7,     0,     8,     9,    10,    11,
       0,     0,     0,     0,     0,     0,     0,    20,     0,     0,
      21,    22,    12,    13,    14,    15,    16,    17,    18,    19,
     229,     0,     0,     0,     0,     0,     7,     0,     8,     9,
      10,    11,     0,     0,     0,     0,     0,     0,     0,    20,
       0,     0,    21,    22,    12,    13,    14,    15,    16,    17,
      18,    19,   231,     0,     0,     0,     0,     0,     7,     0,
       8,     9,    10,    11,     0,     0,     0,     0,     0,     0,
       0,    20,     0,     0,    21,    22,    12,    13,    14,    15,
      16,    17,    18,    19,   233,     0,     0,     0,     0,     0,
       7,     0,     8,     9,    10,    11,     0,     0,     0,     0,
       0,     0,     0,    20,     0,     0,    21,    22,    12,    13,
      14,    15,    16,    17,    18,    19,   235,     0,     0,     0,
       0,     0,     7,     0,     8,     9,    10,    11,     0,     0,
       0,     0,     0,     0,     0,    20,     0,     0,    21,    22,
      12,    13,    14,    15,    16,    17,    18,    19,   237,     0,
       0,     0,     0,     0,     7,     0,     8,     9,    10,    11,
       0,     0,     0,     0,     0,     0,     0,    20,     0,     0,
      21,    22,    12,    13,    14,    15,    16,    17,    18,    19,
     239,     0,     0,     0,     0,     0,     7,     0,     8,     9,
      10,    11,     0,     0,     0,     0,     0,     0,     0,    20,
       0,     0,    21,    22,    12,    13,    14,    15,    16,    17,
      18,    19,   241,     0,     0,     0,     0,     0,     7,     0,
       8,     9,    10,    11,     0,     0,     0,     0,     0,     0,
       0,    20,     0,     0,    21,    22,    12,    13,    14,    15,
      16,    17,    18,    19,   243,     0,     0,     0,     0,     0,
       7,     0,     8,     9,    10,    11,     0,     0,     0,     0,
       0,     0,     0,    20,     0,     0,    21,    22,    12,    13,
      14,    15,    16,    17,    18,    19,   245,     0,     0,     0,
       0,     0,     7,     0,     8,     9,    10,    11,     0,     0,
       0,     0,     0,     0,     0,    20,     0,     0,    21,    22,
      12,    13,    14,    15,    16,    17,    18,    19,   247,     0,
       0,     0,     0,     0,     7,     0,     8,     9,    10,    11,
       0,     0,     0,     0,     0,     0,     0,    20,     0,     0,
      21,    22,    12,    13,    14,    15,    16,    17,    18,    19,
     249,     0,     0,     0,     0,     0,     7,     0,     8,     9,
      10,    11,     0,     0,     0,     0,     0,     0,     0,    20,
       0,     0,    21,    22,    12,    13,    14,    15,    16,    17,
      18,    19,   251,     0,     0,     0,     0,     0,     7,     0,
       8,     9,    10,    11,     0,     0,     0,     0,     0,     0,
       0,    20,     0,     0,    21,    22,    12,    13,    14,    15,
      16,    17,    18,    19,   253,     0,     0,     0,     0,     0,
       7,     0,     8,     9,    10,    11,     0,     0,     0,     0,
       0,     0,     0,    20,     0,     0,    21,    22,    12,    13,
      14,    15,    16,    17,    18,    19,   337,     0,     0,     0,
       0,     0,     7,     0,     8,     9,    10,    11,     0,     0,
       0,     0,     0,     0,     0,    20,     0,     0,    21,    22,
      12,    13,    14,    15,    16,    17,    18,    19,   383,     0,
       0,     0,     0,     0,     7,     0,     8,     9,    10,    11,
       0,     0,     0,     0,     0,     0,     0,    20,     0,     0,
      21,    22,    12,    13,    14,    15,    16,    17,    18,    19,
     385,     0,     0,     0,     0,     0,     7,     0,     8,     9,
      10,    11,     0,     0,     0,     0,     0,     0,     0,    20,
       0,     0,    21,    22,    12,    13,    14,    15,    16,    17,
      18,    19,   387,     0,     0,     0,     0,     0,     7,     0,
       8,     9,    10,    11,     0,     0,     0,     0,     0,     0,
       0,    20,     0,     0,    21,    22,    12,    13,    14,    15,
      16,    17,    18,    19,   389,     0,     0,     0,     0,     0,
       7,   317,     8,     9,    10,    11,   319,    72,     0,    73,
       0,     0,    72,    20,    73,     0,    21,    22,    12,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,    74,
     321,     0,     0,     0,    74,   323,    72,     0,    73,     0,
       0,    72,     0,    73,     0,    20,     0,     0,    21,    22,
       0,     0,    20,     0,     0,    21,    22,    20,    74,   325,
      21,    22,     0,    74,   327,    72,     0,    73,     0,   329,
      72,     0,    73,     0,     0,    72,     0,    73,     0,     0,
       0,    20,   162,   163,    21,    22,    20,    74,     0,    21,
      22,     0,    74,     0,     0,     0,     0,    74,    55,    56,
     166,    57,   167,    58,   168,   169,   170,     0,     0,     0,
      20,     0,     0,    21,    22,    20,     0,     0,    21,    22,
      20,     0,     0,    21,    22,     7,     0,     8,     9,    10,
      11,     0,     0,     0,     0,     0,   160,   161,   162,   163,
     164,   165,     0,    12,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,    55,    56,   166,    57,   167,    58,
     168,   169,   170,     0,     0,     0,     0,     0,     0,     0,
      20,     0,     0,    21,    22,   172,     0,     0,     0,     0,
       0,   174,    61,    62,    63,   160,   161,   162,   163,   164,
     165,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      20,     0,     0,    55,    56,   166,    57,   167,    58,   168,
     169,   170,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,     0,     0,
     174,    61,    62,    63,   160,   161,   162,   163,   164,   165,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    56,   166,    57,   167,    58,   168,   169,
     170
  };

  const short
  parser::yycheck_[] =
  {
       7,    57,     1,     3,     4,    12,    13,    14,    15,    16,
      70,    11,    13,    61,     1,   100,    58,    59,   103,     2,
       3,     4,     1,    72,    84,    74,    18,    19,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,     0,     8,
      42,    43,    13,     8,     8,    70,    33,    30,     1,     8,
       7,    51,    57,    53,    54,    33,    57,    42,    57,    84,
      60,   146,    42,    43,    43,    44,    58,    50,    68,    61,
      57,    57,    72,     8,    74,     2,     3,     4,    57,    16,
      17,    18,   142,   143,   144,   170,    57,    70,    57,    96,
       1,    44,    57,    57,   154,    78,    33,    57,    57,   159,
     107,   108,   109,   110,    57,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,    42,   142,   143,   144,
     168,   169,    57,    42,    43,     1,     1,    46,    42,   154,
      28,    42,    46,    44,   159,    46,   185,   186,   187,   188,
     189,   190,   191,    17,    18,    57,    57,    21,    22,    23,
      24,    42,   152,     7,    57,     9,    42,    17,    18,    33,
     160,   161,   162,   163,   164,   165,    42,    57,    44,    44,
      46,   171,   172,    33,   174,    42,   168,   169,   170,    46,
     180,    57,    57,     1,    42,   185,   186,   187,   188,   189,
     190,   191,    42,    47,    48,    49,    50,    42,   205,   206,
      57,   208,   209,     1,   211,   212,   213,   214,    42,    43,
      42,   218,   219,   220,   221,   300,    42,   277,     1,    42,
      43,   281,     3,   283,    42,   285,    44,   287,    46,   289,
      11,   291,    15,    16,    17,    18,    19,    20,    57,    57,
       3,     4,     5,     6,    42,   305,    44,   307,    46,   309,
      33,   311,   277,    11,    44,    45,   281,     1,   283,    57,
     285,    -1,   287,     7,   289,     9,   291,    42,    43,    42,
      51,    46,    53,    54,    57,    21,    22,    23,    24,    60,
     305,    42,   307,    42,   309,    29,   311,    68,    42,    21,
      22,    23,    24,    51,    -1,    53,    54,    -1,    16,    17,
      18,    33,    60,    21,    22,    23,    24,    -1,    52,    -1,
      68,    55,    56,    57,    -1,    33,    44,    45,     2,    42,
      43,    -1,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    44,    45,   345,   346,
      42,    43,    42,    43,     1,   352,   353,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    44,    45,    -1,   371,   372,    44,    45,   375,   376,
      -1,   152,    42,    43,    -1,    44,    45,    -1,    -1,   160,
     161,   162,   163,   164,   165,    42,    -1,    44,    -1,    46,
     171,   172,    -1,   174,   152,    -1,    -1,    -1,    -1,   180,
      57,    -1,   160,   161,   162,   163,   164,   165,     1,    -1,
      -1,    -1,    -1,   171,   172,    -1,   174,   126,   127,   128,
     129,   130,   180,    -1,    -1,   134,   135,   136,   137,   138,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      -1,     1,   126,   127,   128,   129,   130,     7,    33,     9,
     134,   135,   136,   137,   138,    -1,    -1,    -1,    -1,    52,
      -1,    -1,    -1,    -1,    57,    -1,    -1,    -1,    -1,    29,
      -1,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     1,    -1,    -1,
      -1,    -1,    52,    -1,    -1,    55,    56,    -1,     1,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    -1,    -1,
      13,    -1,    15,    16,    17,    18,    19,    20,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    -1,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    53,
      54,    -1,    -1,    57,    -1,    -1,    60,    61,    62,    63,
      53,    54,     1,    -1,    57,    -1,    -1,    60,    61,    62,
      63,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,
      19,    20,     1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
       9,    10,    11,    12,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    25,    26,    27,    28,
      29,    30,    31,    32,    53,    54,    -1,    -1,    57,     1,
      -1,    60,    61,    62,    63,     7,    -1,     9,    10,    -1,
      12,    -1,    -1,    52,    -1,    -1,    55,    56,    57,    15,
      16,    17,    18,    19,    20,    -1,    -1,    29,    -1,    -1,
      -1,    33,    34,    -1,    36,     1,    38,    33,    -1,    -1,
      -1,     7,    -1,     9,    10,    -1,    12,    -1,    -1,    51,
      52,    -1,    -1,    55,    56,    57,    -1,    -1,    60,    61,
      62,    63,    -1,    29,    -1,    -1,    -1,    33,    34,    -1,
      36,     1,    38,    -1,    -1,    -1,    -1,     7,    -1,     9,
      10,    -1,    12,    -1,    -1,    51,    52,    -1,    -1,    55,
      56,    -1,    -1,    -1,    60,    61,    62,    63,    -1,    29,
      -1,    -1,    -1,    33,    34,    -1,    36,     1,    38,    -1,
      -1,    -1,    -1,     7,    -1,     9,    10,    -1,    12,    -1,
      -1,    51,    52,    -1,    -1,    55,    56,    -1,    -1,    -1,
      60,    61,    62,    63,    -1,    29,    -1,    -1,    -1,    33,
      34,    -1,    36,     1,    38,    -1,    -1,    -1,    -1,     7,
      -1,     9,    10,    -1,    12,    -1,    -1,    51,    52,    -1,
      -1,    55,    56,    -1,    -1,    -1,    60,    61,    62,    63,
      -1,    29,    -1,    -1,    -1,    33,    34,    -1,    36,     1,
      38,    -1,    -1,    -1,    -1,     7,    -1,     9,    10,    -1,
      12,    -1,    -1,    51,    52,    -1,    -1,    55,    56,    -1,
      -1,    -1,    60,    61,    62,    63,    -1,    29,    -1,    -1,
      -1,    33,    34,    -1,    36,     1,    38,    -1,    -1,    -1,
      -1,     7,    -1,     9,    10,    -1,    12,    -1,    -1,    51,
      52,    -1,    -1,    55,    56,    -1,    -1,    -1,    60,    61,
      62,    63,    -1,    29,    -1,    -1,    -1,    33,    34,    -1,
      36,     1,    38,    -1,    -1,    -1,    -1,     7,    -1,     9,
      10,    -1,    12,    -1,    -1,    51,    52,    -1,    -1,    55,
      56,    -1,    -1,    -1,    60,    61,    62,    63,    -1,    29,
      -1,    -1,    -1,    33,    34,    -1,    36,     1,    38,    -1,
      -1,    -1,    -1,     7,    -1,     9,    10,    -1,    12,    -1,
      -1,    51,    52,    -1,    -1,    55,    56,    -1,    -1,    -1,
      60,    61,    62,    63,    -1,    29,    -1,    -1,    -1,    33,
      34,    -1,    36,     1,    38,    -1,    -1,    -1,    -1,     7,
      -1,     9,    10,    -1,    12,    -1,    -1,    51,    52,    -1,
      -1,    55,    56,    -1,    -1,    -1,    60,    61,    62,    63,
      -1,    29,    -1,    -1,    -1,    33,    34,    -1,    36,     1,
      38,    -1,    -1,    -1,    -1,     7,    -1,     9,    10,    -1,
      12,    -1,    -1,    51,    52,    -1,    -1,    55,    56,    -1,
      -1,    -1,    60,    61,    62,    63,    -1,    29,    -1,    -1,
      -1,    33,    34,    -1,    36,     1,    38,    -1,    -1,    -1,
      -1,     7,    -1,     9,    10,    -1,    12,    -1,    -1,    51,
      52,    -1,    -1,    55,    56,    -1,    -1,    -1,    60,    61,
      62,    63,    -1,    29,    -1,    -1,    -1,    33,    34,    -1,
      36,     1,    38,    -1,    -1,    -1,    -1,     7,    -1,     9,
      10,    -1,    12,    -1,    -1,    51,    52,    -1,    -1,    55,
      56,    -1,    -1,    -1,    60,    61,    62,    63,    -1,    29,
      -1,    -1,    -1,    33,    34,    -1,    36,     1,    38,    -1,
      -1,    -1,    -1,     7,    -1,     9,    10,    -1,    12,    -1,
      -1,    51,    52,    -1,    -1,    55,    56,    -1,     1,    -1,
      60,    61,    62,    63,     7,    29,     9,    -1,    -1,    33,
      34,    -1,    36,     1,    38,    -1,    -1,    -1,    -1,     7,
      -1,     9,    10,    -1,    12,    -1,    29,    51,    52,    -1,
      -1,    55,    56,    -1,    -1,    -1,    60,    61,    62,    63,
      -1,    29,    -1,    -1,    -1,    33,    34,    -1,    36,    52,
      38,    -1,    55,    56,     7,    -1,     9,    10,    -1,    12,
      -1,    -1,    -1,    51,    52,    -1,    -1,    55,    56,    16,
      17,    18,    60,    61,    62,    63,    29,    -1,    -1,    -1,
      33,    34,    -1,    36,    -1,    38,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    -1,    -1,     1,    51,    52,
      -1,    -1,    55,    56,     8,    -1,    -1,    60,    61,    62,
      63,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,     1,    -1,    -1,    -1,    -1,    -1,    33,
       8,    -1,    -1,    -1,    -1,    -1,     8,    15,    16,    17,
      18,    19,    20,    15,    16,    17,    18,    19,    20,    -1,
      -1,    -1,    -1,    57,    -1,    33,    -1,    -1,    -1,    -1,
      -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    57,
      -1,    53,    54,    -1,    -1,    57,     8,    -1,    60,    61,
      62,    63,    -1,    15,    16,    17,    18,    19,    20,     1,
      -1,    -1,    -1,    -1,    -1,     7,    -1,     9,    10,    11,
      12,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      -1,    -1,    -1,    25,    26,    27,    28,    29,    30,    31,
      32,    53,    54,    -1,    -1,    -1,     1,    -1,    60,    61,
      62,    63,     7,    -1,     9,    10,    11,    12,    -1,    -1,
      52,    -1,    -1,    55,    56,    -1,    -1,    -1,    -1,    -1,
      25,    26,    27,    28,    29,    30,    31,    32,     1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,
      55,    56,    25,    26,    27,    28,    29,    30,    31,    32,
       1,    -1,    -1,    -1,    -1,    -1,     7,    -1,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,
      -1,    -1,    55,    56,    25,    26,    27,    28,    29,    30,
      31,    32,     1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
       9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    55,    56,    25,    26,    27,    28,
      29,    30,    31,    32,     1,    -1,    -1,    -1,    -1,    -1,
       7,    -1,     9,    10,    11,    12,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    -1,    -1,    55,    56,    25,    26,
      27,    28,    29,    30,    31,    32,     1,    -1,    -1,    -1,
      -1,    -1,     7,    -1,     9,    10,    11,    12,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    55,    56,
      25,    26,    27,    28,    29,    30,    31,    32,     1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,
      55,    56,    25,    26,    27,    28,    29,    30,    31,    32,
       1,    -1,    -1,    -1,    -1,    -1,     7,    -1,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,
      -1,    -1,    55,    56,    25,    26,    27,    28,    29,    30,
      31,    32,     1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
       9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    55,    56,    25,    26,    27,    28,
      29,    30,    31,    32,     1,    -1,    -1,    -1,    -1,    -1,
       7,    -1,     9,    10,    11,    12,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    -1,    -1,    55,    56,    25,    26,
      27,    28,    29,    30,    31,    32,     1,    -1,    -1,    -1,
      -1,    -1,     7,    -1,     9,    10,    11,    12,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    55,    56,
      25,    26,    27,    28,    29,    30,    31,    32,     1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,
      55,    56,    25,    26,    27,    28,    29,    30,    31,    32,
       1,    -1,    -1,    -1,    -1,    -1,     7,    -1,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,
      -1,    -1,    55,    56,    25,    26,    27,    28,    29,    30,
      31,    32,     1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
       9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    55,    56,    25,    26,    27,    28,
      29,    30,    31,    32,     1,    -1,    -1,    -1,    -1,    -1,
       7,    -1,     9,    10,    11,    12,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    -1,    -1,    55,    56,    25,    26,
      27,    28,    29,    30,    31,    32,     1,    -1,    -1,    -1,
      -1,    -1,     7,    -1,     9,    10,    11,    12,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    55,    56,
      25,    26,    27,    28,    29,    30,    31,    32,     1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,
      55,    56,    25,    26,    27,    28,    29,    30,    31,    32,
       1,    -1,    -1,    -1,    -1,    -1,     7,    -1,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,
      -1,    -1,    55,    56,    25,    26,    27,    28,    29,    30,
      31,    32,     1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
       9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    55,    56,    25,    26,    27,    28,
      29,    30,    31,    32,     1,    -1,    -1,    -1,    -1,    -1,
       7,    -1,     9,    10,    11,    12,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    -1,    -1,    55,    56,    25,    26,
      27,    28,    29,    30,    31,    32,     1,    -1,    -1,    -1,
      -1,    -1,     7,    -1,     9,    10,    11,    12,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    55,    56,
      25,    26,    27,    28,    29,    30,    31,    32,     1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,
      55,    56,    25,    26,    27,    28,    29,    30,    31,    32,
       1,    -1,    -1,    -1,    -1,    -1,     7,    -1,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,
      -1,    -1,    55,    56,    25,    26,    27,    28,    29,    30,
      31,    32,     1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
       9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    55,    56,    25,    26,    27,    28,
      29,    30,    31,    32,     1,    -1,    -1,    -1,    -1,    -1,
       7,     1,     9,    10,    11,    12,     1,     7,    -1,     9,
      -1,    -1,     7,    52,     9,    -1,    55,    56,    25,    26,
      27,    28,    29,    30,    31,    32,    -1,    -1,    -1,    29,
       1,    -1,    -1,    -1,    29,     1,     7,    -1,     9,    -1,
      -1,     7,    -1,     9,    -1,    52,    -1,    -1,    55,    56,
      -1,    -1,    52,    -1,    -1,    55,    56,    52,    29,     1,
      55,    56,    -1,    29,     1,     7,    -1,     9,    -1,     1,
       7,    -1,     9,    -1,    -1,     7,    -1,     9,    -1,    -1,
      -1,    52,    17,    18,    55,    56,    52,    29,    -1,    55,
      56,    -1,    29,    -1,    -1,    -1,    -1,    29,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      52,    -1,    -1,    55,    56,    52,    -1,    -1,    55,    56,
      52,    -1,    -1,    55,    56,     7,    -1,     9,    10,    11,
      12,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,
      19,    20,    -1,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,    55,    56,    54,    -1,    -1,    -1,    -1,
      -1,    60,    61,    62,    63,    15,    16,    17,    18,    19,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    -1,    -1,
      60,    61,    62,    63,    15,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    34,    35,    36,    37,    38,    39,    40,
      41
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,     3,     4,     5,     6,    81,     1,     7,     9,    10,
      11,    12,    25,    26,    27,    28,    29,    30,    31,    32,
      52,    55,    56,    57,    82,    83,    94,    95,    97,    98,
     100,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    82,    83,    94,
     101,     7,     9,    12,    29,    33,    34,    36,    38,    51,
      60,    61,    62,    63,    82,    83,    89,    90,    93,    95,
      96,    97,     7,     9,    29,    82,    83,    95,    99,     0,
      57,     1,   100,     1,    96,     1,   100,     1,   100,     1,
     100,     1,   100,     1,   100,     1,    43,    44,    86,     1,
      44,    86,     1,    44,    86,    58,    59,    47,    48,    49,
      50,    98,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    33,    57,    83,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,    57,
      83,     1,    96,    96,    96,     1,    44,    46,    84,    85,
      86,    87,     7,     1,    96,     1,    86,    87,     1,    96,
      15,    16,    17,    18,    19,    20,    35,    37,    39,    40,
      41,    53,    54,    57,    60,    83,    88,    90,    91,    92,
      93,     1,    99,     1,    99,    15,    16,    17,    18,    19,
      20,    33,    57,    83,     8,    57,     1,     8,    57,    13,
      57,     1,    13,    14,    57,    42,    43,   100,    42,    43,
      57,    42,    43,    42,    43,    46,    84,    57,    42,    43,
      42,    43,    46,    84,    57,     1,   100,     1,   100,     1,
     100,     1,   100,     1,   100,     1,   100,     1,   100,     1,
     100,     1,   100,     1,   100,     1,   100,     1,   100,     1,
     100,     1,   100,     1,   100,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,     8,    57,     8,    57,    42,
      42,    46,    84,    44,    45,    42,    57,    96,    42,    57,
       1,    96,     1,    96,     1,    96,     1,    96,     1,    96,
       1,    96,     1,    86,    87,     1,    86,    87,     1,    42,
      44,    46,    84,    86,     1,    96,     1,    96,     1,    96,
       1,    96,     8,    57,     1,     8,    57,     1,    99,     1,
      99,     1,    99,     1,    99,     1,    99,     1,    99,     1,
      99,     8,    57,    13,    57,   100,   100,     1,   100,   100,
     100,   100,   100,   100,    44,    42,    43,   100,   100,   100,
     100,    44,    42,    43,    44,    42,    42,     8,    42,    57,
      42,    57,    42,    42,    46,    84,    44,    42,    57,     8,
      57,    42,    43,   100,   100,    42,    43,   100,   100,    42,
      44,    42,    42,     1,   100,     1,   100,     1,   100,     1,
     100,    42
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    80,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    82,    83,
      84,    84,    85,    85,    86,    86,    87,    87,    87,    87,
      87,    88,    88,    88,    88,    88,    88,    88,    88,    89,
      89,    90,    90,    90,    90,    90,    91,    91,    91,    91,
      91,    92,    92,    92,    93,    93,    93,    93,    93,    94,
      95,    95,    95,    95,    95,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    97,    97,    97,    97,    97,    97,    97,
      98,    98,    98,    98,    98,    98,    98,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101
  };

  const unsigned char
  parser::yyr2_[] =
  {
       0,     2,     3,     2,     3,     2,     3,     2,     3,     2,
       3,     2,     3,     2,     3,     2,     3,     2,     1,     2,
       1,     2,     0,     1,     0,     1,     4,     3,     3,     2,
       2,     5,     4,     4,     3,     2,     3,     3,     3,     1,
       1,     1,     1,     2,     3,     3,     1,     1,     2,     3,
       3,     2,     3,     3,     2,     3,     3,     1,     1,     1,
       1,     2,     2,     1,     1,     1,     2,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     3,     3,     2,     2,     3,
       3,     1,     2,     2,     2,     2,     3,     3,     3,     3,
       3,     3,     4,     3,     4,     3,     3,     4,     3,     1,
       1,     3,     4,     3,     3,     4,     3,     1,     1,     3,
       4,     3,     3,     4,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     4,     4,     6,
       6,     5,     5,     6,     6,     3,     4,     4,     2,     2,
       6,     6,     5,     5,     4,     4,     6,     6,     3,     4,
       4,     2,     2,     2,     2,     4,     4,     4,     3,     4,
       4,     3,     2,     2,     1,     3,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     2,     3,     3,
       3,     3,     3,     2,     2,     2,     3,     3,     3,     3,
       3,     1,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "$end", "error", "$undefined", "\"LTL start marker\"",
  "\"LBT start marker\"", "\"SERE start marker\"",
  "\"BOOLEAN start marker\"", "\"opening parenthesis\"",
  "\"closing parenthesis\"", "\"(...) block\"", "\"{...} block\"",
  "\"{...}! block\"", "\"opening brace\"", "\"closing brace\"",
  "\"closing brace-bang\"", "\"or operator\"", "\"xor operator\"",
  "\"and operator\"", "\"short and operator\"", "\"implication operator\"",
  "\"equivalent operator\"", "\"until operator\"", "\"release operator\"",
  "\"weak until operator\"", "\"strong release operator\"",
  "\"sometimes operator\"", "\"always operator\"", "\"next operator\"",
  "\"strong next operator\"", "\"not operator\"", "\"X[.] operator\"",
  "\"F[.] operator\"", "\"G[.] operator\"", "\"star operator\"",
  "\"bracket star operator\"", "\"bracket fusion-star operator\"",
  "\"plus operator\"", "\"fusion-plus operator\"",
  "\"opening bracket for star operator\"",
  "\"opening bracket for fusion-star operator\"",
  "\"opening bracket for equal operator\"",
  "\"opening bracket for goto operator\"", "\"closing bracket\"",
  "\"closing !]\"", "\"number for square bracket operator\"",
  "\"unbounded mark\"", "\"separator for square bracket operator\"",
  "\"universal concat operator\"", "\"existential concat operator\"",
  "\"universal non-overlapping concat operator\"",
  "\"existential non-overlapping concat operator\"", "\"first_match\"",
  "\"atomic proposition\"", "\"concat operator\"", "\"fusion operator\"",
  "\"constant true\"", "\"constant false\"", "\"end of formula\"",
  "\"negative suffix\"", "\"positive suffix\"", "\"SVA delay operator\"",
  "\"opening bracket for SVA delay operator\"", "\"##[+] operator\"",
  "\"##[*] operator\"", "'!'", "'&'", "'|'", "'^'", "'i'", "'e'", "'X'",
  "'F'", "'G'", "'U'", "'V'", "'R'", "'W'", "'M'", "'t'", "'f'", "$accept",
  "result", "emptyinput", "enderror", "OP_SQBKT_SEP_unbounded",
  "OP_SQBKT_SEP_opt", "error_opt", "sqbracketargs", "gotoargs",
  "kleen_star", "starargs", "fstarargs", "equalargs", "delayargs",
  "atomprop", "booleanatom", "sere", "bracedsere", "parenthesedsubformula",
  "boolformula", "subformula", "lbtformula", YY_NULLPTR
  };

#if TLYYDEBUG
  const unsigned short
  parser::yyrline_[] =
  {
       0,   293,   293,   298,   303,   308,   310,   315,   320,   325,
     327,   332,   337,   342,   344,   349,   354,   359,   362,   368,
     374,   374,   375,   376,   377,   378,   381,   383,   385,   387,
     389,   393,   395,   397,   399,   401,   403,   405,   408,   413,
     413,   415,   417,   419,   421,   424,   428,   430,   432,   434,
     438,   443,   445,   448,   453,   455,   458,   462,   464,   467,
     475,   476,   477,   481,   483,   486,   487,   499,   500,   509,
     511,   517,   521,   527,   529,   532,   534,   537,   539,   541,
     543,   545,   547,   549,   551,   553,   556,   558,   568,   570,
     580,   582,   591,   600,   609,   627,   645,   659,   661,   674,
     676,   690,   692,   695,   697,   701,   706,   711,   716,   722,
     732,   740,   742,   746,   751,   755,   760,   768,   769,   778,
     780,   784,   789,   794,   799,   805,   807,   809,   811,   813,
     815,   817,   819,   821,   823,   825,   827,   829,   831,   833,
     835,   838,   839,   840,   842,   844,   846,   848,   850,   852,
     854,   856,   858,   860,   862,   864,   866,   868,   870,   872,
     874,   876,   878,   880,   882,   884,   886,   888,   893,   899,
     902,   906,   910,   914,   917,   920,   923,   927,   931,   933,
     935,   938,   942,   946,   950,   955,   962,   965,   968,   971,
     975,   979,   981,   983,   985,   987,   989,   991,   994,   996,
    1000,  1003,  1006,  1008,  1010,  1012,  1014,  1016,  1019,  1021,
    1025,  1030,  1034,  1039,  1043,  1046,  1057,  1058,  1060,  1062,
    1064,  1066,  1068,  1070,  1072,  1074,  1076,  1078,  1080,  1082,
    1084,  1086,  1088
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
#endif // TLYYDEBUG

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
       2,     2,     2,    64,     2,     2,     2,     2,    65,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      71,    72,     2,     2,     2,     2,     2,    77,     2,     2,
       2,     2,    75,     2,     2,    73,    74,    76,    70,     2,
       2,     2,     2,     2,    67,     2,     2,     2,     2,     2,
       2,    69,    79,     2,     2,    68,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    78,     2,     2,     2,
       2,     2,     2,     2,    66,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63
    };
    const unsigned user_token_number_max_ = 318;
    const token_number_type undef_token_ = 2;

    if (static_cast<int> (t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // tlyy
#line 3735 "parsetl.cc" // lalr1.cc:1242
#line 1092 "parsetl.yy" // lalr1.cc:1243


void
tlyy::parser::error(const location_type& location, const std::string& message)
{
  error_list.emplace_back(location, message);
}

namespace spot
{
  parsed_formula
  parse_infix_psl(const std::string& ltl_string,
		  environment& env,
		  bool debug, bool lenient)
  {
    parsed_formula result(ltl_string);
    flex_set_buffer(ltl_string,
		    tlyy::parser::token::START_LTL,
		    lenient);
    tlyy::parser parser(result.errors, env, result.f);
    parser.set_debug_level(debug);
    parser.parse();
    flex_unset_buffer();
    return result;
  }

  parsed_formula
  parse_infix_boolean(const std::string& ltl_string,
		      environment& env,
		      bool debug, bool lenient)
  {
    parsed_formula result(ltl_string);
    flex_set_buffer(ltl_string,
		    tlyy::parser::token::START_BOOL,
		    lenient);
    tlyy::parser parser(result.errors, env, result.f);
    parser.set_debug_level(debug);
    parser.parse();
    flex_unset_buffer();
    return result;
  }

  parsed_formula
  parse_prefix_ltl(const std::string& ltl_string,
		   environment& env,
		   bool debug)
  {
    parsed_formula result(ltl_string);
    flex_set_buffer(ltl_string,
		    tlyy::parser::token::START_LBT,
		    false);
    tlyy::parser parser(result.errors, env, result.f);
    parser.set_debug_level(debug);
    parser.parse();
    flex_unset_buffer();
    return result;
  }

  parsed_formula
  parse_infix_sere(const std::string& sere_string,
		   environment& env,
		   bool debug,
		   bool lenient)
  {
    parsed_formula result(sere_string);
    flex_set_buffer(sere_string,
		    tlyy::parser::token::START_SERE,
		    lenient);
    tlyy::parser parser(result.errors, env, result.f);
    parser.set_debug_level(debug);
    parser.parse();
    flex_unset_buffer();
    return result;
  }

  formula
  parse_formula(const std::string& ltl_string, environment& env)
  {
    parsed_formula pf = parse_infix_psl(ltl_string, env);
    std::ostringstream s;
    if (pf.format_errors(s))
      {
	parsed_formula pg = parse_prefix_ltl(ltl_string, env);
	if (pg.errors.empty())
	  return pg.f;
	else
	  throw parse_error(s.str());
      }
    return pf.f;
  }
}

// Local Variables:
// mode: c++
// End:
