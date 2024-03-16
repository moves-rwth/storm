#include "storm-pars/modelchecker/region/monotonicity/OrderBasedMonotonicityBackend.h"

#include "storm-pars/transformer/ParameterLifter.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/NotImplementedException.h"

namespace storm::modelchecker {

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
void OrderBasedMonotonicityBackend<ParametricType, ConstantType>::initializeMonotonicity(detail::AnnotatedRegion<ParametricType>& region) {
    // TODO: Implement
}

template<typename ParametricType, typename ConstantType>
void OrderBasedMonotonicityBackend<ParametricType, ConstantType>::updateMonotonicity(detail::AnnotatedRegion<ParametricType>& region) {
    // TODO: Implement
}

template<typename ParametricType, typename ConstantType>
std::map<typename OrderBasedMonotonicityBackend<ParametricType, ConstantType>::VariableType,
         typename OrderBasedMonotonicityBackend<ParametricType, ConstantType>::MonotonicityKind>
OrderBasedMonotonicityBackend<ParametricType, ConstantType>::getOptimisticMonotonicityApproximation(detail::AnnotatedRegion<ParametricType> const& region) {
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
    orderExtender = storm::analysis::OrderExtender<ParametricType, ConstantType>(parametricTransitionMatrix, topStates, bottomStates);
}

template<typename ParametricType, typename ConstantType>
storm::storage::BitVector OrderBasedMonotonicityBackend<ParametricType, ConstantType>::getChoicesToFixForPLASolver(
    detail::AnnotatedRegion<ParametricType> const& region, storm::transformer::ParameterLifter<ParametricType, ConstantType> const& parameterLifter,
    storm::OptimizationDirection dir, std::vector<uint64_t>& schedulerChoices) {
    if (useOnlyGlobal) {
        return {};
    }
    auto monotonicityAnnotation = region.getOrderBasedMonotonicityAnnotation();
    STORM_LOG_ASSERT(monotonicityAnnotation.has_value() && monotonicityAnnotation->localMonotonicityResult != nullptr,
                     "Order-based monotonicity annotation must be present.");
    auto const& localMonotonicityResult = *monotonicityAnnotation->localMonotonicityResult;

    storm::storage::BitVector result(schedulerChoices.size(), false);

    auto const& occurringVariables = parameterLifter->getOccurringVariablesAtState();
    for (uint64_t state = 0; state < parameterLifter->getRowGroupCount(); ++state) {
        auto oldStateNumber = parameterLifter->getOriginalStateNumber(state);
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

}  // namespace storm::modelchecker