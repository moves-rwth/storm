#include "src/builder/jit/ExplicitJitJaniModelBuilder.h"

#include <iostream>
#include <cstdio>
#include <chrono>

#include "src/adapters/CarlAdapter.h"

#include "src/solver/SmtSolver.h"
#include "src/storage/jani/AutomatonComposition.h"
#include "src/storage/jani/ParallelComposition.h"

#include "src/builder/RewardModelInformation.h"

#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidArgumentException.h"
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
                // Register all transient variables as transient.
                for (auto const& variable : this->model.getGlobalVariables().getTransientVariables()) {
                    transientVariables.insert(variable.getExpressionVariable());
                }

                // Construct vector of the automata to be put in parallel.
                storm::jani::Composition const& topLevelComposition = this->model.getSystemComposition();
                if (topLevelComposition.isAutomatonComposition()) {
                    parallelAutomata.push_back(this->model.getAutomaton(topLevelComposition.asAutomatonComposition().getAutomatonName()));
                } else {
                    STORM_LOG_ASSERT(topLevelComposition.isParallelComposition(), "Expected parallel composition.");
                    storm::jani::ParallelComposition const& parallelComposition = topLevelComposition.asParallelComposition();
                    
                    for (auto const& composition : parallelComposition.getSubcompositions()) {
                        STORM_LOG_ASSERT(composition->isAutomatonComposition(), "Expected flat parallel composition.");
                        parallelAutomata.push_back(this->model.getAutomaton(composition->asAutomatonComposition().getAutomatonName()));
                    }
                }
                
                for (auto& automaton : this->model.getAutomata()) {
                    automaton.pushEdgeAssignmentsToDestinations();
                }
                
                for (auto const& automaton : parallelAutomata) {
                    automatonToLocationVariable.emplace(automaton.get().getName(), this->model.getManager().declareFreshIntegerVariable(false, automaton.get().getName() + "_"));
                }
                
                // If the program still contains undefined constants and we are not in a parametric setting, assemble an appropriate error message.
#ifdef STORM_HAVE_CARL
                if (!std::is_same<ValueType, storm::RationalFunction>::value && this->model.hasUndefinedConstants()) {
#else
                    if (this->model.hasUndefinedConstants()) {
#endif
                        std::vector<std::reference_wrapper<storm::jani::Constant const>> undefinedConstants = this->model.getUndefinedConstants();
                        std::stringstream stream;
                        bool printComma = false;
                        for (auto const& constant : undefinedConstants) {
                            if (printComma) {
                                stream << ", ";
                            } else {
                                printComma = true;
                            }
                            stream << constant.get().getName() << " (" << constant.get().getType() << ")";
                        }
                        stream << ".";
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Model still contains these undefined constants: " + stream.str());
                    }
                    
#ifdef STORM_HAVE_CARL
                    else if (std::is_same<ValueType, storm::RationalFunction>::value && !this->model.undefinedConstantsAreGraphPreserving()) {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The input model contains undefined constants that influence the graph structure of the underlying model, which is not allowed.");
                    }
#endif
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
                
#include "src/builder/jit/StateSet.h"
#include "src/builder/jit/JitModelBuilderInterface.h"
#include "src/builder/jit/StateBehaviour.h"
#include "src/builder/jit/ModelComponentsBuilder.h"
#include "src/builder/RewardModelInformation.h"
                
                namespace storm {
                    namespace builder {
                        namespace jit {
                            
                            typedef uint32_t IndexType;
                            typedef double ValueType;
                            
                            struct StateType {
                                // Boolean variables.
                                {% for variable in nontransient_variables.boolean %}bool {$variable.name} : 1;
                                {% endfor %}
                                // Bounded integer variables.
                                {% for variable in nontransient_variables.boundedInteger %}uint64_t {$variable.name} : {$variable.numberOfBits};
                                {% endfor %}
                                // Unbounded integer variables.
                                {% for variable in nontransient_variables.unboundedInteger %}int64_t {$variable.name};
                                {% endfor %}
                                // Real variables.
                                {% for variable in nontransient_variables.real %}double {$variable.name};
                                {% endfor %}
                                // Location variables.
                                {% for variable in nontransient_variables.locations %}uint64_t {$variable.name} : {$variable.numberOfBits};
                                {% endfor %}
                            };
                            
                            bool operator==(StateType const& first, StateType const& second) {
                                bool result = true;
                                {% for variable in nontransient_variables.boolean %}result &= !(first.{$variable.name} ^ second.{$variable.name});
                                {% endfor %}
                                {% for variable in nontransient_variables.boundedInteger %}result &= first.{$variable.name} == second.{$variable.name};
                                {% endfor %}
                                {% for variable in nontransient_variables.unboundedInteger %}result &= first.{$variable.name} == second.{$variable.name};
                                {% endfor %}
                                {% for variable in nontransient_variables.real %}result &= first.{$variable.name} == second.{$variable.name};
                                {% endfor %}
                                {% for variable in nontransient_variables.locations %}result &= first.{$variable.name} == second.{$variable.name};
                                {% endfor %}
                                return result;
                            }
                            
                            std::ostream& operator<<(std::ostream& out, StateType const& in) {
                                out << "<";
                                {% for variable in nontransient_variables.boolean %}out << "{$variable.name}=" << std::boolalpha << in.{$variable.name} << ", " << std::noboolalpha;
                                {% endfor %}
                                {% for variable in nontransient_variables.boundedInteger %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in nontransient_variables.unboundedInteger %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in nontransient_variables.real %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in nontransient_variables.locations %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                out << ">";
                                return out;
                            }
                            
                            struct TransientVariables {
                                TransientVariables() {
                                    reset();
                                }
                                
                                void reset() {
                                    {% for variable in transient_variables.boolean %}{$variable.name} = {$variable.init};
                                    {% endfor %}
                                    {% for variable in transient_variables.boundedInteger %}{$variable.name} = {$variable.init};
                                    {% endfor %}
                                    {% for variable in transient_variables.unboundedInteger %}{$variable.name} = {$variable.init};
                                    {% endfor %}
                                    {% for variable in transient_variables.real %}{$variable.name} = {$variable.init};
                                    {% endfor %}
                                }
                                
                                // Boolean variables.
                                {% for variable in transient_variables.boolean %}bool {$variable.name} : 1;
                                {% endfor %}
                                // Bounded integer variables.
                                {% for variable in transient_variables.boundedInteger %}uint64_t {$variable.name} : {$variable.numberOfBits};
                                {% endfor %}
                                // Unbounded integer variables.
                                {% for variable in transient_variables.unboundedInteger %}int64_t {$variable.name};
                                {% endfor %}
                                // Real variables.
                                {% for variable in transient_variables.real %}double {$variable.name};
                                {% endfor %}
                            };
                            
                            std::ostream& operator<<(std::ostream& out, TransientVariables const& in) {
                                out << "<";
                                {% for variable in transient_variables.boolean %}out << "{$variable.name}=" << std::boolalpha << in.{$variable.name} << ", " << std::noboolalpha;
                                {% endfor %}
                                {% for variable in transient_variables.boundedInteger %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in transient_variables.unboundedInteger %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in transient_variables.real %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
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
                        std::size_t operator()(storm::builder::jit::StateType const& in) const {
                            // Note: this is faster than viewing the struct as a bit field and taking hash_combine of the bytes.
                            std::size_t seed = 0;
                            {% for variable in nontransient_variables.boolean %}spp::hash_combine(seed, in.{$variable.name});
                            {% endfor %}
                            {% for variable in nontransient_variables.boundedInteger %}spp::hash_combine(seed, in.{$variable.name});
                            {% endfor %}
                            {% for variable in nontransient_variables.unboundedInteger %}spp::hash_combine(seed, in.{$variable.name});
                            {% endfor %}
                            {% for variable in nontransient_variables.real %}spp::hash_combine(seed, in.{$variable.name});
                            {% endfor %}
                            {% for variable in nontransient_variables.locations %}spp::hash_combine(seed, in.{$variable.name});
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

                            // Non-synchronizing edges.
                            {% for edge in nonsynch_edges %}static bool edge_enabled_{$edge.name}(StateType const& in) {
                                if ({$edge.guard}) {
                                    return true;
                                }
                                return false;
                            }

                            static void edge_perform_{$edge.name}(StateType const& in, TransientVariables const& transientIn, TransientVariables& transientOut) {
                                {% for assignment in edge.transient_assignments %}transientOut.{$assignment.variable} = {$assignment.value};
                                {% endfor %}
                            }
                            
                            {% for destination in edge.destinations %}
                            static void destination_perform_level_{$edge.name}_{$destination.name}(int_fast64_t level, StateType const& in, StateType& out) {
                                {% for level in destination.levels %}if (level == {$level.index}) {
                                    {% for assignment in level.non_transient_assignments %}out.{$assignment.variable} = {$assignment.value};
                                    {% endfor %}
                                }
                                {% endfor %}
                            }
                            
                            static void destination_perform_{$edge.name}_{$destination.name}(StateType const& in, StateType& out) {
                                {% for level in destination.levels %}
                                destination_perform_level_{$edge.name}_{$destination.name}({$level.index}, in, out);
                                {% endfor %}
                            }

                            static void destination_perform_level_{$edge.name}_{$destination.name}(int_fast64_t level, StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut) {
                                {% for level in destination.levels %}if (level == {$level.index}) {
                                    {% for assignment in level.non_transient_assignments %}out.{$assignment.variable} = {$assignment.value};
                                    {% endfor %}
                                    {% for assignment in level.transient_assignments %}transientOut.{$assignment.variable} = {$assignment.value};
                                    {% endfor %}
                                }
                                {% endfor %}
                            }
                            
                            static void destination_perform_{$edge.name}_{$destination.name}(StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut) {
                                {% for level in destination.levels %}
                                destination_perform_level_{$edge.name}_{$destination.name}({$level.index}, in, out, transientIn, transientOut);
                                {% endfor %}
                            }
                            {% endfor %}{% endfor %}

                            // Synchronizing edges.
                            {% for edge in synch_edges %}static bool edge_enabled_{$edge.name}(StateType const& in) {
                                if ({$edge.guard}) {
                                    return true;
                                }
                                return false;
                            }
                            
                            static void edge_perform_{$edge.name}(StateType const& in, TransientVariables const& transientIn, TransientVariables& transientOut) {
                                {% for assignment in edge.transient_assignments %}transientOut.{$assignment.variable} = {$assignment.value};
                                {% endfor %}
                            }
                            
                            {% for destination in edge.destinations %}
                            static void destination_perform_level_{$edge.name}_{$destination.name}(int_fast64_t level, StateType const& in, StateType& out) {
                                {% for level in destination.levels %}if (level == {$level.index}) {
                                    {% for assignment in level.non_transient_assignments %}out.{$assignment.variable} = {$assignment.value};
                                    {% endfor %}
                                }
                                {% endfor %}
                            }
                            
                            static void destination_perform_{$edge.name}_{$destination.name}(StateType const& in, StateType& out) {
                                {% for level in destination.levels %}
                                destination_perform_level_{$edge.name}_{$destination.name}({$level.index}, in, out);
                                {% endfor %}
                            }
                            
                            static void destination_perform_level_{$edge.name}_{$destination.name}(int_fast64_t level, StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut) {
                                {% for level in destination.levels %}if (level == {$level.index}) {
                                    {% for assignment in level.non_transient_assignments %}out.{$assignment.variable} = {$assignment.value};
                                    {% endfor %}
                                    {% for assignment in level.transient_assignments %}transientOut.{$assignment.variable} = {$assignment.value};
                                    {% endfor %}
                                }
                                {% endfor %}
                            }
                            
                            static void destination_perform_{$edge.name}_{$destination.name}(StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut) {
                                {% for level in destination.levels %}
                                destination_perform_level_{$edge.name}_{$destination.name}({$level.index}, in, out, transientIn, transientOut);
                                {% endfor %}
                            }
                            {% endfor %}{% endfor %}
                            
                            typedef void (*DestinationLevelFunctionPtr)(int_fast64_t, StateType const&, StateType&, TransientVariables const&, TransientVariables&);
                            typedef void (*DestinationFunctionPtr)(StateType const&, StateType&, TransientVariables const&, TransientVariables&);
                            typedef void (*DestinationWithoutTransientLevelFunctionPtr)(int_fast64_t, StateType const&, StateType&);
                            typedef void (*DestinationWithoutTransientFunctionPtr)(StateType const&, StateType&);
                            
                            class Destination {
                            public:
                                Destination() : mLowestLevel(0), mHighestLevel(0), mValue(), destinationLevelFunction(nullptr), destinationFunction(nullptr), destinationWithoutTransientLevelFunction(nullptr), destinationWithoutTransientFunction(nullptr) {
                                    // Intentionally left empty.
                                }
                                
                                Destination(int_fast64_t lowestLevel, int_fast64_t highestLevel, ValueType const& value, DestinationLevelFunctionPtr destinationLevelFunction, DestinationFunctionPtr destinationFunction, DestinationWithoutTransientLevelFunctionPtr destinationWithoutTransientLevelFunction, DestinationWithoutTransientFunctionPtr destinationWithoutTransientFunction) : mLowestLevel(lowestLevel), mHighestLevel(highestLevel), mValue(value), destinationLevelFunction(destinationLevelFunction), destinationFunction(destinationFunction), destinationWithoutTransientLevelFunction(destinationWithoutTransientLevelFunction), destinationWithoutTransientFunction(destinationWithoutTransientFunction) {
                                    // Intentionally left empty.
                                }
                                
                                int_fast64_t lowestLevel() const {
                                    return mLowestLevel;
                                }

                                int_fast64_t highestLevel() const {
                                    return mHighestLevel;
                                }
                                
                                ValueType const& value() const {
                                    return mValue;
                                }
                                
                                void perform(int_fast64_t level, StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut) const {
                                    destinationLevelFunction(level, in, out, transientIn, transientOut);
                                }

                                void perform(StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut) const {
                                    destinationFunction(in, out, transientIn, transientOut);
                                }

                                void perform(int_fast64_t level, StateType const& in, StateType& out) const {
                                    destinationWithoutTransientLevelFunction(level, in, out);
                                }
                                
                                void perform(StateType const& in, StateType& out) const {
                                    destinationWithoutTransientFunction(in, out);
                                }

                            private:
                                int_fast64_t mLowestLevel;
                                int_fast64_t mHighestLevel;
                                ValueType mValue;
                                DestinationLevelFunctionPtr destinationLevelFunction;
                                DestinationFunctionPtr destinationFunction;
                                DestinationWithoutTransientLevelFunctionPtr destinationWithoutTransientLevelFunction;
                                DestinationWithoutTransientFunctionPtr destinationWithoutTransientFunction;
                            };

                            typedef bool (*EdgeEnabledFunctionPtr)(StateType const&);
                            typedef void (*EdgeTransientFunctionPtr)(StateType const&, TransientVariables const& transientIn, TransientVariables& out);
                            
                            class Edge {
                            public:
                                typedef std::vector<Destination> ContainerType;
                                
                                Edge() : edgeEnabledFunction(nullptr), edgeTransientFunction(nullptr) {
                                    // Intentionally left empty.
                                }
                                
                                Edge(EdgeEnabledFunctionPtr edgeEnabledFunction, EdgeTransientFunctionPtr edgeTransientFunction = nullptr) : edgeEnabledFunction(edgeEnabledFunction), edgeTransientFunction(edgeTransientFunction) {
                                    // Intentionally left empty.
                                }
                                
                                bool isEnabled(StateType const& in) const {
                                    return edgeEnabledFunction(in);
                                }
                                
                                void addDestination(Destination const& destination) {
                                    destinations.push_back(destination);
                                }

                                void addDestination(int_fast64_t lowestLevel, int_fast64_t highestLevel, ValueType const& value, DestinationLevelFunctionPtr destinationLevelFunction, DestinationFunctionPtr destinationFunction, DestinationWithoutTransientLevelFunctionPtr destinationWithoutTransientLevelFunction, DestinationWithoutTransientFunctionPtr destinationWithoutTransientFunction) {
                                    destinations.emplace_back(lowestLevel, highestLevel, value, destinationLevelFunction, destinationFunction, destinationWithoutTransientLevelFunction, destinationWithoutTransientFunction);
                                }

                                std::vector<Destination> const& getDestinations() const {
                                    return destinations;
                                }
                                
                                ContainerType::const_iterator begin() const {
                                    return destinations.begin();
                                }

                                ContainerType::const_iterator end() const {
                                    return destinations.end();
                                }
                                
                                void perform(StateType const& in, TransientVariables const& transientIn, TransientVariables& transientOut) const {
                                    edgeTransientFunction(in, transientIn, transientOut);
                                }
                                
                            private:
                                EdgeEnabledFunctionPtr edgeEnabledFunction;
                                EdgeTransientFunctionPtr edgeTransientFunction;
                                ContainerType destinations;
                            };
                            
                            void locations_perform(StateType const& in, TransientVariables const& transientIn, TransientVariables& transientOut) {
                                {% for location in locations %}if ({$location.guard}) {
                                    {% for assignment in location.assignments %}transientOut.{$assignment.variable} = {$assignment.value};{% endfor %}
                                }
                                {% endfor %}
                            }

                            class JitBuilder : public JitModelBuilderInterface<IndexType, ValueType> {
                            public:
                                JitBuilder(ModelComponentsBuilder<IndexType, ValueType>& modelComponentsBuilder) : JitModelBuilderInterface(modelComponentsBuilder) {
                                    {% for state in initialStates %}{
                                        StateType state;
                                        {% for assignment in state %}state.{$assignment.variable} = {$assignment.value};
                                        {% endfor %}
                                        initialStates.push_back(state);
                                    }{% endfor %}
                                    {% for edge in nonsynch_edges %}{
                                        edge_{$edge.name} = Edge(&edge_enabled_{$edge.name}, edge_perform_{$edge.name});
                                        {% for destination in edge.destinations %}edge_{$edge.name}.addDestination({$destination.lowestLevel}, {$destination.highestLevel}, {$destination.value}, &destination_perform_level_{$edge.name}_{$destination.name}, &destination_perform_{$edge.name}_{$destination.name}, &destination_perform_level_{$edge.name}_{$destination.name}, &destination_perform_{$edge.name}_{$destination.name});
                                        {% endfor %}
                                    }
                                    {% endfor %}
                                    {% for edge in synch_edges %}{
                                        edge_{$edge.name} = Edge(&edge_enabled_{$edge.name}, edge_perform_{$edge.name});
                                        {% for destination in edge.destinations %}edge_{$edge.name}.addDestination({$destination.lowestLevel}, {$destination.highestLevel}, {$destination.value}, &destination_perform_level_{$edge.name}_{$destination.name}, &destination_perform_{$edge.name}_{$destination.name}, &destination_perform_level_{$edge.name}_{$destination.name}, &destination_perform_{$edge.name}_{$destination.name});
                                        {% endfor %}
                                    }
                                    {% endfor %}
                                    {% for reward in rewards %}
                                    modelComponentsBuilder.registerRewardModel(RewardModelInformation("{$reward.name}", {$reward.location_rewards}, {$reward.edge_rewards} || {$reward.destination_rewards}, false));
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
                                        auto const& in = stateEntry.first;
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
                                    StateSet<StateType> statesToExplore;
                                    getOrAddIndex(initialState, statesToExplore);
                                    
                                    StateBehaviour<IndexType, ValueType> behaviour;
                                    while (!statesToExplore.empty()) {
                                        StateType currentState = statesToExplore.get();
                                        IndexType currentIndex = getIndex(currentState);
                                        
                                        if (!isTerminalState(currentState)) {
#ifndef NDEBUG
                                            std::cout << "Exploring state " << currentState << ", id " << currentIndex << std::endl;
#endif
                                        
                                            behaviour.setExpanded();
                                            
                                            // Perform transient location assignments.
                                            TransientVariables transientIn;
                                            TransientVariables transientOut;
                                            locations_perform(currentState, transientIn, transientOut);
                                            {% for reward in location_rewards %}
                                            behaviour.addStateReward(transientOut.{$reward.variable});
                                            {% endfor %}
                                            
                                            // Explore all edges that do not take part in synchronization vectors.
                                            exploreNonSynchronizingEdges(currentState, behaviour, statesToExplore);
                                            
                                            // Explore all edges that participate in synchronization vectors.
                                            exploreSynchronizingEdges(currentState, behaviour, statesToExplore);
                                        }
                                            
                                        this->addStateBehaviour(currentIndex, behaviour);
                                        behaviour.clear();
                                    }
                                }
                                
                                bool isTerminalState(StateType const& state) const {
                                    {% for expression in terminalExpressions %}if ({$expression}) {
                                        return true;
                                    }
                                    {% endfor %}
                                    return false;
                                }
                                
                                void exploreNonSynchronizingEdges(StateType const& in, StateBehaviour<IndexType, ValueType>& behaviour, StateSet<StateType>& statesToExplore) {
                                    {% for edge in nonsynch_edges %}{
                                        if ({$edge.guard}) {
                                            Choice<IndexType, ValueType>& choice = behaviour.addChoice();
                                            choice.resizeRewards({$edge_destination_rewards_count}, 0);
                                            {
                                                TransientVariables transient;
                                                {% if edge.transient_assignments %}
                                                edge_perform_{$edge.name}(in, transient, transient);
                                                {% endif %}
                                                {% for reward in edge_rewards %}
                                                choice.addReward({$reward.index}, transient.{$reward.variable});
                                                {% endfor %}
                                            }
                                            {% for destination in edge.destinations %}{
                                                StateType out(in);
                                                TransientVariables transientIn;
                                                TransientVariables transientOut;
                                                destination_perform_{$edge.name}_{$destination.name}(in, out{% if edge.transient_variables_in_destinations %}, transientIn, transientOut{% endif %});
                                                IndexType outStateIndex = getOrAddIndex(out, statesToExplore);
                                                choice.add(outStateIndex, {$destination.value});
                                                {% for reward in destination_rewards %}
                                                choice.addReward({$reward.index}, transientOut.{$reward.variable});
                                                {% endfor %}
                                            }
                                            {% endfor %}
                                        }
                                    }
                                    {% endfor %}
                                }
                                
                                {% for vector in synch_vectors %}{$vector.functions}
                                {% endfor %}

                                void exploreSynchronizingEdges(StateType const& state, StateBehaviour<IndexType, ValueType>& behaviour, StateSet<StateType>& statesToExplore) {
                                    {% for vector in synch_vectors %}{
                                        exploreSynchronizationVector_{$vector.index}(state, behaviour, statesToExplore);
                                    }
                                    {% endfor %}
                                }
                                
                                IndexType getOrAddIndex(StateType const& state, StateSet<StateType>& statesToExplore) {
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
                                
                                {% for edge in nonsynch_edges %}Edge edge_{$edge.name};
                                {% endfor %}
                                {% for edge in synch_edges %}Edge edge_{$edge.name};
                                {% endfor %}
                            };
                            
                            BOOST_DLL_ALIAS(storm::builder::jit::JitBuilder::create, create_builder)
                        }
                    }
                }
                )";
                
                cpptempl::data_map modelData;
                generateVariables(modelData);
                cpptempl::data_list initialStates = generateInitialStates();
                modelData["initialStates"] = cpptempl::make_data(initialStates);
                
                // We need to generate the reward information before the edges, because we already use it there.
                generateRewards(modelData);
                generateEdges(modelData);
                generateLocations(modelData);
                cpptempl::data_list labels = generateLabels();
                modelData["labels"] = cpptempl::make_data(labels);
                cpptempl::data_list terminalExpressions = generateTerminalExpressions();
                modelData["terminalExpressions"] = cpptempl::make_data(terminalExpressions);
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
                solver->add(model.getInitialStatesExpression(parallelAutomata));
                
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
                    for (auto const& automatonRef : parallelAutomata) {
                        storm::jani::Automaton const& automaton = automatonRef.get();
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
                    for (auto const& automaton : parallelAutomata) {
                        initialLocationsIterators.push_back(automaton.get().getInitialLocationIndices().cbegin());
                    }
                    
                    // Now iterate through all combinations of initial locations.
                    while (true) {
                        cpptempl::data_list completeAssignment(initialStateAssignment);
                        
                        for (uint64_t index = 0; index < initialLocationsIterators.size(); ++index) {
                            storm::jani::Automaton const& automaton = parallelAutomata[index].get();
                            if (automaton.getNumberOfLocations() > 1) {
                                completeAssignment.push_back(generateLocationAssignment(automaton, *initialLocationsIterators[index]));
                            }
                        }
                        initialStates.push_back(cpptempl::make_data(completeAssignment));
                        
                        uint64_t index = 0;
                        for (; index < initialLocationsIterators.size(); ++index) {
                            ++initialLocationsIterators[index];
                            if (initialLocationsIterators[index] == parallelAutomata[index].get().getInitialLocationIndices().cend()) {
                                initialLocationsIterators[index] = parallelAutomata[index].get().getInitialLocationIndices().cbegin();
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
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateBooleanVariable(storm::jani::BooleanVariable const& variable) {
                cpptempl::data_map result;
                result["name"] = registerVariable(variable.getExpressionVariable(), variable.isTransient());
                if (variable.hasInitExpression()) {
                    result["init"] = asString(variable.getInitExpression().evaluateAsBool());
                }
                return result;
            }

            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateBoundedIntegerVariable(storm::jani::BoundedIntegerVariable const& variable) {
                cpptempl::data_map result;

                int_fast64_t lowerBound = variable.getLowerBound().evaluateAsInt();
                int_fast64_t upperBound = variable.getUpperBound().evaluateAsInt();
                
                lowerBounds[variable.getExpressionVariable()] = lowerBound;
                if (lowerBound != 0) {
                    lowerBoundShiftSubstitution[variable.getExpressionVariable()] = variable.getExpressionVariable() + model.getManager().integer(lowerBound);
                }
                uint64_t range = static_cast<uint64_t>(upperBound - lowerBound + 1);
                uint64_t numberOfBits = static_cast<uint64_t>(std::ceil(std::log2(range)));
                
                result["name"] = registerVariable(variable.getExpressionVariable(), variable.isTransient());
                result["numberOfBits"] = std::to_string(numberOfBits);
                if (variable.hasInitExpression()) {
                    result["init"] = asString(variable.getInitExpression().evaluateAsInt() - lowerBound);
                }

                return result;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateUnboundedIntegerVariable(storm::jani::UnboundedIntegerVariable const& variable) {
                cpptempl::data_map result;
                
                result["name"] = registerVariable(variable.getExpressionVariable(), variable.isTransient());
                if (variable.hasInitExpression()) {
                    result["init"] = asString(variable.getInitExpression().evaluateAsInt());
                }
                
                return result;
            }

            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateRealVariable(storm::jani::RealVariable const& variable) {
                cpptempl::data_map result;
                
                result["name"] = registerVariable(variable.getExpressionVariable(), variable.isTransient());
                if (variable.hasInitExpression()) {
                    result["init"] = asString(variable.getInitExpression().evaluateAsDouble());
                }

                return result;
            }

            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateLocationVariable(storm::jani::Automaton const& automaton) {
                cpptempl::data_map result;
                
                result["name"] = registerVariable(getLocationVariable(automaton), false);
                result["numberOfBits"] = static_cast<uint64_t>(std::ceil(std::log2(automaton.getNumberOfLocations())));
                
                return result;
            }

            template <typename ValueType, typename RewardModelType>
            void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateVariables(cpptempl::data_map& modelData) {
                cpptempl::data_list nonTransientBooleanVariables;
                cpptempl::data_list transientBooleanVariables;
                cpptempl::data_list nonTransientBoundedIntegerVariables;
                cpptempl::data_list transientBoundedIntegerVariables;
                cpptempl::data_list nonTransientUnboundedIntegerVariables;
                cpptempl::data_list transientUnboundedIntegerVariables;
                cpptempl::data_list nonTransientRealVariables;
                cpptempl::data_list transientRealVariables;
                cpptempl::data_list locationVariables;
                
                for (auto const& variable : model.getGlobalVariables().getBooleanVariables()) {
                    cpptempl::data_map newBooleanVariable = generateBooleanVariable(variable.asBooleanVariable());
                    if (variable.isTransient()) {
                        transientBooleanVariables.push_back(newBooleanVariable);
                    } else {
                        nonTransientBooleanVariables.push_back(newBooleanVariable);
                    }
                }
                for (auto const& variable : model.getGlobalVariables().getBoundedIntegerVariables()) {
                    cpptempl::data_map newBoundedIntegerVariable = generateBoundedIntegerVariable(variable.asBoundedIntegerVariable());
                    if (variable.isTransient()) {
                        transientBoundedIntegerVariables.push_back(newBoundedIntegerVariable);
                    } else {
                        nonTransientBoundedIntegerVariables.push_back(newBoundedIntegerVariable);
                    }
                }
                for (auto const& variable : model.getGlobalVariables().getUnboundedIntegerVariables()) {
                    cpptempl::data_map newUnboundedIntegerVariable = generateUnboundedIntegerVariable(variable.asUnboundedIntegerVariable());
                    if (variable.isTransient()) {
                        transientUnboundedIntegerVariables.push_back(newUnboundedIntegerVariable);
                    } else {
                        nonTransientUnboundedIntegerVariables.push_back(newUnboundedIntegerVariable);
                    }
                }
                for (auto const& variable : model.getGlobalVariables().getRealVariables()) {
                    cpptempl::data_map newRealVariable = generateRealVariable(variable.asRealVariable());
                    if (variable.isTransient()) {
                        transientRealVariables.push_back(newRealVariable);
                    } else {
                        nonTransientRealVariables.push_back(newRealVariable);
                    }
                }
                for (auto const& automatonRef : parallelAutomata) {
                    storm::jani::Automaton const& automaton = automatonRef.get();
                    for (auto const& variable : automaton.getVariables().getBooleanVariables()) {
                        cpptempl::data_map newBooleanVariable = generateBooleanVariable(variable.asBooleanVariable());
                        if (variable.isTransient()) {
                            transientBooleanVariables.push_back(newBooleanVariable);
                        } else {
                            nonTransientBooleanVariables.push_back(newBooleanVariable);
                        }
                    }
                    for (auto const& variable : automaton.getVariables().getBoundedIntegerVariables()) {
                        cpptempl::data_map newBoundedIntegerVariable = generateBoundedIntegerVariable(variable.asBoundedIntegerVariable());
                        if (variable.isTransient()) {
                            transientBoundedIntegerVariables.push_back(newBoundedIntegerVariable);
                        } else {
                            nonTransientBoundedIntegerVariables.push_back(newBoundedIntegerVariable);
                        }
                    }
                    for (auto const& variable : automaton.getVariables().getUnboundedIntegerVariables()) {
                        cpptempl::data_map newUnboundedIntegerVariable = generateUnboundedIntegerVariable(variable.asUnboundedIntegerVariable());
                        if (variable.isTransient()) {
                            transientUnboundedIntegerVariables.push_back(newUnboundedIntegerVariable);
                        } else {
                            nonTransientUnboundedIntegerVariables.push_back(newUnboundedIntegerVariable);
                        }
                    }
                    for (auto const& variable : automaton.getVariables().getRealVariables()) {
                        cpptempl::data_map newRealVariable = generateRealVariable(variable.asRealVariable());
                        if (variable.isTransient()) {
                            transientRealVariables.push_back(newRealVariable);
                        } else {
                            nonTransientRealVariables.push_back(newRealVariable);
                        }
                    }
                    
                    // Only generate a location variable if there is more than one location for the automaton.
                    if (automaton.getNumberOfLocations() > 1) {
                        locationVariables.push_back(generateLocationVariable(automaton));
                    }
                }
                
                cpptempl::data_map nonTransientVariables;
                nonTransientVariables["boolean"] = cpptempl::make_data(nonTransientBooleanVariables);
                nonTransientVariables["boundedInteger"] = cpptempl::make_data(nonTransientBoundedIntegerVariables);
                nonTransientVariables["unboundedInteger"] = cpptempl::make_data(nonTransientUnboundedIntegerVariables);
                nonTransientVariables["real"] = cpptempl::make_data(nonTransientRealVariables);
                nonTransientVariables["locations"] = cpptempl::make_data(locationVariables);
                modelData["nontransient_variables"] = nonTransientVariables;
                
                cpptempl::data_map transientVariables;
                transientVariables["boolean"] = cpptempl::make_data(transientBooleanVariables);
                transientVariables["boundedInteger"] = cpptempl::make_data(transientBoundedIntegerVariables);
                transientVariables["unboundedInteger"] = cpptempl::make_data(transientUnboundedIntegerVariables);
                transientVariables["real"] = cpptempl::make_data(transientRealVariables);
                modelData["transient_variables"] = transientVariables;
            }
            
            template <typename ValueType, typename RewardModelType>
            void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateLocations(cpptempl::data_map& modelData) {
                cpptempl::data_list locations;

                for (auto const& automatonRef : parallelAutomata) {
                    storm::jani::Automaton const& automaton = automatonRef.get();
                    cpptempl::data_map locationData;
                    uint64_t locationIndex = 0;
                    for (auto const& location : automaton.getLocations()) {
                        cpptempl::data_list assignments;
                        for (auto const& assignment : location.getAssignments()) {
                            assignments.push_back(generateAssignment(assignment));
                        }
                        locationData["assignments"] = cpptempl::make_data(assignments);
                        if (automaton.getNumberOfLocations() > 1) {
                            locationData["guard"] = expressionTranslator.translate(shiftVariablesWrtLowerBound(getLocationVariable(automaton) == this->model.getManager().integer(locationIndex)), storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName));
                        } else {
                            locationData["guard"] = asString(true);
                        }
                        ++locationIndex;
                    }
                    if (!locationData["assignments"]->empty()) {
                        locations.push_back(locationData);
                    }
                }
                    
                modelData["locations"] = cpptempl::make_data(locations);
            }
                
            template <typename ValueType, typename RewardModelType>
            void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateRewards(cpptempl::data_map& modelData) {
                // Extract the reward models from the program based on the names we were given.
                std::vector<storm::expressions::Variable> rewardVariables;
                auto const& globalVariables = model.getGlobalVariables();
                for (auto const& rewardModelName : this->options.getRewardModelNames()) {
                    if (globalVariables.hasVariable(rewardModelName)) {
                        rewardVariables.push_back(globalVariables.getVariable(rewardModelName).getExpressionVariable());
                    } else {
                        STORM_LOG_THROW(rewardModelName.empty(), storm::exceptions::InvalidArgumentException, "Cannot build unknown reward model '" << rewardModelName << "'.");
                        STORM_LOG_THROW(globalVariables.getNumberOfRealTransientVariables() + globalVariables.getNumberOfUnboundedIntegerTransientVariables() == 1, storm::exceptions::InvalidArgumentException, "Reference to standard reward model is ambiguous.");
                    }
                }
                
                // If no reward model was yet added, but there was one that was given in the options, we try to build the
                // standard reward model.
                if (rewardVariables.empty() && !this->options.getRewardModelNames().empty()) {
                    bool foundTransientVariable = false;
                    for (auto const& transientVariable : globalVariables.getTransientVariables()) {
                        if (transientVariable.isUnboundedIntegerVariable() || transientVariable.isRealVariable()) {
                            rewardVariables.push_back(transientVariable.getExpressionVariable());
                            foundTransientVariable = true;
                            break;
                        }
                    }
                    STORM_LOG_ASSERT(foundTransientVariable, "Expected to find a fitting transient variable.");
                }

                std::vector<storm::builder::RewardModelInformation> rewardModels;
                cpptempl::data_list rewards;
                cpptempl::data_list locationRewards;
                cpptempl::data_list edgeRewards;
                cpptempl::data_list destinationRewards;
                uint64_t rewardModelIndex = 0;
                uint64_t stateActionRewardCount = 0;
                for (auto const& variable : rewardVariables) {
                    bool hasLocationRewards = false;
                    bool hasEdgeRewards = false;
                    bool hasDestinationRewards = false;
                    
                    for (auto const& automatonRef : parallelAutomata) {
                        storm::jani::Automaton const& automaton = automatonRef.get();
                        
                        for (auto const& location : automaton.getLocations()) {
                            for (auto const& assignment : location.getAssignments()) {
                                if (assignment.getExpressionVariable() == variable) {
                                    hasLocationRewards = true;
                                    break;
                                }
                            }
                        }
                        
                        for (auto const& edge : automaton.getEdges()) {
                            for (auto const& assignment : edge.getAssignments()) {
                                if (assignment.getExpressionVariable() == variable) {
                                    hasEdgeRewards = true;
                                }
                            }
                            
                            for (auto const& destination : edge.getDestinations()) {
                                for (auto const& assignment : destination.getOrderedAssignments()) {
                                    if (assignment.getExpressionVariable() == variable) {
                                        hasDestinationRewards = true;
                                    }
                                }
                            }
                        }
                    }
                    
                    rewardModels.emplace_back(variable.getName(), hasLocationRewards, hasEdgeRewards || hasDestinationRewards, false);
                    
                    if (hasEdgeRewards || hasDestinationRewards) {
                        ++stateActionRewardCount;
                    }
                    
                    cpptempl::data_map reward;
                    reward["name"] = variable.getName();
                    reward["location_rewards"] = asString(hasLocationRewards);
                    reward["edge_rewards"] = asString(hasEdgeRewards);
                    reward["destination_rewards"] = asString(hasDestinationRewards);
                    rewards.push_back(reward);
                    
                    if (hasLocationRewards) {
                        cpptempl::data_map locationReward;
                        locationReward["variable"] = variable.getName();
                        locationRewards.push_back(locationReward);
                    }
                    if (hasEdgeRewards) {
                        cpptempl::data_map edgeReward;
                        edgeReward["variable"] = variable.getName();
                        edgeReward["index"] = asString(rewardModelIndex);
                        edgeRewards.push_back(edgeReward);
                    }
                    if (hasDestinationRewards) {
                        cpptempl::data_map destinationReward;
                        destinationReward["index"] = asString(rewardModelIndex);
                        destinationReward["variable"] = variable.getName();
                        destinationRewards.push_back(destinationReward);
                    }
                    ++rewardModelIndex;
                }
                
                modelData["location_rewards"] = cpptempl::make_data(locationRewards);
                modelData["edge_rewards"] = cpptempl::make_data(edgeRewards);
                modelData["destination_rewards"] = cpptempl::make_data(destinationRewards);
                modelData["rewards"] = cpptempl::make_data(rewards);
                modelData["edge_destination_rewards_count"] = asString(stateActionRewardCount);
            }
                
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_list ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateLabels() {
                cpptempl::data_list labels;
                
                // As in JANI we can use transient boolean variable assignments in locations to identify states, we need to
                // create a list of boolean transient variables and the expressions that define them.
                for (auto const& variable : model.getGlobalVariables().getTransientVariables()) {
                    if (variable.isBooleanVariable()) {
                        if (this->options.isBuildAllLabelsSet() || this->options.getLabelNames().find(variable.getName()) != this->options.getLabelNames().end()) {
                            cpptempl::data_map label;
                            label["name"] = variable.getName();
                            label["predicate"] = expressionTranslator.translate(shiftVariablesWrtLowerBound(model.getLabelExpression(variable.asBooleanVariable(), automatonToLocationVariable)), storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName));
                            labels.push_back(label);
                        }
                    }
                }
                
                for (auto const& expression : this->options.getExpressionLabels()) {
                    cpptempl::data_map label;
                    label["name"] = expression.toString();
                    label["predicate"] = expressionTranslator.translate(shiftVariablesWrtLowerBound(expression), storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName));
                    labels.push_back(label);
                }
                
                return labels;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_list ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateTerminalExpressions() {
                cpptempl::data_list terminalExpressions;
                
                for (auto const& terminalEntry : options.getTerminalStates()) {
                    LabelOrExpression const& labelOrExpression = terminalEntry.first;
                    if (labelOrExpression.isLabel()) {
                        auto const& variables = model.getGlobalVariables();
                        STORM_LOG_THROW(variables.hasVariable(labelOrExpression.getLabel()), storm::exceptions::WrongFormatException, "Terminal label refers to unknown identifier '" << labelOrExpression.getLabel() << "'.");
                        auto const& variable = variables.getVariable(labelOrExpression.getLabel());
                        STORM_LOG_THROW(variable.isBooleanVariable(), storm::exceptions::WrongFormatException, "Terminal label refers to non-boolean variable '" << variable.getName() << ".");
                        STORM_LOG_THROW(variable.isTransient(), storm::exceptions::WrongFormatException, "Terminal label refers to non-transient variable '" << variable.getName() << ".");
                        auto labelExpression = model.getLabelExpression(variable.asBooleanVariable(), automatonToLocationVariable);
                        if (terminalEntry.second) {
                            labelExpression = !labelExpression;
                        }
                        terminalExpressions.push_back(expressionTranslator.translate(shiftVariablesWrtLowerBound(labelExpression), storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName)));
                    } else {
                        auto expression = terminalEntry.second ? labelOrExpression.getExpression() : !labelOrExpression.getExpression();
                        terminalExpressions.push_back(expressionTranslator.translate(shiftVariablesWrtLowerBound(expression), storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName)));
                    }
                }

                return terminalExpressions;
            }

            std::ostream& indent(std::ostream& out, uint64_t indentLevel) {
                for (uint64_t i = 0; i < indentLevel; ++i) {
                    out << "\t";
                }
                return out;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateSynchronizationVector(cpptempl::data_map& modelData, storm::jani::ParallelComposition const& parallelComposition, storm::jani::SynchronizationVector const& synchronizationVector, uint64_t synchronizationVectorIndex) {
                std::stringstream vectorSource;
                uint64_t numberOfActionInputs = synchronizationVector.getNumberOfActionInputs();
                
                // First, we check whether we need to generate code for a) different assignment levels and b) transient variables.
                uint64_t position = 0;
                int_fast64_t lowestLevel;
                int_fast64_t highestLevel;
                bool firstDestination = true;
                bool generateTransient = false;
                for (auto const& inputActionName : synchronizationVector.getInput()) {
                    if (!storm::jani::SynchronizationVector::isNoActionInput(inputActionName)) {
                        storm::jani::Automaton const& automaton = model.getAutomaton(parallelComposition.getSubcomposition(position).asAutomatonComposition().getAutomatonName());
                        uint64_t actionIndex = model.getActionIndex(inputActionName);
                        for (auto const& edge : automaton.getEdges()) {
                            if (edge.getActionIndex() == actionIndex) {
                                for (auto const& destination : edge.getDestinations()) {
                                    if (!destination.getOrderedAssignments().empty()) {
                                        if (firstDestination) {
                                            lowestLevel = destination.getOrderedAssignments().getLowestLevel();
                                            highestLevel = destination.getOrderedAssignments().getHighestLevel();
                                            firstDestination = false;
                                        } else {
                                            lowestLevel = std::min(lowestLevel, destination.getOrderedAssignments().getLowestLevel());
                                            highestLevel = std::max(highestLevel, destination.getOrderedAssignments().getHighestLevel());
                                        }
                                    }
                                    if (!generateTransient) {
                                        for (auto const& assignment : destination.getOrderedAssignments()) {
                                            if (assignment.isTransient()) {
                                                generateTransient = true;
                                            }
                                            std::set<storm::expressions::Variable> usedVariables = assignment.getAssignedExpression().getVariables();
                                            for (auto const& variable : usedVariables) {
                                                if (transientVariables.find(variable) != transientVariables.end()) {
                                                    generateTransient = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    ++position;
                }
                bool generateLevelCode = lowestLevel != highestLevel;
                
                uint64_t indentLevel = 4;
                indent(vectorSource, indentLevel - 4) << "void performSynchronizedDestinations_" << synchronizationVectorIndex << "(StateType const& in, StateBehaviour<IndexType, ValueType>& behaviour, StateSet<StateType>& statesToExplore, ";
                for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
                    vectorSource << "Destination const& destination" << index << ", ";
                }
                if (generateLevelCode) {
                    vectorSource << "int64_t lowestLevel, int64_t highestLevel, ";
                }
                vectorSource << "Choice<IndexType, ValueType>& choice) {" << std::endl;
                indent(vectorSource, indentLevel + 1) << "StateType out(in);" << std::endl;
                indent(vectorSource, indentLevel + 1) << "TransientVariables transientIn;" << std::endl;
                indent(vectorSource, indentLevel + 1) << "TransientVariables transientOut;" << std::endl;
                
                if (generateLevelCode) {
                    indent(vectorSource, indentLevel + 1) << "for (int64_t level = lowestLevel; level <= highestLevel; ++level) {" << std::endl;
                    ++indentLevel;
                }
                for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
                    indent(vectorSource, indentLevel + 1) << "destination" << index << ".perform(";
                    if (generateLevelCode) {
                        vectorSource << "level, ";
                    }
                    vectorSource << "in, out, transientIn, transientOut);" << std::endl;
                }
                if (generateLevelCode) {
                    --indentLevel;
                    indent(vectorSource, indentLevel + 1) << "}" << std::endl;
                }
                indent(vectorSource, indentLevel + 1) << "IndexType outStateIndex = getOrAddIndex(out, statesToExplore);" << std::endl;
                indent(vectorSource, indentLevel + 1) << "ValueType probability = ";
                for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
                    vectorSource << "destination" << index << ".value()";
                    if (index + 1 < numberOfActionInputs) {
                        vectorSource << " * ";
                    }
                }
                vectorSource << ";" << std::endl;
                indent(vectorSource, indentLevel + 1) << "choice.add(outStateIndex, probability);" << std::endl;
                
                std::stringstream tmp;
                indent(tmp, indentLevel + 1) << "{% for reward in destination_rewards %}choice.addReward({$reward.index}, probability * transientOut.{$reward.variable});" << std::endl;
                indent(tmp, indentLevel + 1) << "{% endfor %}" << std::endl;
                indent(vectorSource, indentLevel) << "}" << std::endl << std::endl;
                
                indent(vectorSource, indentLevel) << "void performSynchronizedDestinations_" << synchronizationVectorIndex << "(StateType const& in, StateBehaviour<IndexType, ValueType>& behaviour, StateSet<StateType>& statesToExplore, ";
                for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
                    vectorSource << "Edge const& edge" << index << ", ";
                }
                vectorSource << "Choice<IndexType, ValueType>& choice) {" << std::endl;
                if (generateLevelCode) {
                    indent(vectorSource, indentLevel + 1) << "int64_t lowestLevel; int64_t highestLevel;";
                }
                for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
                    indent(vectorSource, indentLevel + 1 + index) << "for (auto const& destination" << index << " : edge" << index << ") {" << std::endl;
                    if (generateLevelCode) {
                        if (index == 0) {
                            indent(vectorSource, indentLevel + 2 + index) << "lowestLevel = edge0.lowestLevel();" << std::endl;
                            indent(vectorSource, indentLevel + 2 + index) << "highestLevel = edge0.highestLevel();" << std::endl;
                        } else {
                            indent(vectorSource, indentLevel + 2 + index) << "lowestLevel = std::min(lowestLevel, edge0.lowestLevel());" << std::endl;
                            indent(vectorSource, indentLevel + 2 + index) << "highestLevel = std::max(highestLevel, edge0.highestLevel());" << std::endl;
                        }
                    }
                }
                indent(vectorSource, indentLevel + 1 + numberOfActionInputs) << "performSynchronizedDestinations_" << synchronizationVectorIndex << "(in, behaviour, statesToExplore, ";
                for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
                    vectorSource << "destination" << index << ", ";
                }
                if (generateLevelCode) {
                    vectorSource << "lowestLevel, highestLevel, ";
                }
                vectorSource << "choice);" << std::endl;
                for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
                    indent(vectorSource, indentLevel + numberOfActionInputs - index) << "}" << std::endl;
                }
                indent(vectorSource, indentLevel) << "}" << std::endl << std::endl;
            
                for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
                    indent(vectorSource, indentLevel) << "void performSynchronizedEdges_" << synchronizationVectorIndex << "_" << index << "(StateType const& in, std::vector<std::vector<std::reference_wrapper<Edge const>>> const& edges,  StateBehaviour<IndexType, ValueType>& behaviour, StateSet<StateType>& statesToExplore";
                    if (index > 0) {
                        vectorSource << ", ";
                    }
                    for (uint64_t innerIndex = 0; innerIndex < index; ++innerIndex) {
                        vectorSource << "Edge const& edge" << innerIndex;
                        if (innerIndex + 1 < index) {
                            vectorSource << ", ";
                        }
                    }
                    if (index > 0) {
                        vectorSource << ", TransientVariables const& transientIn, TransientVariables& transientOut";
                    }
                    vectorSource << ") {" << std::endl;
                    if (index == 0) {
                        indent(vectorSource, indentLevel + 1) << "TransientVariables transientIn;" << std::endl;
                        indent(vectorSource, indentLevel + 1) << "TransientVariables transientOut;" << std::endl;
                    }
                    indent(vectorSource, indentLevel + 1) << "for (auto const& edge" << index << " : edges[" << index << "]) {" << std::endl;
                    indent(vectorSource, indentLevel + 2) << "edge" << index << ".get().perform(in, transientIn, transientOut);" << std::endl;
                    if (index + 1 < numberOfActionInputs) {
                        indent(vectorSource, indentLevel + 2) << "performSynchronizedEdges_" << synchronizationVectorIndex << "_" << (index + 1) << "(in, edges, behaviour, statesToExplore, ";
                        for (uint64_t innerIndex = 0; innerIndex <= index; ++innerIndex) {
                            vectorSource << "edge" << innerIndex << ", ";
                        }
                        vectorSource << "transientIn, transientOut);" << std::endl;
                    } else {
                        indent(vectorSource, indentLevel + 2) << "Choice<IndexType, ValueType>& choice = behaviour.addChoice();" << std::endl;
                        
                        std::stringstream tmp;
                        indent(tmp, indentLevel + 2) << "choice.resizeRewards({$edge_destination_rewards_count}, 0);" << std::endl;
                        indent(tmp, indentLevel + 2) << "{% for reward in edge_rewards %}choice.addReward({$reward.index}, transientOut.{$reward.variable});" << std::endl;
                        indent(tmp, indentLevel + 2) << "{% endfor %}" << std::endl;
                        
                        vectorSource << cpptempl::parse(tmp.str(), modelData);
                        
                        indent(vectorSource, indentLevel + 2) << "performSynchronizedDestinations_" << synchronizationVectorIndex << "(in, behaviour, statesToExplore, ";
                        for (uint64_t innerIndex = 0; innerIndex <= index; ++innerIndex) {
                            vectorSource << "edge" << innerIndex << ", ";
                        }
                        vectorSource << " choice);" << std::endl;

                    }
                    indent(vectorSource, indentLevel + 1) << "}" << std::endl;
                    indent(vectorSource, indentLevel) << "}" << std::endl << std::endl;
                }
                
                indent(vectorSource, indentLevel) << "void get_edges_" << synchronizationVectorIndex << "(StateType const& state, std::vector<std::reference_wrapper<Edge const>>& edges, uint64_t position) {" << std::endl;
                position = 0;
                uint64_t participatingPosition = 0;
                for (auto const& inputActionName : synchronizationVector.getInput()) {
                    if (!storm::jani::SynchronizationVector::isNoActionInput(inputActionName)) {
                        indent(vectorSource, indentLevel + 1) << "if (position == " << participatingPosition << ") {" << std::endl;
                        
                        storm::jani::Automaton const& automaton = model.getAutomaton(parallelComposition.getSubcomposition(position).asAutomatonComposition().getAutomatonName());
                        uint64_t actionIndex = model.getActionIndex(inputActionName);
                        uint64_t edgeIndex = 0;
                        for (auto const& edge : automaton.getEdges()) {
                            if (edge.getActionIndex() == actionIndex) {
                                std::string edgeName = automaton.getName() + "_" + std::to_string(edgeIndex);
                                indent(vectorSource, indentLevel + 2) << "if (edge_enabled_" << edgeName  << "(state)) {" << std::endl;
                                indent(vectorSource, indentLevel + 3) << "edges.emplace_back(edge_" << edgeName << ");" << std::endl;
                                indent(vectorSource, indentLevel + 2) << "}" << std::endl;
                            }
                            ++edgeIndex;
                        }
                        
                        indent(vectorSource, indentLevel + 1) << "}" << std::endl;
                        ++participatingPosition;
                    }
                    ++position;
                }
                indent(vectorSource, indentLevel) << "}" << std::endl << std::endl;
                
                indent(vectorSource, indentLevel) << "void exploreSynchronizationVector_" << synchronizationVectorIndex << "(StateType const& state, StateBehaviour<IndexType, ValueType>& behaviour, StateSet<StateType>& statesToExplore) {" << std::endl;
                indent(vectorSource, indentLevel + 1) << "std::vector<std::vector<std::reference_wrapper<Edge const>>> edges(" << synchronizationVector.getNumberOfActionInputs() << ");" << std::endl;
                
                participatingPosition = 0;
                for (auto const& input : synchronizationVector.getInput()) {
                    if (!storm::jani::SynchronizationVector::isNoActionInput(input)) {
                        indent(vectorSource, indentLevel + 1) << "get_edges_" << synchronizationVectorIndex << "(state, edges[" << participatingPosition << "], " << participatingPosition << ");" << std::endl;
                        indent(vectorSource, indentLevel + 1) << "if (edges[" << participatingPosition << "].empty()) {" << std::endl;
                        indent(vectorSource, indentLevel + 2) << "return;" << std::endl;
                        indent(vectorSource, indentLevel + 1) << "}" << std::endl;
                        ++participatingPosition;
                    }
                }
                indent(vectorSource, indentLevel + 1) << "performSynchronizedEdges_" << synchronizationVectorIndex << "_0(state, edges, behaviour, statesToExplore);" << std::endl;
                indent(vectorSource, indentLevel) << "}" << std::endl << std::endl;
                
                cpptempl::data_map vector;
                vector["functions"] = vectorSource.str();
                vector["index"] = asString(synchronizationVectorIndex);
                return vector;
            }
            
            template <typename ValueType, typename RewardModelType>
            void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateEdges(cpptempl::data_map& modelData) {
                STORM_LOG_THROW(model.hasStandardCompliantComposition(), storm::exceptions::WrongFormatException, "Model builder only supports non-nested parallel compositions.");
                
                cpptempl::data_list nonSynchronizingEdges;
                cpptempl::data_list synchronizingEdges;
                cpptempl::data_list vectors;
                
                storm::jani::Composition const& topLevelComposition = model.getSystemComposition();
                if (topLevelComposition.isAutomatonComposition()) {
                    storm::jani::Automaton const& automaton = model.getAutomaton(topLevelComposition.asAutomatonComposition().getAutomatonName());
                    uint64_t edgeIndex = 0;
                    for (auto const& edge : automaton.getEdges()) {
                        nonSynchronizingEdges.push_back(generateEdge(automaton, edgeIndex, edge));
                        ++edgeIndex;
                    }
                } else {
                    storm::jani::ParallelComposition const& parallelComposition = topLevelComposition.asParallelComposition();
                    
                    std::vector<std::set<uint64_t>> synchronizingActions(parallelComposition.getNumberOfSubcompositions());
                    uint64_t synchronizationVectorIndex = 0;
                    for (auto const& synchronizationVector : parallelComposition.getSynchronizationVectors()) {
                        // If the synchronization vector has at most one action input, there is no synchronization going on.
                        if (synchronizationVector.getNumberOfActionInputs() <= 1) {
                            continue;
                        }
                        
                        bool createVector = true;
                        uint64_t position = 0;
                        for (auto const& inputActionName : synchronizationVector.getInput()) {
                            if (!storm::jani::SynchronizationVector::isNoActionInput(inputActionName)) {
                                uint64_t actionIndex = model.getActionIndex(inputActionName);
                                synchronizingActions[position].insert(actionIndex);
                                
                                storm::jani::Automaton const& automaton = model.getAutomaton(parallelComposition.getSubcomposition(position).asAutomatonComposition().getAutomatonName());
                                bool hasParticipatingEdge = false;
                                for (auto const& edge : automaton.getEdges()) {
                                    if (edge.getActionIndex() == actionIndex) {
                                        hasParticipatingEdge = true;
                                        break;
                                    }
                                }
                                
                                if (!hasParticipatingEdge) {
                                    createVector = false;
                                }
                            }
                            ++position;
                        }
                        
                        if (createVector) {
                            cpptempl::data_map vector = generateSynchronizationVector(modelData, parallelComposition, synchronizationVector, synchronizationVectorIndex);
                            vectors.push_back(vector);
                        }
                        ++synchronizationVectorIndex;
                    }
                    
                    uint64_t position = 0;
                    for (auto const& composition : parallelComposition.getSubcompositions()) {
                        storm::jani::Automaton const& automaton = model.getAutomaton(composition->asAutomatonComposition().getAutomatonName());
                        
                        // Add all edges with an action index that is not mentioned in any synchronization vector as
                        // non-synchronizing edges.
                        uint64_t edgeIndex = 0;
                        for (auto const& edge : automaton.getEdges()) {
                            if (synchronizingActions[position].find(edge.getActionIndex()) != synchronizingActions[position].end()) {
                                synchronizingEdges.push_back(generateEdge(automaton, edgeIndex, edge));
                            } else {
                                nonSynchronizingEdges.push_back(generateEdge(automaton, edgeIndex, edge));
                            }
                            ++edgeIndex;
                        }
                        
                        ++position;
                    }
                }
                
                modelData["nonsynch_edges"] = cpptempl::make_data(nonSynchronizingEdges);
                modelData["synch_edges"] = cpptempl::make_data(synchronizingEdges);
                modelData["synch_vectors"] = cpptempl::make_data(vectors);
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateEdge(storm::jani::Automaton const& automaton, uint64_t edgeIndex, storm::jani::Edge const& edge) {
                cpptempl::data_map edgeData;
                
                std::set<storm::expressions::Variable> transientVariablesInEdge;
                cpptempl::data_list edgeAssignments;
                for (auto const& assignment : edge.getAssignments()) {
                    transientVariablesInEdge.insert(assignment.getExpressionVariable());
                    std::set<storm::expressions::Variable> usedVariables = assignment.getAssignedExpression().getVariables();
                    for (auto const& variable : usedVariables) {
                        if (transientVariables.find(variable) != transientVariables.end()) {
                            transientVariablesInEdge.insert(variable);
                        }
                    }
                    edgeAssignments.push_back(generateAssignment(assignment));
                }
                
                cpptempl::data_list destinations;
                uint64_t destinationIndex = 0;
                std::set<storm::expressions::Variable> transientVariablesInDestinations;
                for (auto const& destination : edge.getDestinations()) {
                    destinations.push_back(generateDestination(destinationIndex, destination));
                    
                    for (auto const& assignment : destination.getOrderedAssignments().getAllAssignments()) {
                        if (assignment.isTransient()) {
                            transientVariablesInDestinations.insert(assignment.getExpressionVariable());
                        }
                        std::set<storm::expressions::Variable> usedVariables = assignment.getAssignedExpression().getVariables();
                        for (auto const& variable : usedVariables) {
                            if (transientVariables.find(variable) != transientVariables.end()) {
                                transientVariablesInDestinations.insert(variable);
                            }
                        }
                    }
                    
                    ++destinationIndex;
                }
                
                edgeData["guard"] = expressionTranslator.translate(shiftVariablesWrtLowerBound(edge.getGuard()), storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName));
                edgeData["destinations"] = cpptempl::make_data(destinations);
                edgeData["name"] = automaton.getName() + "_" + std::to_string(edgeIndex);
                edgeData["transient_assignments"] = cpptempl::make_data(edgeAssignments);

                cpptempl::data_list transientVariablesInDestinationsData;
                for (auto const& variable : transientVariablesInDestinations) {
                    transientVariablesInDestinationsData.push_back(getVariableName(variable));
                }
                edgeData["transient_variables_in_destinations"] = cpptempl::make_data(transientVariablesInDestinationsData);
                cpptempl::data_list transientVariablesInEdgeData;
                for (auto const& variable : transientVariablesInEdge) {
                    transientVariablesInEdgeData.push_back(getVariableName(variable));
                }
                edgeData["transient_variables_in_edge"] = cpptempl::make_data(transientVariablesInEdgeData);
                return edgeData;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateDestination(uint64_t destinationIndex, storm::jani::EdgeDestination const& destination) {
                cpptempl::data_map destinationData;
                
                cpptempl::data_list levels = generateLevels(destination.getOrderedAssignments());
                destinationData["name"] = asString(destinationIndex);
                destinationData["levels"] = cpptempl::make_data(levels);
                destinationData["value"] = expressionTranslator.translate(shiftVariablesWrtLowerBound(destination.getProbability()), storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, "double"));
                if (destination.getOrderedAssignments().empty()) {
                    destinationData["lowestLevel"] = "0";
                    destinationData["highestLevel"] = "0";
                } else {
                    destinationData["lowestLevel"] = asString(destination.getOrderedAssignments().getLowestLevel());
                    destinationData["highestLevel"] = asString(destination.getOrderedAssignments().getHighestLevel());
                }

                return destinationData;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_list ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateLevels(storm::jani::OrderedAssignments const& orderedAssignments) {
                cpptempl::data_list levels;
                
                auto const& assignments = orderedAssignments.getAllAssignments();
                if (!assignments.empty()) {
                    int_fast64_t currentLevel = assignments.begin()->getLevel();
                    
                    cpptempl::data_list nonTransientAssignmentData;
                    cpptempl::data_list transientAssignmentData;
                    for (auto const& assignment : assignments) {
                        if (assignment.getLevel() != currentLevel) {
                            cpptempl::data_map level;
                            level["non_transient_assignments"] = cpptempl::make_data(nonTransientAssignmentData);
                            level["transient_assignments"] = cpptempl::make_data(transientAssignmentData);
                            level["index"] = asString(currentLevel);
                            levels.push_back(level);
                            
                            nonTransientAssignmentData = cpptempl::data_list();
                            transientAssignmentData = cpptempl::data_list();
                            currentLevel = assignment.getLevel();
                        }
                        
                        if (assignment.isTransient()) {
                            transientAssignmentData.push_back(generateAssignment(assignment));
                        } else {
                            nonTransientAssignmentData.push_back(generateAssignment(assignment));
                        }
                    }

                    // Close the last (open) level.
                    cpptempl::data_map level;
                    level["non_transient_assignments"] = cpptempl::make_data(nonTransientAssignmentData);
                    level["transient_assignments"] = cpptempl::make_data(transientAssignmentData);
                    level["index"] = asString(currentLevel);
                    levels.push_back(level);
                }
                
                return levels;
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
                result["variable"] = getVariableName(getLocationVariable(automaton));
                result["value"] = asString(value);
                return result;
            }
            
            template <typename ValueType, typename RewardModelType>
            cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateAssignment(storm::jani::Assignment const& assignment) {
                cpptempl::data_map result;
                result["variable"] = getVariableName(assignment.getExpressionVariable());
                auto lowerBoundIt = lowerBounds.find(assignment.getExpressionVariable());
                if (lowerBoundIt != lowerBounds.end()) {
                    if (lowerBoundIt->second < 0) {
                        result["value"] = expressionTranslator.translate(shiftVariablesWrtLowerBound(assignment.getAssignedExpression()) + model.getManager().integer(lowerBoundIt->second), storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName));
                    } else if (lowerBoundIt->second == 0) {
                        result["value"] = expressionTranslator.translate(shiftVariablesWrtLowerBound(assignment.getAssignedExpression()), storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName));
                    } else {
                        result["value"] = expressionTranslator.translate(shiftVariablesWrtLowerBound(assignment.getAssignedExpression()) - model.getManager().integer(lowerBoundIt->second), storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName));
                    }
                } else {
                    result["value"] = expressionTranslator.translate(shiftVariablesWrtLowerBound(assignment.getAssignedExpression()), storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName));
                }
                return result;
            }
            
            template <typename ValueType, typename RewardModelType>
            std::string const& ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::getVariableName(storm::expressions::Variable const& variable) const {
                return variableToName.at(variable);
            }

            template <typename ValueType, typename RewardModelType>
            std::string const& ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::registerVariable(storm::expressions::Variable const& variable, bool transient) {
                std::string variableName;
                
                // FIXME: Technically, we would need to catch all keywords here...
                if (variable.getName() == "default") {
                    variableName = "__default";
                } else {
                    variableName = variable.getName();
                }
                
                variableToName[variable] = variableName;
                if (transient) {
                    transientVariables.insert(variable);
                    variablePrefixes[variable] = "transientIn.";
                } else {
                    nontransientVariables.insert(variable);
                    variablePrefixes[variable] = "in.";
                }
                return variableToName[variable] ;
            }
            
            template <typename ValueType, typename RewardModelType>
            storm::expressions::Variable const& ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::getLocationVariable(storm::jani::Automaton const& automaton) const {
                return automatonToLocationVariable.at(automaton.getName());
            }
            
            template <typename ValueType, typename RewardModelType>
            std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::asString(bool value) const {
                std::stringstream out;
                out << std::boolalpha << value;
                return out.str();
            }
            
            template <typename ValueType, typename RewardModelType>
            storm::expressions::Expression ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::shiftVariablesWrtLowerBound(storm::expressions::Expression const& expression) {
                return expression.substitute(lowerBoundShiftSubstitution);
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
