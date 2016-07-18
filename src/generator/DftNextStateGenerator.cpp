#include "src/generator/DftNextStateGenerator.h"

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType>
        DftNextStateGenerator<ValueType, StateType>::DftNextStateGenerator(storm::storage::DFT<ValueType> const& dft) : mDft(dft), state(nullptr), comparator() {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        bool DftNextStateGenerator<ValueType, StateType>::isDeterministicModel() const {
            assert(false);
            return true;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<StateType> DftNextStateGenerator<ValueType, StateType>::getInitialStates(StateToIdCallback const& stateToIdCallback) {
            // FIXME: This only works for models with exactly one initial state. We should make this more general.
            /*CompressedState initialState(variableInformation.getTotalBitOffset());
            
            // We need to initialize the values of the variables to their initial value.
            for (auto const& booleanVariable : variableInformation.booleanVariables) {
                initialState.set(booleanVariable.bitOffset, booleanVariable.initialValue);
            }
            for (auto const& integerVariable : variableInformation.integerVariables) {
                initialState.setFromInt(integerVariable.bitOffset, integerVariable.bitWidth, static_cast<uint_fast64_t>(integerVariable.initialValue - integerVariable.lowerBound));
            }

            // Register initial state and return it.
            StateType id = stateToIdCallback(initialState);
            return {id};*/
        }
        
        template<typename ValueType, typename StateType>
        void DftNextStateGenerator<ValueType, StateType>::load(std::shared_ptr<storm::storage::DFTState<ValueType>> const& state) {
            /*// Since almost all subsequent operations are based on the evaluator, we load the state into it now.
            unpackStateIntoEvaluator(state, variableInformation, evaluator);
            
            // Also, we need to store a pointer to the state itself, because we need to be able to access it when expanding it.
            this->state = &state;*/
        }
        
        template<typename ValueType, typename StateType>
        bool DftNextStateGenerator<ValueType, StateType>::satisfies(storm::expressions::Expression const& expression) const {
           /* if (expression.isTrue()) {
                return true;
            }
            return evaluator.asBool(expression);*/
        }
        
        template<typename ValueType, typename StateType>
        StateBehavior<ValueType, StateType> DftNextStateGenerator<ValueType, StateType>::expand(StateToIdCallback const& stateToIdCallback) {
            /*// Prepare the result, in case we return early.
            StateBehavior<ValueType, StateType> result;
            
            // First, construct the state rewards, as we may return early if there are no choices later and we already
            // need the state rewards then.
            for (auto const& rewardModel : selectedRewardModels) {
                ValueType stateRewardValue = storm::utility::zero<ValueType>();
                if (rewardModel.get().hasStateRewards()) {
                    for (auto const& stateReward : rewardModel.get().getStateRewards()) {
                        if (evaluator.asBool(stateReward.getStatePredicateExpression())) {
                            stateRewardValue += ValueType(evaluator.asRational(stateReward.getRewardValueExpression()));
                        }
                    }
                }
                result.addStateReward(stateRewardValue);
            }
            
            // If a terminal expression was set and we must not expand this state, return now.
            if (terminalExpression && evaluator.asBool(terminalExpression.get())) {
                return result;
            }
            
            // Get all choices for the state.
            std::vector<Choice<ValueType>> allChoices = getUnlabeledChoices(*this->state, stateToIdCallback);
            std::vector<Choice<ValueType>> allLabeledChoices = getLabeledChoices(*this->state, stateToIdCallback);
            for (auto& choice : allLabeledChoices) {
                allChoices.push_back(std::move(choice));
            }
            
            std::size_t totalNumberOfChoices = allChoices.size();
            
            // If there is not a single choice, we return immediately, because the state has no behavior (other than
            // the state reward).
            if (totalNumberOfChoices == 0) {
                return result;
            }
            
            // If the model is a deterministic model, we need to fuse the choices into one.
            if (program.isDeterministicModel() && totalNumberOfChoices > 1) {
                Choice<ValueType> globalChoice;
                
                // For CTMCs, we need to keep track of the total exit rate to scale the action rewards later. For DTMCs
                // this is equal to the number of choices, which is why we initialize it like this here.
                ValueType totalExitRate = program.isDiscreteTimeModel() ? static_cast<ValueType>(totalNumberOfChoices) : storm::utility::zero<ValueType>();
                
                // Iterate over all choices and combine the probabilities/rates into one choice.
                for (auto const& choice : allChoices) {
                    for (auto const& stateProbabilityPair : choice) {
                        if (program.isDiscreteTimeModel()) {
                            globalChoice.addProbability(stateProbabilityPair.first, stateProbabilityPair.second / totalNumberOfChoices);
                        } else {
                            globalChoice.addProbability(stateProbabilityPair.first, stateProbabilityPair.second);
                        }
                    }
                    
                    if (hasStateActionRewards && !program.isDiscreteTimeModel()) {
                        totalExitRate += choice.getTotalMass();
                    }
                    
                    if (buildChoiceLabeling) {
                        globalChoice.addChoiceLabels(choice.getChoiceLabels());
                    }
                }
                
                // Now construct the state-action reward for all selected reward models.
                for (auto const& rewardModel : selectedRewardModels) {
                    ValueType stateActionRewardValue = storm::utility::zero<ValueType>();
                    if (rewardModel.get().hasStateActionRewards()) {
                        for (auto const& stateActionReward : rewardModel.get().getStateActionRewards()) {
                            for (auto const& choice : allChoices) {
                                if (stateActionReward.getActionIndex() == choice.getActionIndex() && evaluator.asBool(stateActionReward.getStatePredicateExpression())) {
                                    stateActionRewardValue += ValueType(evaluator.asRational(stateActionReward.getRewardValueExpression())) * choice.getTotalMass() / totalExitRate;
                                }
                            }
                            
                        }
                    }
                    globalChoice.addChoiceReward(stateActionRewardValue);
                }
                
                // Move the newly fused choice in place.
                allChoices.clear();
                allChoices.push_back(std::move(globalChoice));
            }
            
            // Move all remaining choices in place.
            for (auto& choice : allChoices) {
                result.addChoice(std::move(choice));
            }
            
            result.setExpanded();
            return result;*/
        }
        
        
        template class DftNextStateGenerator<double>;
        template class DftNextStateGenerator<storm::RationalFunction>;
    }
}