#include "storm/generator/VariableInformation.h"

#include "storm/storage/prism/Program.h"
#include "storm/storage/jani/Model.h"

#include "storm/storage/jani/Automaton.h"
#include "storm/storage/jani/ArrayEliminator.h"
#include "storm/storage/jani/AutomatonComposition.h"
#include "storm/storage/jani/ParallelComposition.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/WrongFormatException.h"

#include <cmath>

namespace storm {
    namespace generator {
        
        BooleanVariableInformation::BooleanVariableInformation(storm::expressions::Variable const& variable, uint_fast64_t bitOffset, bool global) : variable(variable), bitOffset(bitOffset), global(global) {
            // Intentionally left empty.
        }
        
        IntegerVariableInformation::IntegerVariableInformation(storm::expressions::Variable const& variable, int_fast64_t lowerBound, int_fast64_t upperBound, uint_fast64_t bitOffset, uint_fast64_t bitWidth, bool global) : variable(variable), lowerBound(lowerBound), upperBound(upperBound), bitOffset(bitOffset), bitWidth(bitWidth), global(global) {
            // Intentionally left empty.
        }
        
        LocationVariableInformation::LocationVariableInformation(storm::expressions::Variable const& variable, uint64_t highestValue, uint_fast64_t bitOffset, uint_fast64_t bitWidth) : variable(variable), highestValue(highestValue), bitOffset(bitOffset), bitWidth(bitWidth) {
            // Intentionally left empty.
        }
        
        VariableInformation::VariableInformation(storm::prism::Program const& program, bool outOfBoundsState) : totalBitOffset(0) {
            if(outOfBoundsState) {
                outOfBoundsBit = 0;
                ++totalBitOffset;
            } else {
                outOfBoundsBit = boost::none;
            }



            for (auto const& booleanVariable : program.getGlobalBooleanVariables()) {
                booleanVariables.emplace_back(booleanVariable.getExpressionVariable(), totalBitOffset, true);
                ++totalBitOffset;
            }
            for (auto const& integerVariable : program.getGlobalIntegerVariables()) {
                int_fast64_t lowerBound = integerVariable.getLowerBoundExpression().evaluateAsInt();
                int_fast64_t upperBound = integerVariable.getUpperBoundExpression().evaluateAsInt();
                STORM_LOG_THROW(lowerBound <= upperBound, storm::exceptions::WrongFormatException, "Lower bound must not be above upper bound");
                uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                integerVariables.emplace_back(integerVariable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth, true);
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
                    STORM_LOG_THROW(lowerBound <= upperBound, storm::exceptions::WrongFormatException, "Lower bound must not be above upper bound");
                    uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                    integerVariables.emplace_back(integerVariable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth);
                    totalBitOffset += bitwidth;
                }
            }
            
            sortVariables();
        }
        
        VariableInformation::VariableInformation(storm::jani::Model const& model, std::vector<std::reference_wrapper<storm::jani::Automaton const>> const& parallelAutomata, bool outOfBoundsState) : totalBitOffset(0) {
            // Check that the model does not contain non-transient unbounded integer or non-transient real variables.
            STORM_LOG_THROW(!model.getGlobalVariables().containsNonTransientRealVariables(), storm::exceptions::InvalidArgumentException, "Cannot build model from JANI model that contains global non-transient real variables.");
            STORM_LOG_THROW(!model.getGlobalVariables().containsNonTransientUnboundedIntegerVariables(), storm::exceptions::InvalidArgumentException, "Cannot build model from JANI model that contains non-transient unbounded integer variables.");
            for (auto const& automaton : model.getAutomata()) {
                STORM_LOG_THROW(!automaton.getVariables().containsNonTransientUnboundedIntegerVariables(), storm::exceptions::InvalidArgumentException, "Cannot build model from JANI model that contains non-transient unbounded integer variables in automaton '" << automaton.getName() << "'.");
                STORM_LOG_THROW(!automaton.getVariables().containsNonTransientRealVariables(), storm::exceptions::InvalidArgumentException, "Cannot build model from JANI model that contains non-transient real variables in automaton '" << automaton.getName() << "'.");
            }
            if(outOfBoundsState) {
                outOfBoundsBit = 0;
                ++totalBitOffset;
            } else {
                outOfBoundsBit = boost::none;
            }


            
            for (auto const& variable : model.getGlobalVariables().getBooleanVariables()) {
                if (!variable.isTransient()) {
                    booleanVariables.emplace_back(variable.getExpressionVariable(), totalBitOffset, true);
                    ++totalBitOffset;
                }
            }
            for (auto const& variable : model.getGlobalVariables().getBoundedIntegerVariables()) {
                if (!variable.isTransient()) {
                    int_fast64_t lowerBound = variable.getLowerBound().evaluateAsInt();
                    int_fast64_t upperBound = variable.getUpperBound().evaluateAsInt();
                    uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                    integerVariables.emplace_back(variable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth, true);
                    totalBitOffset += bitwidth;
                }
            }
            
            for (auto const& automatonRef : parallelAutomata) {
                createVariablesForAutomaton(automatonRef.get());
            }
                        
            sortVariables();
        }
        
        void VariableInformation::registerArrayVariableReplacements(storm::jani::ArrayEliminatorData const& arrayEliminatorData) {
            arrayVariableToElementInformations.clear();
            // Find for each replaced array variable the corresponding references in this variable information
            for (auto const& arrayReplacements : arrayEliminatorData.replacements) {
                std::vector<uint64_t> varInfoIndices;
                for (auto const& replacedVar : arrayReplacements.second) {
                    if (replacedVar->getExpressionVariable().hasIntegerType()) {
                        uint64_t index = 0;
                        for (auto const& intInfo : integerVariables) {
                            if (intInfo.variable == replacedVar->getExpressionVariable()) {
                                varInfoIndices.push_back(index);
                                break;
                            }
                            ++index;
                        }
                        STORM_LOG_ASSERT(!varInfoIndices.empty() && varInfoIndices.back() == index, "Could not find a basic variable for replacement of array variable " << replacedVar->getExpressionVariable().getName() << " .");
                    } else if (replacedVar->getExpressionVariable().hasBooleanType()) {
                        uint64_t index = 0;
                        for (auto const& boolInfo : booleanVariables) {
                            if (boolInfo.variable == replacedVar->getExpressionVariable()) {
                                varInfoIndices.push_back(index);
                                break;
                            }
                            ++index;
                        }
                        STORM_LOG_ASSERT(!varInfoIndices.empty() && varInfoIndices.back() == index, "Could not find a basic variable for replacement of array variable " << replacedVar->getExpressionVariable().getName() << " .");
                    } else {
                        STORM_LOG_ASSERT(false, "Unhandled type of base variable.");
                    }
                }
                this->arrayVariableToElementInformations.emplace(arrayReplacements.first, std::move(varInfoIndices));
            }
        }
        
        BooleanVariableInformation const& VariableInformation::getBooleanArrayVariableReplacement(storm::expressions::Variable const& arrayVariable, uint64_t arrayIndex) {
            std::vector<uint64_t> const& boolInfoIndices = arrayVariableToElementInformations.at(arrayVariable);
            STORM_LOG_THROW(arrayIndex < boolInfoIndices.size(), storm::exceptions::WrongFormatException, "Array access at array " << arrayVariable.getName() << " evaluates to array index " << arrayIndex << " which is out of bounds as the array size is " << boolInfoIndices.size());
            return booleanVariables[boolInfoIndices[arrayIndex]];
        }
        
        IntegerVariableInformation const& VariableInformation::getIntegerArrayVariableReplacement(storm::expressions::Variable const& arrayVariable, uint64_t arrayIndex) {
            std::vector<uint64_t> const& intInfoIndices = arrayVariableToElementInformations.at(arrayVariable);
            STORM_LOG_THROW(arrayIndex < intInfoIndices.size(), storm::exceptions::WrongFormatException, "Array access at array " << arrayVariable.getName() << " evaluates to array index " << arrayIndex << " which is out of bounds as the array size is " << intInfoIndices.size());
            return integerVariables[intInfoIndices[arrayIndex]];
        }
        
        void VariableInformation::createVariablesForAutomaton(storm::jani::Automaton const& automaton) {
            uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(automaton.getNumberOfLocations())));
            locationVariables.emplace_back(automaton.getLocationExpressionVariable(), automaton.getNumberOfLocations() - 1, totalBitOffset, bitwidth);
            totalBitOffset += bitwidth;
            
            for (auto const& variable : automaton.getVariables().getBooleanVariables()) {
                if (!variable.isTransient()) {
                    booleanVariables.emplace_back(variable.getExpressionVariable(), totalBitOffset);
                    ++totalBitOffset;
                }
            }
            for (auto const& variable : automaton.getVariables().getBoundedIntegerVariables()) {
                if (!variable.isTransient()) {
                    int_fast64_t lowerBound = variable.getLowerBound().evaluateAsInt();
                    int_fast64_t upperBound = variable.getUpperBound().evaluateAsInt();
                    uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                    integerVariables.emplace_back(variable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth);
                    totalBitOffset += bitwidth;
                }
            }
        }
        
        uint_fast64_t VariableInformation::getTotalBitOffset(bool roundTo64Bit) const {
            uint_fast64_t result = totalBitOffset;
            if (roundTo64Bit & ((result & ((1ull << 6) - 1)) != 0)) {
                result = ((result >> 6) + 1) << 6;
            }
            return result;
        }

        bool VariableInformation::hasOutOfBoundsBit() const {
            return outOfBoundsBit != boost::none;
        }

        uint64_t VariableInformation::getOutOfBoundsBit() const {
            assert(hasOutOfBoundsBit());
            return outOfBoundsBit.get();
        }

        
        void VariableInformation::sortVariables() {
            // Sort the variables so we can make some assumptions when iterating over them (in the next-state generators).
            std::sort(booleanVariables.begin(), booleanVariables.end(), [] (BooleanVariableInformation const& a, BooleanVariableInformation const& b) { return a.variable < b.variable; } );
            std::sort(integerVariables.begin(), integerVariables.end(), [] (IntegerVariableInformation const& a, IntegerVariableInformation const& b) { return a.variable < b.variable; });
        }
    }
}
