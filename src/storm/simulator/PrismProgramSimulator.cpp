#include "storm/simulator/PrismProgramSimulator.h"
#include "storm/exceptions/NotSupportedException.h"

using namespace storm::generator;

namespace storm {
    namespace simulator {

        template<typename ValueType>
        DiscreteTimePrismProgramSimulator<ValueType>::DiscreteTimePrismProgramSimulator(storm::prism::Program const& program, storm::generator::NextStateGeneratorOptions const& options)
        : program(program), currentState(), stateGenerator(std::make_shared<storm::generator::PrismNextStateGenerator<ValueType, uint32_t>>(program, options)), zeroRewards(stateGenerator->getNumberOfRewardModels(), storm::utility::zero<ValueType>()), lastActionRewards(zeroRewards)
        {
            // Current state needs to be overwritten to actual initial state.
            // But first, let us create a state generator.

            clearStateCaches();
            resetToInitial();
        }

        template<typename ValueType>
        void DiscreteTimePrismProgramSimulator<ValueType>::setSeed(uint64_t newSeed) {
            generator = storm::utility::RandomProbabilityGenerator<ValueType>(newSeed);
        }

        template<typename ValueType>
        bool DiscreteTimePrismProgramSimulator<ValueType>::step(uint64_t actionNumber) {
            uint32_t nextState = behavior.getChoices()[actionNumber].sampleFromDistribution(generator.random());
            lastActionRewards = behavior.getChoices()[actionNumber].getRewards();
            STORM_LOG_ASSERT(lastActionRewards.size() == stateGenerator->getNumberOfRewardModels(), "Reward vector should have as many rewards as model.");
            currentState = idToState[nextState];
            // TODO we do not need to do this in every step!
            clearStateCaches();
            explore();
            return true;
        }

        template<typename ValueType>
        bool DiscreteTimePrismProgramSimulator<ValueType>::explore() {
            // Load the current state into the next state generator.
            stateGenerator->load(currentState);
            // TODO: This low-level code currently expands all actions, while this may not be necessary.
            behavior = stateGenerator->expand(stateToIdCallback);
            if (behavior.getStateRewards().size() > 0) {
                STORM_LOG_ASSERT(behavior.getStateRewards().size() == lastActionRewards.size(), "Reward vectors should have same length.");
            }
            return true;
        }

        template<typename ValueType>
        std::vector<generator::Choice<ValueType, uint32_t>> const& DiscreteTimePrismProgramSimulator<ValueType>::getChoices() {
            return behavior.getChoices();
        }

        template<typename ValueType>
        std::vector<ValueType> const& DiscreteTimePrismProgramSimulator<ValueType>::getLastRewards() const {
            return lastActionRewards;
        }

        template<typename ValueType>
        CompressedState const& DiscreteTimePrismProgramSimulator<ValueType>::getCurrentState() const {
            return currentState;
        }

        template<typename ValueType>
        expressions::SimpleValuation DiscreteTimePrismProgramSimulator<ValueType>::getCurrentStateAsValuation() const {
            return unpackStateIntoValuation(currentState, stateGenerator->getVariableInformation(), program.getManager());
        }

        template<typename ValueType>
        std::string DiscreteTimePrismProgramSimulator<ValueType>::getCurrentStateString() const {
            return stateGenerator->stateToString(currentState);
        }

        template<typename ValueType>
        bool DiscreteTimePrismProgramSimulator<ValueType>::resetToInitial() {
            auto indices = stateGenerator->getInitialStates(stateToIdCallback);
            STORM_LOG_THROW(indices.size() == 1, storm::exceptions::NotSupportedException, "Program must have a unique initial state");
            currentState = idToState[indices[0]];
            return explore();
        }

        template<typename ValueType>
        uint32_t DiscreteTimePrismProgramSimulator<ValueType>::getOrAddStateIndex(generator::CompressedState const& state) {
            uint32_t newIndex = static_cast<uint32_t>(stateToId.size());

            // Check, if the state was already registered.
            std::pair<uint32_t, std::size_t> actualIndexBucketPair = stateToId.findOrAddAndGetBucket(state, newIndex);

            uint32_t actualIndex = actualIndexBucketPair.first;
            if (actualIndex == newIndex) {
                idToState[actualIndex] = state;
            }
            return actualIndex;
        }

        template<typename ValueType>
        void DiscreteTimePrismProgramSimulator<ValueType>::clearStateCaches() {
            idToState.clear();
            stateToId = storm::storage::BitVectorHashMap<uint32_t>(stateGenerator->getStateSize());
        }

        template class DiscreteTimePrismProgramSimulator<double>;
    }
}
