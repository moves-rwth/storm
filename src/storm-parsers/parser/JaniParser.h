#pragma once
#include "storm/storage/jani/Constant.h"
#include "storm/logic/Formula.h"
#include "storm/logic/Bound.h"
#include "storm/logic/RewardAccumulation.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/storage/expressions/ExpressionManager.h"


// JSON parser
#include "json.hpp"

using json = nlohmann::json;

namespace storm {
    namespace jani {
        class Model;
        class Automaton;
        class Variable;
        class Composition;
        class Property;
        struct PropertyInterval;
    }
    
    namespace logic {
        enum class FormulaContext;
    }


    namespace parser {
        /*
         * The JANI format parser.
         * Parses Models and Properties
         *
         * TODO some parts are copy-heavy, a bit of cleaning is good as soon as the format is stable.
         */
        class JaniParser {

        public:
            typedef std::vector<storm::jani::Property> PropertyVector;

            JaniParser() : expressionManager(new storm::expressions::ExpressionManager()) {}
            JaniParser(std::string const& jsonstring);
            static std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> parse(std::string const& path);


        protected:
            void readFile(std::string const& path);
            std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> parseModel(bool parseProperties = true);
            storm::jani::Property parseProperty(json const& propertyStructure, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants);
            storm::jani::Automaton parseAutomaton(json const& automatonStructure, storm::jani::Model const& parentModel, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants);
            struct ParsedType {
                enum class BasicType {Bool, Int, Real};
    
                boost::optional<BasicType> basicType;
                boost::optional<std::pair<storm::expressions::Expression, storm::expressions::Expression>> bounds;
                boost::optional<ParsedType> arrayBase;
            };
            void JaniParser::parseType(ParsedType& result, json const& typeStructure, std::string variableName, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars);
            std::shared_ptr<storm::jani::Variable>  parseVariable(json const& variableStructure, bool requireInitialValues, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars = {}, bool prefWithScope = false);
            storm::expressions::Expression parseExpression(json const& expressionStructure, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars = {}, bool returnNoneOnUnknownOpString = false);
            
        private:
            std::shared_ptr<storm::jani::Constant> parseConstant(json const& constantStructure,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants, std::string const& scopeDescription = "global");

            /**
             * Helper for parsing the actions of a model.
             */
            void parseActions(json const& actionStructure, storm::jani::Model& parentModel);
            std::shared_ptr<storm::logic::Formula const> parseFormula(json const& propertyStructure,   storm::logic::FormulaContext formulaContext, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants, std::string const& context,  boost::optional<storm::logic::Bound> bound = boost::none);
            std::vector<storm::expressions::Expression> parseUnaryExpressionArguments(json const& expressionStructure, std::string const& opstring, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars= {}, bool returnNoneOnUnknownOpString = false);
            std::vector<storm::expressions::Expression> parseBinaryExpressionArguments(json const& expressionStructure, std::string const& opstring,  std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars = {}, bool returnNoneOnUnknownOpString = false);


            std::vector<std::shared_ptr<storm::logic::Formula const>> parseUnaryFormulaArgument(json const& propertyStructure, storm::logic::FormulaContext formulaContext,  std::string const& opstring, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants, std::string const& context);
            std::vector<std::shared_ptr<storm::logic::Formula const>> parseBinaryFormulaArguments(json const& propertyStructure, storm::logic::FormulaContext formulaContext, std::string const& opstring, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants, std::string const& context);
            storm::jani::PropertyInterval parsePropertyInterval(json const& piStructure, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants);
            storm::logic::RewardAccumulation parseRewardAccumulation(json const& accStructure, std::string const& context);
            
            
            std::shared_ptr<storm::jani::Composition> parseComposition(json const& compositionStructure);
            storm::expressions::Variable getVariableOrConstantExpression(std::string const& ident, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars = {});


            
            /**
             * The overall structure currently under inspection.
             */
            json parsedStructure;
            /**
             * The expression manager to be used.
             */
            std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;
            
            std::set<std::string> labels = {};

            bool allowRecursion = true;

            //////////
            //   Default values -- assumptions from JANI.
            //////////
            static const bool defaultVariableTransient;
            
            static const bool defaultBooleanInitialValue;
            static const double defaultRationalInitialValue;
            static const int64_t defaultIntegerInitialValue;

            static const std::set<std::string> unsupportedOpstrings;

        };
    }
}

