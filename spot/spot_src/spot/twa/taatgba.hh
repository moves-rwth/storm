// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2011-2019 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
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

#include <set>
#include <iosfwd>
#include <vector>
#include <string>
#include <spot/misc/hash.hh>
#include <spot/tl/formula.hh>
#include <spot/twa/bdddict.hh>
#include <spot/twa/twa.hh>

namespace spot
{
  /// \brief A self-loop Transition-based Alternating Automaton (TAA)
  /// which is seen as a TGBA (abstract class, see below).
  class SPOT_API taa_tgba: public twa
  {
  public:
    taa_tgba(const bdd_dict_ptr& dict);

    struct transition;
    typedef std::list<transition*> state;
    typedef std::set<state*> state_set;

    /// Explicit transitions.
    struct transition
    {
      bdd condition;
      acc_cond::mark_t acceptance_conditions;
      const state_set* dst;
    };

    void add_condition(transition* t, formula f);

    /// TGBA interface.
    virtual ~taa_tgba();
    virtual spot::state* get_init_state() const override final;
    virtual twa_succ_iterator* succ_iter(const spot::state* state)
      const override final;

  protected:

    typedef std::vector<taa_tgba::state_set*> ss_vec;

    taa_tgba::state_set* init_;
    ss_vec state_set_vec_;

    std::map<formula, acc_cond::mark_t> acc_map_;

  private:
    // Disallow copy.
    taa_tgba(const taa_tgba& other) = delete;
    taa_tgba& operator=(const taa_tgba& other) = delete;
  };

  /// Set of states deriving from spot::state.
  class SPOT_API set_state final: public spot::state
  {
  public:
    set_state(const taa_tgba::state_set* s, bool delete_me = false)
      : s_(s), delete_me_(delete_me)
    {
    }

    virtual int compare(const spot::state*) const override;
    virtual size_t hash() const override;
    virtual set_state* clone() const override;

    virtual ~set_state()
    {
      if (delete_me_)
        delete s_;
    }

    const taa_tgba::state_set* get_state() const;
  private:
    const taa_tgba::state_set* s_;
    bool delete_me_;
  };

  class SPOT_API taa_succ_iterator final: public twa_succ_iterator
  {
  public:
    taa_succ_iterator(const taa_tgba::state_set* s, const acc_cond& acc);
    virtual ~taa_succ_iterator();

    virtual bool first() override;
    virtual bool next() override;
    virtual bool done() const override;

    virtual set_state* dst() const override;
    virtual bdd cond() const override;
    virtual acc_cond::mark_t acc() const override;

  private:
    /// Those typedefs are used to generate all possible successors in
    /// the constructor using a cartesian product.
    typedef taa_tgba::state::const_iterator iterator;
    typedef std::pair<iterator, iterator> iterator_pair;
    typedef std::vector<iterator_pair> bounds_t;
    typedef std::unordered_map<const spot::set_state*,
                               std::vector<taa_tgba::transition*>,
                               state_ptr_hash, state_ptr_equal> seen_map;

    struct distance_sort :
      public std::binary_function<const iterator_pair&,
                                  const iterator_pair&, bool>
    {
      bool
      operator()(const iterator_pair& lhs, const iterator_pair& rhs) const
      {
        return std::distance(lhs.first, lhs.second) <
               std::distance(rhs.first, rhs.second);
      }
    };

    std::vector<taa_tgba::transition*>::const_iterator i_;
    std::vector<taa_tgba::transition*> succ_;
    seen_map seen_;
    const acc_cond& acc_;
  };

  /// A taa_tgba instance with states labeled by a given type.
  /// Still an abstract class, see below.
  template<typename label>
  class SPOT_API taa_tgba_labelled: public taa_tgba
  {
  public:
    taa_tgba_labelled(const bdd_dict_ptr& dict) : taa_tgba(dict) {};

    ~taa_tgba_labelled()
    {
      for (auto i: name_state_map_)
        {
          for (auto i2: *i.second)
            delete i2;
          delete i.second;
        }
    }

    void set_init_state(const label& s)
    {
      std::vector<label> v(1);
      v[0] = s;
      set_init_state(v);
    }
    void set_init_state(const std::vector<label>& s)
    {
      init_ = add_state_set(s);
    }

    transition*
    create_transition(const label& s,
                      const std::vector<label>& d)
    {
      state* src = add_state(s);
      state_set* dst = add_state_set(d);
      transition* t = new transition;
      t->dst = dst;
      t->condition = bddtrue;
      t->acceptance_conditions = {};
      src->emplace_back(t);
      return t;
    }

    transition*
    create_transition(const label& s, const label& d)
    {
      std::vector<std::string> vec;
      vec.emplace_back(d);
      return create_transition(s, vec);
    }

    void add_acceptance_condition(transition* t, formula f)
    {
      auto p = acc_map_.emplace(f, acc_cond::mark_t({}));
      if (p.second)
        p.first->second = acc_cond::mark_t({acc().add_set()});
      t->acceptance_conditions |= p.first->second;
    }

    /// \brief Format the state as a string for printing.
    ///
    /// If state is a spot::set_state of only one element, then the
    /// string corresponding to state->get_state() is returned.
    ///
    /// Otherwise a string composed of each string corresponding to
    /// each state->get_state() in the spot::set_state is returned,
    /// e.g. like {string_1,...,string_n}.
    virtual std::string format_state(const spot::state* s) const override
    {
      const spot::set_state* se = down_cast<const spot::set_state*>(s);
      const state_set* ss = se->get_state();
      return format_state_set(ss);
    }

    /// \brief Output a TAA in a stream.
    void output(std::ostream& os) const
    {
      typename ns_map::const_iterator i;
      for (i = name_state_map_.begin(); i != name_state_map_.end(); ++i)
      {
        taa_tgba::state::const_iterator i2;
        os << "State: " << label_to_string(i->first) << std::endl;
        for (i2 = i->second->begin(); i2 != i->second->end(); ++i2)
        {
          os << ' ' << format_state_set((*i2)->dst)
             << ", C:" << (*i2)->condition
             << ", A:" << (*i2)->acceptance_conditions << std::endl;
        }
      }
    }

  protected:
    typedef label label_t;

    typedef std::unordered_map<label, taa_tgba::state*> ns_map;
    typedef std::unordered_map<const taa_tgba::state*, label,
                               ptr_hash<taa_tgba::state> > sn_map;

    ns_map name_state_map_;
    sn_map state_name_map_;

    /// \brief Return a label as a string.
    virtual std::string label_to_string(const label_t& lbl) const = 0;

  private:
    /// \brief Return the taa_tgba::state for \a name, creating it
    /// when it does not exist already.
    taa_tgba::state* add_state(const label& name)
    {
      typename ns_map::iterator i = name_state_map_.find(name);
      if (i == name_state_map_.end())
      {
        taa_tgba::state* s = new taa_tgba::state;
        name_state_map_[name] = s;
        state_name_map_[s] = name;
        return s;
      }
      return i->second;
    }

    /// \brief Return the taa::state_set for \a names.
    taa_tgba::state_set* add_state_set(const std::vector<label>& names)
    {
      state_set* ss = new state_set;
      for (unsigned i = 0; i < names.size(); ++i)
        ss->insert(add_state(names[i]));
      state_set_vec_.emplace_back(ss);
      return ss;
    }

    std::string format_state_set(const taa_tgba::state_set* ss) const
    {
      state_set::const_iterator i1 = ss->begin();
      typename sn_map::const_iterator i2;
      if (ss->empty())
        return std::string("{}");
      if (ss->size() == 1)
      {
        i2 = state_name_map_.find(*i1);
        SPOT_ASSERT(i2 != state_name_map_.end());
        return "{" + label_to_string(i2->second) + "}";
      }
      else
      {
        std::string res("{");
        while (i1 != ss->end())
        {
          i2 = state_name_map_.find(*i1++);
          SPOT_ASSERT(i2 != state_name_map_.end());
          res += label_to_string(i2->second);
          res += ",";
        }
        res[res.size() - 1] = '}';
        return res;
      }
    }
  };

  class SPOT_API taa_tgba_string final:
#ifndef SWIG
    public taa_tgba_labelled<std::string>
#else
    public taa_tgba
#endif
  {
  public:
    taa_tgba_string(const bdd_dict_ptr& dict) :
      taa_tgba_labelled<std::string>(dict) {}
    ~taa_tgba_string()
      {}
  protected:
    virtual std::string label_to_string(const std::string& label)
      const override;
  };

  typedef std::shared_ptr<taa_tgba_string> taa_tgba_string_ptr;
  typedef std::shared_ptr<const taa_tgba_string> const_taa_tgba_string_ptr;

  inline taa_tgba_string_ptr make_taa_tgba_string(const bdd_dict_ptr& dict)
  {
    return SPOT_make_shared_enabled__(taa_tgba_string, dict);
  }

  class SPOT_API taa_tgba_formula final:
#ifndef SWIG
    public taa_tgba_labelled<formula>
#else
    public taa_tgba
#endif
  {
  public:
    taa_tgba_formula(const bdd_dict_ptr& dict) :
      taa_tgba_labelled<formula>(dict) {}
    ~taa_tgba_formula()
      {}
  protected:
    virtual std::string label_to_string(const label_t& label)
      const override;
  };

  typedef std::shared_ptr<taa_tgba_formula> taa_tgba_formula_ptr;
  typedef std::shared_ptr<const taa_tgba_formula> const_taa_tgba_formula_ptr;

  inline taa_tgba_formula_ptr make_taa_tgba_formula(const bdd_dict_ptr& dict)
  {
    return SPOT_make_shared_enabled__(taa_tgba_formula, dict);
  }
}
