#pragma once


#include "src/storage/expressions/ExpressionVisitor.h"
#include "src/logic/FormulaVisitor.h"
#include "Model.h"
#include "src/adapters/NumberAdapter.h"
// JSON parser
#include "json.hpp"
namespace modernjson {
    using json = nlohmann::basic_json<std::map, std::vector, std::string, bool, int64_t, uint64_t, double, std::allocator>;
}

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
        
        class FormulaToJaniJson : public storm::logic::FormulaVisitor {
            
        public:
            static modernjson::json translate(storm::logic::Formula const& formula, storm::jani::Model const& modeln);
            virtual boost::any visit(storm::logic::AtomicExpressionFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::AtomicLabelFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::BinaryBooleanStateFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::BooleanLiteralFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::BoundedUntilFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::ConditionalFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::CumulativeRewardFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::EventuallyFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::TimeOperatorFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::GloballyFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::InstantaneousRewardFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::LongRunAverageOperatorFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::LongRunAverageRewardFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::NextFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::ProbabilityOperatorFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::RewardOperatorFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::UnaryBooleanStateFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::UntilFormula const& f, boost::any const& data) const;
      
        private:
            FormulaToJaniJson(storm::jani::Model const& model) : model(model) { }
            storm::jani::Model const& model;
        };
        
        class JsonExporter {
            JsonExporter() = default;
            
        public:
            static void toFile(storm::jani::Model const& janiModel, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, std::string const& filepath, bool checkValid = true);
            static void toStream(storm::jani::Model const& janiModel, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, std::ostream& ostream, bool checkValid = false);
            
        private:
            void convertModel(storm::jani::Model const& model);
            void convertProperties(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::jani::Model const& model);
            void appendVariableDeclaration(storm::jani::Variable const& variable);
            
            modernjson::json finalize() {
                return jsonStruct;
            }
            
            modernjson::json jsonStruct;
            
            
        };
    }
}
