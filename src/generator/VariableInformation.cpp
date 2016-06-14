#include "src/generator/VariableInformation.h"

#include "src/storage/prism/Program.h"
#include "src/storage/jani/Model.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace generator {
        
        BooleanVariableInformation::BooleanVariableInformation(storm::expressions::Variable const& variable, uint_fast64_t bitOffset) : variable(variable), bitOffset(bitOffset) {
            // Intentionally left empty.
        }
        
        IntegerVariableInformation::IntegerVariableInformation(storm::expressions::Variable const& variable, int_fast64_t lowerBound, int_fast64_t upperBound, uint_fast64_t bitOffset, uint_fast64_t bitWidth) : variable(variable), lowerBound(lowerBound), upperBound(upperBound), bitOffset(bitOffset), bitWidth(bitWidth) {
            // Intentionally left empty.
        }
        
        VariableInformation::VariableInformation(storm::prism::Program const& program) : totalBitOffset(0) {
            for (auto const& booleanVariable : program.getGlobalBooleanVariables()) {
                booleanVariables.emplace_back(booleanVariable.getExpressionVariable(), totalBitOffset);
                ++totalBitOffset;
            }
            for (auto const& integerVariable : program.getGlobalIntegerVariables()) {
                int_fast64_t lowerBound = integerVariable.getLowerBoundExpression().evaluateAsInt();
                int_fast64_t upperBound = integerVariable.getUpperBoundExpression().evaluateAsInt();
                uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                integerVariables.emplace_back(integerVariable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth);
                totalBitOffset += bitwidth;
            }
            for (auto const& module : program.getModules()) {
                for (auto const& booleanVariable : module.getBooleanVariables()) {
                    booleanVariables.emplace_back(booleanVariable.getExpressionVariable(), totalBitOffset);
                    ++totalBitOffset;
                }
                for (auto const& integerVariable : module.getIntegerVariables()) {
                    int_fast64_t lowerBound = integerVariable.getLowerBoundExpression().evaluateAsInt();
                    int_fast64_t upperBound = integerVariable.getUpperBoundExpression().evaluateAsInt();
                    uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                    integerVariables.emplace_back(integerVariable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth);
                    totalBitOffset += bitwidth;
                }
            }

            sortVariables();
        }
        
        VariableInformation::VariableInformation(storm::jani::Model const& model) {
            for (auto const& variable : model.getGlobalVariables().getBooleanVariables()) {
                booleanVariables.emplace_back(variable.getExpressionVariable(), totalBitOffset);
                ++totalBitOffset;
            }
            for (auto const& variable : model.getGlobalVariables().getBoundedIntegerVariables()) {
                int_fast64_t lowerBound = variable.getLowerBound().evaluateAsInt();
                int_fast64_t upperBound = variable.getUpperBound().evaluateAsInt();
                uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                integerVariables.emplace_back(variable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth);
                totalBitOffset += bitwidth;
            }
            for (auto const& automaton : model.getAutomata()) {
                for (auto const& variable : automaton.getVariables().getBooleanVariables()) {
                    booleanVariables.emplace_back(variable.getExpressionVariable(), totalBitOffset);
                    ++totalBitOffset;
                }
                for (auto const& variable : automaton.getVariables().getBoundedIntegerVariables()) {
                    int_fast64_t lowerBound = variable.getLowerBound().evaluateAsInt();
                    int_fast64_t upperBound = variable.getUpperBound().evaluateAsInt();
                    uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                    integerVariables.emplace_back(variable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth);
                    totalBitOffset += bitwidth;
                }
            }
            
            sortVariables();
        }
        
        uint_fast64_t VariableInformation::getTotalBitOffset(bool roundTo64Bit) const {
            uint_fast64_t result = totalBitOffset;
            if (roundTo64Bit) {
                result = ((result >> 6) + 1) << 6;
            }
            return result;
        }
        
        void VariableInformation::sortVariables() {
            // Sort the variables so we can make some assumptions when iterating over them (in the next-state generators).
            std::sort(booleanVariables.begin(), booleanVariables.end(), [] (BooleanVariableInformation const& a, BooleanVariableInformation const& b) { return a.variable < b.variable; } );
            std::sort(integerVariables.begin(), integerVariables.end(), [] (IntegerVariableInformation const& a, IntegerVariableInformation const& b) { return a.variable < b.variable; });
        }
    }
}