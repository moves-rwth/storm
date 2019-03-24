#pragma once

#include "storm/generator/NextStateGenerator.h"
#include "storm/utility/ConstantsComparator.h"

#include "storm-dft/storage/dft/DFT.h"


namespace storm {
    namespace generator {
        
        /*!
         * Next state generator for DFTs.
         */
        template<typename ValueType, typename StateType = uint32_t>
        class DftNextStateGenerator {
            // TODO: inherit from NextStateGenerator

            using DFTStatePointer = std::shared_ptr<storm::storage::DFTState<ValueType>>;
            using DFTElementPointer = std::shared_ptr<storm::storage::DFTElement<ValueType>>;
            using DFTGatePointer = std::shared_ptr<storm::storage::DFTGate<ValueType>>;
            using DFTRestrictionPointer = std::shared_ptr<storm::storage::DFTRestriction<ValueType>>;

        public:
            typedef std::function<StateType (DFTStatePointer const&)> StateToIdCallback;
            
            DftNextStateGenerator(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTStateGenerationInfo const& stateGenerationInfo, bool enableDC, bool mergeFailedStates);
                        
            bool isDeterministicModel() const;
            std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback);

            void load(storm::storage::BitVector const& state);
            void load(DFTStatePointer const& state);
            StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback);

            /*!
             * Create unique failed state.
             *
             * @param stateToIdCallback Callback for state. The callback should just return the id and not use the state.
             *
             * @return Behavior of state.
             */
            StateBehavior<ValueType, StateType> createMergeFailedState(StateToIdCallback const& stateToIdCallback);

        private:

            StateBehavior<ValueType, StateType> exploreState(StateToIdCallback const& stateToIdCallback, bool exploreDependencies);

            // The dft used for the generation of next states.
            storm::storage::DFT<ValueType> const& mDft;

            // General information for the state generation.
            storm::storage::DFTStateGenerationInfo const& mStateGenerationInfo;

            // Current state
            DFTStatePointer state;

            // Flag indicating if dont care propagation is enabled.
            bool enableDC;

            // Flag indication if all failed states should be merged into one.
            bool mergeFailedStates = true;

            // Id of the merged failed state
            StateType mergeFailedStateId = 0;

            // Flag indicating if the model is deterministic.
            bool deterministicModel = false;

        };
        
    }
}
