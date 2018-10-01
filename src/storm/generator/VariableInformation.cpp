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
        
        BooleanVariableInformation::BooleanVariableInformation(storm::expressions::Variable const& variable, uint_fast64_t bitOffset, bool global, bool observable) : variable(variable), bitOffset(bitOffset), global(global), observable(observable) {
            // Intentionally left empty.
        }

        IntegerVariableInformation::IntegerVariableInformation(storm::expressions::Variable const& variable, int_fast64_t lowerBound, int_fast64_t upperBound, uint_fast64_t bitOffset, uint_fast64_t bitWidth, bool global, bool observable, bool forceOutOfBoundsCheck) : variable(variable), lowerBound(lowerBound), upperBound(upperBound), bitOffset(bitOffset), bitWidth(bitWidth), global(global), observable(observable), forceOutOfBoundsCheck(forceOutOfBoundsCheck) {
             // Intentionally left empty.
        }
        
        LocationVariableInformation::LocationVariableInformation(storm::expressions::Variable const& variable, uint64_t highestValue, uint_fast64_t bitOffset, uint_fast64_t bitWidth, bool observable) : variable(variable), highestValue(highestValue), bitOffset(bitOffset), bitWidth(bitWidth), observable(observable) {
            // Intentionally left empty.
        }
        
        VariableInformation::VariableInformation(storm::prism::Program const& program, bool outOfBoundsState) : totalBitOffset(0) {
            if (outOfBoundsState) {
                outOfBoundsBit = 0;
                ++totalBitOffset;
            } else {
                outOfBoundsBit = boost::none;
            }

            for (auto const& booleanVariable : program.getGlobalBooleanVariables()) {
                booleanVariables.emplace_back(booleanVariable.getExpressionVariable(), totalBitOffset, true, booleanVariable.isObservable());
                ++totalBitOffset;
            }
            for (auto const& integerVariable : program.getGlobalIntegerVariables()) {
                int_fast64_t lowerBound = integerVariable.getLowerBoundExpression().evaluateAsInt();
                int_fast64_t upperBound = integerVariable.getUpperBoundExpression().evaluateAsInt();
                STORM_LOG_THROW(lowerBound <= upperBound, storm::exceptions::WrongFormatException, "Lower bound must not be above upper bound");
                uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                integerVariables.emplace_back(integerVariable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth, true, integerVariable.isObservable());
                totalBitOffset += bitwidth;
            }
            for (auto const& module : program.getModules()) {
                for (auto const& booleanVariable : module.getBooleanVariables()) {
                    booleanVariables.emplace_back(booleanVariable.getExpressionVariable(), totalBitOffset, false, booleanVariable.isObservable());
                    ++totalBitOffset;
                }
                for (auto const& integerVariable : module.getIntegerVariables()) {
                    int_fast64_t lowerBound = integerVariable.getLowerBoundExpression().evaluateAsInt();
                    int_fast64_t upperBound = integerVariable.getUpperBoundExpression().evaluateAsInt();
                    STORM_LOG_THROW(lowerBound <= upperBound, storm::exceptions::WrongFormatException, "Lower bound must not be above upper bound");
                    uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                    integerVariables.emplace_back(integerVariable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth, false, integerVariable.isObservable());
                    totalBitOffset += bitwidth;
                }
            }
            
            sortVariables();
        }
        
        VariableInformation::VariableInformation(storm::jani::Model const& model, std::vector<std::reference_wrapper<storm::jani::Automaton const>> const& parallelAutomata, uint64_t reservedBitsForUnboundedVariables, bool outOfBoundsState) : totalBitOffset(0) {
            // Check that the model does not contain non-transient real variables.
            STORM_LOG_THROW(!model.getGlobalVariables().containsNonTransientRealVariables(), storm::exceptions::InvalidArgumentException, "Cannot build model from JANI model that contains global non-transient real variables.");
            for (auto const& automaton : model.getAutomata()) {
                STORM_LOG_THROW(!automaton.getVariables().containsNonTransientRealVariables(), storm::exceptions::InvalidArgumentException, "Cannot build model from JANI model that contains non-transient real variables in automaton '" << automaton.getName() << "'.");
            }
//            
//            for (auto const& variable : model.getGlobalVariables().getBooleanVariables()) {
//                if (!variable.isTransient()) {
//                    booleanVariables.emplace_back(variable.getExpressionVariable(), totalBitOffset, true, true);
//                    ++totalBitOffset;
//                }
//            }
//            for (auto const& variable : model.getGlobalVariables().getBoundedIntegerVariables()) {
//                if (!variable.isTransient()) {
//                    int_fast64_t lowerBound = variable.getLowerBound().evaluateAsInt();
//                    int_fast64_t upperBound = variable.getUpperBound().evaluateAsInt();
//                    uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
//                    integerVariables.emplace_back(variable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth, true, true);
//                    totalBitOffset += bitwidth;
//                }
//            }

            if (outOfBoundsState) {
                outOfBoundsBit = 0;
                ++totalBitOffset;
            } else {
                outOfBoundsBit = boost::none;
            }
            
            createVariablesForVariableSet(model.getGlobalVariables(), reservedBitsForUnboundedVariables, true);
            
            for (auto const& automatonRef : parallelAutomata) {
                createVariablesForAutomaton(automatonRef.get(), reservedBitsForUnboundedVariables);
            }
                        
            sortVariables();
        }
        
        void VariableInformation::registerArrayVariableReplacements(storm::jani::ArrayEliminatorData const& arrayEliminatorData) {
            arrayVariableToElementInformations.clear();
            // Find for each replaced array variable the corresponding references in this variable information
            for (auto const& arrayVariable : arrayEliminatorData.eliminatedArrayVariables) {
                if (!arrayVariable->isTransient()) {
                    STORM_LOG_ASSERT(arrayEliminatorData.replacements.count(arrayVariable->getExpressionVariable()) > 0, "No replacement for array variable.");
                    auto const& replacements = arrayEliminatorData.replacements.find(arrayVariable->getExpressionVariable())->second;
                    std::vector<uint64_t> varInfoIndices;
                    for (auto const& replacedVar : replacements) {
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
                    this->arrayVariableToElementInformations.emplace(arrayVariable->getExpressionVariable(), std::move(varInfoIndices));
                }
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
        
        void VariableInformation::createVariablesForAutomaton(storm::jani::Automaton const& automaton, uint64_t reservedBitsForUnboundedVariables) {
            uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(automaton.getNumberOfLocations())));
            locationVariables.emplace_back(automaton.getLocationExpressionVariable(), automaton.getNumberOfLocations() - 1, totalBitOffset, bitwidth, true);
            totalBitOffset += bitwidth;
            
            createVariablesForVariableSet(automaton.getVariables(), reservedBitsForUnboundedVariables, false);
        }
        
        void VariableInformation::createVariablesForVariableSet(storm::jani::VariableSet const& variableSet, uint64_t reservedBitsForUnboundedVariables, bool global) {
            for (auto const& variable : variableSet.getBooleanVariables()) {
                if (!variable.isTransient()) {
                    booleanVariables.emplace_back(variable.getExpressionVariable(), totalBitOffset, global, true);
                    ++totalBitOffset;
                }
            }
            for (auto const& variable : variableSet.getBoundedIntegerVariables()) {
                if (!variable.isTransient()) {
                    int64_t lowerBound;
                    int64_t upperBound;
                    if (variable.hasLowerBound()) {
                        lowerBound = variable.getLowerBound().evaluateAsInt();
                        if (variable.hasUpperBound()) {
                            upperBound = variable.getUpperBound().evaluateAsInt();
                        } else {
                            upperBound = lowerBound + ((1ll << reservedBitsForUnboundedVariables) - 1);
                        }
                    } else {
                        STORM_LOG_THROW(variable.hasUpperBound(), storm::exceptions::WrongFormatException, "Bounded integer variable has neither a lower nor an upper bound.");
                        upperBound = variable.getUpperBound().evaluateAsInt();
                        lowerBound = upperBound - ((1ll << reservedBitsForUnboundedVariables) - 1);
                    }
                    uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                    integerVariables.emplace_back(variable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, bitwidth, global, true, !variable.hasLowerBound() || !variable.hasUpperBound());
                    totalBitOffset += bitwidth;
                }
            }
            for (auto const& variable : variableSet.getUnboundedIntegerVariables()) {
                if (!variable.isTransient()) {
                    int64_t lowerBound = -(1ll << (reservedBitsForUnboundedVariables - 1));
                    int64_t upperBound = (1ll << (reservedBitsForUnboundedVariables - 1)) - 1;
                    integerVariables.emplace_back(variable.getExpressionVariable(), lowerBound, upperBound, totalBitOffset, reservedBitsForUnboundedVariables, global, true, true);
                    totalBitOffset += reservedBitsForUnboundedVariables;
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
