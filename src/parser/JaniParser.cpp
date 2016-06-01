#include "JaniParser.h"
#include "src/storage/jani/Model.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/InvalidJaniException.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/storage/jani/ModelType.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/lexical_cast.hpp>

#include "src/utility/macros.h"

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
            // Automatons can only be parsed after constants and variables.
            for(auto const& automataEntry : parsedStructure.at("automata")) {
                storm::jani::Automaton automaton = parseAutomaton(automataEntry);

            }
            STORM_LOG_THROW(parsedStructure.count("system") == 1, storm::exceptions::InvalidJaniException, "Exactly one system description must be given");
            std::shared_ptr<storm::jani::Composition> composition = parseComposition(parsedStructure.at("system"));

            return model;
        }

        std::shared_ptr<storm::jani::Variable> JaniParser::parseVariable(json const &variableStructure, std::string const& scopeDescription, bool prefWithScope) {
            STORM_LOG_THROW(variableStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Variable (scope: " + scopeDescription + ") must have a name");
            std::string pref = prefWithScope  ? scopeDescription + "." : "";
            std::string name = getString(variableStructure.at("name"), "variable-name in " + scopeDescription + "-scope");
            // TODO check existance of name.
            // TODO store prefix in variable.
            std::string exprManagerName = pref + name;
            STORM_LOG_THROW(variableStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "Variable '" + name + "' (scope: " + scopeDescription + ") must have a (single) type-declaration.");
            // TODO DEPRECATED make initial value optional?  --- still rpesent in files, so keep it for now
            STORM_LOG_THROW(variableStructure.count("initial-value") == 1, storm::exceptions::InvalidJaniException, "Initial value for variable '" + name +  "' +  (scope: " + scopeDescription + ") must be given once.");
            // Read initial value before; that makes creation later on a bit easier, and has as an additional benefit that we do not need to check whether the variable occurs also on the assignment.
            storm::expressions::Expression initExpr = parseExpression(variableStructure.at("initial-value"), "Initial value of variable " + name + " (scope: " + scopeDescription + ")");
            assert(initExpr.isInitialized());
            if(variableStructure.at("type").is_string()) {
                if(variableStructure.at("type") == "real") {
                    // expressionManager->declareRationalVariable(name);
                    // TODO something.
                } else if(variableStructure.at("type") == "bool") {
                    STORM_LOG_THROW(initExpr.hasBooleanType(), storm::exceptions::InvalidJaniException, "Initial value for Boolean variable " << name << " (scope: " << scopeDescription << ") should have Boolean type.");
                    // TODO: reenable and put initExpr in the place where it belongs.
//                    return std::make_shared<storm::jani::BooleanVariable>(name, expressionManager->declareBooleanVariable(exprManagerName), initExpr);
                } else if(variableStructure.at("type") == "int") {
                    STORM_LOG_THROW(initExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Initial value for interger variable "  << name << " (scope: " << scopeDescription << ") should have integer type.");
                    // TODO: reenable and put initExpr in the place where it belongs.
//                    return std::make_shared<storm::jani::UnboundedIntegerVariable>(name, expressionManager->declareIntegerVariable(exprManagerName), initExpr);
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
                        // TODO: reenable and put initExpr in the place where it belongs.
//                        return std::make_shared<storm::jani::BoundedIntegerVariable>(name, expressionManager->declareIntegerVariable(exprManagerName), lowerboundExpr, upperboundExpr, initExpr);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported base " << basictype << " for bounded variable " << name << "(scope: " << scopeDescription << ") ");
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported kind " << kind << " for complex type of variable " << name << "(scope: " << scopeDescription << ") ");
                }
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description, " << variableStructure.at("type").dump()  << " for Variable '" << name << "' (scope: " << scopeDescription << ")");
        }

        /**
         * Helper for parse expression.
         */
        void ensureNumberOfArguments(uint64_t expected, uint64_t actual, std::string const& opstring, std::string const& errorInfo) {
            STORM_LOG_THROW(expected == actual, storm::exceptions::InvalidJaniException, "Operator " << opstring  << " expects " << expected << " arguments, but got " << actual << " in " << errorInfo << ".");
        }
        /**
         * Helper for parse expression.
         */
        void ensureBooleanType(storm::expressions::Expression const& expr, std::string const& opstring, unsigned argNr, std::string const& errorInfo) {
            STORM_LOG_THROW(expr.hasBooleanType(), storm::exceptions::InvalidJaniException, "Operator " << opstring << " expects argument " + std::to_string(argNr) + " to be Boolean in " << errorInfo << ".");
        }

        /**
         * Helper for parse expression.
         */
        void ensureNumericalType(storm::expressions::Expression const& expr, std::string const& opstring, unsigned argNr, std::string const& errorInfo) {
            STORM_LOG_THROW(expr.hasNumericalType(), storm::exceptions::InvalidJaniException, "Operator " << opstring << " expects argument " + std::to_string(argNr) + " to be numerical in " << errorInfo << ".");
        }


        storm::expressions::Expression JaniParser::parseExpression(json const& expressionStructure, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars) {
            if(expressionStructure.is_boolean()) {
                if(expressionStructure.get<bool>()) {
                    return expressionManager->boolean(true);
                } else {
                    return expressionManager->boolean(false);
                }
            } else if(expressionStructure.is_number_integer()) {
                return expressionManager->integer(expressionStructure.get<uint64_t>());
            } else if(expressionStructure.is_number_float()) {
                // For now, just take the double.
                // TODO make this a rational number
                return expressionManager->rational(expressionStructure.get<double>());
            } else if(expressionStructure.is_string()) {
                std::string ident = expressionStructure.get<std::string>();
                if(localVars.count(ident) == 1) {
                    return storm::expressions::Expression(localVars.at(ident)->getExpressionVariable());
                } else {
                    STORM_LOG_THROW(expressionManager->hasVariable(ident), storm::exceptions::InvalidJaniException, "Unknown identifier '" << ident << "' occurs in  " << scopeDescription);
                    return expressionManager->getVariableExpression(ident);
                }
            } else if(expressionStructure.is_object()) {
                if(expressionStructure.count("op") == 1) {
                    std::string opstring = getString(expressionStructure.at("op"), scopeDescription);
                    STORM_LOG_THROW(expressionStructure.count("args") == 1, storm::exceptions::InvalidJaniException, "Operation arguments are not given in " << expressionStructure.dump() << " in " << scopeDescription << "." );
                    std::vector<storm::expressions::Expression> arguments;
                    unsigned i = 1;
                    for(json const& argStructure : expressionStructure.at("args")) {
                        arguments.push_back(parseExpression(argStructure, scopeDescription + "in argument " + std::to_string(i), localVars));
                        assert(arguments.back().isInitialized());
                        ++i;
                    }
                    if(opstring == "?:") {
                        ensureNumberOfArguments(3, arguments.size(), opstring, scopeDescription);
                        assert(arguments.size() == 3);
                        return storm::expressions::ite(arguments[0], arguments[1], arguments[2]);
                    } else if (opstring == "∨") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                        assert(arguments.size() == 2);
                        return arguments[0] || arguments[1];
                    } else if (opstring == "∧") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                        assert(arguments.size() == 2);
                        return arguments[0] && arguments[1];
                    } else if (opstring == "!") {
                        ensureNumberOfArguments(1, arguments.size(), opstring, scopeDescription);
                        ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        assert(arguments.size() == 1);
                        return !arguments[0];
                    } else if (opstring == "=") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        assert(arguments.size() == 2);
                        if(arguments[0].hasBooleanType()) {
                            ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                            return storm::expressions::iff(arguments[0], arguments[1]);
                        } else {
                            ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                            return arguments[0] == arguments[1];
                        }
                    } else if (opstring == "≠") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        assert(arguments.size() == 2);
                        if(arguments[0].hasBooleanType()) {
                            ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                            return storm::expressions::xclusiveor(arguments[0], arguments[1]);
                        } else {
                            ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                            return arguments[0] != arguments[1];
                        }
                    } else if (opstring == "<") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] < arguments[1];
                    } else if (opstring == "≤") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] <= arguments[1];
                    } else if (opstring == ">") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] > arguments[1];
                    } else if (opstring == "≥") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] >= arguments[1];
                    } else if (opstring == "+") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] + arguments[1];
                    } else if (opstring == "-") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] - arguments[1];
                    } else if (opstring == "--") {
                        ensureNumberOfArguments(1, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        assert(arguments.size() == 1);
                        return -arguments[0];
                    } else if (opstring == "*") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] * arguments[1];
                    } else if (opstring == "/") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] / arguments[1];
                    } else if (opstring == "%") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "modulo operation is not yet implemented");
                    } else if (opstring == "max") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return storm::expressions::maximum(arguments[0],arguments[1]);
                    } else if (opstring == "min") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return storm::expressions::minimum(arguments[0],arguments[1]);
                    } else if (opstring == "⌊⌋") {
                        ensureNumberOfArguments(1, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::floor(arguments[0]);
                    } else if (opstring == "⌈⌉") {
                        ensureNumberOfArguments(1, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::ceil(arguments[0]);
                    } else if (opstring == "abs") {
                        ensureNumberOfArguments(1, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::abs(arguments[0]);
                    } else if (opstring == "sgn") {
                        ensureNumberOfArguments(1, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::sign(arguments[0]);
                    } else if (opstring == "trc") {
                        ensureNumberOfArguments(1, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::abs(arguments[0]);
                    } else if (opstring == "pow") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "pow operation is not yet implemented");
                    } else if (opstring == "exp") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "exp operation is not yet implemented");
                    } else if (opstring == "log") {
                        ensureNumberOfArguments(2, arguments.size(), opstring, scopeDescription);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "log operation is not yet implemented");
                    }  else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown operator " << opstring << " in  " << scopeDescription << ".");
                    }
                }
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Only standard operators are supported for complex expressions as " << expressionStructure.dump() << " in  " << scopeDescription << ".");
            }
            assert(false);

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
            STORM_LOG_THROW(automatonStructure.count("locations") > 0, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' does not have locations.");
            std::unordered_map<std::string, uint64_t> locIds;
            for(auto const& locEntry : automatonStructure.at("locations")) {
                STORM_LOG_THROW(locEntry.count("name"), storm::exceptions::InvalidJaniException, "Locations for automaton '" << name << "' must have exactly one name");
                std::string locName = getString(locEntry.at("name"), "location of automaton " + name);
                STORM_LOG_THROW(locIds.count(locName) == 0, storm::exceptions::InvalidJaniException, "Location with name '" + locName + "' already exists in automaton '" + name + "'");
                uint64_t id = automaton.addLocation(storm::jani::Location(locName));
                locIds.emplace(locName, id);
            }
            uint64_t varDeclCount = automatonStructure.count("variables");
            STORM_LOG_THROW(varDeclCount < 2, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' has more than one list of variables");
            std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> localVars;
            if(varDeclCount > 0) {
                for(auto const& varStructure : automatonStructure.at("variables")) {
                    std::shared_ptr<storm::jani::Variable> var = parseVariable(varStructure, name, true);
                    std::cout << "Automaton " << name << "     variable " << var->getName() << std::endl;
                    assert(localVars.count(var->getName()) == 0);
                    localVars.emplace(var->getName(), var);
                }
            }


            STORM_LOG_THROW(automatonStructure.count("edges") > 0, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' must have a list of edges");
            for(auto const& edgeEntry : automatonStructure.at("edges")) {
                // source location
                STORM_LOG_THROW(edgeEntry.count("location") == 1, storm::exceptions::InvalidJaniException, "Each edge in automaton '" << name << "' must have a source");
                std::string sourceLoc = getString(edgeEntry.at("location"), "source location for edge in automaton '" + name + "'");
                STORM_LOG_THROW(locIds.count(sourceLoc) == 1, storm::exceptions::InvalidJaniException, "Source of edge has unknown location '" << sourceLoc << "' in automaton '" << name << "'.");
                // action
                STORM_LOG_THROW(edgeEntry.count("action") < 2, storm::exceptions::InvalidJaniException, "Edge from " << sourceLoc << " in automaton " << name << " has multiple actions");
                std::string action = ""; // def is tau
                if(edgeEntry.count("action") > 0) {
                    action = getString(edgeEntry.at("action"), "action name in edge from '" + sourceLoc + "' in automaton '" + name + "'");
                    // TODO check if action is known
                    assert(action != "");
                }
                // rate
                STORM_LOG_THROW(edgeEntry.count("rate") < 2, storm::exceptions::InvalidJaniException, "Edge from '" << sourceLoc << "' in automaton '" << name << "' has multiple rates");
                storm::expressions::Expression rateExpr;
                if(edgeEntry.count("rate") > 0) {
                    rateExpr = parseExpression(edgeEntry.at("rate"), "rate expression in edge from '" + sourceLoc + "' in automaton '" + name + "'");
                    STORM_LOG_THROW(rateExpr.hasNumericalType(), storm::exceptions::InvalidJaniException, "Rate '" << rateExpr << "' has not a numerical type");
                }
                // guard
                STORM_LOG_THROW(edgeEntry.count("guard") == 1, storm::exceptions::InvalidJaniException, "A single guard must be given in edge from '" << sourceLoc << "' in automaton '" << name << "'");
                storm::expressions::Expression guardExpr = parseExpression(edgeEntry.at("guard"), "guard expression in edge from '" + sourceLoc + "' in automaton '" + name + "'", localVars);
                STORM_LOG_THROW(guardExpr.hasBooleanType(), storm::exceptions::InvalidJaniException, "Guard " << guardExpr << " does not have Boolean type.");
                STORM_LOG_THROW(edgeEntry.count("destinations") == 1, storm::exceptions::InvalidJaniException, "A single list of destinations must be given in edge from '" << sourceLoc << "' in automaton '" << name << "'");
                for(auto const& destEntry : edgeEntry.at("destinations")) {
                    // target location
                    STORM_LOG_THROW(edgeEntry.count("location") == 1, storm::exceptions::InvalidJaniException, "Each destination in edge from '" << sourceLoc << "' in automaton '" << name << "' must have a source");
                    std::string targetLoc = getString(edgeEntry.at("location"), "target location for edge from '" + sourceLoc + "' in automaton '" + name + "'");
                    STORM_LOG_THROW(locIds.count(targetLoc) == 1, storm::exceptions::InvalidJaniException, "Target of edge has unknown location '" << targetLoc << "' in automaton '" << name << "'.");
                    // probability
                    storm::expressions::Expression probExpr;
                    unsigned probDeclCount = destEntry.count("probability");
                    STORM_LOG_THROW(probDeclCount, storm::exceptions::InvalidJaniException, "Destination in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "' has multiple probabilites");
                    if(probDeclCount == 0) {
                        probExpr = expressionManager->rational(1.0);
                    } else {
                        probExpr = parseExpression(destEntry.at("probability"), "probability expression in edge from '" + sourceLoc + "' to  '"  + targetLoc + "' in automaton '" + name + "'", localVars);
                    }
                    assert(probExpr.isInitialized());
                    STORM_LOG_THROW(probExpr.hasRationalType(), storm::exceptions::InvalidJaniException, "Probability expr " << probExpr << " does not have rational type." );
                    // assignments

                }
            }



            return automaton;
        }

        std::shared_ptr<storm::jani::Composition> JaniParser::parseComposition(json const &compositionStructure) {

        }
    }
}