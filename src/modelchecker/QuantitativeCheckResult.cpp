#include "src/modelchecker/QuantitativeCheckResult.h"

#include "storm-config.h"
#include "src/storage/parameters.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        bool QuantitativeCheckResult<ValueType>::isQuantitative() const {
            return true;
        }
        
        template class QuantitativeCheckResult<double>;
        
#ifdef PARAMETRIC_SYSTEMS
        template class QuantitativeCheckResult<storm::RationalFunction>;
#endif
    }
}