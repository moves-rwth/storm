#pragma once

#include "defines.h"
#include "ProgramEdgeGroup.h"

namespace storm {
    namespace ppg {
        class ProgramLocation {
        public:
            using EdgeGroupIterator = std::vector<ProgramEdgeGroup*>::iterator;
            
            ProgramLocation(ProgramGraph* graph, ProgramLocationIdentifier id, bool initial) : graph(graph), locId(id), init(initial){
                // Intentionally left empty
            }
            
            virtual ~ProgramLocation() {
                for( auto const& e : edgeGroups) {
                    delete e;
                }
            }
            
            
            std::vector<ProgramEdge*> addProgramEdgeToAllGroups(ProgramActionIdentifier action, storm::expressions::Expression const& condition, ProgramLocationIdentifier targetId);
            std::vector<ProgramEdge*> addProgramEdgeToAllGroups(ProgramActionIdentifier action, ProgramLocationIdentifier targetId);
            ProgramEdgeGroup* addProgramEdgeGroup(storm::expressions::Expression const& probability);
            
            
            ProgramEdgeGroup* emplaceEdgeGroup(ProgramEdgeGroupIdentifier id, storm::expressions::Expression const& probability) {
                edgeGroups.emplace_back(new ProgramEdgeGroup(graph, id, locId, probability));
                return edgeGroups.back();
            }
            
            bool isInitial() const {
                return init;
            }
            
            size_t nrOutgoingEdgeGroups() const {
                return edgeGroups.size();
            }
            
            EdgeGroupIterator getOutgoingEdgeGroupBegin() {
                return edgeGroups.begin();
            }
            
            EdgeGroupIterator getOutgoingEdgeGroupEnd() {
                return edgeGroups.end();
            }
            
            ProgramLocationIdentifier id() const {
                return locId;
            }
            
        private:
            ProgramGraph* graph;
            ProgramLocationIdentifier locId;
            bool init;
            std::vector<ProgramEdgeGroup*> edgeGroups;
            
            
        };
    }
}
