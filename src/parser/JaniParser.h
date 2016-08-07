#pragma once
#include <src/storage/jani/Constant.h>
#include <src/logic/Formula.h>
#include "src/exceptions/FileIoException.h"
#include "src/storage/expressions/ExpressionManager.h"

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
            static std::pair<storm::jani::Model, PropertyVector> parse(std::string const& path);


        protected:
            void readFile(std::string const& path);
            std::pair<storm::jani::Model, PropertyVector> parseModel(bool parseProperties = true);
            storm::jani::Property parseProperty(json const& propertyStructure);
            storm::jani::Automaton parseAutomaton(json const& automatonStructure, storm::jani::Model const& parentModel);
            std::shared_ptr<storm::jani::Variable>  parseVariable(json const& variableStructure, std::string const& scopeDescription, bool prefWithScope = false);
            storm::expressions::Expression parseExpression(json const& expressionStructure, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars = {});
        private:
            std::shared_ptr<storm::jani::Constant> parseConstant(json const& constantStructure, std::string const& scopeDescription = "global");

            /**
             * Helper for parsing the actions of a model.
             */
            void parseActions(json const& actionStructure, storm::jani::Model& parentModel);
            std::vector<storm::expressions::Expression> parseUnaryExpressionArguments(json const& expressionStructure, std::string const& opstring, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars = {});
            std::vector<storm::expressions::Expression> parseBinaryExpressionArguments(json const& expressionStructure, std::string const& opstring,  std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars = {});

            std::shared_ptr<storm::jani::Composition> parseComposition(json const& compositionStructure);
            storm::expressions::Variable getVariableOrConstantExpression(std::string const& ident, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars = {});


            /**
             * The overall structure currently under inspection.
             */
            json parsedStructure;
            /**
             * The expression manager to be used.
             */
            std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;


            //////////
            //   Default values -- assumptions from JANI.
            //////////
            static const bool defaultVariableTransient;

        };
    }
}

