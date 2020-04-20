#include "storm/simulator/DiscreteTimeSparseModelSimulator.h"
#include "storm/models/sparse/Model.h"

namespace storm {
    namespace simulator {
        template<typename ValueType, typename RewardModelType>
        DiscreteTimeSparseModelSimulator<ValueType,RewardModelType>::DiscreteTimeSparseModelSimulator(storm::models::sparse::Model<ValueType, RewardModelType> const& model) : currentState(*model.getInitialStates().begin()), model(model) {
            STORM_LOG_WARN_COND(model.getInitialStates().getNumberOfSetBits()==1, "The model has multiple initial states. This simulator assumes it starts from the initial state with the lowest index.");

        }

        template<typename ValueType, typename RewardModelType>
        void DiscreteTimeSparseModelSimulator<ValueType,RewardModelType>::setSeed(uint64_t seed) {
            generator = storm::utility::RandomProbabilityGenerator<double>(seed);
        }

        template<typename ValueType, typename RewardModelType>
        bool DiscreteTimeSparseModelSimulator<ValueType,RewardModelType>::step(uint64_t action) {
            // TODO lots of optimization potential.
            //  E.g., do not sample random numbers if there is only a single transition
            ValueType probability = generator.random();
            STORM_LOG_ASSERT(action < model.getTransitionMatrix().getRowGroupSize(currentState), "Action index higher than number of actions");
            uint64_t row = model.getTransitionMatrix().getRowGroupIndices()[currentState] + action;
            ValueType sum = storm::utility::zero<ValueType>();
            for (auto const& entry : model.getTransitionMatrix().getRow(row)) {
                sum += entry.getValue();
                if (sum >= probability) {
                    currentState = entry.getColumn();
                    return true;
                }
            }
            return false;
            STORM_LOG_ASSERT(false, "This position should never be reached");
        }

        template<typename ValueType, typename RewardModelType>
        uint64_t DiscreteTimeSparseModelSimulator<ValueType, RewardModelType>::getCurrentState() const {
            return currentState;
        }

        template<typename ValueType, typename RewardModelType>
        bool DiscreteTimeSparseModelSimulator<ValueType,RewardModelType>::resetToInitial() {
            currentState = *model.getInitialStates().begin();
            return true;
        }


        template class DiscreteTimeSparseModelSimulator<double>;
    }
}
