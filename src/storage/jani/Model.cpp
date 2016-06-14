#include "src/storage/jani/Model.h"

#include "src/storage/expressions/ExpressionManager.h"

#include "src/storage/jani/AutomatonComposition.h"
#include "src/storage/jani/ParallelComposition.h"
#include "src/storage/jani/RenameComposition.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace jani {
        
        static const std::string SILENT_ACTION_NAME = "";
        
        Model::Model() {
            // Intentionally left empty.
        }
        
        Model::Model(std::string const& name, ModelType const& modelType, uint64_t version, boost::optional<std::shared_ptr<storm::expressions::ExpressionManager>> const& expressionManager) : name(name), modelType(modelType), version(version), composition(nullptr) {
            // Use the provided manager or create a new one.
            if (expressionManager) {
                this->expressionManager = expressionManager.get();
            } else {
                this->expressionManager = std::make_shared<storm::expressions::ExpressionManager>();
            }
            
            // Add a prefined action that represents the silent action.
            silentActionIndex = addAction(storm::jani::Action(SILENT_ACTION_NAME));
        }
        
        uint64_t Model::getJaniVersion() const {
            return version;
        }
        
        ModelType const& Model::getModelType() const {
            return modelType;
        }
        
        std::string const& Model::getName() const {
            return name;
        }
        
        uint64_t Model::addAction(Action const& action) {
            auto it = actionToIndex.find(action.getName());
            STORM_LOG_THROW(it == actionToIndex.end(), storm::exceptions::WrongFormatException, "Action with name '" << action.getName() << "' already exists");
            actionToIndex.emplace(action.getName(), actions.size());
            actions.push_back(action);
            return actions.size() - 1;
        }
        
        Action const& Model::getAction(uint64_t index) const {
            return actions[index];
        }
        
        bool Model::hasAction(std::string const& name) const {
            return actionToIndex.find(name) != actionToIndex.end();
        }
        
        uint64_t Model::getActionIndex(std::string const& name) const {
            auto it = actionToIndex.find(name);
            STORM_LOG_THROW(it != actionToIndex.end(), storm::exceptions::InvalidOperationException, "Unable to retrieve index of unknown action '" << name << "'.");
            return it->second;
        }
        
        std::vector<Action> const& Model::getActions() const {
            return actions;
        }
        
        uint64_t Model::addConstant(Constant const& constant) {
            auto it = constantToIndex.find(constant.getName());
            STORM_LOG_THROW(it == constantToIndex.end(), storm::exceptions::WrongFormatException, "Cannot add constant with name '" << constant.getName() << "', because a constant with that name already exists.");
            constantToIndex.emplace(constant.getName(), constants.size());
            constants.push_back(constant);
            return constants.size() - 1;
        }
        
        bool Model::hasConstant(std::string const& name) const {
            return constantToIndex.find(name) != constantToIndex.end();
        }
        
        Constant const& Model::getConstant(std::string const& name) const {
            auto it = constantToIndex.find(name);
            STORM_LOG_THROW(it != constantToIndex.end(), storm::exceptions::WrongFormatException, "Unable to retrieve unknown constant '" << name << "'.");
            return constants[it->second];
        }
        
        std::vector<Constant> const& Model::getConstants() const {
            return constants;
        }

        std::vector<Constant>& Model::getConstants() {
            return constants;
        }

        void Model::addBooleanVariable(BooleanVariable const& variable) {
            globalVariables.addBooleanVariable(variable);
        }
        
        void Model::addBoundedIntegerVariable(BoundedIntegerVariable const& variable) {
            globalVariables.addBoundedIntegerVariable(variable);
        }
        
        void Model::addUnboundedIntegerVariable(UnboundedIntegerVariable const& variable) {
            globalVariables.addUnboundedIntegerVariable(variable);
        }

        VariableSet& Model::getGlobalVariables() {
            return globalVariables;
        }

        VariableSet const& Model::getGlobalVariables() const {
            return globalVariables;
        }
        
        storm::expressions::ExpressionManager& Model::getExpressionManager() {
            return *expressionManager;
        }
        
        storm::expressions::ExpressionManager const& Model::getExpressionManager() const {
            return *expressionManager;
        }
        
        uint64_t Model::addAutomaton(Automaton const& automaton) {
            auto it = automatonToIndex.find(automaton.getName());
            STORM_LOG_THROW(it == automatonToIndex.end(), storm::exceptions::WrongFormatException, "Automaton with name '" << automaton.getName() << "' already exists.");
            automatonToIndex.emplace(automaton.getName(), automata.size());
            automata.push_back(automaton);
            return automata.size() - 1;
        }
        
        std::vector<Automaton>& Model::getAutomata() {
            return automata;
        }
        
        std::vector<Automaton> const& Model::getAutomata() const {
            return automata;
        }
        
        Automaton& Model::getAutomaton(std::string const& name) {
            auto it = automatonToIndex.find(name);
            STORM_LOG_THROW(it != automatonToIndex.end(), storm::exceptions::InvalidOperationException, "Unable to retrieve unknown automaton '" << name << "'.");
            return automata[it->second];
        }
        
        Automaton const& Model::getAutomaton(std::string const& name) const {
            auto it = automatonToIndex.find(name);
            STORM_LOG_THROW(it != automatonToIndex.end(), storm::exceptions::InvalidOperationException, "Unable to retrieve unknown automaton '" << name << "'.");
            return automata[it->second];
        }
        
        std::size_t Model::getNumberOfAutomata() const {
            return automata.size();
        }
        
        std::shared_ptr<Composition> Model::getStandardSystemComposition() const {
            std::shared_ptr<Composition> current;
            current = std::make_shared<AutomatonComposition>(this->automata.front().getName());
            std::set<uint64_t> leftHandActionIndices = this->automata.front().getActionIndices();
            
            for (uint64_t index = 1; index < automata.size(); ++index) {
                std::set<uint64_t> newActionIndices = automata[index].getActionIndices();
                
                // Compute the intersection of actions of the left- and right-hand side.
                std::set<uint64_t> intersectionActions;
                std::set_intersection(leftHandActionIndices.begin(), leftHandActionIndices.end(), newActionIndices.begin(), newActionIndices.end(), std::inserter(intersectionActions, intersectionActions.begin()));
                
                // If the silent action is in the intersection, we remove it since we cannot synchronize over it.
                auto it = intersectionActions.find(this->getSilentActionIndex());
                if (it != intersectionActions.end()) {
                    intersectionActions.erase(it);
                }
                
                // Then join the actions to reflect the actions of the new left-hand side.
                leftHandActionIndices.insert(newActionIndices.begin(), newActionIndices.end());
                
                // Create the set of strings that represents the actions over which to synchronize.
                std::set<std::string> intersectionActionNames;
                for (auto const& actionIndex : intersectionActions) {
                    intersectionActionNames.insert(this->getAction(actionIndex).getName());
                }

                current = std::make_shared<ParallelComposition>(current, std::make_shared<AutomatonComposition>(automata[index].getName()), intersectionActionNames);
            }
            return current;
        }
        
        Composition const& Model::getSystemComposition() const {
            return *composition;
        }
        
        void Model::setSystemComposition(std::shared_ptr<Composition> const& composition) {
            this->composition = composition;
        }
        
        std::set<std::string> Model::getActionNames(bool includeSilent) const {
            std::set<std::string> result;
            for (auto const& entry : actionToIndex) {
                if (includeSilent || entry.second != silentActionIndex) {
                    result.insert(entry.first);
                }
            }
            return result;
        }
        
        std::string const& Model::getSilentActionName() const {
            return actions[silentActionIndex].getName();
        }
        
        uint64_t Model::getSilentActionIndex() const {
            return silentActionIndex;
        }
        
        Model Model::defineUndefinedConstants(std::map<storm::expressions::Variable, storm::expressions::Expression> const& constantDefinitions) const {
            Model result(*this);
            
            std::set<storm::expressions::Variable> definedUndefinedConstants;
            for (auto& constant : result.constants) {
                // If the constant is already defined, we need to replace the appearances of undefined constants in its
                // defining expression
                if (constant.isDefined()) {
                    // Make sure we are not trying to define an already defined constant.
                    STORM_LOG_THROW(constantDefinitions.find(constant.getExpressionVariable()) == constantDefinitions.end(), storm::exceptions::InvalidOperationException, "Illegally defining already defined constant '" << constant.getName() << "'.");
                } else {
                    auto const& variableExpressionPair = constantDefinitions.find(constant.getExpressionVariable());
                    
                    if (variableExpressionPair != constantDefinitions.end()) {
                        // If we need to define it, we add it to the defined constants and assign it the appropriate expression.
                        definedUndefinedConstants.insert(constant.getExpressionVariable());
                        
                        // Make sure the type of the constant is correct.
                        STORM_LOG_THROW(variableExpressionPair->second.getType() == constant.getType(), storm::exceptions::InvalidOperationException, "Illegal type of expression defining constant '" << constant.getName() << "'.");
                        
                        // Now define the constant.
                        constant.define(variableExpressionPair->second);
                    }
                }
            }
            
            // As a sanity check, we make sure that the given mapping does not contain any definitions for identifiers
            // that are not undefined constants.
            for (auto const& constantExpressionPair : constantDefinitions) {
                STORM_LOG_THROW(definedUndefinedConstants.find(constantExpressionPair.first) != definedUndefinedConstants.end(), storm::exceptions::InvalidOperationException, "Unable to define non-existant constant '" << constantExpressionPair.first.getName() << "'.");
            }
            return result;
        }
        
        bool Model::hasUndefinedConstants() const {
            for (auto const& constant : constants) {
                if (!constant.isDefined()) {
                    return true;
                }
            }
            return false;
        }
        
        std::vector<std::reference_wrapper<Constant const>> Model::getUndefinedConstants() const {
            std::vector<std::reference_wrapper<Constant const>> result;
            
            for (auto const& constant : constants) {
                if (!constant.isDefined()) {
                    result.push_back(constant);
                }
            }
            
            return result;
        }
        
        Model Model::substituteConstants() const {
            Model result(*this);

            // Gather all defining expressions of constants.
            std::map<storm::expressions::Variable, storm::expressions::Expression> constantSubstitution;
            for (auto& constant : result.getConstants()) {
                if (constant.isDefined()) {
                    constant.define(constant.getExpression().substitute(constantSubstitution));
                    constantSubstitution[constant.getExpressionVariable()] = constant.getExpression();
                }
            }
            
            // Substitute constants in all global variables.
            for (auto& variable : result.getGlobalVariables().getBoundedIntegerVariables()) {
                variable.setLowerBound(variable.getLowerBound().substitute(constantSubstitution));
                variable.setUpperBound(variable.getUpperBound().substitute(constantSubstitution));
            }
            
            // Substitute constants in initial states expression.
            if (this->hasInitialStatesExpression()) {
                result.setInitialStatesExpression(this->getInitialStatesExpression().substitute(constantSubstitution));
            }
            
            // Substitute constants in variables of automata and their edges.
            for (auto& automaton : result.getAutomata()) {
                for (auto& variable : automaton.getVariables().getBoundedIntegerVariables()) {
                    variable.setLowerBound(variable.getLowerBound().substitute(constantSubstitution));
                    variable.setUpperBound(variable.getUpperBound().substitute(constantSubstitution));
                }
                
                if (automaton.hasInitialStatesExpression()) {
                    automaton.setInitialStatesExpression(automaton.getInitialStatesExpression().substitute(constantSubstitution));
                }
                
                for (auto& edge : automaton.getEdges()) {
                    edge.setGuard(edge.getGuard().substitute(constantSubstitution));
                    if (edge.hasRate()) {
                        edge.setRate(edge.getRate().substitute(constantSubstitution));
                    }
                    for (auto& destination : edge.getDestinations()) {
                        destination.setProbability(destination.getProbability().substitute(constantSubstitution));
                        for (auto& assignment : destination.getAssignments()) {
                            assignment.setAssignedExpression(assignment.getAssignedExpression().substitute(constantSubstitution));
                        }
                    }
                }
            }
            
            return result;
        }
        
        std::map<storm::expressions::Variable, storm::expressions::Expression> Model::getConstantsSubstitution() const {
            std::map<storm::expressions::Variable, storm::expressions::Expression> result;
            
            for (auto const& constant : constants) {
                if (constant.isDefined()) {
                    result.emplace(constant.getExpressionVariable(), constant.getExpression());
                }
            }
            
            return result;
        }
        
        bool Model::hasInitialStatesExpression() const {
            return initialStatesExpression.isInitialized();
        }
     
        storm::expressions::Expression Model::getInitialStatesExpression(bool includeAutomataInitialStatesExpressions) const {
            STORM_LOG_THROW(globalVariables.empty() || this->hasInitialStatesExpression(), storm::exceptions::InvalidOperationException, "Cannot retrieve global initial states expression, because there is none.");
            storm::expressions::Expression result = this->hasInitialStatesExpression() ? initialStatesExpression : expressionManager->boolean(true);
            if (includeAutomataInitialStatesExpressions) {
                for (auto const& automaton : automata) {
                    STORM_LOG_THROW(automaton.getVariables().empty() || automaton.hasInitialStatesExpression(), storm::exceptions::InvalidOperationException, "Cannot retrieve initial states expression from automaton '" << automaton.getName() << "', because there is none.");
                    if (!automaton.getVariables().empty()) {
                        result = result && automaton.getInitialStatesExpression();
                    }
                }
            }
            return result;
        }
        
        void Model::setInitialStatesExpression(storm::expressions::Expression const& initialStatesExpression) {
            this->initialStatesExpression = initialStatesExpression;
        }
        
        bool Model::isDeterministicModel() const {
            return this->getModelType() == ModelType::DTMC || this->getModelType() == ModelType::CTMC;
        }
        
        bool Model::checkValidity(bool logdbg) const {
            // TODO switch to exception based return value.
            
            if (version == 0) {
                if(logdbg) STORM_LOG_DEBUG("Jani version is unspecified");
                return false;
            }
            
            if(modelType == ModelType::UNDEFINED) {
                if(logdbg) STORM_LOG_DEBUG("Model type is unspecified");
                return false;
            }
            
            if(automata.empty()) {
                if(logdbg) STORM_LOG_DEBUG("No automata specified");
                return false;
            }
            // All checks passed.
            return true;
            
        }
        
    }
}