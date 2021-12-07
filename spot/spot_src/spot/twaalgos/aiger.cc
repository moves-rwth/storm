// -*- coding: utf-8 -*-
// Copyright (C) 2017-2019 Laboratoire de Recherche et Développement
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
#include <spot/twaalgos/aiger.hh>

#include <cmath>
#include <deque>
#include <map>
#include <unordered_map>
#include <vector>

#include <spot/twa/twagraph.hh>
#include <spot/misc/bddlt.hh>
#include <spot/misc/minato.hh>

namespace spot
{
  namespace
  {
    static std::vector<std::string>
    name_vector(unsigned n, const std::string& prefix)
    {
      std::vector<std::string> res(n);
      for (unsigned i = 0; i != n; ++i)
        res[i] = prefix + std::to_string(i);
      return res;
    }

    // A class to represent an AIGER circuit
    class aig
    {
    private:
      unsigned num_inputs_;
      unsigned max_var_;
      std::map<unsigned, std::pair<unsigned, unsigned>> and_gates_;
      std::vector<unsigned> latches_;
      std::vector<unsigned> outputs_;
      std::vector<std::string> input_names_;
      std::vector<std::string> output_names_;
      // Cache the function computed by each variable as a bdd.
      std::unordered_map<unsigned, bdd> var2bdd_;
      std::unordered_map<bdd, unsigned, bdd_hash> bdd2var_;

    public:
      aig(const std::vector<std::string>& inputs,
          const std::vector<std::string>& outputs,
          unsigned num_latches)
        : num_inputs_(inputs.size()),
          max_var_((inputs.size() + num_latches)*2),
          latches_(num_latches),
          outputs_(outputs.size()),
          input_names_(inputs),
          output_names_(outputs)
      {
        bdd2var_[bddtrue] = 1;
        var2bdd_[1] = bddtrue;
        bdd2var_[bddfalse] = 0;
        var2bdd_[0] = bddfalse;
      }

      aig(unsigned num_inputs, unsigned num_latches, unsigned num_outputs)
        : aig(name_vector(num_inputs, "in"), name_vector(num_outputs, "out"),
              num_latches)
      {}

      unsigned input_var(unsigned i, bdd b)
      {
        assert(i < num_inputs_);
        unsigned v = (1 + i) * 2;
        bdd2var_[b] = v;
        var2bdd_[v] = b;
        return v;
      }

      unsigned latch_var(unsigned i, bdd b)
      {
        assert(i < latches_.size());
        unsigned v = (1 + num_inputs_ + i) * 2;
        bdd2var_[b] = v;
        var2bdd_[v] = b;
        return v;
      }

      void set_output(unsigned i, unsigned v)
      {
        outputs_[i] = v;
      }

      void set_latch(unsigned i, unsigned v)
      {
        latches_[i] = v;
      }

      unsigned aig_true() const
      {
        return 1;
      }

      unsigned aig_false() const
      {
        return 0;
      }

      unsigned aig_not(unsigned v)
      {
        unsigned not_v = v ^ 1;
        assert(var2bdd_.count(v));
        var2bdd_[not_v] = !(var2bdd_[v]);
        bdd2var_[var2bdd_[not_v]] = not_v;
        return not_v;
      }

      unsigned aig_and(unsigned v1, unsigned v2)
      {
        assert(var2bdd_.count(v1));
        assert(var2bdd_.count(v2));
        bdd b = var2bdd_[v1] & var2bdd_[v2];
        auto it = bdd2var_.find(b);
        if (it != bdd2var_.end())
          return it->second;
        max_var_ += 2;
        and_gates_[max_var_] = {v1, v2};
        bdd2var_[b] = max_var_;
        var2bdd_[max_var_] = b;
        return max_var_;
      }

      unsigned aig_and(std::vector<unsigned> vs)
      {
        if (vs.empty())
          return aig_true();
        if (vs.size() == 1)
          return vs[0];
        if (vs.size() == 2)
          return aig_and(vs[0], vs[1]);
        unsigned m = vs.size() / 2;
        unsigned left =
          aig_and(std::vector<unsigned>(vs.begin(), vs.begin() + m));
        unsigned right =
          aig_and(std::vector<unsigned>(vs.begin() + m, vs.end()));
        return aig_and(left, right);
      }

      unsigned aig_or(unsigned v1, unsigned v2)
      {
        unsigned n1 = aig_not(v1);
        unsigned n2 = aig_not(v2);
        return aig_not(aig_and(n1, n2));
      }

      unsigned aig_or(std::vector<unsigned> vs)
      {
        for (unsigned i = 0; i < vs.size(); ++i)
          vs[i] = aig_not(vs[i]);
        return aig_not(aig_and(vs));
      }

      unsigned aig_pos(unsigned v)
      {

        return v & ~1;
      }

      void remove_unused()
      {
        // Run a DFS on the gates and latches to collect
        // all nodes connected to the output.
        std::deque<unsigned> todo;
        std::vector<bool> used(max_var_ / 2 + 1, false);
        auto mark = [&](unsigned v)
                    {
                      unsigned pos = aig_pos(v);
                      if (!used[pos/2])
                        {
                          used[pos/2] = 1;
                          todo.push_back(pos);
                        }
                    };
        for (unsigned v : outputs_)
          mark(v);
        while (!todo.empty())
          {
            unsigned v = todo.front();
            todo.pop_front();
            auto it_and = and_gates_.find(v);
            if (it_and != and_gates_.end())
              {
                mark(it_and->second.first);
                mark(it_and->second.second);
              }
            else if (v <= (num_inputs_ + latches_.size()) * 2
                     && v > num_inputs_ * 2)
              {
                mark(latches_[v / 2 - num_inputs_ - 1]);
              }
          }
        // Erase and_gates that were not seen in the above
        // exploration.
        auto e = and_gates_.end();
        for (auto i = and_gates_.begin(); i != e;)
          {
            if (!used[i->first/2])
              i = and_gates_.erase(i);
            else
              ++i;
          }
      }

      void
      print(std::ostream& os) const
      {
        os << "aag " << max_var_ / 2
           << ' ' << num_inputs_
           << ' ' << latches_.size()
           << ' ' << outputs_.size()
           << ' ' << and_gates_.size() << '\n';
        for (unsigned i = 0; i < num_inputs_; ++i)
          os << (1 + i) * 2 << '\n';
        for (unsigned i = 0; i < latches_.size(); ++i)
          os << (1 + num_inputs_ + i) * 2 << ' ' << latches_[i] << '\n';
        for (unsigned i = 0; i < outputs_.size(); ++i)
          os << outputs_[i] << '\n';
        for (auto& p : and_gates_)
          os << p.first
             << ' ' << p.second.first
             << ' ' << p.second.second << '\n';
        for (unsigned i = 0; i < num_inputs_; ++i)
          os << 'i' << i << ' ' << input_names_[i] << '\n';
        for (unsigned i = 0; i < outputs_.size(); ++i)
          os << 'o' << i << ' ' << output_names_[i] << '\n';
      }

    };

    static std::vector<bool>
    state_to_vec(unsigned s, unsigned size)
    {
      std::vector<bool> v(size);
      for (unsigned i = 0; i < size; ++i)
        {
          v[i] = s & 1;
          s >>= 1;
        }
      return v;
    }

    static std::vector<bool>
    output_to_vec(bdd b,
        const std::unordered_map<unsigned, unsigned>& bddvar_to_outputnum)
    {
      std::vector<bool> v(bddvar_to_outputnum.size());
      while (b != bddtrue && b != bddfalse)
        {
           unsigned i = bddvar_to_outputnum.at(bdd_var(b));
           v[i] = (bdd_low(b) == bddfalse);
           if (v[i])
             b = bdd_high(b);
           else
             b = bdd_low(b);
        }
      return v;
    }

    static bdd
    state_to_bdd(unsigned s, bdd all)
    {
      bdd b = bddtrue;
      unsigned size = bdd_nodecount(all);
      if (size)
        {
          unsigned st0 = bdd_var(all);
          for (unsigned i = 0; i < size; ++i)
            {
              b &= (s & 1)  ? bdd_ithvar(st0 + i) : bdd_nithvar(st0 + i);
              s >>= 1;
            }
        }
      return b;
    }

    // Switch initial state and 0 in the AIGER encoding, so that the
    // 0-initialized latches correspond to the initial state.
    static unsigned
    encode_init_0(unsigned src, unsigned init)
    {
      return src == init ? 0 : src == 0 ? init : src;
    }

    // Transforms an automaton into an AIGER circuit
    static aig
    aut_to_aiger(const const_twa_graph_ptr& aut, const bdd& all_outputs)
    {
      // The aiger circuit cannot encode the acceptance condition
      // Test that the acceptance condition is true
      if (!aut->acc().is_t())
        throw std::runtime_error("Cannot turn automaton into aiger circuit");

      // Encode state in log2(num_states) latches.
      // TODO how hard is it to compute the binary log of a binary integer??
      unsigned log2n = std::ceil(std::log2(aut->num_states()));
      unsigned st0 = aut->get_dict()->register_anonymous_variables(log2n, aut);
      bdd all_states = bddtrue;
      for (unsigned i = 0; i < log2n; ++i)
        all_states &= bdd_ithvar(st0 + i);

      std::vector<std::string> input_names;
      std::vector<std::string> output_names;
      bdd all_inputs = bddtrue;
      std::unordered_map<unsigned, unsigned> bddvar_to_inputnum;
      std::unordered_map<unsigned, unsigned> bddvar_to_outputnum;
      for (const auto& ap : aut->ap())
        {
          int bddvar = aut->get_dict()->has_registered_proposition(ap, aut);
          assert(bddvar >= 0);
          bdd b = bdd_ithvar(bddvar);
          if (bdd_implies(all_outputs, b)) // ap is an output AP
            {
              bddvar_to_outputnum[bddvar] = output_names.size();
              output_names.emplace_back(ap.ap_name());
            }
          else // ap is an input AP
            {
              bddvar_to_inputnum[bddvar] = input_names.size();
              input_names.emplace_back(ap.ap_name());
              all_inputs &= b;
            }
        }

      unsigned num_outputs = bdd_nodecount(all_outputs);
      unsigned num_latches = bdd_nodecount(all_states);
      unsigned init = aut->get_init_state_number();

      aig circuit(input_names, output_names, num_latches);
      bdd b;

      // Latches and outputs are expressed as a DNF in which each term
      // represents a transition.
      // latch[i] (resp. out[i]) represents the i-th latch (resp. output) DNF.
      std::vector<std::vector<unsigned>> latch(num_latches);
      std::vector<std::vector<unsigned>> out(num_outputs);
      for (unsigned s = 0; s < aut->num_states(); ++s)
        for (auto& e: aut->out(s))
          {
            minato_isop cond(e.cond);
            while ((b = cond.next()) != bddfalse)
              {
                bdd input = bdd_existcomp(b, all_inputs);
                bdd letter_out = bdd_existcomp(b, all_outputs);
                auto out_vec = output_to_vec(letter_out, bddvar_to_outputnum);
                unsigned dst = encode_init_0(e.dst, init);
                auto next_state_vec = state_to_vec(dst, log2n);
                unsigned src = encode_init_0(s, init);
                bdd state_bdd = state_to_bdd(src, all_states);
                std::vector<unsigned> prod;
                while (input != bddfalse && input != bddtrue)
                  {
                    unsigned v =
                      circuit.input_var(bddvar_to_inputnum[bdd_var(input)],
                                        bdd_ithvar(bdd_var(input)));
                    if (bdd_high(input) == bddfalse)
                      {
                        v = circuit.aig_not(v);
                        input = bdd_low(input);
                      }
                    else
                      input = bdd_high(input);
                    prod.push_back(v);
                  }

                while (state_bdd != bddfalse && state_bdd != bddtrue)
                  {
                    unsigned v =
                      circuit.latch_var(bdd_var(state_bdd) - st0,
                                        bdd_ithvar(bdd_var(state_bdd)));
                    if (bdd_high(state_bdd) == bddfalse)
                      {
                        v = circuit.aig_not(v);
                        state_bdd = bdd_low(state_bdd);
                      }
                    else
                        state_bdd = bdd_high(state_bdd);
                    prod.push_back(v);
                  }
                unsigned t = circuit.aig_and(prod);
                for (unsigned i = 0; i < next_state_vec.size(); ++i)
                  if (next_state_vec[i])
                    latch[i].push_back(t);
                for (unsigned i = 0; i < num_outputs; ++i)
                  if (out_vec[i])
                    out[i].push_back(t);
              }
          }
      for (unsigned i = 0; i < num_latches; ++i)
        circuit.set_latch(i, circuit.aig_or(latch[i]));
      for (unsigned i = 0; i < num_outputs; ++i)
        circuit.set_output(i, circuit.aig_or(out[i]));
      circuit.remove_unused();
      return circuit;
    }
  }

  std::ostream&
  print_aiger(std::ostream& os, const const_twa_ptr& aut)
  {
    auto a = down_cast<const_twa_graph_ptr>(aut);
    if (!a)
      throw std::runtime_error("aiger output is only for twa_graph");

    bdd* all_outputs = aut->get_named_prop<bdd>("synthesis-outputs");

    aig circuit = aut_to_aiger(a, all_outputs ? *all_outputs : bdd(bddfalse));
    circuit.print(os);
    return os;
  }
}
