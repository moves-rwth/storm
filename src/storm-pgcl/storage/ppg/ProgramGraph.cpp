
#include "ProgramGraph.h"

namespace storm {
namespace ppg {

std::vector<ProgramVariableIdentifier> ProgramGraph::noeffectVariables() const {
    std::vector<ProgramVariableIdentifier> transientCandidates = variablesNotInGuards();
    bool removedCandidate = true;  // tmp
    while (removedCandidate && !transientCandidates.empty()) {
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

std::pair<bool, bool> ProgramGraph::checkIfRewardVariableHelper(
    storm::expressions::Variable const& var, std::unordered_map<ProgramActionIdentifier, DeterministicProgramAction> const& detActions) const {
    bool pos = true;
    bool first = true;
    for (auto const& act : detActions) {
        for (auto const& group : act.second) {
            for (auto const& assignment : group) {
                if (assignment.first == var.getIndex()) {
                    // storm::expressions::Expression expr = (assignment.second - var.getExpression()).simplify();
                    if (!assignment.second.isLinear()) {
                        return {false, true};
                    }
                    auto const& vars = assignment.second.getVariables();
                    if (vars.empty() || vars.size() > 1) {
                        return {false, true};
                    }
                    if (vars.begin()->getIndex() == assignment.first) {
                        std::map<storm::expressions::Variable, storm::expressions::Expression> eta0;
                        std::map<storm::expressions::Variable, storm::expressions::Expression> eta1;
                        eta0.emplace(var, expManager->integer(0));
                        eta1.emplace(var, expManager->integer(1));
                        if (assignment.second.substitute(eta1).evaluateAsInt() - assignment.second.substitute(eta0).evaluateAsInt() != 1) {
                            return {false, true};
                        }
                        if (assignment.second.substitute(eta0).evaluateAsInt() < 0) {
                            if (!first && pos) {
                                return {false, true};
                            }
                            pos = false;
                        }
                        first = false;
                    }
                }
            }
        }
    }
    return {true, pos};
}

std::vector<ProgramVariableIdentifier> ProgramGraph::constantAssigned() const {
    std::set<ProgramVariableIdentifier> contained;
    for (auto const& act : deterministicActions) {
        for (auto const& group : act.second) {
            for (auto const& assignment : group) {
                if (assignment.second.containsVariables()) {
                    contained.insert(assignment.first);
                }
            }
        }
    }
    std::vector<ProgramVariableIdentifier> result;
    for (auto const& varEntry : variables) {
        if (contained.count(varEntry.second.getIndex()) == 0) {
            // Currently we can only do this for integer assigned initials
            if (initialValues.at(varEntry.second.getIndex()).containsVariables() || !varEntry.second.hasIntegerType()) {
                continue;
            }
            result.push_back(varEntry.first);
        }
    }
    return result;
}

std::vector<ProgramVariableIdentifier> ProgramGraph::constants() const {
    std::set<ProgramVariableIdentifier> contained;
    for (auto const& act : deterministicActions) {
        for (auto const& group : act.second) {
            for (auto const& assignment : group) {
                contained.insert(assignment.first);
            }
        }
    }
    for (auto const& act : probabilisticActions) {
        contained.insert(act.second.getVariableIdentifier());
    }
    std::vector<ProgramVariableIdentifier> result;
    for (auto const& varEntry : variables) {
        if (contained.count(varEntry.second.getIndex()) == 0) {
            result.push_back(varEntry.first);
        }
    }
    return result;
}

storm::storage::IntegerInterval ProgramGraph::supportForConstAssignedVariable(ProgramVariableIdentifier i) const {
    storm::storage::IntegerInterval support(initialValues.at(i).evaluateAsInt());
    for (auto const& act : deterministicActions) {
        for (auto const& group : act.second) {
            for (auto const& assignment : group) {
                if (assignment.first == i) {
                    support.extend(assignment.second.evaluateAsInt());
                }
            }
        }
    }
    for (auto const& act : probabilisticActions) {
        if (act.second.getVariableIdentifier() == i) {
            support.extend(act.second.getSupportInterval());
        }
    }
    return support;
}

std::vector<ProgramVariableIdentifier> ProgramGraph::rewardVariables() const {
    std::vector<ProgramVariableIdentifier> rewardCandidates = noeffectVariables();
    std::vector<ProgramVariableIdentifier> result;
    for (auto const& var : rewardCandidates) {
        if (variables.at(var).hasIntegerType()) {
            // TODO add some checks here; refine the evaluate as int stuff.
            if (initialValues.at(var).evaluateAsInt() == 0 && checkIfRewardVariableHelper(variables.at(var), this->deterministicActions).first) {
                result.push_back(var);
            }
        }
    }
    return result;
}

std::vector<ProgramVariableIdentifier> ProgramGraph::variablesNotInGuards() const {
    std::vector<ProgramVariableIdentifier> result;
    std::set<storm::expressions::Variable> contained;
    for (auto const& loc : locations) {
        for (auto const& edgegroup : loc.second) {
            for (auto const& edge : *edgegroup) {
                auto conditionVars = edge->getCondition().getVariables();
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

//        void ProgramGraph::mergeActions() {
//            auto initialLocs = initialLocations();
//            assert(initialLocs.size() == 1);
//            // For now, we only follow the unique path.
//            ProgramLocationIdentifier currentLocId = initialLocs.front();
//            while(locations.at(currentLocId).hasUniqueSuccessor()) {
//                auto& edgeGroup = **locations.at(currentLocId()).begin();
//                auto& edge = **edgeGroup.begin();
//
//            }
//        }

void ProgramGraph::printDot(std::ostream& os) const {
    os << "digraph ppg {\n";

    for (auto const& loc : locations) {
        os << "\tl" << loc.first << "[label=" << loc.first << "];\n";
    }
    os << '\n';
    for (auto const& loc : locations) {
        if (loc.second.nrOutgoingEdgeGroups() > 1) {
            for (auto const& edgegroup : loc.second) {
                os << "\teg" << edgegroup->getId() << "[shape=rectangle];\n";
            }
        }
    }
    os << '\n';
    for (auto const& loc : locations) {
        for (auto const& edgegroup : loc.second) {
            if (loc.second.nrOutgoingEdgeGroups() > 1) {
                os << "\tl" << loc.first << " -> eg" << edgegroup->getId() << ";\n";
                for (auto const& edge : *edgegroup) {
                    os << "\teg" << edgegroup->getId() << " -> l" << edge->getTargetId();
                    if (!edge->hasNoAction()) {
                        os << " [label=\"" << edge->getActionId() << "\"]";
                    }
                    os << ";\n";
                }
            } else {
                for (auto const& edge : *edgegroup) {
                    os << "\tl" << loc.first << " -> l" << edge->getTargetId();
                    if (!edge->hasNoAction()) {
                        os << " [label=\"" << edge->getActionId() << "\"]";
                    }
                    os << ";\n";
                }
            }
        }
    }
    os << "}\n";
}
}  // namespace ppg
}  // namespace storm
