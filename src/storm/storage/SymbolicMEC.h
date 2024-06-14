#ifndef STORM_STORAGE_SYMBOLICMEC_H
#define STORM_STORAGE_SYMBOLICMEC_H

#include <stack>
#include <unordered_map>
#include "storm/storage/dd/DdType.h"
#include "storm/storage/dd/Bdd.h"
#include "storm/storage/SymbolicSCCDecomposition.h"

namespace symbolicMEC {

template<storm::dd::DdType Type>
struct StateActionPair {
    storm::dd::Bdd<Type> states;
    storm::dd::Bdd<Type> actions; // [rmnt]: Actions is really state-action pairs.

    StateActionPair<Type>& operator|=(StateActionPair<Type> const & other)  {
        states  |= other.states;
        actions |= other.actions;
        return *this;
    }
};

template<storm::dd::DdType Type>
static bool isTrivialSccWithoutSelfEdge(StateActionPair<Type> const & scc,
                                        storm::dd::Bdd<Type> const & transitionsWithActions,
                                        std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const & metaVariablesRowColumnPairs) {
    bool isTrivialScc = (1 == scc.states.getNonZeroCount());
    // [rmnt] TODO : Review the noSelfEdge condition. Do you need && scc.states also?
    bool noSelfEdge = (transitionsWithActions && scc.actions && scc.states.swapVariables(metaVariablesRowColumnPairs)).isZero();
    return (isTrivialScc && noSelfEdge);
}


/* Given a set of vertices T, the random attractor Attr_R(T)
 * is a set of vertices consisting of
 * (1) T,
 * (2) random vertices with an edge to some vertex in Attr_R(T),
 * (3) player-1 vertices with all outgoing edges in Attr_R(T).
 */
template<storm::dd::DdType Type>
static StateActionPair<Type> computeRandomAttractor(storm::dd::Bdd<Type> const & actionsToApplyOn,
                                                    storm::dd::Bdd<Type> const & allStates,
                                                    storm::dd::Bdd<Type> const & transitionsWithActions,
                                                    std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                                    std::set<storm::expressions::Variable> const & metaVariablesActions,
                                                    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const & metaVariablesRowColumnPairs) {
    StateActionPair<Type> nextSet = { .states = allStates.getDdManager().getBddZero(), .actions = actionsToApplyOn };
    StateActionPair<Type> currentSet;
    do {
        currentSet = nextSet;

        // Vertices
        storm::dd::Bdd<Type> actionsCannotIntoCurrent = ( transitionsWithActions && ( ! currentSet.actions)
        ).existsAbstract(metaVariablesColumn); // [rmnt]: All (s,a) pairs which aren't included in currentSet
        storm::dd::Bdd<Type> newVertices = currentSet.actions.existsAbstract(metaVariablesActions) 
        && ( ! (actionsCannotIntoCurrent.existsAbstract(metaVariablesActions)));
        // [rmnt]: All states s such that some (s,a1) in currentSet and all (s,a) pairs in currentSet
        nextSet.states = (currentSet.states || newVertices) && allStates;

        // Actions [rmnt] Actions is really state-action pairs.
        storm::dd::Bdd<Type> currentVerticesAsColumn = nextSet.states.swapVariables(metaVariablesRowColumnPairs);
        storm::dd::Bdd<Type> actionsCanIntoCurrentVertices = (transitionsWithActions && currentVerticesAsColumn).existsAbstract(metaVariablesColumn);
        nextSet.actions = currentSet.actions || actionsCanIntoCurrentVertices;
    } while (currentSet.states != nextSet.states);
    return currentSet;
}

// For a set of states S,
// return all actions which have a non-zero-probability of leaving S.
template<storm::dd::DdType Type>
static storm::dd::Bdd<Type> ROut(storm::dd::Bdd<Type> const & sccStates,
                                 storm::dd::Bdd<Type> const & transitionsWithActions,
                                 std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                 std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const & metaVariablesRowColumnPairs) {
    storm::dd::Bdd<Type> transitionsFromSccToOutside = sccStates && transitionsWithActions && ( ! sccStates.swapVariables(metaVariablesRowColumnPairs));
    storm::dd::Bdd<Type> actionsLeavingScc = transitionsFromSccToOutside.existsAbstract(metaVariablesColumn);
    return actionsLeavingScc;
}


// As described in
// "Symbolic algorithms for graphs and Markov decision processes with fairness objectives"
template<storm::dd::DdType Type, typename ValueType>
std::vector<storm::dd::Bdd<Type>> symbolicMECDecompositionNaive(storm::dd::Bdd<Type> const & allStates,
                                                           storm::dd::Bdd<Type> const & transitionsWithActions,
                                                           std::set<storm::expressions::Variable> const & metaVariablesRow,
                                                           std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                                           std::set<storm::expressions::Variable> const & metaVariablesActions,
                                                           std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const & metaVariablesRowColumnPairs) {
    storm::dd::Bdd<Type> workingCopyTransitionsWithActions(transitionsWithActions);
    std::vector<storm::dd::Bdd<Type>> result{};
    std::stack<storm::dd::Bdd<Type>> mecCandidates{};
    for (const auto& scc : symbolicScc::decomposition<Type, ValueType>(allStates, workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions),
                                                                       metaVariablesRow, metaVariablesColumn)) {
        mecCandidates.push(scc);
    }
    while ( ! mecCandidates.empty()) {
        storm::dd::Bdd<Type> scc = mecCandidates.top();
        mecCandidates.pop();

        if (isTrivialSccWithoutSelfEdge(scc, workingCopyTransitionsWithActions, metaVariablesRowColumnPairs)) {
            continue;
        }

        storm::dd::Bdd<Type> sccROut = ROut(scc, workingCopyTransitionsWithActions, metaVariablesColumn, metaVariablesRowColumnPairs);
        if (sccROut.isZero()) {
            result.template emplace_back(scc);
        } else {
            StateActionPair<Type> attractor = computeRandomAttractor(sccROut, scc, workingCopyTransitionsWithActions, metaVariablesColumn, metaVariablesActions, metaVariablesRowColumnPairs);
            workingCopyTransitionsWithActions &= !attractor.actions;
            for (auto const& subScc : symbolicScc::decomposition<Type, ValueType>(
                     scc && !attractor.states, workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions), metaVariablesRow, metaVariablesColumn)) {
                mecCandidates.push(subScc);
            }
        }
    }
    return result;
}

// As described in
// "Symbolic Algorithms for Graphs and Markov Decision Processes with Fairness Objectives"
// Slightly simplified we only care about the bottom scc
template<storm::dd::DdType Type, typename ValueType>
storm::dd::Bdd<Type> LockStepSearch(storm::dd::Bdd<Type> const & states, // S
                                    storm::dd::Bdd<Type> const & statesWithRemovedEdges, // T_S
                                    storm::dd::Bdd<Type> const & transitionsWithoutActions,
                                    std::set<storm::expressions::Variable> const & metaVariablesRow,
                                    std::set<storm::expressions::Variable> const & metaVariablesColumn) {
    storm::dd::Bdd<Type> ts = storm::dd::Bdd<Type>(statesWithRemovedEdges);
    std::unordered_map<storm::dd::Bdd<Type>, storm::dd::Bdd<Type>> c{};
    while ( ! ts.isZero()) {
        storm::dd::Bdd<Type> v = pick<Type, ValueType>(ts);
        ts &= ! v;
        c[v] = storm::dd::Bdd<Type>(v);
    }

    ts = storm::dd::Bdd<Type>(statesWithRemovedEdges);
    while (true) {
        for (auto it = c.cbegin(); it != c.cend(); ) {
            storm::dd::Bdd<Type> t = storm::dd::Bdd<Type>(it->first);
            storm::dd::Bdd<Type> ct = storm::dd::Bdd<Type>(it->second);
            storm::dd::Bdd<Type> ctNew = ct || post(ct, states, transitionsWithoutActions, metaVariablesRow, metaVariablesColumn);
            if ((ctNew && ts).getNonZeroCount() > 1) { // [rmnt] there was another state in T_S in the same SCC as t, so we can ignore t.
                ts &= ! t;
                c.erase(it++);
            } else {
                if (ctNew == ct) {
                    return ctNew;
                }
                c[t] = ctNew;
                ++it;
            }
        }
    }
}

// As described in
// "Symbolic Algorithms for Graphs and Markov Decision Processes with Fairness Objectives"
// Uses a lockstep search
template<storm::dd::DdType Type, typename ValueType>
std::vector<storm::dd::Bdd<Type>> symbolicMECDecompositionLockstep(storm::dd::Bdd<Type> const & allStates,
                                                                   storm::dd::Bdd<Type> const & transitionsWithActions,
                                                                   std::set<storm::expressions::Variable> const & metaVariablesRow,
                                                                   std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                                                   std::set<storm::expressions::Variable> const & metaVariablesActions,
                                                                   std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const & metaVariablesRowColumnPairs) {
    struct MecCandidate {
        storm::dd::Bdd<Type> states; // In paper: S
        storm::dd::Bdd<Type> statesWithRemovedEdge; // In Paper: T_s
    };

    // Helper function for better readability
    auto hasAtLeastOneEdge =
        [metaVariablesActions, metaVariablesRow, metaVariablesColumn, metaVariablesRowColumnPairs]
        (storm::dd::Bdd<Type> scc, storm::dd::Bdd<Type> transitionsWithActions) {
        // Paper suggestion
        return ! (post(scc, scc, transitionsWithActions.existsAbstract(metaVariablesActions), metaVariablesRow, metaVariablesColumn)).isZero();
    };

    uint_fast64_t m = (transitionsWithActions.existsAbstract(metaVariablesActions) && allStates).getNonZeroCount();
    storm::dd::Bdd<Type> workingCopyTransitionsWithActions(transitionsWithActions);
    std::vector<storm::dd::Bdd<Type>> result{}; // In paper: "goodC"
    std::stack<MecCandidate> mecCandidates{}; // In paper: X
    for (const auto & scc : symbolicScc::decomposition<Type, ValueType>(allStates, workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions), metaVariablesRow, metaVariablesColumn)) {
        MecCandidate pair = { scc, scc.getDdManager().getBddZero() };
        mecCandidates.push(pair);
    }
    while ( ! mecCandidates.empty()) {
        MecCandidate currentCandidate = mecCandidates.top();
        storm::dd::Bdd<Type> scc = currentCandidate.states;
        storm::dd::Bdd<Type> sccTs = currentCandidate.statesWithRemovedEdge;
        mecCandidates.pop();

        storm::dd::Bdd<Type> sccROut = ROut(scc, workingCopyTransitionsWithActions, metaVariablesColumn, metaVariablesRowColumnPairs);
        StateActionPair<Type> randomAttractor = computeRandomAttractor(sccROut, scc, workingCopyTransitionsWithActions, metaVariablesColumn, metaVariablesActions, metaVariablesRowColumnPairs);
        scc &= ! randomAttractor.states;
        workingCopyTransitionsWithActions &= ! randomAttractor.actions;
        sccTs = (sccTs || randomAttractor.actions.existsAbstract(metaVariablesActions)) && scc;
        if (hasAtLeastOneEdge(scc, workingCopyTransitionsWithActions)) {
            if (sccTs.isZero()) {
                result.template emplace_back(scc);
            } else if (sccTs.getNonZeroCount() >= sqrt(m)) {
                std::vector<storm::dd::Bdd<Type>> subSccs = symbolicScc::decomposition<Type, ValueType>(
                    scc, workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions), metaVariablesRow, metaVariablesColumn);
                if (subSccs.size() == 1) {
                    result.template emplace_back(scc);
                } else {
                    for (auto const& subScc : subSccs) {
                        MecCandidate toProcess = {subScc, scc.getDdManager().getBddZero()};
                        mecCandidates.push(toProcess);
                    }
                }
            } else {
                // bottomScc is "C" in paper
                storm::dd::Bdd<Type> bottomScc = LockStepSearch<Type, ValueType>(scc, sccTs,
                                                                                 workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions),
                                                                                 metaVariablesRow, metaVariablesColumn);
                if (hasAtLeastOneEdge(bottomScc, workingCopyTransitionsWithActions)) {
                    result.template emplace_back(bottomScc);
                }

                storm::dd::Bdd<Type> statesLeadingIntoBottomScc = pre(bottomScc, scc, workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions), metaVariablesRow, metaVariablesColumn);
                mecCandidates.emplace((MecCandidate) {
                    scc && ( ! bottomScc),
                    (scc && ( ! bottomScc)) && (statesLeadingIntoBottomScc || sccTs),
                });
            }
        }
    }
    return result;
}

} // namespace symbolicMEC

#endif