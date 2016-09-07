#include "JaniParser.h"
#include "src/storage/jani/Model.h"
#include "src/storage/jani/Property.h"
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

        ////////////
        // Defaults
        ////////////
        const bool JaniParser::defaultVariableTransient = false;
        const bool JaniParser::defaultBooleanInitialValue = false;
        const int64_t JaniParser::defaultIntegerInitialValue = 0;
        const std::set<std::string> JaniParser::unsupportedOpstrings({"sin", "cos", "tan", "cot", "sec", "csc", "asin", "acos", "atan", "acot", "asec", "acsc",
                                                         "sinh", "cosh", "tanh", "coth", "sech", "csch", "asinh", "acosh", "atanh", "asinh", "acosh"});


        std::string getString(json const& structure, std::string const& errorInfo) {
            STORM_LOG_THROW(structure.is_string(), storm::exceptions::InvalidJaniException, "Expected a string in " << errorInfo << ", got '" << structure.dump() << "'");
            return structure.front();
        }

        bool getBoolean(json const& structure, std::string const& errorInfo) {
            STORM_LOG_THROW(structure.is_boolean(), storm::exceptions::InvalidJaniException, "Expected a Boolean in " << errorInfo << ", got " << structure.dump() << "'");
            return structure.front();
        }

        uint64_t getUnsignedInt(json const& structure, std::string const& errorInfo) {
            STORM_LOG_THROW(structure.is_number(), storm::exceptions::InvalidJaniException, "Expected a number in " << errorInfo << ", got '" << structure.dump() << "'");
            int num = structure.front();
            STORM_LOG_THROW(num >= 0, storm::exceptions::InvalidJaniException, "Expected a positive number in " << errorInfo << ", got '" << num << "'");
            return static_cast<uint64_t>(num);
        }


        std::pair<storm::jani::Model, std::vector<storm::jani::Property>> JaniParser::parse(std::string const& path) {
            JaniParser parser;
            parser.readFile(path);
            return parser.parseModel();
        }

        JaniParser::JaniParser(std::string const& jsonstring) {
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

        std::pair<storm::jani::Model, std::vector<storm::jani::Property>> JaniParser::parseModel(bool parseProperties) {
            //jani-version
            STORM_LOG_THROW(parsedStructure.count("jani-version") == 1, storm::exceptions::InvalidJaniException, "Jani-version must be given exactly once.");
            uint64_t version = getUnsignedInt(parsedStructure.at("jani-version"), "jani version");
            //name
            STORM_LOG_THROW(parsedStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "A model must have a (single) name");
            std::string name = getString(parsedStructure.at("name"), "model name");
            //model type
            STORM_LOG_THROW(parsedStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "A type must be given exactly once");
            std::string modeltypestring = getString(parsedStructure.at("type"), "type of the model");
            storm::jani::ModelType type = storm::jani::getModelType(modeltypestring);
            STORM_LOG_THROW(type != storm::jani::ModelType::UNDEFINED, storm::exceptions::InvalidJaniException, "model type " + modeltypestring + " not recognized");
            storm::jani::Model model(name, type, version, expressionManager);
            STORM_LOG_THROW(parsedStructure.count("actions") < 2, storm::exceptions::InvalidJaniException, "Action-declarations can be given at most once.");
            parseActions(parsedStructure.at("actions"), model);
            size_t constantsCount = parsedStructure.count("constants");
            STORM_LOG_THROW(constantsCount < 2, storm::exceptions::InvalidJaniException, "Constant-declarations can be given at most once.");
            if(constantsCount == 1) {
                for (auto const &constStructure : parsedStructure.at("constants")) {
                    model.addConstant(*parseConstant(constStructure, "global"));
                }
            }
            size_t variablesCount = parsedStructure.count("variables");
            STORM_LOG_THROW(variablesCount < 2, storm::exceptions::InvalidJaniException, "Variable-declarations can be given at most once for global variables.");
            if(variablesCount == 1) {
                for(auto const& varStructure : parsedStructure.at("variables")) {
                    model.addVariable(*parseVariable(varStructure, "global"));
                }
            }
            STORM_LOG_THROW(parsedStructure.count("automata") == 1, storm::exceptions::InvalidJaniException, "Exactly one list of automata must be given");
            STORM_LOG_THROW(parsedStructure.at("automata").is_array(), storm::exceptions::InvalidJaniException, "Automata must be an array");
            // Automatons can only be parsed after constants and variables.
            for(auto const& automataEntry : parsedStructure.at("automata")) {
                model.addAutomaton(parseAutomaton(automataEntry, model));
            }
            STORM_LOG_THROW(parsedStructure.count("system") == 1, storm::exceptions::InvalidJaniException, "Exactly one system description must be given");
            //std::shared_ptr<storm::jani::Composition> composition = parseComposition(parsedStructure.at("system"));
            STORM_LOG_THROW(parsedStructure.count("properties") <= 1, storm::exceptions::InvalidJaniException, "At most one list of properties can be given");
            STORM_LOG_THROW(parsedStructure.at("properties").is_array(), storm::exceptions::InvalidJaniException, "Properties should be an array");
            PropertyVector properties;
            if(parseProperties) {
                for(auto const& propertyEntry : parsedStructure.at("properties")) {
                    properties.push_back(this->parseProperty(propertyEntry));
                }
            }
            return {model, properties};
        }


        storm::jani::Property JaniParser::parseProperty(json const& propertyStructure) {
            STORM_LOG_THROW(propertyStructure.count("name") ==  1, storm::exceptions::InvalidJaniException, "Property must have a name");
            // TODO check unique name
            std::string name = getString(propertyStructure.at("name"), "property-name");
            std::string comment = "";
            if(propertyStructure.count("comment") > 0) {
                comment = getString(propertyStructure.at("comment"), "comment for property named '" + name + "'.");
            }

        }

        std::shared_ptr<storm::jani::Constant> JaniParser::parseConstant(json const& constantStructure, std::string const& scopeDescription) {
            STORM_LOG_THROW(constantStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Variable (scope: " + scopeDescription + ") must have a name");
            std::string name = getString(constantStructure.at("name"), "variable-name in " + scopeDescription + "-scope");
            // TODO check existance of name.
            // TODO store prefix in variable.
            std::string exprManagerName = name;
            STORM_LOG_THROW(constantStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "Constant '" + name + "' (scope: " + scopeDescription + ") must have a (single) type-declaration.");
            size_t valueCount = constantStructure.count("value");
            storm::expressions::Expression initExpr;
            STORM_LOG_THROW(valueCount < 2, storm::exceptions::InvalidJaniException, "Value for constant '" + name +  "'  (scope: " + scopeDescription + ") must be given at most once.");
            if(valueCount == 1) {
                // Read initial value before; that makes creation later on a bit easier, and has as an additional benefit that we do not need to check whether the variable occurs also on the assignment.
                initExpr = parseExpression(constantStructure.at("value"), "Value of constant " + name + " (scope: " + scopeDescription + ")");
                assert(initExpr.isInitialized());
            }

            if(constantStructure.at("type").is_object()) {
//                STORM_LOG_THROW(variableStructure.at("type").count("kind") == 1, storm::exceptions::InvalidJaniException, "For complex type as in variable " << name << "(scope: " << scopeDescription << ")  kind must be given");
//                std::string kind = getString(variableStructure.at("type").at("kind"), "kind for complex type as in variable " + name  + "(scope: " + scopeDescription + ") ");
//                if(kind == "bounded") {
//                    // First do the bounds, that makes the code a bit more streamlined
//                    STORM_LOG_THROW(variableStructure.at("type").count("lower-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") lower-bound must be given");
//                    storm::expressions::Expression lowerboundExpr = parseExpression(variableStructure.at("type").at("lower-bound"), "Lower bound for variable "+ name + " (scope: " + scopeDescription + ")");
//                    assert(lowerboundExpr.isInitialized());
//                    STORM_LOG_THROW(variableStructure.at("type").count("upper-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") upper-bound must be given");
//                    storm::expressions::Expression upperboundExpr = parseExpression(variableStructure.at("type").at("upper-bound"), "Upper bound for variable "+ name + " (scope: " + scopeDescription + ")");
//                    assert(upperboundExpr.isInitialized());
//                    STORM_LOG_THROW(variableStructure.at("type").count("base") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") base must be given");
//                    std::string basictype = getString(variableStructure.at("type").at("base"), "base for bounded type as in variable " + name  + "(scope: " + scopeDescription + ") ");
//                    if(basictype == "int") {
//                        STORM_LOG_THROW(lowerboundExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Lower bound for bounded integer variable " << name << "(scope: " << scopeDescription << ") must be integer-typed");
//                        STORM_LOG_THROW(upperboundExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Upper bound for bounded integer variable " << name << "(scope: " << scopeDescription << ") must be integer-typed");
//                        return std::make_shared<storm::jani::BoundedIntegerVariable>(name, expressionManager->declareIntegerVariable(exprManagerName), lowerboundExpr, upperboundExpr);
//                    } else {
//                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported base " << basictype << " for bounded variable " << name << "(scope: " << scopeDescription << ") ");
//                    }
//                } else {
//                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported kind " << kind << " for complex type of variable " << name << "(scope: " << scopeDescription << ") ");
//                }
             }
             else if(constantStructure.at("type").is_string()) {
                if(constantStructure.at("type") == "real") {
                    // expressionManager->declareRationalVariable(name);
                    // TODO something.
                } else if(constantStructure.at("type") == "bool") {
                    if(initExpr.isInitialized()) {
                        return std::make_shared<storm::jani::Constant>(name, expressionManager->declareBooleanVariable(exprManagerName), initExpr);
                    } else {
                        return std::make_shared<storm::jani::Constant>(name, expressionManager->declareBooleanVariable(exprManagerName));
                    }

                } else if(constantStructure.at("type") == "int") {
                    if(initExpr.isInitialized()) {
                        return std::make_shared<storm::jani::Constant>(name, expressionManager->declareIntegerVariable(exprManagerName), initExpr);
                    } else {
                        return std::make_shared<storm::jani::Constant>(name, expressionManager->declareIntegerVariable(exprManagerName));
                    }
                } else {
                    // TODO clocks.
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description " << constantStructure.at("type").dump()  << " for Variable '" << name << "' (scope: " << scopeDescription << ")");
                }
            }

            STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description, " << constantStructure.at("type").dump()  << " for Variable '" << name << "' (scope: " << scopeDescription << ")");
        }

        std::shared_ptr<storm::jani::Variable> JaniParser::parseVariable(json const &variableStructure, std::string const& scopeDescription, bool prefWithScope) {
            STORM_LOG_THROW(variableStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Variable (scope: " + scopeDescription + ") must have a name");
            std::string pref = prefWithScope  ? scopeDescription + "." : "";
            std::string name = getString(variableStructure.at("name"), "variable-name in " + scopeDescription + "-scope");
            // TODO check existance of name.
            // TODO store prefix in variable.
            std::string exprManagerName = pref + name;
            bool transientVar = defaultVariableTransient; // Default value for variables.
            size_t tvarcount = variableStructure.count("transient");
            STORM_LOG_THROW(tvarcount <= 1, storm::exceptions::InvalidJaniException, "Multiple definitions of transient not allowed in variable '" + name  + "' (scope: " + scopeDescription + ")  ");
            if(tvarcount == 1) {
                transientVar = getBoolean(variableStructure.at("transient"), "transient-attribute in variable '" + name  + "' (scope: " + scopeDescription + ")  ");
            }
            size_t initvalcount = variableStructure.count("initial-value");
            if(transientVar) {
                STORM_LOG_THROW(initvalcount == 1, storm::exceptions::InvalidJaniException, "Initial value must be given once for transient variable '" + name + "' (scope: " + scopeDescription + ")  "+ name + "' (scope: " + scopeDescription + ")  ");
            } else {
                STORM_LOG_THROW(initvalcount <= 1, storm::exceptions::InvalidJaniException, "Initial value can be given at most one for variable " + name + "' (scope: " + scopeDescription + ")");
            }
            STORM_LOG_THROW(variableStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "Variable '" + name + "' (scope: " + scopeDescription + ") must have a (single) type-declaration.");
            boost::optional<storm::expressions::Expression> initVal;
            if(variableStructure.at("type").is_object()) {
                STORM_LOG_THROW(variableStructure.at("type").count("kind") == 1, storm::exceptions::InvalidJaniException, "For complex type as in variable " << name << "(scope: " << scopeDescription << ")  kind must be given");
                std::string kind = getString(variableStructure.at("type").at("kind"), "kind for complex type as in variable " + name  + "(scope: " + scopeDescription + ") ");
                if(kind == "bounded") {
                    // First do the bounds, that makes the code a bit more streamlined
                    STORM_LOG_THROW(variableStructure.at("type").count("lower-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") lower-bound must be given");
                    storm::expressions::Expression lowerboundExpr = parseExpression(variableStructure.at("type").at("lower-bound"), "Lower bound for variable " + name + " (scope: " + scopeDescription + ")");
                    assert(lowerboundExpr.isInitialized());
                    STORM_LOG_THROW(variableStructure.at("type").count("upper-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") upper-bound must be given");
                    storm::expressions::Expression upperboundExpr = parseExpression(variableStructure.at("type").at("upper-bound"), "Upper bound for variable "+ name + " (scope: " + scopeDescription + ")");
                    assert(upperboundExpr.isInitialized());
                    STORM_LOG_THROW(variableStructure.at("type").count("base") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") base must be given");
                    if(initvalcount == 1) {
                        if(variableStructure.at("initial-value").is_null()) {
                            initVal = storm::expressions::ite(lowerboundExpr < 0 && upperboundExpr > 0, expressionManager->integer(0), lowerboundExpr);
                                                                                          // TODO as soon as we support half-open intervals, we have to change this.
                        } else {
                            initVal = parseExpression(variableStructure.at("initial-value"), "Initial value for variable " + name + " (scope: " + scopeDescription + ") ");
                        }
                    }
                    std::string basictype = getString(variableStructure.at("type").at("base"), "base for bounded type as in variable " + name  + "(scope: " + scopeDescription + ") ");
                    if(basictype == "int") {
                        if(initVal) {
                            STORM_LOG_THROW(initVal.get().hasIntegerType(), storm::exceptions::InvalidJaniException, "Initial value for integer variable " + name + "(scope " + scopeDescription + ") should be an integer");
                        }
                        STORM_LOG_THROW(lowerboundExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Lower bound for bounded integer variable " << name << "(scope: " << scopeDescription << ") must be integer-typed");
                        STORM_LOG_THROW(upperboundExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Upper bound for bounded integer variable " << name << "(scope: " << scopeDescription << ") must be integer-typed");
                        return storm::jani::makeBoundedIntegerVariable(name, expressionManager->declareIntegerVariable(exprManagerName), initVal, transientVar, lowerboundExpr, upperboundExpr);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported base " << basictype << " for bounded variable " << name << "(scope: " << scopeDescription << ") ");
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported kind " << kind << " for complex type of variable " << name << "(scope: " << scopeDescription << ") ");
                }
            }
            else if(variableStructure.at("type").is_string()) {
                if(variableStructure.at("type") == "real") {
                    // expressionManager->declareRationalVariable(name);
                    // TODO something.
                } else if(variableStructure.at("type") == "bool") {
                    if(initvalcount == 1) {
                        if(variableStructure.at("initial-value").is_null()) {
                            initVal = expressionManager->boolean(defaultBooleanInitialValue);
                        } else {
                            initVal = parseExpression(variableStructure.at("initial-value"), "Initial value for variable " + name + " (scope: " + scopeDescription + ") ");
                            STORM_LOG_THROW(initVal.get().hasIntegerType(), storm::exceptions::InvalidJaniException, "Initial value for integer variable " + name + "(scope " + scopeDescription + ") should be a Boolean");
                        }
                        return std::make_shared<storm::jani::BooleanVariable>(name, expressionManager->declareBooleanVariable(exprManagerName), initVal.get(), transientVar);
                    }
                    return std::make_shared<storm::jani::BooleanVariable>(name, expressionManager->declareBooleanVariable(exprManagerName), transientVar);
                } else if(variableStructure.at("type") == "int") {
                    if(initvalcount == 1) {
                        if(variableStructure.at("initial-value").is_null()) {
                            initVal = expressionManager->integer(defaultIntegerInitialValue);
                        } else {
                            initVal = parseExpression(variableStructure.at("initial-value"), "Initial value for variable " + name + " (scope: " + scopeDescription + ") ");
                            STORM_LOG_THROW(initVal.get().hasIntegerType(), storm::exceptions::InvalidJaniException, "Initial value for integer variable " + name + "(scope " + scopeDescription + ") should be an integer");
                        }
                        return std::make_shared<storm::jani::UnboundedIntegerVariable>(name, expressionManager->declareIntegerVariable(exprManagerName), initVal.get(), transientVar);
                    }
                    return std::make_shared<storm::jani::UnboundedIntegerVariable>(name, expressionManager->declareIntegerVariable(exprManagerName), transientVar);
                } else if(variableStructure.at("type") == "clock") {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported type 'clock' for variable '" << name << "' (scope: " << scopeDescription << ")");
                } else if(variableStructure.at("type") == "continuous") {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported type 'continuous' for variable ''" << name << "' (scope: " << scopeDescription << ")");
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description " << variableStructure.at("type").dump()  << " for variable '" << name << "' (scope: " << scopeDescription << ")");
                }
            }

            STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description, " << variableStructure.at("type").dump()  << " for variable '" << name << "' (scope: " << scopeDescription << ")");
        }

        /**
         * Helper for parse expression.
         */
        void ensureNumberOfArguments(uint64_t expected, uint64_t actual, std::string const& opstring, std::string const& errorInfo) {
            STORM_LOG_THROW(expected == actual, storm::exceptions::InvalidJaniException, "Operator " << opstring  << " expects " << expected << " arguments, but got " << actual << " in " << errorInfo << ".");
        }

        std::vector<storm::expressions::Expression> JaniParser::parseUnaryExpressionArguments(json const& expressionDecl, std::string const& opstring, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars) {
            storm::expressions::Expression left = parseExpression(expressionDecl.at("exp"), "Left argument of operator " + opstring + " in " + scopeDescription, localVars);
            return {left};
        }

        std::vector<storm::expressions::Expression> JaniParser::parseBinaryExpressionArguments(json const& expressionDecl, std::string const& opstring, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars) {
            storm::expressions::Expression left = parseExpression(expressionDecl.at("left"), "Left argument of operator " + opstring + " in " + scopeDescription, localVars);
            storm::expressions::Expression right = parseExpression(expressionDecl.at("right"), "Right argument of operator " + opstring + " in " + scopeDescription, localVars);
            return {left, right};
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

        storm::jani::Variable const& getLValue(std::string const& ident, storm::jani::VariableSet const& globalVars, storm::jani::VariableSet const& localVars, std::string const& scopeDescription) {
            if(localVars.hasVariable(ident)) {
                return globalVars.getVariable(ident);
            } else if(globalVars.hasVariable(ident)) {
                return globalVars.getVariable(ident);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown identifier '" << ident << "' occurs in  " << scopeDescription);
            }
        }

        storm::expressions::Variable JaniParser::getVariableOrConstantExpression(std::string const &ident,std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars) {
            if(localVars.count(ident) == 1) {
                return localVars.at(ident)->getExpressionVariable();
            } else {
                STORM_LOG_THROW(expressionManager->hasVariable(ident), storm::exceptions::InvalidJaniException, "Unknown identifier '" << ident << "' occurs in  " << scopeDescription);
                return expressionManager->getVariable(ident);
            }
        }

        storm::expressions::Expression JaniParser::parseExpression(json const& expressionStructure, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars) {
            if(expressionStructure.is_boolean()) {
                if(expressionStructure.get<bool>()) {
                    return expressionManager->boolean(true);
                } else {
                    return expressionManager->boolean(false);
                }
            } else if(expressionStructure.is_number_integer()) {
                return expressionManager->integer(expressionStructure.get<int64_t>());
            } else if(expressionStructure.is_number_float()) {
                // For now, just take the double.
                // TODO make this a rational number
                return expressionManager->rational(expressionStructure.get<double>());
            } else if(expressionStructure.is_string()) {
                std::string ident = expressionStructure.get<std::string>();
                return storm::expressions::Expression(getVariableOrConstantExpression(ident, scopeDescription, localVars));
            } else if(expressionStructure.is_object()) {
                if(expressionStructure.count("distribution") == 1) {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Distributions are not supported by storm expressions, cannot import " << expressionStructure.dump() << " in  " << scopeDescription << ".");
                }
                if(expressionStructure.count("op") == 1) {
                    std::string opstring = getString(expressionStructure.at("op"), scopeDescription);
                    std::vector<storm::expressions::Expression> arguments = {};
                    if(opstring == "?:") {
                        ensureNumberOfArguments(3, arguments.size(), opstring, scopeDescription);
                        assert(arguments.size() == 3);
                        return storm::expressions::ite(arguments[0], arguments[1], arguments[2]);
                    } else if (opstring == "∨") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] || arguments[1];
                    } else if (opstring == "∧") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] && arguments[1];
                    } else if (opstring == "⇒") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        ensureBooleanType(arguments[0], opstring, 1, scopeDescription);
                        return (!arguments[0]) || arguments[1];
                    } else if (opstring == "¬") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 1);
                        ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        return !arguments[0];
                    } else if (opstring == "=") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        if(arguments[0].hasBooleanType()) {
                            ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                            return storm::expressions::iff(arguments[0], arguments[1]);
                        } else {
                            ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                            return arguments[0] == arguments[1];
                        }
                    } else if (opstring == "≠") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        if(arguments[0].hasBooleanType()) {
                            ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                            return storm::expressions::xclusiveor(arguments[0], arguments[1]);
                        } else {
                            ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                            return arguments[0] != arguments[1];
                        }
                    } else if (opstring == "<") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] < arguments[1];
                    } else if (opstring == "≤") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] <= arguments[1];
                    } else if (opstring == ">") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] > arguments[1];
                    } else if (opstring == "≥") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] >= arguments[1];
                    } else if (opstring == "+") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] + arguments[1];
                    } else if (opstring == "-") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] - arguments[1];
                    } else if (opstring == "--") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return -arguments[0];
                    } else if (opstring == "*") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] * arguments[1];
                    } else if (opstring == "/") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] / arguments[1];
                    } else if (opstring == "%") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "modulo operation is not yet implemented");
                    } else if (opstring == "max") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return storm::expressions::maximum(arguments[0],arguments[1]);
                    } else if (opstring == "min") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return storm::expressions::minimum(arguments[0],arguments[1]);
                    } else if (opstring == "⌊⌋") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::floor(arguments[0]);
                    } else if (opstring == "⌈⌉") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::ceil(arguments[0]);
                    } else if (opstring == "abs") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::abs(arguments[0]);
                    } else if (opstring == "sgn") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::sign(arguments[0]);
                    } else if (opstring == "trc") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::abs(arguments[0]);
                    } else if (opstring == "pow") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "pow operation is not yet implemented");
                    } else if (opstring == "exp") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "exp operation is not yet implemented");
                    } else if (opstring == "log") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, localVars);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "log operation is not yet implemented");
                    }  else if (unsupportedOpstrings.count(opstring) > 0){
                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Opstring " + opstring + " is not supported by storm");

                    } else {
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

        storm::jani::Automaton JaniParser::parseAutomaton(json const &automatonStructure, storm::jani::Model const& parentModel) {
            STORM_LOG_THROW(automatonStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Each automaton must have a name");
            std::string name = getString(automatonStructure.at("name"), " the name field for automaton");
            storm::jani::Automaton automaton(name);
            STORM_LOG_THROW(automatonStructure.count("locations") > 0, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' does not have locations.");
            std::unordered_map<std::string, uint64_t> locIds;
            for(auto const& locEntry : automatonStructure.at("locations")) {
                STORM_LOG_THROW(locEntry.count("name") == 1, storm::exceptions::InvalidJaniException, "Locations for automaton '" << name << "' must have exactly one name");
                std::string locName = getString(locEntry.at("name"), "location of automaton " + name);
                STORM_LOG_THROW(locIds.count(locName) == 0, storm::exceptions::InvalidJaniException, "Location with name '" + locName + "' already exists in automaton '" + name + "'");
                STORM_LOG_THROW(locEntry.count("invariant") > 0, storm::exceptions::InvalidJaniException, "Invariants in locations as in '" + locName + "' are not supported");
                //STORM_LOG_THROW(locEntry.count("invariant") > 0 && !supportsInvariants(parentModel.getModelType()), storm::exceptions::InvalidJaniException, "Invariants are not supported in the model type " + to_string(parentModel.getModelType()));
                std::vector<storm::jani::Assignment> transientAssignments;
                for(auto const& transientValueEntry : locEntry.at("transient-values")) {
                    STORM_LOG_THROW(transientValueEntry.count("ref") == 1, storm::exceptions::InvalidJaniException, "Transient values in location " << locName << " need exactly one ref that is assigned to");
                    STORM_LOG_THROW(transientValueEntry.count("value") == 1, storm::exceptions::InvalidJaniException, "Transient values in location " << locName << " need exactly one assigned value");
                    storm::jani::Variable const& lhs = getLValue(transientValueEntry.at("ref"), parentModel.getGlobalVariables(), automaton.getVariables(), "LHS of assignment in location " + locName + " (automaton '" + name + "')");
                    STORM_LOG_THROW(lhs.isTransient(), storm::exceptions::InvalidJaniException, "Assigned non-transient variable " + lhs.getName() + " in location " + locName + " (automaton: '" + name + "')");
                    storm::expressions::Expression rhs = parseExpression(transientValueEntry.at("value"), "Assignment of variable " + lhs.getName() + " in location " + locName + " (automaton: '" + name + "')");
                    transientAssignments.emplace_back(lhs, rhs);
                }
                uint64_t id = automaton.addLocation(storm::jani::Location(locName, transientAssignments));
                locIds.emplace(locName, id);
            }
            STORM_LOG_THROW(automatonStructure.count("initial-locations") == 1, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' does not have initial locations.");
            for(json const& initLocStruct : automatonStructure.at("initial-locations")) {
                automaton.addInitialLocation(getString(initLocStruct, "Initial locations for automaton '" + name + "'."));
            }
            STORM_LOG_THROW(automatonStructure.count("restrict-initial") < 2, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' has multiple initial value restrictions");
            storm::expressions::Expression initialValueRestriction = expressionManager->boolean(true);
            if(automatonStructure.count("restrict-initial") > 0) {
                initialValueRestriction  = parseExpression(automatonStructure.at("restrict-initial"), "Initial value restriction for automaton " + name);
            }
            automaton.setInitialStatesRestriction(initialValueRestriction);
            uint64_t varDeclCount = automatonStructure.count("variables");
            STORM_LOG_THROW(varDeclCount < 2, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' has more than one list of variables");
            std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> localVars;
            if(varDeclCount > 0) {
                for(auto const& varStructure : automatonStructure.at("variables")) {
                    std::shared_ptr<storm::jani::Variable> var = parseVariable(varStructure, name, true);
                    assert(localVars.count(var->getName()) == 0);
                    automaton.addVariable(*var);
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
                STORM_LOG_THROW(edgeEntry.count("guard") <= 1, storm::exceptions::InvalidJaniException, "Guard can be given at most once in edge from '" << sourceLoc << "' in automaton '" << name << "'");
                storm::expressions::Expression guardExpr = expressionManager->boolean(true);
                if(edgeEntry.count("guard") == 1) {
                    STORM_LOG_THROW(edgeEntry.at("guard").count("exp") == 1, storm::exceptions::InvalidJaniException, "Guard  in edge from '" + sourceLoc + "' in automaton '" + name + "' must have one expression");
                    guardExpr = parseExpression(edgeEntry.at("guard").at("exp"), "guard expression in edge from '" + sourceLoc + "' in automaton '" + name + "'", localVars);
                    STORM_LOG_THROW(guardExpr.hasBooleanType(), storm::exceptions::InvalidJaniException, "Guard " << guardExpr << " does not have Boolean type.");
                }
                assert(guardExpr.isInitialized());
                STORM_LOG_THROW(edgeEntry.count("destinations") == 1, storm::exceptions::InvalidJaniException, "A single list of destinations must be given in edge from '" << sourceLoc << "' in automaton '" << name << "'");
                std::vector<storm::jani::EdgeDestination> edgeDestinations;
                for(auto const& destEntry : edgeEntry.at("destinations")) {
                    // target location
                    STORM_LOG_THROW(edgeEntry.count("location") == 1, storm::exceptions::InvalidJaniException, "Each destination in edge from '" << sourceLoc << "' in automaton '" << name << "' must have a target location");
                    std::string targetLoc = getString(edgeEntry.at("location"), "target location for edge from '" + sourceLoc + "' in automaton '" + name + "'");
                    STORM_LOG_THROW(locIds.count(targetLoc) == 1, storm::exceptions::InvalidJaniException, "Target of edge has unknown location '" << targetLoc << "' in automaton '" << name << "'.");
                    // probability
                    storm::expressions::Expression probExpr;
                    unsigned probDeclCount = destEntry.count("probability");
                    STORM_LOG_THROW(probDeclCount < 2, storm::exceptions::InvalidJaniException, "Destination in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "' has multiple probabilites");
                    if(probDeclCount == 0) {
                        probExpr = expressionManager->rational(1.0);
                    } else {
                        STORM_LOG_THROW(destEntry.at("probability").count("exp") == 1, storm::exceptions::InvalidJaniException, "Destination in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "' must have one expression.");
                        probExpr = parseExpression(destEntry.at("probability").at("exp"), "probability expression in edge from '" + sourceLoc + "' to  '"  + targetLoc + "' in automaton '" + name + "'", localVars);
                    }
                    assert(probExpr.isInitialized());
                    STORM_LOG_THROW(probExpr.hasNumericalType(), storm::exceptions::InvalidJaniException, "Probability expression " << probExpr << " does not have a numerical type." );
                    // assignments
                    std::vector<storm::jani::Assignment> assignments;
                    unsigned assignmentDeclCount = destEntry.count("assignments");
                    STORM_LOG_THROW(assignmentDeclCount < 2, storm::exceptions::InvalidJaniException, "Destination in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "' has multiple assignment lists");
                    for(auto const& assignmentEntry : destEntry.at("assignments")) {
                        // ref
                        STORM_LOG_THROW(assignmentEntry.count("ref") == 1, storm::exceptions::InvalidJaniException, "Assignment in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "'  must have one ref field");
                        std::string refstring = getString(assignmentEntry.at("ref"), "assignment in edge from '" + sourceLoc + "' to '" + targetLoc + "' in automaton '" + name + "'");
                        storm::jani::Variable const& lhs = getLValue(refstring, parentModel.getGlobalVariables(), automaton.getVariables(), "Assignment variable in edge from '" + sourceLoc + "' to '" + targetLoc + "' in automaton '" + name + "'");
                        // value
                        STORM_LOG_THROW(assignmentEntry.count("value") == 1, storm::exceptions::InvalidJaniException, "Assignment in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "'  must have one value field");
                        storm::expressions::Expression assignmentExpr = parseExpression(assignmentEntry.at("value"), "assignment in edge from '" + sourceLoc + "' to '" + targetLoc + "' in automaton '" + name + "'");
                        // TODO check types
                        assignments.emplace_back(lhs, assignmentExpr);
                    }
                    edgeDestinations.emplace_back(locIds.at(targetLoc), probExpr, assignments);
                }
                automaton.addEdge(storm::jani::Edge(locIds.at(sourceLoc), parentModel.getActionIndex(action), rateExpr.isInitialized() ? boost::optional<storm::expressions::Expression>(rateExpr) : boost::none, guardExpr, edgeDestinations));
            }



            return automaton;
        }

        std::shared_ptr<storm::jani::Composition> JaniParser::parseComposition(json const &compositionStructure) {

        }
    }
}