#ifndef STORM_JANIPARSER_H
#define STORM_JANIPARSER_H

#include <src/storage/jani/Model.h>
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

            static storm::jani::Model parse(std::string const& path);

        protected:
            void readFile(std::string const& path);
            storm::jani::Automaton parseAutomaton(json const& automatonStructure);




        };
    }
}



#endif //STORM_JANIPARSER_H
