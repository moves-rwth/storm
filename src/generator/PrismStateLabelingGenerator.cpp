#include "src/generator/PrismStateLabelingGenerator.h"

#include "src/generator/CompressedState.h"

#include "src/storage/expressions/ExpressionEvaluator.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType>
        PrismStateLabelingGenerator<ValueType, StateType>::PrismStateLabelingGenerator(storm::prism::Program const& program, VariableInformation const& variableInformation) : program(program), variableInformation(variableInformation) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        storm::models::sparse::StateLabeling PrismStateLabelingGenerator<ValueType, StateType>::generate(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices) {
            std::vector<storm::prism::Label> const& labels = program.getLabels();
            
            storm::expressions::ExpressionEvaluator<ValueType> evaluator(program.getManager());
            storm::models::sparse::StateLabeling result(states.size());
            
            // Initialize labeling.
            for (auto const& label : labels) {
                result.addLabel(label.getName());
            }
            for (auto const& stateIndexPair : states) {
                unpackStateIntoEvaluator(stateIndexPair.first, variableInformation, evaluator);
                
                for (auto const& label : labels) {
                    // Add label to state, if the corresponding expression is true.
                    if (evaluator.asBool(label.getStatePredicateExpression())) {
                        result.addLabelToState(label.getName(), stateIndexPair.second);
                    }
                }
            }
            
            // Also label the initial state with the special label "init".
            result.addLabel("init");
            for (auto index : initialStateIndices) {
                result.addLabelToState("init", index);
            }
            
            return result;
        }
        
        template class PrismStateLabelingGenerator<double, uint32_t>;
        template class PrismStateLabelingGenerator<storm::RationalFunction, uint32_t>;

    }
}