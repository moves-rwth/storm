#include "src/generator/VariableInformation.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace generator {
        
        BooleanVariableInformation::BooleanVariableInformation(storm::expressions::Variable const& variable, bool initialValue, uint_fast64_t bitOffset) : variable(variable), initialValue(initialValue), bitOffset(bitOffset) {
            // Intentionally left empty.
        }
        
        IntegerVariableInformation::IntegerVariableInformation(storm::expressions::Variable const& variable, int_fast64_t initialValue, int_fast64_t lowerBound, int_fast64_t upperBound, uint_fast64_t bitOffset, uint_fast64_t bitWidth) : variable(variable), initialValue(initialValue), lowerBound(lowerBound), upperBound(upperBound), bitOffset(bitOffset), bitWidth(bitWidth) {
            // Intentionally left empty.
        }
        
        VariableInformation::VariableInformation(storm::prism::Program const& program) : totalBitOffset(0) {
            for (auto const& booleanVariable : program.getGlobalBooleanVariables()) {
                booleanVariables.emplace_back(booleanVariable.getExpressionVariable(), booleanVariable.getInitialValueExpression().evaluateAsBool(), totalBitOffset);
                ++totalBitOffset;
                booleanVariableToIndexMap[booleanVariable.getExpressionVariable()] = booleanVariables.size() - 1;
            }
            for (auto const& integerVariable : program.getGlobalIntegerVariables()) {
                int_fast64_t lowerBound = integerVariable.getLowerBoundExpression().evaluateAsInt();
                int_fast64_t upperBound = integerVariable.getUpperBoundExpression().evaluateAsInt();
                uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                integerVariables.emplace_back(integerVariable.getExpressionVariable(), integerVariable.getInitialValueExpression().evaluateAsInt(), lowerBound, upperBound, totalBitOffset, bitwidth);
                totalBitOffset += bitwidth;
                integerVariableToIndexMap[integerVariable.getExpressionVariable()] = integerVariables.size() - 1;
            }
            for (auto const& module : program.getModules()) {
                for (auto const& booleanVariable : module.getBooleanVariables()) {
                    booleanVariables.emplace_back(booleanVariable.getExpressionVariable(), booleanVariable.getInitialValueExpression().evaluateAsBool(), totalBitOffset);
                    ++totalBitOffset;
                    booleanVariableToIndexMap[booleanVariable.getExpressionVariable()] = booleanVariables.size() - 1;
                }
                for (auto const& integerVariable : module.getIntegerVariables()) {
                    int_fast64_t lowerBound = integerVariable.getLowerBoundExpression().evaluateAsInt();
                    int_fast64_t upperBound = integerVariable.getUpperBoundExpression().evaluateAsInt();
                    uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                    integerVariables.emplace_back(integerVariable.getExpressionVariable(), integerVariable.getInitialValueExpression().evaluateAsInt(), lowerBound, upperBound, totalBitOffset, bitwidth);
                    totalBitOffset += bitwidth;
                    integerVariableToIndexMap[integerVariable.getExpressionVariable()] = integerVariables.size() - 1;
                }
            }
        }
        
        uint_fast64_t VariableInformation::getTotalBitOffset() const {
            return totalBitOffset;
        }
        
        uint_fast64_t VariableInformation::getBitOffset(storm::expressions::Variable const& variable) const {
            auto const& booleanIndex = booleanVariableToIndexMap.find(variable);
            if (booleanIndex != booleanVariableToIndexMap.end()) {
                return booleanVariables[booleanIndex->second].bitOffset;
            }
            auto const& integerIndex = integerVariableToIndexMap.find(variable);
            if (integerIndex != integerVariableToIndexMap.end()) {
                return integerVariables[integerIndex->second].bitOffset;
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot look-up bit index of unknown variable.");
        }
        
        uint_fast64_t VariableInformation::getBitWidth(storm::expressions::Variable const& variable) const {
            auto const& integerIndex = integerVariableToIndexMap.find(variable);
            if (integerIndex != integerVariableToIndexMap.end()) {
                return integerVariables[integerIndex->second].bitWidth;
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot look-up bit width of unknown variable.");
        }
        
    }
}