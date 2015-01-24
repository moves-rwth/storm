#include "src/modelchecker/ExplicitQuantitativeCheckResult.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        std::vector<ValueType> const& ExplicitQuantitativeCheckResult<ValueType>::getValues() const {
            return values;
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