#include "storm-pomdp/analysis/MemlessStrategySearchQualitative.h"


namespace storm {
    namespace pomdp {

        template <typename ValueType>
        void MemlessStrategySearchQualitative<ValueType>::initialize(uint64_t k) {
            if (maxK == std::numeric_limits<uint64_t>::max()) {
                // not initialized at all.
                // Create some data structures.
                for(uint64_t obs = 0; obs < pomdp.getNrObservations(); ++obs) {
                    actionSelectionVars.push_back(std::vector<storm::expressions::Variable>());
                    actionSelectionVarExpressions.push_back(std::vector<storm::expressions::Expression>());
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
                    reachVars.push_back(expressionManager->declareBooleanVariable("C-" + std::to_string(stateId)));
                    reachVarExpressions.push_back(reachVars.back().getExpression());
                    continuationVars.push_back(expressionManager->declareBooleanVariable("D-" + std::to_string(stateId)));
                    continuationVarExpressions.push_back(continuationVars.back().getExpression());
                    statesPerObservation.at(obs).push_back(stateId++);
                }
                assert(pathVars.size() == pomdp.getNumberOfStates());
                assert(reachVars.size() == pomdp.getNumberOfStates());
                assert(reachVarExpressions.size() == pomdp.getNumberOfStates());

                // Create the action selection variables.
                uint64_t obs = 0;
                for(auto const& statesForObservation : statesPerObservation) {
                    for (uint64_t a = 0; a < pomdp.getNumberOfChoices(statesForObservation.front()); ++a) {
                        std::string varName = "A-" + std::to_string(obs) + "-" + std::to_string(a);
                        actionSelectionVars.at(obs).push_back(expressionManager->declareBooleanVariable(varName));
                        actionSelectionVarExpressions.at(obs).push_back(actionSelectionVars.at(obs).back().getExpression());
                    }
                    schedulerVariables.push_back(expressionManager->declareBitVectorVariable("scheduler-obs-" + std::to_string(obs), statesPerObservation.size()));
                    schedulerVariableExpressions.push_back(schedulerVariables.back());
                    switchVars.push_back(expressionManager->declareBooleanVariable("S-" + std::to_string(obs)));
                    switchVarExpressions.push_back(switchVars.back().getExpression());

                    ++obs;
                }

                for (auto const& actionVars : actionSelectionVarExpressions) {
                    smtSolver->add(storm::expressions::disjunction(actionVars));
                }

                for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                    if (targetStates.get(state)) {
                        smtSolver->add(pathVars[state][0]);
                    } else {
                        smtSolver->add(!pathVars[state][0]);
                    }
                }

                uint64_t rowindex = 0;
                for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {

                    for (uint64_t action = 0; action < pomdp.getNumberOfChoices(state); ++action) {
                        std::vector<storm::expressions::Expression> subexprreachSwitch;
                        std::vector<storm::expressions::Expression> subexprreachNoSwitch;
                        subexprreachSwitch.push_back(!reachVarExpressions[state]);
                        subexprreachSwitch.push_back(!actionSelectionVarExpressions[pomdp.getObservation(state)][action]);
                        subexprreachSwitch.push_back(!switchVarExpressions[pomdp.getObservation(state)]);
                        subexprreachNoSwitch.push_back(!reachVarExpressions[state]);
                        subexprreachNoSwitch.push_back(!actionSelectionVarExpressions[pomdp.getObservation(state)][action]);
                        subexprreachNoSwitch.push_back(switchVarExpressions[pomdp.getObservation(state)]);
                        for (auto const &entries : pomdp.getTransitionMatrix().getRow(rowindex)) {
                            subexprreachSwitch.push_back(continuationVarExpressions.at(entries.getColumn()));
                            smtSolver->add(storm::expressions::disjunction(subexprreachSwitch));
                            subexprreachSwitch.pop_back();
                            subexprreachNoSwitch.push_back(reachVarExpressions.at(entries.getColumn()));
                            smtSolver->add(storm::expressions::disjunction(subexprreachNoSwitch));
                            subexprreachNoSwitch.pop_back();
                        }

                        rowindex++;
                    }
                }

                smtSolver->push();
            } else {
                smtSolver->pop();
                smtSolver->pop();
                smtSolver->push();
                assert(false);
            }

            uint64_t rowindex = 0;
            for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {

                if (surelyReachSinkStates.get(state)) {
                    smtSolver->add(!reachVarExpressions[state]);
                    for (uint64_t j = 1; j < k; ++j) {
                        smtSolver->add(!pathVars[state][j]);
                    }
                    smtSolver->add(!continuationVarExpressions[state]);
                } else if(!targetStates.get(state)) {
                    std::vector<std::vector<std::vector<storm::expressions::Expression>>> pathsubsubexprs;
                    for (uint64_t j = 1; j < k; ++j) {
                        pathsubsubexprs.push_back(std::vector<std::vector<storm::expressions::Expression>>());
                        for (uint64_t action = 0; action < pomdp.getNumberOfChoices(state); ++action) {
                            pathsubsubexprs.back().push_back(std::vector<storm::expressions::Expression>());
                        }
                    }

                    for (uint64_t action = 0; action < pomdp.getNumberOfChoices(state); ++action) {
                        std::vector<storm::expressions::Expression> subexprreach;

//                        subexprreach.push_back(!reachVarExpressions.at(state));
//                        subexprreach.push_back(!actionSelectionVarExpressions.at(pomdp.getObservation(state)).at(action));
//                        subexprreach.push_back(!switchVarExpressions[pomdp.getObservation(state)]);
//                        for (auto const &entries : pomdp.getTransitionMatrix().getRow(rowindex)) {
//                            subexprreach.push_back(reachVarExpressions.at(entries.getColumn()));
//                        }
//                        smtSolver->add(storm::expressions::disjunction(subexprreach));
                        for (auto const &entries : pomdp.getTransitionMatrix().getRow(rowindex)) {
                            for (uint64_t j = 1; j < k; ++j) {
                                pathsubsubexprs[j - 1][action].push_back(pathVars[entries.getColumn()][j - 1]);
                            }
                        }
                        rowindex++;
                    }
                    smtSolver->add(storm::expressions::implies(reachVarExpressions.at(state), pathVars.at(state).back()));

                    for (uint64_t j = 1; j < k; ++j) {
                        std::vector<storm::expressions::Expression> pathsubexprs;

                        for (uint64_t action = 0; action < pomdp.getNumberOfChoices(state); ++action) {
                            pathsubexprs.push_back(actionSelectionVarExpressions.at(pomdp.getObservation(state)).at(action) && storm::expressions::disjunction(pathsubsubexprs[j - 1][action]));
                        }
                        pathsubexprs.push_back(switchVarExpressions.at(pomdp.getObservation(state)));
                        smtSolver->add(storm::expressions::iff(pathVars[state][j], storm::expressions::disjunction(pathsubexprs)));
                    }
                }
            }

            uint64_t obs = 0;
            for(auto const& statesForObservation : statesPerObservation) {
                for(auto const& state : statesForObservation) {
                    smtSolver->add(!continuationVars[state] || schedulerVariableExpressions[obs] > 0);
                }
                ++obs;
            }

            // These constraints ensure that the right solver is used.
//            obs = 0;
//            for(auto const& statesForObservation : statesPerObservation) {
//                smtSolver->add(schedulerVariableExpressions[obs] >= schedulerForObs.size());
//                ++obs;
//            }

            // TODO updateFoundSchedulers();
        }

        template <typename ValueType>
        bool MemlessStrategySearchQualitative<ValueType>::analyze(uint64_t k, storm::storage::BitVector const& oneOfTheseStates, storm::storage::BitVector const& allOfTheseStates) {
            if (k < maxK) {
                initialize(k);
                maxK = k;
            }


            std::vector<storm::expressions::Expression> atLeastOneOfStates;

            for (uint64_t state : oneOfTheseStates) {
                STORM_LOG_ASSERT(reachVarExpressions.size() > state, "state id " << state << " exceeds number of states (" <<  reachVarExpressions.size() << ")" );
                atLeastOneOfStates.push_back(reachVarExpressions[state]);
            }
            assert(atLeastOneOfStates.size() > 0);
            smtSolver->add(storm::expressions::disjunction(atLeastOneOfStates));

            for (uint64_t state : allOfTheseStates) {
                assert(reachVarExpressions.size() > state);
                smtSolver->add(reachVarExpressions[state]);
            }

            smtSolver->push();
            uint64_t obs = 0;
            for(auto const& statesForObservation : statesPerObservation) {
                smtSolver->add(schedulerVariableExpressions[obs] <= schedulerForObs.size());
                ++obs;
            }
            for (uint64_t ob = 0; ob < pomdp.getNrObservations(); ++ob) {
                schedulerForObs.push_back(std::vector<uint64_t>());
            }


            InternalObservationScheduler scheduler;
            scheduler.switchObservations = storm::storage::BitVector(pomdp.getNrObservations());
            storm::storage::BitVector observations(pomdp.getNrObservations());
            storm::storage::BitVector observationsAfterSwitch(pomdp.getNrObservations());
            storm::storage::BitVector remainingstates(pomdp.getNumberOfStates());

            uint64_t iterations  = 0;
            while(true) {
                scheduler.clear();

                observations.clear();
                observationsAfterSwitch.clear();
                remainingstates.clear();

                while (true) {
                    ++iterations;
                    std::cout << "Call to SMT Solver (" <<iterations << ")" << std::endl;
                    std::cout << smtSolver->getSmtLibString() << std::endl;

                    auto result = smtSolver->check();
                    uint64_t i = 0;

                    if (result == storm::solver::SmtSolver::CheckResult::Unknown) {
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "SMT solver yielded an unexpected result");
                    } else if (result == storm::solver::SmtSolver::CheckResult::Unsat) {
                        std::cout << std::endl << "Unsatisfiable!" << std::endl;
                        break;
                    }

                    std::cout << std::endl << "Satisfying assignment: " << std::endl << smtSolver->getModelAsValuation().toString(true) << std::endl;
                    auto model = smtSolver->getModel();


                    observations.clear();
                    observationsAfterSwitch.clear();
                    remainingstates.clear();
                    scheduler.clear();

                    for (auto rv : reachVars) {
                        if (model->getBooleanValue(rv)) {
                            smtSolver->add(rv.getExpression());
                            observations.set(pomdp.getObservation(i));
                        } else {
                            remainingstates.set(i);
                        }
                        ++i;
                    }

                    i = 0;
                    std::cout << "states from which we continue" << std::endl;
                    for (auto rv : continuationVars) {
                        if (model->getBooleanValue(rv)) {
                            smtSolver->add(rv.getExpression());
                            observationsAfterSwitch.set(pomdp.getObservation(i));
                            std::cout << " " << i;
                        }
                        ++i;
                    }
                    std::cout << std::endl;

                    std::cout << "states that are okay" << std::endl;
                    for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                        if (!remainingstates.get(state)) {
                            std::cout << " " << state;
                        }
                    }

                    std::vector<storm::expressions::Expression> schedulerSoFar;
                    uint64_t obs = 0;
                    for (auto const &actionSelectionVarsForObs : actionSelectionVars) {
                        uint64_t act = 0;
                        scheduler.actions.push_back(std::set<uint64_t>());
                        if (observations.get(obs)) {
                            for (uint64_t act = 0; act < actionSelectionVarsForObs.size(); ++act) {
                                auto const& asv = actionSelectionVarsForObs[act];
                                if (model->getBooleanValue(asv)) {
                                    scheduler.actions.back().insert(act);
                                    schedulerSoFar.push_back(actionSelectionVarExpressions[obs][act]);
                                }
                            }
                            if (model->getBooleanValue(switchVars[obs])) {
                                scheduler.switchObservations.set(obs);
                                schedulerSoFar.push_back(switchVarExpressions[obs]);
                            } else {
                                schedulerSoFar.push_back(!switchVarExpressions[obs]);
                            }
                        }

                        if (observationsAfterSwitch.get(obs)) {
                            scheduler.schedulerRef.push_back(model->getIntegerValue(schedulerVariables[obs]));
                            schedulerSoFar.push_back(schedulerVariableExpressions[obs] == expressionManager->integer(scheduler.schedulerRef.back()));
                        } else {
                            scheduler.schedulerRef.push_back(0);
                        }
                        obs++;
                    }

                    std::cout << "the scheduler so far: " << std::endl;
                    scheduler.printForObservations(observations,observationsAfterSwitch);




                    std::vector<storm::expressions::Expression> remainingExpressions;
                    for (auto index : remainingstates) {
                        remainingExpressions.push_back(reachVarExpressions[index]);
                    }
                    // Add scheduler
                    smtSolver->add(storm::expressions::conjunction(schedulerSoFar));
                    smtSolver->add(storm::expressions::disjunction(remainingExpressions));

                }
                if (scheduler.empty()) {
                    break;
                }
                smtSolver->pop();
                std::cout << "states that are okay" << std::endl;
                for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                    if (!remainingstates.get(state)) {
                        std::cout << " " << state;
                    }
                }
                std::cout << std::endl;
                std::cout << "the scheduler: " << std::endl;
                scheduler.printForObservations(observations,observationsAfterSwitch);

                std::vector<storm::expressions::Expression> remainingExpressions;
                for (auto index : remainingstates) {
                    remainingExpressions.push_back(reachVarExpressions[index]);
                }

                smtSolver->add(storm::expressions::disjunction(remainingExpressions));

                uint64_t obs = 0;
                for (auto const &statesForObservation : statesPerObservation) {

                    if (observations.get(obs)) {
                        std::cout << "We have a new policy ( " << finalSchedulers.size() << " ) for states with observation " << obs << "." << std::endl;
                        assert(schedulerForObs.size() > obs);
                        schedulerForObs[obs].push_back(finalSchedulers.size());
                        std::cout << "We now have "  << schedulerForObs[obs].size() << " policies for states with observation " << obs << std::endl;

                        for (auto const &state : statesForObservation) {
                            if (remainingstates.get(state)) {
                                auto constant = expressionManager->integer(schedulerForObs[obs].size());
                                smtSolver->add(!(continuationVarExpressions[state] && (schedulerVariableExpressions[obs] == constant)));
                            }
                        }

                    }
                    ++obs;
                }
                finalSchedulers.push_back(scheduler);
                smtSolver->push();

                for (uint64_t obs = 0; obs < pomdp.getNrObservations(); ++obs) {
                    auto constant = expressionManager->integer(schedulerForObs[obs].size());
                    smtSolver->add(schedulerVariableExpressions[obs] <= constant);
                }

            }
            return true;
        }

        template<typename ValueType>
        void MemlessStrategySearchQualitative<ValueType>::printScheduler(std::vector<InternalObservationScheduler> const& ) {

        }


        template <typename ValueType>
        storm::expressions::Expression const& MemlessStrategySearchQualitative<ValueType>::getDoneActionExpression(uint64_t obs) const {
            return actionSelectionVarExpressions[obs].back();
        }


        template class MemlessStrategySearchQualitative<double>;
        template class MemlessStrategySearchQualitative<storm::RationalNumber>;
    }
}
