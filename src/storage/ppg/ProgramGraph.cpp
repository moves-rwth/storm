
#include "ProgramGraph.h"


namespace storm {
    namespace ppg {
        void ProgramGraph::printDot(std::ostream& os) const {
            os << "digraph ppg {" << std::endl;
            
            for (auto const& loc : locations) {
                os << "\tl" << loc.first << "[label=" << loc.first << "];"<< std::endl;
            }
            os << std::endl;
            for (auto const& loc : locations) {
                if (loc.second.nrOutgoingEdgeGroups() > 1) {
                    for (auto const& edgegroup : loc.second) {
                        os << "\teg" << edgegroup->getId() << "[shape=circle];"<< std::endl;
                    }
                }
            }
            os << std::endl;
            for (auto const& loc : locations) {
                for (auto const& edgegroup : loc.second) {
                    
                    if (loc.second.nrOutgoingEdgeGroups() > 1) {
                        os << "\tl" << loc.first << " -> eg" << edgegroup->getId() << ";" << std::endl;
                        for (auto const& edge : *edgegroup) {
                            os << "\teg" << edgegroup->getId() << " -> l" << edge->getTargetId();
                            if (!edge->hasNoAction()) {
                                os << " [label=\"" << edge->getActionId()  << "\"]";
                            }
                            os << ";" << std::endl;
                        }
                    } else {
                        for (auto const& edge : *edgegroup) {
                            os << "\tl" << loc.first << " -> l" << edge->getTargetId();
                            if (!edge->hasNoAction()) {
                                os << " [label=\"" << edge->getActionId()  << "\"]";
                            }
                            os << ";" << std::endl;
                        }
                    }
                }
            }
            os << "}" << std::endl;
        }
    }
}