// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2012, 2014-2020 Laboratoire de
// Recherche et Développement de l'Epita (LRDE)
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
#include <ltdl.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

// MinGW does not define this.
#ifndef WEXITSTATUS
# define WEXITSTATUS(x) ((x) & 0xff)
#endif

#include <spot/ltsmin/ltsmin.hh>
#include <spot/misc/hashfunc.hh>
#include <spot/misc/fixpool.hh>
#include <spot/misc/mspool.hh>
#include <spot/misc/intvcomp.hh>
#include <spot/misc/intvcmp2.hh>

using namespace std::string_literals;

namespace spot
{
  namespace
  {

    ////////////////////////////////////////////////////////////////////////
    // spins interface

    typedef struct transition_info {
      int* labels; // edge labels, NULL, or pointer to the edge label(s)
      int  group;  // holds transition group or -1 if unknown
    } transition_info_t;

    typedef void (*TransitionCB)(void *ctx,
                                 transition_info_t *transition_info,
                                 int *dst);
  }

  struct spins_interface
  {
    lt_dlhandle handle;        // handle to the dynamic library
    void (*get_initial_state)(void *to);
    int (*have_property)();
    int (*get_successors)(void* m, int *in, TransitionCB, void *arg);
    int (*get_state_size)();
    const char* (*get_state_variable_name)(int var);
    int (*get_state_variable_type)(int var);
    int (*get_type_count)();
    const char* (*get_type_name)(int type);
    int (*get_type_value_count)(int type);
    const char* (*get_type_value_name)(int type, int value);

    ~spins_interface()
    {
      if (handle)
        lt_dlclose(handle);
      lt_dlexit();
    }
  };

  namespace
  {
    typedef std::shared_ptr<const spins_interface> spins_interface_ptr;

    ////////////////////////////////////////////////////////////////////////
    // STATE

    struct spins_state final: public state
    {
      spins_state(int s, fixed_size_pool* p)
        : pool(p), size(s), count(1)
      {
      }

      void compute_hash()
      {
        hash_value = 0;
        for (int i = 0; i < size; ++i)
          hash_value = wang32_hash(hash_value ^ vars[i]);
      }

      spins_state* clone() const override
      {
        ++count;
        return const_cast<spins_state*>(this);
      }

      void destroy() const override
      {
        if (--count)
          return;
        pool->deallocate(const_cast<spins_state*>(this));
      }

      size_t hash() const override
      {
        return hash_value;
      }

      int compare(const state* other) const override
      {
        if (this == other)
          return 0;
        const spins_state* o = down_cast<const spins_state*>(other);
        if (hash_value < o->hash_value)
          return -1;
        if (hash_value > o->hash_value)
          return 1;
        return memcmp(vars, o->vars, size * sizeof(*vars));
      }

    private:

      ~spins_state()
      {
      }

    public:
      fixed_size_pool* pool;
      size_t hash_value: 32;
      int size: 16;
      mutable unsigned count: 16;
      int vars[1];
    };

    struct spins_compressed_state final: public state
    {
      spins_compressed_state(int s, multiple_size_pool* p)
        : pool(p), size(s), count(1)
      {
      }

      void compute_hash()
      {
        hash_value = 0;
        for (int i = 0; i < size; ++i)
          hash_value = wang32_hash(hash_value ^ vars[i]);
      }

      spins_compressed_state* clone() const override
      {
        ++count;
        return const_cast<spins_compressed_state*>(this);
      }

      void destroy() const override
      {
        if (--count)
          return;
        pool->deallocate(this, sizeof(*this) - sizeof(vars)
                         + size * sizeof(*vars));
      }

      size_t hash() const override
      {
        return hash_value;
      }

      int compare(const state* other) const override
      {
        if (this == other)
          return 0;
        const spins_compressed_state* o =
          down_cast<const spins_compressed_state*>(other);
        if (hash_value < o->hash_value)
          return -1;
        if (hash_value > o->hash_value)
          return 1;

        if (size < o->size)
          return -1;
        if (size > o->size)
          return 1;

        return memcmp(vars, o->vars, size * sizeof(*vars));
      }

    private:

      ~spins_compressed_state()
      {
      }

    public:
      multiple_size_pool* pool;
      size_t hash_value: 32;
      int size: 16;
      mutable unsigned count: 16;
      int vars[1];
    };

    ////////////////////////////////////////////////////////////////////////
    // CALLBACK FUNCTION for transitions.

    struct callback_context
    {
      typedef std::list<state*> transitions_t;
      transitions_t transitions;
      int state_size;
      void* pool;
      int* compressed;
      void (*compress)(const int*, size_t, int*, size_t&);

      ~callback_context()
      {
        for (auto t: transitions)
          t->destroy();
      }
    };

    void transition_callback(void* arg, transition_info_t*, int *dst)
    {
      callback_context* ctx = static_cast<callback_context*>(arg);
      fixed_size_pool* p = static_cast<fixed_size_pool*>(ctx->pool);
      spins_state* out =
        new(p->allocate()) spins_state(ctx->state_size, p);
      SPOT_ASSUME(out != nullptr);
      memcpy(out->vars, dst, ctx->state_size * sizeof(int));
      out->compute_hash();
      ctx->transitions.emplace_back(out);
    }

    void transition_callback_compress(void* arg, transition_info_t*, int *dst)
    {
      callback_context* ctx = static_cast<callback_context*>(arg);
      multiple_size_pool* p = static_cast<multiple_size_pool*>(ctx->pool);

      size_t csize = ctx->state_size * 2;
      ctx->compress(dst, ctx->state_size, ctx->compressed, csize);

      void* mem = p->allocate(sizeof(spins_compressed_state)
                              - sizeof(spins_compressed_state::vars)
                              + sizeof(int) * csize);
      spins_compressed_state* out = new(mem) spins_compressed_state(csize, p);
      SPOT_ASSUME(out != nullptr);
      memcpy(out->vars, ctx->compressed, csize * sizeof(int));
      out->compute_hash();
      ctx->transitions.emplace_back(out);
    }

    ////////////////////////////////////////////////////////////////////////
    // SUCC_ITERATOR

    class spins_succ_iterator final: public kripke_succ_iterator
    {
    public:

      spins_succ_iterator(const callback_context* cc,
                         bdd cond)
        : kripke_succ_iterator(cond), cc_(cc)
      {
      }

      void recycle(const callback_context* cc, bdd cond)
      {
        delete cc_;
        cc_ = cc;
        kripke_succ_iterator::recycle(cond);
      }

      ~spins_succ_iterator()
      {
        delete cc_;
      }

      virtual bool first() override
      {
        it_ = cc_->transitions.begin();
        return it_ != cc_->transitions.end();
      }

      virtual bool next() override
      {
        ++it_;
        return it_ != cc_->transitions.end();
      }

      virtual bool done() const override
      {
        return it_ == cc_->transitions.end();
      }

      virtual state* dst() const override
      {
        return (*it_)->clone();
      }

    private:
      const callback_context* cc_;
      callback_context::transitions_t::const_iterator it_;
    };

    ////////////////////////////////////////////////////////////////////////
    // PREDICATE EVALUATION

    typedef enum { OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE } relop;

    struct one_prop
    {
      int var_num;
      relop op;
      int val;
      int bddvar;  // if "var_num op val" is true, output bddvar,
                   // else its negation
    };
    typedef std::vector<one_prop> prop_set;


    struct var_info
    {
      int num;
      int type;
    };


    void
    convert_aps(const atomic_prop_set* aps,
                spins_interface_ptr d,
                bdd_dict_ptr dict,
                formula dead,
                prop_set& out)
    {
      int errors = 0;
      std::ostringstream err;

      int state_size = d->get_state_size();
      typedef std::map<std::string, var_info> val_map_t;
      val_map_t val_map;

      for (int i = 0; i < state_size; ++i)
        {
          const char* name = d->get_state_variable_name(i);
          int type = d->get_state_variable_type(i);
          var_info v = { i , type };
          val_map[name] = v;
        }

      int type_count = d->get_type_count();
      typedef std::map<std::string, int> enum_map_t;
      std::vector<enum_map_t> enum_map(type_count);
      for (int i = 0; i < type_count; ++i)
        {
          int enum_count = d->get_type_value_count(i);
          for (int j = 0; j < enum_count; ++j)
            enum_map[i].emplace(d->get_type_value_name(i, j), j);
        }

      for (atomic_prop_set::const_iterator ap = aps->begin();
           ap != aps->end(); ++ap)
        {
          if (*ap == dead)
            continue;

          const std::string& str = ap->ap_name();
          const char* s = str.c_str();

          // Skip any leading blank.
          while (*s && (*s == ' ' || *s == '\t'))
            ++s;
          if (!*s)
            {
              err << "Proposition `" << str << "' cannot be parsed.\n";
              ++errors;
              continue;
            }


          char* name = (char*) malloc(str.size() + 1);
          char* name_p = name;
          char* lastdot = nullptr;
          while (*s && (*s != '=') && *s != '<' && *s != '!'  && *s != '>')
            {

              if (*s == ' ' || *s == '\t')
                ++s;
              else
                {
                  if (*s == '.')
                    lastdot = name_p;
                  *name_p++ = *s++;
                }
            }
          *name_p = 0;

          if (name == name_p)
            {
              err << "Proposition `" << str << "' cannot be parsed.\n";
              free(name);
              ++errors;
              continue;
            }

          // Lookup the name
          val_map_t::const_iterator ni = val_map.find(name);
          if (ni == val_map.end())
            {
              // We may have a name such as X.Y.Z
              // If it is not a known variable, it might mean
              // an enumerated variable X.Y with value Z.
              if (lastdot)
                {
                  *lastdot++ = 0;
                  ni = val_map.find(name);
                }

              if (ni == val_map.end())
                {
                  err << "No variable `" << name
                      << "' found in model (for proposition `"
                      << str << "').\n";
                  free(name);
                  ++errors;
                  continue;
                }

              // We have found the enumerated variable, and lastdot is
              // pointing to its expected value.
              int type_num = ni->second.type;
              enum_map_t::const_iterator ei = enum_map[type_num].find(lastdot);
              if (ei == enum_map[type_num].end())
                {
                  err << "No state `" << lastdot << "' known for variable `"
                      << name << "'.\n";
                  err << "Possible states are:";
                  for (auto& ej: enum_map[type_num])
                    err << ' ' << ej.first;
                  err << '\n';

                  free(name);
                  ++errors;
                  continue;
                }

              // At this point, *s should be 0.
              if (*s)
                {
                  err << "Trailing garbage `" << s
                      << "' at end of proposition `"
                      << str << "'.\n";
                  free(name);
                  ++errors;
                  continue;
                }

              // Record that X.Y must be equal to Z.
              int v = dict->register_proposition(*ap, d.get());
              one_prop p = { ni->second.num, OP_EQ, ei->second, v };
              out.emplace_back(p);
              free(name);
              continue;
            }

          int var_num = ni->second.num;

          if (!*s)                // No operator?  Assume "!= 0".
            {
              int v = dict->register_proposition(*ap, d);
              one_prop p = { var_num, OP_NE, 0, v };
              out.emplace_back(p);
              free(name);
              continue;
            }

          relop op;

          switch (*s)
            {
            case '!':
              if (s[1] != '=')
                goto report_error;
              op = OP_NE;
              s += 2;
              break;
            case '=':
              if (s[1] != '=')
                goto report_error;
              op = OP_EQ;
              s += 2;
              break;
            case '<':
              if (s[1] == '=')
                {
                  op = OP_LE;
                  s += 2;
                }
              else
                {
                  op = OP_LT;
                  ++s;
                }
              break;
            case '>':
              if (s[1] == '=')
                {
                  op = OP_GE;
                  s += 2;
                }
              else
                {
                  op = OP_GT;
                  ++s;
                }
              break;
            default:
            report_error:
              err << "Unexpected `" << s
                  << "' while parsing atomic proposition `" << str
                  << "'.\n";
              ++errors;
              free(name);
              continue;
            }

          while (*s && (*s == ' ' || *s == '\t'))
            ++s;

          int val = 0; // Initialize to kill a warning from old compilers.
          int type_num = ni->second.type;
          if (type_num == 0 || (*s >= '0' && *s <= '9') || *s == '-')
            {
              char* s_end;
              val = strtol(s, &s_end, 10);
              if (s == s_end)
                {
                  err << "Failed to parse `" << s << "' as an integer.\n";
                  ++errors;
                  free(name);
                  continue;
                }
              s = s_end;
            }
          else
            {
              // We are in a case such as P_0 == S, trying to convert
              // the string S into an integer.
              const char* end = s;
              while (*end && *end != ' ' && *end != '\t')
                ++end;
              std::string st(s, end);

              // Lookup the string.
              enum_map_t::const_iterator ei = enum_map[type_num].find(st);
              if (ei == enum_map[type_num].end())
                {
                  err << "No state `" << st << "' known for variable `"
                      << name << "'.\n";
                  err << "Possible states are:";
                  for (ei = enum_map[type_num].begin();
                       ei != enum_map[type_num].end(); ++ei)
                    err << ' ' << ei->first;
                  err << '\n';

                  free(name);
                  ++errors;
                  continue;
                }
              s = end;
              val = ei->second;
            }

          free(name);

          while (*s && (*s == ' ' || *s == '\t'))
            ++s;
          if (*s)
            {
              err << "Unexpected `" << s
                  << "' while parsing atomic proposition `" << str
                  << "'.\n";
              ++errors;
              continue;
            }


          int v = dict->register_proposition(*ap, d);
          one_prop p = { var_num, op, val, v };
          out.emplace_back(p);
        }

      if (errors)
        throw std::runtime_error(err.str());
    }

    ////////////////////////////////////////////////////////////////////////
    // KRIPKE

    class spins_kripke final: public kripke
    {
    public:

      spins_kripke(spins_interface_ptr d, const bdd_dict_ptr& dict,
                   const spot::prop_set* ps, formula dead,
                   int compress)
        : kripke(dict),
          d_(d),
          state_size_(d_->get_state_size()),
          ps_(ps),
          compress_(compress == 0 ? nullptr
                    : compress == 1 ? int_array_array_compress
                    : int_array_array_compress2),
          decompress_(compress == 0 ? nullptr
                      : compress == 1 ? int_array_array_decompress
                      : int_array_array_decompress2),
          uncompressed_(compress ? new int[state_size_ + 30] : nullptr),
          compressed_(compress ? new int[state_size_ * 2] : nullptr),
          statepool_(compress ?
                     (sizeof(spins_compressed_state)
                      - sizeof(spins_compressed_state::vars)) :
                     (sizeof(spins_state) - sizeof(spins_state::vars)
                      + (state_size_ * sizeof(int)))),
          state_condition_last_state_(nullptr),
          state_condition_last_cc_(nullptr)
      {
        vname_ = new const char*[state_size_];
        format_filter_ = new bool[state_size_];
        for (int i = 0; i < state_size_; ++i)
          {
            vname_[i] = d_->get_state_variable_name(i);
            // We don't want to print variables that can take a single
            // value (e.g. process with a single state) to shorten the
            // output.
            int type = d->get_state_variable_type(i);
            format_filter_[i] =
              (d->get_type_value_count(type) != 1);
          }

        // Register the "dead" proposition.  There are three cases to
        // consider:
        //  * If DEAD is "false", it means we are not interested in finite
        //    sequences of the system.
        //  * If DEAD is "true", we want to check finite sequences as well
        //    as infinite sequences, but do not need to distinguish them.
        //  * If DEAD is any other string, this is the name a property
        //    that should be true when looping on a dead state, and false
        //    otherwise.
        // We handle these three cases by setting ALIVE_PROP and DEAD_PROP
        // appropriately.  ALIVE_PROP is the bdd that should be ANDed
        // to all transitions leaving a live state, while DEAD_PROP should
        // be ANDed to all transitions leaving a dead state.
        if (dead.is_ff())
          {
            alive_prop = bddtrue;
            dead_prop = bddfalse;
          }
        else if (dead.is_tt())
          {
            alive_prop = bddtrue;
            dead_prop = bddtrue;
          }
        else
          {
            int var = dict->register_proposition(dead, d_);
            dead_prop = bdd_ithvar(var);
            alive_prop = bdd_nithvar(var);
          }
      }

      ~spins_kripke()
      {
        if (iter_cache_)
          {
            delete iter_cache_;
            iter_cache_ = nullptr;
          }
        delete[] format_filter_;
        delete[] vname_;
        if (compress_)
          {
            delete[] uncompressed_;
            delete[] compressed_;
          }
        dict_->unregister_all_my_variables(d_.get());

        delete ps_;

        if (state_condition_last_state_)
          state_condition_last_state_->destroy();
        delete state_condition_last_cc_; // Might be 0 already.
      }

      virtual state* get_init_state() const override
      {
        if (compress_)
          {
            d_->get_initial_state(uncompressed_);
            size_t csize = state_size_ * 2;
            compress_(uncompressed_, state_size_, compressed_, csize);

            multiple_size_pool* p =
              const_cast<multiple_size_pool*>(&compstatepool_);
            void* mem = p->allocate(sizeof(spins_compressed_state)
                                    - sizeof(spins_compressed_state::vars)
                                    + sizeof(int) * csize);
            spins_compressed_state* res = new(mem)
              spins_compressed_state(csize, p);
            SPOT_ASSUME(res != nullptr);
            memcpy(res->vars, compressed_, csize * sizeof(int));
            res->compute_hash();
            return res;
          }
        else
          {
            fixed_size_pool* p = const_cast<fixed_size_pool*>(&statepool_);
            spins_state* res = new(p->allocate()) spins_state(state_size_, p);
            SPOT_ASSUME(res != nullptr);
            d_->get_initial_state(res->vars);
            res->compute_hash();
            return res;
          }
      }

      bdd
      compute_state_condition_aux(const int* vars) const
      {
        bdd res = bddtrue;
        for (auto& i: *ps_)
          {
            int l = vars[i.var_num];
            int r = i.val;

            bool cond = false;
            switch (i.op)
              {
              case OP_EQ:
                cond = (l == r);
                break;
              case OP_NE:
                cond = (l != r);
                break;
              case OP_LT:
                cond = (l < r);
                break;
              case OP_GT:
                cond = (l > r);
                break;
              case OP_LE:
                cond = (l <= r);
                break;
              case OP_GE:
                cond = (l >= r);
                break;
              }

            if (cond)
              res &= bdd_ithvar(i.bddvar);
            else
              res &= bdd_nithvar(i.bddvar);
          }
        return res;
      }

      callback_context* build_cc(const int* vars, int& t) const
      {
        callback_context* cc = new callback_context;
        cc->state_size = state_size_;
        cc->pool =
          const_cast<void*>(compress_
                            ? static_cast<const void*>(&compstatepool_)
                            : static_cast<const void*>(&statepool_));
        cc->compress = compress_;
        cc->compressed = compressed_;
        t = d_->get_successors(nullptr, const_cast<int*>(vars),
                               compress_
                               ? transition_callback_compress
                               : transition_callback,
                               cc);
        assert((unsigned)t == cc->transitions.size());
        return cc;
      }

      bdd
      compute_state_condition(const state* st) const
      {
        // If we just computed it, don't do it twice.
        if (st == state_condition_last_state_)
          return state_condition_last_cond_;

        if (state_condition_last_state_)
          {
            state_condition_last_state_->destroy();
            delete state_condition_last_cc_; // Might be 0 already.
            state_condition_last_cc_ = nullptr;
          }

        const int* vars = get_vars(st);

        bdd res = compute_state_condition_aux(vars);
        int t;
        callback_context* cc = build_cc(vars, t);

        if (t)
          {
            res &= alive_prop;
          }
        else
          {
            res &= dead_prop;

            // Add a self-loop to dead-states if we care about these.
            if (res != bddfalse)
              cc->transitions.emplace_back(st->clone());
          }

        state_condition_last_cc_ = cc;
        state_condition_last_cond_ = res;
        state_condition_last_state_ = st->clone();

        return res;
      }

      const int*
      get_vars(const state* st) const
      {
        const int* vars;
        if (compress_)
          {
            const spins_compressed_state* s =
              down_cast<const spins_compressed_state*>(st);

            decompress_(s->vars, s->size, uncompressed_, state_size_);
            vars = uncompressed_;
          }
        else
          {
            const spins_state* s = down_cast<const spins_state*>(st);
            vars = s->vars;
          }
        return vars;
      }


      virtual
      spins_succ_iterator* succ_iter(const state* st) const override
      {
        // This may also compute successors in state_condition_last_cc
        bdd scond = compute_state_condition(st);

        callback_context* cc;
        if (state_condition_last_cc_)
          {
            cc = state_condition_last_cc_;
            state_condition_last_cc_ = nullptr; // Now owned by the iterator.
          }
        else
          {
            int t;
            cc = build_cc(get_vars(st), t);

            // Add a self-loop to dead-states if we care about these.
            if (t == 0 && scond != bddfalse)
              cc->transitions.emplace_back(st->clone());
          }

        if (iter_cache_)
          {
            spins_succ_iterator* it =
              down_cast<spins_succ_iterator*>(iter_cache_);
            it->recycle(cc, scond);
            iter_cache_ = nullptr;
            return it;
          }
        return new spins_succ_iterator(cc, scond);
      }

      virtual
      bdd state_condition(const state* st) const override
      {
        return compute_state_condition(st);
      }

      virtual
      std::string format_state(const state *st) const override
      {
        const int* vars = get_vars(st);

        std::stringstream res;

        if (state_size_ == 0)
          return "empty state";

        int i = 0;
        for (;;)
          {
            if (!format_filter_[i])
              {
                ++i;
                if (i == state_size_)
                  break;
                continue;
              }
            res << vname_[i] << '=' << vars[i];
            ++i;
            if (i == state_size_)
              break;
            res << ", ";
          }
        return res.str();
      }

    private:
      spins_interface_ptr d_;
      int state_size_;
      const char** vname_;
      bool* format_filter_;
      const spot::prop_set* ps_;
      bdd alive_prop;
      bdd dead_prop;
      void (*compress_)(const int*, size_t, int*, size_t&);
      void (*decompress_)(const int*, size_t, int*, size_t);
      int* uncompressed_;
      int* compressed_;
      fixed_size_pool statepool_;
      multiple_size_pool compstatepool_;

      // This cache is used to speedup repeated calls to state_condition()
      // and get_succ().
      // If state_condition_last_state_ != 0, then state_condition_last_cond_
      // contain its (recently computed) condition.  If additionally
      // state_condition_last_cc_ != 0, then it contains the successors.
      mutable const state* state_condition_last_state_;
      mutable bdd state_condition_last_cond_;
      mutable callback_context* state_condition_last_cc_;
    };


    //////////////////////////////////////////////////////////////////////////
    // LOADER


    // Call spins to compile "foo.prom" as "foo.prom.spins" if the latter
    // does not exist already or is older.
    static void
    compile_model(std::string& filename, const std::string& ext)
    {
      std::string command;
      std::string compiled_ext;

      if (ext == ".gal")
        {
          command = "gal2c " + filename;
          compiled_ext = "2C";
        }
      else if (ext == ".prom" || ext == ".pm" || ext == ".pml")
        {
          command = "spins " + filename;
          compiled_ext = ".spins";
        }
      else if (ext == ".dve")
        {
          command = "divine compile --ltsmin " + filename;
          compiled_ext = "2C";
        }
      else
        {
          throw std::runtime_error("Unknown extension '"s + ext +
                                   "'.  Use '.prom', '.pm', '.pml', "
                                   "'.dve', '.dve2C', '.gal', '.gal2C' or "
                                   "'.prom.spins'.");
        }

      struct stat s;
      if (stat(filename.c_str(), &s) != 0)
        throw std::runtime_error("Cannot open "s + filename);

      filename += compiled_ext;

      // Remove any directory, because the new file will
      // be compiled in the current directory.
      size_t pos = filename.find_last_of("/\\");
      if (pos != std::string::npos)
        filename = "./" + filename.substr(pos + 1);

      struct stat d;
      if (stat(filename.c_str(), &d) == 0)
        if (s.st_mtime < d.st_mtime)
          // The .spins or .dve2C or .gal2C is up-to-date, no need to compile.
          return;

      int res = system(command.c_str());
      if (res)
        throw std::runtime_error("Execution of '"s
                                 + command.c_str() + "' returned exit code "
                                 + std::to_string(WEXITSTATUS(res)));
    }

  }

  ltsmin_model
  ltsmin_model::load(const std::string& file_arg)
  {
    std::string file;
    if (file_arg.find_first_of("/\\") != std::string::npos)
      file = file_arg;
    else
      file = "./" + file_arg;

    std::string ext = file.substr(file.find_last_of("."));
    if (ext != ".spins" && ext != ".dve2C" && ext != ".gal2C")
      {
        compile_model(file, ext);
        ext = file.substr(file.find_last_of("."));
      }

    if (lt_dlinit())
      throw std::runtime_error("Failed to initialize libltldl.");

    lt_dlhandle h = lt_dlopen(file.c_str());
    if (!h)
      {
        std::string lt_error = lt_dlerror();
        lt_dlexit();
        throw std::runtime_error("Failed to load '"s
                                 + file + "'.\n" + lt_error);
      }

    auto d = std::make_shared<spins_interface>();
    assert(d); // Superfluous, but Debian's GCC 7 snapshot 20161207-1 warns
               // about potential null pointer dereference on the next line.
    d->handle = h;


    auto sym = [&](auto* dst, const char* name)
      {
        // Work around -Wpendantic complaining that pointer-to-objects
        // should not be converted to pointer-to-functions (we have to
        // assume they can for POSIX).
        *reinterpret_cast<void**>(dst) = lt_dlsym(h, name);
        if (*dst == nullptr)
          throw std::runtime_error("Failed to resolve symbol '"s
                                   + name + "' in '" + file + "'.");
      };

    // SpinS interface.
    if (ext == ".spins")
      {
        sym(&d->get_initial_state, "spins_get_initial_state");
        d->have_property = nullptr;
        sym(&d->get_successors, "spins_get_successor_all");
        sym(&d->get_state_size, "spins_get_state_size");
        sym(&d->get_state_variable_name, "spins_get_state_variable_name");
        sym(&d->get_state_variable_type, "spins_get_state_variable_type");
        sym(&d->get_type_count, "spins_get_type_count");
        sym(&d->get_type_name, "spins_get_type_name");
        sym(&d->get_type_value_count, "spins_get_type_value_count");
        sym(&d->get_type_value_name, "spins_get_type_value_name");
      }
    // dve2 and gal2C interfaces.
    else
      {
        sym(&d->get_initial_state, "get_initial_state");
        *reinterpret_cast<void**>(&d->have_property) =
          lt_dlsym(h, "have_property");
        sym(&d->get_successors, "get_successors");
        sym(&d->get_state_size, "get_state_variable_count");
        sym(&d->get_state_variable_name, "get_state_variable_name");
        sym(&d->get_state_variable_type, "get_state_variable_type");
        sym(&d->get_type_count, "get_state_variable_type_count");
        sym(&d->get_type_name, "get_state_variable_type_name");
        sym(&d->get_type_value_count, "get_state_variable_type_value_count");
        sym(&d->get_type_value_name, "get_state_variable_type_value");
      }

    if (d->have_property && d->have_property())
      throw std::runtime_error("Models with embedded properties "
                               "are not supported.");

    return { d };
  }


  kripke_ptr
  ltsmin_model::kripke(const atomic_prop_set* to_observe,
                       bdd_dict_ptr dict,
                       const formula dead, int compress) const
  {
    spot::prop_set* ps = new spot::prop_set;
    try
      {
        convert_aps(to_observe, iface, dict, dead, *ps);
      }
    catch (const std::runtime_error&)
      {
        delete ps;
        dict->unregister_all_my_variables(iface.get());
        throw;
      }
    auto res = SPOT_make_shared_enabled__(spins_kripke,
                                          iface, dict, ps, dead, compress);
    // All atomic propositions have been registered to the bdd_dict
    // for iface, but we also need to add them to the automaton so
    // twa::ap() works.
    for (auto ap: *to_observe)
      res->register_ap(ap);
    if (dead.is(op::ap))
      res->register_ap(dead);
    return res;
  }

  ltsmin_model::~ltsmin_model()
  {
  }


  int ltsmin_model::state_size() const
  {
    return iface->get_state_size();
  }

  const char* ltsmin_model::state_variable_name(int var) const
  {
    return iface->get_state_variable_name(var);
  }

  int ltsmin_model::state_variable_type(int var) const
  {
    return iface->get_state_variable_type(var);
  }

  int ltsmin_model::type_count() const
  {
    return iface->get_type_count();
  }

  const char* ltsmin_model::type_name(int type) const
  {
    return iface->get_type_name(type);
  }

  int ltsmin_model::type_value_count(int type)
  {
    return iface->get_type_value_count(type);
  }

  const char* ltsmin_model::type_value_name(int type, int val)
  {
    return iface->get_type_value_name(type, val);
  }

}
