#include "storm-pars/analysis/OrderExtenderMdp.h"

namespace storm {
    namespace analysis {

        template<typename ValueType, typename ConstantType>
        OrderExtenderMdp<ValueType, ConstantType>::OrderExtenderMdp(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula, storage::ParameterRegion<ValueType> region, bool prMax) : OrderExtender<ValueType, ConstantType>(model, formula, region) {
            initMdpStateMap();
            this->prMax = prMax;
        }

        template<typename ValueType, typename ConstantType>
        OrderExtenderMdp<ValueType, ConstantType>::OrderExtenderMdp(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix, bool prMax) : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix) {
            initMdpStateMap();
            this->prMax = prMax;
        }

        template<typename ValueType, typename ConstantType>
        void OrderExtenderMdp<ValueType, ConstantType>::initMdpStateMap() {
            for (uint64_t stateNr = 0; stateNr < this->numberOfStates; stateNr++) {
                for (auto & row : this->matrix.getRowGroup(stateNr)) {
                    auto i = 0;
                    for (auto & rowEntry : row) {
                        mdpStateMap[stateNr][i].push_back(rowEntry.getColumn());
                    }
                    i++;
                }
            }
        }

        template<typename ValueType, typename ConstantType>
        storm::storage::BitVector OrderExtenderMdp<ValueType, ConstantType>::gatherPotentialSuccs(uint64_t state) {
            auto succs = mdpStateMap[state];
            auto res = storm::storage::BitVector(this->numberOfStates);
            for (auto & act : succs) {
                for (auto & succ : act) {
                    res.set(succ, true);
                }
            }
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> OrderExtenderMdp<ValueType, ConstantType>::extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState) {
            // Finding the best action for the current state
            uint64_t  bestAct = 0;
            if (order->isTopState(currentState)) {
                // in this case the state should be absorbing so we just take action 0
                order->addToMdpScheduler(currentState, bestAct);
                order->addToNode(currentState, order->getTop());
                // TODO what to return?
            }
            if (order ->isBottomState(currentState)) {
                // in this case the state should be absorbing so we just take action 0
                order->addToMdpScheduler(currentState, bestAct);
                order->addToNode(currentState, order->getBottom());
                // TODO what to return?
            }
            if (mdpStateMap[currentState].size() == 1){
                // if we only have one possible action, we already know which one we take.
                order->addToMdpScheduler(currentState, bestAct);
            } else {
                // note that succs in this function mean potential succs
                auto orderedSuccs = order->sortStates(gatherPotentialSuccs(currentState));
                auto nrOfSuccs = orderedSuccs.size();
                if (prMax) {
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
                            // TODO how to compare rational functions (shouldn't there be regions involved?)
                            // Yes if you want to compare them you need regions, maybe the assumptionvalidater could inspire you here?
                            if (!bestCoeff || bestCoeff < currentCoeff) {
                                bestCoeff = currentCoeff;
                                bestAct = index;
                            }
                            index++;
                        }

                    }
                } else {
                    // We are interested in PrMin
                    // TODO make sure I am not having a big miscalculation in my thoughts here
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
                            // TODO how to compare rational functions (shouldn't there be regions involved?)
                            if (!bestCoeff || currentCoeff < bestCoeff) {
                                bestCoeff = currentCoeff;
                                bestAct = index;
                            }
                            index++;
                        }

                    }
                }
            }

            // Actual extending of the order here
            std::vector<uint64_t> successors = mdpStateMap[currentState][bestAct]; // Get actual succs
            successors = order->sortStates(&successors); // Order them
            return this->extendByBackwardReasoning(order, currentState, successors); // Call Base Class function.

        }

        // Method to compare two functions
        // We want to prove f1 >= f2, to do so we need UNSAT for f1 < f2
        // std::shared_ptr<utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<utility::solver::MathsatSmtSolverFactory>();
        // std::shared_ptr<expressions::ExpressionManager> manager(new expressions::ExpressionManager());
        // 1) transform functions into expressions
        // auto valueTypeToExpression = expressions::RationalFunctionToExpression<ValueType>(manager);
        // exprF1 = valueTypeToExpression(f1);
        // exprF2 = valueTypeToExpression(f2);
        // 2) Now add the bounds for the parameters
        //                 expressions::Expression exprBounds = manager->boolean(true);
        // auto variables = manager->getVariables();
        //                 for (auto var : variables) {
        //auto lb = utility::convertNumber<RationalNumber>(region.getLowerBoundary(var.getName()));
        //                        auto ub = utility::convertNumber<RationalNumber>(region.getUpperBoundary(var.getName()));
        //                        exprBounds = exprBounds && manager->rational(lb) < var && var < manager->rational(ub);
        // }
        // auto exprToCheck = f1 < f2
        //                 solver::Z3SmtSolver s(*manager);
        // s.add(exprToCheck);
        // s.add(exprBounds);
        // auto smtRes = s.check();
        // if (smtRes == solver::SmtSolver::CheckResult::Unsat) {}
        //



    }
}