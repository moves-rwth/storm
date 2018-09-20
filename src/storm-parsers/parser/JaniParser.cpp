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
#include "storm/storage/jani/ModelType.h"
#include "storm/storage/jani/CompositionInformationVisitor.h"
#include "storm/storage/jani/expressions/JaniExpressions.h"
#include "storm/logic/RewardAccumulationEliminationVisitor.h"

#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/InvalidJaniException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/NotImplementedException.h"

#include "storm/modelchecker/results/FilterType.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include "storm/storage/jani/ArrayVariable.h"

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

        int64_t getSignedInt(json const& structure, std::string const& errorInfo) {
            STORM_LOG_THROW(structure.is_number(), storm::exceptions::InvalidJaniException, "Expected a number in " << errorInfo << ", got '" << structure.dump() << "'");
            int num = structure.front();
            return static_cast<int64_t>(num);
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
            size_t featuresCount = parsedStructure.count("features");
            STORM_LOG_THROW(featuresCount < 2, storm::exceptions::InvalidJaniException, "features-declarations can be given at most once.");
            if (featuresCount == 1) {
                auto allKnownModelFeatures = storm::jani::getAllKnownModelFeatures();
                for (auto const& feature : parsedStructure.at("features")) {
					std::string featureStr = getString(feature, "Model feature");
                    bool found = false;
                    for (auto const& knownFeature : allKnownModelFeatures.asSet()) {
                        if (featureStr == storm::jani::toString(knownFeature)) {
                            model.getModelFeatures().add(knownFeature);
                            found = true;
                            break;
                        }
                    }
                    STORM_LOG_THROW(found, storm::exceptions::NotSupportedException, "Storm does not support the model feature " << featureStr);
                }
            }
            size_t actionCount = parsedStructure.count("actions");
            STORM_LOG_THROW(actionCount < 2, storm::exceptions::InvalidJaniException, "Action-declarations can be given at most once.");
            if (actionCount > 0) {
                parseActions(parsedStructure.at("actions"), model);
            }
            
            Scope scope(name);
            
            // Parse constants
            size_t constantsCount = parsedStructure.count("constants");
            ConstantsMap constants;
            scope.constants = &constants;
            STORM_LOG_THROW(constantsCount < 2, storm::exceptions::InvalidJaniException, "Constant-declarations can be given at most once.");
            if (constantsCount == 1) {
                for (auto const &constStructure : parsedStructure.at("constants")) {
                    std::shared_ptr<storm::jani::Constant> constant = parseConstant(constStructure, scope.refine("constants[" + std::to_string(constants.size()) + "]"));
                    constants.emplace(constant->getName(), &model.addConstant(*constant));
                }
            }
            
            // Parse variables
            size_t variablesCount = parsedStructure.count("variables");
            STORM_LOG_THROW(variablesCount < 2, storm::exceptions::InvalidJaniException, "Variable-declarations can be given at most once for global variables.");
            VariablesMap globalVars;
            scope.globalVars = &globalVars;
            if (variablesCount == 1) {
                bool requireInitialValues = parsedStructure.count("restrict-initial") == 0;
                for (auto const& varStructure : parsedStructure.at("variables")) {
                    std::shared_ptr<storm::jani::Variable> variable = parseVariable(varStructure, requireInitialValues, scope.refine("variables[" + std::to_string(globalVars.size())));
                    globalVars.emplace(variable->getName(), &model.addVariable(*variable));
                }
            }
            
            uint64_t funDeclCount = parsedStructure.count("functions");
            STORM_LOG_THROW(funDeclCount < 2, storm::exceptions::InvalidJaniException, "Model '" << name << "' has more than one list of functions");
            FunctionsMap globalFuns;
            scope.globalFunctions = &globalFuns;
            if (funDeclCount > 0) {
                for (auto const& funStructure : parsedStructure.at("functions")) {
                    storm::jani::FunctionDefinition funDef = parseFunctionDefinition(funStructure, scope.refine("functions[" + std::to_string(globalFuns.size()) + "] of model " + name));
                    assert(globalFuns.count(funDef.getName()) == 0);
                    globalFuns.emplace(funDef.getName(), &model.addFunctionDefinition(funDef));
                }
            }
            
            // Parse Automata
            STORM_LOG_THROW(parsedStructure.count("automata") == 1, storm::exceptions::InvalidJaniException, "Exactly one list of automata must be given");
            STORM_LOG_THROW(parsedStructure.at("automata").is_array(), storm::exceptions::InvalidJaniException, "Automata must be an array");
            // Automatons can only be parsed after constants and variables.
            for (auto const& automataEntry : parsedStructure.at("automata")) {
                model.addAutomaton(parseAutomaton(automataEntry, model, scope.refine("automata[" + std::to_string(model.getNumberOfAutomata()) + "]")));
            }
            STORM_LOG_THROW(parsedStructure.count("restrict-initial") < 2, storm::exceptions::InvalidJaniException, "Model has multiple initial value restrictions");
            storm::expressions::Expression initialValueRestriction = expressionManager->boolean(true);
            if (parsedStructure.count("restrict-initial") > 0) {
                STORM_LOG_THROW(parsedStructure.at("restrict-initial").count("exp") == 1, storm::exceptions::InvalidJaniException, "Model needs an expression inside the initial restricion");
                initialValueRestriction  = parseExpression(parsedStructure.at("restrict-initial").at("exp"), scope.refine("Initial value restriction"));
            }
            model.setInitialStatesRestriction(initialValueRestriction);
            STORM_LOG_THROW(parsedStructure.count("system") == 1, storm::exceptions::InvalidJaniException, "Exactly one system description must be given");
            std::shared_ptr<storm::jani::Composition> composition = parseComposition(parsedStructure.at("system"));
            model.setSystemComposition(composition);
            model.finalize();
            
            // Parse properties
            storm::logic::RewardAccumulationEliminationVisitor rewAccEliminator(model);
            STORM_LOG_THROW(parsedStructure.count("properties") <= 1, storm::exceptions::InvalidJaniException, "At most one list of properties can be given");
            std::map<std::string, storm::jani::Property> properties;
            if (parseProperties && parsedStructure.count("properties") == 1) {
                STORM_LOG_THROW(parsedStructure.at("properties").is_array(), storm::exceptions::InvalidJaniException, "Properties should be an array");
                for(auto const& propertyEntry : parsedStructure.at("properties")) {
                    try {
                        auto prop = this->parseProperty(propertyEntry, scope.refine("property[" + std::to_string(properties.size()) + "]"));
                        // Eliminate reward accumulations as much as possible
                        rewAccEliminator.eliminateRewardAccumulations(prop);
                        properties.emplace(prop.getName(), prop);
                    } catch (storm::exceptions::NotSupportedException const& ex) {
                        STORM_LOG_WARN("Cannot handle property: " << ex.what());
                    } catch (storm::exceptions::NotImplementedException const&  ex) {
                        STORM_LOG_WARN("Cannot handle property: " << ex.what());
                    }
                }
            }
            return {model, properties};
        }

        
        std::vector<std::shared_ptr<storm::logic::Formula const>> JaniParser::parseUnaryFormulaArgument(json const& propertyStructure, storm::logic::FormulaContext formulaContext, std::string const& opstring, Scope const& scope) {
            STORM_LOG_THROW(propertyStructure.count("exp") == 1, storm::exceptions::InvalidJaniException, "Expecting operand for operator " << opstring << " in  " << scope.description);
            return  { parseFormula(propertyStructure.at("exp"), formulaContext, scope.refine("Operand of operator " + opstring)) };
        }
        
        
        std::vector<std::shared_ptr<storm::logic::Formula const>> JaniParser::parseBinaryFormulaArguments(json const& propertyStructure, storm::logic::FormulaContext formulaContext, std::string const& opstring, Scope const& scope) {
            STORM_LOG_THROW(propertyStructure.count("left") == 1, storm::exceptions::InvalidJaniException, "Expecting left operand for operator " << opstring << " in  " << scope.description);
            STORM_LOG_THROW(propertyStructure.count("right") == 1, storm::exceptions::InvalidJaniException, "Expecting right operand for operator " << opstring << " in  " << scope.description);
            return { parseFormula(propertyStructure.at("left"), formulaContext, scope.refine("Operand of operator " + opstring)),  parseFormula(propertyStructure.at("right"), formulaContext, scope.refine("Operand of operator " + opstring))  };
        }
        
        storm::jani::PropertyInterval JaniParser::parsePropertyInterval(json const& piStructure, Scope const& scope) {
            storm::jani::PropertyInterval pi;
            if (piStructure.count("lower") > 0) {
                pi.lowerBound = parseExpression(piStructure.at("lower"), scope.refine("Lower bound for property interval"));
            }
            if (piStructure.count("lower-exclusive") > 0) {
                STORM_LOG_THROW(pi.lowerBound.isInitialized(), storm::exceptions::InvalidJaniException, "Lower-exclusive can only be set if a lower bound is present");
                pi.lowerBoundStrict = piStructure.at("lower-exclusive");
                
            }
            if (piStructure.count("upper") > 0) {
                pi.upperBound = parseExpression(piStructure.at("upper"), scope.refine("Upper bound for property interval"));
                
            }
            if (piStructure.count("upper-exclusive") > 0) {
                STORM_LOG_THROW(pi.upperBound.isInitialized(), storm::exceptions::InvalidJaniException, "Lower-exclusive can only be set if a lower bound is present");
                pi.upperBoundStrict = piStructure.at("upper-exclusive");
            }
            STORM_LOG_THROW(pi.lowerBound.isInitialized() || pi.upperBound.isInitialized(), storm::exceptions::InvalidJaniException, "Bounded operator must have a bounded interval, but no bounds found in '" << piStructure << "'");
            return pi;
        }

        storm::logic::RewardAccumulation JaniParser::parseRewardAccumulation(json const& accStructure, std::string const& context) {
            bool accTime = false;
            bool accSteps = false;
            bool accExit = false;
            STORM_LOG_THROW(accStructure.is_array(), storm::exceptions::InvalidJaniException, "Accumulate should be an array");
            for (auto const& accEntry : accStructure) {
                if (accEntry == "steps") {
                    accSteps = true;
                } else if (accEntry == "time") {
                    accTime = true;
                } else if (accEntry == "exit") {
                    accExit = true;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "One may only accumulate either 'steps' or 'time' or 'exit', got " << accEntry.dump() << " in "  << context);
                }
            }
            return storm::logic::RewardAccumulation(accSteps, accTime, accExit);
        }
        
        std::shared_ptr<storm::logic::Formula const> JaniParser::parseFormula(json const& propertyStructure, storm::logic::FormulaContext formulaContext, Scope const& scope, boost::optional<storm::logic::Bound> bound) {
            if (propertyStructure.is_boolean()) {
                return std::make_shared<storm::logic::BooleanLiteralFormula>(propertyStructure.get<bool>());
            }
            if (propertyStructure.is_string()) {
                if (labels.count(propertyStructure.get<std::string>()) > 0) {
                    return std::make_shared<storm::logic::AtomicLabelFormula>(propertyStructure.get<std::string>());
                }
            }
            storm::expressions::Expression expr = parseExpression(propertyStructure, scope.refine("expression in property"), true);
            if(expr.isInitialized()) {
                assert(bound == boost::none);
                return std::make_shared<storm::logic::AtomicExpressionFormula>(expr);
            } else if(propertyStructure.count("op") == 1) {
                std::string opString = getString(propertyStructure.at("op"), "Operation description");
                
                if(opString == "Pmin" || opString == "Pmax") {
                    std::vector<std::shared_ptr<storm::logic::Formula const>> args = parseUnaryFormulaArgument(propertyStructure, storm::logic::FormulaContext::Probability, opString, scope);
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
                    STORM_LOG_THROW(propertyStructure.count("exp") == 1, storm::exceptions::InvalidJaniException, "Expecting reward-expression for operator " << opString << " in  " << scope.description);
                    storm::expressions::Expression rewExpr = parseExpression(propertyStructure.at("exp"), scope.refine("Reward expression"));
                    if (rewExpr.isVariable()) {
                        time = false;
                    } else {
                        time = true;
                    }

                    storm::logic::OperatorInformation opInfo;
                    opInfo.optimalityType =  opString == "Emin" ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize;
                    opInfo.bound = bound;

                    storm::logic::RewardAccumulation rewardAccumulation(false, false, false);
                    if (propertyStructure.count("accumulate") > 0) {
                        rewardAccumulation = parseRewardAccumulation(propertyStructure.at("accumulate"), scope.description);
                    }
                    
                    if (propertyStructure.count("step-instant") > 0) {
                        STORM_LOG_THROW(propertyStructure.count("time-instant") == 0, storm::exceptions::NotSupportedException, "Storm does not support to have a step-instant and a time-instant in " + scope.description);
                        STORM_LOG_THROW(propertyStructure.count("reward-instants") == 0, storm::exceptions::NotSupportedException, "Storm does not support to have a step-instant and a reward-instant in " + scope.description);

                        storm::expressions::Expression stepInstantExpr = parseExpression(propertyStructure.at("step-instant"), scope.refine("Step instant"));
                        if(rewardAccumulation.isEmpty()) {
                            if (rewExpr.isVariable()) {
                                std::string rewardName = rewExpr.getVariables().begin()->getName();
                                return std::make_shared<storm::logic::RewardOperatorFormula>(std::make_shared<storm::logic::InstantaneousRewardFormula>(stepInstantExpr, storm::logic::TimeBoundType::Steps), rewardName, opInfo);
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Only simple reward expressions are currently supported");
                            }
                        } else {
                            if (rewExpr.isVariable()) {
                                std::string rewardName = rewExpr.getVariables().begin()->getName();
                                return std::make_shared<storm::logic::RewardOperatorFormula>(std::make_shared<storm::logic::CumulativeRewardFormula>(storm::logic::TimeBound(false, stepInstantExpr), storm::logic::TimeBoundReference(storm::logic::TimeBoundType::Steps), rewardAccumulation), rewardName, opInfo);
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Only simple reward expressions are currently supported");
                            }
                        }
                    } else if (propertyStructure.count("time-instant") > 0) {
                        STORM_LOG_THROW(propertyStructure.count("reward-instants") == 0, storm::exceptions::NotSupportedException, "Storm does not support to have a time-instant and a reward-instant in " + scope.description);

                        storm::expressions::Expression timeInstantExpr = parseExpression(propertyStructure.at("time-instant"), scope.refine("time instant"));

                        if(rewardAccumulation.isEmpty()) {
                            if (rewExpr.isVariable()) {
                                std::string rewardName = rewExpr.getVariables().begin()->getName();
                                return std::make_shared<storm::logic::RewardOperatorFormula>(std::make_shared<storm::logic::InstantaneousRewardFormula>(timeInstantExpr, storm::logic::TimeBoundType::Time), rewardName, opInfo);
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Only simple reward expressions are currently supported");
                            }
                        } else {
                            if (rewExpr.isVariable()) {
                                std::string rewardName = rewExpr.getVariables().begin()->getName();
                                return std::make_shared<storm::logic::RewardOperatorFormula>(std::make_shared<storm::logic::CumulativeRewardFormula>(storm::logic::TimeBound(false, timeInstantExpr), storm::logic::TimeBoundReference(storm::logic::TimeBoundType::Time), rewardAccumulation), rewardName, opInfo);
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Only simple reward expressions are currently supported");
                            }
                        }
                    } else if (propertyStructure.count("reward-instants") > 0) {
                        std::vector<storm::logic::TimeBound> bounds;
                        std::vector<storm::logic::TimeBoundReference> boundReferences;
                        for (auto const& rewInst : propertyStructure.at("reward-instants")) {
                            storm::expressions::Expression rewInstExpression = parseExpression(rewInst.at("exp"), scope.refine("Reward expression"));
                            STORM_LOG_THROW(!rewInstExpression.isVariable(), storm::exceptions::NotSupportedException, "Reward bounded cumulative reward formulas should only argue over reward expressions.");
                            storm::logic::RewardAccumulation boundRewardAccumulation = parseRewardAccumulation(rewInst.at("accumulate"), scope.description);
                            boundReferences.emplace_back(rewInstExpression.getVariables().begin()->getName(), boundRewardAccumulation);
                            storm::expressions::Expression rewInstantExpr = parseExpression(rewInst.at("instant"), scope.refine("reward instant"));
                            bounds.emplace_back(false, rewInstantExpr);
                        }
                        if (rewExpr.isVariable()) {
                            std::string rewardName = rewExpr.getVariables().begin()->getName();
                            return std::make_shared<storm::logic::RewardOperatorFormula>(std::make_shared<storm::logic::CumulativeRewardFormula>(bounds, boundReferences, rewardAccumulation), rewardName, opInfo);
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Only simple reward expressions are currently supported");
                        }
                    } else {
                        std::shared_ptr<storm::logic::Formula const> subformula;
                        if (propertyStructure.count("reach") > 0) {
                            auto formulaContext = time ? storm::logic::FormulaContext::Time : storm::logic::FormulaContext::Reward;
                            subformula = std::make_shared<storm::logic::EventuallyFormula>(parseFormula(propertyStructure.at("reach"), formulaContext, scope.refine("Reach-expression of operator " + opString)), formulaContext, rewardAccumulation);
                        } else {
                            subformula = std::make_shared<storm::logic::TotalRewardFormula>(rewardAccumulation);
                        }
                        if (rewExpr.isVariable()) {
                            assert(!time);
                            std::string rewardName = rewExpr.getVariables().begin()->getName();
                            return std::make_shared<storm::logic::RewardOperatorFormula>(subformula, rewardName, opInfo);
                        } else if (!rewExpr.containsVariables()) {
                            assert(time);
                            assert(subformula->isTotalRewardFormula() || subformula->isTimePathFormula());
                            if(rewExpr.hasIntegerType()) {
                                if (rewExpr.evaluateAsInt() == 1) {
                                    return std::make_shared<storm::logic::TimeOperatorFormula>(subformula, opInfo);
                                } else {
                                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Expected steps/time only works with constant one.");
                                }
                            } else if (rewExpr.hasRationalType()){
                                if (rewExpr.evaluateAsDouble() == 1.0) {
                                    
                                    return std::make_shared<storm::logic::TimeOperatorFormula>(subformula, opInfo);
                                } else {
                                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Expected steps/time only works with constant one.");
                                }
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Only numerical reward expressions are allowed");
                            }
                            
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "No complex reward expressions are supported at the moment");
                        }
                    }
                } else if (opString == "Smin" || opString == "Smax") {
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Smin and Smax are currently not supported");
                } else if (opString == "U" || opString == "F") {
                    assert(bound == boost::none);
                    std::vector<std::shared_ptr<storm::logic::Formula const>> args;
                    if (opString == "U") {
                        args = parseBinaryFormulaArguments(propertyStructure, formulaContext, opString, scope);
                    } else {
                        assert(opString == "F");
                        args = parseUnaryFormulaArgument(propertyStructure, formulaContext, opString, scope);
                        args.push_back(args[0]);
                        args[0] = storm::logic::BooleanLiteralFormula::getTrueFormula();
                    }
                    
                    std::vector<boost::optional<storm::logic::TimeBound>> lowerBounds, upperBounds;
                    std::vector<storm::logic::TimeBoundReference> tbReferences;
                    if (propertyStructure.count("step-bounds") > 0) {
                        storm::jani::PropertyInterval pi = parsePropertyInterval(propertyStructure.at("step-bounds"), scope.refine("step-bounded until").clearVariables());
                        boost::optional<storm::logic::TimeBound> lowerBound, upperBound;
                        if (pi.hasLowerBound()) {
                            lowerBounds.push_back(storm::logic::TimeBound(pi.lowerBoundStrict, pi.lowerBound));
                        } else {
                            lowerBounds.push_back(boost::none);
                        }
                        if (pi.hasUpperBound()) {
                            upperBounds.push_back(storm::logic::TimeBound(pi.upperBoundStrict, pi.upperBound));
                        } else {
                            upperBounds.push_back(boost::none);
                        }
                        tbReferences.emplace_back(storm::logic::TimeBoundType::Steps);
                    }
                    if (propertyStructure.count("time-bounds") > 0) {
                        storm::jani::PropertyInterval pi = parsePropertyInterval(propertyStructure.at("time-bounds"), scope.refine("time-bounded until").clearVariables());
                        boost::optional<storm::logic::TimeBound> lowerBound, upperBound;
                        if (pi.hasLowerBound()) {
                            lowerBounds.push_back(storm::logic::TimeBound(pi.lowerBoundStrict, pi.lowerBound));
                        } else {
                            lowerBounds.push_back(boost::none);
                        }
                        if (pi.hasUpperBound()) {
                            upperBounds.push_back(storm::logic::TimeBound(pi.upperBoundStrict, pi.upperBound));
                        } else {
                            upperBounds.push_back(boost::none);
                        }
                        tbReferences.emplace_back(storm::logic::TimeBoundType::Time);
                    }
                    if (propertyStructure.count("reward-bounds") > 0 ) {
                        for (auto const& rbStructure : propertyStructure.at("reward-bounds")) {
                            storm::jani::PropertyInterval pi = parsePropertyInterval(rbStructure.at("bounds"), scope.refine("reward-bounded until").clearVariables());
                            STORM_LOG_THROW(rbStructure.count("exp") == 1, storm::exceptions::InvalidJaniException, "Expecting reward-expression for operator " << opString << " in  " << scope.description);
                            storm::expressions::Expression rewExpr = parseExpression(rbStructure.at("exp"), scope.refine("Reward expression"));
                            STORM_LOG_THROW(rewExpr.isVariable(), storm::exceptions::NotSupportedException, "Storm currently does not support complex reward expressions.");
                            storm::logic::RewardAccumulation boundRewardAccumulation = parseRewardAccumulation(rbStructure.at("accumulate"), scope.description);
                            tbReferences.emplace_back(rewExpr.getVariables().begin()->getName(), boundRewardAccumulation);
                            std::string rewardName = rewExpr.getVariables().begin()->getName();
                            STORM_LOG_WARN("Reward-type (steps, time) is deduced from model type.");
                            if (pi.hasLowerBound()) {
                                lowerBounds.push_back(storm::logic::TimeBound(pi.lowerBoundStrict, pi.lowerBound));
                            } else {
                                lowerBounds.push_back(boost::none);
                            }
                            if (pi.hasUpperBound()) {
                                upperBounds.push_back(storm::logic::TimeBound(pi.upperBoundStrict, pi.upperBound));
                            } else {
                                upperBounds.push_back(boost::none);
                            }
                            tbReferences.push_back(storm::logic::TimeBoundReference(rewardName));
                        }
                    }
                    if (!tbReferences.empty()) {
                        return std::make_shared<storm::logic::BoundedUntilFormula const>(args[0], args[1], lowerBounds, upperBounds, tbReferences);
                    } else if (args[0]->isTrueFormula()) {
                        return std::make_shared<storm::logic::EventuallyFormula const>(args[1], formulaContext);
                    } else {
                        return std::make_shared<storm::logic::UntilFormula const>(args[0], args[1]);
                    }
                } else if (opString == "G") {
                    assert(bound == boost::none);
                    std::vector<std::shared_ptr<storm::logic::Formula const>> args = parseUnaryFormulaArgument(propertyStructure, formulaContext, opString, scope.refine("Subformula of globally operator "));
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
                    std::vector<std::shared_ptr<storm::logic::Formula const>> args = parseBinaryFormulaArguments(propertyStructure, formulaContext, opString, scope);
                    assert(args.size() == 2);
                    storm::logic::BinaryBooleanStateFormula::OperatorType oper = opString ==  "∧" ? storm::logic::BinaryBooleanStateFormula::OperatorType::And : storm::logic::BinaryBooleanStateFormula::OperatorType::Or;
                    return std::make_shared<storm::logic::BinaryBooleanStateFormula const>(oper, args[0], args[1]);
                } else if (opString == "⇒") {
                    assert(bound == boost::none);
                    std::vector<std::shared_ptr<storm::logic::Formula const>> args = parseBinaryFormulaArguments(propertyStructure, formulaContext, opString, scope);
                    assert(args.size() == 2);
                    std::shared_ptr<storm::logic::UnaryBooleanStateFormula const> tmp = std::make_shared<storm::logic::UnaryBooleanStateFormula const>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, args[0]);
                    return std::make_shared<storm::logic::BinaryBooleanStateFormula const>(storm::logic::BinaryBooleanStateFormula::OperatorType::Or, tmp, args[1]);
                } else if (opString == "¬") {
                    assert(bound == boost::none);
                    std::vector<std::shared_ptr<storm::logic::Formula const>> args = parseUnaryFormulaArgument(propertyStructure, formulaContext, opString, scope);
                    assert(args.size() == 1);
                    return std::make_shared<storm::logic::UnaryBooleanStateFormula const>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, args[0]);
                    
                } else if (opString == "≥" || opString == "≤" || opString == "<" || opString == ">" || opString == "=" || opString == "≠") {
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
                    
                    std::vector<std::string> const leftRight = {"left", "right"};
                    for (uint64_t i = 0; i < 2; ++i) {
                        if (propertyStructure.at(leftRight[i]).count("op") > 0) {
                            std::string propertyOperatorString = getString(propertyStructure.at(leftRight[i]).at("op"), "property-operator");
                            std::set<std::string> const propertyOperatorStrings = {"Pmin", "Pmax","Emin", "Emax", "Smin", "Smax"};
                            if (propertyOperatorStrings.count(propertyOperatorString) > 0) {
                                auto boundExpr = parseExpression(propertyStructure.at(leftRight[1-i]), scope.refine("Threshold for operator " + propertyStructure.at(leftRight[i]).at("op").get<std::string>()));
                                if ((opString == "=" || opString == "≠")) {
                                    STORM_LOG_THROW(!boundExpr.containsVariables(), storm::exceptions::NotSupportedException, "Comparison operators '=' or '≠' in property specifications are currently not supported.");
                                    auto boundValue = boundExpr.evaluateAsRational();
                                    if (storm::utility::isZero(boundValue)) {
                                        if (opString == "=") {
                                            ct = storm::logic::ComparisonType::LessEqual;
                                        } else {
                                            ct = storm::logic::ComparisonType::Greater;
                                        }
                                    } else if (storm::utility::isOne(boundValue) && (propertyOperatorString == "Pmin" || propertyOperatorString == "Pmax")) {
                                        if (opString == "=") {
                                            ct = storm::logic::ComparisonType::GreaterEqual;
                                        } else {
                                            ct = storm::logic::ComparisonType::Less;
                                        }
                                    } else {
                                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Comparison operators '=' or '≠' in property specifications are currently not supported.");
                                    }
                                }
                                return parseFormula(propertyStructure.at(leftRight[i]), formulaContext, scope, storm::logic::Bound(ct, boundExpr));
                            }
                        }
                    }
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "No complex comparisons for properties are supported.");
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown operator " << opString);
                }
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Looking for operator for formula " << propertyStructure.dump() << ", but did not find one");
            }
        }
        
        storm::jani::Property JaniParser::parseProperty(json const& propertyStructure, Scope const& scope) {
            STORM_LOG_THROW(propertyStructure.count("name") ==  1, storm::exceptions::InvalidJaniException, "Property must have a name");
            // TODO check unique name
            std::string name = getString(propertyStructure.at("name"), "property-name");
            STORM_LOG_TRACE("Parsing property named: " << name);
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
            std::shared_ptr<storm::logic::Formula const> statesFormula;
            if (expressionStructure.at("states").count("op") > 0) {
                std::string statesDescr = getString(expressionStructure.at("states").at("op"), "Filtered states in property named " + name);
                if (statesDescr == "initial") {
                    statesFormula = std::make_shared<storm::logic::AtomicLabelFormula>("init");
                }
            }
            if (!statesFormula) {
                try {
                    // Try to parse the states as formula.
                    statesFormula = parseFormula(expressionStructure.at("states"), storm::logic::FormulaContext::Undefined, scope.refine("Values of property " + name));
                } catch (storm::exceptions::NotSupportedException const& ex) {
                    throw ex;
                } catch (storm::exceptions::NotImplementedException const& ex) {
                    throw ex;
                }
            }
            STORM_LOG_THROW(statesFormula, storm::exceptions::NotImplementedException, "Could not derive states formula.");
            STORM_LOG_THROW(expressionStructure.count("values") == 1, storm::exceptions::InvalidJaniException, "Values as input for a filter must be given");
            auto formula = parseFormula(expressionStructure.at("values"), storm::logic::FormulaContext::Undefined, scope.refine("Values of property " + name));
            return storm::jani::Property(name, storm::jani::FilterExpression(formula, ft, statesFormula), comment);
        }

        std::shared_ptr<storm::jani::Constant> JaniParser::parseConstant(json const& constantStructure, Scope const& scope) {
            STORM_LOG_THROW(constantStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Variable (scope: " + scope.description + ") must have a name");
            std::string name = getString(constantStructure.at("name"), "variable-name in " + scope.description + "-scope");
            // TODO check existance of name.
            // TODO store prefix in variable.
            std::string exprManagerName = name;
            STORM_LOG_THROW(constantStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "Constant '" + name + "' (scope: " + scope.description + ") must have a (single) type-declaration.");
            size_t valueCount = constantStructure.count("value");
            storm::expressions::Expression initExpr;
            STORM_LOG_THROW(valueCount < 2, storm::exceptions::InvalidJaniException, "Value for constant '" + name +  "'  (scope: " + scope.description + ") must be given at most once.");
            if (valueCount == 1) {
                // Read initial value before; that makes creation later on a bit easier, and has as an additional benefit that we do not need to check whether the variable occurs also on the assignment.
                initExpr = parseExpression(constantStructure.at("value"), scope.refine("Value of constant " + name));
                assert(initExpr.isInitialized());
            }

            if (constantStructure.at("type").is_object()) {
//                STORM_LOG_THROW(variableStructure.at("type").count("kind") == 1, storm::exceptions::InvalidJaniException, "For complex type as in variable " << name << "(scope: " << scope.description << ")  kind must be given");
//                std::string kind = getString(variableStructure.at("type").at("kind"), "kind for complex type as in variable " + name  + "(scope: " + scope.description + ") ");
//                if(kind == "bounded") {
//                    // First do the bounds, that makes the code a bit more streamlined
//                    STORM_LOG_THROW(variableStructure.at("type").count("lower-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scope.description << ") lower-bound must be given");
//                    storm::expressions::Expression lowerboundExpr = parseExpression(variableStructure.at("type").at("lower-bound"), "Lower bound for variable "+ name + " (scope: " + scope.description + ")");
//                    assert(lowerboundExpr.isInitialized());
//                    STORM_LOG_THROW(variableStructure.at("type").count("upper-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scope.description << ") upper-bound must be given");
//                    storm::expressions::Expression upperboundExpr = parseExpression(variableStructure.at("type").at("upper-bound"), "Upper bound for variable "+ name + " (scope: " + scope.description + ")");
//                    assert(upperboundExpr.isInitialized());
//                    STORM_LOG_THROW(variableStructure.at("type").count("base") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << name << "(scope: " << scope.description << ") base must be given");
//                    std::string basictype = getString(variableStructure.at("type").at("base"), "base for bounded type as in variable " + name  + "(scope: " + scope.description + ") ");
//                    if(basictype == "int") {
//                        STORM_LOG_THROW(lowerboundExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Lower bound for bounded integer variable " << name << "(scope: " << scope.description << ") must be integer-typed");
//                        STORM_LOG_THROW(upperboundExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Upper bound for bounded integer variable " << name << "(scope: " << scope.description << ") must be integer-typed");
//                        return std::make_shared<storm::jani::BoundedIntegerVariable>(name, expressionManager->declareIntegerVariable(exprManagerName), lowerboundExpr, upperboundExpr);
//                    } else {
//                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported base " << basictype << " for bounded variable " << name << "(scope: " << scope.description << ") ");
//                    }
//                } else {
//                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported kind " << kind << " for complex type of variable " << name << "(scope: " << scope.description << ") ");
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
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description " << constantStructure.at("type").dump()  << " for constant '" << name << "' (scope: " << scope.description << ")");
                }
            }

            STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description, " << constantStructure.at("type").dump()  << " for Variable '" << name << "' (scope: " << scope.description << ")");
        }
        
        void JaniParser::parseType(ParsedType& result, json const& typeStructure, std::string variableName, Scope const& scope) {
            if (typeStructure.is_string()) {
                if (typeStructure == "real") {
                    result.basicType = ParsedType::BasicType::Real;
                    result.expressionType = expressionManager->getRationalType();
                } else if (typeStructure == "bool") {
                    result.basicType = ParsedType::BasicType::Bool;
                    result.expressionType = expressionManager->getBooleanType();
                } else if (typeStructure == "int") {
                    result.basicType = ParsedType::BasicType::Int;
                    result.expressionType = expressionManager->getIntegerType();
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported type " << typeStructure.dump() << " for variable '" << variableName << "' (scope: " << scope.description << ")");
                }
            } else if (typeStructure.is_object()) {
                STORM_LOG_THROW(typeStructure.count("kind") == 1, storm::exceptions::InvalidJaniException, "For complex type as in variable " << variableName << "(scope: " << scope.description << ")  kind must be given");
                std::string kind = getString(typeStructure.at("kind"), "kind for complex type as in variable " + variableName  + "(scope: " + scope.description + ") ");
                if (kind == "bounded") {
                    STORM_LOG_THROW(typeStructure.count("lower-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << variableName << "(scope: " << scope.description << ") lower-bound must be given");
                    storm::expressions::Expression lowerboundExpr = parseExpression(typeStructure.at("lower-bound"), scope.refine("Lower bound for variable " + variableName));
                    assert(lowerboundExpr.isInitialized());
                    STORM_LOG_THROW(typeStructure.count("upper-bound") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << variableName << "(scope: " << scope.description << ") upper-bound must be given");
                    storm::expressions::Expression upperboundExpr = parseExpression(typeStructure.at("upper-bound"), scope.refine("Upper bound for variable "+ variableName));
                    assert(upperboundExpr.isInitialized());
                    STORM_LOG_THROW(typeStructure.count("base") == 1, storm::exceptions::InvalidJaniException, "For bounded type as in variable " << variableName << "(scope: " << scope.description << ") base must be given");
                    std::string basictype = getString(typeStructure.at("base"), "base for bounded type as in variable " + variableName  + "(scope: " + scope.description + ") ");
                    if (basictype == "int") {
                        STORM_LOG_THROW(lowerboundExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Lower bound for bounded integer variable " << variableName << "(scope: " << scope.description << ") must be integer-typed");
                        STORM_LOG_THROW(upperboundExpr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Upper bound for bounded integer variable " << variableName << "(scope: " << scope.description << ") must be integer-typed");
                        if (!lowerboundExpr.containsVariables() && !upperboundExpr.containsVariables()) {
                            STORM_LOG_THROW(lowerboundExpr.evaluateAsInt() <= upperboundExpr.evaluateAsInt(), storm::exceptions::InvalidJaniException, "Lower bound must not be larger than upper bound for bounded integer variable "  << variableName << "(scope: " << scope.description << ")");
                        }
                        result.basicType = ParsedType::BasicType::Int;
                        result.expressionType = expressionManager->getIntegerType();
                        result.bounds = std::make_pair(lowerboundExpr, upperboundExpr);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported base " << basictype << " for bounded variable " << variableName << "(scope: " << scope.description << ") ");
                    }
                } else if (kind == "array") {
                    STORM_LOG_THROW(typeStructure.count("base") == 1, storm::exceptions::InvalidJaniException, "For array type as in variable " << variableName << "(scope: " << scope.description << ") base must be given");
                    result.arrayBase = std::make_unique<ParsedType>();
                    parseType(*result.arrayBase, typeStructure.at("base"), variableName, scope);
                    result.expressionType = expressionManager->getArrayType(result.arrayBase->expressionType);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unsupported kind " << kind << " for complex type of variable " << variableName << "(scope: " << scope.description << ") ");
                }
            }
        }
        
        storm::jani::FunctionDefinition JaniParser::parseFunctionDefinition(json const& functionDefinitionStructure, Scope const& scope, std::string const& parameterNamePrefix) {
            STORM_LOG_THROW(functionDefinitionStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Function definition (scope: " + scope.description + ") must have a name");
            std::string functionName = getString(functionDefinitionStructure.at("name"), "function-name in " + scope.description);
            // TODO check existence of name.
            STORM_LOG_THROW(functionDefinitionStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "Function definition '" + functionName + "' (scope: " + scope.description + ") must have a (single) type-declaration.");
            ParsedType type;
            parseType(type, functionDefinitionStructure.at("type"), functionName, scope);
            
            std::unordered_map<std::string, storm::expressions::Variable> parameterNameToVariableMap;
            std::vector<storm::expressions::Variable> parameters;
            if (functionDefinitionStructure.count("parameters")) {
                STORM_LOG_THROW(functionDefinitionStructure.count("parameters") == 1, storm::exceptions::InvalidJaniException, "Function definition '" + functionName + "' (scope: " + scope.description + ") must have exactly one list of parameters.");
                for (auto const& parameterStructure : functionDefinitionStructure.at("parameters")) {
                    STORM_LOG_THROW(parameterStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Parameter declaration of parameter " + std::to_string(parameters.size()) + " of Function definition '" + functionName + "' (scope: " + scope.description + ") must have a name");
                    std::string parameterName = getString(parameterStructure.at("name"), "parameter-name of parameter " + std::to_string(parameters.size()) + " of Function definition '" + functionName + "' (scope: " + scope.description + ")");
                    ParsedType parameterType;
                    STORM_LOG_THROW(parameterStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "Parameter declaration of parameter " + std::to_string(parameters.size()) + " of Function definition '" + functionName + "' (scope: " + scope.description + ") must have exactly one type.");
                    parseType(parameterType, parameterStructure.at("type"), parameterName, scope.refine("parameter declaration of parameter " + std::to_string(parameters.size()) + " of function definition " + functionName));
                    STORM_LOG_WARN_COND(!parameterType.bounds.is_initialized(), "Bounds on parameter" + parameterName + " of function definition " + functionName + " will be ignored.");
                    
                    std::string exprParameterName = parameterNamePrefix + functionName + VARIABLE_AUTOMATON_DELIMITER + parameterName;
                    parameters.push_back(expressionManager->declareVariable(exprParameterName, parameterType.expressionType));
                    parameterNameToVariableMap.emplace(parameterName, parameters.back());
                }
            }
            
            STORM_LOG_THROW(functionDefinitionStructure.count("body") == 1, storm::exceptions::InvalidJaniException, "Function definition '" + functionName + "' (scope: " + scope.description + ") must have a (single) body.");
            storm::expressions::Expression functionBody = parseExpression(functionDefinitionStructure.at("body"), scope.refine("body of function definition " + functionName), false, parameterNameToVariableMap);
            STORM_LOG_WARN_COND(functionBody.getType() == type.expressionType, "Type of body of function " + functionName + "' (scope: " + scope.description + ") has type " << functionBody.getType() << " although the function type is given as " << type.expressionType);
            
            return storm::jani::FunctionDefinition(functionName, type.expressionType, parameters, functionBody);
        }

        
        std::shared_ptr<storm::jani::Variable> JaniParser::parseVariable(json const& variableStructure, bool requireInitialValues, Scope const& scope, std::string const& namePrefix) {
            STORM_LOG_THROW(variableStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Variable (scope: " + scope.description + ") must have a name");
            std::string name = getString(variableStructure.at("name"), "variable-name in " + scope.description + "-scope");
            // TODO check existance of name.
            // TODO store prefix in variable.
            std::string exprManagerName = namePrefix + name;
            bool transientVar = defaultVariableTransient; // Default value for variables.
            size_t tvarcount = variableStructure.count("transient");
            STORM_LOG_THROW(tvarcount <= 1, storm::exceptions::InvalidJaniException, "Multiple definitions of transient not allowed in variable '" + name  + "' (scope: " + scope.description + ")  ");
            if(tvarcount == 1) {
                transientVar = getBoolean(variableStructure.at("transient"), "transient-attribute in variable '" + name  + "' (scope: " + scope.description + ")  ");
            }
            STORM_LOG_THROW(variableStructure.count("type") == 1, storm::exceptions::InvalidJaniException, "Variable '" + name + "' (scope: " + scope.description + ") must have a (single) type-declaration.");
            ParsedType type;
            parseType(type, variableStructure.at("type"), name, scope);

            size_t initvalcount = variableStructure.count("initial-value");
            if(transientVar) {
                STORM_LOG_THROW(initvalcount == 1, storm::exceptions::InvalidJaniException, "Initial value must be given once for transient variable '" + name + "' (scope: " + scope.description + ")  "+ name + "' (scope: " + scope.description + ")  ");
            } else {
                STORM_LOG_THROW(initvalcount <= 1, storm::exceptions::InvalidJaniException, "Initial value can be given at most one for variable " + name + "' (scope: " + scope.description + ")");
            }
            boost::optional<storm::expressions::Expression> initVal;
            if (initvalcount == 1 && !variableStructure.at("initial-value").is_null()) {
                initVal = parseExpression(variableStructure.at("initial-value"), scope.refine("Initial value for variable " + name));
            } else {
                assert(!transientVar);
            }
            
            bool setInitValFromDefault = !initVal.is_initialized() && requireInitialValues;
            if (type.basicType) {
                switch (type.basicType.get()) {
                    case ParsedType::BasicType::Real:
                        if (setInitValFromDefault) {
                            initVal = expressionManager->rational(defaultRationalInitialValue);
                        }
                        if (initVal) {
                            STORM_LOG_THROW(initVal.get().hasRationalType() || initVal.get().hasIntegerType(), storm::exceptions::InvalidJaniException, "Initial value for rational variable " + name + "(scope " + scope.description + ") should be a rational");
                            return std::make_shared<storm::jani::RealVariable>(name, expressionManager->declareRationalVariable(exprManagerName), initVal.get(), transientVar);
                        } else {
                            return std::make_shared<storm::jani::RealVariable>(name, expressionManager->declareRationalVariable(exprManagerName));
                        }
                    case ParsedType::BasicType::Int:
                        if (setInitValFromDefault) {
                            if (type.bounds) {
                                initVal = storm::expressions::ite(type.bounds->first < 0 && type.bounds->second > 0, expressionManager->integer(defaultIntegerInitialValue), type.bounds->first);
                                // TODO as soon as we support half-open intervals, we have to change this.
                            } else {
                                initVal = expressionManager->integer(defaultIntegerInitialValue);
                            }
                        }
                        if (initVal) {
                            STORM_LOG_THROW(initVal.get().hasIntegerType(), storm::exceptions::InvalidJaniException, "Initial value for integer variable " + name + "(scope " + scope.description + ") should be an integer");
                            if (type.bounds) {
                                return storm::jani::makeBoundedIntegerVariable(name, expressionManager->declareIntegerVariable(exprManagerName), initVal, transientVar, type.bounds->first, type.bounds->second);
                            } else {
                                return std::make_shared<storm::jani::UnboundedIntegerVariable>(name, expressionManager->declareIntegerVariable(exprManagerName), initVal.get(), transientVar);
                            }
                        } else {
                            if (type.bounds) {
                                return std::make_shared<storm::jani::UnboundedIntegerVariable>(name, expressionManager->declareIntegerVariable(exprManagerName), initVal.get(), transientVar);
                            } else {
                                return storm::jani::makeBoundedIntegerVariable(name, expressionManager->declareIntegerVariable(exprManagerName), boost::none, false, type.bounds->first, type.bounds->second);
                            }
                        }
                        break;
                    case ParsedType::BasicType::Bool:
                        if (setInitValFromDefault) {
                            initVal = expressionManager->boolean(defaultBooleanInitialValue);
                        }
                        if (initVal) {
                            STORM_LOG_THROW(initVal.get().hasBooleanType(), storm::exceptions::InvalidJaniException, "Initial value for boolean variable " + name + "(scope " + scope.description + ") should be a Boolean");
                            if (transientVar) {
                                labels.insert(name);
                            }
                            return std::make_shared<storm::jani::BooleanVariable>(name, expressionManager->declareBooleanVariable(exprManagerName), initVal.get(), transientVar);
                        } else {
                            return std::make_shared<storm::jani::BooleanVariable>(name, expressionManager->declareBooleanVariable(exprManagerName));
                        }
                }
            } else if (type.arrayBase) {
                STORM_LOG_THROW(type.arrayBase->basicType, storm::exceptions::InvalidJaniException, "Array base type for variable " + name + "(scope " + scope.description + ") should be a BasicType or a BoundedType.");
                storm::jani::ArrayVariable::ElementType elementType;
                storm::expressions::Type exprVariableType = type.expressionType;
                switch (type.arrayBase->basicType.get()) {
                    case ParsedType::BasicType::Real:
                        elementType = storm::jani::ArrayVariable::ElementType::Real;
                        break;
                    case ParsedType::BasicType::Bool:
                        elementType = storm::jani::ArrayVariable::ElementType::Bool;
                        break;
                    case ParsedType::BasicType::Int:
                        elementType = storm::jani::ArrayVariable::ElementType::Int;
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unsupported type");
                }
                if (setInitValFromDefault) {
                    initVal = storm::expressions::ValueArrayExpression(*expressionManager, exprVariableType, {}).toExpression();
                }
                std::shared_ptr<storm::jani::ArrayVariable> result;
                if (initVal) {
                    STORM_LOG_THROW(initVal->getType().isArrayType(), storm::exceptions::InvalidJaniException, "Initial value for array variable " + name + "(scope " + scope.description + ") should be an Array");
                    result = std::make_shared<storm::jani::ArrayVariable>(name, expressionManager->declareArrayVariable(exprManagerName, exprVariableType.getElementType()), elementType, initVal.get(), transientVar);
                } else {
                    result = std::make_shared<storm::jani::ArrayVariable>(name, expressionManager->declareArrayVariable(exprManagerName, exprVariableType.getElementType()), elementType);
                }
                if (type.arrayBase->bounds) {
                    result->setElementTypeBounds(type.arrayBase->bounds->first, type.arrayBase->bounds->second);
                }
                return result;
            }
            
            STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown type description, " << variableStructure.at("type").dump()  << " for variable '" << name << "' (scope: " << scope.description << ")");
        }

        /**
         * Helper for parse expression.
         */
        void ensureNumberOfArguments(uint64_t expected, uint64_t actual, std::string const& opstring, std::string const& errorInfo) {
            STORM_LOG_THROW(expected == actual, storm::exceptions::InvalidJaniException, "Operator " << opstring  << " expects " << expected << " arguments, but got " << actual << " in " << errorInfo << ".");
        }

        std::vector<storm::expressions::Expression> JaniParser::parseUnaryExpressionArguments(json const& expressionDecl, std::string const& opstring, Scope const& scope, bool returnNoneInitializedOnUnknownOperator, std::unordered_map<std::string, storm::expressions::Variable> const& auxiliaryVariables) {
            storm::expressions::Expression left = parseExpression(expressionDecl.at("exp"), scope.refine("Argument of operator " + opstring), returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
            return {left};
        }

        std::vector<storm::expressions::Expression> JaniParser::parseBinaryExpressionArguments(json const& expressionDecl, std::string const& opstring, Scope const& scope, bool returnNoneInitializedOnUnknownOperator, std::unordered_map<std::string, storm::expressions::Variable> const& auxiliaryVariables) {
            storm::expressions::Expression left = parseExpression(expressionDecl.at("left"), scope.refine("Left argument of operator " + opstring), returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
            storm::expressions::Expression right = parseExpression(expressionDecl.at("right"), scope.refine("Right argument of operator " + opstring), returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
            return {left, right};
        }
        /**
         * Helper for parse expression.
         */
        void ensureBooleanType(storm::expressions::Expression const& expr, std::string const& opstring, unsigned argNr, std::string const& errorInfo) {
            STORM_LOG_THROW(expr.hasBooleanType(), storm::exceptions::InvalidJaniException, "Operator " << opstring << " expects argument[" << argNr << "]: '" << expr << "' to be Boolean in " << errorInfo << ".");
        }

        /**
         * Helper for parse expression.
         */
        void ensureNumericalType(storm::expressions::Expression const& expr, std::string const& opstring, unsigned argNr, std::string const& errorInfo) {
            STORM_LOG_THROW(expr.hasNumericalType(), storm::exceptions::InvalidJaniException, "Operator " << opstring << " expects argument " + std::to_string(argNr) + " to be numerical in " << errorInfo << ".");
        }

        /**
         * Helper for parse expression.
         */
        void ensureIntegerType(storm::expressions::Expression const& expr, std::string const& opstring, unsigned argNr, std::string const& errorInfo) {
            STORM_LOG_THROW(expr.hasIntegerType(), storm::exceptions::InvalidJaniException, "Operator " << opstring << " expects argument " + std::to_string(argNr) + " to be numerical in " << errorInfo << ".");
        }
        
        /**
         * Helper for parse expression.
         */
        void ensureArrayType(storm::expressions::Expression const& expr, std::string const& opstring, unsigned argNr, std::string const& errorInfo) {
            STORM_LOG_THROW(expr.getType().isArrayType(), storm::exceptions::InvalidJaniException, "Operator " << opstring << " expects argument " + std::to_string(argNr) + " to be of type 'array' in " << errorInfo << ".");
        }

        storm::jani::LValue JaniParser::parseLValue(json const& lValueStructure, Scope const& scope) {
            if (lValueStructure.is_string()) {
                std::string ident = getString(lValueStructure, scope.description);
                if (scope.localVars != nullptr) {
                    auto localVar = scope.localVars->find(ident);
                    if (localVar != scope.localVars->end()) {
                        return storm::jani::LValue(*localVar->second);
                    }
                }
                STORM_LOG_THROW(scope.globalVars != nullptr, storm::exceptions::InvalidJaniException, "Unknown identifier '" << ident << "' occurs in " << scope.description);
                auto globalVar = scope.globalVars->find(ident);
                STORM_LOG_THROW(globalVar != scope.globalVars->end(), storm::exceptions::InvalidJaniException, "Unknown identifier '" << ident << "' occurs in " << scope.description);
                return storm::jani::LValue(*globalVar->second);
            } else if (lValueStructure.count("op") == 1) {
                std::string opstring = getString(lValueStructure.at("op"), scope.description);
                STORM_LOG_THROW(opstring == "aa", storm::exceptions::InvalidJaniException, "Unknown operation '" << opstring << "' occurs in " << scope.description);
                STORM_LOG_THROW(lValueStructure.count("exp"), storm::exceptions::InvalidJaniException, "Missing 'exp' in array access at " << scope.description);
                storm::jani::LValue exp = parseLValue(lValueStructure.at("exp"), scope.refine("LValue description of array expression"));
                STORM_LOG_THROW(lValueStructure.count("index"), storm::exceptions::InvalidJaniException, "Missing 'index' in array access at " << scope.description);
                storm::expressions::Expression index = parseExpression(lValueStructure.at("index"), scope.refine("Index expression of array access"));
                return storm::jani::LValue(exp, index);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown LValue '" << lValueStructure.dump() << "' occurs in " << scope.description);
                // Silly warning suppression.
                return storm::jani::LValue(*scope.globalVars->end()->second);
            }
        }

        storm::expressions::Variable JaniParser::getVariableOrConstantExpression(std::string const& ident, Scope const& scope, std::unordered_map<std::string, storm::expressions::Variable> const& auxiliaryVariables) {
            {
                auto it = auxiliaryVariables.find(ident);
                if (it != auxiliaryVariables.end()) {
                    return it->second;
                }
            }
            if (scope.localVars != nullptr) {
                auto it = scope.localVars->find(ident);
                if (it != scope.localVars->end()) {
                    return it->second->getExpressionVariable();
                }
            }
            if (scope.globalVars != nullptr) {
                auto it = scope.globalVars->find(ident);
                if (it != scope.globalVars->end()) {
                    return it->second->getExpressionVariable();
                }
            }
            if (scope.constants != nullptr) {
                auto it = scope.constants->find(ident);
                if (it != scope.constants->end()) {
                    return it->second->getExpressionVariable();
                }
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown identifier '" << ident << "' occurs in " << scope.description);
            // Silly warning suppression.
            return storm::expressions::Variable();
        }

        storm::expressions::Expression JaniParser::parseExpression(json const& expressionStructure, Scope const& scope, bool returnNoneInitializedOnUnknownOperator, std::unordered_map<std::string, storm::expressions::Variable> const& auxiliaryVariables) {
            if (expressionStructure.is_boolean()) {
                if (expressionStructure.get<bool>()) {
                    return expressionManager->boolean(true);
                } else {
                    return expressionManager->boolean(false);
                }
            } else if (expressionStructure.is_number_integer()) {
                return expressionManager->integer(expressionStructure.get<int64_t>());
            } else if (expressionStructure.is_number_float()) {
                // For now, just take the double.
                // TODO make this a rational number
                return expressionManager->rational(expressionStructure.get<double>());
            } else if (expressionStructure.is_string()) {
                std::string ident = expressionStructure.get<std::string>();
                return storm::expressions::Expression(getVariableOrConstantExpression(ident, scope, auxiliaryVariables));
            } else if (expressionStructure.is_object()) {
                if (expressionStructure.count("distribution") == 1) {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Distributions are not supported by storm expressions, cannot import " << expressionStructure.dump() << " in  " << scope.description << ".");
                }
                if (expressionStructure.count("op") == 1) {
                    std::string opstring = getString(expressionStructure.at("op"), scope.description);
                    std::vector<storm::expressions::Expression> arguments = {};
                    if(opstring == "ite") {
                        STORM_LOG_THROW(expressionStructure.count("if") == 1, storm::exceptions::InvalidJaniException, "If operator required");
                        STORM_LOG_THROW(expressionStructure.count("else") == 1, storm::exceptions::InvalidJaniException, "Else operator required");
                        STORM_LOG_THROW(expressionStructure.count("then") == 1, storm::exceptions::InvalidJaniException, "Then operator required");
                        arguments.push_back(parseExpression(expressionStructure.at("if"), scope.refine("if-formula"), returnNoneInitializedOnUnknownOperator, auxiliaryVariables));
                        arguments.push_back(parseExpression(expressionStructure.at("then"), scope.refine("then-formula"), returnNoneInitializedOnUnknownOperator, auxiliaryVariables));
                        arguments.push_back(parseExpression(expressionStructure.at("else"), scope.refine("else-formula"), returnNoneInitializedOnUnknownOperator, auxiliaryVariables));
                        ensureNumberOfArguments(3, arguments.size(), opstring, scope.description);
                        assert(arguments.size() == 3);
                                            ensureBooleanType(arguments[0], opstring, 0, scope.description);
                        return storm::expressions::ite(arguments[0], arguments[1], arguments[2]);
                    } else if (opstring == "∨") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureBooleanType(arguments[0], opstring, 0, scope.description);
                        ensureBooleanType(arguments[1], opstring, 1, scope.description);
                        return arguments[0] || arguments[1];
                    } else if (opstring == "∧") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureBooleanType(arguments[0], opstring, 0, scope.description);
                        ensureBooleanType(arguments[1], opstring, 1, scope.description);
                        return arguments[0] && arguments[1];
                    } else if (opstring == "⇒") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureBooleanType(arguments[0], opstring, 0, scope.description);
                        ensureBooleanType(arguments[1], opstring, 1, scope.description);
                        return (!arguments[0]) || arguments[1];
                    } else if (opstring == "¬") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 1);
                        if(!arguments[0].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureBooleanType(arguments[0], opstring, 0, scope.description);
                        return !arguments[0];
                    } else if (opstring == "=") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        if(arguments[0].hasBooleanType()) {
                            ensureBooleanType(arguments[1], opstring, 1, scope.description);
                            return storm::expressions::iff(arguments[0], arguments[1]);
                        } else {
                            ensureNumericalType(arguments[1], opstring, 1, scope.description);
                            return arguments[0] == arguments[1];
                        }
                    } else if (opstring == "≠") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        if(arguments[0].hasBooleanType()) {
                            ensureBooleanType(arguments[1], opstring, 1, scope.description);
                            return storm::expressions::xclusiveor(arguments[0], arguments[1]);
                        } else {
                            ensureNumericalType(arguments[1], opstring, 1, scope.description);
                            return arguments[0] != arguments[1];
                        }
                    } else if (opstring == "<") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return arguments[0] < arguments[1];
                    } else if (opstring == "≤") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return arguments[0] <= arguments[1];
                    } else if (opstring == ">") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return arguments[0] > arguments[1];
                    } else if (opstring == "≥") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        if(!arguments[0].isInitialized() || !arguments[1].isInitialized()) {
                            return storm::expressions::Expression();
                        }
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return arguments[0] >= arguments[1];
                    } else if (opstring == "+") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return arguments[0] + arguments[1];
                    } else if (opstring == "-") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return arguments[0] - arguments[1];
                    } else if (opstring == "-") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        return -arguments[0];
                    } else if (opstring == "*") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return arguments[0] * arguments[1];
                    } else if (opstring == "/") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return arguments[0] / arguments[1];
                    } else if (opstring == "%") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return arguments[0] % arguments[1];
                    } else if (opstring == "max") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return storm::expressions::maximum(arguments[0],arguments[1]);
                    } else if (opstring == "min") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return storm::expressions::minimum(arguments[0],arguments[1]);
                    } else if (opstring == "floor") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        return storm::expressions::floor(arguments[0]);
                    } else if (opstring == "ceil") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        return storm::expressions::ceil(arguments[0]);
                    } else if (opstring == "abs") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        return storm::expressions::abs(arguments[0]);
                    } else if (opstring == "sgn") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        return storm::expressions::sign(arguments[0]);
                    } else if (opstring == "trc") {
                        arguments = parseUnaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 1);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        return storm::expressions::truncate(arguments[0]);
                    } else if (opstring == "pow") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        return arguments[0]^arguments[1];
                    } else if (opstring == "exp") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "exp operation is not yet implemented");
                    } else if (opstring == "log") {
                        arguments = parseBinaryExpressionArguments(expressionStructure, opstring, scope, returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        assert(arguments.size() == 2);
                        ensureNumericalType(arguments[0], opstring, 0, scope.description);
                        ensureNumericalType(arguments[1], opstring, 1, scope.description);
                        // TODO implement
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "log operation is not yet implemented");
                    } else if (opstring == "aa") {
                        STORM_LOG_THROW(expressionStructure.count("exp") == 1, storm::exceptions::InvalidJaniException, "Array access operator requires exactly one exp (at " + scope.description + ").");
                        storm::expressions::Expression exp = parseExpression(expressionStructure.at("exp"), scope.refine("'exp' of array access operator"), returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        STORM_LOG_THROW(expressionStructure.count("index") == 1, storm::exceptions::InvalidJaniException, "Array access operator requires exactly one index (at " + scope.description + ").");
                        storm::expressions::Expression index = parseExpression(expressionStructure.at("index"), scope.refine("index of array access operator"), returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        ensureArrayType(exp, opstring, 0, scope.description);
                        ensureIntegerType(index, opstring, 1, scope.description);
                        return std::make_shared<storm::expressions::ArrayAccessExpression>(exp.getManager(), exp.getType().getElementType(), exp.getBaseExpressionPointer(), index.getBaseExpressionPointer())->toExpression();
                    } else if (opstring == "av") {
                        STORM_LOG_THROW(expressionStructure.count("elements") == 1, storm::exceptions::InvalidJaniException, "Array value operator requires exactly one 'elements' (at " + scope.description + ").");
                        std::vector<std::shared_ptr<storm::expressions::BaseExpression const>> elements;
                        storm::expressions::Type commonType;
                        bool first = true;
                        for (auto const& element : expressionStructure.at("elements")) {
                            elements.push_back(parseExpression(element, scope.refine("element " + std::to_string(elements.size()) + " of array value expression"), returnNoneInitializedOnUnknownOperator, auxiliaryVariables).getBaseExpressionPointer());
                            if (first) {
                                commonType = elements.back()->getType();
                                first = false;
                            } else if (!(commonType == elements.back()->getType())) {
                                if (commonType.isIntegerType() && elements.back()->getType().isRationalType()) {
                                    commonType = elements.back()->getType();
                                } else {
                                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Incompatible element types " << commonType << " and " << elements.back()->getType() << " of array value expression at " << scope.description);
                                }
                            }
                        }
                        return std::make_shared<storm::expressions::ValueArrayExpression>(*expressionManager, expressionManager->getArrayType(commonType), elements)->toExpression();
                    } else if (opstring == "ac") {
                        STORM_LOG_THROW(expressionStructure.count("length") == 1, storm::exceptions::InvalidJaniException, "Array access operator requires exactly one length (at " + scope.description + ").");
                        storm::expressions::Expression length = parseExpression(expressionStructure.at("length"), scope.refine("index of array constructor expression"), returnNoneInitializedOnUnknownOperator, auxiliaryVariables);
                        ensureIntegerType(length, opstring, 1, scope.description);
                        STORM_LOG_THROW(expressionStructure.count("var") == 1, storm::exceptions::InvalidJaniException, "Array access operator requires exactly one var (at " + scope.description + ").");
                        std::string indexVarName = getString(expressionStructure.at("var"), "Field 'var' of Array access operator (at " + scope.description + ").");
                        STORM_LOG_THROW(auxiliaryVariables.find(indexVarName) == auxiliaryVariables.end(), storm::exceptions::InvalidJaniException, "Index variable " << indexVarName << " is already defined as an auxiliary variable (at " + scope.description + ").");
                        auto newAuxVars = auxiliaryVariables;
                        storm::expressions::Variable indexVar = expressionManager->declareFreshIntegerVariable(false, "ac_" + indexVarName);
                        newAuxVars.emplace(indexVarName, indexVar);
                        STORM_LOG_THROW(expressionStructure.count("exp") == 1, storm::exceptions::InvalidJaniException, "Array constructor operator requires exactly one exp (at " + scope.description + ").");
                        storm::expressions::Expression exp = parseExpression(expressionStructure.at("exp"), scope.refine("exp of array constructor"), returnNoneInitializedOnUnknownOperator, newAuxVars);
                        return std::make_shared<storm::expressions::ConstructorArrayExpression>(*expressionManager, expressionManager->getArrayType(exp.getType()), length.getBaseExpressionPointer(), indexVar, exp.getBaseExpressionPointer())->toExpression();
                    } else if (opstring == "call") {
                        STORM_LOG_THROW(expressionStructure.count("function") == 1, storm::exceptions::InvalidJaniException, "Function call operator requires exactly one function (at " + scope.description + ").");
                        std::string functionName = getString(expressionStructure.at("function"), "in function call operator (at " + scope.description + ").");
                        storm::jani::FunctionDefinition const* functionDefinition;
                        if (scope.localFunctions != nullptr && scope.localFunctions->count(functionName) > 0) {
                            functionDefinition = scope.localFunctions->at(functionName);
                        } else if (scope.globalFunctions != nullptr && scope.globalFunctions->count(functionName) > 0){
                            functionDefinition = scope.globalFunctions->at(functionName);
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Function call operator calls unknown function '" + functionName + "' (at " + scope.description + ").");
                        }
                        STORM_LOG_THROW(expressionStructure.count("args") == 1, storm::exceptions::InvalidJaniException, "Function call operator requires exactly one args (at " + scope.description + ").");
                        std::vector<std::shared_ptr<storm::expressions::BaseExpression const>> args;
                        if (expressionStructure.count("args") > 0) {
                            STORM_LOG_THROW(expressionStructure.count("args") == 1, storm::exceptions::InvalidJaniException, "Function call operator requires exactly one args (at " + scope.description + ").");
                            for (auto const& arg : expressionStructure.at("args")) {
                                args.push_back(parseExpression(arg, scope.refine("argument " + std::to_string(args.size()) + " of function call expression"), returnNoneInitializedOnUnknownOperator, auxiliaryVariables).getBaseExpressionPointer());
                            }
                        }
                        return std::make_shared<storm::expressions::FunctionCallExpression>(*expressionManager, functionDefinition->getType(), functionName, args)->toExpression();
                    }  else if (unsupportedOpstrings.count(opstring) > 0) {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Opstring " + opstring + " is not supported by storm");
                    } else {
                        if(returnNoneInitializedOnUnknownOperator) {
                            return storm::expressions::Expression();
                        }
                        STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Unknown operator " << opstring << " in  " << scope.description << ".");
                    }
                }
                STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "No supported operator declaration found for complex expressions as " << expressionStructure.dump() << " in  " << scope.description << ".");
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "No supported expression found at " << expressionStructure.dump() << " in  " << scope.description << ".");
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

        storm::jani::Automaton JaniParser::parseAutomaton(json const &automatonStructure, storm::jani::Model const& parentModel, Scope const& globalScope) {
            STORM_LOG_THROW(automatonStructure.count("name") == 1, storm::exceptions::InvalidJaniException, "Each automaton must have a name");
            std::string name = getString(automatonStructure.at("name"), " the name field for automaton");
            Scope scope = globalScope.refine(name);
            storm::jani::Automaton automaton(name, expressionManager->declareIntegerVariable("_loc_" + name));

            uint64_t varDeclCount = automatonStructure.count("variables");
            STORM_LOG_THROW(varDeclCount < 2, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' has more than one list of variables");
            VariablesMap localVars;
            scope.localVars = &localVars;
            if (varDeclCount > 0) {
                bool requireInitialValues = automatonStructure.count("restrict-initial") == 0;
                for(auto const& varStructure : automatonStructure.at("variables")) {
                    std::shared_ptr<storm::jani::Variable> var = parseVariable(varStructure, requireInitialValues, scope.refine("variables[" + std::to_string(localVars.size()) + "] of automaton " + name), name + VARIABLE_AUTOMATON_DELIMITER);
                    assert(localVars.count(var->getName()) == 0);
                    localVars.emplace(var->getName(), &automaton.addVariable(*var));
                }
            }

            uint64_t funDeclCount = automatonStructure.count("functions");
            STORM_LOG_THROW(funDeclCount < 2, storm::exceptions::InvalidJaniException, "Automaton '" << name << "' has more than one list of functions");
            FunctionsMap localFuns;
            scope.localFunctions = &localFuns;
            if (funDeclCount > 0) {
                for (auto const& funStructure : automatonStructure.at("functions")) {
                    storm::jani::FunctionDefinition funDef = parseFunctionDefinition(funStructure, scope.refine("functions[" + std::to_string(localFuns.size()) + "] of automaton " + name),  name + VARIABLE_AUTOMATON_DELIMITER);
                    assert(localFuns.count(funDef.getName()) == 0);
                    //TODO localVars.emplace(funDef.getName(), &automaton.addFunction(funDef));
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
                        storm::jani::LValue lValue = parseLValue(transientValueEntry.at("ref"), scope.refine("LHS of assignment in location " + locName));
                        STORM_LOG_THROW(lValue.isTransient(), storm::exceptions::InvalidJaniException, "Assigned non-transient variable " << lValue << " in location " + locName + " (automaton: '" + name + "')");
                        storm::expressions::Expression rhs = parseExpression(transientValueEntry.at("value"), scope.refine("Assignment of lValue in location " + locName));
                        transientAssignments.emplace_back(lValue, rhs);
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
                initialValueRestriction  = parseExpression(automatonStructure.at("restrict-initial").at("exp"), scope.refine("Initial value restriction"));
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
                    rateExpr = parseExpression(edgeEntry.at("rate").at("exp"), scope.refine("rate expression in edge from '" + sourceLoc));
                    STORM_LOG_THROW(rateExpr.hasNumericalType(), storm::exceptions::InvalidJaniException, "Rate '" << rateExpr << "' has not a numerical type");
                }
                // guard
                STORM_LOG_THROW(edgeEntry.count("guard") <= 1, storm::exceptions::InvalidJaniException, "Guard can be given at most once in edge from '" << sourceLoc << "' in automaton '" << name << "'");
                storm::expressions::Expression guardExpr = expressionManager->boolean(true);
                if(edgeEntry.count("guard") == 1) {
                    STORM_LOG_THROW(edgeEntry.at("guard").count("exp") == 1, storm::exceptions::InvalidJaniException, "Guard  in edge from '" + sourceLoc + "' in automaton '" + name + "' must have one expression");
                    guardExpr = parseExpression(edgeEntry.at("guard").at("exp"), scope.refine("guard expression in edge from '" + sourceLoc));
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
                        probExpr = parseExpression(destEntry.at("probability").at("exp"), scope.refine("probability expression in edge from '" + sourceLoc + "' to  '"  + targetLoc + "' in automaton '" + name + "'"));
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
                            storm::jani::LValue lValue = parseLValue(assignmentEntry.at("ref"), scope.refine("Assignment variable in edge from '" + sourceLoc + "' to '" + targetLoc + "' in automaton '" + name + "'"));
                            // value
                            STORM_LOG_THROW(assignmentEntry.count("value") == 1, storm::exceptions::InvalidJaniException, "Assignment in edge from '" << sourceLoc << "' to '" << targetLoc << "' in automaton '" << name << "'  must have one value field");
                            storm::expressions::Expression assignmentExpr = parseExpression(assignmentEntry.at("value"), scope.refine("assignment in edge from '" + sourceLoc + "' to '" + targetLoc + "' in automaton '" + name + "'"));
                            // TODO check types
                            // index
                            int64_t assignmentIndex = 0; // default.
                            if(assignmentEntry.count("index") > 0) {
                                assignmentIndex = getSignedInt(assignmentEntry.at("index"), "assignment index in edge from '" + sourceLoc + "' to '" + targetLoc + "' in automaton '" + name + "'");
                            }
                            assignments.emplace_back(lValue, assignmentExpr, assignmentIndex);
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
