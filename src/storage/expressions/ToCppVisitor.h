#pragma once

#include <sstream>
#include <unordered_map>

#include "src/storage/expressions/Variable.h"
#include "src/storage/expressions/ExpressionVisitor.h"

namespace storm {
    namespace expressions {
        class Expression;
        
        class ToCppTranslationOptions {
        public:
            ToCppTranslationOptions(std::unordered_map<storm::expressions::Variable, std::string> const& prefixes, std::unordered_map<storm::expressions::Variable, std::string> const& names, std::string const& valueTypeCast = "");
            
            std::unordered_map<storm::expressions::Variable, std::string> const& getPrefixes() const;
            std::unordered_map<storm::expressions::Variable, std::string> const& getNames() const;
            
            bool hasValueTypeCast() const;
            std::string const& getValueTypeCast() const;
            void clearValueTypeCast();
            
        private:
            std::unordered_map<storm::expressions::Variable, std::string> const& prefixes;
            std::unordered_map<storm::expressions::Variable, std::string> const& names;
            std::string valueTypeCast;
        };
        
        class ToCppVisitor : public ExpressionVisitor {
        public:
            std::string translate(storm::expressions::Expression const& expression, ToCppTranslationOptions const& options);
            
            virtual boost::any visit(IfThenElseExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(BinaryBooleanFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(BinaryNumericalFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(BinaryRelationExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(VariableExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(UnaryNumericalFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(BooleanLiteralExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(IntegerLiteralExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(RationalLiteralExpression const& expression, boost::any const& data) override;
            
        private:
            std::stringstream stream;
        };
        
    }
}
