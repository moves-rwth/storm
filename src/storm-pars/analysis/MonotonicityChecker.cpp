#include "MonotonicityChecker.h"

namespace storm {
namespace analysis {
/*** Constructor ***/
template<typename ValueType>
MonotonicityChecker<ValueType>::MonotonicityChecker(storage::SparseMatrix<ValueType> matrix) {
    this->matrix = matrix;
}

/*** Public methods ***/
template<typename ValueType>
typename MonotonicityChecker<ValueType>::Monotonicity MonotonicityChecker<ValueType>::checkLocalMonotonicity(
    std::shared_ptr<Order> const& order, uint_fast64_t state, VariableType const& var, storage::ParameterRegion<ValueType> const& region) {
    if (order->isActionSetAtState(state)) {
        return checkLocalMonotonicity(order, state, var, region, order->getActionAtState(state));
    } else {
        Monotonicity localMonotonicity = checkLocalMonotonicity(order, state, var, region, 0);
        for (typename storage::SparseMatrix<ValueType>::index_type act = 1; act < this->matrix.getRowGroupSize(state); ++act) {
            if (localMonotonicity == Monotonicity::Constant) {
                localMonotonicity = checkLocalMonotonicity(order, state, var, region, act);
            } else if (localMonotonicity == Monotonicity::Not || localMonotonicity == Monotonicity::Unknown) {
                break;
            } else {
                auto res = checkLocalMonotonicity(order, state, var, region, act);
                if (res != localMonotonicity) {
                    localMonotonicity = Monotonicity::Unknown;
                    break;
                }
            }
        }
        return localMonotonicity;
    }
}

/*** Private methods ***/

template<typename ValueType>
typename MonotonicityChecker<ValueType>::Monotonicity MonotonicityChecker<ValueType>::checkLocalMonotonicity(std::shared_ptr<Order> const& order,
                                                                                                             uint_fast64_t state, VariableType const& var,
                                                                                                             storage::ParameterRegion<ValueType> const& region,
                                                                                                             uint_fast64_t action) {
    // Create + fill Vector containing the Monotonicity of the transitions to the succs
    auto row = matrix.getRow(state, action);

    // Check if all transition probabilities are constant
    uint_fast64_t numberOfConstantTransitions = 0;
    for (auto& entry : row) {
        if (entry.getValue().isConstant()) {
            numberOfConstantTransitions++;
        }
    }
    if (row.getNumberOfEntries() == numberOfConstantTransitions) {
        return Monotonicity::Constant;
    }

    // Check if there are exactly two non-constant transition probabilities
    if (row.getNumberOfEntries() - numberOfConstantTransitions == 2) {
        uint_fast64_t offset1 = 0;
        while (row.begin()->getValue().isConstant()) {
            offset1++;
        }
        auto succ0 = (row.begin() + offset1)->getColumn();
        uint_fast64_t offset2 = offset1 + 1;
        while (row.begin()->getValue().isConstant()) {
            offset2++;
        }
        auto succ1 = (row.begin() + offset2)->getColumn();

        auto sorting = order->compare(succ0, succ1);
        if (sorting == Order::NodeComparison::SAME) {
            // It doesn't matter what we do, as it results to nodes at the same level in the order
            return Monotonicity::Constant;
        }

        if (sorting == Order::ABOVE) {
            // 0 is above 1
            return checkTransitionMonRes(row.begin()->getValue(), var, region);
        } else if (sorting == Order::BELOW) {
            return checkTransitionMonRes((row.begin() + 1)->getValue(), var, region);
        } else {
            return Monotonicity::Unknown;
        }
    }

    // There are more than two non-constant transition probabilities.
    // We need to go over all and combine the results
    // Vector containing the successor states
    std::vector<uint_fast64_t> succs;
    // Vector containing the monotonicity result of the transition from the state to the successor states
    std::vector<Monotonicity> succsMonUnsorted;
    for (auto entry : row) {
        auto succState = entry.getColumn();
        auto mon = checkTransitionMonRes(entry.getValue(), var, region);
        succsMonUnsorted.push_back(mon);
        succs.push_back(succState);
    }

    // Sort the successors based on the order
    auto succsSorted = order->sortStates(succs);
    // if the states could not be sorted, we should return unknown
    if (succsSorted.size() != succs.size() || succsSorted[succsSorted.size() - 1] == matrix.getRowGroupCount()) {
        return Monotonicity::Unknown;
    }
    uint_fast64_t succSize = succs.size();
    // First check as long as it stays constant and either incr or decr
    bool allowedToSwap = true;
    Monotonicity localMonotonicity = Monotonicity::Constant;
    uint_fast64_t index = 0;
    while (index < succSize && localMonotonicity == Monotonicity::Constant) {
        auto itr = std::find(succs.begin(), succs.end(), succsSorted[index]);
        auto newIndex = std::distance(succs.begin(), itr);
        auto transitionMon = succsMonUnsorted[newIndex];
        localMonotonicity = transitionMon;
        if (transitionMon == Monotonicity::Not && succSize != 1) {
            localMonotonicity = Monotonicity::Unknown;
        }
        index++;
    }

    while (index < succSize && localMonotonicity != Monotonicity::Not && localMonotonicity != Monotonicity::Unknown) {
        // We get here as soon as we have seen incr/decr once
        auto itr = std::find(succs.begin(), succs.end(), succsSorted[index]);
        STORM_LOG_ASSERT(itr != succs.end(), "Itr cannot be end of successors");
        auto newIndex = std::distance(succs.begin(), itr);
        auto transitionMon = succsMonUnsorted[newIndex];

        if (transitionMon == Monotonicity::Not || transitionMon == Monotonicity::Unknown) {
            return Monotonicity::Unknown;
        }
        if (allowedToSwap) {
            // So far we have only seen constant and either incr or decr, but not both
            if (transitionMon != Monotonicity::Constant && transitionMon != localMonotonicity) {
                allowedToSwap = false;
            }
        } else if (!allowedToSwap) {
            // So we have been at the point where we changed from incr to decr (or decr to incr)
            if (transitionMon == localMonotonicity || transitionMon == Monotonicity::Not || transitionMon == Monotonicity::Unknown) {
                localMonotonicity = Monotonicity::Unknown;
            }
        }
        index++;
    }
    return localMonotonicity;
}

template<typename ValueType>
typename MonotonicityChecker<ValueType>::Monotonicity MonotonicityChecker<ValueType>::checkTransitionMonRes(
    ValueType function, typename MonotonicityChecker<ValueType>::VariableType param, typename MonotonicityChecker<ValueType>::Region region) {
    std::pair<bool, bool> res = MonotonicityChecker<ValueType>::checkDerivative(getDerivative(function, param), region);
    if (res.first && !res.second) {
        return Monotonicity::Incr;
    } else if (!res.first && res.second) {
        return Monotonicity::Decr;
    } else if (res.first && res.second) {
        return Monotonicity::Constant;
    } else {
        return Monotonicity::Not;
    }
}

template<typename ValueType>
ValueType MonotonicityChecker<ValueType>::getDerivative(ValueType function, typename MonotonicityChecker<ValueType>::VariableType var) {
    return function.derivative(var);
}

template class MonotonicityChecker<RationalFunction>;
}  // namespace analysis
}  // namespace storm
