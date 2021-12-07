/* -*- coding: utf-8 -*-
** Copyright (C) 2010-2015, 2017-2019, Laboratoire de Recherche et
** Développement de l'Epita (LRDE).
** Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6
** (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
** Pierre et Marie Curie.
**
** This file is part of Spot, a model checking library.
**
** Spot is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** Spot is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
** License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
%option noyywrap warn 8bit batch
%option prefix="tlyy"
%option outfile="lex.yy.c"
%option stack
%option never-interactive

%top{
#include "config.h"
}
%{
#include <cstdlib>
#include <string>

#include <spot/parsetl/parsedecl.hh>
#include "spot/priv/trim.hh"

#define YY_USER_ACTION \
  yylloc->columns(yyleng);

static int start_token = 0;
static int parent_level = 0;
static bool missing_parent = false;
static bool lenient_mode = false;
static int orig_cond = 0;
static unsigned comment_level = 0;

typedef tlyy::parser::token token;

%}

%s not_prop
%x in_par
%x in_bra
%x sqbracket
%x lbt
%x in_COMMENT in_STRING

BOX       "[]"|"□"|"⬜"|"◻"
DIAMOND   "<>"|"◇"|"⋄"|"♢"
ARROWL    "->"|"-->"|"→"|"⟶"
DARROWL   "=>"|"⇒"|"⟹"
ARROWLR   "<->"|"<-->"|"↔"
DARROWLR  "<=>"|"⇔"
CIRCLE    "()"|"○"|"◯"
CIRCLEX   "Ⓧ"
NOT       "!"|"~"|"¬"
BOXARROW  {BOX}{ARROWL}|"|"{ARROWL}|"↦"
BOXDARROW {BOX}{DARROWL}|"|"{DARROWL}|"⤇"
eol         \n+|\r+
eol2        (\n\r)+|(\r\n)+

%%

%{
  if (start_token)
    {
      int t = start_token;
      start_token = 0;
      return t;
    }
  yylloc->step();
  std::string s;
%}

<*>"/""*"+		{
			  if (YY_START != in_COMMENT)
			    {
                              orig_cond = YY_START;
                              BEGIN(in_COMMENT);
			      comment_level = 0;
			    }
			  ++comment_level;
			}
<in_COMMENT>{
  [^*/\n\r]*		continue;
  "/"[^*\n\r]*		continue;
  "*"+[^*/\n\r]*	continue;
  {eol}			yylloc->lines(yyleng); yylloc->end.column = 1;
  {eol2}		yylloc->lines(yyleng / 2); yylloc->end.column = 1;
  "*"+"/"		if (--comment_level == 0) BEGIN(orig_cond);
  <<EOF>>		{
                           BEGIN(orig_cond);
                           error_list.push_back(
			     spot::one_parse_error(*yylloc,
			       "unclosed comment"));
			   return 0;
                        }
}

"("				{
				  if (!lenient_mode)
                                    {
				      BEGIN(0);
				      return token::PAR_OPEN;
                                    }
                                  /* Parse any (...) block as a single block,
				     taking care of nested parentheses.  The
				     parser will then try to parse this block
				     recursively.  */
                                  BEGIN(in_par);
                                  parent_level = 1;
				  yylval->str = new std::string();
                                }
<in_par>{
	 "("			{
				  ++parent_level;
				  yylval->str->append(yytext, yyleng);
				}
	 ")"			{
				  if (--parent_level)
				    {
                                      yylval->str->append(yytext, yyleng);
				    }
				  else
				    {
                                      BEGIN(not_prop);
				      spot::trim(*yylval->str);
				      return token::PAR_BLOCK;
				    }
				}
         [^()]+			yylval->str->append(yytext, yyleng);
	 <<EOF>>		{
				  unput(')');
				  if (!missing_parent)
                                    error_list.push_back(
				      spot::one_parse_error(*yylloc,
 					"missing closing parenthesis"));
				  missing_parent = true;
				}
}

"{"				{
				  if (!lenient_mode)
				  {
				     BEGIN(0);
			             return token::BRACE_OPEN;
                                  }
                                  /* Parse any {...} block as a single block,
				     taking care of nested parentheses.  The
				     parser will then try to parse this block
				     recursively.  */
                                  BEGIN(in_bra);
                                  parent_level = 1;
				  yylval->str = new std::string();
                                }
<in_bra>{
	 "{"			{
				  ++parent_level;
				  yylval->str->append(yytext, yyleng);
				}
         "}"[ \t]*"!"           {
				  if (--parent_level)
				    {
                                      yylval->str->append(yytext, yyleng);
				    }
				  else
				    {
                                      BEGIN(not_prop);
				      spot::trim(*yylval->str);
				      return token::BRA_BANG_BLOCK;
				    }
                                }
	 "}"			{
				  if (--parent_level)
				    {
                                      yylval->str->append(yytext, yyleng);
				    }
				  else
				    {
                                      BEGIN(not_prop);
				      spot::trim(*yylval->str);
				      return token::BRA_BLOCK;
				    }
				}
         [^{}]+			yylval->str->append(yytext, yyleng);
	 <<EOF>>		{
				  unput('}');
				  if (!missing_parent)
                                    error_list.push_back(
				      spot::one_parse_error(*yylloc,
 					"missing closing brace"));
				  missing_parent = true;
				}
}

")"				BEGIN(not_prop); return token::PAR_CLOSE;
"}"[ \t]*"!"			BEGIN(not_prop); return token::BRACE_BANG_CLOSE;
"}"				BEGIN(not_prop); return token::BRACE_CLOSE;

  /* Must go before the other operators, because the F of FALSE
     should not be mistaken with a unary F. */
"1"|[tT][rR][uU][eE]		BEGIN(0); return token::CONST_TRUE;
"0"|[fF][aA][lL][sS][eE]	BEGIN(0); return token::CONST_FALSE;


  /* ~ comes from Goal, ! from everybody else */
{NOT}				BEGIN(0); return token::OP_NOT;

"first_match"                   BEGIN(0); return token::OP_FIRST_MATCH;

  /* SVA operators */
"##"[0-9]+                      {
				  errno = 0;
				  unsigned long n = strtoul(yytext + 2, 0, 10);
                                  yylval->num = n;
				  if (errno || yylval->num != n)
				    {
                                      error_list.push_back(
				        spot::one_parse_error(*yylloc,
					  "value too large ignored"));
                                      yylval->num = 1;
                                    }
                                  return token::OP_DELAY_N;
                                }
"##[+]"                         BEGIN(0); return token::OP_DELAY_PLUS;
"##[*]"                         BEGIN(0); return token::OP_DELAY_STAR;
"##["                           BEGIN(sqbracket); return token::OP_DELAY_OPEN;

  /* PSL operators */
{BOXARROW}			BEGIN(0); return token::OP_UCONCAT;
{DIAMOND}{ARROWL}		BEGIN(0); return token::OP_ECONCAT;
{BOXDARROW}			BEGIN(0); return token::OP_UCONCAT_NONO;
{DIAMOND}{DARROWL}		BEGIN(0); return token::OP_ECONCAT_NONO;
";"				BEGIN(0); return token::OP_CONCAT;
":"				BEGIN(0); return token::OP_FUSION;
"*"				BEGIN(0); return token::OP_STAR;
"[*]"				BEGIN(0); return token::OP_BSTAR;
"[+]"				BEGIN(0); return token::OP_PLUS;
"[*"				BEGIN(sqbracket); return token::OP_STAR_OPEN;
"[:*]"				BEGIN(0); return token::OP_BFSTAR;
"[:+]"				BEGIN(0); return token::OP_FPLUS;
"[:*"				BEGIN(sqbracket); return token::OP_FSTAR_OPEN;
"[="				BEGIN(sqbracket); return token::OP_EQUAL_OPEN;
"["{ARROWL}			BEGIN(sqbracket); return token::OP_GOTO_OPEN;
<sqbracket>"]"			BEGIN(0); return token::OP_SQBKT_CLOSE;
<sqbracket>"!]"                 BEGIN(0); return token::OP_SQBKT_STRONG_CLOSE;
<sqbracket>[0-9]+		{
				  errno = 0;
				  unsigned long n = strtoul(yytext, 0, 10);
                                  yylval->num = n;
				  if (errno || yylval->num != n)
				    {
                                      error_list.push_back(
				        spot::one_parse_error(*yylloc,
					  "value too large ignored"));
				      // Skip this number and read next token
                                      yylloc->step();
                                    }
				  else
                                    {
                                      return token::OP_SQBKT_NUM;
                                    }
				}
  /* .. is from PSL and EDL
   : is from Verilog and PSL
   to is from VHDL
   , is from Perl */
<sqbracket>","|".."|":"|"to"	return token::OP_SQBKT_SEP;

  /* In SVA you use [=1:$] instead of [=1..].  We will also accept
     [=1..$] and [=1:].  The PSL LRM shows examples like [=1:inf]
     instead, so will accept this too.  */
<sqbracket>"$"|"inf"		return token::OP_UNBOUNDED;

  /* & and | come from Spin.  && and || from LTL2BA.
     /\, \/, and xor are from LBTT.
     --> and <--> come from Goal.
     +,*,^ are from Wring. */
"||"|"|"|"+"|"\\/"|"∨"|"∪"	BEGIN(0); return token::OP_OR;
"&&"|"/\\"|"∧"|"∩"		BEGIN(0); return token::OP_AND;
"&"				BEGIN(0); return token::OP_SHORT_AND;
"^"|"xor"|"⊕"			BEGIN(0); return token::OP_XOR;
{ARROWL}|{DARROWL}		BEGIN(0); return token::OP_IMPLIES;
{ARROWLR}|{DARROWLR}		BEGIN(0); return token::OP_EQUIV;

 /* TSLF-like syntactic sugar:
    X[3]f = XXXf
    F[2..4]f = XX(f | X(f | Xf))
    G[2..4]f = XX(f & X(f & Xf))
    We also have to deal with the Spin-like notation for G.
    X[]f = XGf
 */
"X"[ \t\n\r]*"[]"               yyless(1); return token::OP_X;
"F"[ \t\n\r]*"[]"               yyless(1); return token::OP_F;
"G"[ \t\n\r]*"[]"               yyless(1); return token::OP_G;
"X"[ \t\n\r]*"["                BEGIN(sqbracket); return token::OP_XREP;
"F"[ \t\n\r]*"["                BEGIN(sqbracket); return token::OP_FREP;
"G"[ \t\n\r]*"["                BEGIN(sqbracket); return token::OP_GREP;
  /* <> (DIAMOND) and [] (BOX), are used in Spin.
     () (CIRCLE) is not, but would make sense.  */
"F"|{DIAMOND}			BEGIN(0); return token::OP_F;
"G"|{BOX}			BEGIN(0); return token::OP_G;
"U"				BEGIN(0); return token::OP_U;
"R"|"V"				BEGIN(0); return token::OP_R;
"X"|{CIRCLE}			BEGIN(0); return token::OP_X;
{CIRCLEX}                       BEGIN(0); return token::OP_STRONG_X;
"W"				BEGIN(0); return token::OP_W;
"M"				BEGIN(0); return token::OP_M;

  /* The combining overline or macron (overbar) should normally
     occur only after a single letter, but we do not check that. */
"=0"|"̅"|"̄"			return token::OP_POST_NEG;
"=1"				return token::OP_POST_POS;

<*>[ \t\n\r]+			/* discard whitespace */ yylloc->step ();

  /* An Atomic proposition cannot start with the letter
     used by a unary operator (F,G,X), unless this
     letter is followed by a digit in which case we assume
     it's an ATOMIC_PROP (even though F0 could be seen as Ffalse, we
     don't, because Ffalse is never used in practice).
  */
<INITIAL>[a-zA-EH-WYZ_.][a-zA-Z0-9_.]* |
<INITIAL>[FGX][0-9][a-zA-Z0-9_.]* |
  /*
     However if we have just parsed an atomic proposition, then we are
     not expecting another atomic proposition, so we can be stricter
     and disallow propositions that start with M, U, R, V, and W.  If
     you wonder why we do this, consider the Wring formula `p=0Uq=1'.
     When p is parsed, we enter the not_prop start condition, we
     remain into this condition when `=0' is processed, and then
     because we are in this condition we will not consider `Uq' as an
     atomic proposition but as a `U' operator followed by a `q' atomic
     proposition.

     We also disable atomic proposition that may look like a combination
     of a binary operator followed by several unary operators.
     E.g. UFXp.   This way, `p=0UFXp=1' will be parsed as `(p=0)U(F(X(p=1)))'.
  */
<not_prop>[a-zA-EH-LN-QSTYZ_.][a-zA-EH-WYZ0-9_.]* |
<not_prop>[a-zA-EH-LN-QSTYZ_.][a-zA-EH-WYZ0-9_.][a-zA-Z0-9_.]* {
			  yylval->str = new std::string(yytext, yyleng);
			  BEGIN(not_prop);
			  return token::ATOMIC_PROP;
			}

  /* Atomic propositions can also be enclosed in double quotes.

     orig_cond stores the parsing condition to use after the
     string has been parsed. */
"\""                    {
                          orig_cond = not_prop;
			  BEGIN(in_STRING);
			}
<lbt>"\""		{
			  orig_cond = lbt;
			  BEGIN(in_STRING);
			}

<in_STRING>{
  \"	                {
                           BEGIN(orig_cond);
			   yylval->str = new std::string(s);
			   return token::ATOMIC_PROP;
 			}
  {eol}			{
  			  s.append(yytext, yyleng);
                          yylloc->lines(yyleng); yylloc->end.column = 1;
			}
  {eol2}		{
  			  s.append(yytext, yyleng);
                          yylloc->lines(yyleng / 2); yylloc->end.column = 1;
			}
  \\.			s += yytext[1];
  [^\\\"\n\r]+		s.append(yytext, yyleng);
  <<EOF>>		{
                           error_list.push_back(
			     spot::one_parse_error(*yylloc,
			       "unclosed string"));
                           BEGIN(orig_cond);
			   yylval->str = new std::string(s);
			   return token::ATOMIC_PROP;
                        }
}

  /* these are operators */
<lbt>[eitfXFGUVRWM]	return *yytext;
  /* in LBT's format, atomic proposition look like p0 or p3141592, but
     for compatibility with ltl2dstar we also accept any alphanumeric
     string that is not an operator. */
<lbt>[a-zA-Z._][a-zA-Z0-9._]*            {
			  yylval->str = new std::string(yytext, yyleng);
			  return token::ATOMIC_PROP;
			}


<*>.			return *yytext;

<<EOF>>			return token::END_OF_INPUT;

%%

void
flex_set_buffer(const std::string& buf,
		int start_tok, bool lenient)
{
  yypush_buffer_state(YY_CURRENT_BUFFER);
  (void) yy_scan_bytes(buf.c_str(), buf.size());
  start_token = start_tok;
  if (start_tok == token::START_LBT)
    yy_push_state(lbt);
  else
    yy_push_state(0);
  lenient_mode = lenient;
}

void
flex_unset_buffer()
{
  (void)&yy_top_state; // shut up a g++ warning.
  yy_pop_state();
  yypop_buffer_state();
  missing_parent = false;
}
