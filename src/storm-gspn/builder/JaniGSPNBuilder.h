#pragma once

#include "storm-gspn/storage/gspn/GSPN.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace builder {
        class JaniGSPNBuilder {
        public:
            JaniGSPNBuilder(storm::gspn::GSPN const& gspn)
                    : gspn(gspn), expressionManager(gspn.getExpressionManager()) {
                
            }
            
            virtual ~JaniGSPNBuilder() {
                // Intentionally left empty.
            }
            
            
            storm::jani::Model* build(std::string const& automatonName = "gspn_automaton", bool buildStandardProperties = false);
            
            storm::jani::Variable const& getPlaceVariable(uint64_t placeId) const {
                return *vars.at(placeId);
            }
            
            std::vector<storm::jani::Property> const& getStandardProperties() const;

            
        private:
            void addVariables(storm::jani::Model* model);

            uint64_t addLocation(storm::jani::Automaton& automaton);

            void addEdges(storm::jani::Automaton& automaton, uint64_t locId);

            storm::jani::Variable const& addDeadlockTransientVariable(storm::jani::Model* model, std::string name, bool ignoreCapacities = false, bool ignoreInhibitorArcs = false, bool ignoreEmptyPlaces = false);
            void buildProperties(storm::jani::Model* model);
            
            const uint64_t janiVersion = 1;
            storm::gspn::GSPN const& gspn;
            std::map<uint64_t, storm::jani::Variable const*> vars;
            std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;
            
            std::vector<storm::jani::Property> standardProperties;
            
        };
    }
}
