#pragma once

#include "ProgramEdgeGroup.h"
#include "defines.h"

namespace storm {
namespace ppg {
class ProgramLocation {
   public:
    using EdgeGroupIterator = std::vector<ProgramEdgeGroup*>::iterator;
    using const_iterator = std::vector<ProgramEdgeGroup*>::const_iterator;

    ProgramLocation(ProgramGraph* graph, ProgramLocationIdentifier id, bool initial) : graph(graph), locId(id), init(initial) {
        // Intentionally left empty
    }

    virtual ~ProgramLocation() {
        for (auto const& e : edgeGroups) {
            delete e;
        }
    }

    std::vector<ProgramEdge*> addProgramEdgeToAllGroups(ProgramActionIdentifier action, storm::expressions::Expression const& condition,
                                                        ProgramLocationIdentifier targetId);
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

    bool hasNonDeterminism() const {
        for (auto const& eg : edgeGroups) {
            if (eg->nrEdges() > 1)
                return true;
        }
        return false;
    }

    bool hasUniqueSuccessor() const {
        return nrOutgoingEdgeGroups() == 1 && !hasNonDeterminism();
    }

    const_iterator begin() const {
        return edgeGroups.begin();
    }

    const_iterator end() const {
        return edgeGroups.end();
    }

    // Todo rename?
    EdgeGroupIterator getOutgoingEdgeGroupBegin() {
        return edgeGroups.begin();
    }

    // Todo rename?
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
}  // namespace ppg
}  // namespace storm
