#include "ActionComparator.h"

#include "storm-pars/api/export.h"
#include "storm-pars/api/region.h"
#include "storm/api/verification.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/macros.h"

namespace storm {
namespace analysis {

template<typename ValueType>
ActionComparator<ValueType>::ActionComparator() {
    // Intentionally left empty
}

template<typename ValueType>
typename ActionComparator<ValueType>::ComparisonResult ActionComparator<ValueType>::actionSMTCompare(
    std::shared_ptr<Order> order, const std::vector<uint64_t>& orderedSuccs, storage::ParameterRegion<ValueType>& region, ValueType reward1, ValueType reward2,
    ActionComparator::Rows row1, ActionComparator::Rows row2) const {
    STORM_LOG_ASSERT(reward1.isConstant() && reward2.isConstant(), "Expecting rewards to be constant");
    auto res = actionQuickCheck(order, orderedSuccs, region, reward1, reward2, row1, row2);
    if (res != UNKNOWN) {
        return res;
    }
    std::shared_ptr<expressions::ExpressionManager> manager(new expressions::ExpressionManager());

    // Get ordered vector of the succs actually occurring in the two actions
    std::vector<uint64_t> occSuccs = std::vector<uint64_t>();
    std::set<uint64_t> occSuccSet = std::set<uint64_t>();
    for (auto entry : *row1) {
        occSuccSet.insert(entry.getColumn());
    }
    for (auto entry : *row2) {
        occSuccSet.insert(entry.getColumn());
    }
    for (auto a : orderedSuccs) {
        if (occSuccSet.find(a) != occSuccSet.end()) {
            occSuccs.push_back(a);
        }
    }

    // Turn everything we know about our succs into expressions
    expressions::Expression exprStateVars = manager->boolean(true);
    std::set<std::string> stateVarNames;
    for (uint_fast64_t i = 0; i < occSuccs.size(); i++) {
        std::string varName = "s" + std::to_string(occSuccs[i]);
        stateVarNames.insert(varName);
        auto var = manager->declareRationalVariable(varName);
        exprStateVars = exprStateVars && manager->rational(0) < var && var < manager->rational(1);
        if (i > 0) {
            if (order->compare(occSuccs[i], occSuccs[i - 1]) == Order::SAME) {
                auto sameVar = manager->getVariable("s" + std::to_string(occSuccs[i - 1]));
                expressions::Expression exprSame = sameVar.getExpression() == var.getExpression();
                exprStateVars = exprStateVars && exprSame;
            } else {
                auto biggerVar = manager->getVariable("s" + std::to_string(occSuccs[i - 1]));
                expressions::Expression exprBigger = biggerVar.getExpression() > var.getExpression();
                exprStateVars = exprStateVars && exprBigger;
            }
        }
    }

    // Turn rational functions into expressions
    auto valueTypeToExpression = expressions::RationalFunctionToExpression<ValueType>(manager);
    auto exprF1 = manager->rational(storm::utility::convertNumber<storm::RationalNumber>(reward1.constantPart()));
    for (auto entry : *row1) {
        uint64_t column = entry.getColumn();
        std::string name = "s" + std::to_string(column);
        exprF1 = exprF1 + valueTypeToExpression.toExpression(entry.getValue()) * manager->getVariable(name);
    }
    auto exprF2 = manager->rational(storm::utility::convertNumber<storm::RationalNumber>(reward2.constantPart()));
    for (auto entry : *row2) {
        uint64_t column = entry.getColumn();
        std::string name = "s" + std::to_string(column);
        exprF2 = exprF2 + valueTypeToExpression.toExpression(entry.getValue()) * manager->getVariable(name);
    }

    // Turn parameter bounds into expressions
    expressions::Expression exprParamBounds = manager->boolean(true);
    auto variables = manager->getVariables();
    for (auto var : variables) {
        std::string name = var.getName();
        if (stateVarNames.find(name) == stateVarNames.end()) {
            auto lb = utility::convertNumber<RationalNumber>(region.getLowerBoundary(name));
            auto ub = utility::convertNumber<RationalNumber>(region.getUpperBoundary(name));
            exprParamBounds = exprParamBounds && manager->rational(lb) < var && var < manager->rational(ub);
        }
    }

    // Check if (action1 >= action2) -> check if (action2 > action1) is UNSAT. If yes --> GEQ. If no --> continue
    auto exprToCheck = exprF1 < exprF2;
    solver::Z3SmtSolver s1(*manager);
    s1.add(exprToCheck);
    s1.add(exprStateVars);
    s1.add(exprParamBounds);
    auto smtRes = s1.check();
    if (smtRes == solver::SmtSolver::CheckResult::Unsat) {
        return GEQ;
    }

    // Check if (action2 >= action1) -> check if (action1 > action2) is UNSAT. If yes --> LEQ. If no --> UNKNOWN
    exprToCheck = exprF2 < exprF1;
    solver::Z3SmtSolver s2(*manager);
    s2.add(exprToCheck);
    s2.add(exprStateVars);
    s2.add(exprParamBounds);
    smtRes = s2.check();
    if (smtRes == solver::SmtSolver::CheckResult::Unsat) {
        return LEQ;
    } else {
        return UNKNOWN;
    }
}

template<typename ValueType>
typename ActionComparator<ValueType>::ComparisonResult ActionComparator<ValueType>::actionQuickCheck(
    std::shared_ptr<Order> order, const std::vector<uint64_t>& orderedSuccs, storage::ParameterRegion<ValueType>& region, ValueType rew1, ValueType rew2,
    ActionComparator::Rows row1, ActionComparator::Rows row2) const {
    if (row1->begin() + 1 == row1->end() && row2->begin() + 1 == row2->end()) {
        // row1 and row2 only have one entry
        if (orderedSuccs.size() == 1) {
            // they have same order
            return rew1.constantPart() >= rew2.constantPart() ? GEQ : LEQ;
        }
        for (auto i = 0; i < orderedSuccs.size() - 2; ++i) {
            if (orderedSuccs[i] == row1->begin()->getColumn() && rew1.constantPart() >= rew2.constantPart()) {
                // row1 is above row2
                return GEQ;
            } else if (orderedSuccs[i] == row2->begin()->getColumn() && rew2.constantPart() >= rew1.constantPart()) {
                return LEQ;
            }
        }
    }
    if (row1->begin() + 1 == row1->end() && orderedSuccs[0] == row1->begin()->getColumn() && rew1.constantPart() >= rew2.constantPart()) {
        // row1 only has one successor, and this succ is above all others
        return GEQ;
    }
    if (row1->begin() + 1 == row1->end() && orderedSuccs[orderedSuccs.size() - 1] == row1->begin()->getColumn() && rew2.constantPart() >= rew1.constantPart()) {
        // row1 only has one successor, and this succ is below all others
        return LEQ;
    }
    if (row2->begin() + 1 == row2->end() && orderedSuccs[0] == row2->begin()->getColumn() && rew2.constantPart() >= rew1.constantPart()) {
        // row2 only has one successor, and this succ is above all others
        return LEQ;
    }
    if (row2->begin() + 1 == row2->end() && orderedSuccs[orderedSuccs.size() - 1] == row2->begin()->getColumn() && rew1.constantPart() >= rew2.constantPart()) {
        // row2 only has one successor, and this succ is below all others
        return GEQ;
    }

    bool intersect = true;
    for (auto& entry1 : *row1) {
        bool found = false;
        for (auto& entry2 : *row2) {
            found = entry1.getColumn() == entry2.getColumn();
            if (found) {
                intersect &= entry1.getValue() == entry2.getValue();
                break;
            }
        }
        intersect &= found;
        if (!intersect) {
            break;
        }
    }
    if (intersect && rew1.constantPart() >= rew2.constantPart()) {
        return GEQ;
    } else if (intersect && rew2.constantPart() >= rew1.constantPart()) {
        return LEQ;
    }
    return UNKNOWN;
}

template<typename ValueType>
bool ActionComparator<ValueType>::isFunctionGreaterEqual(storm::RationalFunction f1, storm::RationalFunction f2,
                                                         storage::ParameterRegion<ValueType> region) const {
    // We want to prove f1 >= f2, so we need UNSAT for f1 < f2
    std::shared_ptr<expressions::ExpressionManager> manager(new expressions::ExpressionManager());

    // Transform functions into expressions
    auto valueTypeToExpression = expressions::RationalFunctionToExpression<ValueType>(manager);
    auto exprF1 = valueTypeToExpression.toExpression(f1);
    auto exprF2 = valueTypeToExpression.toExpression(f2);

    // Add bounds for parameters from region
    expressions::Expression exprBounds = manager->boolean(true);
    auto variables = manager->getVariables();
    for (auto var : variables) {
        auto lb = utility::convertNumber<RationalNumber>(region.getLowerBoundary(var.getName()));
        auto ub = utility::convertNumber<RationalNumber>(region.getUpperBoundary(var.getName()));
        exprBounds = exprBounds && manager->rational(lb) < var && var < manager->rational(ub);
    }

    // Use SMTSolver
    auto exprToCheck = exprF1 < exprF2;
    solver::Z3SmtSolver s(*manager);
    s.add(exprToCheck);
    s.add(exprBounds);
    auto smtRes = s.check();

    // Evaluate Result
    if (smtRes == solver::SmtSolver::CheckResult::Unsat) {
        return true;
    } else {
        return false;
    }
}

template<typename ValueType>
std::pair<uint64_t, uint64_t> ActionComparator<ValueType>::rangeOfSuccsForAction(typename storage::SparseMatrix<ValueType>::rows* action,
                                                                                 std::vector<uint64_t> orderedSuccs) const {
    uint64_t start = orderedSuccs.size();
    uint64_t end = 0;
    for (auto entry : *action) {
        auto succ = entry.getColumn();
        for (uint64_t i = 0; i < orderedSuccs.size(); i++) {
            if (succ == orderedSuccs[i] && i < start) {
                start = i;
            }
            if (succ == orderedSuccs[i] && i > end) {
                end = i;
            }
        }
    }

    return std::make_pair(start, end);
}

template class ActionComparator<RationalFunction>;
}  // namespace analysis
}  // namespace storm
