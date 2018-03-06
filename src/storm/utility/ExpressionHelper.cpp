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
            // As the sum can potentially have many summands, we want to make sure that the formula tree is (roughly balanced)
            auto it = summands.begin();
            while (summands.size() > 1) {
                if (it == summands.end() || it == summands.end() - 1) {
                    it = summands.begin();
                }
                *it = *it + summands.back();
                summands.pop_back();
                ++it;
            }
            return summands.front();
        }
        
    }
}