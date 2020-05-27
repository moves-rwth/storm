#include "storm/storage/sparse/StateValuations.h"

#include "storm/storage/BitVector.h"

#include "storm/utility/vector.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidTypeException.h"

namespace storm {
    namespace storage {
        namespace sparse {
            
            StateValuations::StateValuation::StateValuation(std::vector<bool>&& booleanValues, std::vector<int64_t>&& integerValues, std::vector<storm::RationalNumber>&& rationalValues) : booleanValues(std::move(booleanValues)), integerValues(std::move(integerValues)), rationalValues(std::move(rationalValues)) {
                // Intentionally left empty
            }
            
            typename StateValuations::StateValuation const& StateValuations::getValuation(storm::storage::sparse::state_type const& stateIndex) const {
                STORM_LOG_ASSERT(stateIndex < valuations.size(), "Invalid state index.");
                STORM_LOG_ASSERT(assertValuation(valuations[stateIndex]), "Invalid  state valuations");
                return valuations[stateIndex];
            }
            
            bool StateValuations::getBooleanValue(storm::storage::sparse::state_type const& stateIndex, storm::expressions::Variable const& booleanVariable) const {
                auto const& valuation = getValuation(stateIndex);
                STORM_LOG_ASSERT(variableToIndexMap.count(booleanVariable) > 0, "Variable " << booleanVariable.getName() << " is not part of this valuation.");
                return valuation.booleanValues[variableToIndexMap.at(booleanVariable)];
            }
            
            int64_t const& StateValuations::getIntegerValue(storm::storage::sparse::state_type const& stateIndex, storm::expressions::Variable const& integerVariable) const {
                auto const& valuation = getValuation(stateIndex);
                STORM_LOG_ASSERT(variableToIndexMap.count(integerVariable) > 0, "Variable " << integerVariable.getName() << " is not part of this valuation.");
                return valuation.integerValues[variableToIndexMap.at(integerVariable)];
            }
            
            storm::RationalNumber const& StateValuations::getRationalValue(storm::storage::sparse::state_type const& stateIndex, storm::expressions::Variable const& rationalVariable) const {
                auto const& valuation = getValuation(stateIndex);
                STORM_LOG_ASSERT(variableToIndexMap.count(rationalVariable) > 0, "Variable " << rationalVariable.getName() << " is not part of this valuation.");
                return valuation.rationalValues[variableToIndexMap.at(rationalVariable)];
            }
            
            bool StateValuations::isEmpty(storm::storage::sparse::state_type const& stateIndex) const {
                auto const& valuation = getValuation(stateIndex);
                return valuation.booleanValues.empty() && valuation.integerValues.empty() && valuation.rationalValues.empty();
            }
            
            std::string StateValuations::toString(storm::storage::sparse::state_type const& stateIndex, bool pretty, boost::optional<std::set<storm::expressions::Variable>> const& selectedVariables) const {
                auto const& valuation = getValuation(stateIndex);
                typename std::map<storm::expressions::Variable, uint64_t>::const_iterator mapIt = variableToIndexMap.begin();
                typename std::set<storm::expressions::Variable>::const_iterator setIt;
                if (selectedVariables) {
                    setIt = selectedVariables->begin();
                }
                std::vector<std::string> assignments;
                while (mapIt != variableToIndexMap.end() && (!selectedVariables || setIt != selectedVariables->end())) {
                    
                    // Move Map iterator to next relevant position
                    if (selectedVariables) {
                        while (mapIt->first != *setIt) {
                            ++mapIt;
                            STORM_LOG_ASSERT(mapIt != variableToIndexMap.end(), "Valuation does not consider selected variable " << setIt->getName() << ".");
                        }
                    }
                    
                    auto const& variable = mapIt->first;
                    std::stringstream stream;
                    if (pretty) {
                        if (variable.hasBooleanType() && !valuation.booleanValues[mapIt->second]) {
                            stream << "!";
                        }
                        stream << variable.getName();
                        if (variable.hasIntegerType()) {
                            stream << "=" << valuation.integerValues[mapIt->second];
                        } else if (variable.hasRationalType()) {
                            stream << "=" << valuation.rationalValues[mapIt->second];
                        } else {
                            STORM_LOG_THROW(variable.hasBooleanType(), storm::exceptions::InvalidTypeException, "Unexpected variable type.");
                        }
                    } else {
                        if (variable.hasBooleanType()) {
                            stream << std::boolalpha << valuation.booleanValues[mapIt->second] << std::noboolalpha;
                        } else if (variable.hasIntegerType()) {
                            stream << valuation.integerValues[mapIt->second];
                        } else if (variable.hasRationalType()) {
                            stream << valuation.rationalValues[mapIt->second];
                        }
                    }
                    assignments.push_back(stream.str());
                    
                    // Go to next position
                    if (selectedVariables) {
                        ++setIt;
                    } else {
                        ++mapIt;
                    }
                }
                if (pretty) {
                    return "[" + boost::join(assignments, "\t& ") + "]";
                } else {
                    return "[" + boost::join(assignments, "\t") + "]";
                }
            }
            
            typename StateValuations::Json StateValuations::toJson(storm::storage::sparse::state_type const& stateIndex, boost::optional<std::set<storm::expressions::Variable>> const& selectedVariables) const {
                auto const& valuation = getValuation(stateIndex);
                typename std::map<storm::expressions::Variable, uint64_t>::const_iterator mapIt = variableToIndexMap.begin();
                typename std::set<storm::expressions::Variable>::const_iterator setIt;
                if (selectedVariables) {
                    setIt = selectedVariables->begin();
                }
                Json result;
                while (mapIt != variableToIndexMap.end() && (!selectedVariables || setIt != selectedVariables->end())) {
                    // Move Map iterator to next relevant position
                    if (selectedVariables) {
                        while (mapIt->first != *setIt) {
                            ++mapIt;
                            STORM_LOG_ASSERT(mapIt != variableToIndexMap.end(), "Valuation does not consider selected variable " << setIt->getName() << ".");
                        }
                    }
                    
                    auto const& variable = mapIt->first;
                    if (variable.hasBooleanType()) {
                        result[variable.getName()] = bool(valuation.booleanValues[mapIt->second]);
                    } else if (variable.hasIntegerType()) {
                        result[variable.getName()] = valuation.integerValues[mapIt->second];
                    } else if (variable.hasRationalType()) {
                        result[variable.getName()] = valuation.rationalValues[mapIt->second];
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unexpected variable type.");
                    }
                
                    // Go to next position
                    if (selectedVariables) {
                        ++setIt;
                    } else {
                        ++mapIt;
                    }
                }
                return result;
            }
            
            bool StateValuations::assertValuation(StateValuation const& valuation) const {
                storm::storage::BitVector foundBooleanValues(valuation.booleanValues.size(), false);
                storm::storage::BitVector foundIntegerValues(valuation.integerValues.size(), false);
                storm::storage::BitVector foundRationalValues(valuation.rationalValues.size(), false);
                for (auto const& varIndex : variableToIndexMap) {
                    storm::storage::BitVector* bv;
                    if (varIndex.first.hasBooleanType()) {
                        bv = &foundBooleanValues;
                    } else if (varIndex.first.hasIntegerType()) {
                        bv = &foundIntegerValues;
                    } else if (varIndex.first.hasRationalType()) {
                        bv = &foundRationalValues;
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unexpected variable type.");
                    }
                    if (varIndex.second < bv->size()) {
                        if (bv->get(varIndex.second)) {
                            STORM_LOG_ERROR("Multiple variables refer to the same value index");
                            return false;
                        }
                        bv->set(varIndex.second, true);
                    } else {
                        STORM_LOG_ERROR("Valuation does not provide a value for all variables");
                        return false;
                    }
                }
                if (!(foundBooleanValues.full() && foundIntegerValues.full() && foundRationalValues.full())) {
                    STORM_LOG_ERROR("Valuation has too many entries.");
                }
                return true;
            }
            
            StateValuations::StateValuations(std::map<storm::expressions::Variable, uint64_t> const& variableToIndexMap, std::vector<StateValuation>&& valuations) : variableToIndexMap(variableToIndexMap), valuations(valuations) {
                // Intentionally left empty
            }
            
            std::string StateValuations::getStateInfo(state_type const& state) const {
                STORM_LOG_ASSERT(state < getNumberOfStates(), "Invalid state index.");
                return this->toString(state);
            }
            
            uint_fast64_t StateValuations::getNumberOfStates() const {
                return valuations.size();
            }

            std::size_t StateValuations::hash() const {
                return 0;
            }
            
            StateValuations StateValuations::selectStates(storm::storage::BitVector const& selectedStates) const {
                return StateValuations(variableToIndexMap, storm::utility::vector::filterVector(valuations, selectedStates));
            }

            StateValuations StateValuations::selectStates(std::vector<storm::storage::sparse::state_type> const& selectedStates) const {
                std::vector<StateValuation> selectedValuations;
                selectedValuations.reserve(selectedStates.size());
                for (auto const& selectedState : selectedStates){
                    if (selectedState < valuations.size()) {
                        selectedValuations.push_back(valuations[selectedState]);
                    } else {
                        selectedValuations.emplace_back();
                    }
                }
                return StateValuations(variableToIndexMap, std::move(selectedValuations));
            }
            
            StateValuationsBuilder::StateValuationsBuilder() : booleanVarCount(0), integerVarCount(0), rationalVarCount(0) {
                // Intentionally left empty.
            }
            
            void StateValuationsBuilder::addVariable(storm::expressions::Variable const& variable) {
                STORM_LOG_ASSERT(currentStateValuations.valuations.empty(), "Tried to add a variable, although a state has already been added before.");
                STORM_LOG_ASSERT(currentStateValuations.variableToIndexMap.count(variable) == 0, "Variable " << variable.getName() << " already added.");
                if (variable.hasBooleanType()) {
                    currentStateValuations.variableToIndexMap[variable] = booleanVarCount++;
                }
                if (variable.hasIntegerType()) {
                    currentStateValuations.variableToIndexMap[variable] = integerVarCount++;
                }
                if (variable.hasRationalType()) {
                    currentStateValuations.variableToIndexMap[variable] = rationalVarCount++;
                }
            }
            
            void StateValuationsBuilder::addState(storm::storage::sparse::state_type const& state, std::vector<bool>&& booleanValues, std::vector<int64_t>&& integerValues, std::vector<storm::RationalNumber>&& rationalValues) {
                if (state > currentStateValuations.valuations.size()) {
                    currentStateValuations.valuations.resize(state);
                }
                if (state == currentStateValuations.valuations.size()) {
                    currentStateValuations.valuations.emplace_back(std::move(booleanValues), std::move(integerValues), std::move(rationalValues));
                } else {
                    STORM_LOG_ASSERT(currentStateValuations.isEmpty(state), "Adding a valuation to the same state multiple times.");
                    currentStateValuations.valuations[state] = typename StateValuations::StateValuation(std::move(booleanValues), std::move(integerValues), std::move(rationalValues));
                }
            }
            
            StateValuations StateValuationsBuilder::build(std::size_t totalStateCount) {
                return std::move(currentStateValuations);
                booleanVarCount = 0;
                integerVarCount = 0;
                rationalVarCount = 0;
            }
        }
    }
}
