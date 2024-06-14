#ifndef STORM_STORAGE_SYMBOLICSCCDECOMPOSITION_STATS_H
#define STORM_STORAGE_SYMBOLICSCCDECOMPOSITION_STATS_H

#include <stack>
#include "storm/storage/SymbolicOperations_stats.h"

namespace symbolicSCC_stats {

template<storm::dd::DdType Type>
struct TaskEntry {
    storm::dd::Bdd<Type> states;
    storm::dd::Bdd<Type> s;
    storm::dd::Bdd<Type> node;
};

/* Ported from the paper
 * "Computing Strongly Connected Components in a Number of Symbolic Steps"
 * Modified to not use recursion, but a queue instead
 * (Gentilini, Piazza, Policriti) */
template<storm::dd::DdType Type, typename ValueType>
static std::vector<storm::dd::Bdd<Type>> decomposition_stats(storm::dd::Bdd<Type> const& allStates, storm::dd::Bdd<Type> const& transitions,
                                                       std::set<storm::expressions::Variable> const& metaVariablesRow,
                                                       std::set<storm::expressions::Variable> const& metaVariablesColumn,
                                                       uint_fast64_t &countSymbolicOps) {
    std::vector<storm::dd::Bdd<Type>> result = {};
    if (allStates.isZero()) { return result; }
    std::stack<TaskEntry<Type>> workQueue;
    {
        TaskEntry<Type> initialTask = {allStates, allStates.getDdManager().getBddZero(), allStates.getDdManager().getBddZero()};
        workQueue.push(initialTask);
    }

    // Work through tasks
    while (!workQueue.empty()) {
        TaskEntry<Type> currentTask = workQueue.top();
        workQueue.pop();
        assert( ! currentTask.states.isZero());

        // "Determine the node for which the scc is computed"
        if (currentTask.node.isZero() && currentTask.s.isZero()) {  // Modified to allow for specification of a starting vertex
            currentTask.node = pick_stats<Type, ValueType>(currentTask.states, countSymbolicOps);
        }

        // "Compute the forward-set of the vertex in NODE together with a skeleton"
        storm::dd::Bdd<Type> fw = allStates.getDdManager().getBddZero();
        storm::dd::Bdd<Type> newS;
        storm::dd::Bdd<Type> newNode;
        {
            // Inlined Skel_Forward function

            // "Compute the Forward-set and push onto STACK the onion rings"
            std::stack<storm::dd::Bdd<Type>> stack;
            storm::dd::Bdd<Type> level = storm::dd::Bdd<Type>(currentTask.node);
            while (!level.isZero()) {
                stack.push(level);
                fw |= level;
                level = (!fw) && post_stats(level, currentTask.states, transitions, metaVariablesRow, metaVariablesColumn, countSymbolicOps);
            }

            // "Determine a Skeleton of the Forward-Set"
            level = stack.top();
            stack.pop();
            newNode = pick_stats<Type, ValueType>(level, countSymbolicOps);  // TODO: Better Name
            newS = storm::dd::Bdd<Type>(newNode);    // TODO: Better name for this variable
            while (!stack.empty()) {
                level = stack.top();
                stack.pop();
                storm::dd::Bdd<Type> toPickFrom = level && pre_stats(newS, currentTask.states, transitions, metaVariablesRow, metaVariablesColumn, countSymbolicOps);
                newS |= pick_stats<Type, ValueType>(toPickFrom, countSymbolicOps);
            }
        }

        // "Determine the scc containing NODE"
        storm::dd::Bdd<Type> scc = storm::dd::Bdd<Type>(currentTask.node);
        {
            bool changed = true;
            while (changed) {
                storm::dd::Bdd<Type> updatedScc = fw && pre_stats(scc, currentTask.states, transitions, metaVariablesRow, metaVariablesColumn, countSymbolicOps);
                storm::dd::Bdd<Type> addedStates = updatedScc && (!scc);
                scc |= updatedScc;
                changed = (!addedStates.isZero());
            }
        }

        // "Insert the scc in the scc Partition"
        result.push_back(scc);

        // "First recursive call: Computation of the scc's in V \ FW"
        {
            storm::dd::Bdd<Type> sToCheck = currentTask.s && (!scc);
            TaskEntry<Type> newTask = {
                currentTask.states && (!fw),
                sToCheck,
                sToCheck && pre_stats(scc && currentTask.s, currentTask.states, transitions, metaVariablesRow, metaVariablesColumn, countSymbolicOps),
            };
            if ( ! newTask.states.isZero()) { workQueue.push(newTask); }
        }

        // "Second recursive call : Computation of the scc's in FW \ SCC"
        {
            TaskEntry<Type> newTask = {
                fw && (!scc),
                newS && (!scc),
                newNode && (!scc),
            };
            if ( ! newTask.states.isZero()) { workQueue.push(newTask); }
        }
    }
    return result;
}

} // namespace symbolicSCC_stats

#endif