
#include "ProgramGraph.h"


namespace storm {
    namespace ppg {
        
        std::vector<ProgramVariableIdentifier> ProgramGraph::transientVariables() const {
            std::vector<ProgramVariableIdentifier> transientCandidates = variablesNotInGuards();
            bool removedCandidate = true; // tmp
            while(removedCandidate && !transientCandidates.empty()) {
                removedCandidate = false;
                for (auto const& act : this->deterministicActions) {
                    for (auto const& group : act.second) {
                        for (auto const& assignment : group) {
                            if (std::find(transientCandidates.begin(), transientCandidates.end(), assignment.first) == transientCandidates.end()) {
                                for (auto const& vars : assignment.second.getVariables()) {
                                    auto it = std::find(transientCandidates.begin(), transientCandidates.end(), vars.getIndex());
                                    if (it != transientCandidates.end()) {
                                        transientCandidates.erase(it);
                                        removedCandidate = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return transientCandidates;
        }
        
        std::vector<ProgramVariableIdentifier> ProgramGraph::variablesNotInGuards() const {
            std::vector<ProgramVariableIdentifier> result;
            std::set<storm::expressions::Variable> contained;
            for (auto const& loc : locations) {
                for (auto const& edgegroup : loc.second) {
                    for (auto const& edge : *edgegroup) {
                        auto conditionVars = edge->getCondition().getVariables();
                        std::cout << "Guard " << edge->getCondition() << " contains ";
                        for (auto const& cv : conditionVars) {
                            std::cout << cv.getName() << ", ";
                        }
                        std::cout << std::endl;
                        contained.insert(conditionVars.begin(), conditionVars.end());
                    }
                }
            }
            for (auto const& varEntry : variables) {
                if (contained.count(varEntry.second) == 0) {
                    result.push_back(varEntry.first);
                }
            }
            
            return result;
        }
        
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