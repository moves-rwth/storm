#include "src/modelchecker/ExplicitQuantitativeCheckResult.h"

#include "src/modelchecker/ExplicitQualitativeCheckResult.h"
#include "src/storage/BitVector.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        std::vector<ValueType> const& ExplicitQuantitativeCheckResult<ValueType>::getValues() const {
            return values;
        }
        
        template<typename ValueType>
        std::ostream& ExplicitQuantitativeCheckResult<ValueType>::writeToStream(std::ostream& out, storm::storage::BitVector const& filter) const {
            out << "[";
            storm::storage::BitVector::const_iterator it = filter.begin();
            storm::storage::BitVector::const_iterator itPlusOne = filter.begin();
            ++itPlusOne;
            storm::storage::BitVector::const_iterator ite = filter.end();
            
            for (; it != ite; ++itPlusOne, ++it) {
                out << values[*it];
                if (itPlusOne != ite) {
                    out << ", ";
                }
            }
            out << "]";
            return out;
        }
        
        template<typename ValueType>
        std::ostream& ExplicitQuantitativeCheckResult<ValueType>::writeToStream(std::ostream& out) const {
            out << "[";
            if (!values.empty()) {
                for (auto element: values) {
                    out << element << " ";
                }
            }
            out << "]";
            return out;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> ExplicitQuantitativeCheckResult<ValueType>::compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const {
            storm::storage::BitVector result(values.size());
            switch (comparisonType) {
                case logic::Less:
                    for (uint_fast64_t index = 0; index < values.size(); ++index) {
                        if (result[index] < bound) {
                            result.set(index);
                        }
                    }
                    break;
                case logic::LessEqual:
                    for (uint_fast64_t index = 0; index < values.size(); ++index) {
                        if (result[index] <= bound) {
                            result.set(index);
                        }
                    }
                    break;
                case logic::Greater:
                    for (uint_fast64_t index = 0; index < values.size(); ++index) {
                        if (result[index] > bound) {
                            result.set(index);
                        }
                    }
                    break;
                case logic::GreaterEqual:
                    for (uint_fast64_t index = 0; index < values.size(); ++index) {
                        if (result[index] >= bound) {
                            result.set(index);
                        }
                    }
                    break;
            }
            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(std::move(result)));
        }
        
        template<typename ValueType>
        ValueType ExplicitQuantitativeCheckResult<ValueType>::operator[](uint_fast64_t index) const {
            return values[index];
        }
        
        template<typename ValueType>
        bool ExplicitQuantitativeCheckResult<ValueType>::isExplicit() const {
            return true;
        }
        
        template<typename ValueType>
        bool ExplicitQuantitativeCheckResult<ValueType>::isResultForAllStates() const {
            return true;
        }
        
        template<typename ValueType>
        bool ExplicitQuantitativeCheckResult<ValueType>::isExplicitQuantitativeCheckResult() const {
            return true;
        }
        
        template class ExplicitQuantitativeCheckResult<double>;
    }
}