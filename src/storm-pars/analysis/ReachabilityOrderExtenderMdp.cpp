#include "storm-pars/analysis/ReachabilityOrderExtenderMdp.h"
#include "storm-pars/api/region.h"

namespace storm {
    namespace analysis {

        template<typename ValueType, typename ConstantType>
        ReachabilityOrderExtenderMdp<ValueType, ConstantType>::ReachabilityOrderExtenderMdp(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula, bool prMax) : ReachabilityOrderExtender<ValueType, ConstantType>(model, formula) {
            if (formula->isProbabilityOperatorFormula()) {
                this->prMax = storm::solver::maximize(formula->asProbabilityOperatorFormula().getOptimalityType());
            } else {
                assert (false);
            }
        }

        template<typename ValueType, typename ConstantType>
        ReachabilityOrderExtenderMdp<ValueType, ConstantType>::ReachabilityOrderExtenderMdp(storm::storage::BitVector& topStates,  storm::storage::BitVector& bottomStates, storm::storage::SparseMatrix<ValueType> matrix, bool prMax) : ReachabilityOrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix) {
            this->prMax = prMax;
        }

        template<typename ValueType, typename ConstantType>
        bool ReachabilityOrderExtenderMdp<ValueType, ConstantType>::findBestAction(std::shared_ptr<Order> order, storage::ParameterRegion<ValueType>& region, uint_fast64_t state) {
            // Finding the best action for the current state
            STORM_LOG_INFO("Looking for best action for state " << state << std::endl);
            if (order->isActionSetAtState(state)) {
                STORM_LOG_INFO("Best action for state " << state << " is already set." << std::endl);
                return true;
            }
            if (this->stateMap[state].size() == 1){
                // if we only have one possible action, we already know which one we take.
                STORM_LOG_INFO("   Only one Action available, take it." << std::endl);
                order->addToMdpScheduler(state, 0);
                return true;

            }
            if (order->isTopState(state)) {
                // in this case the state should be absorbing so we just take action 0
                STORM_LOG_INFO("   State is top state, thus absorbing. Take action 0." << std::endl);
                order->addToMdpScheduler(state, 0);
                return true;
            }
            if (order->isBottomState(state)) {
                // in this case the state should be absorbing so we just take action 0
                STORM_LOG_INFO("   State is bottom state, thus absorbing. Take action 0." << std::endl);
                order->addToMdpScheduler(state, 0);
                return true;
            }

            // note that succs in this function mean potential succs
            uint64_t bestAct = this->matrix.getRowCount();
            auto successors = this->getSuccessors(state, order);
            auto orderedSuccs = order->sortStates(successors.second);
            if (orderedSuccs.back() == this->numberOfStates){
                order->toDotOutput();
                STORM_LOG_WARN("    No best action found, as the successors could not be ordered.");
                return false;
            }
            auto nrOfSuccs = orderedSuccs.size();
            if (prMax) {
                STORM_LOG_INFO("   Interested in PrMax." << std::endl);
                auto action = 0;
                auto bestSoFar = action;
                auto numberOfOptionsForState = this->matrix.getRowGroupSize(state);
                std::set<uint_fast64_t> actionsToIgnore;
                while (action < numberOfOptionsForState) {
                    if (actionsToIgnore.find(action) == actionsToIgnore.end()) {
                        auto rowA = this->matrix.getRow(state, action);
                        bool changed = false;
                        for (uint_fast64_t i = action + 1; i < numberOfOptionsForState; ++i) {
                            if (actionsToIgnore.find(i) == actionsToIgnore.end()) {
                                auto rowB = this->matrix.getRow(state, i);
                                auto compRes = actionSMTCompare(order, orderedSuccs, region, &rowA, &rowB);
                                if (compRes == GEQ) {
                                    // rowA is smaller or equal than rowB, so action is smaller than i, so we continue with action
                                    // We will ignore i as we know action is better
                                    actionsToIgnore.insert(i);
                                } else if (compRes == LEQ) {
                                    changed = true;
                                } else {
                                    // we ignore both action and i as i is sometimes better than action and vice versa
                                    actionsToIgnore.insert(i);
                                    actionsToIgnore.insert(action);
                                }
                            }
                        }
                        if (!changed) {
                            // this action is better than all other actions
                            order->addToMdpScheduler(state, action);
                            return true;
                        }
                    }
                    action++;
                }
                return false;
            } else {
                STORM_LOG_INFO("   Interested in PrMin." << std::endl);
                auto action = 0;
                auto bestSoFar = action;
                auto numberOfOptionsForState = this->matrix.getRowGroupSize(state);
                std::set<uint_fast64_t> actionsToIgnore;
                while (action < numberOfOptionsForState) {
                    if (actionsToIgnore.find(action) == actionsToIgnore.end()) {
                        auto rowA = this->matrix.getRow(state, action);
                        bool changed = false;
                        for (uint_fast64_t i = action + 1; i < numberOfOptionsForState; ++i) {
                            if (actionsToIgnore.find(i) == actionsToIgnore.end()) {
                                auto rowB = this->matrix.getRow(state, i);
                                auto compRes = actionSMTCompare(order, orderedSuccs, region, &rowA, &rowB);
                                if (compRes == LEQ) {
                                    // rowA is smaller or equal than rowB, so action is smaller than i, so we continue with action
                                    // We will ignore i as we know action is better
                                    actionsToIgnore.insert(i);
                                } else if (compRes == GEQ) {
                                    changed = true;
                                } else {
                                    // we ignore both action and i as i is sometimes better than action and vice versa
                                    actionsToIgnore.insert(i);
                                    actionsToIgnore.insert(action);
                                }
                            }
                        }
                        if (!changed) {
                            // this action is better than all other actions
                            order->addToMdpScheduler(state, action);
                            return true;
                        }
                    }
                    action++;
                }
                return false;
            }
        }

        template<typename ValueType, typename ConstantType>
        bool ReachabilityOrderExtenderMdp<ValueType, ConstantType>::isFunctionGreaterEqual(storm::RationalFunction f1, storm::RationalFunction f2, storage::ParameterRegion<ValueType> region) {
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

        template<typename ValueType, typename ConstantType>
        std::pair<uint64_t, uint64_t> ReachabilityOrderExtenderMdp<ValueType, ConstantType>::rangeOfSuccsForAction(typename storage::SparseMatrix<ValueType>::rows* action, std::vector<uint64_t> orderedSuccs){
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

            return std::make_pair(start,end);
        }

        template<typename ValueType, typename ConstantType>
        typename ReachabilityOrderExtenderMdp<ValueType, ConstantType>::ActionComparison ReachabilityOrderExtenderMdp<ValueType, ConstantType>::actionSMTCompare(
            std::shared_ptr<Order> order, const std::vector<uint64_t>& orderedSuccs, storage::ParameterRegion<ValueType>& region,
            ReachabilityOrderExtenderMdp::Rows action1, ReachabilityOrderExtenderMdp::Rows action2) {
            std::shared_ptr<expressions::ExpressionManager> manager(new expressions::ExpressionManager());

            // Get ordered vector of the succs actually occurring in the two actions
            std::vector<uint64_t> occSuccs = std::vector<uint64_t>();
            std::set<uint64_t> occSuccSet = std::set<uint64_t>();
            for (auto entry : *action1){
                occSuccSet.insert(entry.getColumn());
            }
            for (auto entry : *action2){
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
                if(i > 0) {
                    if (order->compare(occSuccs[i], occSuccs[i - 1]) == Order::SAME){
                        auto sameVar = manager->getVariable("s" + std::to_string(occSuccs[i-1]));
                        expressions::Expression exprSame = sameVar.getExpression() = var.getExpression();
                        exprStateVars = exprStateVars && exprSame;
                    } else {
                        auto biggerVar = manager->getVariable("s" + std::to_string(occSuccs[i-1]));
                        expressions::Expression exprBigger = biggerVar.getExpression() > var.getExpression();
                        exprStateVars = exprStateVars && exprBigger;
                    }
                }

            }

            // Turn rational functions into expressions
            auto valueTypeToExpression = expressions::RationalFunctionToExpression<ValueType>(manager);
            auto exprF1 = manager->rational(0);
            for (auto entry : *action1) {
                uint64_t column = entry.getColumn();
                std::string name = "s" + std::to_string(column);
                exprF1 = exprF1 + valueTypeToExpression.toExpression(entry.getValue()) * manager->getVariable(name);
            }
            auto exprF2 = manager->rational(0);
            for (auto entry : *action2) {
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

        template class ReachabilityOrderExtenderMdp<RationalFunction, double>;
        template class ReachabilityOrderExtenderMdp<RationalFunction, RationalNumber>;

    }
}