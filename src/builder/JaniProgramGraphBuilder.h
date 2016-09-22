#include <string>

#include "src/storage/ppg/ProgramGraph.h"
#include "src/storage/jani/Model.h"
#include "src/storage/jani/Location.h"
#include "src/storage/jani/EdgeDestination.h"
#include "src/storage/IntegerInterval.h"
#include "src/exceptions/NotSupportedException.h"

#include "src/utility/macros.h"

namespace storm {
    namespace builder {
        
        enum class JaniProgramGraphVariableDomainMethod  {
            Unrestricted, IntervalPropagation
        };
        
        struct JaniProgramGraphBuilderSetting {
            JaniProgramGraphVariableDomainMethod variableDomainMethod = JaniProgramGraphVariableDomainMethod::Unrestricted;
        };
        
        class JaniProgramGraphBuilder {
        public:
            static unsigned janiVersion;
            
            JaniProgramGraphBuilder(storm::ppg::ProgramGraph const& pg) : programGraph(pg) {
                transients = programGraph.transientVariables();
            }
            
            //void addVariableRestriction(storm::expressions::Variable const& var, storm::IntegerInterval const& interval ) {
                
            //}
            
        
            void restrictAllVariables(int64_t from, int64_t to) {
                for (auto const& v : programGraph.getVariables()) {
                    variableRestrictions.emplace(v.first, storm::storage::IntegerInterval(from, to));
                }
            }
            
            storm::jani::Model* build(std::string const& name = "program_graph") {
                expManager = programGraph.getExpressionManager();
                storm::jani::Model* model = new storm::jani::Model(name, storm::jani::ModelType::MDP, janiVersion, expManager);
                storm::jani::Automaton mainAutomaton("main");
                addProcedureVariables(mainAutomaton);
                janiLocId = addProcedureLocations(mainAutomaton);
                addVariableOoBLocations(mainAutomaton);
                addEdges(mainAutomaton);
                model->addAutomaton(mainAutomaton);
                model->setStandardSystemComposition();
                return model;
            }
            
        private:
            std::string janiLocationName(storm::ppg::ProgramLocationIdentifier i) {
                return "l" + std::to_string(i);
            }
            
            std::string janiVariableOutOfBoundsLocationName(storm::ppg::ProgramVariableIdentifier i) {
                return "oob-" + programGraph.getVariableName(i);
            }
            
            
            void addEdges(storm::jani::Automaton& automaton);
            std::vector<storm::jani::EdgeDestination> buildDestinations(storm::jani::Automaton& automaton, storm::ppg::ProgramEdge const& edge );
            /**
             * Helper for probabilistic assignments
             */
            std::vector<storm::jani::EdgeDestination> buildProbabilisticDestinations(storm::jani::Automaton& automaton, storm::ppg::ProgramEdge const& edge );
            
            std::pair<std::vector<storm::jani::Edge>, storm::expressions::Expression> addVariableChecks(storm::ppg::ProgramEdge const& edge);
            
            bool isUserRestrictedVariable(storm::ppg::ProgramVariableIdentifier i) {
                return variableRestrictions.count(i) == 1;
            }
            
            bool isRestrictedVariable(storm::ppg::ProgramVariableIdentifier i) {
                // Might be different from user restricted in near future.
                return variableRestrictions.count(i) == 1;
            }
            
            bool isTransientVariable(storm::ppg::ProgramVariableIdentifier i) {
                return std::find(transients.begin(), transients.end(), i) != transients.end();
            }
            
            void addProcedureVariables(storm::jani::Automaton& automaton) {
                for (auto const& v : programGraph.getVariables()) {
                    if(isRestrictedVariable(v.first) && !isTransientVariable(v.first)) {
                        storm::storage::IntegerInterval const& bounds = variableRestrictions.at(v.first);
                        if (bounds.hasLeftBound()) {
                            if (bounds.hasRightBound()) {
                                storm::jani::BoundedIntegerVariable janiVar(v.second.getName(), v.second, expManager->integer(0), false, expManager->integer(bounds.getLeftBound().get()), expManager->integer(bounds.getRightBound().get()));
                                automaton.addVariable(janiVar);
                            } else {
                                // Not yet supported.
                                assert(false);
                            }
                        } else {
                            // Not yet supported.
                            assert(false);
                        }
                    } else {
                        storm::jani::UnboundedIntegerVariable janiVar(v.second.getName(), v.second, expManager->integer(0), isTransientVariable(v.first));
                        automaton.addVariable(janiVar);
                    }
                }
            }
            
            std::map<storm::ppg::ProgramLocationIdentifier, uint64_t> addProcedureLocations(storm::jani::Automaton& automaton) {
                std::map<storm::ppg::ProgramLocationIdentifier, uint64_t> result;
                for (auto it = programGraph.locationBegin(); it != programGraph.locationEnd(); ++it) {
                    storm::jani::Location janiLoc(janiLocationName(it->second.id()));
                    result[it->second.id()] = automaton.addLocation(janiLoc);
                    if (it->second.isInitial()) {
                        automaton.addInitialLocation(result[it->second.id()]);
                    }
                }
                return result;
            }
            
            void addVariableOoBLocations(storm::jani::Automaton& automaton) {
                for(auto const& restr : variableRestrictions) {
                    storm::jani::Location janiLoc(janiVariableOutOfBoundsLocationName(restr.first));
                    uint64_t locId = automaton.addLocation(janiLoc);
                    varOutOfBoundsLocations[restr.first] = locId;
                    
                }
            }
            /// Transient variables
            std::vector<storm::ppg::ProgramVariableIdentifier> transients;
            /// Restrictions on variables
            std::map<uint64_t, storm::storage::IntegerInterval> variableRestrictions;
            /// Locations for variables that would have gone ot o
            std::map<uint64_t, uint64_t> varOutOfBoundsLocations;
            std::map<storm::ppg::ProgramLocationIdentifier, uint64_t> janiLocId;
            /// The expression manager
            std::shared_ptr<storm::expressions::ExpressionManager> expManager;
            /// The program graph to be translated
            storm::ppg::ProgramGraph const& programGraph;
            
            
        };
    }
}