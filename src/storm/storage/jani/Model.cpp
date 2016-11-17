#include "src/storm/storage/jani/Model.h"

#include "src/storm/storage/expressions/ExpressionManager.h"

#include "src/storm/storage/jani/Compositions.h"
#include "src/storm/storage/jani/CompositionInformationVisitor.h"

#include "src/storm/utility/macros.h"
#include "src/storm/exceptions/WrongFormatException.h"
#include "src/storm/exceptions/InvalidArgumentException.h"
#include "src/storm/exceptions/InvalidOperationException.h"
#include "src/storm/exceptions/InvalidTypeException.h"

namespace storm {
    namespace jani {
        
        const std::string Model::SILENT_ACTION_NAME = "";
        const uint64_t Model::SILENT_ACTION_INDEX = 0;
        
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
            uint64_t actionIndex = addAction(storm::jani::Action(SILENT_ACTION_NAME));
            STORM_LOG_ASSERT(actionIndex == SILENT_ACTION_INDEX, "Illegal silent action index.");
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
            // If there's just one automaton, we must not use the parallel composition operator.
            if (this->getNumberOfAutomata() == 1) {
                return std::make_shared<AutomatonComposition>(this->getAutomata().front().getName());
            }
            
            // Determine the action indices used by each of the automata and create the standard subcompositions.
            std::set<uint64_t> allActionIndices;
            std::vector<std::set<uint64_t>> automatonActionIndices;
            std::vector<std::shared_ptr<Composition>> subcompositions;
            for (auto const& automaton : automata) {
                automatonActionIndices.push_back(automaton.getActionIndices());
                automatonActionIndices.back().erase(SILENT_ACTION_INDEX);
                allActionIndices.insert(automatonActionIndices.back().begin(), automatonActionIndices.back().end());
                subcompositions.push_back(std::make_shared<AutomatonComposition>(automaton.getName()));
            }
            
            // Create the standard synchronization vectors: every automaton with that action participates in the
            // synchronization.
            std::vector<storm::jani::SynchronizationVector> synchVectors;
            for (auto actionIndex : allActionIndices) {
                std::string const& actionName = this->getAction(actionIndex).getName();
                std::vector<std::string> synchVectorInputs;
                uint64_t numberOfParticipatingAutomata = 0;
                int i = 0;
                for (auto const& actionIndices : automatonActionIndices) {
                    if (actionIndices.find(actionIndex) != actionIndices.end()) {
                        ++numberOfParticipatingAutomata;
                        synchVectorInputs.push_back(actionName);
                    } else {
                        synchVectorInputs.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
                    }
                    ++i;
                }
                
                // Only add the synchronization vector if there is more than one participating automaton.
                if (numberOfParticipatingAutomata > 1) {
                    synchVectors.push_back(storm::jani::SynchronizationVector(synchVectorInputs, actionName));
                }
            }
            
            return std::make_shared<ParallelComposition>(subcompositions, synchVectors);
        }
        
        Composition const& Model::getSystemComposition() const {
            return *composition;
        }
        
        void Model::setSystemComposition(std::shared_ptr<Composition> const& composition) {
            this->composition = composition;
        }
        
        void Model::setStandardSystemComposition() {
            setSystemComposition(getStandardSystemComposition());
        }
        
        std::set<std::string> Model::getActionNames(bool includeSilent) const {
            std::set<std::string> result;
            for (auto const& entry : actionToIndex) {
                if (includeSilent || entry.second != SILENT_ACTION_INDEX) {
                    result.insert(entry.first);
                }
            }
            return result;
        }

        std::map<uint64_t, std::string> Model::getActionIndexToNameMap() const {
            std::map<uint64_t, std::string> mapping;
            uint64_t i = 0;
            for(auto const& act : actions) {
                mapping[i] = act.getName();
                ++i;
            }
            return mapping;
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
        
        storm::expressions::Expression Model::getInitialStatesExpression(std::vector<std::reference_wrapper<storm::jani::Automaton const>> const& automata) const {
            // Start with the restriction of variables.
            storm::expressions::Expression result = initialStatesRestriction;
            
            // Then add initial values for those non-transient variables that have one.
            for (auto const& variable : globalVariables) {
                if (variable.isTransient()) {
                    continue;
                }
                
                if (variable.hasInitExpression()) {
                    result = result && (variable.isBooleanVariable() ? storm::expressions::iff(variable.getExpressionVariable(), variable.getInitExpression()) : variable.getExpressionVariable() == variable.getInitExpression());
                }
            }
            
            // If we are to include the expressions for the automata, do so now.
            for (auto const& automatonReference : automata) {
                storm::jani::Automaton const& automaton = automatonReference.get();
                if (!automaton.getVariables().empty()) {
                    storm::expressions::Expression automatonInitialStatesExpression = automaton.getInitialStatesExpression();
                    if (automatonInitialStatesExpression.isInitialized() && !automatonInitialStatesExpression.isTrue()) {
                        result = result && automatonInitialStatesExpression;
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
        
        std::vector<storm::expressions::Expression> Model::getAllRangeExpressions(std::vector<std::reference_wrapper<storm::jani::Automaton const>> const& automata) const {
            std::vector<storm::expressions::Expression> result;
            for (auto const& variable : this->getGlobalVariables().getBoundedIntegerVariables()) {
                result.push_back(variable.getRangeExpression());
            }
            
            if (automata.empty()) {
                for (auto const& automaton : this->getAutomata()) {
                    std::vector<storm::expressions::Expression> automatonRangeExpressions = automaton.getAllRangeExpressions();
                    result.insert(result.end(), automatonRangeExpressions.begin(), automatonRangeExpressions.end());
                }
            } else {
                for (auto const& automaton : automata) {
                    std::vector<storm::expressions::Expression> automatonRangeExpressions = automaton.get().getAllRangeExpressions();
                    result.insert(result.end(), automatonRangeExpressions.begin(), automatonRangeExpressions.end());
                }
            }
            return result;
        }
        
        void Model::finalize() {
            for (auto& automaton : getAutomata()) {
                automaton.finalize(*this);
            }
        }
        
        void Model::checkValid() const {
            // TODO switch to exception based return value.
            STORM_LOG_ASSERT(getModelType() != storm::jani::ModelType::UNDEFINED, "Model type not set");
            STORM_LOG_ASSERT(!automata.empty(), "No automata set");
            STORM_LOG_ASSERT(composition != nullptr, "Composition is not set");
        }
        
        storm::expressions::Expression Model::getLabelExpression(BooleanVariable const& transientVariable, std::map<std::string, storm::expressions::Variable> const& automatonToLocationVariableMap) const {
            STORM_LOG_THROW(transientVariable.isTransient(), storm::exceptions::InvalidArgumentException, "Expected transient variable.");
            
            storm::expressions::Expression result;
            bool negate = transientVariable.getInitExpression().isTrue();
            
            for (auto const& automaton : this->getAutomata()) {
                storm::expressions::Variable const& locationVariable = automatonToLocationVariableMap.at(automaton.getName());
                for (auto const& location : automaton.getLocations()) {
                    for (auto const& assignment : location.getAssignments().getTransientAssignments()) {
                        if (assignment.getExpressionVariable() == transientVariable.getExpressionVariable()) {
                            auto newExpression = (locationVariable == this->getManager().integer(automaton.getLocationIndex(location.getName()))) && (negate ? !assignment.getAssignedExpression() : assignment.getAssignedExpression());
                            if (result.isInitialized()) {
                                result = result || newExpression;
                            } else {
                                result = newExpression;
                            }
                        }
                    }
                }
            }
            
            if (result.isInitialized()) {
                if (negate) {
                    result = !result;
                }
            } else {
                result = this->getManager().boolean(negate);
            }
            
            return result;
        }
        
        bool Model::hasStandardComposition() const {
            CompositionInformationVisitor visitor(*this, this->getSystemComposition());
            CompositionInformation info = visitor.getInformation();
            if (info.containsNonStandardParallelComposition()) {
                return false;
            }
            for (auto const& multiplicity : info.getAutomatonToMultiplicityMap()) {
                if (multiplicity.second > 1) {
                    return false;
                }
            }
            return true;
        }
        
        bool Model::hasStandardCompliantComposition() const {
            CompositionInformationVisitor visitor(*this, this->getSystemComposition());
            CompositionInformation info = visitor.getInformation();
            if (info.containsNestedParallelComposition()) {
                return false;
            }
            return true;
        }
        
        bool Model::undefinedConstantsAreGraphPreserving() const {
            if (!this->hasUndefinedConstants()) {
                return true;
            }

            // Gather the variables of all undefined constants.
            std::set<storm::expressions::Variable> undefinedConstantVariables;
            for (auto const& constant : this->getConstants()) {
                if (!constant.isDefined()) {
                    undefinedConstantVariables.insert(constant.getExpressionVariable());
                }
            }
            
            // Start by checking the defining expressions of all defined constants. If it contains a currently undefined
            // constant, we need to mark the target constant as undefined as well.
            for (auto const& constant : this->getConstants()) {
                if (constant.isDefined()) {
                    if (constant.getExpression().containsVariable(undefinedConstantVariables)) {
                        undefinedConstantVariables.insert(constant.getExpressionVariable());
                    }
                }
            }

            // Check global variable definitions.
            if (this->getGlobalVariables().containsVariablesInBoundExpressionsOrInitialValues(undefinedConstantVariables)) {
                return false;
            }
            
            // Check the automata.
            for (auto const& automaton : this->getAutomata()) {
                if (!automaton.containsVariablesOnlyInProbabilitiesOrTransientAssignments(undefinedConstantVariables)) {
                    return false;
                }
            }
            
            // Check initial states restriction.
            if (initialStatesRestriction.containsVariable(undefinedConstantVariables)) {
                return false;
            }
            return true;
        }
     
        void Model::makeStandardJaniCompliant() {
            for (auto& automaton : automata) {
                automaton.pushEdgeAssignmentsToDestinations();
            }
        }
        
        void Model::liftTransientEdgeDestinationAssignments() {
            for (auto& automaton : this->getAutomata()) {
                automaton.liftTransientEdgeDestinationAssignments();
            }
        }
        
        bool Model::hasTransientEdgeDestinationAssignments() const {
            for (auto const& automaton : this->getAutomata()) {
                if (automaton.hasTransientEdgeDestinationAssignments()) {
                    return true;
                }
            }
            return false;
        }
        
        bool Model::usesAssignmentLevels() const {
            for (auto const& automaton : this->getAutomata()) {
                if (automaton.usesAssignmentLevels()) {
                    return true;
                }
            }
            return false;
        }
    }
}
