#ifndef STORM_GENERATOR_PRISMNEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_PRISMNEXTSTATEGENERATOR_H_

#include "src/generator/NextStateGenerator.h"
#include "src/generator/VariableInformation.h"

#include "src/storage/prism/Program.h"
#include "src/storage/expressions/ExpressionEvaluator.h"

#include "src/utility/ConstantsComparator.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType = uint32_t>
        class PrismNextStateGenerator : public NextStateGenerator<ValueType, StateType> {
        public:
            typedef typename NextStateGenerator<ValueType, StateType>::StateToIdCallback StateToIdCallback;
            
            PrismNextStateGenerator(storm::prism::Program const& program, NextStateGeneratorOptions const& options = NextStateGeneratorOptions());
            
            virtual uint64_t getStateSize() const override;
            virtual ModelType getModelType() const override;
            virtual bool isDeterministicModel() const override;
            virtual std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback) override;

            virtual void load(CompressedState const& state) override;
            virtual StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback) override;
            virtual bool satisfies(storm::expressions::Expression const& expression) const override;

            virtual std::size_t getNumberOfRewardModels() const override;
            virtual RewardModelInformation getRewardModelInformation(uint64_t const& index) const override;
            
            virtual storm::expressions::SimpleValuation toValuation(CompressedState const& state) const override;
            
            virtual storm::models::sparse::StateLabeling label(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices = {}) override;

        private:
            /*!
             * Applies an update to the state currently loaded into the evaluator and applies the resulting values to
             * the given compressed state.
             * @params state The state to which to apply the new values.
             * @params update The update to apply.
             * @return The resulting state.
             */
            CompressedState applyUpdate(CompressedState const& state, storm::prism::Update const& update);
            
            /*!
             * Retrieves all commands that are labeled with the given label and enabled in the given state, grouped by
             * modules.
             *
             * This function will iterate over all modules and retrieve all commands that are labeled with the given
             * action and active (i.e. enabled) in the current state. The result is a list of lists of commands in which
             * the inner lists contain all commands of exactly one module. If a module does not have *any* (including
             * disabled) commands, there will not be a list of commands of that module in the result. If, however, the
             * module has a command with a relevant label, but no enabled one, nothing is returned to indicate that there
             * is no legal transition possible.
             *
             * @param The program in which to search for active commands.
             * @param state The current state.
             * @param actionIndex The index of the action label to select.
             * @return A list of lists of active commands or nothing.
             */
            boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> getActiveCommandsByActionIndex(uint_fast64_t const& actionIndex);
            
            /*!
             * Retrieves all unlabeled choices possible from the given state.
             *
             * @param state The state for which to retrieve the unlabeled choices.
             * @return The unlabeled choices of the state.
             */
            std::vector<Choice<ValueType>> getUnlabeledChoices(CompressedState const& state, StateToIdCallback stateToIdCallback);
            
            /*!
             * Retrieves all labeled choices possible from the given state.
             *
             * @param state The state for which to retrieve the unlabeled choices.
             * @return The labeled choices of the state.
             */
            std::vector<Choice<ValueType>> getLabeledChoices(CompressedState const& state, StateToIdCallback stateToIdCallback);
            
            // The program used for the generation of next states.
            storm::prism::Program program;
            
            // The reward models that need to be considered.
            std::vector<std::reference_wrapper<storm::prism::RewardModel const>> rewardModels;
            
            // The expressions that define terminal states.
            std::vector<std::pair<storm::expressions::Expression, bool>> terminalStates;
            
            // A flag that stores whether at least one of the selected reward models has state-action rewards.
            bool hasStateActionRewards;
            
            // Information about how the variables are packed.
            VariableInformation variableInformation;
            
            // An evaluator used to evaluate expressions.
            storm::expressions::ExpressionEvaluator<ValueType> evaluator;
            
            // The currently loaded state.
            CompressedState const* state;
            
            // A comparator used to compare constants.
            storm::utility::ConstantsComparator<ValueType> comparator;
        };
        
    }
}

#endif /* STORM_GENERATOR_PRISMNEXTSTATEGENERATOR_H_ */