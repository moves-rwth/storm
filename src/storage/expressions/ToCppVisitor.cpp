#include "src/storage/expressions/ToCppVisitor.h"

#include "src/storage/expressions/Expressions.h"

namespace storm {
    namespace expressions {
        
        ToCppTranslationOptions::ToCppTranslationOptions(std::string const& prefix, std::string const& valueTypeCast) : valueTypeCast(valueTypeCast), prefix(prefix) {
            // Intentionally left empty.
        }
        
        std::string const& ToCppTranslationOptions::getPrefix() const {
            return prefix;
        }
        
        bool ToCppTranslationOptions::hasValueTypeCast() const {
            return !valueTypeCast.empty();
        }
        
        std::string const& ToCppTranslationOptions::getValueTypeCast() const {
            return valueTypeCast;
        }
        
        void ToCppTranslationOptions::clearValueTypeCast() {
            valueTypeCast = "";
        }
        
        std::string ToCppVisitor::translate(storm::expressions::Expression const& expression, ToCppTranslationOptions const& options) {
            expression.accept(*this, options);
            std::string result = stream.str();
            stream.str("");
            return result;
        }
        
        boost::any ToCppVisitor::visit(IfThenElseExpression const& expression, boost::any const& data) {
            ToCppTranslationOptions conditionOptions = boost::any_cast<ToCppTranslationOptions>(data);
            conditionOptions.clearValueTypeCast();
            stream << "(";
            expression.getCondition()->accept(*this, conditionOptions);
            stream << " ? ";
            expression.getThenExpression()->accept(*this, data);
            stream << " : ";
            expression.getElseExpression()->accept(*this, data);
            stream << ")";
            return boost::none;
        }
        
        boost::any ToCppVisitor::visit(BinaryBooleanFunctionExpression const& expression, boost::any const& data) {
            ToCppTranslationOptions newOptions = boost::any_cast<ToCppTranslationOptions>(data);
            newOptions.clearValueTypeCast();
            
            switch (expression.getOperatorType()) {
                    case BinaryBooleanFunctionExpression::OperatorType::And:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, newOptions);
                    stream << " && ";
                    expression.getSecondOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Or:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, newOptions);
                    stream << " || ";
                    expression.getSecondOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Xor:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, newOptions);
                    stream << " ^ ";
                    expression.getSecondOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Implies:
                    stream << "(!";
                    expression.getFirstOperand()->accept(*this, newOptions);
                    stream << " || ";
                    expression.getSecondOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Iff:
                    stream << "!(";
                    expression.getFirstOperand()->accept(*this, newOptions);
                    stream << " ^ ";
                    expression.getSecondOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
            }
            return boost::none;
        }
        
        boost::any ToCppVisitor::visit(BinaryNumericalFunctionExpression const& expression, boost::any const& data) {
            switch (expression.getOperatorType()) {
                case BinaryNumericalFunctionExpression::OperatorType::Plus:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << " + ";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Minus:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << " - ";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Times:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << " * ";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Divide:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << " / ";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Min:
                    stream << "std::min(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << ", ";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Max:
                    stream << "std::max(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << ", ";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Power:
                    stream << "std::pow(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << ", ";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
            }
            return boost::none;
        }
        
        boost::any ToCppVisitor::visit(BinaryRelationExpression const& expression, boost::any const& data) {
            ToCppTranslationOptions newOptions = boost::any_cast<ToCppTranslationOptions>(data);
            newOptions.clearValueTypeCast();

            switch (expression.getRelationType()) {
                case BinaryRelationExpression::RelationType::Equal:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, newOptions);
                    stream << " == ";
                    expression.getSecondOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::NotEqual:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, newOptions);
                    stream << " != ";
                    expression.getSecondOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::Less:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, newOptions);
                    stream << " < ";
                    expression.getSecondOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::LessOrEqual:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, newOptions);
                    stream << " <= ";
                    expression.getSecondOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::Greater:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, newOptions);
                    stream << " > ";
                    expression.getSecondOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::GreaterOrEqual:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, newOptions);
                    stream << " >= ";
                    expression.getSecondOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
            }
            return boost::none;
        }
        
        boost::any ToCppVisitor::visit(VariableExpression const& expression, boost::any const& data) {
            ToCppTranslationOptions const& options = boost::any_cast<ToCppTranslationOptions const&>(data);
            if (options.hasValueTypeCast()) {
                stream << "static_cast<" << options.getValueTypeCast() << ">(";
            }
            stream << options.getPrefix() << expression.getVariableName();
            if (options.hasValueTypeCast()) {
                stream << ")";
            }
            return boost::none;
        }
        
        boost::any ToCppVisitor::visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) {
            ToCppTranslationOptions newOptions = boost::any_cast<ToCppTranslationOptions>(data);
            newOptions.clearValueTypeCast();
            
            switch (expression.getOperatorType()) {
                case UnaryBooleanFunctionExpression::OperatorType::Not:
                    stream << "!(";
                    expression.getOperand()->accept(*this, newOptions);
                    stream << ")";
                    break;
            }
            return boost::none;
        }
        
        boost::any ToCppVisitor::visit(UnaryNumericalFunctionExpression const& expression, boost::any const& data) {
            switch (expression.getOperatorType()) {
                case UnaryNumericalFunctionExpression::OperatorType::Minus:
                    stream << "-(";
                    expression.getOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case UnaryNumericalFunctionExpression::OperatorType::Floor:
                    stream << "std::floor(";
                    expression.getOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case UnaryNumericalFunctionExpression::OperatorType::Ceil:
                    stream << "std::ceil(";
                    expression.getOperand()->accept(*this, data);
                    stream << ")";
                    break;
            }
            return boost::none;
        }
        
        boost::any ToCppVisitor::visit(BooleanLiteralExpression const& expression, boost::any const& data) {
            stream << std::boolalpha << expression.getValue();
            return boost::none;
        }
        
        boost::any ToCppVisitor::visit(IntegerLiteralExpression const& expression, boost::any const& data) {
            ToCppTranslationOptions const& options = boost::any_cast<ToCppTranslationOptions const&>(data);
            if (options.hasValueTypeCast()) {
                stream << "static_cast<" << options.getValueTypeCast() << ">(";
            }
            stream << expression.getValue();
            if (options.hasValueTypeCast()) {
                stream << ")";
            }
            return boost::none;
        }
        
        boost::any ToCppVisitor::visit(RationalLiteralExpression const& expression, boost::any const& data) {
            stream << expression.getValueAsDouble();
            return boost::none;
        }

    }
}
