#include <string>
#include <unordered_map>

#include "storm/storage/ppg/ProgramGraph.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Location.h"
#include "storm/storage/jani/EdgeDestination.h"
#include "storm/storage/IntegerInterval.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm/utility/macros.h"

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
                rewards = programGraph.rewardVariables();
                constants = programGraph.constants();
                auto boundedVars = programGraph.constantAssigned();
                for(auto const& v : boundedVars) {
                    variableRestrictions.emplace(v, programGraph.supportForConstAssignedVariable(v));
                }
            }
            
            virtual ~JaniProgramGraphBuilder() {
                for (auto& var : variables ) {
                    delete var.second;
                }
            }
            
            //void addVariableRestriction(storm::expressions::Variable const& var, storm::IntegerInterval const& interval ) {
                
            //}
            
        
            void restrictAllVariables(int64_t from, int64_t to) {
                restrictAllVariables(storm::storage::IntegerInterval(from, to));
            }
            
            void restrictAllVariables(storm::storage::IntegerInterval const& restr) {
                for (auto const& v : programGraph.getVariables()) {
                    if(isConstant(v.first)) {
                        continue;
                    }
                    if(variableRestrictions.count(v.first) > 0) {
                        continue; // Currently we ignore user bounds if we have bounded integers;
                    }
                    if(v.second.hasIntegerType() ) {
                        userVariableRestrictions.emplace(v.first, restr);
                    }
                }
            }
            
            storm::jani::Model* build(std::string const& name = "program_graph") {
                expManager = programGraph.getExpressionManager();
                storm::jani::Model* model = new storm::jani::Model(name, storm::jani::ModelType::MDP, janiVersion, expManager);
                storm::jani::Automaton mainAutomaton("main");
                addProcedureVariables(*model, mainAutomaton);
                janiLocId = addProcedureLocations(*model, mainAutomaton);
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
            
            storm::jani::OrderedAssignments buildOrderedAssignments(storm::jani::Automaton& automaton, storm::ppg::DeterministicProgramAction const& act) ;
            void addEdges(storm::jani::Automaton& automaton);
            std::vector<storm::jani::EdgeDestination> buildDestinations(storm::jani::Automaton& automaton, storm::ppg::ProgramEdge const& edge );
            /**
             * Helper for probabilistic assignments
             */
            std::vector<storm::jani::EdgeDestination> buildProbabilisticDestinations(storm::jani::Automaton& automaton, storm::ppg::ProgramEdge const& edge );
            
            std::pair<std::vector<storm::jani::Edge>, storm::expressions::Expression> addVariableChecks(storm::ppg::ProgramEdge const& edge);
            
            bool isUserRestrictedVariable(storm::ppg::ProgramVariableIdentifier i) const {
                return userVariableRestrictions.count(i) == 1 && !isRewardVariable(i);
            }
            
            bool isRestrictedVariable(storm::ppg::ProgramVariableIdentifier i) const {
                // Might be different from user restricted in near future.
                return (variableRestrictions.count(i) == 1 && !isRewardVariable(i)) || isUserRestrictedVariable(i);
            }
            
            storm::storage::IntegerInterval const& variableBounds(storm::ppg::ProgramVariableIdentifier i) const {
                assert(isRestrictedVariable(i));
                if (userVariableRestrictions.count(i) == 1) {
                    return userVariableRestrictions.at(i);
                } else {
                    return variableRestrictions.at(i);
                }
                
            }
            
            bool isRewardVariable(storm::ppg::ProgramVariableIdentifier i) const {
                return std::find(rewards.begin(), rewards.end(), i) != rewards.end();
            }
            
            bool isConstant(storm::ppg::ProgramVariableIdentifier i) const {
                return std::find(constants.begin(), constants.end(), i) != constants.end();
            }
            
            void addProcedureVariables(storm::jani::Model& model, storm::jani::Automaton& automaton);
            
            std::map<storm::ppg::ProgramLocationIdentifier, uint64_t> addProcedureLocations(storm::jani::Model& model, storm::jani::Automaton& automaton) {
                std::map<storm::ppg::ProgramLocationIdentifier, uint64_t> result;
                std::map<std::string, storm::jani::BooleanVariable const*> labelVars;
                std::set<std::string> labels = programGraph.getLabels();
                for(auto const& label : labels) {
                    storm::jani::BooleanVariable janiVar(label, expManager->declareBooleanVariable(label), expManager->boolean(false), true);
                    labelVars.emplace(label, &model.addVariable(janiVar));
                }
                
                for (auto it = programGraph.locationBegin(); it != programGraph.locationEnd(); ++it) {
                    storm::jani::Location janiLoc(janiLocationName(it->second.id()));
                    for(auto const& label : programGraph.getLabels(it->second.id())) {
                        assert(labelVars.count(label) > 0);
                        janiLoc.addTransientAssignment(storm::jani::Assignment(*(labelVars.at(label)), expManager->boolean(true)));
                    }
                    result[it->second.id()] = automaton.addLocation(janiLoc);
                    if (it->second.isInitial()) {
                        automaton.addInitialLocation(result[it->second.id()]);
                    }
                }
                return result;
            }
            
            void addVariableOoBLocations(storm::jani::Automaton& automaton) {
                for(auto const& restr : userVariableRestrictions) {
                    if(!isRewardVariable(restr.first)) {
                        storm::jani::Location janiLoc(janiVariableOutOfBoundsLocationName(restr.first));
                        uint64_t locId = automaton.addLocation(janiLoc);
                        varOutOfBoundsLocations[restr.first] = locId;
                    }
                    
                }
            }
            /// Transient variables
            std::vector<storm::ppg::ProgramVariableIdentifier> rewards;
            /// Variables that are constants
            std::vector<storm::ppg::ProgramVariableIdentifier> constants;
            /// Restrictions on variables (automatic)
            std::map<uint64_t, storm::storage::IntegerInterval> variableRestrictions;
            /// Restrictions on variables (provided by users)
            std::map<uint64_t, storm::storage::IntegerInterval> userVariableRestrictions;
            
            /// Locations for variables that would have gone ot o
            std::map<uint64_t, uint64_t> varOutOfBoundsLocations;
            std::map<storm::ppg::ProgramLocationIdentifier, uint64_t> janiLocId;
            std::map<storm::ppg::ProgramVariableIdentifier, storm::jani::Variable*> variables;
            
            /// The expression manager
            std::shared_ptr<storm::expressions::ExpressionManager> expManager;
            /// The program graph to be translated
            storm::ppg::ProgramGraph const& programGraph;
            
            
        };
    }
}
