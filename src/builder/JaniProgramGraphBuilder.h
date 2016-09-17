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
            
            JaniProgramGraphBuilder() {
                
            }
            
            //void addVariableRestriction(storm::expressions::Variable const& var, storm::IntegerInterval const& interval ) {
                
            //}
            
        
            
            storm::jani::Model* build(storm::ppg::ProgramGraph const& pg, std::string const& name = "program_graph") {
                expManager = pg.getExpressionManager();
                storm::jani::Model* model = new storm::jani::Model(name, storm::jani::ModelType::MDP, janiVersion, expManager);
                storm::jani::Automaton mainAutomaton("main");
                addProcedureVariables(mainAutomaton, pg);
                auto janiLocs = addProcedureLocations(mainAutomaton, pg);
                addEdges(mainAutomaton, pg, janiLocs);
                model->addAutomaton(mainAutomaton);
                model->setStandardSystemComposition();
                return model;
            }
            
        private:
            std::string janiLocationName(storm::ppg::ProgramLocationIdentifier i) {
                return "l" + std::to_string(i);
            }
            
            
            
            void addEdges(storm::jani::Automaton& automaton, storm::ppg::ProgramGraph const& pg, std::map<storm::ppg::ProgramLocationIdentifier, uint64_t>  const& janiLocId);
            std::vector<storm::jani::EdgeDestination> buildDestinations(storm::jani::Automaton& automaton, storm::ppg::ProgramEdge const& edge );
                
            
            void addProcedureVariables(storm::jani::Automaton& automaton, storm::ppg::ProgramGraph const& pg) {
                for (auto const& v : pg.getVariables()) {
                    if(variableRestrictions.count(v.getIndex())) {
                        assert(false);
                    } else {
                        storm::jani::UnboundedIntegerVariable janiVar(v.getName(), v, expManager->integer(0), false);
                        automaton.addVariable(janiVar);
                    }
                }
            }
            
            std::map<storm::ppg::ProgramLocationIdentifier, uint64_t> addProcedureLocations(storm::jani::Automaton& automaton, storm::ppg::ProgramGraph const& pg) {
                std::map<storm::ppg::ProgramLocationIdentifier, uint64_t> result;
                for(auto it = pg.locationBegin(); it != pg.locationEnd(); ++it) {
                    storm::jani::Location janiLoc(janiLocationName(it->second.id()));
                    result[it->second.id()] = automaton.addLocation(janiLoc);
                    if(it->second.isInitial()) {
                        automaton.addInitialLocation(result[it->second.id()]);
                    }
                }
                return result;
            }
            
            /// Restrictions on variables
            std::map<uint64_t, storm::storage::IntegerInterval> variableRestrictions;
            /// The expression manager
            std::shared_ptr<storm::expressions::ExpressionManager> expManager;
            
            
            
        };
    }
}