#include "src/storage/ppg/ProgramGraph.h"
#include "src/storage/jani/Model.h"
#include "src/storage/jani/Location.h"
#include "src/storage/IntegerInterval.h"


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
                addProcedureLocations(mainAutomaton, pg);
                model->addAutomaton(mainAutomaton);
                return model;
            }
            
        private:
            void addProcedureVariables(storm::jani::Automaton& automaton, storm::ppg::ProgramGraph const& pg) {
                for (auto const& v : pg.getVariables()) {
                    if(variableRestrictions.count(v.getIndex())) {
                        assert(false);
                    } else {
                        storm::jani::UnboundedIntegerVariable janiVar(v.getName(), v, expManager->integer(0), false);
                        automaton.addUnboundedIntegerVariable(janiVar);
                    }
                }
            }
            
            void addProcedureLocations(storm::jani::Automaton& automaton, storm::ppg::ProgramGraph const& pg) {
                for(auto it = pg.locationBegin(); it != pg.locationEnd(); ++it) {
                    storm::jani::Location janiLoc(std::to_string(it->second.id()));
                    automaton.addLocation(janiLoc);
                }
            }
            
            std::map<uint64_t, storm::storage::IntegerInterval> variableRestrictions;
            std::shared_ptr<storm::expressions::ExpressionManager> expManager;
            
        };
    }
}