#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult() : truthValues(map_type()) {
            // Intentionally left empty.
        }
        
        ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(map_type const& map) : truthValues(map) {
            // Intentionally left empty.
        }
        
        ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(map_type&& map) : truthValues(map) {
            // Intentionally left empty.
        }
        
        ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(storm::storage::sparse::state_type state, bool value) : truthValues(map_type()) {
            boost::get<map_type>(truthValues)[state] = value;
        }
        
        ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(storm::storage::BitVector const& truthValues) : truthValues(truthValues) {
            // Intentionally left empty.
        }
        
        ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(storm::storage::BitVector&& truthValues) : truthValues(std::move(truthValues)) {
            // Intentionally left empty.
        }
        
        void ExplicitQualitativeCheckResult::performLogicalOperation(ExplicitQualitativeCheckResult& first, CheckResult const& second, std::function<bool (bool, bool)> const& function) {
            STORM_LOG_THROW(typeid(second) == typeid(ExplicitQualitativeCheckResult), storm::exceptions::InvalidOperationException, "Cannot perform logical 'and' on check results of incompatible type.");
            STORM_LOG_THROW(first.isResultForAllStates() == second.isResultForAllStates(), storm::exceptions::InvalidOperationException, "Cannot perform logical 'and' on check results of incompatible type.");
            ExplicitQualitativeCheckResult const& secondCheckResult = static_cast<ExplicitQualitativeCheckResult const&>(second);
            if (first.isResultForAllStates()) {
                boost::get<vector_type>(first.truthValues) &= boost::get<vector_type>(secondCheckResult.truthValues);
            } else {
                map_type& map1 = boost::get<map_type>(first.truthValues);
                map_type const& map2 = boost::get<map_type>(secondCheckResult.truthValues);
                for (auto& element1 : map1) {
                    auto const& keyValuePair = map2.find(element1.first);
                    STORM_LOG_THROW(keyValuePair != map2.end(), storm::exceptions::InvalidOperationException, "Cannot perform logical 'and' on check results of incompatible type.");
                    element1.second = function(element1.second, keyValuePair->second);
                }
                
                // Double-check that there are no entries in map2 that the current result does not have.
                for (auto const& element2 : map2) {
                    auto const& keyValuePair = map1.find(element2.first);
                    STORM_LOG_THROW(keyValuePair != map1.end(), storm::exceptions::InvalidOperationException, "Cannot perform logical 'and' on check results of incompatible type.");
                }
            }
        }
        
        CheckResult& ExplicitQualitativeCheckResult::operator&=(CheckResult const& other) {
            performLogicalOperation(*this, other, [] (bool a, bool b) { return a && b; });
            return *this;
        }
        
        CheckResult& ExplicitQualitativeCheckResult::operator|=(CheckResult const& other) {
            performLogicalOperation(*this, other, [] (bool a, bool b) { return a || b; });
            return *this;
        }
        
        bool ExplicitQualitativeCheckResult::operator[](storm::storage::sparse::state_type state) const {
            if (this->isResultForAllStates()) {
                return boost::get<vector_type>(truthValues).get(state);
            } else {
                map_type const& map = boost::get<map_type>(truthValues);
                auto const& keyValuePair = map.find(state);
                STORM_LOG_THROW(keyValuePair != map.end(), storm::exceptions::InvalidOperationException, "Unknown key '" << state << "'.");
                return keyValuePair->second;
            }
        }
        
        ExplicitQualitativeCheckResult::vector_type const& ExplicitQualitativeCheckResult::getTruthValuesVector() const {
            return boost::get<vector_type>(truthValues);
        }
        
        ExplicitQualitativeCheckResult::map_type const& ExplicitQualitativeCheckResult::getTruthValuesVectorMap() const {
            return boost::get<map_type>(truthValues);
        }
        
        void ExplicitQualitativeCheckResult::complement() {
            if (this->isResultForAllStates()) {
                boost::get<vector_type>(truthValues).complement();
            } else {
                for (auto& element : boost::get<map_type>(truthValues)) {
                    element.second = !element.second;
                }
            }
        }
        
        bool ExplicitQualitativeCheckResult::isExplicit() const {
            return true;
        }
        
        bool ExplicitQualitativeCheckResult::isResultForAllStates() const {
            return truthValues.which() == 0;
        }
        
        bool ExplicitQualitativeCheckResult::isExplicitQualitativeCheckResult() const {
            return true;
        }
        
        std::ostream& ExplicitQualitativeCheckResult::writeToStream(std::ostream& out) const {
            if (this->isResultForAllStates()) {
                out << boost::get<vector_type>(truthValues);
            } else {
                std::ios::fmtflags oldflags(std::cout.flags());
                out << std::boolalpha;

                map_type const& map = boost::get<map_type>(truthValues);

#ifndef WINDOWS
                typename map_type::const_iterator it = map.begin();
                typename map_type::const_iterator itPlusOne = map.begin();
                ++itPlusOne;
                typename map_type::const_iterator ite = map.end();
#else
				map_type::const_iterator it = map.begin();
				map_type::const_iterator itPlusOne = map.begin();
				++itPlusOne;
				map_type::const_iterator ite = map.end();
#endif
                
                for (; it != ite; ++itPlusOne, ++it) {
                    out << it->second;
                    if (itPlusOne != ite) {
                        out << ", ";
                    }
                }
                std::cout.flags(oldflags);
            }
            return out;
        }
        
        std::ostream& ExplicitQualitativeCheckResult::writeToStream(std::ostream& out, storm::storage::BitVector const& filter) const {
            std::ios::fmtflags oldflags(std::cout.flags());
            
            out << "[";
            storm::storage::BitVector::const_iterator it = filter.begin();
            storm::storage::BitVector::const_iterator itPlusOne = filter.begin();
            ++itPlusOne;
            storm::storage::BitVector::const_iterator ite = filter.end();
            
            out << std::boolalpha;
            if (this->isResultForAllStates()) {
                vector_type const& vector = boost::get<vector_type>(truthValues);
                for (; it != ite; ++itPlusOne, ++it) {
                    out << vector[*it];
                    if (itPlusOne != ite) {
                        out << ", ";
                    }
                }
            } else {
                map_type const& map = boost::get<map_type>(truthValues);
                bool allResultsAvailable = true;
                for (; it != ite; ++itPlusOne, ++it) {
                    auto const& keyValuePair = map.find(*it);
                    if (keyValuePair != map.end()) {
                        out << keyValuePair->second;
                        if (itPlusOne != ite) {
                            out << ", ";
                        }
                    } else {
                        allResultsAvailable = false;
                    }
                }
            }
            out << "]";
            
            std::cout.flags(oldflags);
            return out;
        }
    }
}