#include "src/storage/expressions/Valuation.h"
#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace expressions {
        ExpressionManager const& Valuation::getManager() const {
            return manager;
        }
    }
}