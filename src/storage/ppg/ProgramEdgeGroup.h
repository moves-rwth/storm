#pragma once

#include "defines.h"
#include "ProgramEdge.h"

namespace storm {
    namespace ppg {
        
        class ProgramEdgeGroup {
        public:
            ProgramEdgeGroup(ProgramGraph* graph, ProgramEdgeGroupIdentifier id, ProgramLocationIdentifier sourceId, storm::expressions::Expression const& probability)
            : graph(graph), groupId(id), sourceId(sourceId), probability(probability)
            {
                // Intentionally left empty.
            }
            
            virtual ~ProgramEdgeGroup() {
                for( auto const& e : edges) {
                    delete e;
                }
            }
            
            ProgramEdge* addEdge(ProgramActionIdentifier action, ProgramLocationIdentifier target);
            ProgramEdge* addEdge(ProgramActionIdentifier action, storm::expressions::Expression const& condition, ProgramLocationIdentifier target);
            
            /**
             * Constructs an outgoing edge in this edge group.
             */
            ProgramEdge* emplaceEdge(ProgramEdgeIdentifier id, ProgramActionIdentifier action, storm::expressions::Expression const& condition, ProgramLocationIdentifier target) {
                edges.emplace_back(new ProgramEdge(this, id, action, condition, target));
                return edges.back();
            }
            
        private:
            /// Pointer to the graph; not owned.
            ProgramGraph* graph;
            /// Own id (persistent over copy)
            ProgramEdgeGroupIdentifier groupId;
            /// Id of source location
            ProgramLocationIdentifier sourceId;
            /// Probability for this group
            storm::expressions::Expression probability;
            /// Outgoing edges
            std::vector<ProgramEdge*> edges;
            
        };
    }
}
