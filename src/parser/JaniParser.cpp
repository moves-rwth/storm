#include "JaniParser.h"
#include "src/storage/jani/Model.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/InvalidJaniException.h"
#include "src/storage/jani/ModelType.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/lexical_cast.hpp>

namespace storm {
    namespace parser {


        std::string getString(json const& structure, std::string const& errorInfo) {
            STORM_LOG_THROW(structure.is_string(), storm::exceptions::InvalidJaniException, "Expected a string in " << errorInfo << ", got '" << structure.dump() << "'");
            return structure.front();
        }

        uint64_t getUnsignedInt(json const& structure, std::string const& errorInfo) {
            STORM_LOG_THROW(structure.is_number(), storm::exceptions::InvalidJaniException, "Expected a number in " << errorInfo << ", got '" << structure.dump() << "'");
            int num = structure.front();
            STORM_LOG_THROW(num >= 0, storm::exceptions::InvalidJaniException, "Expected a positive number in " << errorInfo << ", got '" << num << "'");
            return static_cast<uint64_t>(num);
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
            uint64_t version = getUnsignedInt(parsedStructure.at("jani-version"), "jani version");
            STORM_LOG_THROW(parsedStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "A model must have a (single) name");
            std::string name = getString(parsedStructure.at("name"), "model name");
            STORM_LOG_THROW(parsedStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "A type must be given exactly once");
            std::string modeltypestring = getString(parsedStructure.at("type"), "type of the model");
            storm::jani::ModelType type = storm::jani::getModelType(modeltypestring);
            STORM_LOG_THROW(type != storm::jani::ModelType::UNDEFINED, storm::exceptions::InvalidJaniException, "model type " + modeltypestring + " not recognized");
            storm::jani::Model model(name, type, version);
            STORM_LOG_THROW(parsedStructure.count("actions") < 2, storm::exceptions::InvalidJaniException, "Action-declarations can be given at most once.");
            parseActions(parsedStructure.at("actions"), model);
            STORM_LOG_THROW(parsedStructure.count("variables") < 2, storm::exceptions::InvalidJaniException, "Variable-declarations can be given at most once for global variables.");
            for(auto const& varStructure : parsedStructure.at("variables")) {
                parseVariable(varStructure, "global");
            }
            STORM_LOG_THROW(parsedStructure.count("automata") == 1, storm::exceptions::InvalidJaniException, "Exactly one list of automata must be given");
            STORM_LOG_THROW(parsedStructure.at("automata").is_array(), storm::exceptions::InvalidJaniException, "Automata must be an array");
            for(auto const& automataEntry : parsedStructure.at("automata")) {
                storm::jani::Automaton automaton = parseAutomaton(automataEntry);

            }
            STORM_LOG_THROW(parsedStructure.count("system") == 1, storm::exceptions::InvalidJaniException, "Exactly one system description must be given");
            std::shared_ptr<storm::jani::Composition> composition = parseComposition(parsedStructure.at("system"));

            return model;
        }

        std::shared_ptr<storm::jani::Variable> JaniParser::parseVariable(json const &variableStructure, std::string const& scopeDescription) {
            STORM_LOG_THROW(variableStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Variable (scope: " + scopeDescription + ") must have a name");
            std::string name = getString(variableStructure.at("name"), "variable-name in " + scopeDescription + "-scope");
            STORM_LOG_THROW(variableStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "Variable '" + name + "' (scope: " + scopeDescription + ") must have a (single) type-declaration.");
            STORM_LOG_THROW(variableStructure.count("initial-value") == 1, storm::exceptions::InvalidJaniException, "Initial value for variable '" + name +  "' +  (scope: " + scopeDescription + ") must be given once.");
            // Read initial value before; that makes creation later on a bit easier, and has as an additional benefit that we do not need to check whether the variable occurs also on the assignment.
            storm::expressions::Expression initExpr = parseExpression(variableStructure.at("initial-value"), "Initial value of variable " + name + " (scope: " + scopeDescription + ")");
            assert(initExpr.isInitialized());
            if(variableStructure.at("type").is_string()) {
                if(variableStructure.at("type") == "real") {
                    // expressionManager->declareRationalVariable(name);
                } else if(variableStructure.at("type") == "bool") {
                    STORM_LOG_THROW(initExpr.hasBooleanType(), storm::exceptions::InvalidJaniException, "Initial value for Boolean variable " << name << " (scope: " << scopeDescription << ") should have Boolean type.");
                    return std::make_shared<storm::jani::BooleanVariable>(name, expressionManager->declareBooleanVariable(name), initExpr);
                } else if(variableStructure.at("type") == "int") {
                    STORM_LOG_THROW(initExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Initial value for interger variable "  << name << " (scope: " << scopeDescription << ") should have integer type.");
                    return std::make_shared<storm::jani::UnboundedIntegerVariable>(name, expressionManager->declareIntegerVariable(name), initExpr);
                } else {
                    // TODO clocks.
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description " << variableStructure.at("type").dump()  << " for Variable '" << name << "' (scope: " << scopeDescription << ")");
                }
            }
            if(variableStructure.at("type").is_object()) {
                STORM_LOG_THROW(variableStructure.at("type").count("kind") == 1, storm::exceptions::InvalidJaniException, "For complex type as in variable " << name << "(scope: " << scopeDescription << ")  kind must be given");
                std::string kind = getString(variableStructure.at("type").at("kind"), "kind for complex type as in variable " + name  + "(scope: " + scopeDescription + ") ");
                if(kind == "bounded") {
                    // First do the bounds, that makes the code a bit more streamlined
                    STORM_LOG_THROW(variableStructure.at("type").count("lower-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") lower-bound must be given");
                    storm::expressions::Expression lowerboundExpr = parseExpression(variableStructure.at("type").at("lower-bound"), "Lower bound for variable "+ name + " (scope: " + scopeDescription + ")");
                    assert(lowerboundExpr.isInitialized());
                    STORM_LOG_THROW(variableStructure.at("type").count("upper-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") upper-bound must be given");
                    storm::expressions::Expression upperboundExpr = parseExpression(variableStructure.at("type").at("upper-bound"), "Upper bound for variable "+ name + " (scope: " + scopeDescription + ")");
                    assert(upperboundExpr.isInitialized());
                    STORM_LOG_THROW(variableStructure.at("type").count("base") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") base must be given");
                    std::string basictype = getString(variableStructure.at("type").at("base"), "base for bounded type as in variable " + name  + "(scope: " + scopeDescription + ") ");
                    if(basictype == "int") {
                        STORM_LOG_THROW(lowerboundExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Lower bound for bounded integer variable " << name << "(scope: " << scopeDescription << ") must be integer-typed");
                        STORM_LOG_THROW(upperboundExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Upper bound for bounded integer variable " << name << "(scope: " << scopeDescription << ") must be integer-typed");
                        return std::make_shared<storm::jani::BoundedIntegerVariable>(name, expressionManager->declareIntegerVariable(name), lowerboundExpr, upperboundExpr, initExpr);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported base " << basictype << " for bounded variable " << name << "(scope: " << scopeDescription << ") ");
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported kind " << kind << " for complex type of variable " << name << "(scope: " << scopeDescription << ") ");
                }
            }
            else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description, " << variableStructure.at("type").dump()  << " for Variable '" << name << "' (scope: " << scopeDescription << ")");
            }
        }

        storm::expressions::Expression JaniParser::parseExpression(json const& expressionStructure, std::string const& scopeDescription) {
            if(expressionStructure.is_boolean()) {
                if(expressionStructure.get<bool>()) {
                    return expressionManager->boolean(true);
                } else {
                    return expressionManager->boolean(false);
                }
            } else if(expressionStructure.is_number()) {
                std::string stringrepr = expressionStructure.dump();
                try {
                    // It has to be an integer whenever it represents an integer.
                    int64_t intval = boost::lexical_cast<int64_t>(stringrepr);
                    return expressionManager->integer(intval);
                } catch (boost::bad_lexical_cast const&) {
                    // For now, just take the double.
                    // TODO make this a rational number
                    return expressionManager->rational(expressionStructure.get<double>());
                }
            }

        }



        void JaniParser::parseActions(json const& actionStructure, storm::jani::Model& parentModel) {
            std::set<std::string> actionNames;
            for(auto const& actionEntry : actionStructure) {
                STORM_LOG_THROW(actionEntry.count("name") == 1, storm::exceptions::InvalidJaniException, "Actions must have exactly one name.");
                std::string actionName = getString(actionEntry.at("name"), "name of action");
                STORM_LOG_THROW(actionNames.count(actionName) == 0, storm::exceptions::InvalidJaniException, "Action with name " + actionName + " already exists.");
                parentModel.addAction(storm::jani::Action(actionName));
                actionNames.emplace(actionName);
            }
        }

        storm::jani::Automaton JaniParser::parseAutomaton(json const &automatonStructure) {
            STORM_LOG_THROW(automatonStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Each automaton must have a name");
            std::string name = getString(automatonStructure.at("name"), " the name field for automaton");
            storm::jani::Automaton automaton(name);
            STORM_LOG_THROW(automatonStructure.count("locations") > 0, storm::exceptions::InvalidJaniException, "Automaton " << name << " does not have locations.");
            std::unordered_map<std::string, uint64_t> locIds;
            for(auto const& locEntry : automatonStructure.at("locations")) {
                STORM_LOG_THROW(locEntry.count("name"), storm::exceptions::InvalidJaniException, "Locations for automaton " << name << " must have exactly one name");
                std::string locName = getString(locEntry.at("name"), "location of automaton " + name);
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