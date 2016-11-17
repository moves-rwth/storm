#include "src/storm/storage/expressions/ToCppVisitor.h"

#include "src/storm/storage/expressions/Expressions.h"

namespace storm {
    namespace expressions {
        
        ToCppTranslationOptions::ToCppTranslationOptions(std::unordered_map<storm::expressions::Variable, std::string> const& prefixes, std::unordered_map<storm::expressions::Variable, std::string> const& names, ToCppTranslationMode mode) : prefixes(prefixes), names(names), mode(mode) {
            // Intentionally left empty.
        }
        
        std::unordered_map<storm::expressions::Variable, std::string> const& ToCppTranslationOptions::getPrefixes() const {
            return prefixes.get();
        }
        
        std::unordered_map<storm::expressions::Variable, std::string> const& ToCppTranslationOptions::getNames() const {
            return names.get();
        }
        
        ToCppTranslationMode const& ToCppTranslationOptions::getMode() const {
            return mode;
        }
        
        std::string ToCppVisitor::translate(storm::expressions::Expression const& expression, ToCppTranslationOptions const& options) {
            expression.accept(*this, options);
            std::string result = stream.str();
            stream.str("");
            return result;
        }
        
        boost::any ToCppVisitor::visit(IfThenElseExpression const& expression, boost::any const& data) {
            ToCppTranslationOptions const& options = boost::any_cast<ToCppTranslationOptions>(data);
            
            // Clear the type cast for the condition.
            ToCppTranslationOptions conditionOptions(options.getPrefixes(), options.getNames());
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

        std::string getVariableName(storm::expressions::Variable const& variable, std::unordered_map<storm::expressions::Variable, std::string> const& prefixes, std::unordered_map<storm::expressions::Variable, std::string> const& names) {
            auto prefixIt = prefixes.find(variable);
            if (prefixIt != prefixes.end()) {
                auto nameIt = names.find(variable);
                if (nameIt != names.end()) {
                    return prefixIt->second + nameIt->second;
                } else {
                    return prefixIt->second + variable.getName();
                }
            } else {
                auto nameIt = names.find(variable);
                if (nameIt != names.end()) {
                    return nameIt->second;
                } else {
                    return variable.getName();
                }
            }
        }
        
        boost::any ToCppVisitor::visit(VariableExpression const& expression, boost::any const& data) {
            ToCppTranslationOptions const& options = boost::any_cast<ToCppTranslationOptions const&>(data);
            storm::expressions::Variable const& variable = expression.getVariable();
            std::string variableName = getVariableName(variable, options.getPrefixes(), options.getNames());
            
            if (variable.hasBooleanType()) {
                stream << variableName;
            } else {
                switch (options.getMode()) {
                    case ToCppTranslationMode::KeepType:
                        stream << variableName;
                        break;
                    case ToCppTranslationMode::CastDouble:
                        stream << "static_cast<double>(" << variableName << ")";
                        break;
                    case ToCppTranslationMode::CastRationalNumber:
                        stream << "carl::rationalize<storm::RationalNumber>(" << variableName << ")";
                        break;
                    case ToCppTranslationMode::CastRationalFunction:
                        // Here, we rely on the variable name mapping to a rational function representing the variable being available.
                        stream << variableName;
                        break;
                }
            }
            return boost::none;
        }

        boost::any ToCppVisitor::visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) {
            ToCppTranslationOptions newOptions = boost::any_cast<ToCppTranslationOptions>(data);
            
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
            switch (options.getMode()) {
                case ToCppTranslationMode::KeepType:
                    stream << expression.getValue();
                    break;
                case ToCppTranslationMode::CastDouble:
                    stream << "static_cast<double>(" << expression.getValue() << ")";
                    break;
                case ToCppTranslationMode::CastRationalNumber:
                    stream << "carl::rationalize<storm::RationalNumber>(\"" << expression.getValue() << "\")";
                    break;
                case ToCppTranslationMode::CastRationalFunction:
                    stream << "storm::RationalFunction(carl::rationalize<storm::RationalNumber>(\"" << expression.getValue() << "\"))";
                    break;
            }
            return boost::none;
        }

        boost::any ToCppVisitor::visit(RationalLiteralExpression const& expression, boost::any const& data) {
            ToCppTranslationOptions const& options = boost::any_cast<ToCppTranslationOptions const&>(data);
            switch (options.getMode()) {
                case ToCppTranslationMode::KeepType:
                    stream << expression.getValue();
                    break;
                case ToCppTranslationMode::CastDouble:
                    stream << "static_cast<double>(" << expression.getValueAsDouble() << ")";
                    break;
                case ToCppTranslationMode::CastRationalNumber:
                    stream << "carl::rationalize<storm::RationalNumber>(\"" << expression.getValue() << "\")";
                    break;
                case ToCppTranslationMode::CastRationalFunction:
                    stream << "storm::RationalFunction(carl::rationalize<storm::RationalNumber>(\"" << expression.getValue() << "\"))";
                    break;
            }
            return boost::none;
        }

    }
}
