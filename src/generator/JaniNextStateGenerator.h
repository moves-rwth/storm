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
                        
            virtual storm::models::sparse::StateLabeling label(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices = {}, std::vector<StateType> const& deadlockStateIndices = {}) override;
            
        private:
            /*!
             * Retrieves the location index from the given state.
             */
            uint64_t getLocation(CompressedState const& state, LocationVariableInformation const& locationVariable) const;
            
            /*!
             * Sets the location index from the given state.
             */
            void setLocation(CompressedState& state, LocationVariableInformation const& locationVariable, uint64_t locationIndex) const;
            
            /*!
             * Retrieves the tuple of locations of the given state.
             */
            std::vector<uint64_t> getLocations(CompressedState const& state) const;
            
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
             * @param locations The current locations of all automata.
             * @param state The state for which to retrieve the silent choices.
             * @return The silent action choices of the state.
             */
            std::vector<Choice<ValueType>> getSilentActionChoices(std::vector<uint64_t> const& locations, CompressedState const& state, StateToIdCallback stateToIdCallback);
            
            /*!
             * Retrieves all choices labeled with some non-silent action possible from the given state.
             *
             * @param locations THe current locations of all automata.
             * @param state The state for which to retrieve the non-silent choices.
             * @return The non-silent action choices of the state.
             */
            std::vector<Choice<ValueType>> getNonsilentActionChoices(std::vector<uint64_t> const& locations, CompressedState const& state, StateToIdCallback stateToIdCallback);
            
            /*!
             * Retrieves a list of lists of edges such that the list at index i are all edges of automaton i enabled in 
             * the current state. If the list is empty, it means there was at least one automaton containing edges with
             * the desired action, but none of them were enabled.
             */
            std::vector<std::vector<storm::jani::Edge const*>> getEnabledEdges(std::vector<uint64_t> const& locationIndices, uint64_t actionIndex);
            
            /*!
             * Checks the list of enabled edges (obtained by a call to <code>getEnabledEdges</code>) for multiple
             * synchronized writes to the same global variable.
             */
            void checkGlobalVariableWritesValid(std::vector<std::vector<storm::jani::Edge const*>> const& enabledEdges) const;
            
            /*!
             * Treats the given transient assignments by calling the callback function whenever a transient assignment
             * to one of the reward variables of this generator is performed.
             */
            void performTransientAssignments(storm::jani::detail::ConstAssignments const& transientAssignments, std::function<void (ValueType const&)> const& callback);
            
            /// The model used for the generation of next states.
            storm::jani::Model model;
            
            // The transient variables of reward models that need to be considered.
            std::vector<storm::expressions::Variable> rewardVariables;

        };
        
    }
}