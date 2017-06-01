#include "storm/generator/NextStateGenerator.h"

#include "storm/adapters/CarlAdapter.h"

#include "storm/logic/Formulas.h"

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/SimpleValuation.h"

#include "storm/models/sparse/StateLabeling.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace generator {
                    
        template<typename ValueType, typename StateType>
        NextStateGenerator<ValueType, StateType>::NextStateGenerator(storm::expressions::ExpressionManager const& expressionManager, VariableInformation const& variableInformation, NextStateGeneratorOptions const& options) : options(options), expressionManager(expressionManager.getSharedPointer()), variableInformation(variableInformation), evaluator(nullptr), state(nullptr) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        NextStateGenerator<ValueType, StateType>::NextStateGenerator(storm::expressions::ExpressionManager const& expressionManager, NextStateGeneratorOptions const& options) : options(options), expressionManager(expressionManager.getSharedPointer()), variableInformation(), evaluator(nullptr), state(nullptr) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        NextStateGeneratorOptions const& NextStateGenerator<ValueType, StateType>::getOptions() const {
            return options;
        }
        
        template<typename ValueType, typename StateType>
        uint64_t NextStateGenerator<ValueType, StateType>::getStateSize() const {
            return variableInformation.getTotalBitOffset(true);
        }
        
        template<typename ValueType, typename StateType>
        void NextStateGenerator<ValueType, StateType>::load(CompressedState const& state) {
            // Since almost all subsequent operations are based on the evaluator, we load the state into it now.
            unpackStateIntoEvaluator(state, variableInformation, *evaluator);
            
            // Also, we need to store a pointer to the state itself, because we need to be able to access it when expanding it.
            this->state = &state;
        }
        
        template<typename ValueType, typename StateType>
        bool NextStateGenerator<ValueType, StateType>::satisfies(storm::expressions::Expression const& expression) const {
            if (expression.isTrue()) {
                return true;
            }
            return evaluator->asBool(expression);
        }
        
        template<typename ValueType, typename StateType>
        storm::models::sparse::StateLabeling NextStateGenerator<ValueType, StateType>::label(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices, std::vector<StateType> const& deadlockStateIndices, std::vector<std::pair<std::string, storm::expressions::Expression>> labelsAndExpressions) {
            
            for (auto const& expression : this->options.getExpressionLabels()) {
                std::stringstream stream;
                stream << expression;
                labelsAndExpressions.push_back(std::make_pair(stream.str(), expression));
            }
            
            // Make the labels unique.
            std::sort(labelsAndExpressions.begin(), labelsAndExpressions.end(), [] (std::pair<std::string, storm::expressions::Expression> const& a, std::pair<std::string, storm::expressions::Expression> const& b) { return a.first < b.first; } );
            auto it = std::unique(labelsAndExpressions.begin(), labelsAndExpressions.end(), [] (std::pair<std::string, storm::expressions::Expression> const& a, std::pair<std::string, storm::expressions::Expression> const& b) { return a.first == b.first; } );
            labelsAndExpressions.resize(std::distance(labelsAndExpressions.begin(), it));
            
            // Prepare result.
            storm::models::sparse::StateLabeling result(states.size());
            
            // Initialize labeling.
            for (auto const& label : labelsAndExpressions) {
                result.addLabel(label.first);
            }
            for (auto const& stateIndexPair : states) {
                unpackStateIntoEvaluator(stateIndexPair.first, variableInformation, *this->evaluator);
                
                for (auto const& label : labelsAndExpressions) {
                    // Add label to state, if the corresponding expression is true.
                    if (evaluator->asBool(label.second)) {
                        result.addLabelToState(label.first, stateIndexPair.second);
                    }
                }
            }
            
            if (!result.containsLabel("init")) {
                // Also label the initial state with the special label "init".
                result.addLabel("init");
                for (auto index : initialStateIndices) {
                    result.addLabelToState("init", index);
                }
            }
            if (!result.containsLabel("deadlock")) {
                result.addLabel("deadlock");
                for (auto index : deadlockStateIndices) {
                    result.addLabelToState("deadlock", index);
                }
            }
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        void NextStateGenerator<ValueType, StateType>::postprocess(StateBehavior<ValueType, StateType>& result) {
            // If the model we build is a Markov Automaton, we postprocess the choices to sum all Markovian choices
            // and make the Markovian choice the very first one (if there is any).
            bool foundPreviousMarkovianChoice = false;
            if (this->getModelType() == ModelType::MA) {
                uint64_t numberOfChoicesToDelete = 0;
                
                for (uint_fast64_t index = 0; index + numberOfChoicesToDelete < result.getNumberOfChoices();) {
                    Choice<ValueType>& choice = result.getChoices()[index];
                    
                    if (choice.isMarkovian()) {
                        if (foundPreviousMarkovianChoice) {
                            // If there was a previous Markovian choice, we need to sum them. Note that we can assume
                            // that the previous Markovian choice is the very first one in the choices vector.
                            result.getChoices().front().add(choice);
                            
                            // Swap the choice to the end to indicate it can be removed (if it's not already there).
                            if (index != result.getNumberOfChoices() - 1 - numberOfChoicesToDelete) {
                                choice = std::move(result.getChoices()[result.getNumberOfChoices() - 1 - numberOfChoicesToDelete]);
                            }
                            ++numberOfChoicesToDelete;
                        } else {
                            // If there is no previous Markovian choice, just move the Markovian choice to the front.
                            if (index != 0) {
                                std::swap(result.getChoices().front(), choice);
                            }
                            foundPreviousMarkovianChoice = true;
                            ++index;
                        }
                    } else {
                        ++index;
                    }
                }
                
                // Finally remove the choices that were added to other Markovian choices.
                if (numberOfChoicesToDelete > 0) {
                    result.getChoices().resize(result.getChoices().size() - numberOfChoicesToDelete);
                }
            }
        }
        
        template<typename ValueType, typename StateType>
        storm::expressions::SimpleValuation NextStateGenerator<ValueType, StateType>::toValuation(CompressedState const& state) const {
            return unpackStateIntoValuation(state, variableInformation, *expressionManager);
        }
        
        template<typename ValueType, typename StateType>
        std::shared_ptr<storm::storage::sparse::ChoiceOrigins> NextStateGenerator<ValueType, StateType>::generateChoiceOrigins(std::vector<boost::any>& dataForChoiceOrigins) const {
            STORM_LOG_ERROR_COND(!options.isBuildChoiceOriginsSet(), "Generating choice origins is not supported for the considered model format.");
            return nullptr;
        }

        template class NextStateGenerator<double>;

#ifdef STORM_HAVE_CARL
        template class NextStateGenerator<storm::RationalNumber>;
        template class NextStateGenerator<storm::RationalFunction>;
#endif
    }
}
