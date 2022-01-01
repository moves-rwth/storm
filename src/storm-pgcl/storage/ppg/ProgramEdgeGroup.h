#pragma once

#include "ProgramEdge.h"
#include "defines.h"

namespace storm {
namespace ppg {

class ProgramEdgeGroup {
   public:
    using iterator = std::vector<ProgramEdge*>::iterator;
    using const_iterator = std::vector<ProgramEdge*>::const_iterator;

    ProgramEdgeGroup(ProgramGraph* graph, ProgramEdgeGroupIdentifier id, ProgramLocationIdentifier sourceId, storm::expressions::Expression const& probability)
        : graph(graph), groupId(id), sourceId(sourceId), probability(probability) {
        // Intentionally left empty.
    }

    virtual ~ProgramEdgeGroup() {
        for (auto const& e : edges) {
            delete e;
        }
    }

    ProgramEdge* addEdge(ProgramActionIdentifier action, ProgramLocationIdentifier target);
    ProgramEdge* addEdge(ProgramActionIdentifier action, storm::expressions::Expression const& condition, ProgramLocationIdentifier target);

    /**
     * Constructs an outgoing edge in this edge group.
     */
    ProgramEdge* emplaceEdge(ProgramEdgeIdentifier id, ProgramActionIdentifier action, storm::expressions::Expression const& condition,
                             ProgramLocationIdentifier target) {
        edges.emplace_back(new ProgramEdge(this, id, action, condition, target));
        return edges.back();
    }

    iterator begin() {
        return edges.begin();
    }

    iterator end() {
        return edges.end();
    }

    const_iterator begin() const {
        return edges.begin();
    }

    const_iterator end() const {
        return edges.end();
    }

    size_t nrEdges() const {
        return edges.size();
    }

    storm::expressions::Expression const& getProbability() const {
        return probability;
    }

    ProgramGraph const& getGraph() const {
        return *graph;
    }

    ProgramLocationIdentifier getSourceId() const {
        return sourceId;
    }

    ProgramEdgeGroupIdentifier getId() const {
        return groupId;
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
}  // namespace ppg
}  // namespace storm
