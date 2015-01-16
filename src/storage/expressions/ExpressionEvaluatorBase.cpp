#include "src/storage/expressions/ExpressionEvaluatorBase.h"

#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace expressions {
        ExpressionEvaluatorBase::ExpressionEvaluatorBase(storm::expressions::ExpressionManager const& manager) : manager(manager.getSharedPointer()) {
            // Intentionally left empty.
        }
        
        storm::expressions::ExpressionManager const& ExpressionEvaluatorBase::getManager() const {
            return *manager;
        }
    }
}