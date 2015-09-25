#include "src/storage/prism/menu_games/AbstractionExpressionInformation.h"

#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            AbstractionExpressionInformation::AbstractionExpressionInformation(storm::expressions::ExpressionManager& manager, std::vector<storm::expressions::Expression> const& predicates, std::set<storm::expressions::Variable> const& variables, std::vector<storm::expressions::Expression> const& rangeExpressions) : manager(manager), predicates(predicates), variables(variables), rangeExpressions(rangeExpressions) {
                // Intentionally left empty.
            }
            
            void AbstractionExpressionInformation::addPredicate(storm::expressions::Expression const& predicate) {
                predicates.push_back(predicate);
            }
            
            void AbstractionExpressionInformation::addPredicates(std::vector<storm::expressions::Expression> const& predicates) {
                for (auto const& predicate : predicates) {
                    this->addPredicate(predicate);
                }
            }
            
        }
    }
}