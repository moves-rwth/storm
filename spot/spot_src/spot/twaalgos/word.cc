// -*- coding: utf-8 -*-
// Copyright (C) 2013-2019 Laboratoire de Recherche et Développement
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

#include "config.h"
#include <spot/twaalgos/word.hh>
#include <spot/twa/bddprint.hh>
#include <spot/twa/bdddict.hh>
#include <spot/tl/parse.hh>
#include <spot/tl/simplify.hh>
#include <spot/tl/apcollect.hh>

using namespace std::string_literals;

namespace spot
{
  twa_word::twa_word(const twa_run_ptr& run) noexcept
    : dict_(run->aut->get_dict())
  {
    for (auto& i: run->prefix)
      prefix.emplace_back(i.label);
    for (auto& i: run->cycle)
      cycle.emplace_back(i.label);
    dict_->register_all_variables_of(run->aut, this);
  }

  twa_word::twa_word(const bdd_dict_ptr& dict) noexcept
    : dict_(dict)
  {
  }

  void
  twa_word::use_all_aps(bdd aps, bool positive)
  {
    bdd def = positive ?
      static_cast<bdd>(bddtrue) : static_cast<bdd>(bddfalse);
    for (bdd& i: prefix)
      i = bdd_satoneset(i, aps, def);
    for (bdd& i: cycle)
      i = bdd_satoneset(i, aps, def);
  }

  void
  twa_word::simplify()
  {
    // If all the formulas on the cycle are compatible, reduce the
    // cycle to a simple conjunction.
    //
    // For instance
    //   !a|!b; b; a&b; cycle{a; b; a&b}
    // can be reduced to
    //   !a|!b; b; a&b; cycle{a&b}
    {
      bdd all = bddtrue;
      for (auto& i: cycle)
        all &= i;
      if (all != bddfalse)
        {
          cycle.clear();
          cycle.emplace_back(all);
        }
    }
    // If the last formula of the prefix is compatible with the
    // last formula of the cycle, we can shift the cycle and
    // reduce the prefix.
    //
    // For instance
    //   !a|!b; b; a&b; cycle{a&b}
    // can be reduced to
    //   !a|!b; cycle{a&b}
    while (!prefix.empty())
      {
        bdd a = prefix.back() & cycle.back();
        if (a == bddfalse)
          break;
        prefix.pop_back();
        cycle.pop_back();
        cycle.push_front(a);
      }
    // Get rid of any disjunction.
    //
    // For instance
    //   !a|!b; cycle{a&b}
    // can be reduced to
    //   !a&!b; cycle{a&b}
    for (auto& i: prefix)
      i = bdd_satone(i);
    for (auto& i: cycle)
      i = bdd_satone(i);
  }

  std::ostream&
  operator<<(std::ostream& os, const twa_word& w)
  {
    if (w.cycle.empty())
      throw std::runtime_error("a twa_word may not have an empty cycle");
    auto d = w.get_dict();
    if (!w.prefix.empty())
      for (auto& i: w.prefix)
        {
          bdd_print_formula(os, d, i);
          os << "; ";
        }
    bool notfirst = false;
    os << "cycle{";
    for (auto& i: w.cycle)
      {
        if (notfirst)
          os << "; ";
        notfirst = true;
        bdd_print_formula(os, d, i);
      }
    os << '}';
    return os;
  }

  namespace
  {
    static void word_parse_error(const std::string& word,
                                 size_t i, parsed_formula pf)
    {
      std::ostringstream os;
      pf.format_errors(os, word, i);
      throw parse_error(os.str());
    }

    static void word_parse_error(const std::string& word, size_t i,
                                 const std::string& message)
    {
      if (i == std::string::npos)
        i = word.size();
      std::ostringstream s;
      s << ">>> " << word << '\n';
      for (auto j = i + 4; j > 0; --j)
        s << ' ';
      s << '^' << '\n';
      s << message << '\n';
      throw parse_error(s.str());
    }

    static size_t skip_next_formula(const std::string& word, size_t begin)
    {
      bool quoted = false;
      auto size = word.size();
      for (auto j = begin; j < size; ++j)
        {
          auto c = word[j];
          if (!quoted && (c == ';' || c == '}'))
            return j;
          if (c == '"')
            quoted = !quoted;
          else if (quoted && c == '\\')
            ++j;
        }
      if (quoted)
        word_parse_error(word, word.size(), "Unclosed string");
      return std::string::npos;
    }
  }

  twa_word_ptr parse_word(const std::string& word, const bdd_dict_ptr& dict)
  {
    atomic_prop_set aps;
    tl_simplifier tls(dict);
    twa_word_ptr tw = make_twa_word(dict);
    size_t i = 0;
    auto ind = i;

    auto extract_bdd =
      [&](typename twa_word::seq_t& seq)
      {
        auto sub = word.substr(i, ind - i);
        auto pf = spot::parse_infix_boolean(sub);
        if (!pf.errors.empty())
          word_parse_error(word, i, pf);
        atomic_prop_collect(pf.f, &aps);
        seq.emplace_back(tls.as_bdd(pf.f));
        if (word[ind] == '}')
          return true;
        // Skip blanks after semi-colon
        i = word.find_first_not_of(' ', ind + 1);
        return false;
      };

    // Parse the prefix part. Can be empty.
    while (word.substr(i, 6) != "cycle{"s)
      {
        ind = skip_next_formula(word, i);
        if (ind == std::string::npos)
          word_parse_error(word, word.size(),
                           "A twa_word must contain a cycle");
        if (word[ind] == '}')
          word_parse_error(word, ind, "Expected ';' delimiter: "
                           "'}' stands for ending a cycle");
        // Exract formula, convert it to bdd and add it to the prefix sequence
        extract_bdd(tw->prefix);
        if (i == std::string::npos)
          word_parse_error(word, ind + 1, "Missing cycle in formula");
      }
    // Consume "cycle{"
    i += 6;
    while (true)
      {
        ind = skip_next_formula(word, i);
        if (ind == std::string::npos)
          word_parse_error(word, word.size(),
                           "Missing ';' or '}' after formula");
        // Extract formula, convert it to bdd and add it to the cycle sequence
        // Break if an '}' is encountered
        if (extract_bdd(tw->cycle))
          break;
        if (i == std::string::npos)
          word_parse_error(word, ind + 1,
                           "Missing end of cycle character: '}'");
      }
    if (ind != word.size() - 1)
      word_parse_error(word, ind + 1, "Input should be finished after cycle");
    for (auto ap: aps)
      dict->register_proposition(ap, tw.get());
    return tw;
  }

  twa_graph_ptr twa_word::as_automaton() const
  {
    twa_graph_ptr aut = make_twa_graph(dict_);

    aut->prop_weak(true);
    aut->prop_universal(true);

    // Register the atomic propositions used in the word.
    {
      bdd support = bddtrue;
      for (auto b: prefix)
        support &= bdd_support(b);
      for (auto b: cycle)
        support &= bdd_support(b);
      while (support != bddtrue)
        {
          int v = bdd_var(support);
          support = bdd_high(support);
          aut->register_ap(dict_->bdd_map[v].f);
        }
    }

    size_t i = 0;
    aut->new_states(prefix.size() + cycle.size());
    for (auto b: prefix)
      {
        aut->new_edge(i, i + 1, b);
        ++i;
      }
    size_t j = i;
    auto b = cycle.begin();
    auto end = --cycle.end();
    for (; b != end; ++b)
      {
        aut->new_edge(i, i + 1, *b);
        ++i;
      }
    // Close the loop
    aut->new_edge(i, j, *b);
    return aut;
  }
}
