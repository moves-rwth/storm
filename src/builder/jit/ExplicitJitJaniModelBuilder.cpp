#include "src/builder/jit/ExplicitJitJaniModelBuilder.h"

#include <iostream>
#include <cstdio>
#include <chrono>

#include "src/adapters/CarlAdapter.h"

#include "src/solver/SmtSolver.h"

#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/NotSupportedException.h"
#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            static const std::string CXX_COMPILER = "clang++";
            static const std::string DYLIB_EXTENSION = ".dylib";
            static const std::string COMPILER_FLAGS = "-std=c++11 -stdlib=libc++ -fPIC -O3 -shared -funroll-loops -undefined dynamic_lookup";
            static const std::string STORM_ROOT = "/Users/chris/work/storm";
            static const std::string L3PP_ROOT = "/Users/chris/work/storm/resources/3rdparty/l3pp";
            static const std::string BOOST_ROOT = "/usr/local/Cellar/boost/1.62.0/include";
            static const std::string GMP_ROOT = "/usr/local/Cellar/gmp/6.1.1";
            static const std::string CARL_ROOT = "/Users/chris/work/carl";
            static const std::string CLN_ROOT = "/usr/local/Cellar/cln/1.3.4";
            static const std::string GINAC_ROOT = "/usr/local/Cellar/ginac/1.6.7_1";
            
            template <typename ValueType, typename RewardModelType>
            ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::ExplicitJitJaniModelBuilder(storm::jani::Model const& model, storm::builder::BuilderOptions const& options) : options(options), model(model), modelComponentsBuilder(model.getModelType()) {

                for (auto const& automaton : this->model.getAutomata()) {
                    locationVariables.emplace(automaton.getName(), model.getManager().declareFreshIntegerVariable(false, automaton.getName() + "_"));
                }
            }
            
            template <typename ValueType, typename RewardModelType>
            std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::createSourceCode() {
                std::string sourceTemplate = R"(
                
#define NDEBUG
                
#include <cstdint>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <unordered_map>
#include <boost/dll/alias.hpp>
                
#include "resources/3rdparty/sparsepp/sparsepp.h"
                
#include "src/builder/jit/JitModelBuilderInterface.h"
#include "src/builder/jit/StateBehaviour.h"
#include "src/builder/jit/ModelComponentsBuilder.h"
                
                namespace storm {
                    namespace builder {
                        namespace jit {
                            
                            typedef uint32_t IndexType;
                            typedef double ValueType;
                            
                            struct StateType {
                                // Boolean variables.
                                {% for variable in stateVariables.boolean %}bool {$variable.name} : 1;
                                {% endfor %}
                                // Bounded integer variables.
                                {% for variable in stateVariables.boundedInteger %}uint64_t {$variable.name} : {$variable.numberOfBits};
                                {% endfor %}
                                // Location variables.
                                {% for variable in stateVariables.locations %}uint64_t {$variable.name} : {$variable.numberOfBits};
                                {% endfor %}
                            };
                            
                            bool operator==(StateType const& first, StateType const& second) {
                                bool result = true;
                                {% for variable in stateVariables.boolean %}result &= !(first.{$variable.name} ^ second.{$variable.name});
                                {% endfor %}
                                {% for variable in stateVariables.boundedInteger %}result &= first.{$variable.name} == second.{$variable.name};
                                {% endfor %}
                                {% for variable in stateVariables.locations %}result &= first.{$variable.name} == second.{$variable.name};
                                {% endfor %}
                                return result;
                            }
                            
                            std::ostream& operator<<(std::ostream& out, StateType const& state) {
                                out << "<";
                                {% for variable in stateVariables.boolean %}out << "{$variable.name}=" << state.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in stateVariables.boundedInteger %}out << "{$variable.name}=" << state.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in stateVariables.locations %}out << "{$variable.name}=" << state.{$variable.name} << ", ";
                                {% endfor %}
                                out << ">";
                                return out;
                            }
                        }
                    }
                }
                
                namespace std {
                    template <>
                    struct hash<storm::builder::jit::StateType> {
                        std::size_t operator()(storm::builder::jit::StateType const& state) const {
                            // Note: this is faster than viewing the struct as a bit field and taking hash_combine of the bytes.
                            std::size_t seed = 0;
                            {% for variable in stateVariables.boolean %}spp::hash_combine(seed, state.{$variable.name});
                            {% endfor %}
                            {% for variable in stateVariables.boundedInteger %}spp::hash_combine(seed, state.{$variable.name});
                            {% endfor %}
                            {% for variable in stateVariables.locations %}spp::hash_combine(seed, state.{$variable.name});
                            {% endfor %}
                            return seed;
                        }
                    };
                }
                
                namespace storm {
                    namespace builder {
                        namespace jit {
                            
                            static bool model_is_deterministic() {
                                return {$deterministic_model};
                            }
                            
                            static bool model_is_discrete_time() {
                                return {$discrete_time_model};
                            }
                            
                            class StateSet {
                            public:
                                StateType const& peek() const {
                                    return storage.front();
                                }
                                
                                StateType get() {
                                    StateType result = std::move(storage.front());
                                    storage.pop();
                                    return result;
                                }
                                
                                void add(StateType const& state) {
                                    storage.push(state);
                                }
                                
                                bool empty() const {
                                    return storage.empty();
                                }
                                
                            private:
                                std::queue<StateType> storage;
                            };
                            
                            class JitBuilder : public JitModelBuilderInterface<IndexType, ValueType> {
                            public:
                                JitBuilder(ModelComponentsBuilder<IndexType, ValueType>& modelComponentsBuilder) : JitModelBuilderInterface(modelComponentsBuilder) {
                                    {% for state in initialStates %}{
                                        StateType state;
                                        {% for assignment in state %}state.{$assignment.variable} = {$assignment.value};
                                        {% endfor %}initialStates.push_back(state);
                                    }
                                    {% endfor %}
                                }
                                
                                virtual storm::models::sparse::Model<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>* build() override {
                                    std::cout << "starting building process" << std::endl;
                                    explore(initialStates);
                                    std::cout << "finished building process with " << stateIds.size() << " states" << std::endl;
                                    
                                    std::cout << "building labeling" << std::endl;
                                    label();
                                    std::cout << "finished building labeling" << std::endl;
                                    
                                    return this->modelComponentsBuilder.build(stateIds.size());
                                }

                                void label() {
                                    uint64_t labelCount = 0;
                                    {% for label in labels %}this->modelComponentsBuilder.registerLabel("{$label.name}", stateIds.size());
                                    ++labelCount;
                                    {% endfor %}
                                    this->modelComponentsBuilder.registerLabel("init", stateIds.size());
                                    this->modelComponentsBuilder.registerLabel("deadlock", stateIds.size());
                                    
                                    for (auto const& stateEntry : stateIds) {
                                        auto const& state = stateEntry.first;
                                        {% for label in labels %}if ({$label.predicate}) {
                                            this->modelComponentsBuilder.addLabel(stateEntry.second, {$loop.index} - 1);
                                        }
                                        {% endfor %}
                                    }
                                    
                                    for (auto const& state : initialStates) {
                                        auto stateIt = stateIds.find(state);
                                        if (stateIt != stateIds.end()) {
                                            this->modelComponentsBuilder.addLabel(stateIt->second, labelCount);
                                        }
                                    }
                                    
                                    for (auto const& stateId : deadlockStates) {
                                        this->modelComponentsBuilder.addLabel(stateId, labelCount + 1);
                                    }
                                }
                                
                                void explore(std::vector<StateType> const& initialStates) {
                                    for (auto const& state : initialStates) {
                                        explore(state);
                                    }
                                }
                                
                                void explore(StateType const& initialState) {
                                    StateSet statesToExplore;
                                    getOrAddIndex(initialState, statesToExplore);
                                    
                                    StateBehaviour<IndexType, ValueType> behaviour;
                                    while (!statesToExplore.empty()) {
                                        StateType currentState = statesToExplore.get();
                                        IndexType currentIndex = getIndex(currentState);
#ifndef NDEBUG
                                        std::cout << "Exploring state " << currentState << ", id " << currentIndex << std::endl;
#endif
                                        
                                        behaviour.setExpanded();
                                        exploreNonSynchronizingEdges(currentState, currentIndex, behaviour, statesToExplore);
                                        
                                        this->addStateBehaviour(currentIndex, behaviour);
                                        behaviour.clear();
                                    }
                                }
                                
                                void exploreNonSynchronizingEdges(StateType const& sourceState, IndexType const& currentIndex, StateBehaviour<IndexType, ValueType>& behaviour, StateSet& statesToExplore) {
                                    {% for edge in nonSynchronizingEdges %}if ({$edge.guard}) {
                                        Choice<IndexType, ValueType>& choice = behaviour.addChoice();
                                        {% for destination in edge.destinations %}{
                                            StateType targetState(sourceState);
                                            {% for assignment in destination.nonTransientAssignments %}targetState.{$assignment.variable} = {$assignment.value};
                                            {% endfor %}
                                            IndexType targetStateIndex = getOrAddIndex(targetState, statesToExplore);
                                            choice.add(targetStateIndex, {$destination.value});
                                        }
                                        {% endfor %}
                                    }
                                    {% endfor %}
                                }
                                
                                IndexType getOrAddIndex(StateType const& state, StateSet& statesToExplore) {
                                    auto it = stateIds.find(state);
                                    if (it != stateIds.end()) {
                                        return it->second;
                                    } else {
                                        IndexType newIndex = static_cast<IndexType>(stateIds.size());
                                        stateIds.insert(std::make_pair(state, newIndex));
#ifndef NDEBUG
                                        std::cout << "inserting state " << state << std::endl;
#endif
                                        statesToExplore.add(state);
                                        return newIndex;
                                    }
                                }
                                
                                IndexType getIndex(StateType const& state) const {
                                    auto it = stateIds.find(state);
                                    if (it != stateIds.end()) {
                                        return it->second;
                                    } else {
                                        return stateIds.at(state);
                                    }
                                }
                                
                                void addStateBehaviour(IndexType const& stateId, StateBehaviour<IndexType, ValueType>& behaviour) {
                                    if (behaviour.empty()) {
                                        deadlockStates.push_back(stateId);
                                    }
                                    
                                    JitModelBuilderInterface<IndexType, ValueType>::addStateBehaviour(stateId, behaviour);
                                }
                                
                                static JitModelBuilderInterface<IndexType, ValueType>* create(ModelComponentsBuilder<IndexType, ValueType>& modelComponentsBuilder) {
                                    return new JitBuilder(modelComponentsBuilder);
                                }
                                
                            private:
                                spp::sparse_hash_map<StateType, IndexType> stateIds;
                                std::vector<StateType> initialStates;
                                std::vector<IndexType> deadlockStates;
                            };
                            
                            BOOST_DLL_ALIAS(storm::builder::jit::JitBuilder::create, create_builder)
                        }
                    }
                }
                )";
                
                cpptempl::data_map modelData;
                modelData["stateVariables"] = generateStateVariables();
                cpptempl::data_list initialStates = generateInitialStates();
                modelData["initialStates"] = cpptempl::make_data(initialStates);
                cpptempl::data_list nonSynchronizingEdges = generateNonSynchronizingEdges();
                modelData["nonSynchronizingEdges"] = cpptempl::make_data(nonSynchronizingEdges);
                cpptempl::data_list labels = generateLabels();
                modelData["labels"] = cpptempl::make_data(labels);
                modelData["deterministic_model"] = model.isDeterministicModel() ? "true" : "false";
                modelData["discrete_time_model"] = model.isDiscreteTimeModel() ? "true" : "false";
                return cpptempl::parse(sourceTemplate, modelData);
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_list ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateInitialStates() {
                cpptempl::data_list initialStates;
                
                // Prepare an SMT solver to enumerate all initial states.
                storm::utility::solver::SmtSolverFactory factory;
                std::unique_ptr<storm::solver::SmtSolver> solver = factory.create(model.getExpressionManager());
                
                std::vector<storm::expressions::Expression> rangeExpressions = model.getAllRangeExpressions();
                for (auto const& expression : rangeExpressions) {
                    solver->add(expression);
                }
                solver->add(model.getInitialStatesExpression(true));
                
                // Proceed as long as the solver can still enumerate initial states.
                while (solver->check() == storm::solver::SmtSolver::CheckResult::Sat) {
                    // Create fresh state.
                    cpptempl::data_list initialStateAssignment;
                    
                    // Read variable assignment from the solution of the solver. Also, create an expression we can use to
                    // prevent the variable assignment from being enumerated again.
                    storm::expressions::Expression blockingExpression;
                    std::shared_ptr<storm::solver::SmtSolver::ModelReference> model = solver->getModel();
                    for (auto const& variable : this->model.getGlobalVariables().getBooleanVariables()) {
                        storm::expressions::Variable const& expressionVariable = variable.getExpressionVariable();
                        bool variableValue = model->getBooleanValue(expressionVariable);
                        initialStateAssignment.push_back(generateAssignment(variable, variableValue));
                        
                        storm::expressions::Expression localBlockingExpression = variableValue ? !expressionVariable : expressionVariable;
                        blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
                    }
                    for (auto const& variable : this->model.getGlobalVariables().getBoundedIntegerVariables()) {
                        storm::expressions::Variable const& expressionVariable = variable.getExpressionVariable();
                        int_fast64_t variableValue = model->getIntegerValue(expressionVariable);
                        initialStateAssignment.push_back(generateAssignment(variable, variableValue));
                        
                        storm::expressions::Expression localBlockingExpression = expressionVariable != model->getManager().integer(variableValue);
                        blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
                    }
                    for (auto const& automaton : this->model.getAutomata()) {
                        for (auto const& variable : automaton.getVariables().getBooleanVariables()) {
                            storm::expressions::Variable const& expressionVariable = variable.getExpressionVariable();
                            bool variableValue = model->getBooleanValue(expressionVariable);
                            initialStateAssignment.push_back(generateAssignment(variable, variableValue));
                            
                            storm::expressions::Expression localBlockingExpression = variableValue ? !expressionVariable : expressionVariable;
                            blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
                        }
                        for (auto const& variable : automaton.getVariables().getBoundedIntegerVariables()) {
                            storm::expressions::Variable const& expressionVariable = variable.getExpressionVariable();
                            int_fast64_t variableValue = model->getIntegerValue(expressionVariable);
                            initialStateAssignment.push_back(generateAssignment(variable, variableValue));
                            
                            storm::expressions::Expression localBlockingExpression = expressionVariable != model->getManager().integer(variableValue);
                            blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
                        }
                    }
                    
                    // Gather iterators to the initial locations of all the automata.
                    std::vector<std::set<uint64_t>::const_iterator> initialLocationsIterators;
                    for (auto const& automaton : this->model.getAutomata()) {
                        initialLocationsIterators.push_back(automaton.getInitialLocationIndices().cbegin());
                    }
                    
                    // Now iterate through all combinations of initial locations.
                    while (true) {
                        cpptempl::data_list completeAssignment(initialStateAssignment);
                        
                        for (uint64_t index = 0; index < initialLocationsIterators.size(); ++index) {
                            storm::jani::Automaton const& automaton = this->model.getAutomata()[index];
                            if (automaton.getNumberOfLocations() > 1) {
                                completeAssignment.push_back(generateLocationAssignment(automaton, *initialLocationsIterators[index]));
                            }
                        }
                        initialStates.push_back(cpptempl::make_data(completeAssignment));
                        
                        uint64_t index = 0;
                        for (; index < initialLocationsIterators.size(); ++index) {
                            ++initialLocationsIterators[index];
                            if (initialLocationsIterators[index] == this->model.getAutomata()[index].getInitialLocationIndices().cend()) {
                                initialLocationsIterators[index] = this->model.getAutomata()[index].getInitialLocationIndices().cbegin();
                            } else {
                                break;
                            }
                        }
                        
                        // If we are at the end, leave the loop.
                        if (index == initialLocationsIterators.size()) {
                            break;
                        }
                    }
                    
                    // Block the current initial state to search for the next one.
                    if (!blockingExpression.isInitialized()) {
                        break;
                    }
                    solver->add(blockingExpression);
                }
                
                return initialStates;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateStateVariables() {
                cpptempl::data_list booleanVariables;
                cpptempl::data_list boundedIntegerVariables;
                cpptempl::data_list locationVariables;
                
                for (auto const& variable : model.getGlobalVariables().getBooleanVariables()) {
                    cpptempl::data_map booleanVariable;
                    std::string variableName = getQualifiedVariableName(variable);
                    variableToName[variable.getExpressionVariable()] = variableName;
                    booleanVariable["name"] = variableName;
                    booleanVariables.push_back(booleanVariable);
                }
                for (auto const& variable : model.getGlobalVariables().getBoundedIntegerVariables()) {
                    cpptempl::data_map boundedIntegerVariable;
                    std::string variableName = getQualifiedVariableName(variable);
                    variableToName[variable.getExpressionVariable()] = variableName;
                    
                    uint64_t range = static_cast<uint64_t>(variable.getUpperBound().evaluateAsInt() - variable.getLowerBound().evaluateAsInt() + 1);
                    uint64_t numberOfBits = static_cast<uint64_t>(std::ceil(std::log2(range)));
                    
                    boundedIntegerVariable["name"] = variableName;
                    boundedIntegerVariable["numberOfBits"] = std::to_string(numberOfBits);
                    boundedIntegerVariables.push_back(boundedIntegerVariable);
                }
                for (auto const& automaton : model.getAutomata()) {
                    for (auto const& variable : automaton.getVariables().getBooleanVariables()) {
                        cpptempl::data_map booleanVariable;
                        std::string variableName = getQualifiedVariableName(automaton, variable);
                        variableToName[variable.getExpressionVariable()] = variableName;
                        booleanVariable["name"] = variableName;
                        booleanVariables.push_back(booleanVariable);
                    }
                    for (auto const& variable : automaton.getVariables().getBoundedIntegerVariables()) {
                        cpptempl::data_map boundedIntegerVariable;
                        std::string variableName = getQualifiedVariableName(automaton, variable);
                        variableToName[variable.getExpressionVariable()] = variableName;
                        
                        uint64_t range = static_cast<uint64_t>(variable.getUpperBound().evaluateAsInt() - variable.getLowerBound().evaluateAsInt());
                        uint64_t numberOfBits = static_cast<uint64_t>(std::ceil(std::log2(range)));
                        
                        boundedIntegerVariable["name"] = variableName;
                        boundedIntegerVariable["numberOfBits"] = std::to_string(numberOfBits);
                        boundedIntegerVariables.push_back(boundedIntegerVariable);
                    }
                    
                    // Only generate a location variable if there is more than one location for the automaton.
                    if (automaton.getNumberOfLocations() > 1) {
                        cpptempl::data_map locationVariable;
                        locationVariable["name"] = getQualifiedVariableName(automaton, this->locationVariables.at(automaton.getName()));
                        locationVariable["numberOfBits"] = static_cast<uint64_t>(std::ceil(std::log2(automaton.getNumberOfLocations())));
                        locationVariables.push_back(locationVariable);
                    }
                }
                
                cpptempl::data_map stateVariables;
                stateVariables["boolean"] = cpptempl::make_data(booleanVariables);
                stateVariables["boundedInteger"] = cpptempl::make_data(boundedIntegerVariables);
                stateVariables["locations"] = cpptempl::make_data(locationVariables);
                return stateVariables;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_list ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateLabels() {
                cpptempl::data_list labels;
                
                // As in JANI we can use transient boolean variable assignments in locations to identify states, we need to
                // create a list of boolean transient variables and the expressions that define them.
                for (auto const& variable : model.getGlobalVariables().getTransientVariables()) {
                    if (variable->isBooleanVariable()) {
                        if (this->options.isBuildAllLabelsSet() || this->options.getLabelNames().find(variable->getName()) != this->options.getLabelNames().end()) {
                            cpptempl::data_map label;
                            label["name"] = variable->getName();
                            label["predicate"] = expressionTranslator.translate(model.getLabelExpression(variable->asBooleanVariable(), locationVariables), storm::expressions::ToCppTranslationOptions("state."));
                            labels.push_back(label);
                        }
                    }
                }
                
                for (auto const& expression : this->options.getExpressionLabels()) {
                    cpptempl::data_map label;
                    label["name"] = expression.toString();
                    label["predicate"] = expressionTranslator.translate(expression, storm::expressions::ToCppTranslationOptions("state."));
                    labels.push_back(label);
                }
                
                return labels;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_list ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateNonSynchronizingEdges() {
                cpptempl::data_list edges;
                for (auto const& automaton : this->model.getAutomata()) {
                    for (auto const& edge : automaton.getEdges()) {
                        if (edge.getActionIndex() == storm::jani::Model::SILENT_ACTION_INDEX) {
                            edges.push_back(generateEdge(edge));
                        }
                    }
                }
                
                return edges;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateEdge(storm::jani::Edge const& edge) {
                cpptempl::data_map edgeData;
                
                cpptempl::data_list destinations;
                for (auto const& destination : edge.getDestinations()) {
                    destinations.push_back(generateDestination(destination));
                }
                
                edgeData["guard"] = expressionTranslator.translate(edge.getGuard(), storm::expressions::ToCppTranslationOptions("sourceState."));
                edgeData["destinations"] = cpptempl::make_data(destinations);
                return edgeData;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateDestination(storm::jani::EdgeDestination const& destination) {
                cpptempl::data_map destinationData;
                
                cpptempl::data_list nonTransientAssignments;
                for (auto const& assignment : destination.getOrderedAssignments().getNonTransientAssignments()) {
                    nonTransientAssignments.push_back(generateAssignment(assignment, "targetState."));
                }
                
                destinationData["nonTransientAssignments"] = cpptempl::make_data(nonTransientAssignments);
                destinationData["value"] = expressionTranslator.translate(destination.getProbability(), storm::expressions::ToCppTranslationOptions("sourceState.", "double"));
                return destinationData;
            }
            
            template <typename ValueType, typename RewardModelType>
            template <typename ValueTypePrime>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateAssignment(storm::jani::Variable const& variable, ValueTypePrime value) const {
                cpptempl::data_map result;
                result["variable"] = getVariableName(variable.getExpressionVariable());
                result["value"] = asString(value);
                return result;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateLocationAssignment(storm::jani::Automaton const& automaton, uint64_t value) const {
                cpptempl::data_map result;
                result["variable"] = getLocationVariableName(automaton);
                result["value"] = asString(value);
                return result;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateAssignment(storm::jani::Assignment const& assignment, std::string const& prefix) {
                cpptempl::data_map result;
                result["variable"] = getVariableName(assignment.getExpressionVariable());
                result["value"] = expressionTranslator.translate(assignment.getAssignedExpression(), prefix);
                return result;
            }
            
            template <typename ValueType, typename RewardModelType>
            std::string const& ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::getVariableName(storm::expressions::Variable const& variable) const {
                return variableToName.at(variable);
            }
            
            template <typename ValueType, typename RewardModelType>
            std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::getQualifiedVariableName(storm::jani::Variable const& variable) const {
                return variable.getName();
            }
            
            template <typename ValueType, typename RewardModelType>
            std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::getQualifiedVariableName(storm::jani::Automaton const& automaton, storm::jani::Variable const& variable) const {
                return variable.getExpressionVariable().getName();
            }

            template <typename ValueType, typename RewardModelType>
            std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::getQualifiedVariableName(storm::jani::Automaton const& automaton, storm::expressions::Variable const& variable) const {
                return variable.getName();
            }

            template <typename ValueType, typename RewardModelType>
            std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::getLocationVariableName(storm::jani::Automaton const& automaton) const {
                return automaton.getName() + "_location";
            }
            
            template <typename ValueType, typename RewardModelType>
            std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::asString(bool value) const {
                std::stringstream out;
                out << std::boolalpha << value;
                return out.str();
            }
            
            template <typename ValueType, typename RewardModelType>
            template <typename ValueTypePrime>
            std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::asString(ValueTypePrime value) const {
                return std::to_string(value);
            }
            
            template <typename ValueType, typename RewardModelType>
            boost::optional<std::string> ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::execute(std::string command) {
                auto start = std::chrono::high_resolution_clock::now();
                char buffer[128];
                std::stringstream output;
                command += " 2>&1";
                
                std::cout << "executing " << command << std::endl;
                
                std::unique_ptr<FILE> pipe(popen(command.c_str(), "r"));
                STORM_LOG_THROW(pipe, storm::exceptions::InvalidStateException, "Call to popen failed.");
                
                while (!feof(pipe.get())) {
                    if (fgets(buffer, 128, pipe.get()) != nullptr)
                        output << buffer;
                }
                int result = pclose(pipe.get());
                pipe.release();
                
                auto end = std::chrono::high_resolution_clock::now();
                std::cout << "Executing command took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
                
                if (WEXITSTATUS(result) == 0) {
                    return boost::none;
                } else {
                    return "Executing command failed. Got response: " + output.str();
                }
            }
            
            template <typename ValueType, typename RewardModelType>
            void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::createBuilder(boost::filesystem::path const& dynamicLibraryPath) {
                jitBuilderGetFunction = boost::dll::import_alias<typename ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::CreateFunctionType>(dynamicLibraryPath, "create_builder");
                builder = std::unique_ptr<JitModelBuilderInterface<IndexType, ValueType>>(jitBuilderGetFunction(modelComponentsBuilder));
            }
            
            template <typename ValueType, typename RewardModelType>
            boost::filesystem::path ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::writeSourceToTemporaryFile(std::string const& source) {
                boost::filesystem::path temporaryFile = boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%.cpp");
                std::ofstream out(temporaryFile.native());
                out << source << std::endl;
                out.close();
                return temporaryFile;
            }
            
            template <typename ValueType, typename RewardModelType>
            boost::filesystem::path ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::compileSourceToSharedLibrary(boost::filesystem::path const& sourceFile) {
                std::string sourceFilename = boost::filesystem::absolute(sourceFile).string();
                auto dynamicLibraryPath = sourceFile;
                dynamicLibraryPath += DYLIB_EXTENSION;
                std::string dynamicLibraryFilename = boost::filesystem::absolute(dynamicLibraryPath).string();
                
                std::string command = CXX_COMPILER + " " + sourceFilename + " " + COMPILER_FLAGS + " -I" + STORM_ROOT + " -I" + STORM_ROOT + "/build_xcode/include -I" + L3PP_ROOT + " -I" + BOOST_ROOT + " -I" + GMP_ROOT + "/include -I" + CARL_ROOT + "/src -I" + CLN_ROOT + "/include -I" + GINAC_ROOT + "/include -o " + dynamicLibraryFilename;
                boost::optional<std::string> error = execute(command);
                
                if (error) {
                    boost::filesystem::remove(sourceFile);
                    STORM_LOG_THROW(false, storm::exceptions::InvalidStateException, "Compiling shared library failed. Error: " << error.get());
                }
                
                return dynamicLibraryPath;
            }
            
            template <typename ValueType, typename RewardModelType>
            std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::build() {
                
                // (1) generate the source code of the shared library
                std::string source;
                try {
                    source = createSourceCode();
                } catch (std::exception const& e) {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "The model could not be successfully built (error: " << e.what() << ").");
                }
                std::cout << "created source code: " << source << std::endl;
                
                // (2) write the source code to a temporary file
                boost::filesystem::path temporarySourceFile = writeSourceToTemporaryFile(source);
                std::cout << "wrote source to file " << temporarySourceFile.native() << std::endl;
                
                // (3) compile the shared library
                boost::filesystem::path dynamicLibraryPath = compileSourceToSharedLibrary(temporarySourceFile);
                std::cout << "successfully compiled shared library" << std::endl;
                
                // (4) remove the source code we just compiled
                boost::filesystem::remove(temporarySourceFile);
                
                // (5) create the loader from the shared library
                createBuilder(dynamicLibraryPath);
                
                // (6) execute the function in the shared lib
                auto start = std::chrono::high_resolution_clock::now();
                auto sparseModel = builder->build();
                auto end = std::chrono::high_resolution_clock::now();
                std::cout << "Building model took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms." << std::endl;
                
                // (7) delete the shared library
                boost::filesystem::remove(dynamicLibraryPath);
                
                // Return the constructed model.
                return std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>>(sparseModel);
            }
            
            template class ExplicitJitJaniModelBuilder<double, storm::models::sparse::StandardRewardModel<double>>;
            template class ExplicitJitJaniModelBuilder<storm::RationalNumber, storm::models::sparse::StandardRewardModel<storm::RationalNumber>>;
            template class ExplicitJitJaniModelBuilder<storm::RationalFunction, storm::models::sparse::StandardRewardModel<storm::RationalFunction>>;
            
        }
    }
}
