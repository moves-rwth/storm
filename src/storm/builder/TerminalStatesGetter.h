#pragma once

#include <functional>
#include <string>
#include <vector>

namespace storm {

namespace expressions {
class Expression;
}
namespace logic {
class Formula;
}

namespace builder {

/*!
 * Traverses the formula. If an expression (or label) is found such that checking the formula does not require further exploration from a state
 * satisfying (or violating) the expression (or label), the corresponding callback function is called.
 * @param formula The formula to analyzed
 * @param terminalExpressionCallback called on terminal expressions. The corresponding flag indicates whether exploration can stop from states satisfying (true)
 * or violating (false) the expression
 * @param terminalLabelCallback called on terminal labels. The corresponding flag indicates whether exploration can stop from states having (true) or not having
 * (false) the label
 */
void getTerminalStatesFromFormula(storm::logic::Formula const& formula,
                                  std::function<void(storm::expressions::Expression const&, bool)> const& terminalExpressionCallback,
                                  std::function<void(std::string const&, bool)> const& terminalLabelCallback);

struct TerminalStates {
    std::vector<storm::expressions::Expression> terminalExpressions;         // if one of these is true, we can stop exploration
    std::vector<storm::expressions::Expression> negatedTerminalExpressions;  // if one of these is false, we can stop exploration
    std::vector<std::string> terminalLabels;                                 // if a state has one of these labels, we can stop exploration
    std::vector<std::string> negatedTerminalLabels;                          // if a state does not have all of these labels, we can stop exploration

    /*!
     * Clears all terminal states. After calling this, empty() holds.
     */
    void clear();

    /*!
     * True if no terminal states are specified
     */
    bool empty() const;

    /*!
     * Returns an expression that evaluates to true only if the exploration can stop at the corresponding state
     * Should only be called if this is not empty.
     */
    storm::expressions::Expression asExpression(std::function<storm::expressions::Expression(std::string const&)> const& labelToExpressionMap) const;
};

/*!
 * Traverses the formula. If an expression (or label) is found such that checking the formula does not require further exploration from a state
 * satisfying (or violating) the expression (or label), it is inserted into the returned struct.
 */
TerminalStates getTerminalStatesFromFormula(storm::logic::Formula const& formula);

}  // namespace builder
}  // namespace storm