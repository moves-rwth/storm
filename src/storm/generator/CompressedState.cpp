#include "storm/generator/CompressedState.h"

#include <boost/algorithm/string/join.hpp>

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

        void extractVariableValues(CompressedState const& state, VariableInformation const& variableInformation, std::vector<int64_t>& locationValues, std::vector<bool>& booleanValues, std::vector<int64_t>& integerValues) {
            for (auto const& locationVariable : variableInformation.locationVariables) {
                if (locationVariable.bitWidth != 0) {
                    locationValues.push_back(state.getAsInt(locationVariable.bitOffset, locationVariable.bitWidth));
                } else {
                    locationValues.push_back(0);
                }
            }
            for (auto const& booleanVariable : variableInformation.booleanVariables) {
                booleanValues.push_back(state.get(booleanVariable.bitOffset));
            }
            for (auto const& integerVariable : variableInformation.integerVariables) {
                integerValues.push_back(state.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth) + integerVariable.lowerBound);
            }
        }
        
        std::string toString(CompressedState const& state, VariableInformation const& variableInformation) {
            std::vector<std::string> assignments;
            for (auto const& locationVariable : variableInformation.locationVariables) {
                assignments.push_back(locationVariable.variable.getName() + "=");
                    assignments.back() += std::to_string(locationVariable.bitWidth == 0 ? 0 : state.getAsInt(locationVariable.bitOffset, locationVariable.bitWidth));
            }
            for (auto const& booleanVariable : variableInformation.booleanVariables) {
                if (!state.get(booleanVariable.bitOffset)) {
                    assignments.push_back("!" + booleanVariable.variable.getName());
                } else {
                    assignments.push_back(booleanVariable.variable.getName());
                }
            }
            for (auto const& integerVariable : variableInformation.integerVariables) {
                assignments.push_back(integerVariable.variable.getName() + "=" + std::to_string(state.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth) + integerVariable.lowerBound));
            }
            return boost::join(assignments, " & ");
        }

        storm::storage::BitVector computeObservabilityMask(VariableInformation const& variableInformation) {
            storm::storage::BitVector result(variableInformation.getTotalBitOffset(true));
            for (auto const& locationVariable : variableInformation.locationVariables) {
                if (locationVariable.observable) {
                    for (uint64_t i = locationVariable.bitOffset; i < locationVariable.bitOffset + locationVariable.bitWidth; ++i) {
                        result.set(i, true);
                    }
                }
            }

            for (auto const& booleanVariable : variableInformation.booleanVariables) {
                if (booleanVariable.observable) {
                    result.set(booleanVariable.bitOffset, true);
                }
            }

            for (auto const& integerVariable : variableInformation.integerVariables) {
                if (integerVariable.observable) {
                    for (uint64_t i = integerVariable.bitOffset; i < integerVariable.bitOffset + integerVariable.bitWidth; ++i) {
                        result.set(i, true);
                    }
                }
            }
            return result;
        }

        uint32_t unpackStateToObservabilityClass(CompressedState const& state, storm::storage::BitVector const& observationVector, std::unordered_map<storm::storage::BitVector,uint32_t>& observabilityMap, storm::storage::BitVector const& mask) {
            STORM_LOG_ASSERT(state.size() == mask.size(), "Mask should be as long as state.");
            storm::storage::BitVector observeClass = state & mask;
            if (observationVector.size() != 0) {
                observeClass.concat(observationVector);
            }

            auto it = observabilityMap.find(observeClass);
            if (it != observabilityMap.end()) {
                return it->second;
            } else {
                uint32_t newClassIndex = observabilityMap.size();
                observabilityMap.emplace(observeClass, newClassIndex);
                return newClassIndex;
            }
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
