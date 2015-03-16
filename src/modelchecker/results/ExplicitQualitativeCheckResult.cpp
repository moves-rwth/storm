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
        
        void ExplicitQualitativeCheckResult::performLogicalOperation(ExplicitQualitativeCheckResult& first, QualitativeCheckResult const& second, std::function<bool (bool, bool)> const& function) {
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
        
        QualitativeCheckResult& ExplicitQualitativeCheckResult::operator&=(QualitativeCheckResult const& other) {
            performLogicalOperation(*this, other, [] (bool a, bool b) { return a && b; });
            return *this;
        }
        
        QualitativeCheckResult& ExplicitQualitativeCheckResult::operator|=(QualitativeCheckResult const& other) {
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
        
        void ExplicitQualitativeCheckResult::filter(QualitativeCheckResult const& filter) {
            STORM_LOG_THROW(filter.isExplicitQualitativeCheckResult(), storm::exceptions::InvalidOperationException, "Cannot filter explicit check result with non-explicit filter.");
            STORM_LOG_THROW(filter.isResultForAllStates(), storm::exceptions::InvalidOperationException, "Cannot filter check result with non-complete filter.");
            ExplicitQualitativeCheckResult const& explicitFilter = filter.asExplicitQualitativeCheckResult();
            vector_type const& filterTruthValues = explicitFilter.getTruthValuesVector();
            
            if (this->isResultForAllStates()) {
                map_type newMap;
                for (auto const& element : filterTruthValues) {
                    newMap.emplace(element, this->getTruthValuesVector().get(element));
                }
                this->truthValues = newMap;
            } else {
                map_type const& map = boost::get<map_type>(truthValues);
                
                map_type newMap;
                for (auto const& element : map) {
                    if (filterTruthValues.get(element.first)) {
                        newMap.insert(element);
                    }
                }
                
                STORM_LOG_THROW(newMap.size() == filterTruthValues.getNumberOfSetBits(), storm::exceptions::InvalidOperationException, "The check result fails to contain some results referred to by the filter.");
                
                this->truthValues = newMap;
            }
        }
    }
}