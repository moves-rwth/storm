#include "src/abstraction/AbstractionInformation.h"

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace abstraction {

        template<storm::dd::DdType DdType, typename ValueType>
        AbstractionInformation<DdType, ValueType>::AbstractionInformation(storm::expressions::ExpressionManager& expressionManager) : expressionManager(expressionManager) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        void AbstractionInformation<DdType, ValueType>::addVariable(storm::expressions::Variable const& variable) {
            variables.insert(variable);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        void AbstractionInformation<DdType, ValueType>::addVariable(storm::expressions::Variable const& variable, storm::expressions::Expression const& constraint) {
            addVariable(variable);
            addConstraint(constraint);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        void AbstractionInformation<DdType, ValueType>::addConstraint(storm::expressions::Expression const& constraint) {
            constraints.push_back(constraint);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        void AbstractionInformation<DdType, ValueType>::addPredicate(storm::expressions::Expression const& predicate) {
            predicates.push_back(predicate);
        }

        template<storm::dd::DdType DdType, typename ValueType>
        void AbstractionInformation<DdType, ValueType>::addPredicates(std::vector<storm::expressions::Expression> const& predicates) {
            for (auto const& predicate : predicates) {
                this->addPredicate(predicate);
            }
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::expressions::ExpressionManager& AbstractionInformation<DdType, ValueType>::getExpressionManager() {
            return expressionManager;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::expressions::ExpressionManager const& AbstractionInformation<DdType, ValueType>::getExpressionManager() const {
            return expressionManager;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::vector<storm::expressions::Expression> const& AbstractionInformation<DdType, ValueType>::getPredicates() const {
            return predicates;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::expressions::Expression const& AbstractionInformation<DdType, ValueType>::getPredicateByIndex(uint_fast64_t index) const {
            return predicates[index];
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::size_t AbstractionInformation<DdType, ValueType>::getNumberOfPredicates() const {
            return predicates.size();
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::set<storm::expressions::Variable> const& AbstractionInformation<DdType, ValueType>::getVariables() const {
            return variables;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::vector<storm::expressions::Expression> const& AbstractionInformation<DdType, ValueType>::getConstraints() const {
            return constraints;
        }
        
        template class AbstractionInformation<storm::dd::DdType::CUDD, double>;
        template class AbstractionInformation<storm::dd::DdType::Sylvan, double>;
    }
}
