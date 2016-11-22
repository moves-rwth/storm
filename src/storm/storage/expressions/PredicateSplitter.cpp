#include "storm/storage/expressions/PredicateSplitter.h"

#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/Expressions.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace expressions {
        
        std::vector<storm::expressions::Expression> PredicateSplitter::split(storm::expressions::Expression const& expression) {
            STORM_LOG_THROW(expression.hasBooleanType(), storm::exceptions::InvalidArgumentException, "Expected predicate of boolean type.");
            
            // Gather all atoms.
            atomicExpressions.clear();
            expression.accept(*this, boost::none);
            
            // Remove all boolean literals from the atoms.
            std::vector<storm::expressions::Expression> atomsToKeep;
            for (auto const& atom : atomicExpressions) {
                if (!atom.isTrue() && !atom.isFalse()) {
                    atomsToKeep.push_back(atom);
                }
            }
            atomicExpressions = std::move(atomsToKeep);
            
            return atomicExpressions;
        }
        
        boost::any PredicateSplitter::visit(IfThenElseExpression const& expression, boost::any const&) {
            atomicExpressions.push_back(expression.shared_from_this());
            return boost::any();
        }
        
        boost::any PredicateSplitter::visit(BinaryBooleanFunctionExpression const& expression, boost::any const& data) {
            expression.getFirstOperand()->accept(*this, data);
            expression.getSecondOperand()->accept(*this, data);
            return boost::any();
        }
        
        boost::any PredicateSplitter::visit(BinaryNumericalFunctionExpression const&, boost::any const&) {
            return boost::any();
        }
        
        boost::any PredicateSplitter::visit(BinaryRelationExpression const& expression, boost::any const&) {
            atomicExpressions.push_back(expression.shared_from_this());
            return boost::any();
        }
        
        boost::any PredicateSplitter::visit(VariableExpression const& expression, boost::any const&) {
            if (expression.hasBooleanType()) {
                atomicExpressions.push_back(expression.shared_from_this());
            }
            return boost::any();
        }
        
        boost::any PredicateSplitter::visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) {
            expression.getOperand()->accept(*this, data);
            return boost::any();
        }
        
        boost::any PredicateSplitter::visit(UnaryNumericalFunctionExpression const&, boost::any const&) {
            return boost::any();
        }
        
        boost::any PredicateSplitter::visit(BooleanLiteralExpression const&, boost::any const&) {
            return boost::any();
        }
        
        boost::any PredicateSplitter::visit(IntegerLiteralExpression const&, boost::any const&) {
            return boost::any();
        }
        
        boost::any PredicateSplitter::visit(RationalLiteralExpression const&, boost::any const&) {
            return boost::any();
        }
        
    }
}
