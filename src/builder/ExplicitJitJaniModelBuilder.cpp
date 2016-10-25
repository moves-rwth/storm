#include "src/builder/ExplicitJitJaniModelBuilder.h"

#include <iostream>
#include <cstdio>
#include <chrono>

#include "src/adapters/CarlAdapter.h"

#include "src/solver/SmtSolver.h"

#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/NotSupportedException.h"
#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace builder {
        
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
        ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::ExplicitJitJaniModelBuilder(storm::jani::Model const& model) : model(model) {
            // Intentionally left empty.
        }
        
        template <typename ValueType, typename RewardModelType>
        std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::createSourceCode() {
            std::string sourceTemplate = R"(
            
#define NDEBUG
            
#include <cstdint>
#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <boost/dll/alias.hpp>

#include "resources/3rdparty/sparsepp/sparsepp.h"

#include "src/builder/JitModelBuilderInterface.h"
#include "src/storage/SparseMatrix.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/StandardRewardModel.h"
            
namespace storm {
    namespace builder {
        
        typedef uint32_t IndexType;

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

namespace std {
    template <>
    struct hash<storm::builder::StateType> {
        std::size_t operator()(storm::builder::StateType const& state) const {
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
    
        typedef double ValueType;
        
        static bool model_is_deterministic() {
            return {$deterministic_model};
        }
        
        static bool model_is_discrete_time() {
            return {$discrete_time_model};
        }
        
        class Entry {
        public:
            Entry() : column(0), value(0) {
                // Intentionally left empty.
            }
            
            Entry(IndexType const& column, ValueType const& value) : column(column), value(value) {
                // Intentionally left empty.
            }
            
            IndexType const& getColumn() const {
                return column;
            }
            
            ValueType const& getValue() const {
                return value;
            }
            
            void addToValue(ValueType const& value) {
                this->value += value;
            }
            
            void scaleValue(ValueType const& divisor) {
                this->value /= divisor;
            }
            
        private:
            IndexType column;
            ValueType value;
        };
        
        class Choice {
        public:
            typedef std::vector<Entry> ContainerType;
            
            void add(IndexType const& stateIndex, ValueType const& value) {
                distribution.push_back(Entry(stateIndex, value));
            }

            void add(Choice&& choice) {
                distribution.insert(distribution.end(), std::make_move_iterator(choice.getDistribution().begin()), std::make_move_iterator(choice.getDistribution().end()));
            }

            void makeUniqueAndShrink() {
                std::sort(distribution.begin(), distribution.end(),
                          [] (Entry const& a, Entry const& b) {
                              return a.getColumn() < b.getColumn();
                          }
                          );
                
                // Code taken from std::unique and modified to fit needs.
                auto first = distribution.begin();
                auto last = distribution.end();
                
                if (first != last) {
                    auto result = first;
                    while (++first != last) {
                        if (!(result->getColumn() == first->getColumn())) {
                            if (++result != first) {
                                *result = std::move(*first);
                            }
                        } else {
                            result->addToValue(first->getValue());
                        }
                    }
                    ++result;
                    
                    distribution.resize(std::distance(distribution.begin(), result));
                }
            }
            
            ContainerType const& getDistribution() const {
                return distribution;
            }
            
            ContainerType& getDistribution() {
                return distribution;
            }
            
        private:
            ContainerType distribution;
        };
        
        class StateSet {
        public:
            StateType const& peek() const {
                return storage.back();
            }
            
            StateType get() {
                StateType result = std::move(storage.back());
                storage.pop_back();
                return result;
            }
            
            void add(StateType const& state) {
                storage.push_back(state);
            }
            
            bool empty() const {
                return storage.empty();
            }
            
        private:
            std::vector<StateType> storage;
        };
        
        struct ModelComponents {
            ModelComponents() : transitionMatrixBuilder(0, 0, 0, true, !model_is_deterministic()), stateLabeling(), rewardModels(), choiceLabeling() {
                // Intentionally left empty.
            }
            
            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder;
            storm::models::sparse::StateLabeling stateLabeling;
            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ValueType>> rewardModels;
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> choiceLabeling;
            
            storm::models::sparse::Model<ValueType>* createModel() {
                if (model_is_discrete_time()) {
                    if (model_is_deterministic()) {
                        return new storm::models::sparse::Dtmc<ValueType>(std::move(this->transitionMatrixBuilder.build()), std::move(this->stateLabeling), std::move(this->rewardModels), std::move(this->choiceLabeling));
                    } else {
                        return nullptr;
                    }
                } else {
                    return nullptr;
                }
            }
        };
                    
        class JitBuilder : public JitModelBuilderInterface<ValueType> {
        public:
            JitBuilder() : currentRow(0), modelComponents() {
                {% for state in initialStates %}{
                    StateType state;
                    {% for assignment in state %}state.{$assignment.variable} = {$assignment.value};
                    {% endfor %}initialStates.push_back(state);
                }
                {% endfor %}
            }
            
            virtual storm::models::sparse::Model<ValueType>* build() override {
                std::cout << "starting building process" << std::endl;
                explore(initialStates);
                std::cout << "finished building process with " << stateIds.size() << " states" << std::endl;
                return modelComponents.createModel();
            }
            
            void explore(std::vector<StateType> const& initialStates) {
                for (auto const& state : initialStates) {
                    explore(state);
                }
            }
            
            void explore(StateType const& initialState) {
                StateSet statesToExplore;
                getOrAddIndex(initialState, statesToExplore);

                std::vector<Choice> choices;
                while (!statesToExplore.empty()) {
                    StateType currentState = statesToExplore.get();
                    IndexType currentIndex = getIndex(currentState);
#ifndef NDEBUG
                    std::cout << "Exploring state " << currentState << ", id " << currentIndex << std::endl;
#endif
                    
                    exploreNonSynchronizingEdges(currentState, currentIndex, choices, statesToExplore);
                    
                    addChoicesForState(currentIndex, choices);
                    choices.clear();
                }
            }
            
            void exploreNonSynchronizingEdges(StateType const& sourceState, IndexType const& currentIndex, std::vector<Choice>& choices, StateSet& statesToExplore) {
                {% for edge in nonSynchronizingEdges %}if ({$edge.guard}) {
                    choices.emplace_back();
                    {% for destination in edge.destinations %}{
                        StateType targetState(sourceState);
                        {% for assignment in destination.nonTransientAssignments %}targetState.{$assignment.variable} = {$assignment.value};
                        {% endfor %}
                        IndexType targetStateIndex = getOrAddIndex(targetState, statesToExplore);
                        choices.back().add(targetStateIndex, {$destination.value});
                    }
                    {% endfor %}
                }
                {% endfor %}
            }
            
            void addChoicesForState(IndexType const& stateId, std::vector<Choice>& choices) {
                // If there is more than one choice in a deterministic model, we need to combine the choices into one
                // global choice.
                bool choiceIsGuaranteedDistribution = false;
                if (model_is_deterministic() && choices.size() > 1) {
                    uint64_t choiceCount = choices.size();
                    
                    // We do this by adding the entries of choices after the first one to the first one and then making
                    // the distribution unique again.
                    for (auto it = ++choices.begin(), ite = choices.end(); it != ite; ++it) {
                        choices.front().add(std::move(*it));
                    }
                    choices.resize(1);
                    choices.front().makeUniqueAndShrink();
                    
                    // If we are dealing with a discrete-time model, we need to scale the probabilities such that they
                    // form a probability distribution.
                    if (model_is_discrete_time()) {
                        for (auto& element : choices.front().getDistribution()) {
                            element.scaleValue(choiceCount);
                        }
                    }
                    choiceIsGuaranteedDistribution = true;
                }
                
                for (auto& choice : choices) {
                    if (!choiceIsGuaranteedDistribution) {
                        // Create a proper distribution without duplicate entries.
                        choice.makeUniqueAndShrink();
                    }
                    
                    // Add the elements to the transition matrix.
                    for (auto const& element : choice.getDistribution()) {
                        modelComponents.transitionMatrixBuilder.addNextValue(currentRow, element.getColumn(), element.getValue());
                    }
                    
                    // Proceed to next row.
                    ++currentRow;
                }
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
            
            static JitModelBuilderInterface<ValueType>* create() {
                return new JitBuilder();
            }
                        
            private:
                spp::sparse_hash_map<StateType, IndexType> stateIds;
                std::vector<StateType> initialStates;
            
                uint64_t currentRow;
                ModelComponents modelComponents;
            };
        
        BOOST_DLL_ALIAS(storm::builder::JitBuilder::create, create_builder)
                    
    }
}
)";
            
            cpptempl::data_map modelData;
            modelData["stateVariables"] = generateStateVariables();
            cpptempl::data_list initialStates = generateInitialStates();
            modelData["initialStates"] = cpptempl::make_data(initialStates);
            cpptempl::data_list nonSynchronizingEdges = generateNonSynchronizingEdges();
            modelData["nonSynchronizingEdges"] = cpptempl::make_data(nonSynchronizingEdges);
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
                    locationVariable["name"] = getLocationVariableName(automaton);
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
            return automaton.getName() + variable.getName();
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
            builder = std::unique_ptr<JitModelBuilderInterface<ValueType>>(jitBuilderGetFunction());
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
            auto sparseModel = builder->build();
            
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
