#pragma once
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/sparse/StateValuations.h"

namespace storm::storage::sparse {

/*!
 * Transforms the given state valuations to a new state valuations over a new variable set.
 * The values of the new variables are determined by evaluating the provided expressions w.r.t. the old variable valuation.
 * The freshly introduced variables may either replace or extend the existing variable set.
class StateValuationTransform {
    // TODO: Also support integer variables.
   public:
    StateValuationTransform(StateValuations const& oldStateValuations) : oldStateValuations(oldStateValuations) {}
    void addBooleanExpression(storm::expressions::Variable const& var, storm::expressions::Expression const& expr);
    StateValuations buildNewStateValuations(bool extend);

   private:
    StateValuations const& oldStateValuations;
    std::vector<storm::expressions::Variable> booleanVariables;
    std::vector<storm::expressions::Expression> booleanExpressions;
};

}  // namespace storm::storage::sparse