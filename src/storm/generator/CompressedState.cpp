#include "storm/generator/CompressedState.h"

#include <boost/algorithm/string/join.hpp>

#include "storm/adapters/JsonAdapter.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotImplementedException.h"

#include "storm/generator/VariableInformation.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/SimpleValuation.h"

namespace storm {
namespace generator {

template<typename ValueType>
void unpackStateIntoEvaluator(CompressedState const& state, VariableInformation const& variableInformation,
                              storm::expressions::ExpressionEvaluator<ValueType>& evaluator) {
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
        evaluator.setIntegerValue(integerVariable.variable,
                                  static_cast<int_fast64_t>(state.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth)) + integerVariable.lowerBound);
    }
}

storm::expressions::SimpleValuation unpackStateIntoValuation(CompressedState const& state, VariableInformation const& variableInformation,
                                                             storm::expressions::ExpressionManager const& manager) {
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
        result.setIntegerValue(integerVariable.variable,
                               static_cast<int_fast64_t>(state.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth)) + integerVariable.lowerBound);
    }
    return result;
}

CompressedState packStateFromValuation(expressions::SimpleValuation const& valuation, VariableInformation const& variableInformation, bool checkOutOfBounds) {
    CompressedState result(variableInformation.getTotalBitOffset(true));
    STORM_LOG_THROW(variableInformation.locationVariables.size() == 0, storm::exceptions::NotImplementedException, "Support for JANI is not implemented");
    for (auto const& booleanVariable : variableInformation.booleanVariables) {
        result.set(booleanVariable.bitOffset, valuation.getBooleanValue(booleanVariable.variable));
    }
    for (auto const& integerVariable : variableInformation.integerVariables) {
        int64_t assignedValue = valuation.getIntegerValue(integerVariable.variable);
        if (checkOutOfBounds) {
            STORM_LOG_THROW(assignedValue >= integerVariable.lowerBound, storm::exceptions::InvalidArgumentException,
                            "The assignment leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << integerVariable.getName() << "'.");
            STORM_LOG_THROW(assignedValue <= integerVariable.upperBound, storm::exceptions::InvalidArgumentException,
                            "The assignment leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << integerVariable.getName() << "'.");
        }
        result.setFromInt(integerVariable.bitOffset, integerVariable.bitWidth, assignedValue - integerVariable.lowerBound);
        STORM_LOG_ASSERT(
            static_cast<int_fast64_t>(result.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth)) + integerVariable.lowerBound == assignedValue,
            "Writing to the bit vector bucket failed (read " << result.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth) << " but wrote "
                                                             << assignedValue << ").");
    }

    return result;
}

void extractVariableValues(CompressedState const& state, VariableInformation const& variableInformation, std::vector<int64_t>& locationValues,
                           std::vector<bool>& booleanValues, std::vector<int64_t>& integerValues) {
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
        integerValues.push_back(static_cast<int_fast64_t>(state.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth)) + integerVariable.lowerBound);
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
        assignments.push_back(
            integerVariable.variable.getName() + "=" +
            std::to_string(static_cast<int_fast64_t>(state.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth)) + integerVariable.lowerBound));
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

uint32_t unpackStateToObservabilityClass(CompressedState const& state, storm::storage::BitVector const& observationVector,
                                         std::unordered_map<storm::storage::BitVector, uint32_t>& observabilityMap, storm::storage::BitVector const& mask) {
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

template<typename ValueType>
storm::json<ValueType> unpackStateIntoJson(CompressedState const& state, VariableInformation const& variableInformation, bool onlyObservable) {
    storm::json<ValueType> result;
    for (auto const& locationVariable : variableInformation.locationVariables) {
        if (onlyObservable && !locationVariable.observable) {
            continue;
        }
        if (locationVariable.bitWidth != 0) {
            result[locationVariable.variable.getName()] = state.getAsInt(locationVariable.bitOffset, locationVariable.bitWidth);
        } else {
            result[locationVariable.variable.getName()] = 0;
        }
    }
    for (auto const& booleanVariable : variableInformation.booleanVariables) {
        if (onlyObservable && !booleanVariable.observable) {
            continue;
        }
        result[booleanVariable.getName()] = state.get(booleanVariable.bitOffset);
    }
    for (auto const& integerVariable : variableInformation.integerVariables) {
        if (onlyObservable && !integerVariable.observable) {
            continue;
        }
        STORM_LOG_ASSERT(integerVariable.bitWidth <= 63, "Only integer variables with at most 63 bits are supported");
        result[integerVariable.getName()] =
            static_cast<int64_t>(state.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth)) + integerVariable.lowerBound;
    }
    return result;
}

storm::expressions::SimpleValuation unpackStateIntoValuation(CompressedState const& state, VariableInformation const& variableInformation,
                                                             storm::expressions::ExpressionManager const& manager);

CompressedState createOutOfBoundsState(VariableInformation const& varInfo, bool roundTo64Bit) {
    CompressedState result(varInfo.getTotalBitOffset(roundTo64Bit));
    assert(varInfo.hasOutOfBoundsBit());
    result.set(varInfo.getOutOfBoundsBit());
    return result;
}

CompressedState createCompressedState(VariableInformation const& varInfo,
                                      std::map<storm::expressions::Variable, storm::expressions::Expression> const& stateDescription, bool checkOutOfBounds) {
    CompressedState result(varInfo.getTotalBitOffset(true));
    auto boolItEnd = varInfo.booleanVariables.end();

    for (auto boolIt = varInfo.booleanVariables.begin(); boolIt != boolItEnd; ++boolIt) {
        STORM_LOG_THROW(stateDescription.count(boolIt->variable) > 0, storm::exceptions::InvalidArgumentException,
                        "Assignment for Boolean variable " << boolIt->getName() << " missing.");
        result.set(boolIt->bitOffset, stateDescription.at(boolIt->variable).evaluateAsBool());
    }

    // Iterate over all integer assignments and carry them out.
    auto integerItEnd = varInfo.integerVariables.end();
    for (auto integerIt = varInfo.integerVariables.begin(); integerIt != integerItEnd; ++integerIt) {
        STORM_LOG_THROW(stateDescription.count(integerIt->variable) > 0, storm::exceptions::InvalidArgumentException,
                        "Assignment for Integer variable " << integerIt->getName() << " missing.");

        int64_t assignedValue = stateDescription.at(integerIt->variable).evaluateAsInt();
        if (checkOutOfBounds) {
            STORM_LOG_THROW(assignedValue >= integerIt->lowerBound, storm::exceptions::InvalidArgumentException,
                            "The assignment leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << integerIt->getName() << "'.");
            STORM_LOG_THROW(assignedValue <= integerIt->upperBound, storm::exceptions::InvalidArgumentException,
                            "The assignment leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << integerIt->getName() << "'.");
        }
        result.setFromInt(integerIt->bitOffset, integerIt->bitWidth, assignedValue - integerIt->lowerBound);
        STORM_LOG_ASSERT(static_cast<int_fast64_t>(result.getAsInt(integerIt->bitOffset, integerIt->bitWidth)) + integerIt->lowerBound == assignedValue,
                         "Writing to the bit vector bucket failed (read " << result.getAsInt(integerIt->bitOffset, integerIt->bitWidth) << " but wrote "
                                                                          << assignedValue << ").");
    }

    STORM_LOG_THROW(varInfo.locationVariables.size() == 0, storm::exceptions::NotImplementedException, "Support for JANI is not implemented");
    return result;
}

template storm::json<double> unpackStateIntoJson<double>(CompressedState const& state, VariableInformation const& variableInformation, bool onlyObservable);
template void unpackStateIntoEvaluator<double>(CompressedState const& state, VariableInformation const& variableInformation,
                                               storm::expressions::ExpressionEvaluator<double>& evaluator);
#ifdef STORM_HAVE_CARL
template storm::json<storm::RationalNumber> unpackStateIntoJson<storm::RationalNumber>(CompressedState const& state,
                                                                                       VariableInformation const& variableInformation, bool onlyObservable);
template storm::json<storm::RationalFunction> unpackStateIntoJson<storm::RationalFunction>(CompressedState const& state,
                                                                                           VariableInformation const& variableInformation, bool onlyObservable);
template void unpackStateIntoEvaluator<storm::RationalNumber>(CompressedState const& state, VariableInformation const& variableInformation,
                                                              storm::expressions::ExpressionEvaluator<storm::RationalNumber>& evaluator);
template void unpackStateIntoEvaluator<storm::RationalFunction>(CompressedState const& state, VariableInformation const& variableInformation,
                                                                storm::expressions::ExpressionEvaluator<storm::RationalFunction>& evaluator);
#endif
}  // namespace generator
}  // namespace storm
