#include "storm/storage/sparse/StateValuations.h"

#include "storm/storage/BitVector.h"

#include "storm/utility/vector.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidTypeException.h"

namespace storm {
    namespace storage {
        namespace sparse {
            
            StateValuation::StateValuation(std::vector<bool>&& booleanValues, std::vector<int64_t>&& integerValues, std::vector<storm::RationalNumber>&& rationalValues) : booleanValues(std::move(booleanValues)), integerValues(std::move(integerValues)), rationalValues(std::move(rationalValues)) {
                // Intentionally left empty
            }
            
            bool StateValuation::getBooleanValue(StateValuations const& valuations, storm::expressions::Variable const& booleanVariable) const {
                STORM_LOG_ASSERT(assertValuations(valuations), "Invalid  state valuations");
                STORM_LOG_ASSERT(valuations.variableToIndexMap.count(booleanVariable) > 0, "Variable " << booleanVariable.getName() << " is not part of this valuation.");
                return booleanValues[valuations.variableToIndexMap.at(booleanVariable)];
            }
            
            int64_t const& StateValuation::getIntegerValue(StateValuations const& valuations, storm::expressions::Variable const& integerVariable) const {
                STORM_LOG_ASSERT(assertValuations(valuations), "Invalid  state valuations");
                STORM_LOG_ASSERT(valuations.variableToIndexMap.count(integerVariable) > 0, "Variable " << integerVariable.getName() << " is not part of this valuation.");
                return integerValues[valuations.variableToIndexMap.at(integerVariable)];
            }
            
            storm::RationalNumber const& StateValuation::getRationalValue(StateValuations const& valuations, storm::expressions::Variable const& rationalVariable) const {
                STORM_LOG_ASSERT(assertValuations(valuations), "Invalid  state valuations");
                STORM_LOG_ASSERT(valuations.variableToIndexMap.count(rationalVariable) > 0, "Variable " << rationalVariable.getName() << " is not part of this valuation.");
                return rationalValues[valuations.variableToIndexMap.at(rationalVariable)];
            }
            
            bool StateValuation::isEmpty() const {
                return booleanValues.empty() && integerValues.empty() && rationalValues.empty();
            }
            
            std::string StateValuation::toString(StateValuations const& valuations, bool pretty, boost::optional<std::set<storm::expressions::Variable>> const& selectedVariables) const {
                STORM_LOG_ASSERT(assertValuations(valuations), "Invalid  state valuations");
                typename std::map<storm::expressions::Variable, uint64_t>::const_iterator mapIt = valuations.variableToIndexMap.begin();
                typename std::set<storm::expressions::Variable>::const_iterator setIt;
                if (selectedVariables) {
                    setIt = selectedVariables->begin();
                }
                std::vector<std::string> assignments;
                while (mapIt != valuations.variableToIndexMap.end() && (!selectedVariables || setIt != selectedVariables->end())) {
                    
                    // Move Map iterator to next relevant position
                    if (selectedVariables) {
                        while (mapIt->first != *setIt) {
                            ++mapIt;
                            STORM_LOG_ASSERT(mapIt != valuations.variableToIndexMap.end(), "Valuation does not consider selected variable " << setIt->getName() << ".");
                        }
                    }
                    
                    auto const& variable = mapIt->first;
                    std::stringstream stream;
                    if (pretty) {
                        if (variable.hasBooleanType() && !booleanValues[mapIt->second]) {
                            stream << "!";
                        }
                        stream << variable.getName();
                        if (variable.hasIntegerType()) {
                            stream << "=" << integerValues[mapIt->second];
                        } else if (variable.hasRationalType()) {
                            stream << "=" << rationalValues[mapIt->second];
                        } else {
                            STORM_LOG_THROW(variable.hasBooleanType(), storm::exceptions::InvalidTypeException, "Unexpected variable type.");
                        }
                    } else {
                        if (variable.hasBooleanType()) {
                            stream << std::boolalpha << booleanValues[mapIt->second] << std::noboolalpha;
                        } else if (variable.hasIntegerType()) {
                            stream << integerValues[mapIt->second];
                        } else if (variable.hasRationalType()) {
                            stream << rationalValues[mapIt->second];
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
            
            typename StateValuation::Json StateValuation::toJson(StateValuations const& valuations, boost::optional<std::set<storm::expressions::Variable>> const& selectedVariables) const {
                STORM_LOG_ASSERT(assertValuations(valuations), "Invalid  state valuations");
                typename std::map<storm::expressions::Variable, uint64_t>::const_iterator mapIt = valuations.variableToIndexMap.begin();
                typename std::set<storm::expressions::Variable>::const_iterator setIt;
                if (selectedVariables) {
                    setIt = selectedVariables->begin();
                }
                Json result;
                while (mapIt != valuations.variableToIndexMap.end() && (!selectedVariables || setIt != selectedVariables->end())) {
                    // Move Map iterator to next relevant position
                    if (selectedVariables) {
                        while (mapIt->first != *setIt) {
                            ++mapIt;
                            STORM_LOG_ASSERT(mapIt != valuations.variableToIndexMap.end(), "Valuation does not consider selected variable " << setIt->getName() << ".");
                        }
                    }
                    
                    auto const& variable = mapIt->first;
                    if (variable.hasBooleanType()) {
                        result[variable.getName()] = bool(booleanValues[mapIt->second]);
                    } else if (variable.hasIntegerType()) {
                        result[variable.getName()] = integerValues[mapIt->second];
                    } else if (variable.hasRationalType()) {
                        result[variable.getName()] = rationalValues[mapIt->second];
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
            
            bool StateValuation::assertValuations(StateValuations const& valuations) const {
                storm::storage::BitVector foundBooleanValues(booleanValues.size(), false);
                storm::storage::BitVector foundIntegerValues(integerValues.size(), false);
                storm::storage::BitVector foundRationalValues(rationalValues.size(), false);
                for (auto const& varIndex : valuations.variableToIndexMap) {
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
                return (*this)[state].toString(*this);
            }
            
            StateValuation& StateValuations::operator[](storm::storage::sparse::state_type const& state) {
                STORM_LOG_ASSERT(state < getNumberOfStates(), "Invalid state index.");
                return valuations[state];
            }
            
            StateValuation const& StateValuations::operator[](storm::storage::sparse::state_type const& state) const {
                STORM_LOG_ASSERT(state < getNumberOfStates(), "Invalid state index.");
                return valuations[state];
            }

            uint_fast64_t StateValuations::getNumberOfStates() const {
                return valuations.size();
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
                    STORM_LOG_ASSERT(currentStateValuations[state].isEmpty(), "Adding a valuation to the same state multiple times.");
                    currentStateValuations[state] = StateValuation(std::move(booleanValues), std::move(integerValues), std::move(rationalValues));
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
