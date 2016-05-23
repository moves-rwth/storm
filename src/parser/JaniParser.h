#ifndef STORM_JANIPARSER_H
#define STORM_JANIPARSER_H

#include <src/storage/jani/Model.h>
#include <src/storage/jani/Composition.h>
#include "src/exceptions/FileIoException.h"

// JSON parser
#include "json.hpp"

using json = nlohmann::json;

namespace storm {
    namespace jani {
        class Model;
        class Automaton;
    }


    namespace parser {
        class JaniParser {

            json parsedStructure;

        public:
            JaniParser() {}
            JaniParser(std::string& jsonstring);
            static storm::jani::Model parse(std::string const& path);

        protected:
            void readFile(std::string const& path);
            storm::jani::Model parseModel();
            storm::jani::Automaton parseAutomaton(json const& automatonStructure);
            void parseActions(json const& actionStructure, storm::jani::Model& parentModel);
            std::shared_ptr<storm::jani::Composition> parseComposition(json const& compositionStructure);



        };
    }
}



#endif //STORM_JANIPARSER_H
