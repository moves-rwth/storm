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
        for (auto act = 1; act < this->matrix.getRowGroupSize(state); ++act) {
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

    if (row.getNumberOfEntries() == 1) {
        return Monotonicity::Constant;
    }
    if (row.getNumberOfEntries() == 2) {
        auto succ0 = row.begin()->getColumn();
        auto succ1 = (row.begin() + 1)->getColumn();
        if (row.begin()->getValue().isConstant()) {
            // All transitions are constant
            return Monotonicity::Constant;
        }

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

    // Ignore if all entries are constant
    bool ignore = true;

    std::vector<uint_fast64_t> succs(row.getNumberOfEntries());
    std::vector<Monotonicity> succsMonUnsorted;
    std::vector<uint_fast64_t> statesIncr;
    std::vector<uint_fast64_t> statesDecr;
    bool checkAllow = true;
    for (auto entry : row) {
        auto succState = entry.getColumn();
        auto mon = checkTransitionMonRes(entry.getValue(), var, region);
        succsMonUnsorted.push_back(mon);
        succs.push_back(succState);
        ignore &= entry.getValue().isConstant();
        if (mon == Monotonicity::Incr) {
            statesIncr.push_back(succState);
        } else if (mon == Monotonicity::Decr) {
            statesDecr.push_back(succState);
        } else if (mon == Monotonicity::Not) {
            checkAllow = false;
        }
    }
    if (ignore) {
        return Monotonicity::Constant;
    }
    auto succsSorted = order->sortStates(succs);

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
