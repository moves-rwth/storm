#pragma once
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/sparse/StateValuations.h"

namespace storm::storage::sparse {

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