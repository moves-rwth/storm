#pragma once

#include "src/generator/NextStateGenerator.h"

#include "src/storage/jani/Model.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType = uint32_t>
        class JaniNextStateGenerator : public NextStateGenerator<ValueType, StateType> {
        public:
            typedef typename NextStateGenerator<ValueType, StateType>::StateToIdCallback StateToIdCallback;
            
            JaniNextStateGenerator(storm::jani::Model const& model, NextStateGeneratorOptions const& options = NextStateGeneratorOptions());
            
            virtual ModelType getModelType() const override;
            virtual bool isDeterministicModel() const override;
            virtual bool isDiscreteTimeModel() const override;
            virtual std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback) override;
            
            virtual StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback) override;
            
            virtual std::size_t getNumberOfRewardModels() const override;
            virtual RewardModelInformation getRewardModelInformation(uint64_t const& index) const override;
                        
            virtual storm::models::sparse::StateLabeling label(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices = {}) override;
            
        private:
            /*!
             * A delegate constructor that is used to preprocess the model before the constructor of the superclass is
             * being called. The last argument is only present to distinguish the signature of this constructor from the
             * public one.
             */
            JaniNextStateGenerator(storm::jani::Model const& model, NextStateGeneratorOptions const& options, bool flag);
            
            /*!
             * Applies an update to the state currently loaded into the evaluator and applies the resulting values to
             * the given compressed state.
             * @params state The state to which to apply the new values.
             * @params update The update to apply.
             * @return The resulting state.
             */
            CompressedState applyUpdate(CompressedState const& state, storm::jani::EdgeDestination const& update);
            
            /*!
             * Retrieves all choices labeled with the silent action possible from the given state.
             *
             * @param state The state for which to retrieve the silent choices.
             * @return The silent action choices of the state.
             */
            std::vector<Choice<ValueType>> getSilentActionChoices(CompressedState const& state, StateToIdCallback stateToIdCallback);
            
            /*!
             * Retrieves all choices labeled with some non-silent action possible from the given state.
             *
             * @param state The state for which to retrieve the non-silent choices.
             * @return The non-silent action choices of the state.
             */
            std::vector<Choice<ValueType>> getNonsilentActionChoices(CompressedState const& state, StateToIdCallback stateToIdCallback);
            
            // The model used for the generation of next states.
            storm::jani::Model model;
        };
        
    }
}