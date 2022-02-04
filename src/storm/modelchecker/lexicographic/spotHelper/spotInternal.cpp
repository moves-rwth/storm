// Copied and adapted from https://gitlab.lrde.epita.fr/spot

#include "storm/modelchecker/lexicographic/spotHelper/spotInternal.h"
#include <deque>

namespace storm {
namespace spothelper {
unsigned int testFunc() {
    return 10;
}
typedef std::pair<unsigned, unsigned> product_state;

struct product_state_hash {
    size_t operator()(product_state s) const noexcept {
        return wang32_hash(s.first ^ wang32_hash(s.second));
    }
};

template<typename T>
static void product_main(const const_twa_graph_ptr& left, const const_twa_graph_ptr& right, unsigned left_state, unsigned right_state, twa_graph_ptr& res,
                         T merge_acc, const output_aborter* aborter) {
    std::unordered_map<product_state, unsigned, product_state_hash> s2n;
    std::deque<std::pair<product_state, unsigned>> todo;

    auto v = new product_states;
    res->set_named_prop("product-states", v);

    auto new_state = [&](unsigned left_state, unsigned right_state) -> unsigned {
        product_state x(left_state, right_state);
        auto p = s2n.emplace(x, 0);
        if (p.second)  // This is a new state
        {
            p.first->second = res->new_state();
            todo.emplace_back(x, p.first->second);
            assert(p.first->second == v->size());
            v->emplace_back(x);
        }
        return p.first->second;
    };

    res->set_init_state(new_state(left_state, right_state));
    if (res->acc().is_f())
        // Do not bother doing any work if the resulting acceptance is
        // false.
        return;
    while (!todo.empty()) {
        if (aborter && aborter->too_large(res)) {
            res = nullptr;
            return;
        }
        auto top = todo.front();
        todo.pop_front();
        for (auto& l : left->out(top.first.first))
            for (auto& r : right->out(top.first.second)) {
                auto cond = l.cond & r.cond;
                if (cond == bddfalse)
                    continue;
                auto dst = new_state(l.dst, r.dst);
                res->new_edge(top.second, dst, cond, merge_acc(l.acc, r.acc));
                // If right is deterministic, we can abort immediately!
            }
    }
}

enum acc_op { and_acc, or_acc, xor_acc, xnor_acc };

static twa_graph_ptr product_aux(const const_twa_graph_ptr& left, const const_twa_graph_ptr& right, unsigned left_state, unsigned right_state, acc_op aop,
                                 const output_aborter* aborter) {
    if (SPOT_UNLIKELY(!(left->is_existential() && right->is_existential())))
        throw std::runtime_error("product() does not support alternating automata");
    if (SPOT_UNLIKELY(left->get_dict() != right->get_dict()))
        throw std::runtime_error(
            "product: left and right automata should "
            "share their bdd_dict");

    auto res = make_twa_graph(left->get_dict());
    res->copy_ap_of(left);
    res->copy_ap_of(right);

    auto left_num = left->num_sets();
    auto& left_acc = left->get_acceptance();
    auto right_acc = right->get_acceptance() << left_num;
    right_acc &= left_acc;

    res->set_acceptance(left_num + right->num_sets(), right_acc);

    product_main(
        left, right, left_state, right_state, res, [&](acc_cond::mark_t ml, acc_cond::mark_t mr) { return ml | (mr << left_num); }, aborter);

    if (!res)  // aborted
        return nullptr;

    if (res->acc().is_f()) {
        assert(res->num_edges() == 0);
        res->prop_universal(true);
        res->prop_complete(false);
        res->prop_stutter_invariant(true);
        res->prop_terminal(true);
        res->prop_state_acc(true);
    } else {
        // The product of two non-deterministic automata could be
        // deterministic.  Likewise for non-complete automata.
        if (left->prop_universal() && right->prop_universal())
            res->prop_universal(true);
        if (left->prop_complete() && right->prop_complete())
            res->prop_complete(true);
        if (left->prop_stutter_invariant() && right->prop_stutter_invariant())
            res->prop_stutter_invariant(true);
        if (left->prop_inherently_weak() && right->prop_inherently_weak())
            res->prop_inherently_weak(true);
        if (left->prop_weak() && right->prop_weak())
            res->prop_weak(true);
        if (left->prop_terminal() && right->prop_terminal())
            res->prop_terminal(true);
        res->prop_state_acc(left->prop_state_acc() && right->prop_state_acc());
    }
    return res;
}

twa_graph_ptr product(const const_twa_graph_ptr& left, const const_twa_graph_ptr& right, unsigned left_state, unsigned right_state,
                      const output_aborter* aborter) {
    return product_aux(left, right, left_state, right_state, and_acc, aborter);
}

twa_graph_ptr product(const const_twa_graph_ptr& left, const const_twa_graph_ptr& right, const output_aborter* aborter) {
    return product(left, right, left->get_init_state_number(), right->get_init_state_number(), aborter);
}
}  // namespace spothelper
}  // namespace storm