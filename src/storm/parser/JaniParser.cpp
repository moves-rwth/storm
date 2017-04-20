#include "JaniParser.h"

#include "storm/storage/jani/Edge.h"
#include "storm/storage/jani/TemplateEdge.h"
#include "storm/storage/jani/EdgeDestination.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Automaton.h"
#include "storm/storage/jani/Location.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/AutomatonComposition.h"
#include "storm/storage/jani/ParallelComposition.h"
#include "storm/storage/jani/CompositionInformationVisitor.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/InvalidJaniException.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/storage/jani/ModelType.h"

#include "storm/modelchecker/results/FilterType.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/lexical_cast.hpp>

#include "storm/utility/macros.h"
#include "storm/utility/file.h"

namespace storm {
    namespace parser {

        ////////////
        // Defaults
        ////////////
        const bool JaniParser::defaultVariableTransient = false;
        const bool JaniParser::defaultBooleanInitialValue = false;
        const double JaniParser::defaultRationalInitialValue = 0.0;
        const int64_t JaniParser::defaultIntegerInitialValue = 0;
        const std::string VARIABLE_AUTOMATON_DELIMITER = "_";
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


        std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> JaniParser::parse(std::string const& path) {
            JaniParser parser;
            parser.readFile(path);
            return parser.parseModel();
        }

        JaniParser::JaniParser(std::string const& jsonstring) {
            parsedStructure = json::parse(jsonstring);
        }

        void JaniParser::readFile(std::string const &path) {
            std::ifstream file;
            storm::utility::openFile(path, file);
            parsedStructure << file;
            storm::utility::closeFile(file);
        }

        std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> JaniParser::parseModel(bool parseProperties) {
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
            std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> constants;
            STORM_LOG_THROW(constantsCount < 2, storm::exceptions::InvalidJaniException, "Constant-declarations can be given at most once.");
            if (constantsCount == 1) {
                for (auto const &constStructure : parsedStructure.at("constants")) {
                    std::shared_ptr<storm::jani::Constant> constant = parseConstant(constStructure, constants, "global");
                    constants.emplace(constant->getName(), constant);
                    model.addConstant(*constant);
                }
            }
            size_t variablesCount = parsedStructure.count("variables");
            STORM_LOG_THROW(variablesCount < 2, storm::exceptions::InvalidJaniException, "Variable-declarations can be given at most once for global variables.");
            std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> globalVars;
            if (variablesCount == 1) {
                for (auto const& varStructure : parsedStructure.at("variables")) {
                    std::shared_ptr<storm::jani::Variable> variable = parseVariable(varStructure, "global", globalVars, constants);
                    globalVars.emplace(variable->getName(), variable);
                    model.addVariable(*variable);
                }
            }
            STORM_LOG_THROW(parsedStructure.count("automata") == 1, storm::exceptions::InvalidJaniException, "Exactly one list of automata must be given");
            STORM_LOG_THROW(parsedStructure.at("automata").is_array(), storm::exceptions::InvalidJaniException, "Automata must be an array");
            // Automatons can only be parsed after constants and variables.
            for (auto const& automataEntry : parsedStructure.at("automata")) {
                model.addAutomaton(parseAutomaton(automataEntry, model, globalVars, constants));
            }
            STORM_LOG_THROW(parsedStructure.count("restrict-initial") < 2, storm::exceptions::InvalidJaniException, "Model has multiple initial value restrictions");
            storm::expressions::Expression initialValueRestriction = expressionManager->boolean(true);
            if(parsedStructure.count("restrict-initial") > 0) {
                STORM_LOG_THROW(parsedStructure.at("restrict-initial").count("exp") == 1, storm::exceptions::InvalidJaniException, "Model needs an expression inside the initial restricion");
                initialValueRestriction  = parseExpression(parsedStructure.at("restrict-initial").at("exp"), "Initial value restriction for automaton " + name,  globalVars, constants);
            }
            model.setInitialStatesRestriction(initialValueRestriction);
            STORM_LOG_THROW(parsedStructure.count("system") == 1, storm::exceptions::InvalidJaniException, "Exactly one system description must be given");
            std::shared_ptr<storm::jani::Composition> composition = parseComposition(parsedStructure.at("system"));
            model.setSystemComposition(composition);
            STORM_LOG_THROW(parsedStructure.count("properties") <= 1, storm::exceptions::InvalidJaniException, "At most one list of properties can be given");
            std::map<std::string, storm::jani::Property> properties;
            if (parseProperties && parsedStructure.count("properties") == 1) {
                STORM_LOG_THROW(parsedStructure.at("properties").is_array(), storm::exceptions::InvalidJaniException, "Properties should be an array");
                for(auto const& propertyEntry : parsedStructure.at("properties")) {
                    try {
                        auto prop = this->parseProperty(propertyEntry, globalVars, constants);
                        properties.emplace(prop.getName(), prop);
                    } catch (storm::exceptions::NotSupportedException const& ex) {
                        STORM_LOG_WARN("Cannot handle property " << ex.what());
                    } catch (storm::exceptions::NotImplementedException const&  ex) {
                        STORM_LOG_WARN("Cannot handle property " << ex.what());
                    }
                }
            }
            model.finalize();
            return {model, properties};
        }

        
        std::vector<std::shared_ptr<storm::logic::Formula const>> JaniParser::parseUnaryFormulaArgument(json const& propertyStructure, storm::logic::FormulaContext formulaContext, std::string const& opstring, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants, std::string const& context) {
            STORM_LOG_THROW(propertyStructure.count("exp") == 1, storm::exceptions::InvalidJaniException, "Expecting operand for operator " << opstring << " in  " << context);
            return  { parseFormula(propertyStructure.at("exp"), formulaContext, globalVars, constants, "Operand of operator " + opstring) };
        }
        
        
        std::vector<std::shared_ptr<storm::logic::Formula const>> JaniParser::parseBinaryFormulaArguments(json const& propertyStructure, storm::logic::FormulaContext formulaContext, std::string const& opstring, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants, std::string const& context) {
            STORM_LOG_THROW(propertyStructure.count("left") == 1, storm::exceptions::InvalidJaniException, "Expecting left operand for operator " << opstring << " in  " << context);
            STORM_LOG_THROW(propertyStructure.count("right") == 1, storm::exceptions::InvalidJaniException, "Expecting right operand for operator " << opstring << " in  " << context);
            return { parseFormula(propertyStructure.at("left"), formulaContext, globalVars, constants, "Operand of operator " + opstring),  parseFormula(propertyStructure.at("right"), formulaContext, globalVars, constants, "Operand of operator " + opstring)  };
        }
        
        storm::jani::PropertyInterval JaniParser::parsePropertyInterval(json const& piStructure) {
            storm::jani::PropertyInterval pi;
            if (piStructure.count("lower") > 0) {
                pi.lowerBound = parseExpression(piStructure.at("lower"), "Lower bound for property interval", {}, {});
                // TODO substitute constants.
                STORM_LOG_THROW(!pi.lowerBound.containsVariables(), storm::exceptions::NotSupportedException, "Only constant expressions are supported as lower bounds");
               
                
            }
            if (piStructure.count("lower-exclusive") > 0) {
                STORM_LOG_THROW(pi.lowerBound.isInitialized(), storm::exceptions::InvalidJaniException, "Lower-exclusive can only be set if a lower bound is present");
                pi.lowerBoundStrict = piStructure.at("lower-exclusive");
                
            }
            if (piStructure.count("upper") > 0) {
                pi.upperBound = parseExpression(piStructure.at("upper"), "Upper bound for property interval", {}, {});
                // TODO substitute constants.
                STORM_LOG_THROW(!pi.upperBound.containsVariables(), storm::exceptions::NotSupportedException, "Only constant expressions are supported as upper bounds");
                
            }
            if (piStructure.count("upper-exclusive") > 0) {
                STORM_LOG_THROW(pi.lowerBound.isInitialized(), storm::exceptions::InvalidJaniException, "Lower-exclusive can only be set if a lower bound is present");
                pi.lowerBoundStrict = piStructure.at("upper-exclusive");
            }
            STORM_LOG_THROW(pi.lowerBound.isInitialized() || pi.upperBound.isInitialized(), storm::exceptions::InvalidJaniException, "Bounded operators must be bounded");
            return pi;
            
            
        }

        
        std::shared_ptr<storm::logic::Formula const> JaniParser::parseFormula(json const& propertyStructure, storm::logic::FormulaContext formulaContext,std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants, std::string const& context, boost::optional<storm::logic::Bound> bound) {
            if (propertyStructure.is_boolean()) {
                return std::make_shared<storm::logic::BooleanLiteralFormula>(propertyStructure.get<bool>());
            }
            if (propertyStructure.is_string()) {
                if (labels.count(propertyStructure.get<std::string>()) > 0) {
                    return std::make_shared<storm::logic::AtomicLabelFormula>(propertyStructure.get<std::string>());
                }
            }
            storm::expressions::Expression expr = parseExpression(propertyStructure, "expression in property", globalVars, constants, {}, true);
            if(expr.isInitialized()) {
                assert(bound == boost::none);
                return std::make_shared<storm::logic::AtomicExpressionFormula>(expr);
            } else if(propertyStructure.count("op") == 1) {
                std::string opString = getString(propertyStructure.at("op"), "Operation description");
                
                if(opString == "Pmin" || opString == "Pmax") {
                    std::vector<std::shared_ptr<storm::logic::Formula const>> args = parseUnaryFormulaArgument(propertyStructure, storm::logic::FormulaContext::Probability, opString, globalVars, constants, "");
                    assert(args.size() == 1);
                    storm::logic::OperatorInformation opInfo;
                    opInfo.optimalityType =  opString == "Pmin" ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize;
                    opInfo.bound = bound;
                    return std::make_shared<storm::logic::ProbabilityOperatorFormula>(args[0], opInfo);
                    
                } else if (opString == "∀" || opString == "∃") {
                    assert(bound == boost::none);
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Forall and Exists are currently not supported");
                } else if (opString == "Emin" || opString == "Emax") {
                    bool time=false;
                    STORM_LOG_THROW(propertyStructure.count("exp") == 1, storm::exceptions::InvalidJaniException, "Expecting reward-expression for operator " << opString << " in  " << context);
                    storm::expressions::Expression rewExpr = parseExpression(propertyStructure.at("exp"), "Reward expression in " + context, globalVars, constants);
                    if (rewExpr.isVariable()) {
                        time = false;
                    } else {
                        time = true;
                    }
                    std::shared_ptr<storm::logic::Formula const> reach;
                    if (propertyStructure.count("reach") > 0) {
                        reach = std::make_shared<storm::logic::EventuallyFormula>(parseFormula(propertyStructure.at("reach"), time ? storm::logic::FormulaContext::Time : storm::logic::FormulaContext::Reward, globalVars, constants, "Reach-expression of operator " + opString));
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Total reward is currently not supported");
                    }
                    storm::logic::OperatorInformation opInfo;
                    opInfo.optimalityType =  opString == "Emin" ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize;
                    opInfo.bound = bound;

                    bool accTime = false;
                    bool accSteps = false;
                    if (propertyStructure.count("accumulate") > 0) {
                        STORM_LOG_THROW(propertyStructure.at("accumulate").is_array(), storm::exceptions::InvalidJaniException, "Accumulate should be an array");
                        for(auto const& accEntry : propertyStructure.at("accumulate")) {
                            if (accEntry == "steps") {
                                accSteps = true;
                            } else if (accEntry == "time") {
                                accTime = true;
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "One may only accumulate either 'steps' or 'time', got " << accEntry.dump() << " in "  << context);
                            }
                        }
                    }
                    STORM_LOG_THROW(!(accTime && accSteps), storm::exceptions::NotSupportedException, "Storm does not allow to accumulate over both time and steps");
                    
                    
                    if (propertyStructure.count("step-instant") > 0) {
                        storm::expressions::Expression stepInstantExpr = parseExpression(propertyStructure.at("step-instant"), "Step instant in " + context, globalVars, constants);
                        STORM_LOG_THROW(!stepInstantExpr.containsVariables(), storm::exceptions::NotSupportedException, "Storm only allows constant step-instants");

                        int64_t stepInstant = stepInstantExpr.evaluateAsInt();
                        STORM_LOG_THROW(stepInstant >= 0, storm::exceptions::InvalidJaniException, "Only non-negative step-instants are allowed");
                        if(!accTime && !accSteps) {
                            if (rewExpr.isVariable()) {
                                std::string rewardName = rewExpr.getVariables().begin()->getName();
                                return std::make_shared<storm::logic::RewardOperatorFormula>(std::make_shared<storm::logic::InstantaneousRewardFormula>(stepInstantExpr, storm::logic::TimeBoundType::Steps), rewardName, opInfo);
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Only simple reward expressions are currently supported");
                            }
                        } else {
                            if (rewExpr.isVariable()) {
                                std::string rewardName = rewExpr.getVariables().begin()->getName();
                                return std::make_shared<storm::logic::RewardOperatorFormula>(std::make_shared<storm::logic::CumulativeRewardFormula>(storm::logic::TimeBound(false, stepInstantExpr), storm::logic::TimeBoundType::Steps), rewardName, opInfo);
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Only simple reward expressions are currently supported");
                            }
                        }
                    } else if (propertyStructure.count("time-instant") > 0) {
                        storm::expressions::Expression timeInstantExpr = parseExpression(propertyStructure.at("time-instant"), "time instant in " + context, globalVars, constants);
                        STORM_LOG_THROW(!timeInstantExpr.containsVariables(), storm::exceptions::NotSupportedException, "Storm only allows constant time-instants");

                        double timeInstant = timeInstantExpr.evaluateAsDouble();
                        STORM_LOG_THROW(timeInstant >= 0, storm::exceptions::InvalidJaniException, "Only non-negative time-instants are allowed");
                        if(!accTime && !accSteps) {
                            if (rewExpr.isVariable()) {
                                std::string rewardName = rewExpr.getVariables().begin()->getName();
                                return std::make_shared<storm::logic::RewardOperatorFormula>(std::make_shared<storm::logic::InstantaneousRewardFormula>(timeInstantExpr, storm::logic::TimeBoundType::Time), rewardName, opInfo);
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Only simple reward expressions are currently supported");
                            }
                        } else {
                            if (rewExpr.isVariable()) {
                                std::string rewardName = rewExpr.getVariables().begin()->getName();
                                return std::make_shared<storm::logic::RewardOperatorFormula>(std::make_shared<storm::logic::CumulativeRewardFormula>(storm::logic::TimeBound(false, timeInstantExpr), storm::logic::TimeBoundType::Time), rewardName, opInfo);
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Only simple reward expressions are currently supported");
                            }
                        }
                    } else if (propertyStructure.count("reward-instants") > 0) {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Instant/Cumul. Reward for reward constraints not supported currently.");
                    }
                    
                    //STORM_LOG_THROW(!accTime && !accSteps, storm::exceptions::NotSupportedException, "Storm only allows accumulation if a step- or time-bound is given.");
                    
                    if (rewExpr.isVariable()) {
                        std::string rewardName = rewExpr.getVariables().begin()->getName();
                        return std::make_shared<storm::logic::RewardOperatorFormula>(reach, rewardName, opInfo);
                    } else if (!rewExpr.containsVariables()) {
                        if(rewExpr.hasIntegerType()) {
                            if (rewExpr.evaluateAsInt() == 1) {
                                
                                return std::make_shared<storm::logic::TimeOperatorFormula>(reach, opInfo);
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Expected steps/time only works with constant one.");
                            }
                        } else if (rewExpr.hasRationalType()){
                            if (rewExpr.evaluateAsDouble() == 1.0) {
                                
                                return std::make_shared<storm::logic::TimeOperatorFormula>(reach, opInfo);
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Expected steps/time only works with constant one.");
                            }
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Only numerical reward expressions are allowed");
                        }
                        
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "No complex reward expressions are supported at the moment");
                    }
                    

                } else if (opString == "Smin" || opString == "Smax") {
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Smin and Smax are currently not supported");
                } else if (opString == "U" || opString == "F") {
                    assert(bound == boost::none);
                    std::vector<std::shared_ptr<storm::logic::Formula const>> args;
                    if (opString == "U") {
                        args = parseBinaryFormulaArguments(propertyStructure, formulaContext, opString, globalVars, constants, "");
                    } else {
                        assert(opString == "F");
                        args = parseUnaryFormulaArgument(propertyStructure, formulaContext, opString, globalVars, constants, "");
                        args.push_back(args[0]);
                        args[0] = storm::logic::BooleanLiteralFormula::getTrueFormula();
                    }
                    if (propertyStructure.count("step-bounds") > 0) {
                        storm::jani::PropertyInterval pi = parsePropertyInterval(propertyStructure.at("step-bounds"));
                        STORM_LOG_THROW(pi.hasUpperBound(), storm::exceptions::NotSupportedException, "Storm only supports step-bounded until with an upper bound");
                        if(pi.hasLowerBound()) {
                            STORM_LOG_THROW(pi.lowerBound.evaluateAsInt() == 0, storm::exceptions::NotSupportedException, "Storm only supports step-bounded until without a (non-trivial) lower-bound");
                        }
                        int64_t upperBound = pi.upperBound.evaluateAsInt();
                        if(pi.upperBoundStrict) {
                            upperBound--;
                        }
                        STORM_LOG_THROW(upperBound >= 0, storm::exceptions::InvalidJaniException, "Step-bounds cannot be negative");
                        return std::make_shared<storm::logic::BoundedUntilFormula const>(args[0], args[1], storm::logic::TimeBound(pi.lowerBoundStrict, pi.lowerBound), storm::logic::TimeBound(pi.upperBoundStrict, pi.upperBound), storm::logic::TimeBoundType::Steps);
                    } else if (propertyStructure.count("time-bounds") > 0) {
                        storm::jani::PropertyInterval pi = parsePropertyInterval(propertyStructure.at("time-bounds"));
                        STORM_LOG_THROW(pi.hasUpperBound(), storm::exceptions::NotSupportedException, "Storm only supports time-bounded until with an upper bound.");
                        double lowerBound = 0.0;
                        if(pi.hasLowerBound()) {
                            lowerBound = pi.lowerBound.evaluateAsDouble();
                        }
                        double upperBound = pi.upperBound.evaluateAsDouble();
                        STORM_LOG_THROW(lowerBound >= 0, storm::exceptions::InvalidJaniException, "(Lower) time-bounds cannot be negative");
                        STORM_LOG_THROW(upperBound >= 0, storm::exceptions::InvalidJaniException, "(Upper) time-bounds cannot be negative");
                        return std::make_shared<storm::logic::BoundedUntilFormula const>(args[0], args[1], storm::logic::TimeBound(pi.lowerBoundStrict, pi.lowerBound), storm::logic::TimeBound(pi.upperBoundStrict, pi.upperBound), storm::logic::TimeBoundType::Time);
                        
                    } else if (propertyStructure.count("reward-bounds") > 0 ) {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Reward bounded properties are not supported by storm");
                    }
                    if (args[0]->isTrueFormula()) {
                        return std::make_shared<storm::logic::EventuallyFormula const>(args[1], formulaContext);
                    } else {
                        return std::make_shared<storm::logic::UntilFormula const>(args[0], args[1]);
                    }
                } else if (opString == "G") {
                    assert(bound == boost::none);
                    std::vector<std::shared_ptr<storm::logic::Formula const>> args = parseUnaryFormulaArgument(propertyStructure, formulaContext, opString, globalVars, constants, "Subformula of globally operator " + context);
                    if (propertyStructure.count("step-bounds") > 0) {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Globally and step-bounds are not supported currently");
                    } else if (propertyStructure.count("time-bounds") > 0) {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Globally and time bounds are not supported by storm");
                    } else if (propertyStructure.count("reward-bounds") > 0 ) {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Reward bounded properties are not supported by storm");
                    }
                    return std::make_shared<storm::logic::GloballyFormula const>(args[1]);

                } else if (opString == "W") {
                    assert(bound == boost::none);
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Weak until is not supported");
                } else if (opString == "R") {
                    assert(bound == boost::none);
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Release is not supported");
                } else if (opString == "∧" || opString == "∨") {
                    assert(bound == boost::none);
                    std::vector<std::shared_ptr<storm::logic::Formula const>> args = parseBinaryFormulaArguments(propertyStructure, formulaContext, opString, globalVars, constants, "");
                    assert(args.size() == 2);
                    storm::logic::BinaryBooleanStateFormula::OperatorType oper = opString ==  "∧" ? storm::logic::BinaryBooleanStateFormula::OperatorType::And : storm::logic::BinaryBooleanStateFormula::OperatorType::Or;
                    return std::make_shared<storm::logic::BinaryBooleanStateFormula const>(oper, args[0], args[1]);
                } else if (opString == "¬") {
                    assert(bound == boost::none);
                    std::vector<std::shared_ptr<storm::logic::Formula const>> args = parseUnaryFormulaArgument(propertyStructure, formulaContext, opString, globalVars, constants, "");
                    assert(args.size() == 1);
                    return std::make_shared<storm::logic::UnaryBooleanStateFormula const>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, args[0]);
                    
                } else if (opString == "≥" || opString == "≤" || opString == "<" || opString == ">") {
                    assert(bound == boost::none);
                    storm::logic::ComparisonType ct;
                    if(opString == "≥") {
                        ct = storm::logic::ComparisonType::GreaterEqual;
                    } else if (opString == "≤") {
                        ct = storm::logic::ComparisonType::LessEqual;
                    } else if (opString == "<") {
                        ct = storm::logic::ComparisonType::Less;
                    } else if (opString == ">") {
                        ct = storm::logic::ComparisonType::Greater;
                    }
                    if (propertyStructure.at("left").count("op") > 0 && (propertyStructure.at("left").at("op") == "Pmin" || propertyStructure.at("left").at("op") == "Pmax" || propertyStructure.at("left").at("op") == "Emin" || propertyStructure.at("left").at("op") == "Emax" || propertyStructure.at("left").at("op") == "Smin" || propertyStructure.at("left").at("op") == "Smax")) {
                        auto expr = parseExpression(propertyStructure.at("right"), "Threshold for operator " + propertyStructure.at("left").at("op").get<std::string>(),{},{});
                        STORM_LOG_THROW(expr.getVariables().empty(), storm::exceptions::NotSupportedException, "Only constant thresholds supported");
                        return parseFormula(propertyStructure.at("left"), formulaContext, globalVars, constants, "", storm::logic::Bound(ct, expr));

                    } else if(propertyStructure.at("right").count("op") > 0 && (propertyStructure.at("right").at("op") == "Pmin" || propertyStructure.at("right").at("op") == "Pmax" || propertyStructure.at("right").at("op") == "Emin" || propertyStructure.at("right").at("op") == "Emax" || propertyStructure.at("right").at("op") == "Smin" || propertyStructure.at("right").at("op") == "Smax")) {
                        auto expr = parseExpression(propertyStructure.at("left"), "Threshold for operator " + propertyStructure.at("right").at("op").get<std::string>(),{},{});
                        STORM_LOG_THROW(expr.getVariables().empty(), storm::exceptions::NotSupportedException, "Only constant thresholds supported");
                        // TODO evaluate this expression directly as rational number
                        return parseFormula(propertyStructure.at("right"),formulaContext, globalVars, constants, "", storm::logic::Bound(ct, expr));
                    } else {
                         STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "No complex comparisons are allowed.");
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown operator " << opString);
                }
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Looking for operator for formula " << propertyStructure.dump() << ", but did not find one");
            }
        }
        
        storm::jani::Property JaniParser::parseProperty(json const& propertyStructure, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants) {
            STORM_LOG_THROW(propertyStructure.count("name") ==  1, storm::exceptions::InvalidJaniException, "Property must have a name");
            // TODO check unique name
            std::string name = getString(propertyStructure.at("name"), "property-name");
            std::string comment = "";
            if (propertyStructure.count("comment") > 0) {
                comment = getString(propertyStructure.at("comment"), "comment for property named '" + name + "'.");
            }
            STORM_LOG_THROW(propertyStructure.count("expression") == 1, storm::exceptions::InvalidJaniException, "Property must have an expression");
            // Parse filter expression.
            json const& expressionStructure = propertyStructure.at("expression");
            
            STORM_LOG_THROW(expressionStructure.count("op") == 1, storm::exceptions::InvalidJaniException, "Expression in property must have an operation description");
            STORM_LOG_THROW(expressionStructure.at("op") == "filter", storm::exceptions::InvalidJaniException, "Top level operation of a property must be a filter");
            STORM_LOG_THROW(expressionStructure.count("fun") == 1, storm::exceptions::InvalidJaniException, "Filter must have a function descritpion");
            std::string funDescr = getString(expressionStructure.at("fun"), "Filter function in property named " + name);
            storm::modelchecker::FilterType ft;
            if (funDescr == "min") {
                ft = storm::modelchecker::FilterType::MIN;
            } else if (funDescr == "max") {
                ft = storm::modelchecker::FilterType::MAX;
            } else if (funDescr == "sum") {
                ft = storm::modelchecker::FilterType::SUM;
            } else if (funDescr == "avg") {
                ft = storm::modelchecker::FilterType::AVG;
            } else if (funDescr == "count") {
                ft = storm::modelchecker::FilterType::COUNT;
            } else if (funDescr == "∀") {
                ft = storm::modelchecker::FilterType::FORALL;
            } else if (funDescr == "∃") {
                ft = storm::modelchecker::FilterType::EXISTS;
            } else if (funDescr == "argmin") {
                ft = storm::modelchecker::FilterType::ARGMIN;
            } else if (funDescr == "argmax") {
                ft = storm::modelchecker::FilterType::ARGMAX;
            } else if (funDescr == "values") {
                ft = storm::modelchecker::FilterType::VALUES;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown filter description " << funDescr << " in property named " << name);
            }
            
            STORM_LOG_THROW(expressionStructure.count("states") == 1, storm::exceptions::InvalidJaniException, "Filter must have a states description");
            STORM_LOG_THROW(expressionStructure.at("states").count("op") > 0, storm::exceptions::NotImplementedException, "We only support properties where the filter has initial states");
            std::string statesDescr = getString(expressionStructure.at("states").at("op"), "Filtered states in property named " + name);
            STORM_LOG_THROW(statesDescr == "initial", storm::exceptions::NotImplementedException, "Only initial states are allowed as set of states we are interested in.");
            STORM_LOG_THROW(expressionStructure.count("values") == 1, storm::exceptions::InvalidJaniException, "Values as input for a filter must be given");
            auto formula = parseFormula(expressionStructure.at("values"), storm::logic::FormulaContext::Undefined, globalVars, constants, "Values of property " + name);
            return storm::jani::Property(name, storm::jani::FilterExpression(formula, ft), comment);
        }

        std::shared_ptr<storm::jani::Constant> JaniParser::parseConstant(json const& constantStructure, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants, std::string const& scopeDescription) {
            STORM_LOG_THROW(constantStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Variable (scope: " + scopeDescription + ") must have a name");
            std::string name = getString(constantStructure.at("name"), "variable-name in " + scopeDescription + "-scope");
            // TODO check existance of name.
            // TODO store prefix in variable.
            std::string exprManagerName = name;
            STORM_LOG_THROW(constantStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "Constant '" + name + "' (scope: " + scopeDescription + ") must have a (single) type-declaration.");
            size_t valueCount = constantStructure.count("value");
            storm::expressions::Expression initExpr;
            STORM_LOG_THROW(valueCount < 2, storm::exceptions::InvalidJaniException, "Value for constant '" + name +  "'  (scope: " + scopeDescription + ") must be given at most once.");
            if (valueCount == 1) {
                // Read initial value before; that makes creation later on a bit easier, and has as an additional benefit that we do not need to check whether the variable occurs also on the assignment.
                initExpr = parseExpression(constantStructure.at("value"), "Value of constant " + name + " (scope: " + scopeDescription + ")", {}, constants);
                assert(initExpr.isInitialized());
            }

            if (constantStructure.at("type").is_object()) {
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
                    if(initExpr.isInitialized()) {
                        return std::make_shared<storm::jani::Constant>(name, expressionManager->declareRationalVariable(exprManagerName), initExpr);
                    } else {
                        return std::make_shared<storm::jani::Constant>(name, expressionManager->declareRationalVariable(exprManagerName));
                    }
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
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description " << constantStructure.at("type").dump()  << " for constant '" << name << "' (scope: " << scopeDescription << ")");
                }
            }

            STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description, " << constantStructure.at("type").dump()  << " for Variable '" << name << "' (scope: " << scopeDescription << ")");
        }

        std::shared_ptr<storm::jani::Variable> JaniParser::parseVariable(json const& variableStructure, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars, bool prefWithScope) {
            STORM_LOG_THROW(variableStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Variable (scope: " + scopeDescription + ") must have a name");
            std::string pref = prefWithScope  ? scopeDescription + VARIABLE_AUTOMATON_DELIMITER : "";
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
            if(variableStructure.at("type").is_string()) {
                if(variableStructure.at("type") == "real") {
                    if(initvalcount == 1) {
                        if(variableStructure.at("initial-value").is_null()) {
                            initVal = expressionManager->rational(defaultRationalInitialValue);
                        } else {
                            initVal = parseExpression(variableStructure.at("initial-value"), "Initial value for variable " + name + " (scope: " + scopeDescription + ") ", globalVars, constants, localVars);
                            STORM_LOG_THROW(initVal.get().hasRationalType(), storm::exceptions::InvalidJaniException, "Initial value for integer variable " + name + "(scope " + scopeDescription + ") should be a rational");
                        }
                        return std::make_shared<storm::jani::RealVariable>(name, expressionManager->declareRationalVariable(exprManagerName), initVal.get(), transientVar);
                        
                    }
                    assert(!transientVar);
                    return std::make_shared<storm::jani::RealVariable>(name, expressionManager->declareRationalVariable(exprManagerName));
                } else if(variableStructure.at("type") == "bool") {
                    if(initvalcount == 1) {
                        if(variableStructure.at("initial-value").is_null()) {
                            initVal = expressionManager->boolean(defaultBooleanInitialValue);
                        } else {
                            initVal = parseExpression(variableStructure.at("initial-value"), "Initial value for variable " + name + " (scope: " + scopeDescription + ") ", globalVars, constants, localVars);
                            STORM_LOG_THROW(initVal.get().hasBooleanType(), storm::exceptions::InvalidJaniException, "Initial value for integer variable " + name + "(scope " + scopeDescription + ") should be a Boolean");
                        }
                        if(transientVar) {
                            labels.insert(name);
                        }
                        return std::make_shared<storm::jani::BooleanVariable>(name, expressionManager->declareBooleanVariable(exprManagerName), initVal.get(), transientVar);
                    }
                    assert(!transientVar);
                    return std::make_shared<storm::jani::BooleanVariable>(name, expressionManager->declareBooleanVariable(exprManagerName));
                } else if(variableStructure.at("type") == "int") {
                    if(initvalcount == 1) {
                        if(variableStructure.at("initial-value").is_null()) {
                            initVal = expressionManager->integer(defaultIntegerInitialValue);
                        } else {
                            initVal = parseExpression(variableStructure.at("initial-value"), "Initial value for variable " + name + " (scope: " + scopeDescription + ") ", globalVars, constants, localVars);
                            STORM_LOG_THROW(initVal.get().hasIntegerType(), storm::exceptions::InvalidJaniException, "Initial value for integer variable " + name + "(scope " + scopeDescription + ") should be an integer");
                        }
                        return std::make_shared<storm::jani::UnboundedIntegerVariable>(name, expressionManager->declareIntegerVariable(exprManagerName), initVal.get(), transientVar);
                    }
                    assert(!transientVar); // Checked earlier.
                    return std::make_shared<storm::jani::UnboundedIntegerVariable>(name, expressionManager->declareIntegerVariable(exprManagerName));
                } else if(variableStructure.at("type") == "clock") {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported type 'clock' for variable '" << name << "' (scope: " << scopeDescription << ")");
                } else if(variableStructure.at("type") == "continuous") {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported type 'continuous' for variable ''" << name << "' (scope: " << scopeDescription << ")");
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description " << variableStructure.at("type").dump()  << " for variable '" << name << "' (scope: " << scopeDescription << ")");
                }
            } else if(variableStructure.at("type").is_object()) {
                STORM_LOG_THROW(variableStructure.at("type").count("kind") == 1, storm::exceptions::InvalidJaniException, "For complex type as in variable " << name << "(scope: " << scopeDescription << ")  kind must be given");
                std::string kind = getString(variableStructure.at("type").at("kind"), "kind for complex type as in variable " + name  + "(scope: " + scopeDescription + ") ");
                if(kind == "bounded") {
                    // First do the bounds, that makes the code a bit more streamlined
                    STORM_LOG_THROW(variableStructure.at("type").count("lower-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") lower-bound must be given");
                    storm::expressions::Expression lowerboundExpr = parseExpression(variableStructure.at("type").at("lower-bound"), "Lower bound for variable " + name + " (scope: " + scopeDescription + ")", globalVars, constants, localVars);
                    assert(lowerboundExpr.isInitialized());
                    STORM_LOG_THROW(variableStructure.at("type").count("upper-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") upper-bound must be given");
                    storm::expressions::Expression upperboundExpr = parseExpression(variableStructure.at("type").at("upper-bound"), "Upper bound for variable "+ name + " (scope: " + scopeDescription + ")", globalVars, constants, localVars);
                    assert(upperboundExpr.isInitialized());
                    STORM_LOG_THROW(variableStructure.at("type").count("base") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scopeDescription << ") base must be given");
                    if(initvalcount == 1) {
                        if(variableStructure.at("initial-value").is_null()) {
                            initVal = storm::expressions::ite(lowerboundExpr < 0 && upperboundExpr > 0, expressionManager->integer(0), lowerboundExpr);
                            // TODO as soon as we support half-open intervals, we have to change this.
                        } else {
                            initVal = parseExpression(variableStructure.at("initial-value"), "Initial value for variable " + name + " (scope: " + scopeDescription + ") ", globalVars, constants, localVars);
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

            STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description, " << variableStructure.at("type").dump()  << " for variable '" << name << "' (scope: " << scopeDescription << ")");
        }

        /**
         * Helper for parse expression.
         */
        void ensureNumberOfArguments(uint64_t expected, uint64_t actual, std::string const& opstring, std::string const& errorInfo) {
            STORM_LOG_THROW(expected == actual, storm::exceptions::InvalidJaniException, "Operator " << opstring  << " expects " << expected << " arguments, but got " << actual << " in " << errorInfo << ".");
        }

        std::vector<storm::expressions::Expression> JaniParser::parseUnaryExpressionArguments(json const& expressionDecl, std::string const& opstring, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars, bool returnNoneInitializedOnUnknownOperator) {
            storm::expressions::Expression left = parseExpression(expressionDecl.at("exp"), "Argument of operator " + opstring + " in " + scopeDescription, globalVars, constants, localVars,returnNoneInitializedOnUnknownOperator);
            return {left};
        }

        std::vector<storm::expressions::Expression> JaniParser::parseBinaryExpressionArguments(json const& expressionDecl, std::string const& opstring, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars, bool returnNoneInitializedOnUnknownOperator) {
            storm::expressions::Expression left = parseExpression(expressionDecl.at("left"), "Left argument of operator " + opstring + " in " + scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
            storm::expressions::Expression right = parseExpression(expressionDecl.at("right"), "Right argument of operator " + opstring + " in " + scopeDescription,  globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
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
                return localVars.getVariable(ident);
            } else if(globalVars.hasVariable(ident)) {
                return globalVars.getVariable(ident);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown identifier '" << ident << "' occurs in  " << scopeDescription);
            }
        }

        storm::expressions::Variable JaniParser::getVariableOrConstantExpression(std::string const& ident, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars) {
            if(localVars.count(ident) == 1) {
                return localVars.at(ident)->getExpressionVariable();
            } else if(globalVars.count(ident) == 1) {
               return globalVars.at(ident)->getExpressionVariable();
            } else if(constants.count(ident) == 1) {
                return constants.at(ident)->getExpressionVariable();
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown identifier '" << ident << "' occurs in " << scopeDescription);
            }
        }

        storm::expressions::Expression JaniParser::parseExpression(json const& expressionStructure, std::string const& scopeDescription, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants,  std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& localVars,  bool returnNoneInitializedOnUnknownOperator) {
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
                return storm::expressions::Expression(getVariableOrConstantExpression(ident, scopeDescription, globalVars, constants, localVars));
            } else if(expressionStructure.is_object()) {
                if(expressionStructure.count("distribution") == 1) {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Distributions are not supported by storm expressions, cannot import " << expressionStructure.dump() << " in  " << scopeDescription << ".");
                }
                if(expressionStructure.count("op") == 1) {
                    std::string opstring = getString(expressionStructure.at("op"), scopeDescription);
                    std::vector<storm::expressions::Expression> arguments = {};
                    if(opstring == "ite") {
                        STORM_LOG_THROW(expressionStructure.count("if") == 1, storm::exceptions::InvalidJaniException, "If operator required");
                        STORM_LOG_THROW(expressionStructure.count("else") == 1, storm::exceptions::InvalidJaniException, "Else operator required");
                        STORM_LOG_THROW(expressionStructure.count("then") == 1, storm::exceptions::InvalidJaniException, "If operator required");
                        arguments.push_back(parseExpression(expressionStructure.at("if"), "if-formula in " + scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator));
                        arguments.push_back(parseExpression(expressionStructure.at("then"), "then-formula in " + scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator));
                        arguments.push_back(parseExpression(expressionStructure.at("else"), "else-formula in " + scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator));
                        ensureNumberOfArguments(3, arguments.size(), opstring, scopeDescription);
                        assert(arguments.size() == 3);
                                            ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::ite(arguments[0], arguments[1], arguments[2]);
                    } else if (opstring == "∨") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] || arguments[1];
                    } else if (opstring == "∧") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] && arguments[1];
                    } else if (opstring == "⇒") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                        return (!arguments[0]) || arguments[1];
                    } else if (opstring == "¬") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 1);
                        if(!arguments[0].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureBooleanType(arguments[0], opstring, 0, scopeDescription);
                        return !arguments[0];
                    } else if (opstring == "=") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        if(arguments[0].hasBooleanType()) {
                            ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                            return storm::expressions::iff(arguments[0], arguments[1]);
                        } else {
                            ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                            return arguments[0] == arguments[1];
                        }
                    } else if (opstring == "≠") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        if(arguments[0].hasBooleanType()) {
                            ensureBooleanType(arguments[1], opstring, 1, scopeDescription);
                            return storm::expressions::xclusiveor(arguments[0], arguments[1]);
                        } else {
                            ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                            return arguments[0] != arguments[1];
                        }
                    } else if (opstring == "<") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] < arguments[1];
                    } else if (opstring == "≤") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] <= arguments[1];
                    } else if (opstring == ">") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] > arguments[1];
                    } else if (opstring == "≥") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] >= arguments[1];
                    } else if (opstring == "+") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] + arguments[1];
                    } else if (opstring == "-") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] - arguments[1];
                    } else if (opstring == "-") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription,globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return -arguments[0];
                    } else if (opstring == "*") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] * arguments[1];
                    } else if (opstring == "/") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return arguments[0] / arguments[1];
                    } else if (opstring == "%") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "modulo operation is not yet implemented");
                    } else if (opstring == "max") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return storm::expressions::maximum(arguments[0],arguments[1]);
                    } else if (opstring == "min") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        return storm::expressions::minimum(arguments[0],arguments[1]);
                    } else if (opstring == "floor") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::floor(arguments[0]);
                    } else if (opstring == "ceil") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::ceil(arguments[0]);
                    } else if (opstring == "abs") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::abs(arguments[0]);
                    } else if (opstring == "sgn") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::sign(arguments[0]);
                    } else if (opstring == "trc") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        return storm::expressions::abs(arguments[0]);
                    } else if (opstring == "pow") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "pow operation is not yet implemented");
                    } else if (opstring == "exp") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "exp operation is not yet implemented");
                    } else if (opstring == "log") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scopeDescription, globalVars, constants, localVars, returnNoneInitializedOnUnknownOperator);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scopeDescription);
                        ensureNumericalType(arguments[1], opstring, 1, scopeDescription);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "log operation is not yet implemented");
                    }  else if (unsupportedOpstrings.count(opstring) > 0){
                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Opstring " + opstring + " is not supported by storm");

                    } else {
                        if(returnNoneInitializedOnUnknownOperator) {
                            return storm::expressions::Expression();
                        }
                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown operator " << opstring << " in  " << scopeDescription << ".");
                    }
                }
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "No supported operator declaration found for complex expressions as " << expressionStructure.dump() << " in  " << scopeDescription << ".");
            }
            assert(false);
            // Silly warning suppression.
            return storm::expressions::Expression();

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

        storm::jani::Automaton JaniParser::parseAutomaton(json const &automatonStructure, storm::jani::Model const& parentModel, std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> const& globalVars, std::unordered_map<std::string, std::shared_ptr<storm::jani::Constant>> const& constants ) {
            STORM_LOG_THROW(automatonStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Each automaton must have a name");
            std::string name = getString(automatonStructure.at("name"), " the name field for automaton");
            storm::jani::Automaton automaton(name, expressionManager->declareIntegerVariable("_loc_" + name));

            uint64_t varDeclCount = automatonStructure.count("variables");
            STORM_LOG_THROW(varDeclCount < 2, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' has more than one list of variables");
            std::unordered_map<std::string, std::shared_ptr<storm::jani::Variable>> localVars;
            if(varDeclCount > 0) {
                for(auto const& varStructure : automatonStructure.at("variables")) {
                    std::shared_ptr<storm::jani::Variable> var = parseVariable(varStructure, name, globalVars, constants, localVars, true);
                    assert(localVars.count(var->getName()) == 0);
                    automaton.addVariable(*var);
                    localVars.emplace(var->getName(), var);
                }
            }


            STORM_LOG_THROW(automatonStructure.count("locations") > 0, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' does not have locations.");
            std::unordered_map<std::string, uint64_t> locIds;
            for(auto const& locEntry : automatonStructure.at("locations")) {
                STORM_LOG_THROW(locEntry.count("name") == 1, storm::exceptions::InvalidJaniException, "Locations for automaton '" << name << "' must have exactly one name");
                std::string locName = getString(locEntry.at("name"), "location of automaton " + name);
                STORM_LOG_THROW(locIds.count(locName) == 0, storm::exceptions::InvalidJaniException, "Location with name '" + locName + "' already exists in automaton '" + name + "'");
                STORM_LOG_THROW(locEntry.count("invariant") == 0, storm::exceptions::InvalidJaniException, "Invariants in locations as in '" + locName + "' in automaton '" + name + "' are not supported");
                //STORM_LOG_THROW(locEntry.count("invariant") > 0 && !supportsInvariants(parentModel.getModelType()), storm::exceptions::InvalidJaniException, "Invariants are not supported in the model type " + to_string(parentModel.getModelType()));
                std::vector<storm::jani::Assignment> transientAssignments;
                if(locEntry.count("transient-values") > 0) {
                    for(auto const& transientValueEntry : locEntry.at("transient-values")) {
                        STORM_LOG_THROW(transientValueEntry.count("ref") == 1, storm::exceptions::InvalidJaniException, "Transient values in location " << locName << " need exactly one ref that is assigned to");
                        STORM_LOG_THROW(transientValueEntry.count("value") == 1, storm::exceptions::InvalidJaniException, "Transient values in location " << locName << " need exactly one assigned value");
                        storm::jani::Variable const& lhs = getLValue(transientValueEntry.at("ref"), parentModel.getGlobalVariables(), automaton.getVariables(), "LHS of assignment in location " + locName + " (automaton '" + name + "')");
                        STORM_LOG_THROW(lhs.isTransient(), storm::exceptions::InvalidJaniException, "Assigned non-transient variable " + lhs.getName() + " in location " + locName + " (automaton: '" + name + "')");
                        storm::expressions::Expression rhs = parseExpression(transientValueEntry.at("value"), "Assignment of variable " + lhs.getName() + " in location " + locName + " (automaton: '" + name + "')", globalVars, constants, localVars);
                        transientAssignments.emplace_back(lhs, rhs);
                    }
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
                STORM_LOG_THROW(automatonStructure.at("restrict-initial").count("exp") == 1, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' needs an expression inside the initial restricion");
                initialValueRestriction  = parseExpression(automatonStructure.at("restrict-initial").at("exp"), "Initial value restriction for automaton " + name, globalVars, constants, localVars);
            }
            automaton.setInitialStatesRestriction(initialValueRestriction);



            STORM_LOG_THROW(automatonStructure.count("edges") > 0, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' must have a list of edges");
            for(auto const& edgeEntry : automatonStructure.at("edges")) {
                // source location
                STORM_LOG_THROW(edgeEntry.count("location") == 1, storm::exceptions::InvalidJaniException, "Each edge in automaton '" << name << "' must have a source");
                std::string sourceLoc = getString(edgeEntry.at("location"), "source location for edge in automaton '" + name + "'");
                STORM_LOG_THROW(locIds.count(sourceLoc) == 1, storm::exceptions::InvalidJaniException, "Source of edge has unknown location '" << sourceLoc << "' in automaton '" << name << "'.");
                // action
                STORM_LOG_THROW(edgeEntry.count("action") < 2, storm::exceptions::InvalidJaniException, "Edge from " << sourceLoc << " in automaton " << name << " has multiple actions");
                std::string action = storm::jani::Model::SILENT_ACTION_NAME; // def is tau
                if(edgeEntry.count("action") > 0) {
                    action = getString(edgeEntry.at("action"), "action name in edge from '" + sourceLoc + "' in automaton '" + name + "'");
                    // TODO check if action is known
                    assert(action != "");
                }
                // rate
                STORM_LOG_THROW(edgeEntry.count("rate") < 2, storm::exceptions::InvalidJaniException, "Edge from '" << sourceLoc << "' in automaton '" << name << "' has multiple rates");
                storm::expressions::Expression rateExpr;
                if(edgeEntry.count("rate") > 0) {
                    STORM_LOG_THROW(edgeEntry.at("rate").count("exp") == 1, storm::exceptions::InvalidJaniException, "Rate in edge from '" << sourceLoc << "' in automaton '" << name << "' must have a defing expression.");
                    rateExpr = parseExpression(edgeEntry.at("rate").at("exp"), "rate expression in edge from '" + sourceLoc + "' in automaton '" + name + "'", globalVars, constants, localVars);
                    STORM_LOG_THROW(rateExpr.hasNumericalType(), storm::exceptions::InvalidJaniException, "Rate '" << rateExpr << "' has not a numerical type");
                }
                // guard
                STORM_LOG_THROW(edgeEntry.count("guard") <= 1, storm::exceptions::InvalidJaniException, "Guard can be given at most once in edge from '" << sourceLoc << "' in automaton '" << name << "'");
                storm::expressions::Expression guardExpr = expressionManager->boolean(true);
                if(edgeEntry.count("guard") == 1) {
                    STORM_LOG_THROW(edgeEntry.at("guard").count("exp") == 1, storm::exceptions::InvalidJaniException, "Guard  in edge from '" + sourceLoc + "' in automaton '" + name + "' must have one expression");
                    guardExpr = parseExpression(edgeEntry.at("guard").at("exp"), "guard expression in edge from '" + sourceLoc + "' in automaton '" + name + "'",  globalVars, constants, localVars);
                    STORM_LOG_THROW(guardExpr.hasBooleanType(), storm::exceptions::InvalidJaniException, "Guard " << guardExpr << " does not have Boolean type.");
                }
                assert(guardExpr.isInitialized());
                
                std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(guardExpr);
                
                STORM_LOG_THROW(edgeEntry.count("destinations") == 1, storm::exceptions::InvalidJaniException, "A single list of destinations must be given in edge from '" << sourceLoc << "' in automaton '" << name << "'");
                std::vector<std::pair<uint64_t, storm::expressions::Expression>> destinationLocationsAndProbabilities;
                for(auto const& destEntry : edgeEntry.at("destinations")) {
                    // target location
                    STORM_LOG_THROW(edgeEntry.count("location") == 1, storm::exceptions::InvalidJaniException, "Each destination in edge from '" << sourceLoc << "' in automaton '" << name << "' must have a target location");
                    std::string targetLoc = getString(destEntry.at("location"), "target location for edge from '" + sourceLoc + "' in automaton '" + name + "'");
                    STORM_LOG_THROW(locIds.count(targetLoc) == 1, storm::exceptions::InvalidJaniException, "Target of edge has unknown location '" << targetLoc << "' in automaton '" << name << "'.");
                    // probability
                    storm::expressions::Expression probExpr;
                    unsigned probDeclCount = destEntry.count("probability");
                    STORM_LOG_THROW(probDeclCount < 2, storm::exceptions::InvalidJaniException, "Destination in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "' has multiple probabilites");
                    if(probDeclCount == 0) {
                        probExpr = expressionManager->rational(1.0);
                    } else {
                        STORM_LOG_THROW(destEntry.at("probability").count("exp") == 1, storm::exceptions::InvalidJaniException, "Destination in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "' must have a probability expression.");
                        probExpr = parseExpression(destEntry.at("probability").at("exp"), "probability expression in edge from '" + sourceLoc + "' to  '"  + targetLoc + "' in automaton '" + name + "'",  globalVars, constants, localVars);
                    }
                    assert(probExpr.isInitialized());
                    STORM_LOG_THROW(probExpr.hasNumericalType(), storm::exceptions::InvalidJaniException, "Probability expression " << probExpr << " does not have a numerical type." );
                    // assignments
                    std::vector<storm::jani::Assignment> assignments;
                    unsigned assignmentDeclCount = destEntry.count("assignments");
                    STORM_LOG_THROW(assignmentDeclCount < 2, storm::exceptions::InvalidJaniException, "Destination in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "' has multiple assignment lists");
                    if (assignmentDeclCount > 0) {
                        for (auto const& assignmentEntry : destEntry.at("assignments")) {
                            // ref
                            STORM_LOG_THROW(assignmentEntry.count("ref") == 1, storm::exceptions::InvalidJaniException, "Assignment in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "'  must have one ref field");
                            std::string refstring = getString(assignmentEntry.at("ref"), "assignment in edge from '" + sourceLoc + "' to '" + targetLoc + "' in automaton '" + name + "'");
                            storm::jani::Variable const& lhs = getLValue(refstring, parentModel.getGlobalVariables(), automaton.getVariables(), "Assignment variable in edge from '" + sourceLoc + "' to '" + targetLoc + "' in automaton '" + name + "'");
                            // value
                            STORM_LOG_THROW(assignmentEntry.count("value") == 1, storm::exceptions::InvalidJaniException, "Assignment in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "'  must have one value field");
                            storm::expressions::Expression assignmentExpr = parseExpression(assignmentEntry.at("value"), "assignment in edge from '" + sourceLoc + "' to '" + targetLoc + "' in automaton '" + name + "'",  globalVars, constants, localVars);
                            // TODO check types
                            // index
                            uint64_t assignmentIndex = 0; // default.
                            if(assignmentEntry.count("index") > 0) {
                                assignmentIndex = getUnsignedInt(assignmentEntry.at("index"), "assignment index in edge from '" + sourceLoc + "' to '" + targetLoc + "' in automaton '" + name + "'");
                            }
                            assignments.emplace_back(lhs, assignmentExpr, assignmentIndex);
                        }
                    }
                    destinationLocationsAndProbabilities.emplace_back(locIds.at(targetLoc), probExpr);
                    templateEdge->addDestination(storm::jani::TemplateEdgeDestination(assignments));
                }
                automaton.addEdge(storm::jani::Edge(locIds.at(sourceLoc), parentModel.getActionIndex(action), rateExpr.isInitialized() ? boost::optional<storm::expressions::Expression>(rateExpr) : boost::none, templateEdge, destinationLocationsAndProbabilities));
            }

            return automaton;
        }
        
        std::vector<storm::jani::SynchronizationVector> parseSyncVectors(json const& syncVectorStructure) {
            std::vector<storm::jani::SynchronizationVector> syncVectors;
            // TODO add error checks
            for (auto const& syncEntry : syncVectorStructure) {
                std::vector<std::string> inputs;
                for (auto const& syncInput : syncEntry.at("synchronise")) {
                    if(syncInput.is_null()) {
                        inputs.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
                    } else {
                        inputs.push_back(syncInput);
                    }
                }
                std::string syncResult = syncEntry.at("result");
                syncVectors.emplace_back(inputs, syncResult);
            }
            return syncVectors;
        }

        std::shared_ptr<storm::jani::Composition> JaniParser::parseComposition(json const &compositionStructure) {
            if(compositionStructure.count("automaton")) {
                return std::shared_ptr<storm::jani::AutomatonComposition>(new storm::jani::AutomatonComposition(compositionStructure.at("automaton").get<std::string>()));
            }
            
            STORM_LOG_THROW(compositionStructure.count("elements") == 1, storm::exceptions::InvalidJaniException, "Elements of a composition must be given, got " << compositionStructure.dump());
            
            if (compositionStructure.at("elements").size() == 1 && compositionStructure.count("syncs") == 0) {
                // We might have an automaton.
                STORM_LOG_THROW(compositionStructure.at("elements").back().count("automaton") == 1, storm::exceptions::InvalidJaniException, "Automaton must be given in composition");
                if (compositionStructure.at("elements").back().at("automaton").is_string()) {
                    std::string name = compositionStructure.at("elements").back().at("automaton");
                    // TODO check whether name exist?
                    return std::shared_ptr<storm::jani::AutomatonComposition>(new storm::jani::AutomatonComposition(name));
                }
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Trivial nesting parallel composition is not yet supported");
                
            }
            
            std::vector<std::shared_ptr<storm::jani::Composition>>  compositions;
            for (auto const& elemDecl : compositionStructure.at("elements")) {
                if(!allowRecursion) {
                    STORM_LOG_THROW(elemDecl.count("automaton") == 1, storm::exceptions::InvalidJaniException, "Automaton must be given in the element");
                }
                compositions.push_back(parseComposition(elemDecl));
            }
            
            STORM_LOG_THROW(compositionStructure.count("syncs") < 2, storm::exceptions::InvalidJaniException, "Sync vectors can be given at most once");
            std::vector<storm::jani::SynchronizationVector> syncVectors;
            if (compositionStructure.count("syncs") > 0) {
                syncVectors = parseSyncVectors(compositionStructure.at("syncs"));
            }
            
            return std::shared_ptr<storm::jani::Composition>(new storm::jani::ParallelComposition(compositions, syncVectors));
            
        }
    }
}
