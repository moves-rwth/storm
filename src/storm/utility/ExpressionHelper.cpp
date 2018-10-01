#include "storm/utility/ExpressionHelper.h"
#include "storm/utility/constants.h"

namespace storm {
    namespace utility {
        
        ExpressionHelper::ExpressionHelper(std::shared_ptr<storm::expressions::ExpressionManager> const& expressionManager) : manager(expressionManager) {
            // Intentionally left empty
        }

        storm::expressions::Expression ExpressionHelper::sum(std::vector<storm::expressions::Expression>&& summands) const {
            if (summands.empty()) {
                return manager->rational(storm::utility::zero<storm::RationalNumber>());
            }
            storm::expressions::Expression res = summands.front();
            bool first = true;
            for (auto& s : summands) {
                if (first) {
                    first = false;
                } else {
                    res = res + s;
                }
            }
            return res.simplify().reduceNesting();
        }
        
    }
}