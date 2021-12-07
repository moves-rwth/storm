/* -*- coding: utf-8 -*-
** Copyright (C) 2014-2018 Laboratoire de Recherche et Développement
** de l'Epita (LRDE).
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
%option noyywrap
%option prefix="hoayy"
%option outfile="lex.yy.c"
%option reentrant
%option extra-type="struct extra_data*"

/* %option debug */
%top{
#include "config.h"
}
%{
#include <string>
#include <sys/stat.h>
#include <spot/parseaut/parsedecl.hh>
#include "spot/priv/trim.hh"

#define YY_USER_ACTION yylloc->columns(yyleng);

typedef hoayy::parser::token token;

struct extra_data
{
  unsigned comment_level = 0;
  unsigned parent_level = 0;
  int orig_cond = 0;
  bool lbtt_s = false;
  bool lbtt_t = false;
  unsigned lbtt_states = 0;
  bool yyin_close = true;
};

%}

eol         \n+|\r+
eol2        (\n\r)+|(\r\n)+
eols        ({eol}|{eol2})*
identifier  [[:alpha:]_][[:alnum:]_.-]*

%x in_COMMENT in_STRING in_NEVER_PAR
%s in_HOA in_NEVER in_LBTT_HEADER
%s in_LBTT_STATE in_LBTT_INIT in_LBTT_TRANS
%s in_LBTT_T_ACC in_LBTT_S_ACC in_LBTT_GUARD
%s in_DSTAR
%%

%{
  std::string s;
  yylloc->step();

  auto parse_int = [&](){
    errno = 0;
    char* end;
    unsigned long n = strtoul(yytext, &end, 10);
    yylval->num = n;
    if (errno || yylval->num != n)
      {
        error_list.push_back(spot::parse_aut_error(*yylloc, "value too large"));
        yylval->num = 0;
      }
    return end;
  };
%}


                        /* skip blanks and comments */
{eol}			yylloc->lines(yyleng); yylloc->step();
{eol2}			yylloc->lines(yyleng / 2); yylloc->step();
[ \t\v\f]+		yylloc->step();
"/*"			{
                          yyextra->orig_cond = YY_START;
			  BEGIN(in_COMMENT);
			  yyextra->comment_level = 1;
			}
<INITIAL>"HOA:"           BEGIN(in_HOA); return token::HOA;
<INITIAL,in_HOA>"--ABORT--" BEGIN(INITIAL); throw spot::hoa_abort{*yylloc};
<INITIAL>"never"	  BEGIN(in_NEVER); return token::NEVER;
<INITIAL>"DSA"		  BEGIN(in_DSTAR); return token::DSA;
<INITIAL>"DRA"		  BEGIN(in_DSTAR); return token::DRA;

<INITIAL>[0-9]+[ \t][0-9]+[ts]?  {
	                  BEGIN(in_LBTT_HEADER);
			  char* end = nullptr;
			  errno = 0;
			  unsigned long n = strtoul(yytext, &end, 10);
			  yylval->num = n;
			  unsigned s = end - yytext;
			  yylloc->end = yylloc->begin;
 			  yylloc->end.columns(s);
			  yyless(s);
			  if (errno || yylval->num != n)
			    {
                              error_list.push_back(
			        spot::parse_aut_error(*yylloc,
				  "value too large"));
			      yylval->num = 0;
                            }
                          yyextra->lbtt_states = yylval->num;
			  return token::LBTT;
			}

<in_HOA>{
  "States:"		return token::STATES;
  "Start:"		return token::START;
  "AP:"			return token::AP;
  "Alias:"              return token::ALIAS;
  "Acceptance:"         return token::ACCEPTANCE;
  "acc-name:"           return token::ACCNAME;
  "tool:"               return token::TOOL;
  "name:"               return token::NAME;
  "properties:"         return token::PROPERTIES;
  "spot.highlight.states:" return token::SPOT_HIGHLIGHT_STATES;
  "spot.highlight.edges:"  return token::SPOT_HIGHLIGHT_EDGES;
  "--BODY--"		return token::BODY;
  "--END--"		BEGIN(INITIAL); return token::END;
  "State:"		return token::STATE;
  [tf{}()\[\]&|!]	return *yytext;

  {identifier}          {
			   yylval->str = new std::string(yytext, yyleng);
			   return token::IDENTIFIER;
			}
  {identifier}":"       {
			   yylval->str = new std::string(yytext, yyleng - 1);
			   return token::HEADERNAME;
			}
  "@"[[:alnum:]_.-]+     {
			   yylval->str = new std::string(yytext + 1, yyleng - 1);
			   return token::ANAME;
			}
  [0-9]+                parse_int(); return token::INT;
}

<in_DSTAR>{
  "States:"		return token::STATES;
  "State:"		return token::STATE;
  "Start:"		return token::START;
  "AP:"			return token::AP;
  "v2"			return token::V2;
  "explicit"		return token::EXPLICIT;
  "Comment:".*		yylloc->step();
  "//".*		yylloc->step();
  "Acceptance-Pairs:"	return token::ACCPAIRS;
  "Acc-Sig:"		return token::ACCSIG;
  "---"			return token::ENDOFHEADER;
  [0-9]+                parse_int(); return token::INT;
  /* The start of any automaton is the end of this one.
     We do not try to detect LBTT automata, as that would
     be too hard to distinguish from state numbers. */
  {eols}("HOA:"|"never"|"DSA"|"DRA")  {
			  yylloc->end = yylloc->begin;
			  yyless(0);
			  BEGIN(INITIAL);
			  return token::ENDDSTAR;
			}
  <<EOF>>	return token::ENDDSTAR;
}

<in_NEVER>{
  "skip"		return token::SKIP;
  "if"			return token::IF;
  "fi"			return token::FI;
  "do"			return token::DO;
  "od"			return token::OD;
  "->"			return token::ARROW;
  "goto"		return token::GOTO;
  "false"|"0"		return token::FALSE;
  "atomic"		return token::ATOMIC;
  "assert"		return token::ASSERT;

  ("!"[ \t]*)?"("	{
			  yyextra->parent_level = 1;
			  BEGIN(in_NEVER_PAR);
			  yylval->str = new std::string(yytext, yyleng);
			}

  "true"|"1"		{
                          yylval->str = new std::string(yytext, yyleng);
			  return token::FORMULA;
                        }

  [a-zA-Z][a-zA-Z0-9_]* {
			  yylval->str = new std::string(yytext, yyleng);
	                  return token::IDENTIFIER;
		        }
}

  /* Note: the LBTT format is scanf friendly, but not Bison-friendly.
     If we only tokenize it as a stream of INTs, the parser will have
     a very hard time recognizing what is a state from what is a
     transitions.  As a consequence we abuse the start conditions to
     maintain a state an return integers with different semantic types
     depending on the purpose of those integers. */
<in_LBTT_HEADER>{
  [0-9]+[st]*           {
			  BEGIN(in_LBTT_STATE);
                          auto end = parse_int();
			  yyextra->lbtt_s = false;
			  yyextra->lbtt_t = false;
			  if (end)
			    while (int c = *end++)
			      {
			         if (c == 's')
			           yyextra->lbtt_s = true;
			         else // c == 't'
			           yyextra->lbtt_t = true;
			      }
  		          if (!yyextra->lbtt_t)
			    yyextra->lbtt_s = true;
			  if (yyextra->lbtt_states == 0)
			    {
                              BEGIN(INITIAL);
                              return token::LBTT_EMPTY;
			    }
			  if (yyextra->lbtt_s && !yyextra->lbtt_t)
			    return token::INT_S;
			  else
			    return token::INT;
			}
}

<in_LBTT_STATE>[0-9]+   {
                           parse_int();
			   BEGIN(in_LBTT_INIT);
			   return token::STATE_NUM;
			}
<in_LBTT_INIT>[01]	{
                           yylval->num = *yytext - '0';
			   if (yyextra->lbtt_s)
			      BEGIN(in_LBTT_S_ACC);
			   else
			      BEGIN(in_LBTT_TRANS);
			   return token::INT;
			}
<in_LBTT_S_ACC>{
  [0-9]+		parse_int(); return token::ACC;
  "-1"                  BEGIN(in_LBTT_TRANS); yylloc->step();
}
<in_LBTT_TRANS>{
  [0-9]+                {
			  parse_int();
			  if (yyextra->lbtt_t)
			    BEGIN(in_LBTT_T_ACC);
			  else
			    BEGIN(in_LBTT_GUARD);
			  return token::DEST_NUM;
			}
  "-1"			{
                          if (--yyextra->lbtt_states)
			    {
			       BEGIN(in_LBTT_STATE);
			       yylloc->step();
			    }
			  else
			    {
			       BEGIN(INITIAL);
			       return token::ENDAUT;
			    }
			}
}
<in_LBTT_T_ACC>{
  [0-9]+	        parse_int(); return token::ACC;
  "-1"			BEGIN(in_LBTT_GUARD); yylloc->step();
}
<in_LBTT_GUARD>{
  [^\n\r]*		{
  			  yylval->str = new std::string(yytext, yyleng);
			  BEGIN(in_LBTT_TRANS);
 			  return token::STRING;
			}
}


<in_COMMENT>{
  "/*"                  ++yyextra->comment_level;
  [^*/\n\r]*		continue;
  "/"[^*\n\r]*		continue;
  "*"                   continue;
  {eol}                 yylloc->lines(yyleng); yylloc->end.column = 1;
  {eol2}		yylloc->lines(yyleng / 2); yylloc->end.column = 1;
  "*/"                  {
			  if (--yyextra->comment_level == 0)
			    {
			      yylloc->step();
                              int oc = yyextra->orig_cond;
		              BEGIN(oc);
		            }
                        }
  <<EOF>>		{
                           int oc = yyextra->orig_cond;
	                   BEGIN(oc);
                           error_list.push_back(
			     spot::parse_aut_error(*yylloc,
			       "unclosed comment"));
			   return 0;
                        }
}

  /* matched late, so that the in_LBTT_GUARD pattern has precedence */
"\""                    {
                          yyextra->orig_cond = YY_START;
			  BEGIN(in_STRING);
			  yyextra->comment_level = 1;
			}

<in_STRING>{
  \"	                {
                           int oc = yyextra->orig_cond;
                           BEGIN(oc);
			   yylval->str = new std::string(s);
			   return token::STRING;
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
			     spot::parse_aut_error(*yylloc,
			       "unclosed string"));
                           int oc = yyextra->orig_cond;
                           BEGIN(oc);
			   yylval->str = new std::string(s);
			   return token::STRING;
                        }
}

<in_NEVER_PAR>{
  "("		        {
			  ++yyextra->parent_level;
			  yylval->str->append(yytext, yyleng);
			}
  /* if we match ")&&(" or ")||(", stay in <in_NEVER_PAR> mode */
  ")"[ \t]*("&&"|"||")[ \t!]*"(" {
	                  yylval->str->append(yytext, yyleng);
			}
  ")"                   {
	                  yylval->str->append(yytext, yyleng);
			  if (!--yyextra->parent_level)
			    {
                              BEGIN(in_NEVER);
			      spot::trim(*yylval->str);
			      return token::FORMULA;
			    }
			}
  {eol}			{
                          yylval->str->append(yytext, yyleng);
			  yylloc->lines(yyleng); yylloc->end.column = 1;
			}
  {eol2}		{
			  yylval->str->append(yytext, yyleng);
  			  yylloc->lines(yyleng / 2); yylloc->end.column = 1;
			}
  [^()\n\r]+		yylval->str->append(yytext, yyleng);
  <<EOF>>		{
                          error_list.push_back(
			    spot::parse_aut_error(*yylloc,
 			      "missing closing parenthese"));
                          yylval->str->append(yyextra->parent_level, ')');
                          BEGIN(in_NEVER);
			  spot::trim(*yylval->str);
			  return token::FORMULA;
			}
}

.			return *yytext;

%{
  /* Dummy use of yyunput to shut up a gcc warning.  */
  (void) &yyunput;
%}

%%

namespace spot
{
  void
  hoayyreset(yyscan_t yyscanner)
  {
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    BEGIN(INITIAL);
    yyextra->comment_level = 0;
    yyextra->parent_level = 0;
  }

  int
  hoayyopen(const std::string &name, yyscan_t* scanner)
  {
    yylex_init_extra(new extra_data, scanner);
    yyscan_t yyscanner = *scanner;
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    bool want_interactive = false;

    // yy_flex_debug = 1;
    if (name == "-")
      {
        // If the input is a pipe, make the scanner
        // interactive so that it does not wait for the input
        // buffer to be full to process automata.
        struct stat s;
        if (fstat(fileno(stdin), &s) < 0)
           throw std::runtime_error("fstat failed");
	if (S_ISFIFO(s.st_mode))
	  want_interactive = true;

        yyin = stdin;
        yyextra->yyin_close = false;
      }
    else
      {
        yyin = fopen(name.c_str(), "r");
        if (!yyin)
	  return 1;
        yyextra->yyin_close = true;
      }

    if (want_interactive)
      yy_set_interactive(1);
    return 0;
  }

  int
  hoayystring(const char* data, yyscan_t* scanner)
  {
    yylex_init_extra(new extra_data, scanner);
    yy_scan_string(data, *scanner);
    return 0;
  }

  int
  hoayyopen(int fd, yyscan_t* scanner)
  {
    yylex_init_extra(new extra_data, scanner);
    yyscan_t yyscanner = *scanner;
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    bool want_interactive = false;
    yyextra->yyin_close = false;

    yyin = fdopen(fd, "r");

    if (!yyin)
      throw std::runtime_error("fdopen failed");

    // If the input is a pipe, make the scanner
    // interactive so that it does not wait for the input
    // buffer to be full to process automata.
    struct stat s;
    if (fstat(fd, &s) < 0)
      throw std::runtime_error("fstat failed");
    if (S_ISFIFO(s.st_mode))
      want_interactive = true;

    if (want_interactive)
      yy_set_interactive(1);
    return 0;
  }

  void
  hoayyclose(yyscan_t yyscanner)
  {
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    if (yyin)
      {
        if (yyextra->yyin_close)
          fclose(yyin);
        yyin = NULL;
      }
    delete yyextra;
    yylex_destroy(yyscanner);
  }
}
