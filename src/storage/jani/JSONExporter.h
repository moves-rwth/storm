#pragma once


#include "src/storage/expressions/ExpressionVisitor.h"
#include "Model.h"
// JSON parser
#include "json.hpp"
namespace modernjson = nlohmann;

namespace storm {
    namespace jani {
        
        class ExpressionToJson : public storm::expressions::ExpressionVisitor {
            
        public:
            static modernjson::json translate(storm::expressions::Expression const& expr);
            
            virtual boost::any visit(storm::expressions::IfThenElseExpression const& expression, boost::any const& data);
            virtual boost::any visit(storm::expressions::BinaryBooleanFunctionExpression const& expression, boost::any const& data);
            virtual boost::any visit(storm::expressions::BinaryNumericalFunctionExpression const& expression, boost::any const& data);
            virtual boost::any visit(storm::expressions::BinaryRelationExpression const& expression, boost::any const& data);
            virtual boost::any visit(storm::expressions::VariableExpression const& expression, boost::any const& data);
            virtual boost::any visit(storm::expressions::UnaryBooleanFunctionExpression const& expression, boost::any const& data);
            virtual boost::any visit(storm::expressions::UnaryNumericalFunctionExpression const& expression, boost::any const& data);
            virtual boost::any visit(storm::expressions::BooleanLiteralExpression const& expression, boost::any const& data);
            virtual boost::any visit(storm::expressions::IntegerLiteralExpression const& expression, boost::any const& data);
            virtual boost::any visit(storm::expressions::RationalLiteralExpression const& expression, boost::any const& data);
            
        };
        
        class JsonExporter {
            JsonExporter() = default;
            
        public:
            static void toFile(storm::jani::Model const& janiModel, std::string const& filepath, bool checkValid = true);
            static void toStream(storm::jani::Model const& janiModel, std::ostream& ostream, bool checkValid = false);
            
        private:
            void convertModel(storm::jani::Model const& model);
            void appendVariableDeclaration(storm::jani::Variable const& variable);
            
            modernjson::json finalize() {
                return jsonStruct;
            }
            
            modernjson::json jsonStruct;
            
            
        };
    }
}
