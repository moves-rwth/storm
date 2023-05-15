#include "storm/solver/helper/ValueIterationHelper.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/solver/helper/ValueIterationOperator.h"
#include "storm/utility/Extremum.h"

namespace storm::solver::helper {

template<typename ValueType, storm::OptimizationDirection Dir, bool Relative>
class VIOperatorBackend {
   public:
    VIOperatorBackend(ValueType const& precision) : precision{precision} {
        // intentionally empty
    }

    void startNewIteration() {
        isConverged = true;
    }

    void firstRow(ValueType&& value, [[maybe_unused]] uint64_t rowGroup, [[maybe_unused]] uint64_t row) {
        best = std::move(value);
    }

    void nextRow(ValueType&& value, [[maybe_unused]] uint64_t rowGroup, [[maybe_unused]] uint64_t row) {
        best &= value;
    }

    void applyUpdate(ValueType& currValue, [[maybe_unused]] uint64_t rowGroup) {
        if (isConverged) {
            if constexpr (Relative) {
                isConverged = storm::utility::abs<ValueType>(currValue - *best) <= storm::utility::abs<ValueType>(precision * currValue);
            } else {
                isConverged = storm::utility::abs<ValueType>(currValue - *best) <= precision;
            }
        }
        currValue = std::move(*best);
    }

    void endOfIteration() const {
        // intentionally left empty.
    }

    bool converged() const {
        return isConverged;
    }

    bool constexpr abort() const {
        return false;
    }

   private:
    storm::utility::Extremum<Dir, ValueType> best;
    ValueType const precision;
    bool isConverged{true};
};

template<typename ValueType, bool TrivialRowGrouping>
ValueIterationHelper<ValueType, TrivialRowGrouping>::ValueIterationHelper(std::shared_ptr<ValueIterationOperator<ValueType, TrivialRowGrouping>> viOperator)
    : viOperator(viOperator) {
    // Intentionally left empty
}

template<typename ValueType, bool TrivialRowGrouping>
template<storm::OptimizationDirection Dir, bool Relative>
SolverStatus ValueIterationHelper<ValueType, TrivialRowGrouping>::VI(std::vector<ValueType>& operand, std::vector<ValueType> const& offsets,
                                                                     uint64_t& numIterations, ValueType const& precision,
                                                                     std::function<SolverStatus(SolverStatus const&)> const& iterationCallback,
                                                                     MultiplicationStyle mult) const {
    VIOperatorBackend<ValueType, Dir, Relative> backend{precision};
    std::vector<ValueType>* operand1{&operand};
    std::vector<ValueType>* operand2{&operand};
    if (mult == MultiplicationStyle::Regular) {
        operand2 = &viOperator->allocateAuxiliaryVector(operand.size());
    }
    bool resultInAuxVector{false};
    SolverStatus status{SolverStatus::InProgress};
    while (status == SolverStatus::InProgress) {
        ++numIterations;
        if (viOperator->template apply(*operand1, *operand2, offsets, backend)) {
            status = SolverStatus::Converged;
        } else if (iterationCallback) {
            status = iterationCallback(status);
        }
        if (mult == MultiplicationStyle::Regular) {
            std::swap(operand1, operand2);
            resultInAuxVector = !resultInAuxVector;
        }
    }
    if (mult == MultiplicationStyle::Regular) {
        if (resultInAuxVector) {
            STORM_LOG_ASSERT(&operand == operand2, "Unexpected operand address");
            std::swap(*operand1, *operand2);
        }
        viOperator->freeAuxiliaryVector();
    }
    return status;
}

template<typename ValueType, bool TrivialRowGrouping>
SolverStatus ValueIterationHelper<ValueType, TrivialRowGrouping>::VI(std::vector<ValueType>& operand, std::vector<ValueType> const& offsets,
                                                                     uint64_t& numIterations, bool relative, ValueType const& precision,
                                                                     std::optional<storm::OptimizationDirection> const& dir,
                                                                     std::function<SolverStatus(SolverStatus const&)> const& iterationCallback,
                                                                     MultiplicationStyle mult) const {
    STORM_LOG_ASSERT(TrivialRowGrouping || dir.has_value(), "no optimization direction given!");
    if (!dir.has_value() || maximize(*dir)) {
        if (relative) {
            return VI<storm::OptimizationDirection::Maximize, true>(operand, offsets, numIterations, precision, iterationCallback, mult);
        } else {
            return VI<storm::OptimizationDirection::Maximize, false>(operand, offsets, numIterations, precision, iterationCallback, mult);
        }
    } else {
        if (relative) {
            return VI<storm::OptimizationDirection::Minimize, true>(operand, offsets, numIterations, precision, iterationCallback, mult);
        } else {
            return VI<storm::OptimizationDirection::Minimize, false>(operand, offsets, numIterations, precision, iterationCallback, mult);
        }
    }
}

template<typename ValueType, bool TrivialRowGrouping>
SolverStatus ValueIterationHelper<ValueType, TrivialRowGrouping>::VI(std::vector<ValueType>& operand, std::vector<ValueType> const& offsets, bool relative,
                                                                     ValueType const& precision, std::optional<storm::OptimizationDirection> const& dir,
                                                                     std::function<SolverStatus(SolverStatus const&)> const& iterationCallback,
                                                                     MultiplicationStyle mult) const {
    uint64_t numIterations = 0;
    return VI(operand, offsets, numIterations, relative, precision, dir, iterationCallback, mult);
}

template class ValueIterationHelper<double, true>;
template class ValueIterationHelper<double, false>;
template class ValueIterationHelper<storm::RationalNumber, true>;
template class ValueIterationHelper<storm::RationalNumber, false>;

}  // namespace storm::solver::helper
