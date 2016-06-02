#include "src/builder/DdJaniModelBuilder.h"

#include <sstream>

#include <boost/algorithm/string/join.hpp>

#include "src/logic/Formulas.h"

#include "src/storage/jani/Model.h"
#include "src/storage/jani/RenameComposition.h"
#include "src/storage/jani/AutomatonComposition.h"
#include "src/storage/jani/ParallelComposition.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"
#include "src/adapters/AddExpressionAdapter.h"

#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Ctmc.h"
#include "src/models/symbolic/Mdp.h"
#include "src/models/symbolic/StandardRewardModel.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/MarkovChainSettings.h"

#include "src/utility/macros.h"
#include "src/utility/jani.h"
#include "src/utility/dd.h"
#include "src/exceptions/InvalidArgumentException.h"

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
        void DdJaniModelBuilder<Type, ValueType>::Options::addConstantDefinitionsFromString(storm::jani::Model const& model, std::string const& constantDefinitionString) {
            std::map<storm::expressions::Variable, storm::expressions::Expression> newConstantDefinitions = storm::utility::jani::parseConstantDefinitionString(model, constantDefinitionString);
            
            // If there is at least one constant that is defined, and the constant definition map does not yet exist,
            // we need to create it.
            if (!constantDefinitions && !newConstantDefinitions.empty()) {
                constantDefinitions = std::map<storm::expressions::Variable, storm::expressions::Expression>();
            }
            
            // Now insert all the entries that need to be defined.
            for (auto const& entry : newConstantDefinitions) {
                constantDefinitions.get().insert(entry);
            }
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        DdJaniModelBuilder<Type, ValueType>::DdJaniModelBuilder(storm::jani::Model const& model, Options const& options) : options(options) {
            if (options.constantDefinitions) {
                this->model = model.defineUndefinedConstants(options.constantDefinitions.get());
            } else {
                this->model = model;
            }
            
            if (this->model->hasUndefinedConstants()) {
                std::vector<std::reference_wrapper<storm::jani::Constant const>> undefinedConstants = this->model->getUndefinedConstants();
                std::vector<std::string> strings;
                for (auto const& constant : undefinedConstants) {
                    std::stringstream stream;
                    stream << constant.get().getName() << " (" << constant.get().getType() << ")";
                    strings.push_back(stream.str());
                }
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Program still contains these undefined constants: " << boost::join(strings, ", ") << ".");
            }
            
            this->model = this->model->substituteConstants();
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
            CompositionVariableCreator(storm::jani::Model const& model) : model(model) {
                // Intentionally left empty.
            }
            
            CompositionVariables<Type, ValueType> create() {
                // First, check whether every automaton appears exactly once in the system composition.
                automata.clear();
                this->model.getSystemComposition().accept(*this, boost::none);
                STORM_LOG_THROW(automata.size() == this->model.getNumberOfAutomata(), storm::exceptions::InvalidArgumentException, "Cannot build symbolic model from JANI model whose system composition refers to a subset of automata.");
                
                // Then, check that the model does not contain unbounded integer variables.
                STORM_LOG_THROW(!this->model.getGlobalVariables().containsUnboundedIntegerVariables(), storm::exceptions::InvalidArgumentException, "Cannot build symbolic model from JANI model that contains global unbounded integer variables.");
                for (auto const& automaton : this->model.getAutomata()) {
                    STORM_LOG_THROW(!automaton.getVariables().containsUnboundedIntegerVariables(), storm::exceptions::InvalidArgumentException, "Cannot build symbolic model from JANI model that contains unbounded integer variables in automaton '" << automaton.getName() << "'.");
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
                composition.getSubcomposition().accept(*this, boost::none);
                return boost::none;
            }
            
            boost::any visit(storm::jani::ParallelComposition const& composition, boost::any const& data) override {
                composition.getLeftSubcomposition().accept(*this, boost::none);
                composition.getRightSubcomposition().accept(*this, boost::none);
                return boost::none;
            }
            
        private:
            CompositionVariables<Type, ValueType> createVariables() {
                CompositionVariables<Type, ValueType> result;
                
                for (auto const& automaton : this->model.getAutomata()) {
                    // Start by creating a meta variable for the location of the automaton.
                    std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = result.manager->addMetaVariable("l_" + automaton.getName(), 0, automaton.getNumberOfLocations() - 1);
                    result.automatonToLocationVariableMap[automaton.getName()] = variablePair;
                    
                    // Add the location variable to the row/column variables.
                    result.rowMetaVariables.insert(variablePair.first);
                    result.columnMetaVariables.insert(variablePair.second);
                }
                
                for (auto const& action : this->model.getActions()) {
                    if (this->model.getActionIndex(action.getName()) != this->model.getSilentActionIndex()) {
                        std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = result.manager->addMetaVariable(action.getName());
                        result.actionVariablesMap[this->model.getActionIndex(action.getName())] = variablePair.first;
                        result.allNondeterminismVariables.insert(variablePair.first);
                    }
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
                
                // Create global variables.
                storm::dd::Bdd<Type> globalVariableRanges = result.manager->getBddOne();
                for (auto const& variable : this->model.getGlobalVariables()) {
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
                
                result.rowColumnMetaVariablePairs.push_back(variablePair);
                result.allGlobalVariables.insert(variable.getExpressionVariable());
            }
            
            storm::jani::Model const& model;
            std::set<std::string> automata;
        };
        
        template<storm::dd::DdType Type, typename ValueType>
        struct EdgeDestinationDd {
            EdgeDestinationDd(storm::dd::Add<Type, ValueType> const& transitionsDd, std::set<storm::expressions::Variable> const& writtenGlobalVariables = {}) : transitionsDd(transitionsDd), writtenGlobalVariables(writtenGlobalVariables) {
                // Intentionally left empty.
            }
            
            storm::dd::Add<Type, ValueType> transitionsDd;
            std::set<storm::expressions::Variable> writtenGlobalVariables;
        };
        
        // This structure represents an edge.
        template<storm::dd::DdType Type, typename ValueType>
        struct EdgeDd {
            EdgeDd(storm::dd::Add<Type> const& guardDd = storm::dd::Add<Type>(), storm::dd::Add<Type, ValueType> const& transitionsDd = storm::dd::Add<Type, ValueType>(), std::set<storm::expressions::Variable> const& writtenGlobalVariables = {}, std::set<storm::expressions::Variable> const& globalVariablesWrittenMultipleTimes = {}) : guardDd(guardDd), transitionsDd(transitionsDd), writtenGlobalVariables(writtenGlobalVariables), globalVariablesWrittenMultipleTimes(globalVariablesWrittenMultipleTimes) {
                // Intentionally left empty.
            }
            
            EdgeDd(EdgeDd const& other) : guardDd(other.guardDd), transitionsDd(other.transitionsDd), writtenGlobalVariables(other.writtenGlobalVariables), globalVariablesWrittenMultipleTimes(other.globalVariablesWrittenMultipleTimes) {
                // Intentionally left empty.
            }
            
            EdgeDd& operator=(EdgeDd const& other) {
                if (this != &other) {
                    globalVariablesWrittenMultipleTimes = other.globalVariablesWrittenMultipleTimes;
                    writtenGlobalVariables = other.writtenGlobalVariables;
                    guardDd = other.guardDd;
                    transitionsDd = other.transitionsDd;
                }
                return *this;
            }
            
            storm::dd::Add<Type, ValueType> guardDd;
            storm::dd::Add<Type, ValueType> transitionsDd;
            std::set<storm::expressions::Variable> writtenGlobalVariables;
            std::set<storm::expressions::Variable> globalVariablesWrittenMultipleTimes;
        };
        
        // This structure represents a subcomponent of a composition.
        template<storm::dd::DdType Type, typename ValueType>
        struct AutomatonDd {
            AutomatonDd(storm::dd::Add<Type, ValueType> const& identity) : identity(identity) {
                // Intentionally left empty.
            }
            
            std::map<uint64_t, std::vector<EdgeDd<Type, ValueType>>> actionIndexToEdges;
            storm::dd::Add<Type, ValueType> identity;
        };
        
        template <storm::dd::DdType Type, typename ValueType>
        EdgeDd<Type, ValueType> extendEdgeWithIdentity(EdgeDd<Type, ValueType> const& edge, storm::dd::Add<Type, ValueType> const& identity) {
            EdgeDd<Type, ValueType> result(edge);
            result.transitionsDd *= identity;
            return result;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        EdgeDd<Type, ValueType> composeEdgesInParallel(EdgeDd<Type, ValueType> const& edge1, EdgeDd<Type, ValueType> const& edge2) {
            EdgeDd<Type, ValueType> result;

            // Compose the guards.
            result.guardDd = edge1.guardDd * edge2.guardDd;
            
            // If the composed guard is already zero, we can immediately return an empty result.
            if (result.guardDd.isZero()) {
                result.transitionsDd = edge1.transitionsDd.getDdManager().template getAddZero<ValueType>();
            }
            
            // Compute the set of variables written multiple times by the composition.
            std::set<storm::expressions::Variable> oldVariablesWrittenMultipleTimes;
            std::set_union(edge1.globalVariablesWrittenMultipleTimes.begin(), edge1.globalVariablesWrittenMultipleTimes.end(), edge2.globalVariablesWrittenMultipleTimes.begin(), edge2.globalVariablesWrittenMultipleTimes.end(), std::inserter(oldVariablesWrittenMultipleTimes, oldVariablesWrittenMultipleTimes.begin()));
            
            std::set<storm::expressions::Variable> newVariablesWrittenMultipleTimes;
            std::set_intersection(edge1.writtenGlobalVariables.begin(), edge1.writtenGlobalVariables.end(), edge2.writtenGlobalVariables.begin(), edge2.writtenGlobalVariables.end(), std::inserter(newVariablesWrittenMultipleTimes, newVariablesWrittenMultipleTimes.begin()));
            
            std::set<storm::expressions::Variable> variablesWrittenMultipleTimes;
            std::set_union(oldVariablesWrittenMultipleTimes.begin(), oldVariablesWrittenMultipleTimes.end(), newVariablesWrittenMultipleTimes.begin(), newVariablesWrittenMultipleTimes.end(), std::inserter(variablesWrittenMultipleTimes, variablesWrittenMultipleTimes.begin()));
            
            result.globalVariablesWrittenMultipleTimes = std::move(variablesWrittenMultipleTimes);
            
            // Compute the set of variables written by the composition.
            std::set<storm::expressions::Variable> variablesWritten;
            std::set_union(edge1.writtenGlobalVariables.begin(), edge1.writtenGlobalVariables.end(), edge2.writtenGlobalVariables.begin(), edge2.writtenGlobalVariables.end(), std::inserter(variablesWritten, variablesWritten.begin()));
            
            result.writtenGlobalVariables = variablesWritten;
            
            // Compose the guards.
            result.guardDd = edge1.guardDd * edge2.guardDd;
            
            // Compose the transitions.
            result.transitionsDd = edge1.transitionsDd * edge2.transitionsDd;
            
            return result;
        }
        
        // A class that is responsible for performing the actual composition.
        template <storm::dd::DdType Type, typename ValueType>
        class AutomatonComposer : public storm::jani::CompositionVisitor {
        public:
            AutomatonComposer(storm::jani::Model const& model, CompositionVariables<Type, ValueType> const& variables) : model(model), variables(variables) {
                // Intentionally left empty.
            }
            
            AutomatonDd<Type, ValueType> compose() {
                return boost::any_cast<AutomatonDd<Type, ValueType>>(this->model.getSystemComposition().accept(*this, boost::none));
            }
            
            boost::any visit(storm::jani::AutomatonComposition const& composition, boost::any const& data) override {
                return buildAutomatonDd(composition.getAutomatonName());
            }
            
            boost::any visit(storm::jani::RenameComposition const& composition, boost::any const& data) override {
                AutomatonDd<Type, ValueType> subautomaton = boost::any_cast<AutomatonDd<Type, ValueType>>(composition.getSubcomposition().accept(*this, boost::none));
                
                // Build a mapping from indices to indices for the renaming.
                std::map<uint64_t, uint64_t> renamingIndexToIndex;
                for (auto const& entry : composition.getRenaming()) {
                    if (this->model.getActionIndex(entry.first) != this->model.getSilentActionIndex()) {
                        // Distinguish the cases where we hide the action or properly rename it.
                        if (entry.second) {
                            renamingIndexToIndex.emplace(this->model.getActionIndex(entry.first), this->model.getActionIndex(entry.second.get()));
                        } else {
                            renamingIndexToIndex.emplace(this->model.getActionIndex(entry.first), this->model.getSilentActionIndex());
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Renaming composition must not rename the silent action.");
                    }
                }
                
                // Finally, apply the renaming.
                AutomatonDd<Type, ValueType> result(subautomaton.identity);
                for (auto const& actionEdges : subautomaton.actionIndexToEdges) {
                    auto it = renamingIndexToIndex.find(actionEdges.first);
                    if (it != renamingIndexToIndex.end()) {
                        // If we are to rename the action, do so.
                        result.actionIndexToEdges[it->second].insert(result.actionIndexToEdges[it->second].end(), actionEdges.second.begin(), actionEdges.second.end());
                    } else {
                        // Otherwise copy over the edges.
                        result.actionIndexToEdges[it->first].insert(result.actionIndexToEdges[it->first].begin(), actionEdges.second.begin(), actionEdges.second.end());
                    }
                }
                return result;
            }
            
            boost::any visit(storm::jani::ParallelComposition const& composition, boost::any const& data) override {
                AutomatonDd<Type, ValueType> leftSubautomaton = boost::any_cast<AutomatonDd<Type, ValueType>>(composition.getLeftSubcomposition().accept(*this, boost::none));
                AutomatonDd<Type, ValueType> rightSubautomaton = boost::any_cast<AutomatonDd<Type, ValueType>>(composition.getRightSubcomposition().accept(*this, boost::none));
                
                // Build the set of synchronizing action indices.
                std::set<uint64_t> synchronizingActionIndices;
                for (auto const& entry : composition.getSynchronizationAlphabet()) {
                    if (this->model.getActionIndex(entry) != this->model.getSilentActionIndex()) {
                        synchronizingActionIndices.insert(this->model.getActionIndex(entry));
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Parallel composition must not synchronize over silent action.");
                    }
                }
                
                // Perform the composition.
                
                // First, consider all actions in the left subcomposition.
                AutomatonDd<Type, ValueType> result(leftSubautomaton.identity * rightSubautomaton.identity);
                uint64_t index = 0;
                for (auto const& actionEdges : leftSubautomaton.actionIndexToEdges) {
                    // If we need to synchronize over this action, do so now.
                    if (synchronizingActionIndices.find(actionEdges.first) != synchronizingActionIndices.end()) {
                        auto rightIt = rightSubautomaton.actionIndexToEdges.find(actionEdges.first);
                        if (rightIt != rightSubautomaton.actionIndexToEdges.end()) {
                            for (auto const& edge1 : actionEdges.second) {
                                for (auto const& edge2 : rightIt->second) {
                                    EdgeDd<Type, ValueType> edgeDd = composeEdgesInParallel(edge1, edge2);
                                    if (!edgeDd.guardDd.isZero()) {
                                        result.actionIndexToEdges[actionEdges.first].push_back(edgeDd);
                                    }
                                    index++;
                                }
                            }
                        }
                    } else {
                        // Extend all edges by the missing identity (unsynchronizing) and copy them over.
                        for (auto const& edge : actionEdges.second) {
                            result.actionIndexToEdges[actionEdges.first].push_back(extendEdgeWithIdentity(edge, rightSubautomaton.identity));
                        }
                    }
                }
                
                // Then, consider all actions in the right subcomposition. All synchronizing actions can be ignored, because
                // we would have dealt with them earlier if there was a suitable synchronization partner. Since there is none,
                // such transitions can not be taken and we can drop them.
                for (auto const& actionEdges : rightSubautomaton.actionIndexToEdges) {
                    if (synchronizingActionIndices.find(actionEdges.first) == synchronizingActionIndices.end()) {
                        for (auto const& edge : actionEdges.second) {
                            result.actionIndexToEdges[actionEdges.first].push_back(extendEdgeWithIdentity(edge, leftSubautomaton.identity));
                        }
                    }
                }
                
                return result;
            }
            
        private:
            EdgeDestinationDd<Type, ValueType> buildEdgeDestinationDd(storm::jani::Automaton const& automaton, storm::jani::EdgeDestination const& destination, storm::dd::Add<Type, ValueType> const& guard) {
                storm::dd::Add<Type, ValueType> transitionsDd = variables.manager->template getAddOne<ValueType>();
                
                STORM_LOG_TRACE("Translating edge destination.");
                
                // Iterate over all assignments (boolean and integer) and build the DD for it.
                std::set<storm::expressions::Variable> assignedVariables;
                for (auto const& assignment : destination.getAssignments()) {
                    // Record the variable as being written.
                    STORM_LOG_TRACE("Assigning to variable " << variables.variableToRowMetaVariableMap->at(assignment.getExpressionVariable()).getName());
                    assignedVariables.insert(assignment.getExpressionVariable());
                    
                    // Translate the written variable.
                    auto const& primedMetaVariable = variables.variableToColumnMetaVariableMap->at(assignment.getExpressionVariable());
                    storm::dd::Add<Type, ValueType> writtenVariable = variables.manager->template getIdentity<ValueType>(primedMetaVariable);
                    
                    // Translate the expression that is being assigned.
                    storm::dd::Add<Type, ValueType> updateExpression = variables.rowExpressionAdapter->translateExpression(assignment.getAssignedExpression());
                    
                    // Combine the update expression with the guard.
                    storm::dd::Add<Type, ValueType> result = updateExpression * guard;

                    // Combine the variable and the assigned expression.
                    result = result.equals(writtenVariable).template toAdd<ValueType>();
                    result *= guard;
                    
                    // Restrict the transitions to the range of the written variable.
                    result = result * variables.manager->getRange(primedMetaVariable).template toAdd<ValueType>();
                    
                    // Combine the assignment DDs.
                    transitionsDd *= result;
                }
                
                // Compute the set of assigned global variables.
                std::set<storm::expressions::Variable> assignedGlobalVariables;
                std::set_intersection(assignedVariables.begin(), assignedVariables.end(), variables.allGlobalVariables.begin(), variables.allGlobalVariables.end(), std::inserter(assignedGlobalVariables, assignedGlobalVariables.begin()));
                
                // All unassigned boolean variables need to keep their value.
                for (storm::jani::BooleanVariable const& variable : automaton.getVariables().getBooleanVariables()) {
                    if (assignedVariables.find(variable.getExpressionVariable()) == assignedVariables.end()) {
                        STORM_LOG_TRACE("Multiplying identity of variable " << variable.getName());
                        transitionsDd *= variables.variableToIdentityMap.at(variable.getExpressionVariable());
                    }
                }
                
                // All unassigned integer variables need to keep their value.
                for (storm::jani::BoundedIntegerVariable const& variable : automaton.getVariables().getBoundedIntegerVariables()) {
                    if (assignedVariables.find(variable.getExpressionVariable()) == assignedVariables.end()) {
                        STORM_LOG_TRACE("Multiplying identity of variable " << variable.getName());
                        transitionsDd *= variables.variableToIdentityMap.at(variable.getExpressionVariable());
                    }
                }
                
                return EdgeDestinationDd<Type, ValueType>(variables.manager->getEncoding(variables.automatonToLocationVariableMap.at(automaton.getName()).second, destination.getLocationId()).template toAdd<ValueType>() * transitionsDd * variables.rowExpressionAdapter->translateExpression(destination.getProbability()), assignedGlobalVariables);
            }
            
            /*!
             * Builds the DD for the given edge.
             */
            EdgeDd<Type, ValueType> buildEdgeDd(storm::jani::Automaton const& automaton, storm::jani::Edge const& edge) {
                STORM_LOG_TRACE("Translating guard " << edge.getGuard());
                storm::dd::Add<Type, ValueType> guard = variables.rowExpressionAdapter->translateExpression(edge.getGuard()) * variables.automatonToRangeMap.at(automaton.getName());
                STORM_LOG_WARN_COND(!guard.isZero(), "The guard '" << edge.getGuard() << "' is unsatisfiable.");
                
                if (!guard.isZero()) {
                    // Create the DDs representing the individual updates.
                    std::vector<EdgeDestinationDd<Type, ValueType>> destinationDds;
                    for (storm::jani::EdgeDestination const& destination : edge.getDestinations()) {
                        destinationDds.push_back(buildEdgeDestinationDd(automaton, destination, guard));
                        
                        STORM_LOG_WARN_COND(!destinationDds.back().transitionsDd.isZero(), "Destination does not have any effect.");
                    }
                    
                    // Start by gathering all variables that were written in at least one update.
                    std::set<storm::expressions::Variable> globalVariablesInSomeUpdate;
                    
                    // If the edge is not labeled with the silent action, we have to analyze which portion of the global
                    // variables was written by any of the updates and make all update results equal w.r.t. this set. If
                    // the edge is labeled with the silent action, we can already multiply the identities of all global variables.
                    if (edge.getActionId() != this->model.getSilentActionIndex()) {
                        std::for_each(destinationDds.begin(), destinationDds.end(), [&globalVariablesInSomeUpdate] (EdgeDestinationDd<Type, ValueType> const& edgeDestinationDd) { globalVariablesInSomeUpdate.insert(edgeDestinationDd.writtenGlobalVariables.begin(), edgeDestinationDd.writtenGlobalVariables.end()); } );
                    } else {
                        globalVariablesInSomeUpdate = variables.allGlobalVariables;
                    }
                    
                    // Then, multiply the missing identities.
                    for (auto& destinationDd : destinationDds) {
                        std::set<storm::expressions::Variable> missingIdentities;
                        std::set_difference(globalVariablesInSomeUpdate.begin(), globalVariablesInSomeUpdate.end(), destinationDd.writtenGlobalVariables.begin(), destinationDd.writtenGlobalVariables.end(), std::inserter(missingIdentities, missingIdentities.begin()));
                        
                        for (auto const& variable : missingIdentities) {
                            STORM_LOG_TRACE("Multiplying identity for variable " << variable.getName() << " to destination DD.");
                            destinationDd.transitionsDd *= variables.variableToIdentityMap.at(variable);
                        }
                    }
                    
                    // Now combine the destination DDs to the edge DD.
                    storm::dd::Add<Type, ValueType> transitionsDd = variables.manager->template getAddZero<ValueType>();
                    for (auto const& destinationDd : destinationDds) {
                        transitionsDd += destinationDd.transitionsDd;
                    }
                    
                    // Add the source location and the guard.
                    transitionsDd *= variables.manager->getEncoding(variables.automatonToLocationVariableMap.at(automaton.getName()).first, edge.getSourceLocationId()).template toAdd<ValueType>() * guard;

                    // If we multiply the ranges of global variables, make sure everything stays within its bounds.
                    if (!globalVariablesInSomeUpdate.empty()) {
                        transitionsDd *= variables.globalVariableRanges;
                    }
                    
                    // If the edge has a rate, we multiply it to the DD.
                    if (edge.hasRate()) {
                        transitionsDd *= variables.rowExpressionAdapter->translateExpression(edge.getRate());
                    }
                    
                    return EdgeDd<Type, ValueType>(guard, transitionsDd, globalVariablesInSomeUpdate);
                } else {
                    return EdgeDd<Type, ValueType>(variables.manager->template getAddZero<ValueType>(), variables.manager->template getAddZero<ValueType>());
                }
            }
            
            /*!
             * Builds the DD for the automaton with the given name.
             */
            AutomatonDd<Type, ValueType> buildAutomatonDd(std::string const& automatonName) {
                AutomatonDd<Type, ValueType> result(variables.automatonToIdentityMap.at(automatonName));
                
                storm::jani::Automaton const& automaton = this->model.getAutomaton(automatonName);
                for (auto const& edge : automaton.getEdges()) {
                    // Build the edge and add it if it adds transitions.
                    EdgeDd<Type, ValueType> edgeDd = buildEdgeDd(automaton, edge);
                    if (!edgeDd.guardDd.isZero()) {
                        result.actionIndexToEdges[edge.getActionId()].push_back(edgeDd);
                    }
                }
                
                return result;
            }
            
            // The model that is referred to by the composition.
            storm::jani::Model const& model;
            
            // The variable to use when building an automaton.
            CompositionVariables<Type, ValueType> const& variables;
        };
        
        template <storm::dd::DdType Type, typename ValueType>
        storm::dd::Add<Type, ValueType> encodeAction(boost::optional<uint64_t> trueIndex, std::vector<storm::expressions::Variable> const& actionVariables, CompositionVariables<Type, ValueType> const& variables) {
            storm::dd::Add<Type, ValueType> encoding = variables.manager->template getAddOne<ValueType>();

            if (trueIndex) {
                *trueIndex = actionVariables.size() - (*trueIndex + 1);
            }
            uint64_t index = 0;
            for (auto it = actionVariables.rbegin(), ite = actionVariables.rend(); it != ite; ++it, ++index) {
                if (trueIndex && index == trueIndex) {
                    encoding *= variables.manager->getEncoding(*it, 1).template toAdd<ValueType>();
                } else {
                    encoding *= variables.manager->getEncoding(*it, 0).template toAdd<ValueType>();
                }
            }
            
            return encoding;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        storm::dd::Add<Type, ValueType> computeMissingGlobalVariableIdentities(EdgeDd<Type, ValueType> const& edge, CompositionVariables<Type, ValueType> const& variables) {
            std::set<storm::expressions::Variable> missingIdentities;
            std::set_difference(variables.allGlobalVariables.begin(), variables.allGlobalVariables.end(), edge.writtenGlobalVariables.begin(), edge.writtenGlobalVariables.end(), std::inserter(missingIdentities, missingIdentities.begin()));
            storm::dd::Add<Type, ValueType> identityEncoding = variables.manager->template getAddOne<ValueType>();
            for (auto const& variable : missingIdentities) {
                identityEncoding *= variables.variableToIdentityMap.at(variable);
            }
            return identityEncoding;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        storm::dd::Add<Type, ValueType> encodeIndex(uint64_t index, std::vector<storm::expressions::Variable> const& localNondeterminismVariables, CompositionVariables<Type, ValueType> const& variables) {
            storm::dd::Add<Type, ValueType> result = variables.manager->template getAddZero<ValueType>();
            
            STORM_LOG_TRACE("Encoding " << index << " with " << localNondeterminismVariables.size() << " binary variable(s).");
            
            std::map<storm::expressions::Variable, int_fast64_t> metaVariableNameToValueMap;
            for (uint_fast64_t i = 0; i < localNondeterminismVariables.size(); ++i) {
                if (index & (1ull << (localNondeterminismVariables.size() - i - 1))) {
                    metaVariableNameToValueMap.emplace(localNondeterminismVariables[i], 1);
                } else {
                    metaVariableNameToValueMap.emplace(localNondeterminismVariables[i], 0);
                }
            }
            
            result.setValue(metaVariableNameToValueMap, 1);
            return result;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        struct SystemDd {
            SystemDd(storm::dd::Add<Type, ValueType> const& transitionsDd, storm::dd::Add<Type, ValueType> const& stateActionDd, uint64_t numberOfNondeterminismVariables = 0) : transitionsDd(transitionsDd), stateActionDd(stateActionDd), numberOfNondeterminismVariables(numberOfNondeterminismVariables) {
                // Intentionally left empty.
            }
            
            storm::dd::Add<Type, ValueType> transitionsDd;
            storm::dd::Add<Type, ValueType> stateActionDd;
            uint64_t numberOfNondeterminismVariables;
        };
        
        template <storm::dd::DdType Type, typename ValueType>
        SystemDd<Type, ValueType> buildSystemDd(storm::jani::Model const& model, AutomatonDd<Type, ValueType> const& automatonDd, CompositionVariables<Type, ValueType>& variables) {
            // If the model is an MDP, we need to encode the nondeterminism using additional variables.
            if (model.getModelType() == storm::jani::ModelType::MDP) {
                // Determine how many nondeterminism variables we need.
                std::vector<storm::expressions::Variable> orderedActionVariables;
                std::set<storm::expressions::Variable> actionVariables;
                std::map<uint64_t, boost::optional<uint64_t>> actionIndexToVariableIndex;
                uint64_t maximalNumberOfEdgesPerAction = 0;
                for (auto const& action : automatonDd.actionIndexToEdges) {
                    if (action.first != model.getSilentActionIndex()) {
                        orderedActionVariables.push_back(variables.actionVariablesMap.at(action.first));
                        actionVariables.insert(orderedActionVariables.back());
                        actionIndexToVariableIndex[action.first] = orderedActionVariables.size() - 1;
                    } else {
                        actionIndexToVariableIndex[action.first] = boost::none;
                    }
                    maximalNumberOfEdgesPerAction = std::max(maximalNumberOfEdgesPerAction, static_cast<uint64_t>(action.second.size()));
                }
                
                // If the maximal number of edges per action is zero, which can happen if the model only has unsatisfiable guards,
                // then we must not compute the number of variables.
                if (maximalNumberOfEdgesPerAction == 0) {
                    return SystemDd<Type, ValueType>(variables.manager->template getAddZero<ValueType>(), variables.manager->template getAddZero<ValueType>(), 0);
                }
                
                uint64_t numberOfNondeterminismVariables = static_cast<uint64_t>(std::ceil(std::log2(maximalNumberOfEdgesPerAction)));
                std::vector<storm::expressions::Variable> localNondeterminismVariables(numberOfNondeterminismVariables);
                std::copy(variables.localNondeterminismVariables.begin(), variables.localNondeterminismVariables.begin() + numberOfNondeterminismVariables, localNondeterminismVariables.begin());
                
                // Prepare result.
                storm::dd::Add<Type, ValueType> result = variables.manager->template getAddZero<ValueType>();
                
                // Add edges to the result.
                for (auto const& action : automatonDd.actionIndexToEdges) {
                    storm::dd::Add<Type, ValueType> edgesForAction = variables.manager->template getAddZero<ValueType>();
                    
                    uint64_t edgeIndex = 0;
                    for (auto const& edge : action.second) {
                        storm::dd::Add<Type, ValueType> dd = edge.transitionsDd * computeMissingGlobalVariableIdentities(edge, variables) * encodeIndex(edgeIndex, localNondeterminismVariables, variables);
                        dd.exportToDot("add" + std::to_string(edgeIndex) + ".dot");
                        edgesForAction += edge.transitionsDd * computeMissingGlobalVariableIdentities(edge, variables) * encodeIndex(edgeIndex, localNondeterminismVariables, variables);
                        ++edgeIndex;
                    }
                    
                    result += edgesForAction * encodeAction<Type, ValueType>(actionIndexToVariableIndex.at(action.first), orderedActionVariables, variables);
                }
                
                return SystemDd<Type, ValueType>(result, result.sumAbstract(variables.columnMetaVariables), numberOfNondeterminismVariables);
            } else if (model.getModelType() == storm::jani::ModelType::DTMC || model.getModelType() == storm::jani::ModelType::CTMC) {
                // Simply add all actions, but make sure to include the missing global variable identities.
                storm::dd::Add<Type, ValueType> result = variables.manager->template getAddZero<ValueType>();
                for (auto const& action : automatonDd.actionIndexToEdges) {
                    for (auto const& edge : action.second) {
                        result += edge.transitionsDd * computeMissingGlobalVariableIdentities(edge, variables);
                    }
                }
                
                return SystemDd<Type, ValueType>(result, result.sumAbstract(variables.columnMetaVariables));
            }

            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal model type.");
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        struct ModelComponents {
            storm::dd::Bdd<Type> reachableStates;
            storm::dd::Bdd<Type> initialStates;
            storm::dd::Add<Type, ValueType> transitionMatrix;
            std::unordered_map<std::string, storm::models::symbolic::StandardRewardModel<Type, ValueType>> rewardModels;
        };
        
        template <storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>> createModel(storm::jani::ModelType const& modelType, CompositionVariables<Type, ValueType> const& variables, ModelComponents<Type, ValueType> const& modelComponents) {
            if (modelType == storm::jani::ModelType::DTMC) {
                return std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>>(new storm::models::symbolic::Dtmc<Type, ValueType>(variables.manager, modelComponents.reachableStates, modelComponents.initialStates, modelComponents.transitionMatrix, variables.rowMetaVariables, variables.rowExpressionAdapter, variables.columnMetaVariables, variables.columnExpressionAdapter, variables.rowColumnMetaVariablePairs, std::map<std::string, storm::expressions::Expression>(), modelComponents.rewardModels));
            } else if (modelType == storm::jani::ModelType::CTMC) {
                return std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>>(new storm::models::symbolic::Ctmc<Type, ValueType>(variables.manager, modelComponents.reachableStates, modelComponents.initialStates, modelComponents.transitionMatrix, variables.rowMetaVariables, variables.rowExpressionAdapter, variables.columnMetaVariables, variables.columnExpressionAdapter, variables.rowColumnMetaVariablePairs, std::map<std::string, storm::expressions::Expression>(), modelComponents.rewardModels));
            } else if (modelType == storm::jani::ModelType::MDP) {
                return std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>>(new storm::models::symbolic::Mdp<Type, ValueType>(variables.manager, modelComponents.reachableStates, modelComponents.initialStates, modelComponents.transitionMatrix, variables.rowMetaVariables, variables.rowExpressionAdapter, variables.columnMetaVariables, variables.columnExpressionAdapter, variables.rowColumnMetaVariablePairs, variables.allNondeterminismVariables, std::map<std::string, storm::expressions::Expression>(), modelComponents.rewardModels));
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid model type.");
            }

        }
        
        template <storm::dd::DdType Type, typename ValueType>
        void postprocessSystemAndVariables(storm::jani::Model const& model, SystemDd<Type, ValueType>& system, CompositionVariables<Type, ValueType>& variables, typename DdJaniModelBuilder<Type, ValueType>::Options const& options) {
            // Get rid of the nondeterminism variables that were not used.
            for (uint64_t index = system.numberOfNondeterminismVariables; index < variables.localNondeterminismVariables.size(); ++index) {
                variables.allNondeterminismVariables.erase(variables.localNondeterminismVariables[index]);
            }
            variables.localNondeterminismVariables.resize(system.numberOfNondeterminismVariables);
            
            // For DTMCs, we normalize each row to 1 (to account for non-determinism).
            if (model.getModelType() == storm::jani::ModelType::DTMC) {
                system.transitionsDd = system.transitionsDd / system.stateActionDd;
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
                
                system.transitionsDd *= (!terminalStatesBdd).template toAdd<ValueType>();
            }
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        storm::dd::Bdd<Type> computeInitialStates(storm::jani::Model const& model, CompositionVariables<Type, ValueType> const& variables) {
            storm::dd::Bdd<Type> initialStates = variables.rowExpressionAdapter->translateExpression(model.getInitialStatesExpression(true)).toBdd();
            for (auto const& automaton : model.getAutomata()) {
                initialStates &= variables.manager->getEncoding(variables.automatonToLocationVariableMap.at(automaton.getName()).first, automaton.getInitialLocationIndex());
            }
            for (auto const& metaVariable : variables.rowMetaVariables) {
                initialStates &= variables.manager->getRange(metaVariable);
            }
            return initialStates;
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        void fixDeadlocks(storm::jani::ModelType const& modelType, storm::dd::Add<Type, ValueType>& transitionMatrix, storm::dd::Bdd<Type> const& transitionMatrixBdd, storm::dd::Bdd<Type> const& reachableStates, CompositionVariables<Type, ValueType> const& variables) {
            // Detect deadlocks and 1) fix them if requested 2) throw an error otherwise.
            storm::dd::Bdd<Type> statesWithTransition = transitionMatrixBdd.existsAbstract(variables.columnMetaVariables);
            storm::dd::Add<Type, ValueType> deadlockStates = (reachableStates && !statesWithTransition).template toAdd<ValueType>();
            
            if (!deadlockStates.isZero()) {
                // If we need to fix deadlocks, we do so now.
                if (!storm::settings::getModule<storm::settings::modules::MarkovChainSettings>().isDontFixDeadlocksSet()) {
                    STORM_LOG_INFO("Fixing deadlocks in " << deadlockStates.getNonZeroCount() << " states. The first three of these states are: ");
                    
                    uint_fast64_t count = 0;
                    for (auto it = deadlockStates.begin(), ite = deadlockStates.end(); it != ite && count < 3; ++it, ++count) {
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
                        transitionMatrix += deadlockStates * globalIdentity;
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
                        transitionMatrix += deadlockStates * globalIdentity * action;
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The model contains " << deadlockStates.getNonZeroCount() << " deadlock states. Please unset the option to not fix deadlocks, if you want to fix them automatically.");
                }
            }
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>> DdJaniModelBuilder<Type, ValueType>::translate() {
            // Create all necessary variables.
            CompositionVariableCreator<Type, ValueType> variableCreator(*this->model);
            CompositionVariables<Type, ValueType> variables = variableCreator.create();
            
            // Compose the automata to a single automaton.
            AutomatonComposer<Type, ValueType> composer(*this->model, variables);
            AutomatonDd<Type, ValueType> globalAutomaton = composer.compose();

            // Combine the edges of the single automaton to the full system DD.
            SystemDd<Type, ValueType> system = buildSystemDd(*this->model, globalAutomaton, variables);
            
            // Postprocess the system. This modifies the systemDd in place.
            postprocessSystemAndVariables(*this->model, system, variables, options);
            
            // Start creating the model components.
            ModelComponents<Type, ValueType> modelComponents;
            
            // Build initial states.
            modelComponents.initialStates = computeInitialStates(*this->model, variables);
    
            // Perform reachability analysis to obtain reachable states.
            system.transitionsDd.exportToDot("trans.dot");
            std::cout << "nnz: " << system.transitionsDd.getNonZeroCount() << std::endl;
            std::cout << "size: " << system.transitionsDd.getNodeCount() << std::endl;
            storm::dd::Bdd<Type> transitionMatrixBdd = system.transitionsDd.notZero();
            if (this->model->getModelType() == storm::jani::ModelType::MDP) {
                transitionMatrixBdd = transitionMatrixBdd.existsAbstract(variables.allNondeterminismVariables);
            }
            modelComponents.reachableStates = storm::utility::dd::computeReachableStates(modelComponents.initialStates, transitionMatrixBdd, variables.rowMetaVariables, variables.columnMetaVariables);

            // Cut transitions to reachable states.
            storm::dd::Add<Type, ValueType> reachableStatesAdd = modelComponents.reachableStates.template toAdd<ValueType>();
            modelComponents.transitionMatrix = system.transitionsDd * reachableStatesAdd;
            system.stateActionDd *= reachableStatesAdd;

            // Fix deadlocks if existing.
            fixDeadlocks(this->model->getModelType(), modelComponents.transitionMatrix, transitionMatrixBdd, modelComponents.reachableStates, variables);
            
            // Finally, create the model.
            return createModel(this->model->getModelType(), variables, modelComponents);
        }
        
        template class DdJaniModelBuilder<storm::dd::DdType::CUDD, double>;
        template class DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>;
    }
}