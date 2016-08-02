#include "src/storage/expressions/ExpressionEvaluatorBase.h"

#include "src/storage/expressions/ExpressionManager.h"
#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace expressions {
        template<typename RationalType>
        ExpressionEvaluatorBase<RationalType>::ExpressionEvaluatorBase(storm::expressions::ExpressionManager const& manager) : manager(manager.getSharedPointer()) {
            // Intentionally left empty.
        }
        
        template<typename RationalType>
        storm::expressions::ExpressionManager const& ExpressionEvaluatorBase<RationalType>::getManager() const {
            return *manager;
        }
        
        template class ExpressionEvaluatorBase<double>;

#ifdef STORM_HAVE_CARL
        template class ExpressionEvaluatorBase<storm::RationalNumber>;
        template class ExpressionEvaluatorBase<storm::RationalFunction>;
#endif
    }
}
