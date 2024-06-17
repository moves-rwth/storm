#ifndef STORM_STORAGE_SYMBOLICSCCDECOMPOSITION_H
#define STORM_STORAGE_SYMBOLICSCCDECOMPOSITION_H

#include <stack>
#include "storm/storage/SymbolicOperations.h"

namespace symbolicSCC {

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
static std::vector<storm::dd::Bdd<Type>> decomposition(storm::dd::Bdd<Type> const& allStates, storm::dd::Bdd<Type> const& transitions,
                                                       std::set<storm::expressions::Variable> const& metaVariablesRow,
                                                       std::set<storm::expressions::Variable> const& metaVariablesColumn) {
    std::vector<storm::dd::Bdd<Type>> result = {};
    if (allStates.isZero()) {
        return result;
    }
    std::stack<TaskEntry<Type>> workQueue;
    {
        TaskEntry<Type> initialTask = {allStates, allStates.getDdManager().getBddZero(), allStates.getDdManager().getBddZero()};
        workQueue.push(initialTask);
    }

    // Work through tasks
    while (!workQueue.empty()) {
        TaskEntry<Type> currentTask = workQueue.top();
        workQueue.pop();
        assert(!currentTask.states.isZero());

        // "Determine the node for which the scc is computed"
        if (currentTask.node.isZero() && currentTask.s.isZero()) {  // Modified to allow for specification of a starting vertex
            currentTask.node = pick<Type, ValueType>(currentTask.states);
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
                level = (!fw) && post(level, currentTask.states, transitions, metaVariablesRow, metaVariablesColumn);
            }

            // "Determine a Skeleton of the Forward-Set"
            level = stack.top();
            stack.pop();
            newNode = pick<Type, ValueType>(level);  // TODO: Better Name
            newS = storm::dd::Bdd<Type>(newNode);    // TODO: Better name for this variable
            while (!stack.empty()) {
                level = stack.top();
                stack.pop();
                storm::dd::Bdd<Type> toPickFrom = level && pre(newS, currentTask.states, transitions, metaVariablesRow, metaVariablesColumn);
                newS |= pick<Type, ValueType>(toPickFrom);
            }
        }

        // "Determine the scc containing NODE"
        storm::dd::Bdd<Type> scc = storm::dd::Bdd<Type>(currentTask.node);
        {
            bool changed = true;
            while (changed) {
                storm::dd::Bdd<Type> updatedScc = fw && pre(scc, currentTask.states, transitions, metaVariablesRow, metaVariablesColumn);
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
                sToCheck && pre(scc && currentTask.s, currentTask.states, transitions, metaVariablesRow, metaVariablesColumn),
            };
            if (!newTask.states.isZero()) {
                workQueue.push(newTask);
            }
        }

        // "Second recursive call : Computation of the scc's in FW \ SCC"
        {
            TaskEntry<Type> newTask = {
                fw && (!scc),
                newS && (!scc),
                newNode && (!scc),
            };
            if (!newTask.states.isZero()) {
                workQueue.push(newTask);
            }
        }
    }
    return result;
}

}  // namespace symbolicSCC

#endif
