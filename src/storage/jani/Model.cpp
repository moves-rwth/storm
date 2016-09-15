#include "src/storage/jani/Model.h"

#include "src/storage/expressions/ExpressionManager.h"

#include "src/storage/jani/Compositions.h"
#include "src/storage/jani/CompositionInformationVisitor.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidOperationException.h"
#include "src/exceptions/InvalidTypeException.h"

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
            
            // Create an initial restriction.
            initialStatesRestriction = this->expressionManager->boolean(true);
            
            // Add a prefined action that represents the silent action.
            silentActionIndex = addAction(storm::jani::Action(SILENT_ACTION_NAME));
        }
        
        storm::expressions::ExpressionManager& Model::getManager() const {
            return *expressionManager;
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
            if (action.getName() != SILENT_ACTION_NAME) {
                nonsilentActionIndices.insert(actions.size() - 1);
            }
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
        
        std::unordered_map<std::string, uint64_t> const& Model::getActionToIndexMap() const {
            return actionToIndex;
        }
        
        std::vector<Action> const& Model::getActions() const {
            return actions;
        }
        
        boost::container::flat_set<uint64_t> const& Model::getNonsilentActionIndices() const {
            return nonsilentActionIndices;
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
        
        Variable const& Model::addVariable(Variable const& variable) {
            if (variable.isBooleanVariable()) {
                return addVariable(variable.asBooleanVariable());
            } else if (variable.isBoundedIntegerVariable()) {
                return addVariable(variable.asBoundedIntegerVariable());
            } else if (variable.isUnboundedIntegerVariable()) {
                return addVariable(variable.asUnboundedIntegerVariable());
            } else if (variable.isRealVariable()) {
                return addVariable(variable.asRealVariable());
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Variable has invalid type.");
            }
        }

        BooleanVariable const& Model::addVariable(BooleanVariable const& variable) {
            return globalVariables.addVariable(variable);
        }
        
        BoundedIntegerVariable const& Model::addVariable(BoundedIntegerVariable const& variable) {
            return globalVariables.addVariable(variable);
        }
        
        UnboundedIntegerVariable const& Model::addVariable(UnboundedIntegerVariable const& variable) {
            return globalVariables.addVariable(variable);
        }

        RealVariable const& Model::addVariable(RealVariable const& variable) {
            return globalVariables.addVariable(variable);
        }

        VariableSet& Model::getGlobalVariables() {
            return globalVariables;
        }

        VariableSet const& Model::getGlobalVariables() const {
            return globalVariables;
        }
        
        bool Model::hasGlobalVariable(std::string const& name) const {
            return globalVariables.hasVariable(name);
        }
        
        Variable const& Model::getGlobalVariable(std::string const& name) const {
            return globalVariables.getVariable(name);
        }
        
        bool Model::hasNonGlobalTransientVariable() const {
            for (auto const& automaton : automata) {
                if (automaton.hasTransientVariable()) {
                    return true;
                }
            }
            return false;
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
        
        uint64_t Model::getAutomatonIndex(std::string const& name) const {
            auto it = automatonToIndex.find(name);
            STORM_LOG_THROW(it != automatonToIndex.end(), storm::exceptions::InvalidOperationException, "Unable to retrieve unknown automaton '" << name << "'.");
            return it->second;
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
        
        std::map<uint64_t, std::string> Model::buildActionToNameMap() const {
            std::map<uint64_t, std::string> mapping;
            uint64_t i = 0;
            for(auto const& act : actions) {
                mapping[i] = act.getName();
                ++i;
            }
            return mapping;
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
                variable.substitute(constantSubstitution);
            }
            
            // Substitute constants in initial states expression.
            result.setInitialStatesRestriction(this->getInitialStatesRestriction().substitute(constantSubstitution));
            
            // Substitute constants in variables of automata and their edges.
            for (auto& automaton : result.getAutomata()) {
                automaton.substitute(constantSubstitution);
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
        
        void Model::setInitialStatesRestriction(storm::expressions::Expression const& initialStatesRestriction) {
            this->initialStatesRestriction = initialStatesRestriction;
        }
        
        storm::expressions::Expression const& Model::getInitialStatesRestriction() const {
            return initialStatesRestriction;
        }
        
        storm::expressions::Expression Model::getInitialStatesExpression(bool includeAutomataInitialStatesExpressions) const {
            // Start with the restriction of variables.
            storm::expressions::Expression result = initialStatesRestriction;
            
            // Then add initial values for those variables that have one.
            for (auto const& variable : globalVariables) {
                if (variable.hasInitExpression()) {
                    result = result && (variable.isBooleanVariable() ? storm::expressions::iff(variable.getExpressionVariable(), variable.getInitExpression()) : variable.getExpressionVariable() == variable.getInitExpression());
                }
            }
            
            // If we are to include the expressions for the automata, do so now.
            if (includeAutomataInitialStatesExpressions) {
                for (auto const& automaton : automata) {
                    if (!automaton.getVariables().empty()) {
                        storm::expressions::Expression automatonInitialStatesExpression = automaton.getInitialStatesExpression();
                        if (automatonInitialStatesExpression.isInitialized() && !automatonInitialStatesExpression.isTrue()) {
                            result = result && automatonInitialStatesExpression;
                        }
                    }
                }
            }
            return result;
        }
        
        bool Model::isDeterministicModel() const {
            return this->getModelType() == ModelType::DTMC || this->getModelType() == ModelType::CTMC;
        }
        
        bool Model::isDiscreteTimeModel() const {
            return this->getModelType() == ModelType::DTMC || this->getModelType() == ModelType::MDP;
        }
        
        std::vector<storm::expressions::Expression> Model::getAllRangeExpressions() const {
            std::vector<storm::expressions::Expression> result;
            for (auto const& variable : this->getGlobalVariables().getBoundedIntegerVariables()) {
                result.push_back(variable.getRangeExpression());
            }
            
            for (auto const& automaton : automata) {
                std::vector<storm::expressions::Expression> automatonRangeExpressions = automaton.getAllRangeExpressions();
                result.insert(result.end(), automatonRangeExpressions.begin(), automatonRangeExpressions.end());
            }
            return result;
        }
        
        void Model::finalize() {
            for (auto& automaton : getAutomata()) {
                automaton.finalize(*this);
            }
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
        
        bool Model::hasDefaultComposition() const {
            CompositionInformationVisitor visitor;
            CompositionInformation info = visitor.getInformation(this->getSystemComposition(), *this);
            if (info.containsNonStandardParallelComposition() || info.containsRenameComposition()) {
                return false;
            }
            for (auto const& multiplicity : info.getAutomatonToMultiplicityMap()) {
                if (multiplicity.second > 1) {
                    return false;
                }
            }
            return true;
        }
        
    }
}