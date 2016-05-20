#include "JaniParser.h"
#include "src/storage/jani/Model.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/InvalidJaniException.h"

#include <iostream>
#include <sstream>
#include <fstream>

namespace storm {
    namespace parser {

        storm::jani::Model JaniParser::parse(std::string const& path) {
            JaniParser parser;
            parser.readFile(path);
            return parser.parseModel();
        }

        JaniParser::JaniParser(std::string &jsonstring) {
            parsedStructure = json::parse(jsonstring);
        }

        void JaniParser::readFile(std::string const &path) {
            std::ifstream file;
            file.exceptions ( std::ifstream::failbit );
            try {
                file.open(path);
            }
            catch (std::ifstream::failure e) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Exception during file opening on " << path << ".");
                return;
            }
            file.exceptions( std::ifstream::goodbit );

            parsedStructure << file;
            file.close();

        }

        storm::jani::Model JaniParser::parseModel() {

            STORM_LOG_THROW(parsedStructure.count("automata") == 1, storm::exceptions::InvalidJaniException, "Exactly one list of automata must be given");
            STORM_LOG_THROW(parsedStructure.at("automata").is_array(), storm::exceptions::InvalidJaniException, "Automata must be an array");
            for(auto const& automataEntry : parsedStructure.at("automata")) {
                storm::jani::Automaton automaton = parseAutomaton(automataEntry);

            }
            STORM_LOG_THROW(parsedStructure.count("system") == 1, storm::exceptions::InvalidJaniException, "Exactly one system description must be given");
            std::shared_ptr<storm::jani::Composition> composition = parseComposition(parsedStructure.at("system"));


        }

        storm::jani::Automaton JaniParser::parseAutomaton(json const &automatonStructure) {

        }

        std::shared_ptr<storm::jani::Composition> JaniParser::parseComposition(json const &compositionStructure) {
            
        }
    }
}