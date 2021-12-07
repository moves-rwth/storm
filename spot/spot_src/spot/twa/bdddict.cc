// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2012-2019 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2003-2006 Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
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
#include <ostream>
#include <sstream>
#include <cassert>
#include <spot/tl/print.hh>
#include <spot/tl/formula.hh>
#include <spot/tl/defaultenv.hh>
#include "spot/priv/bddalloc.hh"
#include <spot/twa/bdddict.hh>

namespace spot
{

  // Empty anonymous namespace so that the style checker does no
  // complain about bdd_dict_priv (which should not be in an anonymous
  // namespace).
  namespace
  {
  }

  class bdd_dict_priv final: public bdd_allocator
  {
  public:

    class anon_free_list : public spot::free_list
    {
    public:
      // WARNING: We need a default constructor so this can be used in
      // a hash; but we should ensure that no object in the hash is
      // constructed with p==0.
      anon_free_list(bdd_dict_priv* p = nullptr)
        : priv_(p)
      {
      }

      anon_free_list(const anon_free_list& other) noexcept
        : free_list(other), priv_(other.priv_)
      {
      }

      spot::bdd_dict_priv::anon_free_list&
      operator=(const anon_free_list& other) noexcept
      {
        spot::free_list::operator=(other);
        priv_ = other.priv_;
        return *this;
      }

      virtual int
      extend(int n) override
      {
        assert(priv_);
        int b = priv_->allocate_variables(n);
        free_anonymous_list_of_type::iterator i;
        for (i = priv_->free_anonymous_list_of.begin();
             i != priv_->free_anonymous_list_of.end(); ++i)
          if (&i->second != this)
            i->second.insert(b, n);
        return b;
      }

    private:
      bdd_dict_priv* priv_;
    };

    bdd_dict_priv()
    {
      free_anonymous_list_of[nullptr] = anon_free_list(this);
    }

    /// List of unused anonymous variable number for each automaton.
    typedef std::map<const void*, anon_free_list> free_anonymous_list_of_type;
    free_anonymous_list_of_type free_anonymous_list_of;
  };

  bdd_dict::bdd_dict()
    // Initialize priv_ first, because it also initializes BuDDy
    : priv_(new bdd_dict_priv()),
      bdd_map(bdd_varnum())
  {
  }

  bdd_dict::~bdd_dict()
  {
    assert_emptiness();
    delete priv_;
  }

  int
  bdd_dict::register_proposition(formula f, const void* for_me)
  {
    int num;
    // Do not build a variable that already exists.
    fv_map::iterator sii = var_map.find(f);
    if (sii != var_map.end())
      {
        num = sii->second;
      }
    else
      {
        num = priv_->allocate_variables(1);
        var_map[f] = num;
        bdd_map.resize(bdd_varnum());
        bdd_map[num].type = var;
        bdd_map[num].f = f;
      }
    bdd_map[num].refs.insert(for_me);
    return num;
  }

  int
  bdd_dict::has_registered_proposition(formula f,
                                       const void* me)
  {
    auto ssi = var_map.find(f);
    if (ssi == var_map.end())
      return -1;
    int num = ssi->second;
    auto& r = bdd_map[num].refs;
    if (r.find(me) == r.end())
      return -1;
    return num;
  }

  int
  bdd_dict::register_acceptance_variable(formula f,
                                         const void* for_me)
  {
    int num;
    // Do not build an acceptance variable that already exists.
    fv_map::iterator sii = acc_map.find(f);
    if (sii != acc_map.end())
      {
        num = sii->second;
      }
    else
      {
        num = priv_->allocate_variables(1);
        acc_map[f] = num;
        bdd_map.resize(bdd_varnum());
        bdd_info& i = bdd_map[num];
        i.type = acc;
        i.f = f;
      }
    bdd_map[num].refs.insert(for_me);
    return num;
  }

  int
  bdd_dict::register_anonymous_variables(int n, const void* for_me)
  {
    typedef bdd_dict_priv::free_anonymous_list_of_type fal;
    fal::iterator i = priv_->free_anonymous_list_of.find(for_me);

    if (i == priv_->free_anonymous_list_of.end())
      {
        i = (priv_->free_anonymous_list_of.insert
             (fal::value_type(for_me,
                              priv_->free_anonymous_list_of[nullptr]))).first;
      }
    int res = i->second.register_n(n);

    bdd_map.resize(bdd_varnum());

    while (n--)
      {
        bdd_map[res + n].type = anon;
        bdd_map[res + n].refs.insert(for_me);
      }

    return res;
  }


  void
  bdd_dict::register_all_variables_of(const void* from_other,
                                      const void* for_me)
  {
    auto j = priv_->free_anonymous_list_of.find(from_other);
    if (j != priv_->free_anonymous_list_of.end())
      priv_->free_anonymous_list_of[for_me] = j->second;

    for (auto& i: bdd_map)
      {
        ref_set& s = i.refs;
        if (s.find(from_other) != s.end())
          s.insert(for_me);
      }

  }

  void
  bdd_dict::unregister_variable(int v, const void* me)
  {
    assert(unsigned(v) < bdd_map.size());

    ref_set& s = bdd_map[v].refs;
    // If the variable is not owned by me, ignore it.
    ref_set::iterator si = s.find(me);
    if (si == s.end())
      return;

    s.erase(si);

    // If var is anonymous, we should reinsert it into the free list
    // of ME's anonymous variables.
    if (bdd_map[v].type == anon)
      priv_->free_anonymous_list_of[me].release_n(v, 1);

    if (!s.empty())
      return;

    // ME was the last user of this variable.
    // Let's free it.  First, we need to find
    // if this is a Var or an Acc variable.
    formula f = nullptr;
    switch (bdd_map[v].type)
      {
      case var:
        f = bdd_map[v].f;
        var_map.erase(f);
        break;
      case acc:
        f = bdd_map[v].f;
        acc_map.erase(f);
        break;
      case anon:
        {
          // Nobody use this variable as an anonymous variable
          // anymore, so remove it entirely from the anonymous
          // free list so it can be used for something else.
          for (auto& fal: priv_->free_anonymous_list_of)
            fal.second.remove(v, 1);
          break;
        }
      }
    // Actually release the associated BDD variables, and the
    // formula itself.
    priv_->release_variables(v, 1);
    bdd_map[v].type = anon;
    bdd_map[v].f = nullptr;
  }

  void
  bdd_dict::unregister_all_my_variables(const void* me)
  {
    unsigned s = bdd_map.size();
    for (unsigned i = 0; i < s; ++i)
      unregister_variable(i, me);
    priv_->free_anonymous_list_of.erase(me);
  }

  std::ostream&
  bdd_dict::dump(std::ostream& os) const
  {
    os << "Variable Map:\n";
    unsigned s = bdd_map.size();
    for (unsigned i = 0; i < s; ++i)
      {
        os << ' ' << i << ' ';
        const bdd_info& r = bdd_map[i];
        switch (r.type)
          {
          case anon:
            os << (r.refs.empty() ? "Free" : "Anon");
            break;
          case acc:
            os << "Acc[";
            print_psl(os, r.f) << ']';
            break;
          case var:
            os << "Var[";
            print_psl(os, r.f) << ']';
            break;
          }
        if (!r.refs.empty())
          {
            os << " x" << r.refs.size() << " {";
            for (ref_set::const_iterator si = r.refs.begin();
                 si != r.refs.end(); ++si)
              os << ' ' << *si;
            os << " }";
          }
        os << '\n';
      }
    os << "Anonymous lists:\n";
    bdd_dict_priv::free_anonymous_list_of_type::const_iterator ai;
    for (ai = priv_->free_anonymous_list_of.begin();
         ai != priv_->free_anonymous_list_of.end(); ++ai)
      {
        os << "  [" << ai->first << "] ";
        ai->second.dump_free_list(os) << std::endl;
      }
    os << "Free list:\n";
    priv_->dump_free_list(os);
    os << '\n';
    return os;
  }

  void
  bdd_dict::assert_emptiness() const
  {
    bool fail = false;
    bool var_seen = false;
    bool acc_seen = false;
    bool refs_seen = false;
    unsigned s = bdd_map.size();
    for (unsigned i = 0; i < s; ++i)
      {
        switch (bdd_map[i].type)
          {
          case var:
            var_seen = true;
            break;
          case acc:
            acc_seen = true;
            break;
          case anon:
            break;
          }
        refs_seen |= !bdd_map[i].refs.empty();
      }
    if (var_map.empty() && acc_map.empty())
      {
        if (var_seen)
          {
            std::cerr << "var_map is empty but Var in map\n";
            fail = true;
          }
        if (acc_seen)
          {
            std::cerr << "acc_map is empty but Acc in map\n";
            fail = true;
          }
        if (refs_seen)
          {
            std::cerr << "maps are empty but var_refs is not\n";
            fail = true;
          }
        if (!fail)
          return;
      }
    else
      {
        std::cerr << "some maps are not empty\n";
      }
    dump(std::cerr);
    abort();
  }



}
