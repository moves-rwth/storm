#include "src/generator/CompressedState.h"

#include "src/generator/VariableInformation.h"
#include "src/storage/expressions/ExpressionEvaluator.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType>
        void unpackStateIntoEvaluator(CompressedState const& state, VariableInformation const& variableInformation, storm::expressions::ExpressionEvaluator<ValueType>& evaluator) {
            for (auto const& booleanVariable : variableInformation.booleanVariables) {
                evaluator.setBooleanValue(booleanVariable.variable, state.get(booleanVariable.bitOffset));
            }
            for (auto const& integerVariable : variableInformation.integerVariables) {
                evaluator.setIntegerValue(integerVariable.variable, state.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth) + integerVariable.lowerBound);
            }

        }

        template void unpackStateIntoEvaluator<double>(CompressedState const& state, VariableInformation const& variableInformation, storm::expressions::ExpressionEvaluator<double>& evaluator);
        template void unpackStateIntoEvaluator<storm::RationalFunction>(CompressedState const& state, VariableInformation const& variableInformation, storm::expressions::ExpressionEvaluator<storm::RationalFunction>& evaluator);
    }
}