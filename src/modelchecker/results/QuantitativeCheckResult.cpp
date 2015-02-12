#include "src/modelchecker/results/QuantitativeCheckResult.h"

#include "storm-config.h"
#include "src/adapters/CarlAdapter.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        bool QuantitativeCheckResult<ValueType>::isQuantitative() const {
            return true;
        }
        
        template class QuantitativeCheckResult<double>;
        
#ifdef STORM_HAVE_CARL
        template class QuantitativeCheckResult<storm::RationalFunction>;
#endif
    }
}