#include "storm/generator/CompressedState.h"

#include "storm/generator/VariableInformation.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/SimpleValuation.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType>
        void unpackStateIntoEvaluator(CompressedState const& state, VariableInformation const& variableInformation, storm::expressions::ExpressionEvaluator<ValueType>& evaluator) {
            for (auto const& locationVariable : variableInformation.locationVariables) {
                if (locationVariable.bitWidth != 0) {
                    evaluator.setIntegerValue(locationVariable.variable, state.getAsInt(locationVariable.bitOffset, locationVariable.bitWidth));
                } else {
                    evaluator.setIntegerValue(locationVariable.variable, 0);
                }
            }
            for (auto const& booleanVariable : variableInformation.booleanVariables) {
                evaluator.setBooleanValue(booleanVariable.variable, state.get(booleanVariable.bitOffset));
            }
            for (auto const& integerVariable : variableInformation.integerVariables) {
                evaluator.setIntegerValue(integerVariable.variable, state.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth) + integerVariable.lowerBound);
            }
        }
        
        storm::expressions::SimpleValuation unpackStateIntoValuation(CompressedState const& state, VariableInformation const& variableInformation, storm::expressions::ExpressionManager const& manager) {
            storm::expressions::SimpleValuation result(manager.getSharedPointer());
            for (auto const& locationVariable : variableInformation.locationVariables) {
                if (locationVariable.bitWidth != 0) {
                    result.setIntegerValue(locationVariable.variable, state.getAsInt(locationVariable.bitOffset, locationVariable.bitWidth));
                } else {
                    result.setIntegerValue(locationVariable.variable, 0);
                }
            }
            for (auto const& booleanVariable : variableInformation.booleanVariables) {
                result.setBooleanValue(booleanVariable.variable, state.get(booleanVariable.bitOffset));
            }
            for (auto const& integerVariable : variableInformation.integerVariables) {
                result.setIntegerValue(integerVariable.variable, state.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth) + integerVariable.lowerBound);
            }
            return result;
        }

        template void unpackStateIntoEvaluator<double>(CompressedState const& state, VariableInformation const& variableInformation, storm::expressions::ExpressionEvaluator<double>& evaluator);
        storm::expressions::SimpleValuation unpackStateIntoValuation(CompressedState const& state, VariableInformation const& variableInformation, storm::expressions::ExpressionManager const& manager);

        CompressedState createOutOfBoundsState(VariableInformation const& varInfo, bool roundTo64Bit) {
            CompressedState result(varInfo.getTotalBitOffset(roundTo64Bit));
            assert(varInfo.hasOutOfBoundsBit());
            result.set(varInfo.getOutOfBoundsBit());
            return result;
        }

#ifdef STORM_HAVE_CARL
        template void unpackStateIntoEvaluator<storm::RationalNumber>(CompressedState const& state, VariableInformation const& variableInformation, storm::expressions::ExpressionEvaluator<storm::RationalNumber>& evaluator);
        template void unpackStateIntoEvaluator<storm::RationalFunction>(CompressedState const& state, VariableInformation const& variableInformation, storm::expressions::ExpressionEvaluator<storm::RationalFunction>& evaluator);
#endif
    }
}
