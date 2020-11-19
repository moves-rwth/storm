#include "storm-pars/analysis/OrderExtenderMdp.h"

namespace storm {
    namespace analysis {

        template<typename ValueType, typename ConstantType>
        OrderExtenderMdp<ValueType, ConstantType>::OrderExtenderMdp(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula, storage::ParameterRegion<ValueType> region, bool prMax) : OrderExtender<ValueType, ConstantType>(model, formula, region) {
            this->prMax = prMax;
        }

        template<typename ValueType, typename ConstantType>
        OrderExtenderMdp<ValueType, ConstantType>::OrderExtenderMdp(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix, bool prMax) : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix) {
            this->prMax = prMax;
        }

        template<typename ValueType, typename ConstantType>
        storm::storage::BitVector OrderExtenderMdp<ValueType, ConstantType>::gatherPotentialSuccs(uint64_t state) {
            auto succs = this->stateMap[state];
            auto res = storm::storage::BitVector(this->numberOfStates);
            for (auto & act : succs) {
                for (auto & succ : act) {
                    res.set(succ, true);
                }
            }
            return res;
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> OrderExtenderMdp<ValueType, ConstantType>::extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState) {
            // Finding the best action for the current state
            // TODO this does not work in every case
            STORM_PRINT("Looking for best action for state " << currentState << std::endl);
            uint64_t  bestAct = 0;
            if (order->isTopState(currentState)) {
                // in this case the state should be absorbing so we just take action 0
                order->addToMdpScheduler(currentState, bestAct);
                order->addToNode(currentState, order->getTop());
                STORM_PRINT("   State is top state + thus absorbing. Take action 0." << std::endl);
                return {this->numberOfStates, this->numberOfStates};
            }
            if (order ->isBottomState(currentState)) {
                // in this case the state should be absorbing so we just take action 0
                order->addToMdpScheduler(currentState, bestAct);
                order->addToNode(currentState, order->getBottom());
                STORM_PRINT("   State is bottom state + thus absorbing. Take action 0." << std::endl);
                return {this->numberOfStates, this->numberOfStates};
            }
            if (this->stateMap[currentState].size() == 1){
                // if we only have one possible action, we already know which one we take.
                STORM_PRINT("   Only one Action available, take it." << std::endl);
                order->addToMdpScheduler(currentState, bestAct);
            } else {
                // note that succs in this function mean potential succs
                auto orderedSuccs = order->sortStates(gatherPotentialSuccs(currentState));
                auto nrOfSuccs = orderedSuccs.size();
                if (prMax) {
                    STORM_PRINT("Interested in PrMax." << std::endl);
                    if (nrOfSuccs == 2) {
                        uint64_t bestSucc = orderedSuccs[0];
                        boost::optional<storm::RationalFunction> bestFunc;
                        auto index = 0;
                        for (auto & action : this->matrix.getRowGroup(currentState)) {
                            auto itr = action.begin();
                            while (itr != action.end() && itr->getColumn() != bestSucc) {
                                itr++;
                            }
                            if (!bestFunc || (itr != action.end() && itr->getEntry() > bestFunc.get())) {
                                bestFunc = itr->getEntry();
                                bestAct = index;
                            }
                            ++index;
                        }
                        STORM_PRINT("   Two potential succs from 2 or more actions. Best action: " << bestAct << std::endl);
                    } else {
                        // more than 2 succs
                        std::map<uint64_t, uint64_t> weightMap;
                        for (uint64_t i = 0; i < nrOfSuccs; i++) {
                            weightMap.insert(orderedSuccs[i], nrOfSuccs - i);
                        }
                        boost::optional<storm::RationalFunction> bestCoeff;
                        auto index = 0;
                        for (auto & action : this->matrix.getRowGroup(currentState)) {
                            storm::RationalFunction currentCoeff;
                            for (auto & entry : action) {
                                currentCoeff += entry.getEntry() * weightMap[entry.getColumn()];
                            }
                            if (!bestCoeff || !isFunctionGreaterEqual(bestCoeff, currentCoeff, this->region)) {
                                bestCoeff = currentCoeff;
                                bestAct = index;
                            }
                            index++;
                        }
                        STORM_PRINT("   More than 2 potential succs from 2 or more actions. Best action: " << bestAct << " with MaxCoeff " << bestCoeff << std::endl);

                    }
                } else {
                    // We are interested in PrMin
                    STORM_PRINT("Interested in PrMax." << std::endl);
                    if (nrOfSuccs == 2) {
                        uint64_t bestSucc = orderedSuccs[1];
                        boost::optional<storm::RationalFunction> bestFunc;
                        auto index = 0;
                        for (auto & action : this->matrix.getRowGroup(currentState)) {
                            auto itr = action.begin();
                            while (itr != action.end() && itr->getColumn != bestSucc) {
                                itr++;
                            }
                            if (!bestFunc || (itr != action.end() && itr->getEntry() < bestFunc)) {
                                bestFunc = itr->getEntry();
                                bestAct = index;
                            }
                            index++;
                        }
                        STORM_PRINT("   Two potential succs from 2 or more actions. Best action: " << bestAct << std::endl);

                    } else {
                        // more than 2 succs
                        std::map<uint64_t, uint64_t> weightMap;
                        for (uint64_t i = 0; i < nrOfSuccs; i++) {
                            weightMap.insert(orderedSuccs[i], nrOfSuccs - i);
                        }
                        boost::optional<storm::RationalFunction> bestCoeff;
                        auto index = 0;
                        for (auto & action : this->matrix.getRowGroup(currentState)) {
                            storm::RationalFunction currentCoeff;
                            for (auto & entry : action) {
                                currentCoeff += entry.getEntry() * weightMap[entry.getColumn()];
                            }
                            if (!bestCoeff || !isFunctionGreaterEqual(currentCoeff, bestCoeff, this->region)) {
                                bestCoeff = currentCoeff;
                                bestAct = index;
                            }
                            index++;
                        }
                        STORM_PRINT("   More than 2 potential succs from 2 or more actions. Best action: " << bestAct << " with MaxCoeff " << bestCoeff << std::endl);

                    }
                }
                order->addToMdpScheduler(currentState, bestAct);
            }

            // Actual extending of the order here
            std::vector<uint64_t> successors = this->stateMap[currentState][bestAct]; // Get actual succs
            successors = order->sortStates(&successors); // Order them
            return this->extendByBackwardReasoning(order, currentState, successors); // Call Base Class function.

        }

        template<typename ValueType, typename ConstantType>
        bool OrderExtenderMdp<ValueType, ConstantType>::isFunctionGreaterEqual(storm::RationalFunction f1, storm::RationalFunction f2, storage::ParameterRegion<ValueType> region) {
            // We want to prove f1 >= f2, so we need UNSAT for f1 < f2
            std::shared_ptr<expressions::ExpressionManager> manager(new expressions::ExpressionManager());

            // Transform functions into expressions
            auto valueTypeToExpression = expressions::RationalFunctionToExpression<ValueType>(manager);
            auto exprF1 = valueTypeToExpression(f1);
            auto exprF2 = valueTypeToExpression(f2);

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
            // TODO what happens if the function is neither always geq nor less than the other in the given regions?
            if (smtRes == solver::SmtSolver::CheckResult::Unsat) {
                return true;
            } else {
                return false;
            }
        }



        template class OrderExtenderMdp<RationalFunction, double>;
        template class OrderExtenderMdp<RationalFunction, RationalNumber>;

    }
}