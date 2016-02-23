#include "src/generator/prism/VariableInformation.h"

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
        
        VariableInformation::VariableInformation(storm::expressions::ExpressionManager const& manager) : manager(manager) {
            // Intentionally left empty.
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