#ifndef STORM_JANIPARSER_H
#define STORM_JANIPARSER_H

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
    }


    namespace parser {
        class JaniParser {

            json parsedStructure;
            std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;

        public:
            JaniParser() : expressionManager(new storm::expressions::ExpressionManager()) {}
            JaniParser(std::string& jsonstring);
            static storm::jani::Model parse(std::string const& path);

        protected:
            void readFile(std::string const& path);
            storm::jani::Model parseModel();
            storm::jani::Automaton parseAutomaton(json const& automatonStructure);
            std::shared_ptr<storm::jani::Variable>  parseVariable(json const& variableStructure, std::string const& scopeDescription, bool prefWithScope = false);
            storm::expressions::Expression parseExpression(json const& expressionStructure, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars = {});
        private:
            /**
             * Helper for parsing the actions of a model.
             */
            void parseActions(json const& actionStructure, storm::jani::Model& parentModel);

            std::shared_ptr<storm::jani::Composition> parseComposition(json const& compositionStructure);



        };
    }
}



#endif //STORM_JANIPARSER_H
