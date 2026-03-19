#pragma once
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/sparse/StateValuations.h"

namespace storm::storage::sparse {

/*!
 * Transforms the given state valuations to a new state valuations over a new variable set.
 * The values of the new variables are determined by evaluating the provided expressions w.r.t. the old variable valuation.
 * The freshly introduced variables may either replace or extend the existing variable set.
 */
class StateValuationTransformer {
   public:
    StateValuationTransformer(StateValuations const& oldStateValuations);
    /*!
     * Add a Boolean variable defined by the given expression. Note that these should all be over the same expression manager.
     * @param var A variable with type Bool
     * @param expr An expression with type Bool
     */
    void addBooleanExpression(storm::expressions::Variable const& var, storm::expressions::Expression const& expr);
    /*!
     * Add a Integer variable defined by the given expression. See also addBooleanExpression.
     */
    void addIntegerExpression(storm::expressions::Variable const& var, storm::expressions::Expression const& expr);
    /*!
     * Build and export the state valuations. Should be called only once.
     * @param extend Whether to maintain also the existing variables.
     * @return
     */
    StateValuations build(bool extend);

   private:
    StateValuations const& oldStateValuations;
    std::vector<storm::expressions::Variable> booleanVariables;
    std::vector<storm::expressions::Expression> booleanExpressions;
    std::vector<storm::expressions::Variable> integerVariables;
    std::vector<storm::expressions::Expression> integerExpressions;
};

}  // namespace storm::storage::sparse