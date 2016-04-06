#include "src/logic/ToExpressionVisitor.h"

#include "src/logic/Formulas.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace logic {
        
        storm::expressions::Expression ToExpressionVisitor::toExpression(Formula const& f) const {
            boost::any result = f.accept(*this, boost::any());
            return boost::any_cast<storm::expressions::Expression>(result);
        }
        
        boost::any ToExpressionVisitor::visit(AtomicExpressionFormula const& f, boost::any const& data) const {
            return f.getExpression();
        }
        
        boost::any ToExpressionVisitor::visit(AtomicLabelFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(BinaryBooleanStateFormula const& f, boost::any const& data) const {
            storm::expressions::Expression left = boost::any_cast<storm::expressions::Expression>(f.getLeftSubformula().accept(*this, data));
            storm::expressions::Expression right = boost::any_cast<storm::expressions::Expression>(f.getRightSubformula().accept(*this, data));
            switch (f.getOperator()) {
                case BinaryBooleanStateFormula::OperatorType::And:
                    return left && right;
                    break;
                case BinaryBooleanStateFormula::OperatorType::Or:
                    return left || right;
                    break;
            }
        }
        
        boost::any ToExpressionVisitor::visit(BooleanLiteralFormula const& f, boost::any const& data) const {
            return std::static_pointer_cast<Formula>(std::make_shared<BooleanLiteralFormula>(f));
        }
        
        boost::any ToExpressionVisitor::visit(BoundedUntilFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(ConditionalFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(CumulativeRewardFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(EventuallyFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(TimeOperatorFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(GloballyFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(InstantaneousRewardFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(LongRunAverageOperatorFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(LongRunAverageRewardFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(NextFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(ProbabilityOperatorFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(RewardOperatorFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
        boost::any ToExpressionVisitor::visit(UnaryBooleanStateFormula const& f, boost::any const& data) const {
            storm::expressions::Expression subexpression = boost::any_cast<storm::expressions::Expression>(f.getSubformula().accept(*this, data));
            switch (f.getOperator()) {
                case UnaryBooleanStateFormula::OperatorType::Not:
                    return !subexpression;
                    break;
            }
        }
        
        boost::any ToExpressionVisitor::visit(UntilFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot assemble expression from formula that contains illegal elements.");
        }
        
    }
}
