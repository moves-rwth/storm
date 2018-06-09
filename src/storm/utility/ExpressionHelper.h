#pragma once

#include <vector>
#include <memory>
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace utility {
    
        class ExpressionHelper {

        public:
            ExpressionHelper(std::shared_ptr<storm::expressions::ExpressionManager> const& expressionManager);
            
            /*!
             * Creates an expression that is the sum over all the given summands.
             */
            storm::expressions::Expression sum(std::vector<storm::expressions::Expression>&& summands) const;

        private:
            
            std::shared_ptr<storm::expressions::ExpressionManager> manager;
        };
    
    
    }
}