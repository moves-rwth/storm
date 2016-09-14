#include "src/builder/DdJaniModelBuilder.h"

#include <sstream>

#include <boost/algorithm/string/join.hpp>

#include "src/logic/Formulas.h"

#include "src/storage/jani/Model.h"
#include "src/storage/jani/RenameComposition.h"
#include "src/storage/jani/AutomatonComposition.h"
#include "src/storage/jani/ParallelComposition.h"
#include "src/storage/jani/CompositionActionInformationVisitor.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"
#include "src/adapters/AddExpressionAdapter.h"

#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Ctmc.h"
#include "src/models/symbolic/Mdp.h"
#include "src/models/symbolic/StandardRewardModel.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/CoreSettings.h"

#include "src/utility/macros.h"
#include "src/utility/jani.h"
#include "src/utility/dd.h"
#include "src/utility/math.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace builder {
        
        template <storm::dd::DdType Type, typename ValueType>
        DdJaniModelBuilder<Type, ValueType>::Options::Options() : buildAllRewardModels(true), rewardModelsToBuild(), constantDefinitions(), terminalStates(), negatedTerminalStates() {
            // Intentionally left empty.
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        DdJaniModelBuilder<Type, ValueType>::Options::Options(storm::logic::Formula const& formula) : buildAllRewardModels(false), rewardModelsToBuild(), constantDefinitions(), terminalStates(), negatedTerminalStates() {
            this->preserveFormula(formula);
            this->setTerminalStatesFromFormula(formula);
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        DdJaniModelBuilder<Type, ValueType>::Options::Options(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) : buildAllRewardModels(false), rewardModelsToBuild(), constantDefinitions(), terminalStates(), negatedTerminalStates() {
            if (formulas.empty()) {
                this->buildAllRewardModels = true;
            } else {
                for (auto const& formula : formulas) {
                    this->preserveFormula(*formula);
                }
                if (formulas.size() == 1) {
                    this->setTerminalStatesFromFormula(*formulas.front());
                }
            }
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        void DdJaniModelBuilder<Type, ValueType>::Options::preserveFormula(storm::logic::Formula const& formula) {
            // If we already had terminal states, we need to erase them.
            if (terminalStates) {
                terminalStates.reset();
            }
            if (negatedTerminalStates) {
                negatedTerminalStates.reset();
            }
            
            // If we are not required to build all reward models, we determine the reward models we need to build.
            if (!buildAllRewardModels) {
                std::set<std::string> referencedRewardModels = formula.getReferencedRewardModels();
                rewardModelsToBuild.insert(referencedRewardModels.begin(), referencedRewardModels.end());
            }
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        void DdJaniModelBuilder<Type, ValueType>::Options::setTerminalStatesFromFormula(storm::logic::Formula const& formula) {
            if (formula.isAtomicExpressionFormula()) {
                terminalStates = formula.asAtomicExpressionFormula().getExpression();
            } else if (formula.isEventuallyFormula()) {
                storm::logic::Formula const& sub = formula.asEventuallyFormula().getSubformula();
                if (sub.isAtomicExpressionFormula() || sub.isAtomicLabelFormula()) {
                    this->setTerminalStatesFromFormula(sub);
                }
            } else if (formula.isUntilFormula()) {
                storm::logic::Formula const& right = formula.asUntilFormula().getRightSubformula();
                if (right.isAtomicExpressionFormula() || right.isAtomicLabelFormula()) {
                    this->setTerminalStatesFromFormula(right);
                }
                storm::logic::Formula const& left = formula.asUntilFormula().getLeftSubformula();
                if (left.isAtomicExpressionFormula()) {
                    negatedTerminalStates = left.asAtomicExpressionFormula().getExpression();
                }
            } else if (formula.isProbabilityOperatorFormula()) {
                storm::logic::Formula const& sub = formula.asProbabilityOperatorFormula().getSubformula();
                if (sub.isEventuallyFormula() || sub.isUntilFormula()) {
                    this->setTerminalStatesFromFormula(sub);
                }
            }
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        std::set<std::string> const& DdJaniModelBuilder<Type, ValueType>::Options::getRewardModelNames() const {
            return rewardModelsToBuild;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        bool DdJaniModelBuilder<Type, ValueType>::Options::isBuildAllRewardModelsSet() const {
            return buildAllRewardModels;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        struct CompositionVariables {
            CompositionVariables() : manager(std::make_shared<storm::dd::DdManager<Type>>()),
            variableToRowMetaVariableMap(std::make_shared<std::map<storm::expressions::Variable, storm::expressions::Variable>>()),
            rowExpressionAdapter(std::make_shared<storm::adapters::AddExpressionAdapter<Type>>(manager, variableToRowMetaVariableMap)),
            variableToColumnMetaVariableMap(std::make_shared<std::map<storm::expressions::Variable, storm::expressions::Variable>>()),
            columnExpressionAdapter(std::make_shared<storm::adapters::AddExpressionAdapter<Type>>(manager, variableToColumnMetaVariableMap)) {
                // Intentionally left empty.
            }
            
            std::shared_ptr<storm::dd::DdManager<Type>> manager;
            
            // The meta variables for the row encoding.
            std::set<storm::expressions::Variable> rowMetaVariables;
            std::shared_ptr<std::map<storm::expressions::Variable, storm::expressions::Variable>> variableToRowMetaVariableMap;
            std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> rowExpressionAdapter;
            
            // The meta variables for the column encoding.
            std::set<storm::expressions::Variable> columnMetaVariables;
            std::shared_ptr<std::map<storm::expressions::Variable, storm::expressions::Variable>> variableToColumnMetaVariableMap;
            std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> columnExpressionAdapter;
            
            // All pairs of row/column meta variables.
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> rowColumnMetaVariablePairs;
            
            // A mapping from automata to the meta variable encoding their location.
            std::map<std::string, std::pair<storm::expressions::Variable, storm::expressions::Variable>> automatonToLocationVariableMap;
            
            // A mapping from action indices to the meta variables used to encode these actions.
            std::map<uint64_t, storm::expressions::Variable> actionVariablesMap;
            
            // The meta variables used to encode the remaining nondeterminism.
            std::vector<storm::expressions::Variable> localNondeterminismVariables;
            
            // The meta variables used to encode the actions and nondeterminism.
            std::set<storm::expressions::Variable> allNondeterminismVariables;
            
            // DDs representing the identity for each variable.
            std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> variableToIdentityMap;
            
            // DDs representing the ranges of each variable.
            std::map<storm::expressions::Variable, storm::dd::Bdd<Type>> variableToRangeMap;
            
            // A set of all meta variables that correspond to global variables.
            std::set<storm::expressions::Variable> allGlobalVariables;
            
            // DDs representing the identity for each automaton.
            std::map<std::string, storm::dd::Add<Type, ValueType>> automatonToIdentityMap;
            
            // DDs representing the valid ranges of the variables of each automaton.
            std::map<std::string, storm::dd::Add<Type, ValueType>> automatonToRangeMap;
            
            // A DD representing the valid ranges of the global variables.
            storm::dd::Add<Type, ValueType> globalVariableRanges;
        };
        
        // A class responsible for creating the necessary variables for a subsequent composition of automata.
        template <storm::dd::DdType Type, typename ValueType>
        class CompositionVariableCreator : public storm::jani::CompositionVisitor {
        public:
            CompositionVariableCreator(storm::jani::Model const& model, storm::jani::ActionInformation const& actionInformation) : model(model), automata(), actionInformation(actionInformation) {
                // Intentionally left empty.
            }
            
            CompositionVariables<Type, ValueType> create() {
                // First, check whether every automaton appears exactly once in the system composition. Simultaneously,
                // we determine the set of non-silent actions used by the composition.
                automata.clear();
                this->model.getSystemComposition().accept(*this, boost::none);
                STORM_LOG_THROW(automata.size() == this->model.getNumberOfAutomata(), storm::exceptions::InvalidArgumentException, "Cannot build symbolic model from JANI model whose system composition refers to a subset of automata.");
                
                // Then, check that the model does not contain unbounded integer or non-transient real variables.
                STORM_LOG_THROW(!this->model.getGlobalVariables().containsUnboundedIntegerVariables(), storm::exceptions::InvalidArgumentException, "Cannot build symbolic model from JANI model that contains global unbounded integer variables.");
                for (auto const& automaton : this->model.getAutomata()) {
                    STORM_LOG_THROW(!automaton.getVariables().containsUnboundedIntegerVariables(), storm::exceptions::InvalidArgumentException, "Cannot build symbolic model from JANI model that contains unbounded integer variables in automaton '" << automaton.getName() << "'.");
                }
                STORM_LOG_THROW(!this->model.getGlobalVariables().containsNonTransientRealVariables(), storm::exceptions::InvalidArgumentException, "Cannot build symbolic model from JANI model that contains global non-transient real variables.");
                for (auto const& automaton : this->model.getAutomata()) {
                    STORM_LOG_THROW(!automaton.getVariables().containsNonTransientRealVariables(), storm::exceptions::InvalidArgumentException, "Cannot build symbolic model from JANI model that contains non-transient real variables in automaton '" << automaton.getName() << "'.");
                }
                
                // Based on this assumption, we create the variables.
                return createVariables();
            }
            
            boost::any visit(storm::jani::AutomatonComposition const& composition, boost::any const& data) override {
                auto it = automata.find(composition.getAutomatonName());
                STORM_LOG_THROW(it == automata.end(), storm::exceptions::InvalidArgumentException, "Cannot build symbolic model from JANI model whose system composition that refers to the automaton '" << composition.getAutomatonName() << "' multiple times.");
                automata.insert(it, composition.getAutomatonName());
                return boost::none;
            }
            
            boost::any visit(storm::jani::RenameComposition const& composition, boost::any const& data) override {
                boost::any_cast<std::set<std::string>>(composition.getSubcomposition().accept(*this, boost::none));
                return boost::none;
            }
            
            boost::any visit(storm::jani::ParallelComposition const& composition, boost::any const& data) override {
                for (auto const& subcomposition : composition.getSubcompositions()) {
                    subcomposition->accept(*this, boost::none);
                }
                return boost::none;
            }
            
        private:
            CompositionVariables<Type, ValueType> createVariables() {
                CompositionVariables<Type, ValueType> result;
                
                for (auto const& nonSilentActionIndex : actionInformation.getNonSilentActionIndices()) {
                    std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = result.manager->addMetaVariable(actionInformation.getActionName(nonSilentActionIndex));
                    result.actionVariablesMap[nonSilentActionIndex] = variablePair.first;
                    result.allNondeterminismVariables.insert(variablePair.first);
                }
                    
                // FIXME: check how many nondeterminism variables we should actually allocate.
                uint64_t numberOfNondeterminismVariables = this->model.getNumberOfAutomata();
                for (auto const& automaton : this->model.getAutomata()) {
                    numberOfNondeterminismVariables += automaton.getNumberOfEdges();
                }
                for (uint_fast64_t i = 0; i < numberOfNondeterminismVariables; ++i) {
                    std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = result.manager->addMetaVariable("nondet" + std::to_string(i));
                    result.localNondeterminismVariables.push_back(variablePair.first);
                    result.allNondeterminismVariables.insert(variablePair.first);
                }
                
                for (auto const& automaton : this->model.getAutomata()) {
                    // Start by creating a meta variable for the location of the automaton.
                    std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = result.manager->addMetaVariable("l_" + automaton.getName(), 0, automaton.getNumberOfLocations() - 1);
                    result.automatonToLocationVariableMap[automaton.getName()] = variablePair;
                    result.rowColumnMetaVariablePairs.push_back(variablePair);
                    
                    // Add the location variable to the row/column variables.
                    result.rowMetaVariables.insert(variablePair.first);
                    result.columnMetaVariables.insert(variablePair.second);
                    
                    // Add the legal range for the location variables.
                    result.variableToRangeMap.emplace(variablePair.first, result.manager->getRange(variablePair.first));
                    result.variableToRangeMap.emplace(variablePair.second, result.manager->getRange(variablePair.second));
                }
                
                // Create global variables.
                storm::dd::Bdd<Type> globalVariableRanges = result.manager->getBddOne();
                for (auto const& variable : this->model.getGlobalVariables()) {
                    // Only create the variable if it's non-transient.
                    if (variable.isTransient()) {
                        continue;
                    }
                    
                    createVariable(variable, result);
                    globalVariableRanges &= result.manager->getRange(result.variableToRowMetaVariableMap->at(variable.getExpressionVariable()));
                }
                result.globalVariableRanges = globalVariableRanges.template toAdd<ValueType>();
                
                // Create the variables for the individual automata.
                for (auto const& automaton : this->model.getAutomata()) {
                    storm::dd::Bdd<Type> identity = result.manager->getBddOne();
                    storm::dd::Bdd<Type> range = result.manager->getBddOne();
                    
                    // Add the identity and ranges of the location variables to the ones of the automaton.
                    std::pair<storm::expressions::Variable, storm::expressions::Variable> const& locationVariables = result.automatonToLocationVariableMap[automaton.getName()];
                    storm::dd::Add<Type, ValueType> variableIdentity = result.manager->template getIdentity<ValueType>(locationVariables.first).equals(result.manager->template getIdentity<ValueType>(locationVariables.second)).template toAdd<ValueType>() * result.manager->getRange(locationVariables.first).template toAdd<ValueType>() * result.manager->getRange(locationVariables.second).template toAdd<ValueType>();
                    identity &= variableIdentity.toBdd();
                    range &= result.manager->getRange(locationVariables.first);
                    
                    // Then create variables for the variables of the automaton.
                    for (auto const& variable : automaton.getVariables()) {
                        // Only create the variable if it's non-transient.
                        if (variable.isTransient()) {
                            continue;
                        }
                        
                        createVariable(variable, result);
                        identity &= result.variableToIdentityMap.at(variable.getExpressionVariable()).toBdd();
                        range &= result.manager->getRange(result.variableToRowMetaVariableMap->at(variable.getExpressionVariable()));
                    }
                    
                    result.automatonToIdentityMap[automaton.getName()] = identity.template toAdd<ValueType>();
                    result.automatonToRangeMap[automaton.getName()] = (range && globalVariableRanges).template toAdd<ValueType>();
                }
                
                return result;
            }
            
            void createVariable(storm::jani::Variable const& variable, CompositionVariables<Type, ValueType>& result) {
                if (variable.isBooleanVariable()) {
                    createVariable(variable.asBooleanVariable(), result);
                } else if (variable.isBoundedIntegerVariable()) {
                    createVariable(variable.asBoundedIntegerVariable(), result);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid type of variable in JANI model.");
                }
            }
            
            void createVariable(storm::jani::BoundedIntegerVariable const& variable, CompositionVariables<Type, ValueType>& result) {
                int_fast64_t low = variable.getLowerBound().evaluateAsInt();
                int_fast64_t high = variable.getUpperBound().evaluateAsInt();
                std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = result.manager->addMetaVariable(variable.getName(), low, high);
                
                STORM_LOG_TRACE("Created meta variables for global integer variable: " << variablePair.first.getName() << "] and " << variablePair.second.getName() << ".");
                
                result.rowMetaVariables.insert(variablePair.first);
                result.variableToRowMetaVariableMap->emplace(variable.getExpressionVariable(), variablePair.first);
                
                result.columnMetaVariables.insert(variablePair.second);
                result.variableToColumnMetaVariableMap->emplace(variable.getExpressionVariable(), variablePair.second);
                
                storm::dd::Add<Type, ValueType> variableIdentity = result.manager->template getIdentity<ValueType>(variablePair.first).equals(result.manager->template getIdentity<ValueType>(variablePair.second)).template toAdd<ValueType>() * result.manager->getRange(variablePair.first).template toAdd<ValueType>() * result.manager->getRange(variablePair.second).template toAdd<ValueType>();
                result.variableToIdentityMap.emplace(variable.getExpressionVariable(), variableIdentity);
                result.rowColumnMetaVariablePairs.push_back(variablePair);
                result.variableToRangeMap.emplace(variablePair.first, result.manager->getRange(variablePair.first));
                result.variableToRangeMap.emplace(variablePair.second, result.manager->getRange(variablePair.second));
                
                result.allGlobalVariables.insert(variable.getExpressionVariable());
            }
            
            void createVariable(storm::jani::BooleanVariable const& variable, CompositionVariables<Type, ValueType>& result) {
                std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = result.manager->addMetaVariable(variable.getName());
                
                STORM_LOG_TRACE("Created meta variables for global boolean variable: " << variablePair.first.getName() << " and " << variablePair.second.getName() << ".");
                
                result.rowMetaVariables.insert(variablePair.first);
                result.variableToRowMetaVariableMap->emplace(variable.getExpressionVariable(), variablePair.first);
                
                result.columnMetaVariables.insert(variablePair.second);
                result.variableToColumnMetaVariableMap->emplace(variable.getExpressionVariable(), variablePair.second);
                
                storm::dd::Add<Type, ValueType> variableIdentity = result.manager->template getIdentity<ValueType>(variablePair.first).equals(result.manager->template getIdentity<ValueType>(variablePair.second)).template toAdd<ValueType>();
                result.variableToIdentityMap.emplace(variable.getExpressionVariable(), variableIdentity);
                
                result.variableToRangeMap.emplace(variablePair.first, result.manager->getRange(variablePair.first));
                result.variableToRangeMap.emplace(variablePair.second, result.manager->getRange(variablePair.second));
                
                result.rowColumnMetaVariablePairs.push_back(variablePair);
                result.allGlobalVariables.insert(variable.getExpressionVariable());
            }
            
            storm::jani::Model const& model;
            std::set<std::string> automata;
            storm::jani::ActionInformation actionInformation;
        };
        
        template <storm::dd::DdType Type, typename ValueType>
        struct ComposerResult {
            ComposerResult(storm::dd::Add<Type, ValueType> const& transitions, std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> const& transientLocationAssignments, std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> const& transientEdgeAssignments, storm::dd::Bdd<Type> const& illegalFragment, uint64_t numberOfNondeterminismVariables = 0) : transitions(transitions), transientLocationAssignments(transientLocationAssignments), transientEdgeAssignments(transientEdgeAssignments), illegalFragment(illegalFragment), numberOfNondeterminismVariables(numberOfNondeterminismVariables) {
                // Intentionally left empty.
            }
            
            storm::dd::Add<Type, ValueType> transitions;
            std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> transientLocationAssignments;
            std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> transientEdgeAssignments;
            storm::dd::Bdd<Type> illegalFragment;
            uint64_t numberOfNondeterminismVariables;
        };
        
        // A class that is responsible for performing the actual composition. This
        template <storm::dd::DdType Type, typename ValueType>
        class SystemComposer : public storm::jani::CompositionVisitor {
        public:
            SystemComposer(storm::jani::Model const& model, CompositionVariables<Type, ValueType> const& variables, std::vector<storm::expressions::Variable> const& transientVariables) : model(model), variables(variables), transientVariables(transientVariables) {
                // Intentionally left empty.
            }
            
            virtual ComposerResult<Type, ValueType> compose() = 0;

        protected:
            // The model that is referred to by the composition.
            storm::jani::Model const& model;
            
            // The variable to use when building an automaton.
            CompositionVariables<Type, ValueType> const& variables;
            
            // The transient variables to consider during system composition.
            std::vector<storm::expressions::Variable> transientVariables;
        };

        // This structure represents an edge destination.
        template <storm::dd::DdType Type, typename ValueType>
        struct EdgeDestinationDd {
            EdgeDestinationDd(storm::dd::Add<Type, ValueType> const& transitions, std::set<storm::expressions::Variable> const& writtenGlobalVariables = {}) : transitions(transitions), writtenGlobalVariables(writtenGlobalVariables) {
                // Intentionally left empty.
            }
            
            storm::dd::Add<Type, ValueType> transitions;
            std::set<storm::expressions::Variable> writtenGlobalVariables;
        };
        
        template <storm::dd::DdType Type, typename ValueType>
        EdgeDestinationDd<Type, ValueType> buildEdgeDestinationDd(storm::jani::Automaton const& automaton, storm::jani::EdgeDestination const& destination, storm::dd::Add<Type, ValueType> const& guard, CompositionVariables<Type, ValueType> const& variables) {
            storm::dd::Add<Type, ValueType> transitions = variables.rowExpressionAdapter->translateExpression(destination.getProbability());
            
            STORM_LOG_TRACE("Translating edge destination.");
            
            // Iterate over all assignments (boolean and integer) and build the DD for it.
            std::set<storm::expressions::Variable> assignedVariables;
            for (auto const& assignment : destination.getNonTransientAssignments()) {
                // Record the variable as being written.
                STORM_LOG_TRACE("Assigning to variable " << variables.variableToRowMetaVariableMap->at(assignment.getExpressionVariable()).getName());
                assignedVariables.insert(assignment.getExpressionVariable());
                
                // Translate the written variable.
                auto const& primedMetaVariable = variables.variableToColumnMetaVariableMap->at(assignment.getExpressionVariable());
                storm::dd::Add<Type, ValueType> writtenVariable = variables.manager->template getIdentity<ValueType>(primedMetaVariable);
                
                // Translate the expression that is being assigned.
                storm::dd::Add<Type, ValueType> assignedExpression = variables.rowExpressionAdapter->translateExpression(assignment.getAssignedExpression());
                
                // Combine the assigned expression with the guard.
                storm::dd::Add<Type, ValueType> result = assignedExpression * guard;
                
                // Combine the variable and the assigned expression.
                result = result.equals(writtenVariable).template toAdd<ValueType>();
                result *= guard;

                // Restrict the transitions to the range of the written variable.
                result = result * variables.variableToRangeMap.at(primedMetaVariable).template toAdd<ValueType>();
                
                // Combine the assignment DDs.
                transitions *= result;
            }
            
            // Compute the set of assigned global variables.
            std::set<storm::expressions::Variable> assignedGlobalVariables;
            std::set_intersection(assignedVariables.begin(), assignedVariables.end(), variables.allGlobalVariables.begin(), variables.allGlobalVariables.end(), std::inserter(assignedGlobalVariables, assignedGlobalVariables.begin()));
            
            // All unassigned boolean variables need to keep their value.
            for (storm::jani::BooleanVariable const& variable : automaton.getVariables().getBooleanVariables()) {
                if (assignedVariables.find(variable.getExpressionVariable()) == assignedVariables.end()) {
                    STORM_LOG_TRACE("Multiplying identity of variable " << variable.getName());
                    transitions *= variables.variableToIdentityMap.at(variable.getExpressionVariable());
                }
            }
            
            // All unassigned integer variables need to keep their value.
            for (storm::jani::BoundedIntegerVariable const& variable : automaton.getVariables().getBoundedIntegerVariables()) {
                if (assignedVariables.find(variable.getExpressionVariable()) == assignedVariables.end()) {
                    STORM_LOG_TRACE("Multiplying identity of variable " << variable.getName());
                    transitions *= variables.variableToIdentityMap.at(variable.getExpressionVariable());
                }
            }
            
            transitions *= variables.manager->getEncoding(variables.automatonToLocationVariableMap.at(automaton.getName()).second, destination.getLocationIndex()).template toAdd<ValueType>();
            
            return EdgeDestinationDd<Type, ValueType>(transitions, assignedGlobalVariables);
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        storm::dd::Add<Type, ValueType> encodeAction(boost::optional<uint64_t> const& actionIndex, CompositionVariables<Type, ValueType> const& variables) {
            storm::dd::Add<Type, ValueType> encoding = variables.manager->template getAddOne<ValueType>();
            
            for (auto it = variables.actionVariablesMap.rbegin(), ite = variables.actionVariablesMap.rend(); it != ite; ++it) {
                if (actionIndex && it->first == actionIndex.get()) {
                    encoding *= variables.manager->getEncoding(it->second, 1).template toAdd<ValueType>();
                } else {
                    encoding *= variables.manager->getEncoding(it->second, 0).template toAdd<ValueType>();
                }
            }
            
            return encoding;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        storm::dd::Add<Type, ValueType> encodeIndex(uint64_t index, uint64_t localNondeterminismVariableOffset, uint64_t numberOfLocalNondeterminismVariables, CompositionVariables<Type, ValueType> const& variables) {
            storm::dd::Add<Type, ValueType> result = variables.manager->template getAddZero<ValueType>();
            
            std::map<storm::expressions::Variable, int_fast64_t> metaVariableNameToValueMap;
            for (uint_fast64_t i = 0; i < numberOfLocalNondeterminismVariables; ++i) {
                if (index & (1ull << (numberOfLocalNondeterminismVariables - i - 1))) {
                    metaVariableNameToValueMap.emplace(variables.localNondeterminismVariables[localNondeterminismVariableOffset + i], 1);
                } else {
                    metaVariableNameToValueMap.emplace(variables.localNondeterminismVariables[localNondeterminismVariableOffset + i], 0);
                }
            }
            
            result.setValue(metaVariableNameToValueMap, 1);
            return result;
        }
        
//        template <storm::dd::DdType Type, typename ValueType>
//        class SeparateEdgesSystemComposer : public SystemComposer<Type, ValueType> {
//        public:
//            // This structure represents an edge.
//            struct EdgeDd {
//                EdgeDd(storm::dd::Add<Type> const& guard = storm::dd::Add<Type>(), storm::dd::Add<Type, ValueType> const& transitions = storm::dd::Add<Type, ValueType>(), std::set<storm::expressions::Variable> const& writtenGlobalVariables = {}, std::set<storm::expressions::Variable> const& globalVariablesWrittenMultipleTimes = {}) : guard(guard), transitions(transitions), writtenGlobalVariables(writtenGlobalVariables), globalVariablesWrittenMultipleTimes(globalVariablesWrittenMultipleTimes) {
//                    // Intentionally left empty.
//                }
//                
//                EdgeDd(EdgeDd const& other) : guard(other.guard), transitions(other.transitions), writtenGlobalVariables(other.writtenGlobalVariables), globalVariablesWrittenMultipleTimes(other.globalVariablesWrittenMultipleTimes) {
//                    // Intentionally left empty.
//                }
//                
//                EdgeDd& operator=(EdgeDd const& other) {
//                    if (this != &other) {
//                        globalVariablesWrittenMultipleTimes = other.globalVariablesWrittenMultipleTimes;
//                        writtenGlobalVariables = other.writtenGlobalVariables;
//                        guard = other.guard;
//                        transitions = other.transitions;
//                    }
//                    return *this;
//                }
//                
//                storm::dd::Add<Type, ValueType> guard;
//                storm::dd::Add<Type, ValueType> transitions;
//                std::set<storm::expressions::Variable> writtenGlobalVariables;
//                std::set<storm::expressions::Variable> globalVariablesWrittenMultipleTimes;
//            };
//            
//            // This structure represents a subcomponent of a composition.
//            struct AutomatonDd {
//                AutomatonDd(storm::dd::Add<Type, ValueType> const& identity) : identity(identity) {
//                    // Intentionally left empty.
//                }
//                
//                std::map<uint64_t, std::vector<EdgeDd>> actionIndexToEdges;
//                storm::dd::Add<Type, ValueType> identity;
//            };
//            
//            SeparateEdgesSystemComposer(storm::jani::Model const& model, CompositionVariables<Type, ValueType> const& variables) : SystemComposer<Type, ValueType>(model, variables) {
//                // Intentionally left empty.
//            }
//            
//            ComposerResult<Type, ValueType> compose() override {
//                AutomatonDd globalAutomaton = boost::any_cast<AutomatonDd>(this->model.getSystemComposition().accept(*this, boost::none));
//                return buildSystemFromAutomaton(globalAutomaton);
//            }
//            
//            boost::any visit(storm::jani::AutomatonComposition const& composition, boost::any const& data) override {
//                return buildAutomatonDd(composition.getAutomatonName());
//            }
//            
//            boost::any visit(storm::jani::RenameComposition const& composition, boost::any const& data) override {
//                AutomatonDd subautomaton = boost::any_cast<AutomatonDd>(composition.getSubcomposition().accept(*this, boost::none));
//                
//                // Build a mapping from indices to indices for the renaming.
//                std::map<uint64_t, uint64_t> renamingIndexToIndex;
//                for (auto const& entry : composition.getRenaming()) {
//                    if (this->model.getActionIndex(entry.first) != this->model.getSilentActionIndex()) {
//                        // Distinguish the cases where we hide the action or properly rename it.
//                        if (entry.second) {
//                            renamingIndexToIndex.emplace(this->model.getActionIndex(entry.first), this->model.getActionIndex(entry.second.get()));
//                        } else {
//                            renamingIndexToIndex.emplace(this->model.getActionIndex(entry.first), this->model.getSilentActionIndex());
//                        }
//                    } else {
//                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Renaming composition must not rename the silent action.");
//                    }
//                }
//                
//                // Finally, apply the renaming.
//                AutomatonDd result(subautomaton.identity);
//                for (auto const& actionEdges : subautomaton.actionIndexToEdges) {
//                    auto it = renamingIndexToIndex.find(actionEdges.first);
//                    if (it != renamingIndexToIndex.end()) {
//                        // If we are to rename the action, do so.
//                        result.actionIndexToEdges[it->second].insert(result.actionIndexToEdges[it->second].end(), actionEdges.second.begin(), actionEdges.second.end());
//                    } else {
//                        // Otherwise copy over the edges.
//                        result.actionIndexToEdges[it->first].insert(result.actionIndexToEdges[it->first].begin(), actionEdges.second.begin(), actionEdges.second.end());
//                    }
//                }
//                return result;
//            }
//            
//            boost::any visit(storm::jani::ParallelComposition const& composition, boost::any const& data) override {
//                AutomatonDd leftSubautomaton = boost::any_cast<AutomatonDd>(composition.getLeftSubcomposition().accept(*this, boost::none));
//                AutomatonDd rightSubautomaton = boost::any_cast<AutomatonDd>(composition.getRightSubcomposition().accept(*this, boost::none));
//                
//                // Build the set of synchronizing action indices.
//                std::set<uint64_t> synchronizingActionIndices;
//                for (auto const& entry : composition.getSynchronizationAlphabet()) {
//                    if (this->model.getActionIndex(entry) != this->model.getSilentActionIndex()) {
//                        synchronizingActionIndices.insert(this->model.getActionIndex(entry));
//                    } else {
//                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Parallel composition must not synchronize over silent action.");
//                    }
//                }
//                
//                // Perform the composition.
//                
//                // First, consider all actions in the left subcomposition.
//                AutomatonDd result(leftSubautomaton.identity * rightSubautomaton.identity);
//                uint64_t index = 0;
//                for (auto const& actionEdges : leftSubautomaton.actionIndexToEdges) {
//                    // If we need to synchronize over this action, do so now.
//                    if (synchronizingActionIndices.find(actionEdges.first) != synchronizingActionIndices.end()) {
//                        auto rightIt = rightSubautomaton.actionIndexToEdges.find(actionEdges.first);
//                        if (rightIt != rightSubautomaton.actionIndexToEdges.end()) {
//                            for (auto const& edge1 : actionEdges.second) {
//                                for (auto const& edge2 : rightIt->second) {
//                                    EdgeDd edgeDd = composeEdgesInParallel(edge1, edge2);
//                                    if (!edgeDd.guard.isZero()) {
//                                        result.actionIndexToEdges[actionEdges.first].push_back(edgeDd);
//                                    }
//                                    index++;
//                                }
//                            }
//                        }
//                    } else {
//                        // Extend all edges by the missing identity (unsynchronizing) and copy them over.
//                        for (auto const& edge : actionEdges.second) {
//                            result.actionIndexToEdges[actionEdges.first].push_back(extendEdgeWithIdentity(edge, rightSubautomaton.identity));
//                        }
//                    }
//                }
//                
//                // Then, consider all actions in the right subcomposition. All synchronizing actions can be ignored, because
//                // we would have dealt with them earlier if there was a suitable synchronization partner. Since there is none,
//                // such transitions can not be taken and we can drop them.
//                for (auto const& actionEdges : rightSubautomaton.actionIndexToEdges) {
//                    if (synchronizingActionIndices.find(actionEdges.first) == synchronizingActionIndices.end()) {
//                        for (auto const& edge : actionEdges.second) {
//                            result.actionIndexToEdges[actionEdges.first].push_back(extendEdgeWithIdentity(edge, leftSubautomaton.identity));
//                        }
//                    }
//                }
//                
//                return result;
//            }
//            
//        private:
//            storm::dd::Add<Type, ValueType> combineEdgesBySummation(std::vector<EdgeDd> const& edges, CompositionVariables<Type, ValueType> const& variables) {
//                storm::dd::Add<Type, ValueType> result = variables.manager->template getAddZero<ValueType>();
//                for (auto const& edge : edges) {
//                    // Simply add all actions, but make sure to include the missing global variable identities.
//                    result += edge.transitions * computeMissingGlobalVariableIdentities(edge, variables);
//                }
//                return result;
//            }
//            
//            std::pair<uint64_t, storm::dd::Add<Type, ValueType>> combineEdgesByNondeterministicChoice(std::vector<EdgeDd>& edges) {
//                // Complete all edges by adding the missing global variable identities.
//                for (auto& edge : edges) {
//                    edge.transitions *= computeMissingGlobalVariableIdentities(edge, this->variables);
//                }
//                
//                // Sum all guards, so we can read off the maximal number of nondeterministic choices in any given state.
//                storm::dd::Add<Type, ValueType> sumOfGuards = this->variables.manager->template getAddZero<ValueType>();
//                for (auto const& edge : edges) {
//                    sumOfGuards += edge.guard;
//                }
//                uint_fast64_t maxChoices = static_cast<uint_fast64_t>(sumOfGuards.getMax());
//                STORM_LOG_TRACE("Found " << maxChoices << " local choices.");
//                
//                // Depending on the maximal number of nondeterminstic choices, we need to use some variables to encode the nondeterminism.
//                if (maxChoices == 0) {
//                    return std::make_pair(0, this->variables.manager->template getAddZero<ValueType>());
//                } else if (maxChoices == 1) {
//                    return std::make_pair(0, combineEdgesBySummation(edges, this->variables));
//                } else {
//                    // Calculate number of required variables to encode the nondeterminism.
//                    uint_fast64_t numberOfBinaryVariables = static_cast<uint_fast64_t>(std::ceil(storm::utility::math::log2(maxChoices)));
//                    
//                    storm::dd::Add<Type, ValueType> allEdges = this->variables.manager->template getAddZero<ValueType>();
//                    
//                    storm::dd::Bdd<Type> equalsNumberOfChoicesDd;
//                    std::vector<storm::dd::Add<Type, ValueType>> choiceDds(maxChoices, this->variables.manager->template getAddZero<ValueType>());
//                    std::vector<storm::dd::Bdd<Type>> remainingDds(maxChoices, this->variables.manager->getBddZero());
//                    
//                    for (uint_fast64_t currentChoices = 1; currentChoices <= maxChoices; ++currentChoices) {
//                        // Determine the set of states with exactly currentChoices choices.
//                        equalsNumberOfChoicesDd = sumOfGuards.equals(this->variables.manager->getConstant(static_cast<double>(currentChoices)));
//                        
//                        // If there is no such state, continue with the next possible number of choices.
//                        if (equalsNumberOfChoicesDd.isZero()) {
//                            continue;
//                        }
//                        
//                        // Reset the previously used intermediate storage.
//                        for (uint_fast64_t j = 0; j < currentChoices; ++j) {
//                            choiceDds[j] = this->variables.manager->template getAddZero<ValueType>();
//                            remainingDds[j] = equalsNumberOfChoicesDd;
//                        }
//                        
//                        for (std::size_t j = 0; j < edges.size(); ++j) {
//                            // Check if edge guard overlaps with equalsNumberOfChoicesDd. That is, there are states with exactly currentChoices
//                            // choices such that one outgoing choice is given by the j-th edge.
//                            storm::dd::Bdd<Type> guardChoicesIntersection = edges[j].guard.toBdd() && equalsNumberOfChoicesDd;
//                            
//                            // If there is no such state, continue with the next command.
//                            if (guardChoicesIntersection.isZero()) {
//                                continue;
//                            }
//                            
//                            // Split the currentChoices nondeterministic choices.
//                            for (uint_fast64_t k = 0; k < currentChoices; ++k) {
//                                // Calculate the overlapping part of command guard and the remaining DD.
//                                storm::dd::Bdd<Type> remainingGuardChoicesIntersection = guardChoicesIntersection && remainingDds[k];
//                                
//                                // Check if we can add some overlapping parts to the current index.
//                                if (!remainingGuardChoicesIntersection.isZero()) {
//                                    // Remove overlapping parts from the remaining DD.
//                                    remainingDds[k] = remainingDds[k] && !remainingGuardChoicesIntersection;
//                                    
//                                    // Combine the overlapping part of the guard with command updates and add it to the resulting DD.
//                                    choiceDds[k] += remainingGuardChoicesIntersection.template toAdd<ValueType>() * edges[j].transitions;
//                                }
//                                
//                                // Remove overlapping parts from the command guard DD
//                                guardChoicesIntersection = guardChoicesIntersection && !remainingGuardChoicesIntersection;
//                                
//                                // If the guard DD has become equivalent to false, we can stop here.
//                                if (guardChoicesIntersection.isZero()) {
//                                    break;
//                                }
//                            }
//                        }
//                        
//                        // Add the meta variables that encode the nondeterminisim to the different choices.
//                        for (uint_fast64_t j = 0; j < currentChoices; ++j) {
//                            allEdges += encodeIndex(j, 0, numberOfBinaryVariables, this->variables) * choiceDds[j];
//                        }
//                        
//                        // Delete currentChoices out of overlapping DD
//                        sumOfGuards = sumOfGuards * (!equalsNumberOfChoicesDd).template toAdd<ValueType>();
//                    }
//                    
//                    return std::make_pair(numberOfBinaryVariables, allEdges);
//                }
//            }
//            
//            storm::dd::Bdd<Type> computeIllegalFragmentFromEdges(std::map<uint64_t, std::vector<EdgeDd>> const& actionIndexToEdges) {
//                // Create the illegal fragment. For this, we need to find the edges that write global variables multiple times.
//                storm::dd::Bdd<Type> illegalFragment = this->variables.manager->getBddZero();
//                for (auto const& action : actionIndexToEdges) {
//                    for (auto const& edge : action.second) {
//                        if (!edge.globalVariablesWrittenMultipleTimes.empty()) {
//                            for (auto const& variable : edge.globalVariablesWrittenMultipleTimes) {
//                                STORM_LOG_WARN("The global variable '" << variable.getName() << "' is written along multiple synchronizing edges. If such a transition is contained in the reachable state space, an error is raised.");
//                                illegalFragment |= edge.guard.toBdd();
//                            }
//                        }
//                    }
//                }
//                return illegalFragment;
//            }
//            
//            ComposerResult<Type, ValueType> buildSystemFromAutomaton(AutomatonDd& automatonDd) {
//                // If the model is an MDP, we need to encode the nondeterminism using additional variables.
//                if (this->model.getModelType() == storm::jani::ModelType::MDP) {
//                    std::map<uint64_t, std::pair<uint64_t, storm::dd::Add<Type, ValueType>>> actionIndexToUsedVariablesAndDd;
//                    
//                    // Combine the edges of each action individually and keep track of how many local nondeterminism variables
//                    // were used.
//                    uint64_t highestNumberOfUsedNondeterminismVariables = 0;
//                    for (auto& action : automatonDd.actionIndexToEdges) {
//                        std::pair<uint64_t, storm::dd::Add<Type, ValueType>> usedVariablesAndDd = combineEdgesByNondeterministicChoice(action.second);
//                        actionIndexToUsedVariablesAndDd.emplace(action.first, usedVariablesAndDd);
//                        highestNumberOfUsedNondeterminismVariables = std::max(highestNumberOfUsedNondeterminismVariables, usedVariablesAndDd.first);
//                    }
//                    
//                    storm::dd::Add<Type, ValueType> result = this->variables.manager->template getAddZero<ValueType>();
//                    for (auto const& element : actionIndexToUsedVariablesAndDd) {
//                        result += element.second.second * encodeAction(element.first != this->model.getSilentActionIndex() ? boost::optional<uint64_t>(element.first) : boost::none, this->variables) * encodeIndex(0, element.second.first, highestNumberOfUsedNondeterminismVariables - element.second.first, this->variables);
//                    }
//                    
//                    return ComposerResult<Type, ValueType>(result, computeIllegalFragmentFromEdges(automatonDd.actionIndexToEdges), highestNumberOfUsedNondeterminismVariables);
//                } else if (this->model.getModelType() == storm::jani::ModelType::DTMC || this->model.getModelType() == storm::jani::ModelType::CTMC) {
//                    storm::dd::Add<Type, ValueType> result = this->variables.manager->template getAddZero<ValueType>();
//                    for (auto const& action : automatonDd.actionIndexToEdges) {
//                        result += combineEdgesBySummation(action.second, this->variables);
//                    }
//                    return ComposerResult<Type, ValueType>(result, computeIllegalFragmentFromEdges(automatonDd.actionIndexToEdges), 0);
//                }
//                
//                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal model type.");
//            }
//            
//            storm::dd::Add<Type, ValueType> computeMissingGlobalVariableIdentities(EdgeDd const& edge, CompositionVariables<Type, ValueType> const& variables) {
//                std::set<storm::expressions::Variable> missingIdentities;
//                std::set_difference(variables.allGlobalVariables.begin(), variables.allGlobalVariables.end(), edge.writtenGlobalVariables.begin(), edge.writtenGlobalVariables.end(), std::inserter(missingIdentities, missingIdentities.begin()));
//                storm::dd::Add<Type, ValueType> identityEncoding = variables.manager->template getAddOne<ValueType>();
//                for (auto const& variable : missingIdentities) {
//                    identityEncoding *= variables.variableToIdentityMap.at(variable);
//                }
//                return identityEncoding;
//            }
//            
//            EdgeDd extendEdgeWithIdentity(EdgeDd const& edge, storm::dd::Add<Type, ValueType> const& identity) {
//                EdgeDd result(edge);
//                result.transitions *= identity;
//                return result;
//            }
//            
//            EdgeDd composeEdgesInParallel(EdgeDd const& edge1, EdgeDd const& edge2) {
//                EdgeDd result;
//                
//                // Compose the guards.
//                result.guard = edge1.guard * edge2.guard;
//                
//                // If the composed guard is already zero, we can immediately return an empty result.
//                if (result.guard.isZero()) {
//                    result.transitions = edge1.transitions.getDdManager().template getAddZero<ValueType>();
//                    return result;
//                }
//                
//                // Compute the set of variables written multiple times by the composition.
//                std::set<storm::expressions::Variable> oldVariablesWrittenMultipleTimes;
//                std::set_union(edge1.globalVariablesWrittenMultipleTimes.begin(), edge1.globalVariablesWrittenMultipleTimes.end(), edge2.globalVariablesWrittenMultipleTimes.begin(), edge2.globalVariablesWrittenMultipleTimes.end(), std::inserter(oldVariablesWrittenMultipleTimes, oldVariablesWrittenMultipleTimes.begin()));
//                
//                std::set<storm::expressions::Variable> newVariablesWrittenMultipleTimes;
//                std::set_intersection(edge1.writtenGlobalVariables.begin(), edge1.writtenGlobalVariables.end(), edge2.writtenGlobalVariables.begin(), edge2.writtenGlobalVariables.end(), std::inserter(newVariablesWrittenMultipleTimes, newVariablesWrittenMultipleTimes.begin()));
//                
//                std::set<storm::expressions::Variable> variablesWrittenMultipleTimes;
//                std::set_union(oldVariablesWrittenMultipleTimes.begin(), oldVariablesWrittenMultipleTimes.end(), newVariablesWrittenMultipleTimes.begin(), newVariablesWrittenMultipleTimes.end(), std::inserter(variablesWrittenMultipleTimes, variablesWrittenMultipleTimes.begin()));
//                
//                result.globalVariablesWrittenMultipleTimes = std::move(variablesWrittenMultipleTimes);
//                
//                // Compute the set of variables written by the composition.
//                std::set<storm::expressions::Variable> variablesWritten;
//                std::set_union(edge1.writtenGlobalVariables.begin(), edge1.writtenGlobalVariables.end(), edge2.writtenGlobalVariables.begin(), edge2.writtenGlobalVariables.end(), std::inserter(variablesWritten, variablesWritten.begin()));
//                
//                result.writtenGlobalVariables = variablesWritten;
//                
//                // Compose the guards.
//                result.guard = edge1.guard * edge2.guard;
//                
//                // Compose the transitions.
//                result.transitions = edge1.transitions * edge2.transitions;
//                
//                return result;
//            }
//            
//            /*!
//             * Builds the DD for the given edge.
//             */
//            EdgeDd buildEdgeDd(storm::jani::Automaton const& automaton, storm::jani::Edge const& edge) {
//                STORM_LOG_TRACE("Translating guard " << edge.getGuard());
//                storm::dd::Add<Type, ValueType> guard = this->variables.rowExpressionAdapter->translateExpression(edge.getGuard());// * this->variables.automatonToRangeMap.at(automaton.getName());
//                STORM_LOG_WARN_COND(!guard.isZero(), "The guard '" << edge.getGuard() << "' is unsatisfiable.");
//                
//                if (!guard.isZero()) {
//                    // Create the DDs representing the individual updates.
//                    std::vector<EdgeDestinationDd<Type, ValueType>> destinationDds;
//                    for (storm::jani::EdgeDestination const& destination : edge.getDestinations()) {
//                        destinationDds.push_back(buildEdgeDestinationDd(automaton, destination, guard, this->variables));
//                        
//                        STORM_LOG_WARN_COND(!destinationDds.back().transitions.isZero(), "Destination does not have any effect.");
//                    }
//
//                    // Start by gathering all variables that were written in at least one update.
//                    std::set<storm::expressions::Variable> globalVariablesInSomeUpdate;
//                    
//                    // If the edge is not labeled with the silent action, we have to analyze which portion of the global
//                    // variables was written by any of the updates and make all update results equal w.r.t. this set. If
//                    // the edge is labeled with the silent action, we can already multiply the identities of all global variables.
//                    if (edge.getActionIndex() != this->model.getSilentActionIndex()) {
//                        for (auto const& edgeDestinationDd : destinationDds) {
//                            globalVariablesInSomeUpdate.insert(edgeDestinationDd.writtenGlobalVariables.begin(), edgeDestinationDd.writtenGlobalVariables.end());
//                        }
//                    } else {
//                        globalVariablesInSomeUpdate = this->variables.allGlobalVariables;
//                    }
//                    
//                    // Then, multiply the missing identities.
//                    for (auto& destinationDd : destinationDds) {
//                        std::set<storm::expressions::Variable> missingIdentities;
//                        std::set_difference(globalVariablesInSomeUpdate.begin(), globalVariablesInSomeUpdate.end(), destinationDd.writtenGlobalVariables.begin(), destinationDd.writtenGlobalVariables.end(), std::inserter(missingIdentities, missingIdentities.begin()));
//                        
//                        for (auto const& variable : missingIdentities) {
//                            STORM_LOG_TRACE("Multiplying identity for variable " << variable.getName() << " to destination DD.");
//                            destinationDd.transitions *= this->variables.variableToIdentityMap.at(variable);
//                        }
//                    }
//                    
//                    // Now combine the destination DDs to the edge DD.
//                    storm::dd::Add<Type, ValueType> transitions = this->variables.manager->template getAddZero<ValueType>();
//                    for (auto const& destinationDd : destinationDds) {
//                        transitions += destinationDd.transitions;
//                    }
//                    
//                    // Add the source location and the guard.
//                    transitions *= this->variables.manager->getEncoding(this->variables.automatonToLocationVariableMap.at(automaton.getName()).first, edge.getSourceLocationIndex()).template toAdd<ValueType>() * guard;
//                    
//                    // If we multiply the ranges of global variables, make sure everything stays within its bounds.
//                    if (!globalVariablesInSomeUpdate.empty()) {
//                        transitions *= this->variables.globalVariableRanges;
//                    }
//                    
//                    // If the edge has a rate, we multiply it to the DD.
//                    if (edge.hasRate()) {
//                        transitions *= this->variables.rowExpressionAdapter->translateExpression(edge.getRate());
//                    }
//                    return EdgeDd(guard, transitions, globalVariablesInSomeUpdate);
//                } else {
//                    return EdgeDd(this->variables.manager->template getAddZero<ValueType>(), this->variables.manager->template getAddZero<ValueType>());
//                }
//            }
//            
//            /*!
//             * Builds the DD for the automaton with the given name.
//             */
//            AutomatonDd buildAutomatonDd(std::string const& automatonName) {
//                AutomatonDd result(this->variables.automatonToIdentityMap.at(automatonName));
//                
//                storm::jani::Automaton const& automaton = this->model.getAutomaton(automatonName);
//                for (auto const& edge : automaton.getEdges()) {
//                    // Build the edge and add it if it adds transitions.
//                    EdgeDd edgeDd = buildEdgeDd(automaton, edge);
//                    if (!edgeDd.guard.isZero()) {
//                        result.actionIndexToEdges[edge.getActionIndex()].push_back(edgeDd);
//                    }
//                }
//                
//                return result;
//            }
//        };
        
        template <storm::dd::DdType Type, typename ValueType>
        class CombinedEdgesSystemComposer : public SystemComposer<Type, ValueType> {
        public:
            // This structure represents an edge.
            struct EdgeDd {
                EdgeDd(storm::dd::Add<Type> const& guard = storm::dd::Add<Type>(), storm::dd::Add<Type, ValueType> const& transitions = storm::dd::Add<Type, ValueType>(), std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> const& transientEdgeAssignments = {}, std::set<storm::expressions::Variable> const& writtenGlobalVariables = {}) : guard(guard), transitions(transitions), transientEdgeAssignments(transientEdgeAssignments), writtenGlobalVariables(writtenGlobalVariables) {
                    // Intentionally left empty.
                }
                
                // A DD that represents all states that have this edge enabled.
                storm::dd::Add<Type, ValueType> guard;
                
                // A DD that represents the transitions of this edge.
                storm::dd::Add<Type, ValueType> transitions;
                
                // A mapping from transient variables to the DDs representing their value assignments.
                std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> transientEdgeAssignments;
                
                // The set of global variables written by this edge.
                std::set<storm::expressions::Variable> writtenGlobalVariables;
            };
            
            // This structure represents an edge.
            struct ActionDd {
                ActionDd(storm::dd::Add<Type> const& guard = storm::dd::Add<Type>(), storm::dd::Add<Type, ValueType> const& transitions = storm::dd::Add<Type, ValueType>(), std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> const& transientEdgeAssignments = {}, std::pair<uint64_t, uint64_t> localNondeterminismVariables = std::pair<uint64_t, uint64_t>(0, 0), std::map<storm::expressions::Variable, storm::dd::Bdd<Type>> const& variableToWritingFragment = {}, storm::dd::Bdd<Type> const& illegalFragment = storm::dd::Bdd<Type>()) : guard(guard), transitions(transitions), transientEdgeAssignments(transientEdgeAssignments), localNondeterminismVariables(localNondeterminismVariables), variableToWritingFragment(variableToWritingFragment), illegalFragment(illegalFragment) {
                    // Intentionally left empty.
                }
                
                uint64_t getLowestLocalNondeterminismVariable() const {
                    return localNondeterminismVariables.first;
                }

                uint64_t getHighestLocalNondeterminismVariable() const {
                    return localNondeterminismVariables.second;
                }
                
                std::pair<uint64_t, uint64_t> const& getLocalNondeterminismVariables() const {
                    return localNondeterminismVariables;
                }

                // A DD that represents all states that have this edge enabled.
                storm::dd::Add<Type, ValueType> guard;
                
                // A DD that represents the transitions of this edge.
                storm::dd::Add<Type, ValueType> transitions;
                
                // A mapping from transient variables to their assignments.
                std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> transientEdgeAssignments;
                
                // The local nondeterminism variables used by this action DD, given as the lowest
                std::pair<uint64_t, uint64_t> localNondeterminismVariables;
                
                // A mapping from global variables to a DD that characterizes choices (nondeterminism variables) in
                // states that write to this global variable.
                std::map<storm::expressions::Variable, storm::dd::Bdd<Type>> variableToWritingFragment;
                
                // A DD characterizing the fragment of the states satisfying the guard that are illegal because
                // there are synchronizing edges enabled that write to the same global variable.
                storm::dd::Bdd<Type> illegalFragment;
            };
            
            // This structure represents a subcomponent of a composition.
            struct AutomatonDd {
                AutomatonDd(storm::dd::Add<Type, ValueType> const& identity, std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> const& transientLocationAssignments = {}) : actionIndexToAction(), transientLocationAssignments(transientLocationAssignments), identity(identity), localNondeterminismVariables(std::make_pair<uint64_t, uint64_t>(0, 0)) {
                    // Intentionally left empty.
                }
                
                uint64_t getLowestLocalNondeterminismVariable() const {
                    return localNondeterminismVariables.first;
                }

                void setLowestLocalNondeterminismVariable(uint64_t newValue) {
                    localNondeterminismVariables.first = newValue;
                }
                
                uint64_t getHighestLocalNondeterminismVariable() const {
                    return localNondeterminismVariables.second;
                }

                void setHighestLocalNondeterminismVariable(uint64_t newValue) {
                    localNondeterminismVariables.second = newValue;
                }
                
                void extendLocalNondeterminismVariables(std::pair<uint64_t, uint64_t> const& localNondeterminismVariables) {
                    setLowestLocalNondeterminismVariable(std::min(localNondeterminismVariables.first, getLowestLocalNondeterminismVariable()));
                    setHighestLocalNondeterminismVariable(std::max(localNondeterminismVariables.second, getHighestLocalNondeterminismVariable()));
                }
                
                // A mapping from action indices to the action DDs.
                std::map<uint64_t, ActionDd> actionIndexToAction;
                
                // A mapping from transient variables to their location-based transient assignment values.
                std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> transientLocationAssignments;
                
                // The identity of the automaton's variables.
                storm::dd::Add<Type, ValueType> identity;
                
                // The local nondeterminism variables used by this action DD, given as the lowest and highest variable index.
                std::pair<uint64_t, uint64_t> localNondeterminismVariables;

            };
            
            CombinedEdgesSystemComposer(storm::jani::Model const& model, storm::jani::ActionInformation const& actionInformation, CompositionVariables<Type, ValueType> const& variables, std::vector<storm::expressions::Variable> const& transientVariables) : SystemComposer<Type, ValueType>(model, variables, transientVariables), actionInformation(actionInformation) {
                // Intentionally left empty.
            }
        
            storm::jani::ActionInformation const& actionInformation;

            ComposerResult<Type, ValueType> compose() override {
                std::map<uint64_t, uint64_t> actionIndexToLocalNondeterminismVariableOffset;
                for (auto const& actionIndex : actionInformation.getNonSilentActionIndices()) {
                    actionIndexToLocalNondeterminismVariableOffset[actionIndex] = 0;
                }

                AutomatonDd globalAutomaton = boost::any_cast<AutomatonDd>(this->model.getSystemComposition().accept(*this, actionIndexToLocalNondeterminismVariableOffset));
                return buildSystemFromAutomaton(globalAutomaton);
            }
            
            boost::any visit(storm::jani::AutomatonComposition const& composition, boost::any const& data) override {
                std::map<uint64_t, uint64_t> const& actionIndexToLocalNondeterminismVariableOffset = boost::any_cast<std::map<uint_fast64_t, uint_fast64_t> const&>(data);
                return buildAutomatonDd(composition.getAutomatonName(), actionIndexToLocalNondeterminismVariableOffset);
            }
            
            boost::any visit(storm::jani::RenameComposition const& composition, boost::any const& data) override {
                // Build a mapping from indices to indices for the renaming.
                std::map<uint64_t, uint64_t> renamingIndexToIndex;
                for (auto const& entry : composition.getRenaming()) {
                    if (actionInformation.getActionIndex(entry.first) != this->model.getSilentActionIndex()) {
                        // Distinguish the cases where we hide the action or properly rename it.
                        if (entry.second) {
                            renamingIndexToIndex.emplace(actionInformation.getActionIndex(entry.first), actionInformation.getActionIndex(entry.second.get()));
                        } else {
                            renamingIndexToIndex.emplace(actionInformation.getActionIndex(entry.first), this->model.getSilentActionIndex());
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Renaming composition must not rename the silent action.");
                    }
                }
                
                // Prepare the new offset mapping.
                std::map<uint64_t, uint64_t> const& actionIndexToLocalNondeterminismVariableOffset = boost::any_cast<std::map<uint64_t, uint64_t> const&>(data);
                std::map<uint64_t, uint64_t> newSynchronizingActionToOffsetMap = actionIndexToLocalNondeterminismVariableOffset;
                for (auto const& indexPair : renamingIndexToIndex) {
                    // FIXME: Check correct? Introduce zero if not contained?
                    auto it = actionIndexToLocalNondeterminismVariableOffset.find(indexPair.second);
                    STORM_LOG_THROW(it != actionIndexToLocalNondeterminismVariableOffset.end(), storm::exceptions::InvalidArgumentException, "Invalid action index " << indexPair.second << ".");
                    newSynchronizingActionToOffsetMap[indexPair.first] = it->second;
                }
                
                // Then, we translate the subcomposition.
                AutomatonDd subautomaton = boost::any_cast<AutomatonDd>(composition.getSubcomposition().accept(*this, newSynchronizingActionToOffsetMap));

                // Now perform the renaming and return result.
                return rename(subautomaton, renamingIndexToIndex);
            }
            
            boost::any visit(storm::jani::ParallelComposition const& composition, boost::any const& data) override {
                std::map<uint64_t, uint64_t> const& actionIndexToLocalNondeterminismVariableOffset = boost::any_cast<std::map<uint64_t, uint64_t> const&>(data);
                
                std::vector<AutomatonDd> subautomata;
                for (uint64_t subcompositionIndex = 0; subcompositionIndex < composition.getNumberOfSubcompositions(); ++subcompositionIndex) {
                    // Prepare the new offset mapping.
                    std::map<uint64_t, uint64_t> newSynchronizingActionToOffsetMap = actionIndexToLocalNondeterminismVariableOffset;
                    if (subcompositionIndex == 0) {
                        for (auto const& synchVector : composition.getSynchronizationVectors()) {
                            auto it = actionIndexToLocalNondeterminismVariableOffset.find(actionInformation.getActionIndex(synchVector.getOutput()));
                            STORM_LOG_THROW(it != actionIndexToLocalNondeterminismVariableOffset.end(), storm::exceptions::InvalidArgumentException, "Invalid action " << synchVector.getOutput() << ".");
                            newSynchronizingActionToOffsetMap[actionInformation.getActionIndex(synchVector.getInput(0))] = it->second;
                        }
                    } else {
                        AutomatonDd const& previousAutomatonDd = subautomata.back();
                        
                        // Based on the previous results, we need to update the offsets.
                        for (auto const& synchVector : composition.getSynchronizationVectors()) {
                            auto it = previousAutomatonDd.actionIndexToAction.find(actionInformation.getActionIndex(synchVector.getInput(subcompositionIndex - 1)));
                            if (it != previousAutomatonDd.actionIndexToAction.end()) {
                                newSynchronizingActionToOffsetMap[actionInformation.getActionIndex(synchVector.getInput(subcompositionIndex))] = it->second.getHighestLocalNondeterminismVariable();
                            } else {
                                STORM_LOG_ASSERT(false, "Subcomposition does not have action that is mentioned in parallel composition.");
                            }
                        }
                    }
                    
                    // Build the DD for the next element of the composition wrt. to the current offset mapping.
                    subautomata.push_back(boost::any_cast<AutomatonDd>(composition.getSubcomposition(subcompositionIndex).accept(*this, newSynchronizingActionToOffsetMap)));
                }
                
                return composeInParallel(subautomata, composition.getSynchronizationVectors());
            }
            
        private:
            AutomatonDd composeInParallel(std::vector<AutomatonDd> const& subautomata, std::vector<storm::jani::SynchronizationVector> const& synchronizationVectors) {
                AutomatonDd result(this->variables.manager->template getAddOne<ValueType>());
                
                for (uint64_t automatonIndex = 0; automatonIndex < subautomata.size(); ++automatonIndex) {
                    AutomatonDd const& subautomaton = subautomata[automatonIndex];
                    
                    // Construct combined identity.
                    result.identity *= subautomaton.identity;
                    
                    addToTransientAssignmentMap(result.transientLocationAssignments, subautomaton.transientLocationAssignments);
                }
                
                result.transientLocationAssignments = joinTransientAssignmentMaps(automaton1.transientLocationAssignments, automaton2.transientLocationAssignments);
                
                // Treat all actions of the first automaton.
                for (auto const& action1 : automaton1.actionIndexToAction) {
                    // If we need to synchronize over this action index, we try to do so now.
                    if (synchronizingActionIndices.find(action1.first) != synchronizingActionIndices.end()) {
                        auto action2It = automaton2.actionIndexToAction.find(action1.first);
                        if (action2It != automaton2.actionIndexToAction.end()) {
                            ActionDd newAction = combineSynchronizingActions(action1.second, action2It->second);
                            result.actionIndexToAction[action1.first] = newAction;
                            result.extendLocalNondeterminismVariables(newAction.getLocalNondeterminismVariables());
                        }
                    } else {
                        // If we don't synchronize over this action, we need to construct the interleaving.
                        
                        // If both automata contain the action, we need to combine the actions in an unsynchronized way.
                        auto action2It = automaton2.actionIndexToAction.find(action1.first);
                        if (action2It != automaton2.actionIndexToAction.end()) {
                            ActionDd newAction = combineUnsynchronizedActions(action1.second, action2It->second, automaton1.identity, automaton2.identity);
                            result.actionIndexToAction[action1.first] = newAction;
                            result.extendLocalNondeterminismVariables(newAction.getLocalNondeterminismVariables());
                        } else {
                            // If only the first automaton has this action, we only need to apply the identity of the
                            // second automaton.
                            result.actionIndexToAction[action1.first] = ActionDd(action1.second.guard, action1.second.transitions * automaton2.identity, action1.second.transientEdgeAssignments, action1.second.localNondeterminismVariables, action1.second.variableToWritingFragment, action1.second.illegalFragment);
                        }
                    }
                }
                
                // Treat the actions of the second automaton.
                for (auto const& action2 : automaton2.actionIndexToAction) {
                    // Here, we only need to treat actions that the first automaton does not have, because we have handled
                    // this case earlier. Likewise, we have to consider non-synchronizing actions only.
                    if (automaton1.actionIndexToAction.find(action2.first) == automaton1.actionIndexToAction.end()) {
                        if (synchronizingActionIndices.find(action2.first) == synchronizingActionIndices.end()) {
                            // If only the second automaton has this action, we only need to apply the identity of the
                            // first automaton.
                            result.actionIndexToAction[action2.first] = ActionDd(action2.second.guard, action2.second.transitions * automaton1.identity, action2.second.transientEdgeAssignments, action2.second.localNondeterminismVariables, action2.second.variableToWritingFragment, action2.second.illegalFragment);
                        }
                    }
                }
                
                result.identity = automaton1.identity * automaton2.identity;
                
                return result;
            }
            
            ActionDd combineSynchronizingActions(ActionDd action1, ActionDd action2) {
                storm::dd::Bdd<Type> illegalFragment = this->variables.manager->getBddZero();
                storm::dd::Bdd<Type> guard1 = action1.guard.toBdd();
                storm::dd::Bdd<Type> guard2 = action2.guard.toBdd();
                storm::dd::Bdd<Type> combinedGuard = guard1 && guard2;
                
                // Cross-multiply the guards to the other fragments that write global variables and check for overlapping
                // parts. This finds illegal parts in which a global variable is written multiple times.
                std::map<storm::expressions::Variable, storm::dd::Bdd<Type>> globalVariableToWritingFragment;
                for (auto& entry : action1.variableToWritingFragment) {
                    entry.second &= guard2;
                    globalVariableToWritingFragment[entry.first] = entry.second;
                }
                for (auto& entry : action2.variableToWritingFragment) {
                    entry.second &= guard1;
                    auto it = globalVariableToWritingFragment.find(entry.first);
                    if (it != globalVariableToWritingFragment.end()) {
                        auto action1LocalNondeterminismVariableSet = std::set<storm::expressions::Variable>(this->variables.localNondeterminismVariables.begin() + action1.getLowestLocalNondeterminismVariable(), this->variables.localNondeterminismVariables.begin() + action1.getHighestLocalNondeterminismVariable());
                        auto action2LocalNondeterminismVariableSet = std::set<storm::expressions::Variable>(this->variables.localNondeterminismVariables.begin() + action2.getLowestLocalNondeterminismVariable(), this->variables.localNondeterminismVariables.begin() + action2.getHighestLocalNondeterminismVariable());
                        illegalFragment |= it->second.existsAbstract(action1LocalNondeterminismVariableSet) && entry.second.existsAbstract(action2LocalNondeterminismVariableSet);
                        it->second &= entry.second;
                    } else {
                        globalVariableToWritingFragment[entry.first] = entry.second;
                    }
                }
                
                return ActionDd(combinedGuard.template toAdd<ValueType>(), action1.transitions * action2.transitions, joinTransientAssignmentMaps(action1.transientEdgeAssignments, action2.transientEdgeAssignments), std::make_pair(std::min(action1.getLowestLocalNondeterminismVariable(), action2.getLowestLocalNondeterminismVariable()), std::max(action1.getHighestLocalNondeterminismVariable(), action2.getHighestLocalNondeterminismVariable())), globalVariableToWritingFragment, illegalFragment);
            }
            
            ActionDd combineUnsynchronizedActions(ActionDd action1, ActionDd action2, storm::dd::Add<Type, ValueType> const& identity1, storm::dd::Add<Type, ValueType> const& identity2) {
                // First extend the action DDs by the other identities.
                STORM_LOG_TRACE("Multiplying identities to combine unsynchronized actions.");
                action1.transitions = action1.transitions * identity2;
                action2.transitions = action2.transitions * identity1;
                
                // Then combine the extended action DDs.
                return combineUnsynchronizedActions(action1, action2);
            }
            
            ActionDd combineUnsynchronizedActions(ActionDd action1, ActionDd action2) {
                STORM_LOG_TRACE("Combining unsynchronized actions.");
                
                if (this->model.getModelType() == storm::jani::ModelType::DTMC || this->model.getModelType() == storm::jani::ModelType::CTMC) {
                    return ActionDd(action1.guard + action2.guard, action1.transitions + action2.transitions, joinTransientAssignmentMaps(action1.transientEdgeAssignments, action2.transientEdgeAssignments), std::make_pair<uint64_t, uint64_t>(0, 0), joinVariableWritingFragmentMaps(action1.variableToWritingFragment, action2.variableToWritingFragment), this->variables.manager->getBddZero());
                } else if (this->model.getModelType() == storm::jani::ModelType::MDP) {
                    if (action1.transitions.isZero()) {
                        return action2;
                    } else if (action2.transitions.isZero()) {
                        return action1;
                    }
                    
                    // Bring both choices to the same number of variables that encode the nondeterminism.
                    STORM_LOG_ASSERT(action1.getLowestLocalNondeterminismVariable() == action2.getLowestLocalNondeterminismVariable(), "Mismatching lowest nondeterminism variable indices.");
                    uint_fast64_t highestLocalNondeterminismVariable = std::max(action1.getHighestLocalNondeterminismVariable(), action2.getHighestLocalNondeterminismVariable());
                    if (action1.getHighestLocalNondeterminismVariable() > action2.getHighestLocalNondeterminismVariable()) {
                        storm::dd::Add<Type, ValueType> nondeterminismEncoding = this->variables.manager->template getAddOne<ValueType>();
                        
                        for (uint_fast64_t i = action2.getHighestLocalNondeterminismVariable(); i < action1.getHighestLocalNondeterminismVariable(); ++i) {
                            nondeterminismEncoding *= this->variables.manager->getEncoding(this->variables.localNondeterminismVariables[i], 0).template toAdd<ValueType>();
                        }
                        action2.transitions *= nondeterminismEncoding;
                        
                        for (auto& transientAssignment : action2.transientEdgeAssignments) {
                            transientAssignment.second *= nondeterminismEncoding;
                        }
                    } else if (action2.getHighestLocalNondeterminismVariable() > action1.getHighestLocalNondeterminismVariable()) {
                        storm::dd::Add<Type, ValueType> nondeterminismEncoding = this->variables.manager->template getAddOne<ValueType>();
                        
                        for (uint_fast64_t i = action1.getHighestLocalNondeterminismVariable(); i < action2.getHighestLocalNondeterminismVariable(); ++i) {
                            nondeterminismEncoding *= this->variables.manager->getEncoding(this->variables.localNondeterminismVariables[i], 0).template toAdd<ValueType>();
                        }
                        action1.transitions *= nondeterminismEncoding;

                        for (auto& transientAssignment : action1.transientEdgeAssignments) {
                            transientAssignment.second *= nondeterminismEncoding;
                        }
                    }
                    
                    // Add a new variable that resolves the nondeterminism between the two choices.
                    storm::dd::Add<Type, ValueType> combinedTransitions = this->variables.manager->getEncoding(this->variables.localNondeterminismVariables[highestLocalNondeterminismVariable], 1).ite(action2.transitions, action1.transitions);
                    
                    // Add the new variable to the writing fragments of each global variable before joining them.
                    for (auto& entry : action1.variableToWritingFragment) {
                        entry.second = this->variables.manager->getEncoding(this->variables.localNondeterminismVariables[highestLocalNondeterminismVariable], 0) && entry.second;
                    }
                    for (auto& entry : action2.variableToWritingFragment) {
                        entry.second = this->variables.manager->getEncoding(this->variables.localNondeterminismVariables[highestLocalNondeterminismVariable], 1) && entry.second;
                    }
                    
                    return ActionDd((action1.guard.toBdd() || action2.guard.toBdd()).template toAdd<ValueType>(), combinedTransitions, joinTransientAssignmentMaps(action1.transientEdgeAssignments, action2.transientEdgeAssignments), std::make_pair(action1.getLowestLocalNondeterminismVariable(), highestLocalNondeterminismVariable + 1), joinVariableWritingFragmentMaps(action1.variableToWritingFragment, action2.variableToWritingFragment), action1.illegalFragment || action2.illegalFragment);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidStateException, "Illegal model type.");
                }
            }
            
            AutomatonDd rename(AutomatonDd const& automaton, std::map<uint64_t, uint64_t> const& indexToIndex) {
                AutomatonDd result(automaton.identity, automaton.transientLocationAssignments);
                
                for (auto const& action : automaton.actionIndexToAction) {
                    auto renamingIt = indexToIndex.find(action.first);
                    if (renamingIt != indexToIndex.end()) {
                        // If the action is to be renamed and an action with the target index already exists, we need
                        // to combine the action DDs.
                        auto itNewActions = result.actionIndexToAction.find(renamingIt->second);
                        if (itNewActions != result.actionIndexToAction.end()) {
                            itNewActions->second = combineUnsynchronizedActions(action.second, itNewActions->second);
                        } else {
                            // In this case, we can simply copy the action over.
                            result.actionIndexToAction[renamingIt->second] = action.second;
                        }
                    } else {
                        // If the action is not to be renamed, we need to copy it over. However, if some other action
                        // was renamed to the very same action name before, we need to combine the transitions.
                        auto itNewActions = result.actionIndexToAction.find(action.first);
                        if (itNewActions != result.actionIndexToAction.end()) {
                            itNewActions->second = combineUnsynchronizedActions(action.second, itNewActions->second);
                        } else {
                            // In this case, we can simply copy the action over.
                            result.actionIndexToAction[action.first] = action.second;
                        }
                    }
                }
                
                return result;
            }
            
            void performTransientAssignments(storm::jani::detail::ConstAssignments const& transientAssignments, std::function<void (storm::jani::Assignment const&)> const& callback) {
                auto transientVariableIt = this->transientVariables.begin();
                auto transientVariableIte = this->transientVariables.end();
                for (auto const& assignment : transientAssignments) {
                    while (transientVariableIt != transientVariableIte && *transientVariableIt < assignment.getExpressionVariable()) {
                        ++transientVariableIt;
                    }
                    if (transientVariableIt == transientVariableIte) {
                        break;
                    }
                    if (*transientVariableIt == assignment.getExpressionVariable()) {
                        callback(assignment);
                        ++transientVariableIt;
                    }
                }
            }
            
            EdgeDd buildEdgeDd(storm::jani::Automaton const& automaton, storm::jani::Edge const& edge) {
                STORM_LOG_TRACE("Translating guard " << edge.getGuard());
                
                // We keep the guard and a "ranged" version seperate, because building the destinations tends to be
                // slower when the full range is applied.
                storm::dd::Add<Type, ValueType> guard = this->variables.rowExpressionAdapter->translateExpression(edge.getGuard());
                storm::dd::Add<Type, ValueType> rangedGuard = guard * this->variables.automatonToRangeMap.at(automaton.getName());
                STORM_LOG_WARN_COND(!rangedGuard.isZero(), "The guard '" << edge.getGuard() << "' is unsatisfiable.");
                
                if (!rangedGuard.isZero()) {
                    // Create the DDs representing the individual updates.
                    std::vector<EdgeDestinationDd<Type, ValueType>> destinationDds;
                    for (storm::jani::EdgeDestination const& destination : edge.getDestinations()) {
                        destinationDds.push_back(buildEdgeDestinationDd(automaton, destination, guard, this->variables));
                        
                        STORM_LOG_WARN_COND(!destinationDds.back().transitions.isZero(), "Destination does not have any effect.");
                    }
                    
                    // Now that we have built the destinations, we always take the full guard.
                    guard = rangedGuard;
                    
                    // Start by gathering all variables that were written in at least one destination.
                    std::set<storm::expressions::Variable> globalVariablesInSomeDestination;
                    
                    // If the edge is not labeled with the silent action, we have to analyze which portion of the global
                    // variables was written by any of the updates and make all update results equal w.r.t. this set. If
                    // the edge is labeled with the silent action, we can already multiply the identities of all global variables.
                    if (edge.getActionIndex() != this->model.getSilentActionIndex()) {
                        for (auto const& edgeDestinationDd : destinationDds) {
                            globalVariablesInSomeDestination.insert(edgeDestinationDd.writtenGlobalVariables.begin(), edgeDestinationDd.writtenGlobalVariables.end());
                        }
                    } else {
                        globalVariablesInSomeDestination = this->variables.allGlobalVariables;
                    }
                    
                    // Then, multiply the missing identities.
                    for (auto& destinationDd : destinationDds) {
                        std::set<storm::expressions::Variable> missingIdentities;
                        std::set_difference(globalVariablesInSomeDestination.begin(), globalVariablesInSomeDestination.end(), destinationDd.writtenGlobalVariables.begin(), destinationDd.writtenGlobalVariables.end(), std::inserter(missingIdentities, missingIdentities.begin()));
                        
                        for (auto const& variable : missingIdentities) {
                            STORM_LOG_TRACE("Multiplying identity for variable " << variable.getName() << " to destination DD.");
                            destinationDd.transitions *= this->variables.variableToIdentityMap.at(variable);
                        }
                    }
                    
                    // Now combine the destination DDs to the edge DD.
                    storm::dd::Add<Type, ValueType> transitions = this->variables.manager->template getAddZero<ValueType>();
                    for (auto const& destinationDd : destinationDds) {
                        transitions += destinationDd.transitions;
                    }
                    
                    // Add the source location and the guard.
                    transitions *= this->variables.manager->getEncoding(this->variables.automatonToLocationVariableMap.at(automaton.getName()).first, edge.getSourceLocationIndex()).template toAdd<ValueType>() * guard;
                    
                    // If we multiply the ranges of global variables, make sure everything stays within its bounds.
                    if (!globalVariablesInSomeDestination.empty()) {
                        transitions *= this->variables.globalVariableRanges;
                    }
                    
                    // If the edge has a rate, we multiply it to the DD.
                    if (edge.hasRate()) {
                        transitions *= this->variables.rowExpressionAdapter->translateExpression(edge.getRate());
                    }
                    
                    // Finally treat the transient assignments.
                    std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> transientEdgeAssignments;
                    if (!this->transientVariables.empty()) {
                        performTransientAssignments(edge.getAssignments().getTransientAssignments(), [this, &transientEdgeAssignments, &guard] (storm::jani::Assignment const& assignment) { transientEdgeAssignments[assignment.getExpressionVariable()] = guard * this->variables.rowExpressionAdapter->translateExpression(assignment.getAssignedExpression()); } );
                    }
                    
                    return EdgeDd(guard, guard * transitions, transientEdgeAssignments, globalVariablesInSomeDestination);
                } else {
                    return EdgeDd(this->variables.manager->template getAddZero<ValueType>(), this->variables.manager->template getAddZero<ValueType>());
                }
            }
            
            ActionDd buildActionDdForActionIndex(storm::jani::Automaton const& automaton, uint64_t actionIndex, uint64_t localNondeterminismVariableOffset) {
                // Translate the individual edges.
                std::vector<EdgeDd> edgeDds;
                for (auto const& edge : automaton.getEdges()) {
                    if (edge.getActionIndex() == actionIndex) {
                        edgeDds.push_back(buildEdgeDd(automaton, edge));
                    }
                }
                
                // Now combine the edges to a single action.
                if (!edgeDds.empty()) {
                    switch (this->model.getModelType()){
                        case storm::jani::ModelType::DTMC:
                        case storm::jani::ModelType::CTMC:
                            return combineEdgesToActionMarkovChain(edgeDds);
                            break;
                        case storm::jani::ModelType::MDP:
                            return combineEdgesToActionMdp(edgeDds, localNondeterminismVariableOffset);
                            break;
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot translate model of this type.");
                    }
                } else {
                    return ActionDd(this->variables.manager->template getAddZero<ValueType>(), this->variables.manager->template getAddZero<ValueType>(), {}, std::make_pair<uint64_t, uint64_t>(0, 0), {}, this->variables.manager->getBddZero());
                }
            }
            
            void addToTransientAssignmentMap(std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>>& transientAssignments, std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> const& assignmentsToAdd) {
                for (auto const& entry : assignmentsToAdd) {
                    auto it = transientAssignments.find(entry.first);
                    if (it != transientAssignments.end()) {
                        it->second += entry.second;
                    } else {
                        transientAssignments[entry.first] = entry.second;
                    }
                }
            }

            void addToTransientAssignmentMap(std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>>& transientAssignments, storm::expressions::Variable const& variable, storm::dd::Add<Type, ValueType> const& assignmentToAdd) {
                auto it = transientAssignments.find(variable);
                if (it != transientAssignments.end()) {
                    it->second += assignmentToAdd;
                } else {
                    transientAssignments[variable] = assignmentToAdd;
                }
            }

            std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> joinTransientAssignmentMaps(std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> const& transientAssignments1, std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> const& transientAssignments2) {
                std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> result = transientAssignments1;
                
                for (auto const& entry : transientAssignments2) {
                    auto resultIt = result.find(entry.first);
                    if (resultIt != result.end()) {
                        resultIt->second += entry.second;
                    } else {
                        result[entry.first] = entry.second;
                    }
                }
                
                return result;
            }

            ActionDd combineEdgesToActionMarkovChain(std::vector<EdgeDd> const& edgeDds) {
                storm::dd::Bdd<Type> allGuards = this->variables.manager->getBddZero();
                storm::dd::Add<Type, ValueType> allTransitions = this->variables.manager->template getAddZero<ValueType>();
                storm::dd::Bdd<Type> temporary;
                
                std::map<storm::expressions::Variable, storm::dd::Bdd<Type>> globalVariableToWritingFragment;
                std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> transientEdgeAssignments;
                for (auto const& edgeDd : edgeDds) {
                    // Check for overlapping guards.
                    storm::dd::Bdd<Type> guardBdd = edgeDd.guard.toBdd();
                    temporary = guardBdd && allGuards;
                    
                    // Issue a warning if there are overlapping guards in a DTMC.
                    STORM_LOG_WARN_COND(temporary.isZero() || this->model.getModelType() == storm::jani::ModelType::CTMC, "Guard of a command overlaps with previous guards.");
                    
                    // Add the elements of the current edge to the global ones.
                    allGuards |= guardBdd;
                    allTransitions += edgeDd.transitions;
                    
                    // Add the transient variable assignments to the resulting one.
                    addToTransientAssignmentMap(transientEdgeAssignments, edgeDd.transientEdgeAssignments);
                    
                    // Keep track of the fragment that is writing global variables.
                    for (auto const& variable : edgeDd.writtenGlobalVariables) {
                        auto it = globalVariableToWritingFragment.find(variable);
                        if (it != globalVariableToWritingFragment.end()) {
                            it->second |= guardBdd;
                        } else {
                            globalVariableToWritingFragment[variable] = guardBdd;
                        }
                    }
                }
                
                return ActionDd(allGuards.template toAdd<ValueType>(), allTransitions, transientEdgeAssignments, std::make_pair<uint64_t, uint64_t>(0, 0), globalVariableToWritingFragment, this->variables.manager->getBddZero());
            }
            
            void addToVariableWritingFragmentMap(std::map<storm::expressions::Variable, storm::dd::Bdd<Type>>& globalVariableToWritingFragment, storm::expressions::Variable const& variable, storm::dd::Bdd<Type> const& partToAdd) const {
                auto it = globalVariableToWritingFragment.find(variable);
                if (it != globalVariableToWritingFragment.end()) {
                    it->second |= partToAdd;
                } else {
                    globalVariableToWritingFragment.emplace(variable, partToAdd);
                }
            }
            
            std::map<storm::expressions::Variable, storm::dd::Bdd<Type>> joinVariableWritingFragmentMaps(std::map<storm::expressions::Variable, storm::dd::Bdd<Type>> const& globalVariableToWritingFragment1, std::map<storm::expressions::Variable, storm::dd::Bdd<Type>> const& globalVariableToWritingFragment2) {
                std::map<storm::expressions::Variable, storm::dd::Bdd<Type>> result = globalVariableToWritingFragment1;
                
                for (auto const& entry : globalVariableToWritingFragment2) {
                    auto resultIt = result.find(entry.first);
                    if (resultIt != result.end()) {
                        resultIt->second |= entry.second;
                    } else {
                        result[entry.first] = entry.second;
                    }
                }
                
                return result;
            }
            
            ActionDd combineEdgesBySummation(storm::dd::Add<Type, ValueType> const& guard, std::vector<EdgeDd> const& edges) {
                storm::dd::Add<Type, ValueType> transitions = this->variables.manager->template getAddZero<ValueType>();
                std::map<storm::expressions::Variable, storm::dd::Bdd<Type>> globalVariableToWritingFragment;
                std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> transientEdgeAssignments;
                for (auto const& edge : edges) {
                    transitions += edge.transitions;
                    addToTransientAssignmentMap(transientEdgeAssignments, edge.transientEdgeAssignments);
                    for (auto const& variable : edge.writtenGlobalVariables) {
                        addToVariableWritingFragmentMap(globalVariableToWritingFragment, variable, edge.guard.toBdd());
                    }
                }
                return ActionDd(guard, transitions, transientEdgeAssignments, std::make_pair<uint64_t, uint64_t>(0, 0), globalVariableToWritingFragment, this->variables.manager->getBddZero());
            }
            
            ActionDd combineEdgesToActionMdp(std::vector<EdgeDd> const& edges, uint64_t localNondeterminismVariableOffset) {
                // Sum all guards, so we can read off the maximal number of nondeterministic choices in any given state.
                storm::dd::Bdd<Type> allGuards = this->variables.manager->getBddZero();
                storm::dd::Add<Type, ValueType> sumOfGuards = this->variables.manager->template getAddZero<ValueType>();
                for (auto const& edge : edges) {
                    sumOfGuards += edge.guard;
                    allGuards |= edge.guard.toBdd();
                }
                uint_fast64_t maxChoices = static_cast<uint_fast64_t>(sumOfGuards.getMax());
                STORM_LOG_TRACE("Found " << maxChoices << " local choices.");
                
                // Depending on the maximal number of nondeterminstic choices, we need to use some variables to encode the nondeterminism.
                if (maxChoices <= 1) {
                    return combineEdgesBySummation(allGuards.template toAdd<ValueType>(), edges);
                } else {
                    // Calculate number of required variables to encode the nondeterminism.
                    uint_fast64_t numberOfBinaryVariables = static_cast<uint_fast64_t>(std::ceil(storm::utility::math::log2(maxChoices)));
                    
                    storm::dd::Add<Type, ValueType> allEdges = this->variables.manager->template getAddZero<ValueType>();
                    std::map<storm::expressions::Variable, storm::dd::Bdd<Type>> globalVariableToWritingFragment;
                    std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> transientAssignments;
                    
                    storm::dd::Bdd<Type> equalsNumberOfChoicesDd;
                    std::vector<storm::dd::Add<Type, ValueType>> choiceDds(maxChoices, this->variables.manager->template getAddZero<ValueType>());
                    std::vector<storm::dd::Bdd<Type>> remainingDds(maxChoices, this->variables.manager->getBddZero());
                    std::vector<std::pair<storm::dd::Bdd<Type>, storm::dd::Add<Type, ValueType>>> indicesEncodedWithLocalNondeterminismVariables;
                    for (uint64_t j = 0; j < maxChoices; ++j) {
                        storm::dd::Add<Type, ValueType> indexEncoding = encodeIndex(j, localNondeterminismVariableOffset, numberOfBinaryVariables, this->variables);
                        indicesEncodedWithLocalNondeterminismVariables.push_back(std::make_pair(indexEncoding.toBdd(), indexEncoding));
                    }
                    
                    for (uint_fast64_t currentChoices = 1; currentChoices <= maxChoices; ++currentChoices) {
                        // Determine the set of states with exactly currentChoices choices.
                        equalsNumberOfChoicesDd = sumOfGuards.equals(this->variables.manager->getConstant(static_cast<double>(currentChoices)));
                        
                        // If there is no such state, continue with the next possible number of choices.
                        if (equalsNumberOfChoicesDd.isZero()) {
                            continue;
                        }
                        
                        // Reset the previously used intermediate storage.
                        for (uint_fast64_t j = 0; j < currentChoices; ++j) {
                            choiceDds[j] = this->variables.manager->template getAddZero<ValueType>();
                            remainingDds[j] = equalsNumberOfChoicesDd;
                        }
                        
                        for (std::size_t j = 0; j < edges.size(); ++j) {
                            EdgeDd const& currentEdge = edges[j];
                            
                            // Check if edge guard overlaps with equalsNumberOfChoicesDd. That is, there are states with exactly currentChoices
                            // choices such that one outgoing choice is given by the j-th edge.
                            storm::dd::Bdd<Type> guardChoicesIntersection = currentEdge.guard.toBdd() && equalsNumberOfChoicesDd;
                            
                            // If there is no such state, continue with the next command.
                            if (guardChoicesIntersection.isZero()) {
                                continue;
                            }
                            
                            // Split the currentChoices nondeterministic choices.
                            for (uint_fast64_t k = 0; k < currentChoices; ++k) {
                                // Calculate the overlapping part of command guard and the remaining DD.
                                storm::dd::Bdd<Type> remainingGuardChoicesIntersection = guardChoicesIntersection && remainingDds[k];
                                
                                // Check if we can add some overlapping parts to the current index.
                                if (!remainingGuardChoicesIntersection.isZero()) {
                                    // Remove overlapping parts from the remaining DD.
                                    remainingDds[k] = remainingDds[k] && !remainingGuardChoicesIntersection;
                                    
                                    // Combine the overlapping part of the guard with command updates and add it to the resulting DD.
                                    choiceDds[k] += remainingGuardChoicesIntersection.template toAdd<ValueType>() * currentEdge.transitions;
                                    
                                    // Keep track of the fragment of transient assignments.
                                    for (auto const& transientAssignment : currentEdge.transientEdgeAssignments) {
                                        addToTransientAssignmentMap(transientAssignments, transientAssignment.first, remainingGuardChoicesIntersection.template toAdd<ValueType>() * transientAssignment.second * indicesEncodedWithLocalNondeterminismVariables[k].first.template toAdd<ValueType>());
                                    }
                                    
                                    // Keep track of the written global variables of the fragment.
                                    for (auto const& variable : currentEdge.writtenGlobalVariables) {
                                        addToVariableWritingFragmentMap(globalVariableToWritingFragment, variable, remainingGuardChoicesIntersection && indicesEncodedWithLocalNondeterminismVariables[k].first);
                                    }
                                }
                                
                                // Remove overlapping parts from the command guard DD
                                guardChoicesIntersection = guardChoicesIntersection && !remainingGuardChoicesIntersection;
                                
                                // If the guard DD has become equivalent to false, we can stop here.
                                if (guardChoicesIntersection.isZero()) {
                                    break;
                                }
                            }
                        }
                        
                        // Add the meta variables that encode the nondeterminisim to the different choices.
                        for (uint_fast64_t j = 0; j < currentChoices; ++j) {
                            allEdges += indicesEncodedWithLocalNondeterminismVariables[j].second * choiceDds[j];
                        }
                        
                        // Delete currentChoices out of overlapping DD
                        sumOfGuards = sumOfGuards * (!equalsNumberOfChoicesDd).template toAdd<ValueType>();
                    }
                    
                    return ActionDd(allGuards.template toAdd<ValueType>(), allEdges, transientAssignments, std::make_pair(localNondeterminismVariableOffset, localNondeterminismVariableOffset + numberOfBinaryVariables), globalVariableToWritingFragment, this->variables.manager->getBddZero());
                }
            }
            
            AutomatonDd buildAutomatonDd(std::string const& automatonName, std::map<uint_fast64_t, uint_fast64_t> const& actionIndexToLocalNondeterminismVariableOffset) {
                AutomatonDd result(this->variables.automatonToIdentityMap.at(automatonName));
                
                storm::jani::Automaton const& automaton = this->model.getAutomaton(automatonName);
                for (auto const& action : this->model.getActions()) {
                    uint64_t actionIndex = this->model.getActionIndex(action.getName());
                    if (!automaton.hasEdgeLabeledWithActionIndex(actionIndex)) {
                        continue;
                    }
                    ActionDd actionDd = buildActionDdForActionIndex(automaton, actionIndex, actionIndexToLocalNondeterminismVariableOffset.at(actionIndex));
                    result.actionIndexToAction[actionIndex] = actionDd;
                    result.setLowestLocalNondeterminismVariable(std::max(result.getLowestLocalNondeterminismVariable(), actionDd.getLowestLocalNondeterminismVariable()));
                    result.setHighestLocalNondeterminismVariable(std::max(result.getHighestLocalNondeterminismVariable(), actionDd.getHighestLocalNondeterminismVariable()));
                }
                
                for (uint64_t locationIndex = 0; locationIndex < automaton.getNumberOfLocations(); ++locationIndex) {
                    auto const& location = automaton.getLocation(locationIndex);
                    performTransientAssignments(location.getAssignments().getTransientAssignments(), [this,&automatonName,locationIndex,&result] (storm::jani::Assignment const& assignment) {
                        storm::dd::Add<Type, ValueType> assignedValues = this->variables.manager->getEncoding(this->variables.automatonToLocationVariableMap.at(automatonName).first, locationIndex).template toAdd<ValueType>() * this->variables.rowExpressionAdapter->translateExpression(assignment.getAssignedExpression());
                        auto it = result.transientLocationAssignments.find(assignment.getExpressionVariable());
                        if (it != result.transientLocationAssignments.end()) {
                            it->second += assignedValues;
                        } else {
                            result.transientLocationAssignments[assignment.getExpressionVariable()] = assignedValues;
                        }
                    });
                }
                
                return result;
            }

            void addMissingGlobalVariableIdentities(ActionDd& action) {
                // Build a DD that we can multiply to the transitions and adds all missing global variable identities that way.
                storm::dd::Add<Type, ValueType> missingIdentities = this->variables.manager->template getAddOne<ValueType>();
                
                for (auto const& variable : this->variables.allGlobalVariables) {
                    auto it = action.variableToWritingFragment.find(variable);
                    if (it != action.variableToWritingFragment.end()) {
                        missingIdentities *= (it->second).ite(this->variables.manager->template getAddOne<ValueType>(), this->variables.variableToIdentityMap.at(variable));
                    } else {
                        missingIdentities *= this->variables.variableToIdentityMap.at(variable);
                    }
                }
                
                action.transitions *= missingIdentities;
            }
            
            ComposerResult<Type, ValueType> buildSystemFromAutomaton(AutomatonDd& automaton) {
                // If the model is an MDP, we need to encode the nondeterminism using additional variables.
                if (this->model.getModelType() == storm::jani::ModelType::MDP) {
                    storm::dd::Add<Type, ValueType> result = this->variables.manager->template getAddZero<ValueType>();
                    storm::dd::Bdd<Type> illegalFragment = this->variables.manager->getBddZero();
                    
                    // First, determine the highest number of nondeterminism variables that is used in any action and make
                    // all actions use the same amout of nondeterminism variables.
                    uint64_t numberOfUsedNondeterminismVariables = automaton.getHighestLocalNondeterminismVariable();
                    
                    // Add missing global variable identities, action and nondeterminism encodings.
                    std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> transientEdgeAssignments;
                    for (auto& action : automaton.actionIndexToAction) {
                        illegalFragment |= action.second.illegalFragment;
                        addMissingGlobalVariableIdentities(action.second);
                        storm::dd::Add<Type, ValueType> actionEncoding = encodeAction(action.first != this->model.getSilentActionIndex() ? boost::optional<uint64_t>(action.first) : boost::none, this->variables);
                        storm::dd::Add<Type, ValueType> missingNondeterminismEncoding = encodeIndex(0, action.second.getHighestLocalNondeterminismVariable(), numberOfUsedNondeterminismVariables - action.second.getHighestLocalNondeterminismVariable(), this->variables);
                        storm::dd::Add<Type, ValueType> extendedTransitions = actionEncoding * missingNondeterminismEncoding * action.second.transitions;
                        
                        for (auto const& transientAssignment : action.second.transientEdgeAssignments) {
                            addToTransientAssignmentMap(transientEdgeAssignments, transientAssignment.first, actionEncoding * missingNondeterminismEncoding * transientAssignment.second);
                        }
                        
                        result += extendedTransitions;
                    }
                    
                    return ComposerResult<Type, ValueType>(result, automaton.transientLocationAssignments, transientEdgeAssignments, illegalFragment, numberOfUsedNondeterminismVariables);
                } else if (this->model.getModelType() == storm::jani::ModelType::DTMC || this->model.getModelType() == storm::jani::ModelType::CTMC) {
                    // Simply add all actions, but make sure to include the missing global variable identities.

                    storm::dd::Add<Type, ValueType> result = this->variables.manager->template getAddZero<ValueType>();
                    storm::dd::Bdd<Type> illegalFragment = this->variables.manager->getBddZero();
                    std::map<storm::expressions::Variable, storm::dd::Add<Type, ValueType>> transientEdgeAssignments;
                    for (auto& action : automaton.actionIndexToAction) {
                        illegalFragment |= action.second.illegalFragment;
                        addMissingGlobalVariableIdentities(action.second);
                        addToTransientAssignmentMap(transientEdgeAssignments, action.second.transientEdgeAssignments);
                        result += action.second.transitions;
                    }

                    return ComposerResult<Type, ValueType>(result, automaton.transientLocationAssignments, transientEdgeAssignments, illegalFragment, 0);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal model type.");
                }
            }
        };
        
        template <storm::dd::DdType Type, typename ValueType>
        struct ModelComponents {
            storm::dd::Bdd<Type> reachableStates;
            storm::dd::Bdd<Type> initialStates;
            storm::dd::Bdd<Type> deadlockStates;
            storm::dd::Add<Type, ValueType> transitionMatrix;
            std::unordered_map<std::string, storm::models::symbolic::StandardRewardModel<Type, ValueType>> rewardModels;
        };
        
        template <storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>> createModel(storm::jani::ModelType const& modelType, CompositionVariables<Type, ValueType> const& variables, ModelComponents<Type, ValueType> const& modelComponents) {
            if (modelType == storm::jani::ModelType::DTMC) {
                return std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>>(new storm::models::symbolic::Dtmc<Type, ValueType>(variables.manager, modelComponents.reachableStates, modelComponents.initialStates, modelComponents.deadlockStates, modelComponents.transitionMatrix, variables.rowMetaVariables, variables.rowExpressionAdapter, variables.columnMetaVariables, variables.columnExpressionAdapter, variables.rowColumnMetaVariablePairs, std::map<std::string, storm::expressions::Expression>(), modelComponents.rewardModels));
            } else if (modelType == storm::jani::ModelType::CTMC) {
                return std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>>(new storm::models::symbolic::Ctmc<Type, ValueType>(variables.manager, modelComponents.reachableStates, modelComponents.initialStates, modelComponents.deadlockStates, modelComponents.transitionMatrix, variables.rowMetaVariables, variables.rowExpressionAdapter, variables.columnMetaVariables, variables.columnExpressionAdapter, variables.rowColumnMetaVariablePairs, std::map<std::string, storm::expressions::Expression>(), modelComponents.rewardModels));
            } else if (modelType == storm::jani::ModelType::MDP) {
                return std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>>(new storm::models::symbolic::Mdp<Type, ValueType>(variables.manager, modelComponents.reachableStates, modelComponents.initialStates, modelComponents.deadlockStates, modelComponents.transitionMatrix, variables.rowMetaVariables, variables.rowExpressionAdapter, variables.columnMetaVariables, variables.columnExpressionAdapter, variables.rowColumnMetaVariablePairs, variables.allNondeterminismVariables, std::map<std::string, storm::expressions::Expression>(), modelComponents.rewardModels));
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid model type.");
            }
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        void postprocessVariables(storm::jani::ModelType const& modelType, ComposerResult<Type, ValueType>& system, CompositionVariables<Type, ValueType>& variables) {
            // Add all action/row/column variables to the DD. If we omitted multiplying edges in the construction, this will
            // introduce the variables so they can later be abstracted without raising an error.
            system.transitions.addMetaVariables(variables.rowMetaVariables);
            system.transitions.addMetaVariables(variables.columnMetaVariables);
            
            // If the model is an MDP, we also add all action variables.
            if (modelType == storm::jani::ModelType::MDP) {
                for (auto const& actionVariablePair : variables.actionVariablesMap) {
                    system.transitions.addMetaVariable(actionVariablePair.second);
                }
            }
            
            // Get rid of the local nondeterminism variables that were not used.
            for (uint64_t index = system.numberOfNondeterminismVariables; index < variables.localNondeterminismVariables.size(); ++index) {
                variables.allNondeterminismVariables.erase(variables.localNondeterminismVariables[index]);
            }
            variables.localNondeterminismVariables.resize(system.numberOfNondeterminismVariables);
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        storm::dd::Bdd<Type> postprocessSystem(storm::jani::Model const& model, ComposerResult<Type, ValueType>& system, CompositionVariables<Type, ValueType> const& variables, typename DdJaniModelBuilder<Type, ValueType>::Options const& options) {
            // For DTMCs, we normalize each row to 1 (to account for non-determinism).
            if (model.getModelType() == storm::jani::ModelType::DTMC) {
                storm::dd::Add<Type, ValueType> stateToNumberOfChoices = system.transitions.sumAbstract(variables.columnMetaVariables);
                system.transitions = system.transitions / stateToNumberOfChoices;
                
                // Scale all state-action rewards.
                for (auto& entry : system.transientEdgeAssignments) {
                    entry.second = entry.second / stateToNumberOfChoices;
                }
            }
            
            // If we were asked to treat some states as terminal states, we cut away their transitions now.
            if (options.terminalStates || options.negatedTerminalStates) {
                std::map<storm::expressions::Variable, storm::expressions::Expression> constantsSubstitution = model.getConstantsSubstitution();
                
                storm::dd::Bdd<Type> terminalStatesBdd = variables.manager->getBddZero();
                if (options.terminalStates) {
                    storm::expressions::Expression terminalExpression = options.terminalStates.get().substitute(constantsSubstitution);
                    STORM_LOG_TRACE("Making the states satisfying " << terminalExpression << " terminal.");
                    terminalStatesBdd = variables.rowExpressionAdapter->translateExpression(terminalExpression).toBdd();
                }
                if (options.negatedTerminalStates) {
                    storm::expressions::Expression negatedTerminalExpression = options.negatedTerminalStates.get().substitute(constantsSubstitution);
                    STORM_LOG_TRACE("Making the states *not* satisfying " << negatedTerminalExpression << " terminal.");
                    terminalStatesBdd |= !variables.rowExpressionAdapter->translateExpression(negatedTerminalExpression).toBdd();
                }
                
                system.transitions *= (!terminalStatesBdd).template toAdd<ValueType>();
                return terminalStatesBdd;
            }
            return variables.manager->getBddZero();
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        storm::dd::Bdd<Type> computeInitialStates(storm::jani::Model const& model, CompositionVariables<Type, ValueType> const& variables) {
            storm::dd::Bdd<Type> initialStates = variables.rowExpressionAdapter->translateExpression(model.getInitialStatesExpression(true)).toBdd();
            for (auto const& automaton : model.getAutomata()) {
                storm::dd::Bdd<Type> initialLocationIndices = variables.manager->getBddZero();
                for (auto const& locationIndex : automaton.getInitialLocationIndices()) {
                    initialLocationIndices |= variables.manager->getEncoding(variables.automatonToLocationVariableMap.at(automaton.getName()).first, locationIndex);
                }
                initialStates &= initialLocationIndices;
            }
            for (auto const& metaVariable : variables.rowMetaVariables) {
                initialStates &= variables.variableToRangeMap.at(metaVariable);
            }
            return initialStates;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        storm::dd::Bdd<Type> fixDeadlocks(storm::jani::ModelType const& modelType, storm::dd::Add<Type, ValueType>& transitionMatrix, storm::dd::Bdd<Type> const& transitionMatrixBdd, storm::dd::Bdd<Type> const& reachableStates, CompositionVariables<Type, ValueType> const& variables) {
            // Detect deadlocks and 1) fix them if requested 2) throw an error otherwise.
            storm::dd::Bdd<Type> statesWithTransition = transitionMatrixBdd.existsAbstract(variables.columnMetaVariables);
            storm::dd::Bdd<Type> deadlockStates = reachableStates && !statesWithTransition;
            
            if (!deadlockStates.isZero()) {
                // If we need to fix deadlocks, we do so now.
                if (!storm::settings::getModule<storm::settings::modules::CoreSettings>().isDontFixDeadlocksSet()) {
                    STORM_LOG_INFO("Fixing deadlocks in " << deadlockStates.getNonZeroCount() << " states. The first three of these states are: ");
                    
                    storm::dd::Add<Type, ValueType> deadlockStatesAdd = deadlockStates.template toAdd<ValueType>();
                    uint_fast64_t count = 0;
                    for (auto it = deadlockStatesAdd.begin(), ite = deadlockStatesAdd.end(); it != ite && count < 3; ++it, ++count) {
                        STORM_LOG_INFO((*it).first.toPrettyString(variables.rowMetaVariables) << std::endl);
                    }
                    
                    // Create a global identity DD.
                    storm::dd::Add<Type, ValueType> globalIdentity = variables.manager->template getAddOne<ValueType>();
                    for (auto const& identity : variables.automatonToIdentityMap) {
                        globalIdentity *= identity.second;
                    }
                    for (auto const& variable : variables.allGlobalVariables) {
                        globalIdentity *= variables.variableToIdentityMap.at(variable);
                    }
                    
                    if (modelType == storm::jani::ModelType::DTMC || modelType == storm::jani::ModelType::CTMC) {
                        // For DTMCs, we can simply add the identity of the global module for all deadlock states.
                        transitionMatrix += deadlockStatesAdd * globalIdentity;
                    } else if (modelType == storm::jani::ModelType::MDP) {
                        // For MDPs, however, we need to select an action associated with the self-loop, if we do not
                        // want to attach a lot of self-loops to the deadlock states.
                        storm::dd::Add<Type, ValueType> action = variables.manager->template getAddOne<ValueType>();
                        for (auto const& variable : variables.actionVariablesMap) {
                            action *= variables.manager->template getIdentity<ValueType>(variable.second);
                        }
                        for (auto const& variable : variables.localNondeterminismVariables) {
                            action *= variables.manager->template getIdentity<ValueType>(variable);
                        }
                        transitionMatrix += deadlockStatesAdd * globalIdentity * action;
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The model contains " << deadlockStates.getNonZeroCount() << " deadlock states. Please unset the option to not fix deadlocks, if you want to fix them automatically.");
                }
            }
            return deadlockStates;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        std::vector<storm::expressions::Variable> selectRewardVariables(storm::jani::Model const& model, typename DdJaniModelBuilder<Type, ValueType>::Options const& options) {
            std::vector<storm::expressions::Variable> result;
            if (options.isBuildAllRewardModelsSet()) {
                for (auto const& variable : model.getGlobalVariables()) {
                    if (variable.isTransient()) {
                        result.push_back(variable.getExpressionVariable());
                    }
                }
            } else {
                auto const& globalVariables = model.getGlobalVariables();
                for (auto const& rewardModelName : options.getRewardModelNames()) {
                    if (globalVariables.hasVariable(rewardModelName)) {
                        result.push_back(globalVariables.getVariable(rewardModelName).getExpressionVariable());
                    } else {
                        STORM_LOG_THROW(rewardModelName.empty(), storm::exceptions::InvalidArgumentException, "Cannot build unknown reward model '" << rewardModelName << "'.");
                        STORM_LOG_THROW(globalVariables.getNumberOfTransientVariables() == 1, storm::exceptions::InvalidArgumentException, "Reference to standard reward model is ambiguous.");
                    }
                }
                
                // If no reward model was yet added, but there was one that was given in the options, we try to build the
                // standard reward model.
                if (result.empty() && !options.getRewardModelNames().empty()) {
                    result.push_back(globalVariables.getTransientVariables().front()->getExpressionVariable());
                }
            }
            
            return result;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        std::unordered_map<std::string, storm::models::symbolic::StandardRewardModel<Type, ValueType>> buildRewardModels(ComposerResult<Type, ValueType> const& system, std::vector<storm::expressions::Variable> const& rewardVariables) {
            std::unordered_map<std::string, storm::models::symbolic::StandardRewardModel<Type, ValueType>> result;
            
            for (auto const& variable : rewardVariables) {
                boost::optional<storm::dd::Add<Type, ValueType>> stateRewards = boost::none;
                boost::optional<storm::dd::Add<Type, ValueType>> stateActionRewards = boost::none;
                boost::optional<storm::dd::Add<Type, ValueType>> transitionRewards = boost::none;
                
                auto it = system.transientLocationAssignments.find(variable);
                if (it != system.transientLocationAssignments.end()) {
                    stateRewards = it->second;
                }
                
                it = system.transientEdgeAssignments.find(variable);
                if (it != system.transientEdgeAssignments.end()) {
                    stateActionRewards = it->second;
                }
                
                result.emplace(variable.getName(), storm::models::symbolic::StandardRewardModel<Type, ValueType>(stateRewards, stateActionRewards, transitionRewards));
            }
            
            return result;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>> DdJaniModelBuilder<Type, ValueType>::build(storm::jani::Model const& model, Options const& options) {
            if (model.hasUndefinedConstants()) {
                std::vector<std::reference_wrapper<storm::jani::Constant const>> undefinedConstants = model.getUndefinedConstants();
                std::vector<std::string> strings;
                for (auto const& constant : undefinedConstants) {
                    std::stringstream stream;
                    stream << constant.get().getName() << " (" << constant.get().getType() << ")";
                    strings.push_back(stream.str());
                }
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Model still contains these undefined constants: " << boost::join(strings, ", ") << ".");
            }
            
            // Determine the actions that will appear in the parallel composition.
            storm::jani::CompositionActionInformationVisitor actionInformationVisitor(model);
            storm::jani::ActionInformation actionInformation = actionInformationVisitor.getActionInformation();
            
            // Create all necessary variables.
            CompositionVariableCreator<Type, ValueType> variableCreator(model, actionInformation);
            CompositionVariables<Type, ValueType> variables = variableCreator.create();
            
            // Determine which transient assignments need to be considered in the building process.
            std::vector<storm::expressions::Variable> rewardVariables = selectRewardVariables<Type, ValueType>(model, options);
            
            // Create a builder to compose and build the model.
            CombinedEdgesSystemComposer<Type, ValueType> composer(model, actionInformation, variables, rewardVariables);
            ComposerResult<Type, ValueType> system = composer.compose();
            
            // Postprocess the variables in place.
            postprocessVariables(model.getModelType(), system, variables);
            
            // Postprocess the system in place and get the states that were terminal (i.e. whose transitions were cut off).
            storm::dd::Bdd<Type> terminalStates = postprocessSystem(model, system, variables, options);
            
            // Start creating the model components.
            ModelComponents<Type, ValueType> modelComponents;
            
            // Build initial states.
            modelComponents.initialStates = computeInitialStates(model, variables);
            
            // Perform reachability analysis to obtain reachable states.
            storm::dd::Bdd<Type> transitionMatrixBdd = system.transitions.notZero();
            if (model.getModelType() == storm::jani::ModelType::MDP) {
                transitionMatrixBdd = transitionMatrixBdd.existsAbstract(variables.allNondeterminismVariables);
            }
            modelComponents.reachableStates = storm::utility::dd::computeReachableStates(modelComponents.initialStates, transitionMatrixBdd, variables.rowMetaVariables, variables.columnMetaVariables);
            
            // Check that the reachable fragment does not overlap with the illegal fragment.
            storm::dd::Bdd<Type> reachableIllegalFragment = modelComponents.reachableStates && system.illegalFragment;
            STORM_LOG_THROW(reachableIllegalFragment.isZero(), storm::exceptions::WrongFormatException, "There are reachable states in the model that have synchronizing edges enabled that write the same global variable.");
            
            // Cut transitions to reachable states.
            storm::dd::Add<Type, ValueType> reachableStatesAdd = modelComponents.reachableStates.template toAdd<ValueType>();
            modelComponents.transitionMatrix = system.transitions * reachableStatesAdd;
            
            // Fix deadlocks if existing.
            modelComponents.deadlockStates = fixDeadlocks(model.getModelType(), modelComponents.transitionMatrix, transitionMatrixBdd, modelComponents.reachableStates, variables);
            
            // Cut the deadlock states by removing all states that we 'converted' to deadlock states by making them terminal.
            modelComponents.deadlockStates = modelComponents.deadlockStates && !terminalStates;
            
            // Build the reward models.
            modelComponents.rewardModels = buildRewardModels(system, rewardVariables);
            
            // Finally, create the model.
            return createModel(model.getModelType(), variables, modelComponents);
        }
        
        template class DdJaniModelBuilder<storm::dd::DdType::CUDD, double>;
        template class DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>;
    }
}
