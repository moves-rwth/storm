#include "storm-pars/modelchecker/region/monotonicity/OrderBasedMonotonicityBackend.h"

#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/NotImplementedException.h"

namespace storm::modelchecker {

namespace detail {

template<typename ParametricType, typename ConstantType>
std::shared_ptr<storm::analysis::Order> extendOrder(storm::analysis::OrderExtender<ParametricType, ConstantType>& orderExtender,
                                                    std::shared_ptr<storm::analysis::Order> order, storm::storage::ParameterRegion<ParametricType> region) {
    auto [orderPtr, unknState1, unknState2] = orderExtender.extendOrder(order, region);
    order = orderPtr;
    if (unknState1 != order->getNumberOfStates()) {
        orderExtender.setUnknownStates(order, unknState1, unknState2);
    }
    return order;
}

template<typename ParametricType, typename ConstantType>
void extendLocalMonotonicityResult(storm::storage::ParameterRegion<ParametricType> const& region, storm::analysis::Order& order,
                                   storm::analysis::LocalMonotonicityResult<ParametricType>& localMonotonicityResult,
                                   storm::analysis::MonotonicityChecker<ParametricType>& monotonicityChecker,
                                   storm::transformer::ParameterLifter<ParametricType, ConstantType> const& parameterLifter) {
    auto state = order.getNextDoneState(-1);
    auto const& variablesAtState = parameterLifter.getOccurringVariablesAtState();
    while (state != order.getNumberOfStates()) {
        if (localMonotonicityResult->getMonotonicity(state) == nullptr) {
            auto variables = variablesAtState[state];
            if (variables.size() == 0 || order.isBottomState(state) || order.isTopState(state)) {
                localMonotonicityResult->setConstant(state);
            } else {
                for (auto const& var : variables) {
                    auto monotonicity = localMonotonicityResult->getMonotonicity(state, var);
                    if (!storm::analysis::isMonotone(monotonicity)) {
                        monotonicity = monotonicityChecker.checkLocalMonotonicity(order, state, var, region);
                        if (storm::analysis::isMonotone(monotonicity)) {
                            localMonotonicityResult->setMonotonicity(state, var, monotonicity);
                        } else {
                            // TODO: Skip for now?
                        }
                    }
                }
            }
        }
        state = order.getNextDoneState(state);
    }
    auto const& statesAtVariable = parameterLifter.getOccuringStatesAtVariable();
    bool allDone = true;
    for (auto const& entry : statesAtVariable) {
        auto states = entry.second;
        auto var = entry.first;
        bool done = true;
        for (auto const& state : states) {
            done &= order.contains(state) && localMonotonicityResult->getMonotonicity(state, var) != storm::analysis::MonotonicityKind::Unknown;
            if (!done) {
                break;
            }
        }

        allDone &= done;
        if (done) {
            localMonotonicityResult->getGlobalMonotonicityResult()->setDoneForVar(var);
        }
    }
    if (allDone) {
        localMonotonicityResult->setDone();
        while (order.existsNextState()) {
            // Simply add the states we couldn't add sofar between =) and =( as we could find local monotonicity for all parametric states
            order.add(order.getNextStateNumber().second);
        }
        assert(order.getDoneBuilding());
    }
}
}  // namespace detail

template<typename ParametricType, typename ConstantType>
OrderBasedMonotonicityBackend<ParametricType, ConstantType>::OrderBasedMonotonicityBackend(bool useOnlyGlobal, bool useBounds)
    : useOnlyGlobal(useOnlyGlobal), useBounds(useBounds) {
    // Intentioanlly left empty
}

template<typename ParametricType, typename ConstantType>
bool OrderBasedMonotonicityBackend<ParametricType, ConstantType>::requiresInteractionWithRegionModelChecker() const {
    return true;
}

template<typename ParametricType, typename ConstantType>
bool OrderBasedMonotonicityBackend<ParametricType, ConstantType>::recommendModelSimplifications() const {
    return false;
}

template<typename ParametricType, typename ConstantType>
void OrderBasedMonotonicityBackend<ParametricType, ConstantType>::initializeMonotonicity(AnnotatedRegion<ParametricType>& region) {
    // TODO: Implement

    /* From extendlocalMOnotonicityResult
    if (!localMonotonicityResult->isFixedParametersSet()) {
        for (auto& var : this->monotoneIncrParameters.get()) {
            localMonotonicityResult->setMonotoneIncreasing(var);
        }
        for (auto& var : this->monotoneDecrParameters.get()) {
            localMonotonicityResult->setMonotoneDecreasing(var);
        }
    }*/
}

template<typename ParametricType, typename ConstantType>
void OrderBasedMonotonicityBackend<ParametricType, ConstantType>::updateMonotonicity(AnnotatedRegion<ParametricType>& region) {
    // TODO: Implement
}

template<typename ParametricType, typename ConstantType>
std::map<typename OrderBasedMonotonicityBackend<ParametricType, ConstantType>::VariableType,
         typename OrderBasedMonotonicityBackend<ParametricType, ConstantType>::MonotonicityKind>
OrderBasedMonotonicityBackend<ParametricType, ConstantType>::getOptimisticMonotonicityApproximation(AnnotatedRegion<ParametricType> const& region) {
    // TODO: implement
}

template<typename ParametricType, typename ConstantType>
void OrderBasedMonotonicityBackend<ParametricType, ConstantType>::initializeMonotonicityChecker(
    storm::storage::SparseMatrix<ParametricType> const& parametricTransitionMatrix) {
    monotonicityChecker = storm::analysis::MonotonicityChecker<ParametricType>(parametricTransitionMatrix);
}

template<typename ParametricType, typename ConstantType>
void OrderBasedMonotonicityBackend<ParametricType, ConstantType>::initializeOrderExtender(
    storm::storage::BitVector const& topStates, storm::storage::BitVector const& bottomStates,
    storm::storage::SparseMatrix<ParametricType> const& parametricTransitionMatrix) {
    orderExtender = storm::analysis::OrderExtender<ParametricType, ConstantType>(topStates, bottomStates, parametricTransitionMatrix);
}

template<typename ParametricType, typename ConstantType>
void OrderBasedMonotonicityBackend<ParametricType, ConstantType>::registerParameterLifterReference(
    storm::transformer::ParameterLifter<ParametricType, ConstantType> const& parameterLifter) {
    this->parameterLifterRef.reset(parameterLifter);
}

template<typename ParametricType, typename ConstantType>
storm::storage::BitVector OrderBasedMonotonicityBackend<ParametricType, ConstantType>::getChoicesToFixForPLASolver(
    AnnotatedRegion<ParametricType> const& region, storm::OptimizationDirection dir, std::vector<uint64_t>& schedulerChoices) {
    if (useOnlyGlobal) {
        return {};
    }
    STORM_LOG_ASSERT(parameterLifterRef.has_value(), "Parameter lifter reference not initialized.");

    auto monotonicityAnnotation = region.getOrderBasedMonotonicityAnnotation();
    STORM_LOG_ASSERT(monotonicityAnnotation.has_value() && monotonicityAnnotation->localMonotonicityResult != nullptr,
                     "Order-based monotonicity annotation must be present.");
    auto const& localMonotonicityResult = *monotonicityAnnotation->localMonotonicityResult;

    storm::storage::BitVector result(schedulerChoices.size(), false);

    auto const& occurringVariables = parameterLifterRef->getOccurringVariablesAtState();
    for (uint64_t state = 0; state < parameterLifterRef->getRowGroupCount(); ++state) {
        auto oldStateNumber = parameterLifterRef->getOriginalStateNumber(state);
        auto const& variables = occurringVariables.at(oldStateNumber);
        // point at which we start with rows for this state

        STORM_LOG_THROW(variables.size() <= 1, storm::exceptions::NotImplementedException,
                        "Using localMonRes not yet implemented for states with 2 or more variables, please run without --use-monotonicity");

        bool allMonotone = true;
        for (auto var : variables) {
            auto const monotonicity = localMonotonicityResult.getMonotonicity(oldStateNumber, var);

            bool const fixToLowerBound =
                monotonicity == MonotonicityKind::Constant || monotonicity == (storm::solver::minimize(dir) ? MonotonicityKind::Incr : MonotonicityKind::Decr);
            bool const fixToUpperBound =
                monotonicity == MonotonicityKind::Constant || monotonicity == (storm::solver::maximize(dir) ? MonotonicityKind::Incr : MonotonicityKind::Decr);
            if (fixToLowerBound || fixToUpperBound) {
                // TODO: Setting the lower/upper bounded choices like this is fragile and should be replaced by a more robust solution
                schedulerChoices[state] = fixToLowerBound ? 0 : 1;
            } else {
                allMonotone = false;
            }
        }
        if (allMonotone) {
            result.set(state);
        }
    }
    return result;
}

template class OrderBasedMonotonicityBackend<storm::RationalFunction, double>;
template class OrderBasedMonotonicityBackend<storm::RationalFunction, storm::RationalNumber>;

}  // namespace storm::modelchecker