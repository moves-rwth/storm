#ifndef STORM_GENERATOR_DFTNEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_DFTNEXTSTATEGENERATOR_H_

#include "src/generator/NextStateGenerator.h"
#include "src/storage/dft/DFT.h"

#include "src/utility/ConstantsComparator.h"

namespace storm {
    namespace generator {
        
        /*!
         * Next state generator for DFTs.
         */
        template<typename ValueType, typename StateType = uint32_t>
        class DftNextStateGenerator : public NextStateGenerator<ValueType, std::shared_ptr<storm::storage::DFTState<ValueType>>, StateType> {

            using DFTStatePointer = std::shared_ptr<storm::storage::DFTState<ValueType>>;
            using DFTElementPointer = std::shared_ptr<storm::storage::DFTElement<ValueType>>;
            using DFTGatePointer = std::shared_ptr<storm::storage::DFTGate<ValueType>>;
            using DFTRestrictionPointer = std::shared_ptr<storm::storage::DFTRestriction<ValueType>>;

        public:
            typedef typename NextStateGenerator<ValueType, DFTStatePointer, StateType>::StateToIdCallback StateToIdCallback;
            
            DftNextStateGenerator(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTStateGenerationInfo const& stateGenerationInfo, bool enableDC, bool mergeFailedStates);
                        
            virtual bool isDeterministicModel() const override;
            virtual std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback) override;

            virtual void load(DFTStatePointer const& state) override;
            void load(CompressedState const& state);
            virtual StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback) override;
            virtual bool satisfies(storm::expressions::Expression const& expression) const override;

            /*!
             * Create unique failed state.
             *
             * @param stateToIdCallback Callback for state. The callback should just return the id and not use the state.
             *
             * @return Behavior of state.
             */
            StateBehavior<ValueType, StateType> createMergeFailedState(StateToIdCallback const& stateToIdCallback);

            /*!
             * Set a new value for the allowed approximation error.
             *
             * @param approximationError Allowed approximation error.
             */
            void setApproximationError(double approximationError);

        private:
            
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
            bool deterministicModel = true;

            // Allowed approximation error.
            double approximationError = 0.0;

            /*!
             * Check if the given state should be skipped for expansion.
             *
             * @param state              State to check for expansion
             * @param rate               Rate of current state
             * @param exitRate           Exit rates of all outgoing transitions
             * @param approximationError Allowed approximation error
             *
             * @return True, if the given state should be skipped.
             */
            bool checkSkipState(DFTStatePointer const& state, ValueType rate, ValueType exitRate, double approximationError);
        };
        
    }
}

#endif /* STORM_GENERATOR_DFTNEXTSTATEGENERATOR_H_ */
