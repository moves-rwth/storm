#include "JaniParser.h"
#include "src/storage/jani/Model.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/InvalidJaniException.h"
#include "src/storage/jani/ModelType.h"

#include <iostream>
#include <sstream>
#include <fstream>

namespace storm {
    namespace parser {


        std::string getString(json const& structure, std::string const& errorInfo) {
            STORM_LOG_THROW(structure.is_string(), storm::exceptions::InvalidJaniException, "Expected a string in " << errorInfo << ", got '" << structure.dump() << "'");
            return structure.front();
        }


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
            STORM_LOG_THROW(parsedStructure.count("jani-version") == 1, storm::exceptions::InvalidJaniException, "Jani-version must be given exactly once.");

            STORM_LOG_THROW(parsedStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "A type must be given exactly once");
            std::string modeltypestring = getString(parsedStructure.at("type"), "type of the model");
            storm::jani::ModelType type = storm::jani::getModelType(modeltypestring);
            STORM_LOG_THROW(type != storm::jani::ModelType::UNDEFINED, storm::exceptions::InvalidJaniException, "model type " + modeltypestring + " not recognized");
            storm::jani::Model model("model", type);

            STORM_LOG_THROW(parsedStructure.count("actions") < 2, storm::exceptions::InvalidJaniException, "Action-declarations can be given at most once");
            parseActions(parsedStructure.at("actions"), model);

            


            STORM_LOG_THROW(parsedStructure.count("automata") == 1, storm::exceptions::InvalidJaniException, "Exactly one list of automata must be given");
            STORM_LOG_THROW(parsedStructure.at("automata").is_array(), storm::exceptions::InvalidJaniException, "Automata must be an array");
            for(auto const& automataEntry : parsedStructure.at("automata")) {
                storm::jani::Automaton automaton = parseAutomaton(automataEntry);

            }
            STORM_LOG_THROW(parsedStructure.count("system") == 1, storm::exceptions::InvalidJaniException, "Exactly one system description must be given");
            std::shared_ptr<storm::jani::Composition> composition = parseComposition(parsedStructure.at("system"));


        }

        void JaniParser::parseActions(json const& actionStructure, storm::jani::Model& parentModel) {
            std::set<std::string> actionNames;
            for(auto const& actionEntry : actionStructure) {
                std::string actionName = getString(actionEntry, "name of action");
                STORM_LOG_THROW(actionNames.count(actionName) == 0, storm::exceptions::InvalidJaniException, "Action with name " + actionName + " already exists.");
                parentModel.addAction(storm::jani::Action(actionName));
                actionNames.emplace(actionName);
            }
        }

        storm::jani::Automaton JaniParser::parseAutomaton(json const &automatonStructure) {
            STORM_LOG_THROW(automatonStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Each automaton must have a name");
            std::string name = getString(parsedStructure.at("name"), " the name field for automaton");
            storm::jani::Automaton automaton(name);
            STORM_LOG_THROW(automatonStructure.count("locations") > 0, storm::exceptions::InvalidJaniException, "Automaton " << name << " does not have locations.");
            std::unordered_map<std::string, uint64_t> locIds;
            for(auto const& locEntry : parsedStructure.at("locations")) {
                std::string locName = getString(locEntry, "location of automaton " + name);
                STORM_LOG_THROW(locIds.count(locName) == 0, storm::exceptions::InvalidJaniException, "Location with name '" + locName + "' already exists in automaton '" + name + "'");
                uint64_t id = automaton.addLocation(storm::jani::Location(locName));
                locIds.emplace(locName, id);
            }




            return automaton;
        }

        std::shared_ptr<storm::jani::Composition> JaniParser::parseComposition(json const &compositionStructure) {

        }
    }
}