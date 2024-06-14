#ifndef STORM_STORAGE_SYMBOLICOPERATIONS_STATS_H
#define STORM_STORAGE_SYMBOLICOPERATIONS_STATS_H

#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"
#include "storm/storage/expressions/SimpleValuation.h"

template<storm::dd::DdType Type, typename ValueType>
static storm::dd::Bdd<Type> pick_stats(storm::dd::Bdd<Type> const & states, uint_fast64_t &countSymbolicOps) {
    assert (states.getNonZeroCount() > 0);
    countSymbolicOps++; // [rmnt] TODO : Counting each pick as one symbolic op. Ok?
    storm::dd::DdManager<Type> const & manager = states.getDdManager();
    storm::dd::Add<Type, ValueType> add = states.template toAdd<ValueType>();
    storm::expressions::SimpleValuation firstNonZeroAssigned = (*add.begin()).first;
    storm::dd::Bdd<Type> result = manager.getBddOne();
    for (auto && variable : add.getContainedMetaVariables()) {
        // Get Value to encode
        int_fast64_t value = 0;
        {
            // variable is either boolean, integer or rational
            storm::expressions::Type variableType = variable.getType();
            if (variableType.isBooleanType()) {
                value = (int_fast64_t)firstNonZeroAssigned.getBooleanValue(variable);
            } else if (variableType.isIntegerType()) {
                value = firstNonZeroAssigned.getIntegerValue(variable);
            } else {
                // How do I even convert this to int_fast64_t for the 'getEncoding'-call?
                assert( ! "Unexpected variable type" );
            }
        }

        // Copy value encoding of meta variable onto BDD
        result &= manager.getEncoding(variable, value);
    }
    assert (result.getNonZeroCount() == 1);
    return result;
}

// [rmnt] Adding new pick version to see if it's faster.
template<storm::dd::DdType Type, typename ValueType>
static storm::dd::Bdd<Type> pickv2_stats(storm::dd::Bdd<Type> const & states, uint_fast64_t &countSymbolicOps) {
    assert (! states.isZero());
    countSymbolicOps++; // [rmnt] TODO : Counting each pick as one symbolic op. Ok?
    storm::dd::Bdd<Type> result = states.existsAbstractRepresentative(states.getContainedMetaVariables());
    assert (result.getNonZeroCount() == 1);
    return result;
}

// NOTE(Felix): Modified from dd.cpp:computeReachableStates
template<storm::dd::DdType Type>
static storm::dd::Bdd<Type> post_stats(storm::dd::Bdd<Type> const & statesToApplyPostOn,
                                 storm::dd::Bdd<Type> const & allStates,
                                 storm::dd::Bdd<Type> const & transitions,
                                 std::set<storm::expressions::Variable> const & metaVariablesRow,
                                 std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                 uint_fast64_t &countSymbolicOps) {
    countSymbolicOps++; // [rmnt] TODO : Counting each post as one symbolic op. Ok?
    return allStates && statesToApplyPostOn.relationalProduct(transitions, metaVariablesRow, metaVariablesColumn);
}

// NOTE(Felix): Modified from dd.cpp:computeBackwardsReachableStates
template<storm::dd::DdType Type>
static storm::dd::Bdd<Type> pre_stats(storm::dd::Bdd<Type> const & statesToApplyPreOn,
                                storm::dd::Bdd<Type> const & allStates,
                                storm::dd::Bdd<Type> const & transitions,
                                std::set<storm::expressions::Variable> const & metaVariablesRow,
                                std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                uint_fast64_t &countSymbolicOps) {
    countSymbolicOps++; // [rmnt] TODO : Counting each pre as one symbolic op. Ok?
    return allStates && statesToApplyPreOn.inverseRelationalProduct(transitions, metaVariablesRow, metaVariablesColumn);
}

#endif