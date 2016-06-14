//#include "src/generator/JaniNextStateGenerator.h"
//
//#include "src/models/sparse/StateLabeling.h"
//
//#include "src/storage/expressions/SimpleValuation.h"
//
//#include "src/utility/constants.h"
//#include "src/utility/macros.h"
//#include "src/exceptions/InvalidSettingsException.h"
//#include "src/exceptions/WrongFormatException.h"
//
//namespace storm {
//    namespace generator {
//     
//        template<typename ValueType, typename StateType>
//        JaniNextStateGenerator<ValueType, StateType>::JaniNextStateGenerator(storm::jani::Model const& model, NextStateGeneratorOptions const& options) : NextStateGenerator<ValueType, StateType>(options), model(model.substituteConstants()), variableInformation(this->model), evaluator(this->model.getManager()), state(nullptr), comparator() {
//            STORM_LOG_THROW(!this->model.hasDefaultComposition(), storm::exceptions::WrongFormatException, "The explicit next-state generator currently does not support custom system compositions.");
//            STORM_LOG_THROW(!this->options.isBuildAllRewardModelsSet() && this->options.getRewardModelNames().empty(), storm::exceptions::InvalidSettingsException, "The explicit next-state generator currently does not support building reward models.");
//            
//            // If there are terminal states we need to handle, we now need to translate all labels to expressions.
//            if (this->options.hasTerminalStates()) {
//                for (auto const& expressionOrLabelAndBool : this->options.getTerminalStates()) {
//                    if (expressionOrLabelAndBool.first.isExpression()) {
//                        terminalStates.push_back(std::make_pair(expressionOrLabelAndBool.first.getExpression(), expressionOrLabelAndBool.second));
//                    } else {
//                        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Cannot make label terminal for JANI models.");
//                    }
//                }
//            }
//        }
//        
//        template<typename ValueType, typename StateType>
//        uint64_t JaniNextStateGenerator<ValueType, StateType>::getStateSize() const {
//            return variableInformation.getTotalBitOffset(true);
//        }
//        
//        template<typename ValueType, typename StateType>
//        ModelType JaniNextStateGenerator<ValueType, StateType>::getModelType() const {
//            switch (model.getModelType()) {
//                case storm::jani::ModelType::DTMC: return ModelType::DTMC;
//                case storm::jani::ModelType::CTMC: return ModelType::CTMC;
//                case storm::jani::ModelType::MDP: return ModelType::MDP;
//                case storm::jani::ModelType::MA: return ModelType::MA;
//                default:
//                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Invalid model type.");
//            }
//        }
//        
//        template<typename ValueType, typename StateType>
//        bool JaniNextStateGenerator<ValueType, StateType>::isDeterministicModel() const {
//            return model.isDeterministicModel();
//        }
//
//        template<typename ValueType, typename StateType>
//        std::vector<StateType> JaniNextStateGenerator<ValueType, StateType>::getInitialStates(StateToIdCallback const& stateToIdCallback) {
//            // FIXME: This only works for models with exactly one initial state. We should make this more general.
//            CompressedState initialState(variableInformation.getTotalBitOffset());
//            
//            // We need to initialize the values of the variables to their initial value.
//            for (auto const& booleanVariable : variableInformation.booleanVariables) {
//                initialState.set(booleanVariable.bitOffset, booleanVariable.initialValue);
//            }
//            for (auto const& integerVariable : variableInformation.integerVariables) {
//                initialState.setFromInt(integerVariable.bitOffset, integerVariable.bitWidth, static_cast<uint_fast64_t>(integerVariable.initialValue - integerVariable.lowerBound));
//            }
//            
//            // Register initial state and return it.
//            StateType id = stateToIdCallback(initialState);
//            return {id};
//        }
//        
//        template<typename ValueType, typename StateType>
//        void JaniNextStateGenerator<ValueType, StateType>::load(CompressedState const& state) {
//            // Since almost all subsequent operations are based on the evaluator, we load the state into it now.
//            unpackStateIntoEvaluator(state, variableInformation, evaluator);
//            
//            // Also, we need to store a pointer to the state itself, because we need to be able to access it when expanding it.
//            this->state = &state;
//        }
//        
//        template<typename ValueType, typename StateType>
//        bool JaniNextStateGenerator<ValueType, StateType>::satisfies(storm::expressions::Expression const& expression) const {
//            if (expression.isTrue()) {
//                return true;
//            }
//            return evaluator.asBool(expression);
//        }
//    }
//}