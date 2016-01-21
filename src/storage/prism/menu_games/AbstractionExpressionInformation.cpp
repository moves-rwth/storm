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
         
            storm::expressions::ExpressionManager& AbstractionExpressionInformation::getManager() {
                return manager;
            }
            
            storm::expressions::ExpressionManager const& AbstractionExpressionInformation::getManager() const {
                return manager;
            }
 
            std::vector<storm::expressions::Expression>& AbstractionExpressionInformation::getPredicates() {
                return predicates;
            }
            
            std::vector<storm::expressions::Expression> const& AbstractionExpressionInformation::getPredicates() const {
                return predicates;
            }
            
            storm::expressions::Expression const& AbstractionExpressionInformation::getPredicateByIndex(uint_fast64_t index) const {
                return predicates[index];
            }
            
            std::size_t AbstractionExpressionInformation::getNumberOfPredicates() const {
                return predicates.size();
            }
            
            std::set<storm::expressions::Variable>& AbstractionExpressionInformation::getVariables() {
                return variables;
            }
            
            std::set<storm::expressions::Variable> const& AbstractionExpressionInformation::getVariables() const {
                return variables;
            }
            
            std::vector<storm::expressions::Expression>& AbstractionExpressionInformation::getRangeExpressions() {
                return rangeExpressions;
            }
            
            std::vector<storm::expressions::Expression> const& AbstractionExpressionInformation::getRangeExpressions() const {
                return rangeExpressions;
            }
            
        }
    }
}