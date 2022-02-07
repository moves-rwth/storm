// Copied and adapted from https://gitlab.lrde.epita.fr/spot

#include "storm/modelchecker/lexicographic/spotHelper/spotInternal.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"
#include <deque>

namespace storm {
namespace spothelper {

typedef std::pair<unsigned, unsigned> product_state;

struct product_state_hash {
    size_t operator()(product_state s) const noexcept {
#ifdef STORM_HAVE_SPOT
        return wang32_hash(s.first ^ wang32_hash(s.second));
#endif
    }
};

enum acc_op { and_acc, or_acc, xor_acc, xnor_acc };

//twa_graph_ptr product(const const_twa_graph_ptr& left, const const_twa_graph_ptr& right) {
void* product(const void* leftVoid, const void* rightVoid) {
    twa_graph_ptr left = static_cast<twa_graph_ptr>(leftVoid);
    twa_graph_ptr right = static_cast<twa_graph_ptr>(rightVoid);
    output_aborter* aborter = nullptr;
    unsigned left_state = left->get_init_state_number();
    unsigned right_state = right->get_init_state_number();
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

    auto merge_acc = [&](acc_cond::mark_t ml, acc_cond::mark_t mr) { return ml | (mr << left_num); };
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
    while (!todo.empty()) {
        if (aborter && aborter->too_large(res)) {
            res = nullptr;
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
    return std::static_pointer_cast<void*>(res);

}
}  // namespace spothelper
}  // namespace storm
