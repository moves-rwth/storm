#pragma once


#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/logic/FormulaVisitor.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/adapters/RationalNumberAdapter.h"
// JSON parser
#include "json.hpp"
namespace modernjson {
    using json = nlohmann::json;
}

namespace storm {
    namespace jani {
        
        class ExpressionToJson : public storm::expressions::ExpressionVisitor {
            
        public:
            static modernjson::json translate(storm::expressions::Expression const& expr, std::vector<storm::jani::Constant> const& constants, VariableSet const& globalVariables, VariableSet const& localVariables);
            
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
        private:

            ExpressionToJson(std::vector<storm::jani::Constant> const& constants, VariableSet const& globalVariables, VariableSet const& localVariables) : constants(constants), globalVariables(globalVariables), localVariables(localVariables) {}
            std::vector<storm::jani::Constant> const& constants;
            VariableSet const& globalVariables;
            VariableSet const& localVariables;
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
            virtual boost::any visit(storm::logic::MultiObjectiveFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::NextFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::ProbabilityOperatorFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::RewardOperatorFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::TotalRewardFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::UnaryBooleanStateFormula const& f, boost::any const& data) const;
            virtual boost::any visit(storm::logic::UntilFormula const& f, boost::any const& data) const;
      
        private:
            FormulaToJaniJson(storm::jani::Model const& model) : model(model) { }

            modernjson::json constructPropertyInterval(boost::optional<storm::expressions::Expression> const& lower, boost::optional<bool> const& lowerExclusive, boost::optional<storm::expressions::Expression> const& upper, boost::optional<bool> const& upperExclusive) const;

            storm::jani::Model const& model;
        };
        
        class JsonExporter {
            JsonExporter() = default;
            
        public:
            static void toFile(storm::jani::Model const& janiModel, std::vector<storm::jani::Property> const& formulas, std::string const& filepath, bool checkValid = true, bool compact = false);
            static void toStream(storm::jani::Model const& janiModel, std::vector<storm::jani::Property> const& formulas, std::ostream& ostream, bool checkValid = false, bool compact = false);
            
            
        private:
            void convertModel(storm::jani::Model const& model, bool commentExpressions = true);
            void convertProperties(std::vector<storm::jani::Property> const& formulas, storm::jani::Model const& model);
            void appendVariableDeclaration(storm::jani::Variable const& variable);
            
            modernjson::json finalize() {
                return jsonStruct;
            }
            
            modernjson::json jsonStruct;
            
            
        };
    }
}
