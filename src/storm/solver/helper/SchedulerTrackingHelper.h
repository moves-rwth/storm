#pragma once
#include <memory>
#include <vector>

#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/helper/ValueIterationOperatorForward.h"

namespace storm::solver::helper {

/*!
 * Helper class to extract optimal scheduler choices from a MinMax equation system solution
 */
template<typename ValueType, typename SolutionType = ValueType>
class SchedulerTrackingHelper {
   public:
    /*!
     * Initializes this helper with the given value iteration operator
     */
    SchedulerTrackingHelper(std::shared_ptr<ValueIterationOperator<ValueType, false, SolutionType>> viOperator);

    /*!
     * Computes the optimal choices from the given solution.
     * Essentially, this applies one iteration of value iteration and stores the optimal choice for each state.
     * @param operandIn Operand for the value iteration step. Should hold the solution values of the MinMax equation system
     * @param offsets Offsets that are added to each choice result.
     * @param dir Optimization direction to consider.
     * @param schedulerStorage where the scheduler choices will be stored. Should have the same size as the operand(s).
     * @param robust Flag whether any uncertainty should be interpreted robustly.
     * @param operandOut if given, the result values of the performed value iteration step will be stored in this vector. Can be the same as operandIn.
     * @return True if the scheduler coincides with the provided scheduler encoded in schedulerStorage
     *
     * @note: schedulers are encoded using row indices that are local to their row group, i.e. schedulerStorage[i]==j means that we choose the j'th row of row
     * group i
     */
    bool computeScheduler(std::vector<SolutionType>& operandIn, std::vector<ValueType> const& offsets, storm::OptimizationDirection const& dir,
                          std::vector<uint64_t>& schedulerStorage, bool robust, std::vector<SolutionType>* operandOut = nullptr) const;

   private:
    /*!
     * Internal variant of computeScheduler
     */
    template<storm::OptimizationDirection Dir, storm::OptimizationDirection RobustDir>
    bool computeScheduler(std::vector<SolutionType>& operandIn, std::vector<ValueType> const& offsets, std::vector<uint64_t>& schedulerStorage,
                          std::vector<SolutionType>* operandOut) const;

   private:
    std::shared_ptr<ValueIterationOperator<ValueType, false, SolutionType>> viOperator;
};

}  // namespace storm::solver::helper
