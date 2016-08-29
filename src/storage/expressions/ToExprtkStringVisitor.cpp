#include "src/storage/expressions/ToExprtkStringVisitor.h"

namespace storm {
    namespace expressions {
        std::string ToExprtkStringVisitor::toString(Expression const& expression) {
            return toString(expression.getBaseExpressionPointer().get());
        }
        
        std::string ToExprtkStringVisitor::toString(BaseExpression const* expression) {
            stream.str("");
            stream.clear();
            expression->accept(*this);
            return stream.str();
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
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << " and ";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Or:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << " or ";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Xor:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << " xor ";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Implies:
                    stream << "(not(";
                    expression.getFirstOperand()->accept(*this);
                    stream << ") or ";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Iff:
                    expression.getFirstOperand()->accept(*this);
                    stream << "==";
                    expression.getSecondOperand()->accept(*this);
                    break;
            }
            return boost::any();
        }
        
        boost::any ToExprtkStringVisitor::visit(BinaryNumericalFunctionExpression const& expression) {
            switch (expression.getOperatorType()) {
                case BinaryNumericalFunctionExpression::OperatorType::Plus:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << "+";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Minus:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << "-";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Times:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << "*";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Divide:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << "/";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Power:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << "^";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
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
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << "==";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::NotEqual:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << "!=";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::Less:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << "<";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::LessOrEqual:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << "<=";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::Greater:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << ">";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::GreaterOrEqual:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this);
                    stream << ">=";
                    expression.getSecondOperand()->accept(*this);
                    stream << ")";
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
        
        boost::any ToExprtkStringVisitor::visit(RationalLiteralExpression const& expression) {
            stream << expression.getValue();
            return boost::any();
        }
    }
}
