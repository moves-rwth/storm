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

            StateValuations::StateValueIterator::StateValueIterator(typename std::map<storm::expressions::Variable, uint64_t>::const_iterator variableIt, StateValuation const* valuation) : variableIt(variableIt), valuation(valuation) {
                // Intentionally left empty.
            }

            storm::expressions::Variable const& StateValuations::StateValueIterator::getVariable() const { return variableIt->first; }
            bool StateValuations::StateValueIterator::isBoolean() const { return getVariable().hasBooleanType(); }
            bool StateValuations::StateValueIterator::isInteger() const { return getVariable().hasIntegerType(); }
            bool StateValuations::StateValueIterator::isRational() const { return getVariable().hasRationalType(); }
            
            bool StateValuations::StateValueIterator::getBooleanValue() const {
                STORM_LOG_ASSERT(isBoolean(), "Variable has no boolean type.");
                return valuation->booleanValues[variableIt->second];
            }
            
            int64_t StateValuations::StateValueIterator::getIntegerValue() const {
                STORM_LOG_ASSERT(isInteger(), "Variable has no integer type.");
                return valuation->integerValues[variableIt->second];
            }
            
            storm::RationalNumber StateValuations::StateValueIterator::getRationalValue() const {
                STORM_LOG_ASSERT(isRational(), "Variable has no rational type.");
                return valuation->rationalValues[variableIt->second];
            }
            
            bool StateValuations::StateValueIterator::operator==(StateValueIterator const& other) {
                STORM_LOG_ASSERT(valuation == valuation, "Comparing iterators for different states");
                return variableIt == other.variableIt;
            }
            bool StateValuations::StateValueIterator::operator!=(StateValueIterator const& other) {
                STORM_LOG_ASSERT(valuation == valuation, "Comparing iterators for different states");
                return variableIt != other.variableIt;
            }
            
            typename StateValuations::StateValueIterator& StateValuations::StateValueIterator::operator++() {
                ++variableIt;
                return *this;
            }
            
            typename StateValuations::StateValueIterator& StateValuations::StateValueIterator::operator--() {
                --variableIt;
                return *this;
            }
            
            StateValuations::StateValueIteratorRange::StateValueIteratorRange(std::map<storm::expressions::Variable, uint64_t> const& variableMap, StateValuation const* valuation) : variableMap(variableMap), valuation(valuation) {
                // Intentionally left empty.
            }
            
            StateValuations::StateValueIterator StateValuations::StateValueIteratorRange::begin() const {
                return StateValueIterator(variableMap.cbegin(), valuation);
            }
            
            StateValuations::StateValueIterator StateValuations::StateValueIteratorRange::end() const {
                return StateValueIterator(variableMap.cend(), valuation);
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
                auto const& valueAssignment = at(stateIndex);
                typename std::set<storm::expressions::Variable>::const_iterator setIt;
                if (selectedVariables) {
                    setIt = selectedVariables->begin();
                }
                std::vector<std::string> assignments;
                for (auto valIt = valueAssignment.begin(); valIt != valueAssignment.end(); ++valIt) {
                    if (selectedVariables && (*setIt != valIt.getVariable())) {
                        continue;
                    }
                 
                    std::stringstream stream;
                    if (pretty) {
                        if (valIt.isBoolean() && !valIt.getBooleanValue()) {
                            stream << "!";
                        }
                        stream << valIt.getVariable().getName();
                        if (valIt.isInteger()) {
                            stream << "=" << valIt.getIntegerValue();
                        } else if (valIt.isRational()) {
                            stream << "=" << valIt.getRationalValue();
                        } else {
                            STORM_LOG_THROW(valIt.isBoolean(), storm::exceptions::InvalidTypeException, "Unexpected variable type.");
                        }
                    } else {
                        if (valIt.isBoolean()) {
                            stream << std::boolalpha << valIt.getBooleanValue() << std::noboolalpha;
                        } else if (valIt.isInteger()) {
                            stream << valIt.getIntegerValue();
                        } else if (valIt.isRational()) {
                            stream << valIt.getRationalValue();
                        }
                    }
                    assignments.push_back(stream.str());
                    
                    if (selectedVariables) {
                        // Go to next selected position
                        ++setIt;
                    }
                }
                STORM_LOG_ASSERT(!selectedVariables || setIt == selectedVariables->end(), "Valuation does not consider selected variable " << setIt->getName() << ".");

                if (pretty) {
                    return "[" + boost::join(assignments, "\t& ") + "]";
                } else {
                    return "[" + boost::join(assignments, "\t") + "]";
                }
            }
            
            typename StateValuations::Json StateValuations::toJson(storm::storage::sparse::state_type const& stateIndex, boost::optional<std::set<storm::expressions::Variable>> const& selectedVariables) const {
                auto const& valueAssignment = at(stateIndex);
                typename std::set<storm::expressions::Variable>::const_iterator setIt;
                if (selectedVariables) {
                    setIt = selectedVariables->begin();
                }
                Json result;
                for (auto valIt = valueAssignment.begin(); valIt != valueAssignment.end(); ++valIt) {
                    if (selectedVariables && (*setIt != valIt.getVariable())) {
                        continue;
                    }
                    
                    if (valIt.isBoolean()) {
                        result[valIt.getVariable().getName()] = valIt.getBooleanValue();
                    } else if (valIt.isInteger()) {
                        result[valIt.getVariable().getName()] = valIt.getIntegerValue();
                    } else {
                        STORM_LOG_ASSERT(valIt.isRational(), "Unexpected variable type.");
                        result[valIt.getVariable().getName()] = valIt.getRationalValue();
                    }
                
                    if (selectedVariables) {
                        // Go to next selected position
                        ++setIt;
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
            
            typename StateValuations::StateValueIteratorRange StateValuations::at(state_type const& state) const {
                STORM_LOG_ASSERT(state < getNumberOfStates(), "Invalid state index.");
                return StateValueIteratorRange({variableToIndexMap, &(valuations[state])});
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

            StateValuations StateValuations::blowup(const std::vector<uint64_t> &mapNewToOld) const {
                std::vector<StateValuation> newValuations;
                for( auto const& oldState : mapNewToOld) {
                    newValuations.push_back(valuations[oldState]);
                }
                return StateValuations(variableToIndexMap, std::move(newValuations));
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
