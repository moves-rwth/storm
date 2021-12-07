// -*- coding: utf-8 -*-
// Copyright (C) 2018-2020 Laboratoire de Recherche et Développement
// de l'Epita.
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
#include <spot/twaalgos/toparity.hh>
#include <spot/twaalgos/sccinfo.hh>

#include <deque>
#include <utility>
#include <spot/twa/twa.hh>
#include <spot/twaalgos/cleanacc.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/toparity.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/parity.hh>
#include <spot/twaalgos/remfin.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/totgba.hh>
#include <unordered_map>

#include <unistd.h>
namespace spot
{

  // Old version of IAR.
  namespace
  {

    using perm_t = std::vector<unsigned>;
    struct iar_state
    {
      unsigned state;
      perm_t perm;

      bool
      operator<(const iar_state& other) const
      {
        return state == other.state ? perm < other.perm : state < other.state;
      }
    };

    template<bool is_rabin>
    class iar_generator
    {
      // helper functions: access fin and inf parts of the pairs
      // these functions negate the Streett condition to see it as a Rabin one
      const acc_cond::mark_t&
      fin(unsigned k) const
      {
        if (is_rabin)
          return pairs_[k].fin;
        else
          return pairs_[k].inf;
      }
      acc_cond::mark_t
      inf(unsigned k) const
      {
        if (is_rabin)
          return pairs_[k].inf;
        else
          return pairs_[k].fin;
      }
    public:
      explicit iar_generator(const const_twa_graph_ptr& a,
                             const std::vector<acc_cond::rs_pair>& p,
                             const bool pretty_print)
      : aut_(a)
      , pairs_(p)
      , scc_(scc_info(a))
      , pretty_print_(pretty_print)
      , state2pos_iar_states(aut_->num_states(), -1U)
      {}

      twa_graph_ptr
      run()
      {
        res_ = make_twa_graph(aut_->get_dict());
        res_->copy_ap_of(aut_);

        build_iar_scc(scc_.initial());

        {
          // resulting automaton has acceptance condition: parity max odd
          // for Rabin-like input and parity max even for Streett-like input.
          // with priorities ranging from 0 to 2*(nb pairs)
          // /!\ priorities are shifted by -1 compared to the original paper
          unsigned sets = 2 * pairs_.size() + 1;
          res_->set_acceptance(sets, acc_cond::acc_code::parity_max(is_rabin,
                                                                    sets));
        }
        {
          unsigned s = iar2num.at(state2iar.at(aut_->get_init_state_number()));
          res_->set_init_state(s);
        }

        // there could be quite a number of unreachable states, prune them
        res_->purge_unreachable_states();

        if (pretty_print_)
        {
          unsigned nstates = res_->num_states();
          auto names = new std::vector<std::string>(nstates);
          for (auto e : res_->edges())
          {
            unsigned s = e.src;
            iar_state iar = num2iar.at(s);
            std::ostringstream st;
            st << iar.state << ' ';
            if (iar.perm.empty())
              st << '[';
            char sep = '[';
            for (unsigned h: iar.perm)
            {
              st << sep << h;
              sep = ',';
            }
            st << ']';
            (*names)[s] = st.str();
          }
          res_->set_named_prop("state-names", names);
        }

        return res_;
      }

      void
      build_iar_scc(unsigned scc_num)
      {
        // we are working on an SCC: the state we start from does not matter
        unsigned init = scc_.one_state_of(scc_num);

        std::deque<iar_state> todo;
        auto get_state = [&](const iar_state& s)
          {
            auto it = iar2num.find(s);
            if (it == iar2num.end())
              {
                unsigned nb = res_->new_state();
                iar2num[s] = nb;
                num2iar[nb] = s;
                unsigned iar_pos = iar_states.size();
                unsigned old_newest_pos = state2pos_iar_states[s.state];
                state2pos_iar_states[s.state] = iar_pos;
                iar_states.push_back({s, old_newest_pos});
                todo.push_back(s);
                return nb;
              }
            return it->second;
          };

        auto get_other_scc = [this](unsigned state)
          {
            auto it = state2iar.find(state);
            // recursively build the destination SCC if we detect that it has
            // not been already built.
            if (it == state2iar.end())
              build_iar_scc(scc_.scc_of(state));
            return iar2num.at(state2iar.at(state));
          };

        if (scc_.is_trivial(scc_num))
          {
            iar_state iar_s{init, perm_t()};
            state2iar[init] = iar_s;
            unsigned src_num = get_state(iar_s);
            // Do not forget to connect to subsequent SCCs
            for (const auto& e : aut_->out(init))
              res_->new_edge(src_num, get_other_scc(e.dst), e.cond);
            return;
          }

        // determine the pairs that appear in the SCC
        auto colors = scc_.acc_sets_of(scc_num);
        std::set<unsigned> scc_pairs;
        for (unsigned k = 0; k != pairs_.size(); ++k)
          if (!inf(k) || (colors & (pairs_[k].fin | pairs_[k].inf)))
            scc_pairs.insert(k);

        perm_t p0;
        for (unsigned k : scc_pairs)
          p0.push_back(k);

        iar_state s0{init, p0};
        get_state(s0); // put s0 in todo

        // the main loop
        while (!todo.empty())
          {
            iar_state current = todo.front();
            todo.pop_front();

            unsigned src_num = get_state(current);

            for (const auto& e : aut_->out(current.state))
              {
                // connect to the appropriate state
                if (scc_.scc_of(e.dst) != scc_num)
                  res_->new_edge(src_num, get_other_scc(e.dst), e.cond);
                else
                  {
                    // find the new permutation
                    perm_t new_perm = current.perm;
                    // Count pairs whose fin-part is seen on this transition
                    unsigned seen_nb = 0;
                    // consider the pairs for this SCC only
                    for (unsigned k : scc_pairs)
                      if (e.acc & fin(k))
                        {
                          ++seen_nb;
                          auto it = std::find(new_perm.begin(),
                                              new_perm.end(),
                                              k);
                          // move the pair in front of the permutation
                          std::rotate(new_perm.begin(), it, it+1);
                        }

                    iar_state dst;
                    unsigned dst_num = -1U;

                    // Optimization: when several indices are seen in the
                    // transition, they move at the front of new_perm in any
                    // order. Check whether there already exists an iar_state
                    // that matches this condition.

                    auto iar_pos = state2pos_iar_states[e.dst];
                    while (iar_pos != -1U)
                    {
                      iar_state& tmp = iar_states[iar_pos].first;
                      iar_pos = iar_states[iar_pos].second;
                      if (std::equal(new_perm.begin() + seen_nb,
                        new_perm.end(),
                        tmp.perm.begin() + seen_nb))
                      {
                        dst = tmp;
                        dst_num = iar2num[dst];
                        break;
                      }
                    }
                    // if such a state was not found, build it
                    if (dst_num == -1U)
                      {
                        dst = iar_state{e.dst, new_perm};
                        dst_num = get_state(dst);
                      }

                    // find the maximal index encountered by this transition
                    unsigned maxint = -1U;
                    for (int k = current.perm.size() - 1; k >= 0; --k)
                      {
                        unsigned pk = current.perm[k];
                        if (!inf(pk) ||
                            (e.acc & (pairs_[pk].fin | pairs_[pk].inf))) {
                              maxint = k;
                              break;
                            }
                      }

                    acc_cond::mark_t acc = {};
                    if (maxint == -1U)
                      acc = {0};
                    else if (e.acc & fin(current.perm[maxint]))
                      acc = {2*maxint+2};
                    else
                      acc = {2*maxint+1};

                    res_->new_edge(src_num, dst_num, e.cond, acc);
                  }
              }
          }

        // Optimization: find the bottom SCC of the sub-automaton we have just
        // built. To that end, we have to ignore edges going out of scc_num.
        auto leaving_edge = [&](unsigned d)
          {
            return scc_.scc_of(num2iar.at(d).state) != scc_num;
          };
        auto filter_edge = [](const twa_graph::edge_storage_t&,
                              unsigned dst,
                              void* filter_data)
          {
            decltype(leaving_edge)* data =
              static_cast<decltype(leaving_edge)*>(filter_data);

            if ((*data)(dst))
              return scc_info::edge_filter_choice::ignore;
            return scc_info::edge_filter_choice::keep;
          };
        scc_info sub_scc(res_, get_state(s0), filter_edge, &leaving_edge);
        // SCCs are numbered in reverse topological order, so the bottom SCC has
        // index 0.
        const unsigned bscc = 0;
        assert(sub_scc.succ(0).empty());
        assert(
            [&]()
            {
              for (unsigned s = 1; s != sub_scc.scc_count(); ++s)
                if (sub_scc.succ(s).empty())
                  return false;
              return true;
            } ());

        assert(sub_scc.states_of(bscc).size()
            >= scc_.states_of(scc_num).size());

        // update state2iar
        for (unsigned scc_state : sub_scc.states_of(bscc))
          {
            iar_state& iar = num2iar.at(scc_state);
            if (state2iar.find(iar.state) == state2iar.end())
              state2iar[iar.state] = iar;
          }
      }

    private:
      const const_twa_graph_ptr& aut_;
      const std::vector<acc_cond::rs_pair>& pairs_;
      const scc_info scc_;
      twa_graph_ptr res_;
      bool pretty_print_;

      // to be used when entering a new SCC
      // maps a state of aut_ onto an iar_state with the appropriate perm
      std::map<unsigned, iar_state> state2iar;

      std::map<iar_state, unsigned> iar2num;
      std::map<unsigned, iar_state> num2iar;

      std::vector<unsigned> state2pos_iar_states;
      std::vector<std::pair<iar_state, unsigned>> iar_states;
    };

    // Make this a function different from iar_maybe(), so that
    // iar() does not have to call a deprecated function.
    static twa_graph_ptr
    iar_maybe_(const const_twa_graph_ptr& aut, bool pretty_print)
    {
      std::vector<acc_cond::rs_pair> pairs;
      if (!aut->acc().is_rabin_like(pairs))
        if (!aut->acc().is_streett_like(pairs))
          return nullptr;
        else
          {
            iar_generator<false> gen(aut, pairs, pretty_print);
            return gen.run();
          }
      else
        {
          iar_generator<true> gen(aut, pairs, pretty_print);
          return gen.run();
        }
    }
  }

  twa_graph_ptr
  iar_maybe(const const_twa_graph_ptr& aut, bool pretty_print)
  {
    return iar_maybe_(aut, pretty_print);
  }

  twa_graph_ptr
  iar(const const_twa_graph_ptr& aut, bool pretty_print)
  {
    if (auto res = iar_maybe_(aut, pretty_print))
      return res;
    throw std::runtime_error("iar() expects Rabin-like or Streett-like input");
  }

// New version for paritizing
namespace
{
struct node
{
    // A color of the permutation or a state.
    unsigned label;
    std::vector<node*> children;
    // is_leaf is true if the label is a state of res_.
    bool is_leaf;

    node()
        : node(0, 0){
    }

    node(int label_, bool is_leaf_)
        : label(label_)
        , children(0)
        , is_leaf(is_leaf_){
    }

    ~node()
    {
        for (auto c : children)
            delete c;
    }

    // Add a permutation to the tree.
    void
    add_new_perm(const std::vector<unsigned>& permu, int pos, unsigned state)
    {
        if (pos == -1)
            children.push_back(new node(state, true));
        else
        {
            auto lab = permu[pos];
            auto child = std::find_if(children.begin(), children.end(),
                [lab](node* n){
                    return n->label == lab;
                });
            if (child == children.end())
            {
                node* new_child = new node(lab, false);
                children.push_back(new_child);
                new_child->add_new_perm(permu, pos - 1, state);
            }
            else
                (*child)->add_new_perm(permu, pos - 1, state);
        }
    }

    node*
    get_sub_tree(const std::vector<unsigned>& elements, int pos)
    {
        if (pos < 0)
            return this;
        unsigned lab = elements[pos];
        auto child = std::find_if(children.begin(), children.end(),
                [lab](node* n){
                    return n->label == lab;
                });
        assert(child != children.end());
        return (*child)->get_sub_tree(elements, pos - 1);
    }

    // Gives a state of res_ (if it exists) reachable from this node.
    // If use_last is true, we take the most recent, otherwise we take
    // the oldest.
    unsigned
    get_end(bool use_last)
    {
        if (children.empty())
        {
            if (!is_leaf)
                return -1U;
            return label;
        }
        if (use_last)
            return children[children.size() - 1]->get_end(use_last);
        return children[0]->get_end(use_last);
    }

    // Try to find a state compatible with the permu when seen_nb colors are
    // moved.
    unsigned
    get_existing(const std::vector<unsigned>& permu, unsigned seen_nb, int pos,
                 bool use_last)
    {
        if (pos < (int) seen_nb)
            return get_end(use_last);
        else
        {
            auto lab = permu[pos];
            auto child = std::find_if(children.begin(), children.end(),
                [lab](node* n){
                    return n->label == lab;
                });
            if (child == children.end())
                return -1U;
            return (*child)->get_existing(permu, seen_nb, pos - 1, use_last);
        }
    }
};

class state_2_car_scc
{
std::vector<node> nodes;

public:
state_2_car_scc(unsigned nb_states)
    : nodes(nb_states, node()){
}

// Try to find a state compatible with the permu when seen_nb colors are
// moved. If use_last is true, it return the last created compatible state.
// If it is false, it returns the oldest.
unsigned
get_res_state(unsigned state, const std::vector<unsigned>& permu,
              unsigned seen_nb, bool use_last)
{
    return nodes[state].get_existing(permu, seen_nb,
                                     permu.size() - 1, use_last);
}

void
add_res_state(unsigned initial, unsigned state,
              const std::vector<unsigned>& permu)
{
    nodes[initial].add_new_perm(permu, ((int) permu.size()) - 1, state);
}

node*
get_sub_tree(const std::vector<unsigned>& elements, unsigned state)
{
    return nodes[state].get_sub_tree(elements, elements.size() - 1);
}
};

class car_generator
{
enum algorithm {
    // Try to have a Büchi condition if we have Rabin.
    Rabin_to_Buchi,
    Streett_to_Buchi,
    // IAR
    IAR_Streett,
    IAR_Rabin,
    // CAR
    CAR,
    // Changing colors transforms acceptance to max even/odd copy.
    Copy_even,
    Copy_odd,
    // If a condition is "t" or "f", we just have to copy the automaton.
    False_clean,
    True_clean,
    None
};


static std::string
algorithm_to_str(algorithm algo)
{
    std::string algo_str;
    switch (algo)
    {
    case IAR_Streett:
        algo_str = "IAR (Streett)";
        break;
    case IAR_Rabin:
        algo_str = "IAR (Rabin)";
        break;
    case CAR:
        algo_str = "CAR";
        break;
    case Copy_even:
        algo_str = "Copy even";
        break;
    case Copy_odd:
        algo_str = "Copy odd";
        break;
    case False_clean:
        algo_str = "False clean";
        break;
    case True_clean:
        algo_str = "True clean";
        break;
    case Streett_to_Buchi:
        algo_str = "Streett to Büchi";
        break;
    case Rabin_to_Buchi:
        algo_str = "Rabin to Büchi";
        break;
    default:
        algo_str = "None";
        break;
    }
    return algo_str;
}

using perm_t = std::vector<unsigned>;

struct car_state
{
    // State of the original automaton
    unsigned state;
    // We create a new automaton for each SCC of the original automaton
    // so we keep a link between a car_state and the state of the
    // subautomaton.
    unsigned state_scc;
    // Permutation used by IAR and CAR.
    perm_t perm;

    bool
    operator<(const car_state &other) const
    {
        if (state < other.state)
            return true;
        if (state > other.state)
            return false;
        if (perm < other.perm)
            return true;
        if (perm > other.perm)
            return false;
        return state_scc < other.state_scc;
    }

    std::string
    to_string(algorithm algo) const
    {
        std::stringstream s;
        s << state;
        unsigned ps = perm.size();
        if (ps > 0)
        {
            s << " [";
            for (unsigned i = 0; i != ps; ++i)
            {
                if (i > 0)
                    s << ',';
                s << perm[i];
            }
            s << ']';
        }
        s << ", ";
        s << algorithm_to_str(algo);
        return s.str();
    }
};

const acc_cond::mark_t &
fin(const std::vector<acc_cond::rs_pair>& pairs, unsigned k, algorithm algo)
const
{
    if (algo == IAR_Rabin)
        return pairs[k].fin;
    else
        return pairs[k].inf;
}

acc_cond::mark_t
inf(const std::vector<acc_cond::rs_pair>& pairs, unsigned k, algorithm algo)
const
{
    if (algo == IAR_Rabin)
        return pairs[k].inf;
    else
        return pairs[k].fin;
}

// Gives for each state a set of marks incoming to this state.
std::vector<std::set<acc_cond::mark_t>>
get_inputs_states(const twa_graph_ptr& aut)
{
    auto used = aut->acc().get_acceptance().used_sets();
    std::vector<std::set<acc_cond::mark_t>> inputs(aut->num_states());
    for (auto e : aut->edges())
    {
        auto elements = e.acc & used;
        if (elements.has_many())
            inputs[e.dst].insert(elements);
    }
    return inputs;
}

// Gives for each state a set of pairs incoming to this state.
std::vector<std::set<std::vector<unsigned>>>
get_inputs_iar(const twa_graph_ptr& aut, algorithm algo,
               const std::set<unsigned>& perm_elem,
               const std::vector<acc_cond::rs_pair>& pairs)
{
    std::vector<std::set<std::vector<unsigned>>> inputs(aut->num_states());
    for (auto e : aut->edges())
    {
        auto acc = e.acc;
        std::vector<unsigned> new_vect;
        for (unsigned k : perm_elem)
            if (acc & fin(pairs, k, algo))
                new_vect.push_back(k);
        std::sort(std::begin(new_vect), std::end(new_vect));
        inputs[e.dst].insert(new_vect);
    }
    return inputs;
}
// Give an order from the set of marks
std::vector<unsigned>
group_to_vector(const std::set<acc_cond::mark_t>& group)
{
    // In this function, we have for example the marks {1, 2}, {1, 2, 3}, {2}
    // A compatible order is [2, 1, 3]
    std::vector<acc_cond::mark_t> group_vect(group.begin(), group.end());

    // We sort the elements by inclusion. This function is called on a
    // set of marks such that each mark is included or includes the others.
    std::sort(group_vect.begin(), group_vect.end(),
              [](const acc_cond::mark_t left, const acc_cond::mark_t right)
                {
                    return (left != right) && ((left & right) == left);
                });
    // At this moment, we have the vector [{2}, {1, 2}, {1, 2, 3}].
    // In order to create the order, we add the elements of the first element.
    // Then we add the elements of the second mark (without duplication), etc.
    std::vector<unsigned> result;
    for (auto mark : group_vect)
    {
        for (unsigned col : mark.sets())
            if (std::find(result.begin(), result.end(), col) == result.end())
                result.push_back(col);
    }
    return result;
}

// Give an order from the set of indices of pairs
std::vector<unsigned>
group_to_vector_iar(const std::set<std::vector<unsigned>>& group)
{
    std::vector<std::vector<unsigned>> group_vect(group.begin(), group.end());
    for (auto& vec : group_vect)
        std::sort(std::begin(vec), std::end(vec));
    std::sort(group_vect.begin(), group_vect.end(),
              [](const std::vector<unsigned> left,
                 const std::vector<unsigned> right)
                {
                    return (right != left)
                        && std::includes(right.begin(), right.end(),
                                         left.begin(), left.end());
                });
    std::vector<unsigned> result;
    for (auto vec : group_vect)
        for (unsigned col : vec)
            if (std::find(result.begin(), result.end(), col) == result.end())
                result.push_back(col);
    return result;
}

// Give a correspondance between a mark and an order for CAR
std::map<acc_cond::mark_t, std::vector<unsigned>>
get_groups(const std::set<acc_cond::mark_t>& marks_input)
{
    std::map<acc_cond::mark_t, std::vector<unsigned>> result;

    std::vector<std::set<acc_cond::mark_t>> groups;
    for (acc_cond::mark_t mark : marks_input)
    {
        bool added = false;
        for (unsigned group = 0; group < groups.size(); ++group)
        {
            if (std::all_of(groups[group].begin(), groups[group].end(),
                            [mark](acc_cond::mark_t element)
                            {
                                return ((element | mark) == mark)
                                || ((element | mark) == element);
                            }))
            {
                groups[group].insert(mark);
                added = true;
                break;
            }
        }
        if (!added)
            groups.push_back({mark});
    }
    for (auto& group : groups)
    {
        auto new_vector = group_to_vector(group);
        for (auto mark : group)
            result.insert({mark, new_vector});
    }
    return result;
}

// Give a correspondance between a mark and an order for IAR
std::map<std::vector<unsigned>, std::vector<unsigned>>
get_groups_iar(const std::set<std::vector<unsigned>>& marks_input)
{
    std::map<std::vector<unsigned>, std::vector<unsigned>> result;

    std::vector<std::set<std::vector<unsigned>>> groups;
    for (auto vect : marks_input)
    {
        bool added = false;
        for (unsigned group = 0; group < groups.size(); ++group)
            if (std::all_of(groups[group].begin(), groups[group].end(),
                            [vect](std::vector<unsigned> element)
                            {
                                return std::includes(vect.begin(), vect.end(),
                                               element.begin(), element.end())
                              || std::includes(element.begin(), element.end(),
                                                     vect.begin(), vect.end());
                            }))
            {
                groups[group].insert(vect);
                added = true;
                break;
            }
        if (!added)
            groups.push_back({vect});
    }
    for (auto& group : groups)
    {
        auto new_vector = group_to_vector_iar(group);
        for (auto vect : group)
            result.insert({vect, new_vector});
    }
    return result;
}

// Give for each state the correspondance between a mark and an order (CAR)
std::vector<std::map<acc_cond::mark_t, std::vector<unsigned>>>
get_mark_to_vector(const twa_graph_ptr& aut)
{
    std::vector<std::map<acc_cond::mark_t, std::vector<unsigned>>> result;
    auto inputs = get_inputs_states(aut);
    for (unsigned state = 0; state < inputs.size(); ++state)
        result.push_back(get_groups(inputs[state]));
    return result;
}

// Give for each state the correspondance between a mark and an order (IAR)
std::vector<std::map<std::vector<unsigned>, std::vector<unsigned>>>
get_iar_to_vector(const twa_graph_ptr& aut, algorithm algo,
                  const std::set<unsigned>& perm_elem,
                  const std::vector<acc_cond::rs_pair>& pairs)
{
    std::vector<std::map<std::vector<unsigned>, std::vector<unsigned>>> result;
    auto inputs = get_inputs_iar(aut, algo, perm_elem, pairs);
    for (unsigned state = 0; state < inputs.size(); ++state)
        result.push_back(get_groups_iar(inputs[state]));
    return result;
}

public:
explicit car_generator(const const_twa_graph_ptr &a, to_parity_options options)
    : aut_(a)
    , scc_(scc_info(a))
    , is_odd(false)
    , options(options)
{
    if (options.pretty_print)
        names = new std::vector<std::string>();
    else
        names = nullptr;
}

// During the creation of the states, we had to choose between a set of
// compatible states. But it is possible to create another compatible state
// after. This function checks if a compatible state was created after and
// use it.
void
change_transitions_destination(twa_graph_ptr& aut,
const std::vector<unsigned>& states,
std::map<unsigned, std::vector<unsigned>>& partial_history,
state_2_car_scc& state_2_car)
{
    for (auto s : states)
        for (auto& edge : aut->out(s))
        {
            unsigned
                src = edge.src,
                dst = edge.dst;
            // We don't change loops
            if (src == dst)
                continue;
            unsigned dst_scc = num2car[dst].state_scc;
            auto cant_change = partial_history[aut->edge_number(edge)];
            edge.dst = state_2_car.get_sub_tree(cant_change, dst_scc)
                                    ->get_end(true);
        }
}

unsigned
apply_false_true_clean(const twa_graph_ptr &sub_automaton, bool is_true,
                       const std::vector<int>& inf_fin_prefix,
                       unsigned max_free_color,
                       std::map<unsigned, car_state>& state2car_local,
                       std::map<car_state, unsigned>& car2num_local)
{
    std::vector<unsigned>* init_states = sub_automaton->
        get_named_prop<std::vector<unsigned>>("original-states");

    for (unsigned state = 0; state < sub_automaton->num_states(); ++state)
    {
        unsigned s_aut = (*init_states)[state];

        car_state new_car = { s_aut, state, perm_t() };
        auto new_state = res_->new_state();
        car2num_local[new_car] = new_state;
        num2car.insert(num2car.begin() + new_state, new_car);
        if (options.pretty_print)
            names->push_back(
                new_car.to_string(is_true ? True_clean : False_clean));
        state2car_local[s_aut] = new_car;
    }
    for (unsigned state = 0; state < sub_automaton->num_states(); ++state)
    {
        unsigned s_aut = (*init_states)[state];
        car_state src = { s_aut, state, perm_t() };
        unsigned src_state = car2num_local[src];
        for (auto e : aut_->out(s_aut))
        {
            auto col = is_true ^ !is_odd;
            if (((unsigned)col) > max_free_color)
                throw std::runtime_error("CAR needs more sets");
            if (scc_.scc_of(s_aut) == scc_.scc_of(e.dst))
            {
                for (auto c : e.acc.sets())
                    if (inf_fin_prefix[c] + is_odd > col)
                        col = inf_fin_prefix[c] + is_odd;
                acc_cond::mark_t cond = { (unsigned) col };
                res_->new_edge(
                    src_state, car2num_local[state2car_local[e.dst]],
                    e.cond, cond);
            }
        }
    }
    return sub_automaton->num_states();
}

unsigned
apply_copy(const twa_graph_ptr &sub_automaton,
           const std::vector<unsigned> &permut,
           bool copy_odd,
           const std::vector<int>& inf_fin_prefix,
           std::map<unsigned, car_state>& state2car_local,
           std::map<car_state, unsigned>& car2num_local)
{
    std::vector<unsigned>* init_states = sub_automaton
        ->get_named_prop<std::vector<unsigned>>("original-states");
    for (unsigned state = 0; state < sub_automaton->num_states(); ++state)
    {
        car_state new_car = { (*init_states)[state], state, perm_t() };
        auto new_state = res_->new_state();
        car2num_local[new_car] = new_state;
        num2car.insert(num2car.begin() + new_state, new_car);
        state2car_local[(*init_states)[state]] = new_car;
        if (options.pretty_print)
            names->push_back(
                new_car.to_string(copy_odd ? Copy_odd : Copy_even));
    }
    auto cond_col = sub_automaton->acc().get_acceptance().used_sets();
    for (unsigned s = 0; s < sub_automaton->num_states(); ++s)
    {
        for (auto e : sub_automaton->out(s))
        {
            acc_cond::mark_t mark = { };
            int max_edge = -1;
            for (auto col : e.acc.sets())
            {
                if (cond_col.has(col))
                    max_edge = std::max(max_edge, (int) permut[col]);
                if (inf_fin_prefix[col] + (is_odd || copy_odd) > max_edge)
                    max_edge = inf_fin_prefix[col] + (is_odd || copy_odd);
            }
            if (max_edge != -1)
                mark.set((unsigned) max_edge);
            car_state src = { (*init_states)[s], s, perm_t() },
                      dst = { (*init_states)[e.dst], e.dst, perm_t() };
            unsigned src_state = car2num_local[src],
                     dst_state = car2num_local[dst];
            res_->new_edge(src_state, dst_state, e.cond, mark);
        }
    }
    return sub_automaton->num_states();
}

unsigned
apply_to_Buchi(const twa_graph_ptr& sub_automaton,
               const twa_graph_ptr& buchi,
               bool is_streett_to_buchi,
               const std::vector<int>& inf_fin_prefix,
               unsigned max_free_color,
               std::map<unsigned, car_state>& state2car_local,
               std::map<car_state, unsigned>& car2num_local)
{
    std::vector<unsigned>* init_states = sub_automaton
        ->get_named_prop<std::vector<unsigned>>("original-states");

    for (unsigned state = 0; state < buchi->num_states(); ++state)
    {
        car_state new_car = { (*init_states)[state], state, perm_t() };
        auto new_state = res_->new_state();
        car2num_local[new_car] = new_state;
        num2car.insert(num2car.begin() + new_state, new_car);
        state2car_local[(*init_states)[state]] = new_car;
        if (options.pretty_print)
            names->push_back(new_car.to_string(
                is_streett_to_buchi ? Streett_to_Buchi : Rabin_to_Buchi));
    }
    auto g = buchi->get_graph();
    for (unsigned s = 0; s < buchi->num_states(); ++s)
    {
        unsigned b = g.state_storage(s).succ;
        while (b)
        {
            auto& e = g.edge_storage(b);
            auto acc = e.acc;
            acc <<= (is_odd + is_streett_to_buchi);
            if ((is_odd || is_streett_to_buchi) && acc == acc_cond::mark_t{ })
                acc = { (unsigned) (is_streett_to_buchi && is_odd) };
            car_state src = { (*init_states)[s], s, perm_t() },
                      dst = { (*init_states)[e.dst], e.dst, perm_t() };
            unsigned src_state = car2num_local[src],
                     dst_state = car2num_local[dst];
            int col = ((int) acc.max_set()) - 1;
            if (col > (int) max_free_color)
                throw std::runtime_error("CAR needs more sets");
            auto& e2 = sub_automaton->get_graph().edge_storage(b);
            for (auto c : e2.acc.sets())
            {
                if (inf_fin_prefix[c] + is_odd > col)
                    col = inf_fin_prefix[c] + is_odd;
            }
            if (col != -1)
                acc = { (unsigned) col };
            else
                acc = {};
            res_->new_edge(src_state, dst_state, e.cond, acc);
            b = e.next_succ;
        }
    }
    return buchi->num_states();
}

// Create a permutation for the first state of a SCC (IAR)
void
initial_perm_iar(std::set<unsigned> &perm_elem, perm_t &p0,
                 algorithm algo, const acc_cond::mark_t &colors,
                 const std::vector<acc_cond::rs_pair> &pairs)
{
    for (unsigned k = 0; k != pairs.size(); ++k)
        if (!inf(pairs, k, algo) || (colors & (pairs[k].fin | pairs[k].inf)))
        {
            perm_elem.insert(k);
            p0.push_back(k);
        }
}

// Create a permutation for the first state of a SCC (CAR)
void
initial_perm_car(perm_t &p0, const acc_cond::mark_t &colors)
{
    auto cont = colors.sets();
    p0.assign(cont.begin(), cont.end());
}

void
find_new_perm_iar(perm_t &new_perm,
                  const std::vector<acc_cond::rs_pair> &pairs,
                  const acc_cond::mark_t &acc,
                  algorithm algo, const std::set<unsigned> &perm_elem,
                  unsigned &seen_nb)
{
    for (unsigned k : perm_elem)
        if (acc & fin(pairs, k, algo))
        {
            ++seen_nb;
            auto it = std::find(new_perm.begin(), new_perm.end(), k);

            // move the pair in front of the permutation
            std::rotate(new_perm.begin(), it, it + 1);
        }
}

// Given the set acc of colors appearing on an edge, create a new
// permutation new_perm, and give the number seen_nb of colors moved to
// the head of the permutation.
void
find_new_perm_car(perm_t &new_perm, const acc_cond::mark_t &acc,
                  unsigned &seen_nb, unsigned &h)
{
    for (unsigned k : acc.sets())
    {
        auto it = std::find(new_perm.begin(), new_perm.end(), k);
        if (it != new_perm.end())
        {
            h = std::max(h, unsigned(it - new_perm.begin()) + 1);
            std::rotate(new_perm.begin(), it, it + 1);
            ++seen_nb;
        }
    }
}

void
get_acceptance_iar(algorithm algo, const perm_t &current_perm,
                   const std::vector<acc_cond::rs_pair> &pairs,
                   const acc_cond::mark_t &e_acc, acc_cond::mark_t &acc)
{
    unsigned delta_acc = (algo == IAR_Streett) && is_odd;

    // find the maximal index encountered by this transition
    unsigned maxint = -1U;

    for (int k = current_perm.size() - 1; k >= 0; --k)
    {
        unsigned pk = current_perm[k];

        if (!inf(pairs, pk,
                 algo)
            || (e_acc & (pairs[pk].fin | pairs[pk].inf)))
        {
            maxint = k;
            break;
        }
    }
    unsigned value;

    if (maxint == -1U)
        value = delta_acc;
    else if (e_acc & fin(pairs, current_perm[maxint], algo))
        value = 2 * maxint + 2 + delta_acc;
    else
        value = 2 * maxint + 1 + delta_acc;
    acc = { value };
}

void
get_acceptance_car(const acc_cond &sub_aut_cond, const perm_t &new_perm,
                   unsigned h, acc_cond::mark_t &acc)
{
    acc_cond::mark_t m(new_perm.begin(), new_perm.begin() + h);
    bool rej = !sub_aut_cond.accepting(m);
    unsigned value = 2 * h + rej + is_odd;
    acc = { value };
}

unsigned
apply_lar(const twa_graph_ptr &sub_automaton,
          unsigned init, std::vector<acc_cond::rs_pair> &pairs,
          algorithm algo, unsigned scc_num,
          const std::vector<int>& inf_fin_prefix,
          unsigned max_free_color,
          std::map<unsigned, car_state>& state2car_local,
          std::map<car_state, unsigned>& car2num_local,
          unsigned max_states)
{
    auto maps = get_mark_to_vector(sub_automaton);
    // For each edge e of res_, we store the elements of the permutation
    // that are not moved, and we respect the order.
    std::map<unsigned, std::vector<unsigned>> edge_to_colors;
    unsigned nb_created_states = 0;
    auto state_2_car = state_2_car_scc(sub_automaton->num_states());
    std::vector<unsigned>* init_states = sub_automaton->
        get_named_prop<std::vector<unsigned>>("original-states");
    std::deque<car_state> todo;
    auto get_state =
        [&](const car_state &s){
            auto it = car2num_local.find(s);

            if (it == car2num_local.end())
            {
                ++nb_created_states;
                unsigned nb = res_->new_state();
                if (options.search_ex)
                    state_2_car.add_res_state(s.state_scc, nb, s.perm);
                car2num_local[s] = nb;
                num2car.insert(num2car.begin() + nb, s);

                todo.push_back(s);
                if (options.pretty_print)
                    names->push_back(s.to_string(algo));
                return nb;
            }
            return it->second;
        };

    auto colors = sub_automaton->acc().get_acceptance().used_sets();
    std::set<unsigned> perm_elem;

    perm_t p0 = { };
    switch (algo)
    {
    case IAR_Streett:
    case IAR_Rabin:
        initial_perm_iar(perm_elem, p0, algo, colors, pairs);
        break;
    case CAR:
        initial_perm_car(p0, colors);
        break;
    default:
        assert(false);
        break;
    }

    std::vector<std::map<std::vector<unsigned>, std::vector<unsigned>>>
    iar_maps;
    if (algo == IAR_Streett || algo == IAR_Rabin)
        iar_maps = get_iar_to_vector(sub_automaton, algo, perm_elem, pairs);

    car_state s0{ (*init_states)[init], init, p0 };
    get_state(s0);         // put s0 in todo

    // the main loop
    while (!todo.empty())
    {
        car_state current = todo.front();
        todo.pop_front();

        unsigned src_num = get_state(current);
        for (const auto &e : sub_automaton->out(current.state_scc))
        {
            perm_t new_perm = current.perm;

            // Count pairs whose fin-part is seen on this transition
            unsigned seen_nb = 0;

            // consider the pairs for this SCC only
            unsigned h = 0;

            switch (algo)
            {
            case IAR_Rabin:
            case IAR_Streett:
                find_new_perm_iar(new_perm, pairs, e.acc, algo,
                                  perm_elem, seen_nb);
                break;
            case CAR:
                find_new_perm_car(new_perm, e.acc, seen_nb, h);
                break;
            default:
                assert(false);
            }

            std::vector<unsigned> not_moved(new_perm.begin() + seen_nb,
                                            new_perm.end());

            if (options.force_order)
            {
                if (algo == CAR && seen_nb > 1)
                {
                    auto map = maps[e.dst];
                    acc_cond::mark_t first_vals(
                        new_perm.begin(), new_perm.begin() + seen_nb);
                    auto new_start = map.find(first_vals);
                    assert(new_start->second.size() >= seen_nb);
                    assert(new_start != map.end());
                    for (unsigned i = 0; i < seen_nb; ++i)
                        new_perm[i] = new_start->second[i];
                }
                else if ((algo == IAR_Streett || algo == IAR_Rabin)
                        && seen_nb > 1)
                {
                    auto map = iar_maps[e.dst];
                    std::vector<unsigned> first_vals(
                            new_perm.begin(), new_perm.begin() + seen_nb);
                    std::sort(std::begin(first_vals), std::end(first_vals));
                    auto new_start = map.find(first_vals);
                    assert(new_start->second.size() >= seen_nb);
                    assert(new_start != map.end());
                    for (unsigned i = 0; i < seen_nb; ++i)
                        new_perm[i] = new_start->second[i];
                }
            }

            // Optimization: when several indices are seen in the
            // transition, they move at the front of new_perm in any
            // order. Check whether there already exists an car_state
            // that matches this condition.
            car_state dst;
            unsigned dst_num = -1U;

            if (options.search_ex)
                dst_num = state_2_car.get_res_state(e.dst, new_perm, seen_nb,
                            options.use_last);

            if (dst_num == -1U)
            {
                auto dst = car_state{ (*init_states)[e.dst], e.dst, new_perm };
                dst_num = get_state(dst);
                if (nb_created_states > max_states)
                    return -1U;
            }

            acc_cond::mark_t acc = { };

            switch (algo)
            {
            case IAR_Rabin:
            case IAR_Streett:
                get_acceptance_iar(algo, current.perm, pairs, e.acc, acc);
                break;
            case CAR:
                get_acceptance_car(sub_automaton->acc(), new_perm, h, acc);
                break;
            default:
                assert(false);
            }

            unsigned acc_col = acc.min_set() - 1;
            if (options.parity_prefix)
            {
                if (acc_col > max_free_color)
                    throw std::runtime_error("CAR needs more sets");
                // parity prefix
                for (auto col : e.acc.sets())
                {
                    if (inf_fin_prefix[col] + is_odd > (int) acc_col)
                        acc_col = (unsigned) inf_fin_prefix[col] + is_odd;
                }
            }
            auto new_e = res_->new_edge(src_num, dst_num, e.cond, { acc_col });
            edge_to_colors.insert({new_e, not_moved});
        }
    }
    if (options.search_ex && options.use_last)
    {
        std::vector<unsigned> added_states;
        std::transform(car2num_local.begin(), car2num_local.end(),
                       std::back_inserter(added_states),
                       [](std::pair<const car_state, unsigned> pair) {
                           return pair.second;
                       });
        change_transitions_destination(
            res_, added_states, edge_to_colors, state_2_car);
    }
    auto leaving_edge =
        [&](unsigned d){
            return scc_.scc_of(num2car.at(d).state) != scc_num;
        };
    auto filter_edge =
        [](const twa_graph::edge_storage_t &,
           unsigned dst,
           void* filter_data){
            decltype(leaving_edge) *data =
                static_cast<decltype(leaving_edge)*>(filter_data);

            if ((*data)(dst))
                return scc_info::edge_filter_choice::ignore;

            return scc_info::edge_filter_choice::keep;
        };
    scc_info sub_scc(res_, get_state(s0), filter_edge, &leaving_edge);

    // SCCs are numbered in reverse topological order, so the bottom SCC has
    // index 0.
    const unsigned bscc = 0;
    assert(sub_scc.scc_count() != 0);
    assert(sub_scc.succ(0).empty());
    assert(
        [&](){
                    for (unsigned s = 1; s != sub_scc.scc_count(); ++s)
                        if (sub_scc.succ(s).empty())
                            return false;

                    return true;
                } ());

    assert(sub_scc.states_of(bscc).size() >= sub_automaton->num_states());

    // update state2car
    for (unsigned scc_state : sub_scc.states_of(bscc))
    {
        car_state &car = num2car.at(scc_state);

        if (state2car_local.find(car.state) == state2car_local.end())
            state2car_local[car.state] = car;
    }
    return sub_scc.states_of(bscc).size();
}

algorithm
chooseAlgo(twa_graph_ptr &sub_automaton,
           twa_graph_ptr &rabin_aut,
           std::vector<acc_cond::rs_pair> &pairs,
           std::vector<unsigned> &permut)
{
    auto scc_condition = sub_automaton->acc();
    if (options.parity_equiv)
    {
        if (scc_condition.is_f())
            return False_clean;
        if (scc_condition.is_t())
            return True_clean;
        std::vector<int> permut_tmp(scc_condition.all_sets().max_set(), -1);

        if (!is_odd && scc_condition.is_parity_max_equiv(permut_tmp, true))
        {
            for (auto c : permut_tmp)
                permut.push_back((unsigned) c);

            scc_condition.apply_permutation(permut);
            sub_automaton->apply_permutation(permut);
            return Copy_even;
        }
        std::fill(permut_tmp.begin(), permut_tmp.end(), -1);
        if (scc_condition.is_parity_max_equiv(permut_tmp, false))
        {
            for (auto c : permut_tmp)
                permut.push_back((unsigned) c);
            scc_condition.apply_permutation(permut);
            sub_automaton->apply_permutation(permut);
            return Copy_odd;
        }
    }

    if (options.rabin_to_buchi)
    {
        auto ra = rabin_to_buchi_if_realizable(sub_automaton);
        if (ra != nullptr)
        {
            rabin_aut = ra;
            return Rabin_to_Buchi;
        }
        else
        {
            bool streett_buchi = false;
            auto sub_cond = sub_automaton->get_acceptance();
            sub_automaton->set_acceptance(sub_cond.complement());
            auto ra = rabin_to_buchi_if_realizable(sub_automaton);
            streett_buchi = (ra != nullptr);
            sub_automaton->set_acceptance(sub_cond);
            if (streett_buchi)
            {
                rabin_aut = ra;
                return Streett_to_Buchi;
            }
        }
    }

    auto pairs1 = std::vector<acc_cond::rs_pair>();
    auto pairs2 = std::vector<acc_cond::rs_pair>();
    std::sort(pairs1.begin(), pairs1.end());
    pairs1.erase(std::unique(pairs1.begin(), pairs1.end()), pairs1.end());
    std::sort(pairs2.begin(), pairs2.end());
    pairs2.erase(std::unique(pairs2.begin(), pairs2.end()), pairs2.end());
    bool is_r_like = scc_condition.is_rabin_like(pairs1);
    bool is_s_like = scc_condition.is_streett_like(pairs2);
    unsigned num_cols = scc_condition.get_acceptance().used_sets().count();
    if (is_r_like)
    {
        if ((is_s_like && pairs1.size() < pairs2.size()) || !is_s_like)
        {
            if (pairs1.size() > num_cols)
                return CAR;
            pairs = pairs1;
            return IAR_Rabin;
        }
        else if (is_s_like)
        {
            if (pairs2.size() > num_cols)
                return CAR;
            pairs = pairs2;
            return IAR_Streett;
        }
    }
    else
    {
        if (is_s_like)
        {
            if (pairs2.size() > num_cols)
                return CAR;
            pairs = pairs2;
            return IAR_Streett;
        }
    }
    return CAR;
}

unsigned
build_scc(twa_graph_ptr &sub_automaton,
          unsigned scc_num,
          std::map<unsigned, car_state>& state2car_local,
          std::map<car_state, unsigned>&car2num_local,
          algorithm& algo,
          unsigned max_states = -1U)
{

    std::vector<int> parity_prefix_colors (SPOT_MAX_ACCSETS,
                                            - SPOT_MAX_ACCSETS - 2);
    unsigned min_prefix_color = SPOT_MAX_ACCSETS + 1;
    if (options.parity_prefix)
    {
        auto new_acc = sub_automaton->acc();
        auto colors = std::vector<unsigned>();
        bool inf_start =
            sub_automaton->acc().has_parity_prefix(new_acc, colors);
        sub_automaton->set_acceptance(new_acc);
        for (unsigned i = 0; i < colors.size(); ++i)
            parity_prefix_colors[colors[i]] =
                SPOT_MAX_ACCSETS - 4 - i - !inf_start;
        if (colors.size() > 0)
            min_prefix_color =
                SPOT_MAX_ACCSETS - 4 - colors.size() - 1 - !inf_start;
    }
    --min_prefix_color;

    unsigned init = 0;

    std::vector<acc_cond::rs_pair> pairs = { };
    auto permut = std::vector<unsigned>();
    twa_graph_ptr rabin_aut = nullptr;
    algo = chooseAlgo(sub_automaton, rabin_aut, pairs, permut);
    switch (algo)
    {
    case False_clean:
    case True_clean:
        return apply_false_true_clean(sub_automaton, (algo == True_clean),
                                      parity_prefix_colors, min_prefix_color,
                                      state2car_local, car2num_local);
        break;
    case IAR_Streett:
    case IAR_Rabin:
    case CAR:
        return apply_lar(sub_automaton, init, pairs, algo, scc_num,
                         parity_prefix_colors, min_prefix_color,
                         state2car_local, car2num_local, max_states);
        break;
    case Copy_odd:
    case Copy_even:
        return apply_copy(sub_automaton, permut, algo == Copy_odd,
                          parity_prefix_colors, state2car_local,
                          car2num_local);
        break;
    case Rabin_to_Buchi:
    case Streett_to_Buchi:
        return apply_to_Buchi(sub_automaton, rabin_aut,
                              (algo == Streett_to_Buchi),
                              parity_prefix_colors, min_prefix_color,
                              state2car_local, car2num_local);
        break;
    default:
        break;
    }
    return -1U;
}

public:
twa_graph_ptr
run()
{
    res_ = make_twa_graph(aut_->get_dict());
    res_->copy_ap_of(aut_);
    for (unsigned scc = 0; scc < scc_.scc_count(); ++scc)
    {
        if (!scc_.is_useful_scc(scc))
            continue;
        auto sub_automata = scc_.split_on_sets(scc, { }, true);
        if (sub_automata.empty())
        {
            for (auto state : scc_.states_of(scc))
            {
                auto new_state = res_->new_state();
                car_state new_car = { state, state, perm_t() };
                car2num[new_car] = new_state;
                num2car.insert(num2car.begin() + new_state, new_car);
                if (options.pretty_print)
                    names->push_back(new_car.to_string(None));
                state2car[state] = new_car;
            }
            continue;
        }

        auto sub_automaton = sub_automata[0];
        auto deg = sub_automaton;
        if (options.acc_clean)
            simplify_acceptance_here(sub_automaton);
        bool has_degeneralized = false;
        if (options.partial_degen)
        {
            std::vector<acc_cond::mark_t> forbid;
            auto m =
                is_partially_degeneralizable(sub_automaton, true,
                                             true, forbid);
            while (m != acc_cond::mark_t {})
            {
                auto tmp = partial_degeneralize(deg, m);
                simplify_acceptance_here(tmp);
                if (tmp->get_acceptance().used_sets().count()
                    < deg->get_acceptance().used_sets().count() ||
                    !(options.reduce_col_deg))
                {
                    deg = tmp;
                    has_degeneralized = true;
                }
                else
                    forbid.push_back(m);
                m = is_partially_degeneralizable(deg, true, true, forbid);
            }
        }

        if (options.propagate_col)
        {
            propagate_marks_here(sub_automaton);
            if (deg != sub_automaton)
              propagate_marks_here(deg);
        }

        std::map<unsigned, car_state> state2car_sub, state2car_deg;
        std::map<car_state, unsigned> car2num_sub, car2num_deg;

        unsigned nb_states_deg = -1U,
                 nb_states_sub = -1U;

        algorithm algo_sub, algo_deg;
        unsigned max_states_sub_car = -1U;
        // We try with and without degeneralization and we keep the best.
        if (has_degeneralized)
        {
            nb_states_deg =
                build_scc(deg, scc, state2car_deg, car2num_deg, algo_deg);
            // We suppose that if we see nb_states_deg + 1000 states when
            // when construct the version without degeneralization during the
            // construction, we will not be able to have nb_states_deg after
            // removing useless states. So we will stop the execution.
            max_states_sub_car =
                10000 + nb_states_deg - 1;
        }
        if (!options.force_degen || !has_degeneralized)
            nb_states_sub =
                build_scc(sub_automaton, scc, state2car_sub, car2num_sub,
                          algo_sub, max_states_sub_car);
        if (nb_states_deg < nb_states_sub)
        {
            state2car.insert(state2car_deg.begin(), state2car_deg.end());
            car2num.insert(car2num_deg.begin(), car2num_deg.end());
            algo_sub = algo_deg;
        }
        else
        {
            state2car.insert(state2car_sub.begin(), state2car_sub.end());
            car2num.insert(car2num_sub.begin(), car2num_sub.end());
        }
        if ((algo_sub == IAR_Rabin || algo_sub == Copy_odd) && !is_odd)
        {
            is_odd = true;
            for (auto &edge : res_->edges())
            {
                if (scc_.scc_of(num2car[edge.src].state) != scc
                   && scc_.scc_of(num2car[edge.dst].state) != scc)
                {
                    if (edge.acc == acc_cond::mark_t{})
                        edge.acc = { 0 };
                    else
                        edge.acc <<= 1;
                }
            }
        }
    }

    for (unsigned state = 0; state < res_->num_states(); ++state)
    {
        unsigned original_state = num2car.at(state).state;
        auto state_scc = scc_.scc_of(original_state);
        for (auto edge : aut_->out(original_state))
        {
            if (scc_.scc_of(edge.dst) != state_scc)
            {
                auto car = state2car.find(edge.dst);
                if (car != state2car.end())
                {
                    unsigned res_dst = car2num.at(car->second);
                    res_->new_edge(state, res_dst, edge.cond, { });
                }
            }
        }
    }
    unsigned initial_state = aut_->get_init_state_number();
    auto initial_car_ptr = state2car.find(initial_state);
    car_state initial_car;
    // If we take an automaton with one state and without transition,
    // the SCC was useless so state2car doesn't have initial_state
    if (initial_car_ptr == state2car.end())
    {
        assert(res_->num_states() == 0);
        auto new_state = res_->new_state();
        car_state new_car = {initial_state, 0, perm_t()};
        state2car[initial_state] = new_car;
        if (options.pretty_print)
            names->push_back(new_car.to_string(None));
        num2car.insert(num2car.begin() + new_state, new_car);
        car2num[new_car] = new_state;
        initial_car = new_car;
    }
    else
        initial_car = initial_car_ptr->second;
    auto initial_state_res = car2num.find(initial_car);
    if (initial_state_res != car2num.end())
        res_->set_init_state(initial_state_res->second);
    else
        res_->new_state();
    if (options.pretty_print)
        res_->set_named_prop("state-names", names);

    res_->purge_unreachable_states();
    // If parity_prefix is used, we use all available colors by
    // default: The IAR/CAR are using lower indices, and the prefix is
    // using the upper indices.  So we use reduce_parity() to clear
    // the mess.   If parity_prefix is not used,
    unsigned max_color = SPOT_MAX_ACCSETS;
    if (!options.parity_prefix)
      {
        acc_cond::mark_t all = {};
        for (auto& e: res_->edges())
          all |= e.acc;
        max_color = all.max_set();
      }
    res_->set_acceptance(acc_cond::acc_code::parity_max(is_odd, max_color));
    if (options.parity_prefix)
      reduce_parity_here(res_);
    return res_;
}

private:
const const_twa_graph_ptr &aut_;
const scc_info scc_;
twa_graph_ptr res_;
// Says if we constructing an odd or even max
bool is_odd;

std::vector<car_state> num2car;
std::map<unsigned, car_state> state2car;
std::map<car_state, unsigned> car2num;

to_parity_options options;

std::vector<std::string>* names;
}; // car_generator

}// namespace


twa_graph_ptr
to_parity(const const_twa_graph_ptr &aut, const to_parity_options options)
{
    return car_generator(aut, options).run();
}

  // Old version of CAR.
  namespace
  {
    struct lar_state
    {
      unsigned state;
      std::vector<unsigned> perm;

      bool operator<(const lar_state& s) const
      {
        return state == s.state ? perm < s.perm : state < s.state;
      }

      std::string to_string() const
      {
        std::stringstream s;
        s << state << " [";
        unsigned ps = perm.size();
        for (unsigned i = 0; i != ps; ++i)
          {
            if (i > 0)
              s << ',';
            s << perm[i];
          }
        s << ']';
        return s.str();
      }
    };

    class lar_generator
    {
      const const_twa_graph_ptr& aut_;
      twa_graph_ptr res_;
      const bool pretty_print;

      std::map<lar_state, unsigned> lar2num;
    public:
      explicit lar_generator(const const_twa_graph_ptr& a, bool pretty_print)
      : aut_(a)
      , res_(nullptr)
      , pretty_print(pretty_print)
      {}

      twa_graph_ptr run()
      {
        res_ = make_twa_graph(aut_->get_dict());
        res_->copy_ap_of(aut_);

        std::deque<lar_state> todo;
        auto get_state = [this, &todo](const lar_state& s)
          {
            auto it = lar2num.emplace(s, -1U);
            if (it.second) // insertion took place
              {
                unsigned nb = res_->new_state();
                it.first->second = nb;
                todo.push_back(s);
              }
            return it.first->second;
          };

        std::vector<unsigned> initial_perm(aut_->num_sets());
        std::iota(initial_perm.begin(), initial_perm.end(), 0);
        {
          lar_state s0{aut_->get_init_state_number(), initial_perm};
          res_->set_init_state(get_state(s0));
        }

        scc_info si(aut_, scc_info_options::NONE);
        // main loop
        while (!todo.empty())
          {
            lar_state current = todo.front();
            todo.pop_front();

            // TODO: todo could store this number to avoid one lookup
            unsigned src_num = get_state(current);

            unsigned source_scc = si.scc_of(current.state);
            for (const auto& e : aut_->out(current.state))
              {
                // find the new permutation
                std::vector<unsigned> new_perm = current.perm;
                unsigned h = 0;
                for (unsigned k : e.acc.sets())
                  {
                    auto it = std::find(new_perm.begin(), new_perm.end(), k);
                    h = std::max(h, unsigned(new_perm.end() - it));
                    std::rotate(it, it+1, new_perm.end());
                  }

                if (source_scc != si.scc_of(e.dst))
                  {
                    new_perm = initial_perm;
                    h = 0;
                  }

                lar_state dst{e.dst, new_perm};
                unsigned dst_num = get_state(dst);

                // Do the h last elements satisfy the acceptance condition?
                // If they do, emit 2h, if they don't emit 2h+1.
                acc_cond::mark_t m(new_perm.end() - h, new_perm.end());
                bool rej = !aut_->acc().accepting(m);
                res_->new_edge(src_num, dst_num, e.cond, {2*h + rej});
              }
          }

        // parity max even
        unsigned sets = 2*aut_->num_sets() + 2;
        res_->set_acceptance(sets, acc_cond::acc_code::parity_max_even(sets));

        if (pretty_print)
          {
            auto names = new std::vector<std::string>(res_->num_states());
            for (const auto& p : lar2num)
              (*names)[p.second] = p.first.to_string();
            res_->set_named_prop("state-names", names);
          }

        return res_;
      }
    };
  }

  twa_graph_ptr
  to_parity_old(const const_twa_graph_ptr& aut, bool pretty_print)
  {
    if (!aut->is_existential())
      throw std::runtime_error("LAR does not handle alternation");
    // if aut is already parity return it as is
    if (aut->acc().is_parity())
      return std::const_pointer_cast<twa_graph>(aut);

    lar_generator gen(aut, pretty_print);
    return gen.run();
  }

}
