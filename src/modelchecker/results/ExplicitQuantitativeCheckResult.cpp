#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/storage/BitVector.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/exceptions/InvalidOperationException.h"
#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        ExplicitQuantitativeCheckResult<ValueType>::ExplicitQuantitativeCheckResult() : values(map_type()) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        ExplicitQuantitativeCheckResult<ValueType>::ExplicitQuantitativeCheckResult(map_type const& values) : values(values) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        ExplicitQuantitativeCheckResult<ValueType>::ExplicitQuantitativeCheckResult(map_type&& values) : values(std::move(values)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        ExplicitQuantitativeCheckResult<ValueType>::ExplicitQuantitativeCheckResult(storm::storage::sparse::state_type const& state, ValueType const& value) : values(map_type()) {
            boost::get<map_type>(values).emplace(state, value);
        }
        
        template<typename ValueType>
        ExplicitQuantitativeCheckResult<ValueType>::ExplicitQuantitativeCheckResult(vector_type const& values) : values(values) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        ExplicitQuantitativeCheckResult<ValueType>::ExplicitQuantitativeCheckResult(vector_type&& values) : values(std::move(values)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        typename ExplicitQuantitativeCheckResult<ValueType>::vector_type const& ExplicitQuantitativeCheckResult<ValueType>::getValueVector() const {
            return boost::get<vector_type>(values);
        }
        
        template<typename ValueType>
        typename ExplicitQuantitativeCheckResult<ValueType>::map_type const& ExplicitQuantitativeCheckResult<ValueType>::getValueMap() const {
            return boost::get<map_type>(values);
        }
        
        template<typename ValueType>
        void ExplicitQuantitativeCheckResult<ValueType>::filter(QualitativeCheckResult const& filter) {
            STORM_LOG_THROW(filter.isExplicitQualitativeCheckResult(), storm::exceptions::InvalidOperationException, "Cannot filter explicit check result with non-explicit filter.");
            STORM_LOG_THROW(filter.isResultForAllStates(), storm::exceptions::InvalidOperationException, "Cannot filter check result with non-complete filter.");
            ExplicitQualitativeCheckResult const& explicitFilter = filter.asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult::vector_type const& filterTruthValues = explicitFilter.getTruthValuesVector();

            if (this->isResultForAllStates()) {
                map_type newMap;
                for (auto const& element : filterTruthValues) {
                    newMap.emplace(element, this->getValueVector()[element]);
                }
                this->values = newMap;
            } else {
                map_type const& map = boost::get<map_type>(values);
                
                map_type newMap;
                for (auto const& element : map) {
                    if (filterTruthValues.get(element.first)) {
                        newMap.insert(element);
                    }
                }
                
                STORM_LOG_THROW(newMap.size() == filterTruthValues.getNumberOfSetBits(), storm::exceptions::InvalidOperationException, "The check result fails to contain some results referred to by the filter.");
                
                this->values = newMap;
            }
        }
        
        template<typename ValueType>
        std::ostream& ExplicitQuantitativeCheckResult<ValueType>::writeToStream(std::ostream& out) const {
            out << "[";
            if (this->isResultForAllStates()) {
                vector_type const& valuesAsVector = boost::get<vector_type>(values);
                bool first = true;
                for (auto const& element : valuesAsVector) {
                    if (!first) {
                        out << ", ";
                    } else {
                        first = false;
                    }
                    out << element;
                }
            } else {
                map_type const& valuesAsMap = boost::get<map_type>(values);
                bool first = true;
                for (auto const& element : valuesAsMap) {
                    if (!first) {
                        out << ", ";
                    } else {
                        first = false;
                    }
                    out << element.second;
                }
            }
            out << "]";
            return out;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> ExplicitQuantitativeCheckResult<ValueType>::compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const {
            if (this->isResultForAllStates()) {
                vector_type const& valuesAsVector = boost::get<vector_type>(values);
                storm::storage::BitVector result(valuesAsVector.size());
                switch (comparisonType) {
                    case logic::Less:
                        for (uint_fast64_t index = 0; index < valuesAsVector.size(); ++index) {
                            if (valuesAsVector[index] < bound) {
                                result.set(index);
                            }
                        }
                        break;
                    case logic::LessEqual:
                        for (uint_fast64_t index = 0; index < valuesAsVector.size(); ++index) {
                            if (valuesAsVector[index] <= bound) {
                                result.set(index);
                            }
                        }
                        break;
                    case logic::Greater:
                        for (uint_fast64_t index = 0; index < valuesAsVector.size(); ++index) {
                            if (valuesAsVector[index] > bound) {
                                result.set(index);
                            }
                        }
                        break;
                    case logic::GreaterEqual:
                        for (uint_fast64_t index = 0; index < valuesAsVector.size(); ++index) {
                            if (valuesAsVector[index] >= bound) {
                                result.set(index);
                            }
                        }
                        break;
                }
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(std::move(result)));
            } else {
                map_type const& valuesAsMap = boost::get<map_type>(values);
                std::map<storm::storage::sparse::state_type, bool> result;
                switch (comparisonType) {
                    case logic::Less:
                        for (auto const& element : valuesAsMap) {
                            result[element.first] = element.second < bound;
                        }
                        break;
                    case logic::LessEqual:
                        for (auto const& element : valuesAsMap) {
                            result[element.first] = element.second <= bound;
                        }
                        break;
                    case logic::Greater:
                        for (auto const& element : valuesAsMap) {
                            result[element.first] = element.second > bound;
                        }
                        break;
                    case logic::GreaterEqual:
                        for (auto const& element : valuesAsMap) {
                            result[element.first] = element.second >= bound;
                        }
                        break;
                }
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(std::move(result)));
            }
        }
        
#ifdef STORM_HAVE_CARL
        template<>
        std::unique_ptr<CheckResult> ExplicitQuantitativeCheckResult<storm::RationalFunction>::compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const {
            // Since it is not possible to compare rational functions against bounds, we simply call the base class method.
            return QuantitativeCheckResult::compareAgainstBound(comparisonType, bound);
        }
#endif
        
        template<typename ValueType>
        ValueType& ExplicitQuantitativeCheckResult<ValueType>::operator[](storm::storage::sparse::state_type state) {
            if (this->isResultForAllStates()) {
                return boost::get<vector_type>(values)[state];
            } else {
                return boost::get<map_type>(values)[state];
            }
        }
        
        template<typename ValueType>
        ValueType const& ExplicitQuantitativeCheckResult<ValueType>::operator[](storm::storage::sparse::state_type state) const {
            if (this->isResultForAllStates()) {
                return boost::get<vector_type>(values)[state];
            } else {
                map_type const& valuesAsMap = boost::get<map_type>(values);
                auto const& keyValuePair = valuesAsMap.find(state);
                STORM_LOG_THROW(keyValuePair != valuesAsMap.end(), storm::exceptions::InvalidOperationException, "Unknown key '" << state << "'.");
                return keyValuePair->second;
            }
        }
        
        template<typename ValueType>
        bool ExplicitQuantitativeCheckResult<ValueType>::isExplicit() const {
            return true;
        }
        
        template<typename ValueType>
        bool ExplicitQuantitativeCheckResult<ValueType>::isResultForAllStates() const {
            return values.which() == 0;
        }
        
        template<typename ValueType>
        bool ExplicitQuantitativeCheckResult<ValueType>::isExplicitQuantitativeCheckResult() const {
            return true;
        }
        
        template class ExplicitQuantitativeCheckResult<double>;
        
#ifdef STORM_HAVE_CARL
        template class ExplicitQuantitativeCheckResult<storm::RationalFunction>;
#endif
    }
}