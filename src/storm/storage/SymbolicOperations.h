#ifndef STORM_STORAGE_SYMBOLICOPERATIONS_H
#define STORM_STORAGE_SYMBOLICOPERATIONS_H

#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"
#include "storm/storage/expressions/SimpleValuation.h"

template<storm::dd::DdType Type, typename ValueType>
static storm::dd::Bdd<Type> pick(storm::dd::Bdd<Type> const & states) {
    assert (states.getNonZeroCount() > 0);
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
static storm::dd::Bdd<Type> pickv2(storm::dd::Bdd<Type> const & states) {
    assert (! states.isZero());
    storm::dd::Bdd<Type> result = states.existsAbstractRepresentative(states.getContainedMetaVariables());
    assert (result.getNonZeroCount() == 1);
    return result;
}

// NOTE(Felix): Modified from dd.cpp:computeReachableStates
template<storm::dd::DdType Type>
static storm::dd::Bdd<Type> post(storm::dd::Bdd<Type> const & statesToApplyPostOn,
                                 storm::dd::Bdd<Type> const & allStates,
                                 storm::dd::Bdd<Type> const & transitions,
                                 std::set<storm::expressions::Variable> const & metaVariablesRow,
                                 std::set<storm::expressions::Variable> const & metaVariablesColumn) {
    return allStates && statesToApplyPostOn.relationalProduct(transitions, metaVariablesRow, metaVariablesColumn);
}

// NOTE(Felix): Modified from dd.cpp:computeBackwardsReachableStates
template<storm::dd::DdType Type>
static storm::dd::Bdd<Type> pre(storm::dd::Bdd<Type> const & statesToApplyPreOn,
                                storm::dd::Bdd<Type> const & allStates,
                                storm::dd::Bdd<Type> const & transitions,
                                std::set<storm::expressions::Variable> const & metaVariablesRow,
                                std::set<storm::expressions::Variable> const & metaVariablesColumn) {
    return allStates && statesToApplyPreOn.inverseRelationalProduct(transitions, metaVariablesRow, metaVariablesColumn);
}

#endif