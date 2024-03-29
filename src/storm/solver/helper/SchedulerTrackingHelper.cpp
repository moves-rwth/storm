#include "storm/solver/helper/SchedulerTrackingHelper.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/solver/helper/ValueIterationOperator.h"
#include "storm/utility/Extremum.h"

namespace storm::solver::helper {

template<typename ValueType, storm::OptimizationDirection Dir>
class SchedulerTrackingBackend {
   public:
    SchedulerTrackingBackend(std::vector<uint64_t>& schedulerStorage,
                             std::vector<typename ValueIterationOperator<ValueType, false>::IndexType> const& rowGroupIndices, bool applyUpdates)
        : schedulerStorage(schedulerStorage), applyUpdates(applyUpdates), rowGroupIndices(rowGroupIndices) {
        // intentionally empty
    }
    void startNewIteration() {
        isConverged = true;
    }

    void firstRow(ValueType&& value, uint64_t rowGroup, uint64_t row) {
        currChoice = row - rowGroupIndices[rowGroup];
        best = std::move(value);
    }

    void nextRow(ValueType&& value, uint64_t rowGroup, uint64_t row) {
        if (best &= value) {
            currChoice = row - rowGroupIndices[rowGroup];
        } else if (*best == value) {
            // For rows that are equally good, we prefer the previously selected one.
            // This is necessary, e.g., for policy iteration correctness.
            if (uint64_t rowChoice = row - rowGroupIndices[rowGroup]; rowChoice == schedulerStorage[rowGroup]) {
                currChoice = rowChoice;
            }
        }
    }

    void applyUpdate(ValueType& currValue, uint64_t rowGroup) {
        if (applyUpdates) {
            currValue = std::move(*best);
        }
        auto& choice = schedulerStorage[rowGroup];
        if (isConverged) {
            isConverged = choice == currChoice;
        }
        choice = currChoice;
    }

    void endOfIteration() const {}

    bool converged() const {
        return isConverged;
    }

    bool constexpr abort() const {
        return false;
    }

   private:
    std::vector<uint64_t>& schedulerStorage;
    bool const applyUpdates;
    std::vector<typename ValueIterationOperator<ValueType, false>::IndexType> const& rowGroupIndices;

    bool isConverged;
    storm::utility::Extremum<Dir, ValueType> best;
    uint64_t currChoice;
};

template<typename ValueType, typename SolutionType>
SchedulerTrackingHelper<ValueType, SolutionType>::SchedulerTrackingHelper(std::shared_ptr<ValueIterationOperator<ValueType, false, SolutionType>> viOperator)
    : viOperator(viOperator) {
    // Intentionally left empty
}

template<typename ValueType, typename SolutionType>
template<storm::OptimizationDirection Dir, storm::OptimizationDirection RobustDir>
bool SchedulerTrackingHelper<ValueType, SolutionType>::computeScheduler(std::vector<SolutionType>& operandIn, std::vector<ValueType> const& offsets,
                                                                        std::vector<uint64_t>& schedulerStorage, std::vector<SolutionType>* operandOut) const {
    bool const applyUpdates = operandOut != nullptr;
    SchedulerTrackingBackend<SolutionType, Dir> backend(schedulerStorage, viOperator->getRowGroupIndices(), applyUpdates);
    if (applyUpdates) {
        return viOperator->template applyRobust<RobustDir>(*operandOut, operandIn, offsets, backend);
    } else {
        return viOperator->template applyInPlaceRobust<RobustDir>(operandIn, offsets, backend);
    }
}

template<typename ValueType, typename SolutionType>
bool SchedulerTrackingHelper<ValueType, SolutionType>::computeScheduler(std::vector<SolutionType>& operandIn, std::vector<ValueType> const& offsets,
                                                                        storm::OptimizationDirection const& dir, std::vector<uint64_t>& schedulerStorage,
                                                                        bool robust, std::vector<SolutionType>* operandOut) const {
    if (maximize(dir)) {
        if (robust) {
            return computeScheduler<storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize>(operandIn, offsets, schedulerStorage,
                                                                                                                    operandOut);
        } else {
            return computeScheduler<storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize>(operandIn, offsets, schedulerStorage,
                                                                                                                    operandOut);
        }
    } else {
        if (robust) {
            return computeScheduler<storm::OptimizationDirection::Minimize, OptimizationDirection::Maximize>(operandIn, offsets, schedulerStorage, operandOut);
        } else {
            return computeScheduler<storm::OptimizationDirection::Minimize, OptimizationDirection::Minimize>(operandIn, offsets, schedulerStorage, operandOut);
        }
    }
}

template class SchedulerTrackingHelper<double>;
template class SchedulerTrackingHelper<storm::RationalNumber>;
template class SchedulerTrackingHelper<storm::Interval, double>;

}  // namespace storm::solver::helper
