#include "src/modelchecker/QuantitativeCheckResult.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        bool QuantitativeCheckResult<ValueType>::isQuantitative() const {
            return true;
        }
        
        template class QuantitativeCheckResult<double>;
    }
}