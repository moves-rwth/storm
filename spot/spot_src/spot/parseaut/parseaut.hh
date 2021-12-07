// A Bison parser, made by GNU Bison 3.3.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file parseaut.hh
 ** Define the hoayy::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.

#ifndef YY_HOAYY_PARSEAUT_HH_INCLUDED
# define YY_HOAYY_PARSEAUT_HH_INCLUDED
// //                    "%code requires" blocks.
#line 33 "parseaut.yy" // lalr1.cc:401

#include "config.h"
#include <spot/misc/common.hh>
#include <string>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <spot/twa/formula2bdd.hh>
#include <spot/parseaut/public.hh>
#include "spot/priv/accmap.hh"
#include <spot/tl/parse.hh>
#include <spot/twaalgos/alternation.hh>

using namespace std::string_literals;

#ifndef HAVE_STRVERSCMP
// If the libc does not have this, a version is compiled in lib/.
extern "C" int strverscmp(const char *s1, const char *s2);
#endif

// Work around Bison not letting us write
//  %lex-param { res.h->errors }
#define PARSE_ERROR_LIST res.h->errors

  inline namespace hoayy_support
  {
    typedef std::map<int, bdd> map_t;

    /* Cache parsed formulae.  Labels on arcs are frequently identical
       and it would be a waste of time to parse them to formula
       over and over, and to register all their atomic_propositions in
       the bdd_dict.  Keep the bdd result around so we can reuse
       it.  */
    typedef std::map<std::string, bdd> formula_cache;

    typedef std::pair<int, std::string*> pair;
    typedef spot::twa_graph::namer<std::string> named_tgba_t;

    // Note: because this parser is meant to be used on a stream of
    // automata, it tries hard to recover from errors, so that we get a
    // chance to reach the end of the current automaton in order to
    // process the next one.  Several variables below are used to keep
    // track of various error conditions.
    enum label_style_t { Mixed_Labels, State_Labels, Trans_Labels,
			 Implicit_Labels };
    enum acc_style_t { Mixed_Acc, State_Acc, Trans_Acc };

    struct result_
    {
      struct state_info
      {
	bool declared = false;
	bool used = false;
	spot::location used_loc;
      };
      spot::parsed_aut_ptr h;
      spot::twa_ptr aut_or_ks;
      spot::automaton_parser_options opts;
      std::string format_version;
      spot::location format_version_loc;
      spot::environment* env;
      formula_cache fcache;
      named_tgba_t* namer = nullptr;
      spot::acc_mapper_int* acc_mapper = nullptr;
      std::vector<int> ap;
      std::vector<bdd> guards;
      std::vector<bdd>::const_iterator cur_guard;
      // If "Alias: ..." occurs before "AP: ..." in the HOA format we
      // are in trouble because the parser would like to turn each
      // alias into a BDD, yet the atomic proposition have not been
      // declared yet.  We solve that by using arbitrary BDD variables
      // numbers (in fact we use the same number given in the Alias:
      // definition) and keeping track of the highest variable number
      // we have seen (unknown_ap_max).  Once AP: is encountered,
      // we can remap everything.  If AP: is never encountered an
      // unknown_ap_max is non-negative, then we can signal an error.
      int unknown_ap_max = -1;
      spot::location unknown_ap_max_location;
      bool in_alias = false;
      map_t dest_map;
      std::vector<state_info> info_states; // States declared and used.
      std::vector<std::pair<spot::location,
                            std::vector<unsigned>>> start; // Initial states;
      std::unordered_map<std::string, bdd> alias;
      struct prop_info
      {
	spot::location loc;
	bool val;
	operator bool() const
	{
	  return val;
	};
      };
      std::unordered_map<std::string, prop_info> props;
      spot::location states_loc;
      spot::location ap_loc;
      spot::location state_label_loc;
      spot::location accset_loc;
      spot::acc_cond::mark_t acc_state;
      spot::acc_cond::mark_t neg_acc_sets = {};
      spot::acc_cond::mark_t pos_acc_sets = {};
      int plus;
      int minus;
      std::vector<std::string>* state_names = nullptr;
      std::map<unsigned, unsigned>* highlight_edges = nullptr;
      std::map<unsigned, unsigned>* highlight_states = nullptr;
      std::map<unsigned, unsigned> states_map;
      std::set<int> ap_set;
      unsigned cur_state;
      int states = -1;
      int ap_count = -1;
      int accset = -1;
      bdd state_label;
      bdd cur_label;
      bool has_state_label = false;
      bool ignore_more_ap = false; // Set to true after the first "AP:"
      // line has been read.
      bool ignore_acc = false;	// Set to true in case of missing
				// Acceptance: lines.
      bool ignore_acc_silent = false;
      bool ignore_more_acc = false; // Set to true after the first
				// "Acceptance:" line has been read.

      label_style_t label_style = Mixed_Labels;
      acc_style_t acc_style = Mixed_Acc;

      bool accept_all_needed = false;
      bool accept_all_seen = false;
      bool aliased_states = false;

      spot::trival universal = spot::trival::maybe();
      spot::trival existential = spot::trival::maybe();
      spot::trival complete = spot::trival::maybe();
      bool trans_acc_seen = false;

      std::map<std::string, spot::location> labels;

      prop_info prop_is_true(const std::string& p)
      {
	auto i = props.find(p);
	if (i == props.end())
	  return prop_info{spot::location(), false};
	return i->second;
      }

      prop_info prop_is_false(const std::string& p)
      {
	auto i = props.find(p);
	if (i == props.end())
	  return prop_info{spot::location(), false};
	return prop_info{i->second.loc, !i->second.val};
      }

      ~result_()
      {
	delete namer;
	delete acc_mapper;
      }
    };
  }

#line 211 "parseaut.hh" // lalr1.cc:401


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif



#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef HOAYYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define HOAYYDEBUG 1
#  else
#   define HOAYYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define HOAYYDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined HOAYYDEBUG */


namespace hoayy {
#line 330 "parseaut.hh" // lalr1.cc:401



  /// A Bison parser.
  class parser
  {
  public:
#ifndef HOAYYSTYPE
    /// Symbol semantic values.
    union semantic_type
    {
    #line 202 "parseaut.yy" // lalr1.cc:401

  std::string* str;
  unsigned int num;
  int b;
  spot::acc_cond::mark_t mark;
  pair* p;
  std::list<pair>* list;
  spot::acc_cond::acc_code* code;
  std::vector<unsigned>* states;

#line 353 "parseaut.hh" // lalr1.cc:401
    };
#else
    typedef HOAYYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef spot::location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Tokens.
    struct token
    {
      enum yytokentype
      {
        ENDOFFILE = 0,
        HOA = 258,
        STATES = 259,
        START = 260,
        AP = 261,
        ALIAS = 262,
        ACCEPTANCE = 263,
        ACCNAME = 264,
        TOOL = 265,
        NAME = 266,
        PROPERTIES = 267,
        BODY = 268,
        END = 269,
        STATE = 270,
        SPOT_HIGHLIGHT_EDGES = 271,
        SPOT_HIGHLIGHT_STATES = 272,
        IDENTIFIER = 273,
        HEADERNAME = 274,
        ANAME = 275,
        STRING = 276,
        INT = 277,
        DRA = 278,
        DSA = 279,
        V2 = 280,
        EXPLICIT = 281,
        ACCPAIRS = 282,
        ACCSIG = 283,
        ENDOFHEADER = 284,
        NEVER = 285,
        SKIP = 286,
        IF = 287,
        FI = 288,
        DO = 289,
        OD = 290,
        ARROW = 291,
        GOTO = 292,
        FALSE = 293,
        ATOMIC = 294,
        ASSERT = 295,
        FORMULA = 296,
        ENDAUT = 297,
        ENDDSTAR = 298,
        LBTT = 299,
        INT_S = 300,
        LBTT_EMPTY = 301,
        ACC = 302,
        STATE_NUM = 303,
        DEST_NUM = 304
      };
    };

    /// (External) token type, as returned by yylex.
    typedef token::yytokentype token_type;

    /// Symbol type: an internal symbol number.
    typedef int symbol_number_type;

    /// The symbol type number to denote an empty symbol.
    enum { empty_symbol = -2 };

    /// Internal symbol number for tokens (subsumed by symbol_number_type).
    typedef unsigned char token_number_type;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol type
    /// via type_get ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol ()
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that);
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);
      /// Constructor for valueless symbols.
      basic_symbol (typename Base::kind_type t,
                    YY_MOVE_REF (location_type) l);

      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    YY_RVREF (semantic_type) v,
                    YY_RVREF (location_type) l);

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }

      /// Destroy contents, and record that is empty.
      void clear ()
      {
        Base::clear ();
      }

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      semantic_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_type
    {
      /// Default constructor.
      by_type ();

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_type (by_type&& that);
#endif

      /// Copy constructor.
      by_type (const by_type& that);

      /// The symbol type as needed by the constructor.
      typedef token_type kind_type;

      /// Constructor from (external) token numbers.
      by_type (kind_type t);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol type from \a that.
      void move (by_type& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_number_type type_get () const YY_NOEXCEPT;

      /// The token.
      token_type token () const YY_NOEXCEPT;

      /// The symbol type.
      /// \a empty_symbol when empty.
      /// An int, not token_number_type, to be able to store empty_symbol.
      int type;
    };

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_type>
    {};

    /// Build a parser object.
    parser (void* scanner_yyarg, result_& res_yyarg, spot::location initial_loc_yyarg);
    virtual ~parser ();

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if HOAYYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);



  private:
    /// This class is not copyable.
    parser (const parser&);
    parser& operator= (const parser&);

    /// State numbers.
    typedef int state_type;

    /// Generate an error message.
    /// \param yystate   the state where the error occurred.
    /// \param yyla      the lookahead token.
    virtual std::string yysyntax_error_ (state_type yystate,
                                         const symbol_type& yyla) const;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    static const short yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token number \a t to a symbol number.
    static token_number_type yytranslate_ (int t);

    // Tables.
  // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
  // STATE-NUM.
  static const short yypact_[];

  // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
  // Performed when YYTABLE does not specify something else to do.  Zero
  // means the default is an error.
  static const unsigned char yydefact_[];

  // YYPGOTO[NTERM-NUM].
  static const short yypgoto_[];

  // YYDEFGOTO[NTERM-NUM].
  static const short yydefgoto_[];

  // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
  // positive, shift that token.  If negative, reduce the rule whose
  // number is the opposite.  If YYTABLE_NINF, syntax error.
  static const short yytable_[];

  static const short yycheck_[];

  // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
  // symbol of state STATE-NUM.
  static const unsigned char yystos_[];

  // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
  static const unsigned char yyr1_[];

  // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
  static const unsigned char yyr2_[];


    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *n);


    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#if HOAYYDEBUG
  // YYRLINE[YYN] -- Source line where rule number YYN was defined.
  static const unsigned short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol type, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol type as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol type from \a that.
      void move (by_state& that);

      /// The (internal) type number (corresponding to \a state).
      /// \a empty_symbol when empty.
      symbol_number_type type_get () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      enum { empty_state = -1 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::reverse_iterator iterator;
      typedef typename S::const_reverse_iterator const_iterator;
      typedef typename S::size_type size_type;

      stack (size_type n = 200)
        : seq_ (n)
      {}

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (size_type i)
      {
        return seq_[size () - 1 - i];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (int i)
      {
        return operator[] (size_type (i));
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (size_type i) const
      {
        return seq_[size () - 1 - i];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (int i) const
      {
        return operator[] (size_type (i));
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (int n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      size_type
      size () const YY_NOEXCEPT
      {
        return seq_.size ();
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.rbegin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.rend ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, int range)
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (int i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        int range_;
      };

    private:
      stack (const stack&);
      stack& operator= (const stack&);
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1);

    /// Constants.
    enum
    {
      yyeof_ = 0,
      yylast_ = 235,     ///< Last index in yytable_.
      yynnts_ = 83,  ///< Number of nonterminal symbols.
      yyfinal_ = 24, ///< Termination state number.
      yyterror_ = 1,
      yyerrcode_ = 256,
      yyntokens_ = 65  ///< Number of tokens.
    };


    // User arguments.
    void* scanner;
    result_& res;
    spot::location initial_loc;
  };



} // hoayy
#line 913 "parseaut.hh" // lalr1.cc:401




#endif // !YY_HOAYY_PARSEAUT_HH_INCLUDED
