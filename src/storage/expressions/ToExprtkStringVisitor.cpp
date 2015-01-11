#include "src/storage/expressions/ToExprtkStringVisitor.h"

namespace storm {
    namespace expressions {
        std::string ToExprtkStringVisitor::toString(Expression const& expression) {
            return toString(expression.getBaseExpressionPointer().get());
        }
        
        std::string ToExprtkStringVisitor::toString(BaseExpression const* expression) {
            stream = std::stringstream();
            expression->accept(*this);
            return std::move(stream.str());
        }
        
        boost::any ToExprtkStringVisitor::visit(IfThenElseExpression const& expression) {
            stream << "if(";
            expression.getCondition()->accept(*this);
            stream << ",";
            expression.getThenExpression()->accept(*this);
            stream << ",";
            expression.getElseExpression()->accept(*this);
            stream << ")";
            return boost::any();
        }
        
        boost::any ToExprtkStringVisitor::visit(BinaryBooleanFunctionExpression const& expression) {
            switch (expression.getOperatorType()) {
                case BinaryBooleanFunctionExpression::OperatorType::And:
                    stream << "and(";
                    expression.getFirstOperand()->accept(*this);
                    stream << ",";
                    expression.getFirstOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Or:
                    stream << "or(";
                    expression.getFirstOperand()->accept(*this);
                    stream << ",";
                    expression.getFirstOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Xor:
                    stream << "xor(";
                    expression.getFirstOperand()->accept(*this);
                    stream << ",";
                    expression.getFirstOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Implies:
                    stream << "or(not(";
                    expression.getFirstOperand()->accept(*this);
                    stream << "),";
                    expression.getFirstOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Iff:
                    expression.getFirstOperand()->accept(*this);
                    stream << "==";
                    expression.getFirstOperand()->accept(*this);
                    break;
            }
            return boost::any();
        }
        
        boost::any ToExprtkStringVisitor::visit(BinaryNumericalFunctionExpression const& expression) {
            switch (expression.getOperatorType()) {
                case BinaryNumericalFunctionExpression::OperatorType::Plus:
                    expression.getFirstOperand()->accept(*this);
                    stream << "+";
                    expression.getSecondOperand()->accept(*this);
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Minus:
                    expression.getFirstOperand()->accept(*this);
                    stream << "-";
                    expression.getSecondOperand()->accept(*this);
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Times:
                    expression.getFirstOperand()->accept(*this);
                    stream << "*";
                    expression.getSecondOperand()->accept(*this);
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Divide:
                    expression.getFirstOperand()->accept(*this);
                    stream << "/";
                    expression.getSecondOperand()->accept(*this);
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Power:
                    expression.getFirstOperand()->accept(*this);
                    stream << "^";
                    expression.getSecondOperand()->accept(*this);
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Max:
                    stream << "max(";
                    expression.getFirstOperand()->accept(*this);
                    stream << ",";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Min:
                    stream << "min(";
                    expression.getFirstOperand()->accept(*this);
                    stream << ",";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
            }
            return boost::any();
        }
        
        boost::any ToExprtkStringVisitor::visit(BinaryRelationExpression const& expression) {
            switch (expression.getRelationType()) {
                case BinaryRelationExpression::RelationType::Equal:
                    expression.getFirstOperand()->accept(*this);
                    stream << "==";
                    expression.getSecondOperand()->accept(*this);
                    break;
                case BinaryRelationExpression::RelationType::NotEqual:
                    expression.getFirstOperand()->accept(*this);
                    stream << "!=";
                    expression.getSecondOperand()->accept(*this);
                    break;
                case BinaryRelationExpression::RelationType::Less:
                    expression.getFirstOperand()->accept(*this);
                    stream << "<";
                    expression.getSecondOperand()->accept(*this);
                    break;
                case BinaryRelationExpression::RelationType::LessOrEqual:
                    expression.getFirstOperand()->accept(*this);
                    stream << "<=";
                    expression.getSecondOperand()->accept(*this);
                    break;
                case BinaryRelationExpression::RelationType::Greater:
                    expression.getFirstOperand()->accept(*this);
                    stream << ">";
                    expression.getSecondOperand()->accept(*this);
                    break;
                case BinaryRelationExpression::RelationType::GreaterOrEqual:
                    expression.getFirstOperand()->accept(*this);
                    stream << ">=";
                    expression.getSecondOperand()->accept(*this);
                    break;
            }
            return boost::any();
        }
        
        boost::any ToExprtkStringVisitor::visit(VariableExpression const& expression) {
            stream << expression.getVariableName();
            return boost::any();
        }
        
        boost::any ToExprtkStringVisitor::visit(UnaryBooleanFunctionExpression const& expression) {
            switch (expression.getOperatorType()) {
                case UnaryBooleanFunctionExpression::OperatorType::Not:
                    stream << "not(";
                    expression.getOperand()->accept(*this);
                    stream << ")";
            }
            return boost::any();
        }
        
        boost::any ToExprtkStringVisitor::visit(UnaryNumericalFunctionExpression const& expression) {
            switch (expression.getOperatorType()) {
                case UnaryNumericalFunctionExpression::OperatorType::Minus:
                    stream << "-(";
                    expression.getOperand()->accept(*this);
                    stream << ")";
                    break;
                case UnaryNumericalFunctionExpression::OperatorType::Floor:
                    stream << "floor(";
                    expression.getOperand()->accept(*this);
                    stream << ")";
                    break;
                case UnaryNumericalFunctionExpression::OperatorType::Ceil:
                    stream << "ceil(";
                    expression.getOperand()->accept(*this);
                    stream << ")";
                    break;
            }
            return boost::any();
        }
        
        boost::any ToExprtkStringVisitor::visit(BooleanLiteralExpression const& expression) {
            stream << expression.getValue();
            return boost::any();
        }
        
        boost::any ToExprtkStringVisitor::visit(IntegerLiteralExpression const& expression) {
            stream << expression.getValue();
            return boost::any();
        }
        
        boost::any ToExprtkStringVisitor::visit(DoubleLiteralExpression const& expression) {
            stream << expression.getValue();
            return boost::any();
        }
    }
}