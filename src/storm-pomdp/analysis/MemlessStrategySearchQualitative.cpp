#include "storm-pomdp/analysis/MemlessStrategySearchQualitative.h"


namespace storm {
    namespace pomdp {

        template <typename ValueType>
        void MemlessStrategySearchQualitative<ValueType>::initialize(uint64_t k) {
            if (maxK == -1) {
                // not initialized at all.

                // Create some data structures.
                for(uint64_t obs = 0; obs < pomdp.getNrObservations(); ++obs) {
                    actionSelectionVars.push_back(std::vector<storm::expressions::Expression>());
                    statesPerObservation.push_back(std::vector<uint64_t>()); // Consider using bitvectors instead.
                }

                // Fill the states-per-observation mapping,
                // declare the reachability variables,
                // declare the path variables.
                uint64_t stateId = 0;
                for(auto obs : pomdp.getObservations()) {
                    pathVars.push_back(std::vector<storm::expressions::Expression>());
                    for (uint64_t i = 0; i < k; ++i) {
                        pathVars.back().push_back(expressionManager->declareBooleanVariable("P-"+std::to_string(stateId)+"-"+std::to_string(i)).getExpression());
                    }
                    reachVars.push_back(expressionManager->declareBooleanVariable("C-" + std::to_string(stateId)).getExpression());

                    statesPerObservation.at(obs).push_back(stateId++);

                }
                assert(pathVars.size() == pomdp.getNumberOfStates());

                // Create the action selection variables.
                uint64_t obs = 0;
                for(auto const& statesForObservation : statesPerObservation) {
                    for (uint64_t a = 0; a < pomdp.getNumberOfChoices(statesForObservation.front()); ++a) {
                        std::string varName = "A-" + std::to_string(obs) + "-" + std::to_string(a);
                        actionSelectionVars.at(obs).push_back(expressionManager->declareBooleanVariable(varName).getExpression());
                    }
                    ++obs;
                }


            } else {

                assert(false);

            }

            uint64_t rowindex = 0;
            for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                std::vector<std::vector<storm::expressions::Expression>> pathsubsubexprs;
                for (uint64_t j = 1; j < k; ++j) {
                    pathsubsubexprs.push_back(std::vector<storm::expressions::Expression>());
                }

                if (targetObservations.count(pomdp.getObservation(state)) > 0) {
                    smtSolver->add(pathVars[state][0]);
                } else {
                    smtSolver->add(!pathVars[state][0]);
                }

                if (surelyReachSinkStates.at(state)) {
                    smtSolver->add(!reachVars[state]);
                }
                else {
                    for (uint64_t action = 0; action < pomdp.getNumberOfChoices(state); ++action) {
                        std::vector<storm::expressions::Expression> subexprreach;

                        subexprreach.push_back(!reachVars.at(state));
                        subexprreach.push_back(!actionSelectionVars.at(pomdp.getObservation(state)).at(action));
                        for (auto const &entries : pomdp.getTransitionMatrix().getRow(rowindex)) {
                            subexprreach.push_back(reachVars.at(entries.getColumn()));
                        }
                        smtSolver->add(storm::expressions::disjunction(subexprreach));
                        for (auto const &entries : pomdp.getTransitionMatrix().getRow(rowindex)) {
                            for (uint64_t j = 1; j < k; ++j) {
                                pathsubsubexprs[j - 1].push_back(pathVars[entries.getColumn()][j - 1]);
                            }
                        }
                        rowindex++;
                    }
                    smtSolver->add(storm::expressions::implies(reachVars.at(state), pathVars.at(state).back()));


                    for (uint64_t j = 1; j < k; ++j) {
                        std::vector<storm::expressions::Expression> pathsubexprs;

                        for (uint64_t action = 0; action < pomdp.getNumberOfChoices(state); ++action) {
                            pathsubexprs.push_back(actionSelectionVars.at(pomdp.getObservation(state)).at(action) && storm::expressions::disjunction(pathsubsubexprs[j - 1]));
                        }
                        smtSolver->add(storm::expressions::iff(pathVars[state][j], storm::expressions::disjunction(pathsubexprs)));
                    }
                }

            }

            for (auto const& actionVars : actionSelectionVars) {
                smtSolver->add(storm::expressions::disjunction(actionVars));
            }




            //for (auto const& )

        }



        template class MemlessStrategySearchQualitative<double>;
    }
}
