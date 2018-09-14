#pragma once
#include "storm/storage/jani/Constant.h"
#include "storm/storage/jani/FunctionDefinition.h"
#include "storm/storage/jani/LValue.h"
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
         */
        class JaniParser {

        public:
            typedef std::vector<storm::jani::Property> PropertyVector;
            typedef std::unordered_map<std::string, storm::jani::Variable const*> VariablesMap;
            typedef std::unordered_map<std::string, storm::jani::Constant const*> ConstantsMap;
            typedef std::unordered_map<std::string, storm::jani::FunctionDefinition const*> FunctionsMap;

            JaniParser() : expressionManager(new storm::expressions::ExpressionManager()) {}
            JaniParser(std::string const& jsonstring);
            static std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> parse(std::string const& path);

        protected:
            void readFile(std::string const& path);
            
            struct Scope {
                Scope(std::string description = "global", ConstantsMap const* constants = nullptr, VariablesMap const* globalVars = nullptr, FunctionsMap const* globalFunctions = nullptr, VariablesMap const* localVars = nullptr, FunctionsMap const* localFunctions = nullptr) : description(description) , constants(constants), globalVars(globalVars), globalFunctions(globalFunctions), localVars(localVars), localFunctions(localFunctions) {};
                
                Scope(Scope const& other) = default;
                std::string description;
                ConstantsMap const* constants;
                VariablesMap const* globalVars;
                FunctionsMap const* globalFunctions;
                VariablesMap const* localVars;
                FunctionsMap const* localFunctions;
                Scope refine(std::string const& prependedDescription = "") const {
                    Scope res(*this);
                    if (prependedDescription != "") {
                        res.description = "'" + prependedDescription + "' at " + res.description;
                    }
                    return res;
                }
                
                Scope& clearVariables() {
                    this->globalVars = nullptr;
                    this->localVars = nullptr;
                    return *this;
                }
            };

            std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> parseModel(bool parseProperties = true);
            storm::jani::Property parseProperty(json const& propertyStructure, Scope const& scope);
            storm::jani::Automaton parseAutomaton(json const& automatonStructure, storm::jani::Model const& parentModel, Scope const& scope);
            struct ParsedType {
                enum class BasicType {Bool, Int, Real};
                boost::optional<BasicType> basicType;
                boost::optional<std::pair<storm::expressions::Expression, storm::expressions::Expression>> bounds;
                std::unique_ptr<ParsedType> arrayBase;
                storm::expressions::Type expressionType;
            };
            void parseType(ParsedType& result, json const& typeStructure, std::string variableName, Scope const& scope);
            storm::jani::LValue parseLValue(json const& lValueStructure, Scope const& scope);
            std::shared_ptr<storm::jani::Variable>  parseVariable(json const& variableStructure, bool requireInitialValues, Scope const& scope, std::string const& namePrefix = "");
            storm::expressions::Expression parseExpression(json const& expressionStructure, Scope const& scope, bool returnNoneOnUnknownOpString = false, std::unordered_map<std::string, storm::expressions::Variable> const& auxiliaryVariables = {});
            
        private:
            std::shared_ptr<storm::jani::Constant> parseConstant(json const& constantStructure, Scope const& scope);
            storm::jani::FunctionDefinition parseFunctionDefinition(json const& functionDefinitionStructure, Scope const& scope, std::string const& parameterNamePrefix = "");

            /**
             * Helper for parsing the actions of a model.
             */
            void parseActions(json const& actionStructure, storm::jani::Model& parentModel);
            std::shared_ptr<storm::logic::Formula const> parseFormula(json const& propertyStructure,   storm::logic::FormulaContext formulaContext, Scope const& scope, boost::optional<storm::logic::Bound> bound = boost::none);
            std::vector<storm::expressions::Expression> parseUnaryExpressionArguments(json const& expressionStructure, std::string const& opstring, Scope const& scope, bool returnNoneOnUnknownOpString = false, std::unordered_map<std::string, storm::expressions::Variable> const& auxiliaryVariables = {});
            std::vector<storm::expressions::Expression> parseBinaryExpressionArguments(json const& expressionStructure, std::string const& opstring,  Scope const& scope, bool returnNoneOnUnknownOpString = false, std::unordered_map<std::string, storm::expressions::Variable> const& auxiliaryVariables = {});


            std::vector<std::shared_ptr<storm::logic::Formula const>> parseUnaryFormulaArgument(json const& propertyStructure, storm::logic::FormulaContext formulaContext,  std::string const& opstring, Scope const& scope);
            std::vector<std::shared_ptr<storm::logic::Formula const>> parseBinaryFormulaArguments(json const& propertyStructure, storm::logic::FormulaContext formulaContext, std::string const& opstring, Scope const& scope);
            storm::jani::PropertyInterval parsePropertyInterval(json const& piStructure, Scope const& scope);
            storm::logic::RewardAccumulation parseRewardAccumulation(json const& accStructure, std::string const& context);
            
            std::shared_ptr<storm::jani::Composition> parseComposition(json const& compositionStructure);
            storm::expressions::Variable getVariableOrConstantExpression(std::string const& ident, Scope const& scope, std::unordered_map<std::string, storm::expressions::Variable> const& auxiliaryVariables = {});


            
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

