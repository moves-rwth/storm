// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2010, 2012-2016, 2018, 2019 Laboratoire de
// Recherche et Développement de l'Epita (LRDE)
// Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
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
// or FITNESS FOR A PARTICULAR PURPOSE.         See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "config.h"
#include <cassert>
#include <sstream>
#include <ctype.h>
#include <ostream>
#include <cstring>
#include <spot/tl/unabbrev.hh>
#include <spot/tl/print.hh>
#include <spot/misc/escape.hh>

namespace spot
{
  namespace
  {
    enum keyword {
      KFalse = 0,
      KTrue = 1,
      KEmptyWord = 2,
      KXor,
      KImplies,
      KEquiv,
      KU,
      KR,
      KW,
      KM,
      KSeq,
      KSeqNext,
      KSeqMarked,
      KSeqMarkedNext,
      KTriggers,
      KTriggersNext,
      KNot,
      KX,
      KF,
      KG,
      KOr,
      KOrRat,
      KAnd,
      KAndRat,
      KAndNLM,
      KConcat,
      KFusion,
      KOpenSERE,
      KCloseSERE,
      KCloseBunop,
      KStarBunop,
      KPlusBunop,
      KFStarBunop,
      KFPlusBunop,
      KEqualBunop,
      KGotoBunop,
      KFirstMatch,
      KStrongX,
    };

    const char* spot_kw[] = {
      "0",
      "1",
      "[*0]",
      " xor ",
      " -> ",
      " <-> ",
      " U ",
      " R ",
      " W ",
      " M ",
      "<>-> ",
      "<>=> ",
      "<>+> ",
      "<>=+> ",
      "[]-> ",
      "[]=> ",
      "!",
      "X",
      "F",
      "G",
      " | ",
      " | ",
      " & ",
      " && ",
      " & ",
      ";",
      ":",
      "{",
      "}",
      "]",
      "[*",
      "[+]",
      "[:*",
      "[:+]",
      "[=",
      "[->",
      "first_match",
      "X[!]",
    };

    const char* spin_kw[] = {
      "false", // 0 doesn't work from the command line
      "true",  // 1 doesn't work from the command line
      "[*0]",                        // not supported
      " xor ",                // rewritten
      " -> ",
      " <-> ",
      " U ",
      " V ",
      " W ",                        // rewritten
      " M ",                        // rewritten
      "<>-> ",                // not supported
      "<>=> ",                // not supported
      "<>+> ",                // not supported
      "<>=+> ",                // not supported
      "[]-> ",                // not supported
      "[]=> ",                // not supported
      "!",
      "X",
      "<>",
      "[]",
      " || ",
      " || ",
      " && ",
      " && ",                   // not supported
      " & ",                    // not supported
      ";",                      // not supported
      ":",                      // not supported
      "{",                      // not supported
      "}",                      // not supported
      "]",                      // not supported
      "[*",                     // not supported
      "[+]",                    // not supported
      "[:*",                    // not supported
      "[:+]",                   // not supported
      "[=",                     // not supported
      "[->",                    // not supported
      "first_match",            // not supported
      "!X!",
    };

    const char* wring_kw[] = {
      "FALSE",
      "TRUE",
      "[*0]",                   // not supported
      " ^ ",
      " -> ",
      " <-> ",
      " U ",
      " R ",
      " W ",                    // rewritten
      " M ",                    // rewritten
      "<>-> ",                  // not supported
      "<>=> ",                  // not supported
      "<>+> ",                  // not supported
      "<>=+> ",                 // not supported
      "[]-> ",                  // not supported
      "[]=> ",                  // not supported
      "!",
      "X",
      "F",
      "G",
      " + ",
      " | ",                    // not supported
      " * ",
      " && ",                   // not supported
      " & ",                    // not supported
      ";",                      // not supported
      ":",                      // not supported
      "{",                      // not supported
      "}",                      // not supported
      "]",                      // not supported
      "[*",                     // not supported
      "[+]",                    // not supported
      "[:*",                    // not supported
      "[:+]",                   // not supported
      "[=",                     // not supported
      "[->",                    // not supported
      "first_match",            // not supported
      "X[!]",                   // not supported, FIXME: we need a syntax
    };

    const char* utf8_kw[] = {
      "0",
      "1",
      "[*0]",
      "⊕",
      " → ",
      " ↔ ",
      " U ",
      " R ",
      " W ",
      " M ",
      "◇→ ",
      "◇⇒ ",
      "◇→̃ ",
      "◇⇒̃ ",
      "□→ ",
      "□⇒ ",
      "¬",
      "○",
      "◇",
      "□",
      "∨",
      " | ",
      "∧",
      " ∩ ",
      " & ",
      ";",
      ":",
      "{",
      "}",
      "]",
      "[*",
      "[+]",
      "[:*",
      "[:+]",
      "[=",
      "[->",
      "first_match",
      "Ⓧ",
    };

    const char* latex_kw[] = {
      "\\ffalse",
      "\\ttrue",
      "\\eword",
      " \\lxor ",
      " \\limplies ",
      " \\liff ",
      " \\U ",
      " \\R ",
      " \\W ",
      " \\M ",
      "\\seq ",
      "\\seqX ",
      "\\seqM ",
      "\\seqXM ",
      "\\triggers ",
      "\\triggersX ",
      "\\lnot ",
      "\\X ",
      "\\F ",
      "\\G ",
      " \\lor ",
      " \\SereOr ",
      " \\land ",
      " \\SereAnd ",
      " \\SereAndNLM ",
      " \\SereConcat ",
      " \\SereFusion ",
      "\\{",
      "\\}",
      "}",
      "\\SereStar{",
      "\\SerePlus{}",
      "\\SereFStar{",
      "\\SereFPlus{}",
      "\\SereEqual{",
      "\\SereGoto{",
      "\\FirstMatch",
      "\\StrongX",
    };

    const char* sclatex_kw[] = {
      "\\bot",
      "\\top",
      "\\varepsilon",
      " \\oplus ",
      " \\rightarrow ",
      " \\leftrightarrow ",
      " \\mathbin{\\mathsf{U}} ",
      " \\mathbin{\\mathsf{R}} ",
      " \\mathbin{\\mathsf{W}} ",
      " \\mathbin{\\mathsf{M}} ",
      ("\\mathrel{\\Diamond\\kern-1.7pt\\raise.4pt"
       "\\hbox{$\\mathord{\\rightarrow}$}} "),
      ("\\mathrel{\\Diamond\\kern-1.7pt\\raise.4pt"
       "\\hbox{$\\mathord{\\Rightarrow}$}} "),
      "\\seqM ",
      "\\seqXM ",
      ("\\mathrel{\\Box\\kern-1.7pt\\raise.4pt"
       "\\hbox{$\\mathord{\\rightarrow}$}} "),
      ("\\mathrel{\\Box\\kern-1.7pt\\raise.4pt"
       "\\hbox{$\\mathord{\\Rightarrow}$}} "),
      "\\lnot ",
      "\\mathsf{X} ",
      "\\mathsf{F} ",
      "\\mathsf{G} ",
      " \\lor ",
      " \\cup ",
      " \\land ",
      " \\cap ",
      " \\mathbin{\\mathsf{\\&}} ",
      " \\mathbin{\\mathsf{;}} ",
      " \\mathbin{\\mathsf{:}} ",
      "\\{",
      "\\}",
      "}",
      "^{\\star",
      "^+",
      "^{\\mathsf{:}\\star",
      "^{\\mathsf{:}+}",
      "^{=",
      "^{\\to",
      "\\mathsf{first\\_match}",
      "\\textcircled{\\mathsf{X}}",
    };

    static bool
    is_bare_word(const char* str)
    {
      // Bare words cannot be empty, start with the letter of a
      // unary operator, or be the name of an existing constant or
      // operator.  Also they should start with an letter.
      if (!*str
          || *str == 'F'
          || *str == 'G'
          || *str == 'X'
          || !(isalpha(*str) || *str == '_' || *str == '.')
          || ((*str == 'U' || *str == 'W' || *str == 'M' || *str == 'R')
              && str[1] == 0)
          || !strcasecmp(str, "true")
          || !strcasecmp(str, "false"))
        return false;
      // The remaining of the word must be alphanumeric.
      while (*++str)
        if (!(isalnum(*str) || *str == '_' || *str == '.'))
          return false;
      return true;
    }

    // If the formula has the form (!b)[*], return b.
    static formula
    strip_star_not(formula f)
    {
      return f.get_child_of({op::Star, op::Not});
    }

    // If the formula at position i in mo has the form
    // (!b)[*];b with b being a Boolean formula, return b.
    static formula
    match_goto(formula mo, unsigned i)
    {
      assert(i + 1 < mo.size());
      formula b = strip_star_not(mo[i]);
      if (b == nullptr || !b.is_boolean())
        return nullptr;
      if (mo[i + 1] == b)
        return b;
      return nullptr;
    }

    class to_string_visitor final
    {
    public:
      to_string_visitor(std::ostream& os,
                        bool full_parent = false,
                        bool ratexp = false,
                        const char** kw = spot_kw)
        : os_(os), top_level_(true),
        full_parent_(full_parent), in_ratexp_(ratexp),
        kw_(kw)
        {
        }

      void
        openp() const
      {
        if (in_ratexp_)
          emit(KOpenSERE);
        else
          os_ << '(';
      }

      void
        closep() const
      {
        if (in_ratexp_)
          emit(KCloseSERE);
        else
          os_ << ')';
      }

      std::ostream&
        emit(int symbol) const
      {
        return os_ << kw_[symbol];
      }

      void
        visit(formula f)
      {
        bool top_level = top_level_;
        top_level_ = false;

        auto s = f.size();
        bool want_par = (full_parent_ || s > 1) && !top_level;
        if (want_par)
          openp();

        auto emit_bunop_child = [this](formula b)
          {
            // b[*] is OK, no need to print {b}[*].  However we want
            // braces for {!b}[*], the only unary operator that can
            // be nested with Star/FStar.
            // If full_parent_ is set, we do NOT emit those extra
            // braces, because they are already output for the node
            // below.
            bool need_parent = (!full_parent_ && b.is(op::Not));
            if (need_parent)
              openp();
            this->visit(b);
            if (need_parent)
              closep();
          };

        op o = f.kind();
        switch (o)
          {
          case op::ff:
            emit(KFalse);
            break;
          case op::tt:
            emit(KTrue);
            break;
          case op::eword:
            emit(KEmptyWord);
            break;
          case op::ap:
            {
              const std::string& str = f.ap_name();
              if (!is_bare_word(str.c_str()))
                {
                  // Spin 6 supports atomic propositions such as (a == 0)
                  // as long as they are enclosed in parentheses.
                  if (kw_ == sclatex_kw  || kw_ == latex_kw)
                    escape_latex(os_ << "``\\mathit{", str)
                      << "}\\textrm{''}";
                  else if (kw_ != spin_kw)
                    escape_str(os_ << '"', str) << '"';
                  else if (!full_parent_)
                    os_ << '(' << str << ')';
                  else
                    os_ << str;
                }
              else
                {
                  if (kw_ == latex_kw || kw_ == sclatex_kw)
                    {
                      size_t s = str.size();
                      while (str[s - 1] >= '0' && str[s - 1] <= '9')
                        {
                          --s;
                          // bare words cannot start with digits
                          assert(s != 0);
                        }
                      if (s > 1)
                        os_ << "\\mathit{";
                      escape_latex(os_, str.substr(0, s));
                      if (s > 1)
                        os_ << '}';
                      if (s != str.size())
                        os_ << "_{"
                            << str.substr(s)
                            << '}';
                    }
                  else
                    {
                      os_ << str;
                    }
                }
              if (kw_ == wring_kw)
                os_ << "=1";

            }
            break;
          case op::Not:
            {
              formula c = f[0];
              if (c.is(op::ap))
                {
                  // If we negate a single letter in UTF-8, use a
                  // combining overline.
                  if (!full_parent_ && kw_ == utf8_kw)
                    {
                      auto& name = c.ap_name();
                      if (name.size() == 1 && is_bare_word(name.c_str()))
                        {
                          os_ << name << "̅";
                          break;
                        }
                    }
                  // If we negate an atomic proposition for Wring,
                  // output prop=0.
                  if (kw_ == wring_kw)
                    {
                      auto& name = c.ap_name();
                      if (is_bare_word(name.c_str()))
                        {
                          os_ << name << "=0";
                          break;
                        }
                    }
                }
              emit(KNot);
              visit(c);
              break;
            }
          case op::X:
            {
              emit(KX);
              bool cst = f[0].is_constant();
              if (cst)
                openp();
              visit(f[0]);
              if (cst)
                closep();
              break;
            }
          case op::strong_X:
            emit(KStrongX);
            visit(f[0]);
            break;
          case op::F:
            emit(KF);
            visit(f[0]);
            break;
          case op::G:
            emit(KG);
            visit(f[0]);
            break;
          case op::NegClosure:
          case op::NegClosureMarked:
            emit(KNot);
            if (o == op::NegClosureMarked)
              os_ << (kw_ == utf8_kw ? "̃": "+");
            SPOT_FALLTHROUGH;
          case op::Closure:
            os_ << '{';
            in_ratexp_ = true;
            top_level_ = true;
            visit(f[0]);
            os_ << '}';
            in_ratexp_ = false;
            top_level_ = false;
            break;
          case op::Xor:
            visit(f[0]);
            emit(KXor);
            visit(f[1]);
            break;
          case op::Implies:
            visit(f[0]);
            emit(KImplies);
            visit(f[1]);
            break;
          case op::Equiv:
            visit(f[0]);
            emit(KEquiv);
            visit(f[1]);
            break;
          case op::U:
            visit(f[0]);
            emit(KU);
            visit(f[1]);
            break;
          case op::R:
            visit(f[0]);
            emit(KR);
            visit(f[1]);
            break;
          case op::W:
            visit(f[0]);
            emit(KW);
            visit(f[1]);
            break;
          case op::M:
            visit(f[0]);
            emit(KM);
            visit(f[1]);
            break;
          case op::EConcat:
          case op::EConcatMarked:
          case op::UConcat:
            {
              in_ratexp_ = true;
              openp();
              top_level_ = true;
              formula left = f[0];
              formula right = f[1];
              unsigned last = left.size() - 1;
              bool onelast = false;
              if (left.is(op::Concat) && left[last].is_tt())
                {
                  visit(left.all_but(last));
                  onelast = true;
                }
              else
                {
                  visit(left);
                }
              top_level_ = false;
              closep();
              in_ratexp_ = false;
              if (o == op::UConcat)
                {
                  emit(onelast ? KTriggersNext : KTriggers);
                  visit(right);
                }
              else if (o == op::EConcatMarked)
                {
                  emit(onelast ? KSeqMarkedNext : KSeqMarked);
                  visit(right);
                }
              else if (o == op::EConcat)
                {
                  if (f[1].is_tt())
                    {
                      os_ << '!';
                      // No recursion on right.
                    }
                  else
                    {
                      emit(onelast ? KSeqNext : KSeq);
                      visit(right);
                    }
                }
              else
                {
                  SPOT_UNREACHABLE();
                }
            }
            break;
          case op::Or:
          case op::OrRat:
          case op::And:
          case op::AndRat:
          case op::AndNLM:
          case op::Fusion:
            {
              visit(f[0]);
              keyword k = KFalse; // Initialize to something to please GCC.
              switch (o)
                {
                case op::Or:
                  k = KOr;
                  break;
                case op::OrRat:
                  k = KOrRat;
                  break;
                case op::And:
                  k = in_ratexp_ ? KAndRat : KAnd;
                  break;
                case op::AndRat:
                  k = KAndRat;
                  break;
                case op::AndNLM:
                  k = KAndNLM;
                  break;
                case op::Fusion:
                  k = KFusion;
                  break;
                default:
                  SPOT_UNREACHABLE();
                }
              assert(k != KFalse);

              unsigned max = f.size();
              for (unsigned n = 1; n < max; ++n)
                {
                  emit(k);
                  visit(f[n]);
                }
              break;
            }
          case op::Concat:
            {
              unsigned max = f.size();

              for (unsigned i = 0; i < max; ++i)
                {
                  if (i > 0)
                    emit(KConcat);
                  if (i + 1 < max)
                    {
                      // Try to match (!b)[*];b
                      formula b = match_goto(f, i);
                      if (b != nullptr)
                        {
                          emit_bunop_child(b);

                          // Wait... maybe we are looking at (!b)[*];b;(!b)[*]
                          // in which case it's b[=1].
                          if (i + 2 < max && f[i] == f[i + 2])
                            {
                              emit(KEqualBunop);
                              os_ << '1';
                              emit(KCloseBunop);
                              i += 2;
                            }
                          else
                            {
                              emit(KGotoBunop);
                              emit(KCloseBunop);
                              ++i;
                            }
                          continue;
                        }
                      // Try to match ((!b)[*];b)[*i..j];(!b)[*]
                      formula fi = f[i];
                      if (fi.is(op::Star))
                        {
                          if (formula b2 = strip_star_not(f[i + 1]))
                            {
                              formula fic = fi[0];
                              if (fic.is(op::Concat))
                                if (formula b1 = match_goto(fic, 0))
                                  if (b1 == b2)
                                    {
                                      emit_bunop_child(b1);
                                      emit(KEqualBunop);
                                      unsigned min = fi.min();
                                      os_ << min;
                                      unsigned max = fi.max();
                                      if (max != min)
                                        {
                                          os_ << "..";
                                          if (max != formula::unbounded())
                                            os_ << max;
                                        }
                                      emit(KCloseBunop);
                                      ++i;
                                      continue;
                                    }
                            }
                        }
                    }
                  visit(f[i]);
                }
              break;
            }
          case op::Star:
          case op::FStar:
            {
              formula c = f[0];
              enum { Star, FStar, Goto } sugar = Star;
              unsigned default_min = 0;
              unsigned default_max = formula::unbounded();

              // Abbreviate "1[*]" as "[*]".
              if (!c.is_tt() || o != op::Star)
                {
                  if (o == op::Star)
                    {
                      // Is this a Goto?
                      if (c.is(op::Concat))
                        {
                          unsigned s = c.size();
                          if (s == 2)
                            if (formula b = match_goto(c, 0))
                              {
                                c = b;
                                sugar = Goto;
                              }
                        }
                    }
                  else if (o == op::FStar)
                    {
                      sugar = FStar;
                    }
                  else
                    {
                      SPOT_UNREACHABLE();
                    }
                  emit_bunop_child(c);
                }

              unsigned min = f.min();
              unsigned max = f.max();
              bool range = true;
              switch (sugar)
                {
                case Star:
                  if (min == 1 && max == formula::unbounded())
                    {
                      range = false;
                      emit(KPlusBunop);
                    }
                  else
                    {
                      emit(KStarBunop);
                    }
                  break;
                case FStar:
                  if (min == 1 && max == formula::unbounded())
                    {
                      range = false;
                      emit(KFPlusBunop);
                    }
                  else
                    {
                      emit(KFStarBunop);
                    }
                  break;
                case Goto:
                  emit(KGotoBunop);
                  default_min = 1;
                  default_max = 1;
                  break;
                }

              // Beware that the default parameters of the Goto operator are
              // not the same as Star or Equal:
              //
              //   [->]   = [->1..1]
              //   [->..] = [->1..unbounded]
              //   [*]    = [*0..unbounded]
              //   [*..]  = [*0..unbounded]
              //   [=]    = [=0..unbounded]
              //   [=..]  = [=0..unbounded]
              //
              // Strictly speaking [=] is not specified by PSL, and anyway we
              // automatically rewrite Exp[=0..unbounded] as
              // Exp[*0..unbounded], so we should never have to print [=]
              // here.
              //
              // Also
              //   [*..]  = [*0..unbounded]

              if (range)
                {
                  if (min != default_min || max != default_max)
                    {
                      // Always print the min_, even when it is equal to
                      // default_min, this way we avoid ambiguities (like
                      // when reading [*..3] near [->..2])
                      os_ << min;
                      if (min != max)
                        {
                          os_ << "..";
                          if (max != formula::unbounded())
                            os_ << max;
                        }
                    }
                  emit(KCloseBunop);
                }
            }
            break;
          case op::first_match:
            emit(KFirstMatch);
            os_ << '(';
            top_level_ = true;
            visit(f[0]);
            os_ << ')';
            break;
          }
        if (want_par)
          closep();
      }

    protected:
      std::ostream& os_;
      bool top_level_;
      bool full_parent_;
      bool in_ratexp_;
      const char** kw_;
    };


    std::ostream&
    printer_(std::ostream& os, formula f, bool full_parent,
             bool ratexp, const char** kw)
    {
      to_string_visitor v(os, full_parent, ratexp, kw);
      v.visit(f);
      return os;
    }

    std::string
    str_(formula f, bool full_parent, bool ratexp, const char** kw)
    {
      std::ostringstream os;
      printer_(os, f, full_parent, ratexp, kw);
      return os.str();
    }

  } // anonymous

  std::ostream&
  print_psl(std::ostream& os, formula f, bool full_parent)
  {
    return printer_(os, f, full_parent, false, spot_kw);
  }

  std::string
  str_psl(formula f, bool full_parent)
  {
    return str_(f, full_parent, false, spot_kw);
  }

  std::ostream&
  print_sere(std::ostream& os, formula f, bool full_parent)
  {
    return printer_(os, f, full_parent, true, spot_kw);
  }

  std::string
  str_sere(formula f, bool full_parent)
  {
    return str_(f, full_parent, true, spot_kw);
  }


  std::ostream&
  print_utf8_psl(std::ostream& os, formula f, bool full_parent)
  {
    return printer_(os, f, full_parent, false, utf8_kw);
  }

  std::string
  str_utf8_psl(formula f, bool full_parent)
  {
    return str_(f, full_parent, false, utf8_kw);
  }

  std::ostream&
  print_utf8_sere(std::ostream& os, formula f, bool full_parent)
  {
    return printer_(os, f, full_parent, true, utf8_kw);
  }

  std::string
  str_utf8_sere(formula f, bool full_parent)
  {
    return str_(f, full_parent, true, utf8_kw);
  }


  std::ostream&
  print_spin_ltl(std::ostream& os, formula f, bool full_parent)
  {
    to_string_visitor v(os, full_parent, false, spin_kw);
    v.visit(unabbreviate(f, "^MW"));
    return os;
  }

  std::string
  str_spin_ltl(formula f, bool full_parent)
  {
    std::ostringstream os;
    print_spin_ltl(os, f, full_parent);
    return os.str();
  }

  std::ostream&
  print_wring_ltl(std::ostream& os, formula f)
  {
    to_string_visitor v(os, true, false, wring_kw);
    v.visit(unabbreviate(f, "MW"));
    return os;
  }

  std::string
  str_wring_ltl(formula f)
  {
    std::ostringstream os;
    print_wring_ltl(os, f);
    return os.str();
  }

  std::ostream&
  print_latex_psl(std::ostream& os, formula f, bool full_parent)
  {
    return printer_(os, f, full_parent, false, latex_kw);
  }

  std::string
  str_latex_psl(formula f, bool full_parent)
  {
    return str_(f, full_parent, false, latex_kw);
  }

  std::ostream&
  print_latex_sere(std::ostream& os, formula f, bool full_parent)
  {
    return printer_(os, f, full_parent, true, latex_kw);
  }

  std::string
  str_latex_sere(formula f, bool full_parent)
  {
    return str_(f, full_parent, true, latex_kw);
  }


  std::ostream&
  print_sclatex_psl(std::ostream& os, formula f, bool full_parent)
  {
    return printer_(os, f, full_parent, false, sclatex_kw);
  }

  std::string
  str_sclatex_psl(formula f, bool full_parent)
  {
    return str_(f, full_parent, false, sclatex_kw);
  }

  std::ostream&
  print_sclatex_sere(std::ostream& os, formula f, bool full_parent)
  {
    return printer_(os, f, full_parent, true, sclatex_kw);
  }

  std::string
  str_sclatex_sere(formula f, bool full_parent)
  {
    return str_(f, full_parent, true, sclatex_kw);
  }

  namespace
  {
    // Does str match p[0-9]+ ?
    static bool
    is_pnum(const char* str)
    {
      if (str[0] != 'p' || str[1] == 0)
        return false;
      while (*++str)
        if (*str < '0' || *str > '9')
          return false;
      return true;
    }

    class lbt_visitor final
    {
    protected:
      std::ostream& os_;
      bool first_;
    public:

      lbt_visitor(std::ostream& os)
        : os_(os), first_(true)
        {
        }

      void
        visit(formula f)
      {
        if (first_)
          first_ = false;
        else
          os_ << ' ';

        op o = f.kind();
        switch (o)
          {
          case op::ff:
            os_ << 'f';
            break;
          case op::tt:
            os_ << 't';
            break;
          case op::ap:
            {
              const std::string& str = f.ap_name();
              if (!is_pnum(str.c_str()))
                escape_str(os_ << '"', str) << '"';
              else
                os_ << str;
              break;
            }
          case op::Not:
            os_ << '!';
            break;
          case op::X:
            os_ << 'X';
            break;
          case op::strong_X:
            os_ << 'X';         // unsupported
            break;
          case op::F:
            os_ << 'F';
            break;
          case op::G:
            os_ << 'G';
            break;
          case op::Xor:
            os_ << '^';
            break;
          case op::Implies:
            os_ << 'i';
            break;
          case op::Equiv:
            os_ << 'e';
            break;
          case op::U:
            os_ << 'U';
            break;
          case op::R:
            os_ << 'V';
            break;
          case op::W:
            os_ << 'W';
            break;
          case op::M:
            os_ << 'M';
            break;
          case op::Or:
            for (unsigned i = f.size() - 1; i != 0; --i)
              os_ << "| ";
            first_ = true;
            break;
          case op::And:
            for (unsigned i = f.size() - 1; i != 0; --i)
              os_ << "& ";
            first_ = true;
            break;
          case op::eword:
          case op::Closure:
          case op::NegClosure:
          case op::NegClosureMarked:
          case op::EConcat:
          case op::EConcatMarked:
          case op::UConcat:
          case op::OrRat:
          case op::AndRat:
          case op::AndNLM:
          case op::Concat:
          case op::Fusion:
          case op::Star:
          case op::FStar:
          case op::first_match:
            SPOT_UNIMPLEMENTED();
          }
        for (auto c: f)
          visit(c);
      }
    };

  } // anonymous

  std::ostream&
  print_lbt_ltl(std::ostream& os, formula f)
  {
    assert(f.is_ltl_formula());
    lbt_visitor v(os);
    v.visit(f);
    return os;
  }

  std::string
  str_lbt_ltl(formula f)
  {
    std::ostringstream os;
    print_lbt_ltl(os, f);
    return os.str();
  }
}
