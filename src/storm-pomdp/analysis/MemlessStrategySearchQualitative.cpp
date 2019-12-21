#include "storm-pomdp/analysis/MemlessStrategySearchQualitative.h"
#include "storm/utility/file.h"


namespace storm {
    namespace pomdp {

        namespace detail {
            void printRelevantInfoFromModel(std::shared_ptr<storm::solver::SmtSolver::ModelReference> const& model, std::vector<storm::expressions::Variable> const& reachVars, std::vector<storm::expressions::Variable> const& continuationVars) {
                uint64_t i = 0;
                std::stringstream ss;
                STORM_LOG_TRACE("states which we have now: ");
                for (auto rv : reachVars) {
                    if (model->getBooleanValue(rv)) {
                        ss << " " << i;
                    }
                    ++i;
                }
                STORM_LOG_TRACE(ss.str());
                i = 0;
                STORM_LOG_TRACE("states from which we continue: ");
                ss.clear();
                for (auto rv : continuationVars) {
                    if (model->getBooleanValue(rv)) {
                        ss << " " << i;
                    }
                    ++i;
                }
                STORM_LOG_TRACE(ss.str());

            }
        }

        template <typename ValueType>
        MemlessStrategySearchQualitative<ValueType>::MemlessStrategySearchQualitative(storm::models::sparse::Pomdp<ValueType> const& pomdp,
                                                                                      std::set<uint32_t> const& targetObservationSet,
                    storm::storage::BitVector const& targetStates,
                    storm::storage::BitVector const& surelyReachSinkStates,
                    std::shared_ptr<storm::utility::solver::SmtSolverFactory>& smtSolverFactory,
                    MemlessSearchOptions const& options) :
            pomdp(pomdp),
            targetStates(targetStates),
            surelyReachSinkStates(surelyReachSinkStates),
            targetObservations(targetObservationSet),
            options(options)
        {
            this->expressionManager = std::make_shared<storm::expressions::ExpressionManager>();
            smtSolver = smtSolverFactory->create(*expressionManager);
            // Initialize states per observation.
            for (uint64_t obs = 0; obs < pomdp.getNrObservations(); ++obs) {
                statesPerObservation.push_back(std::vector<uint64_t>()); // Consider using bitvectors instead.
            }
            uint64_t state = 0;
            for (auto obs : pomdp.getObservations()) {
                statesPerObservation.at(obs).push_back(state++);
            }
            // Initialize winning region
            std::vector<uint64_t> nrStatesPerObservation;
            for (auto const &states : statesPerObservation) {
                nrStatesPerObservation.push_back(states.size());
            }
            winningRegion = WinningRegion(nrStatesPerObservation);
        }

        template <typename ValueType>
        void MemlessStrategySearchQualitative<ValueType>::initialize(uint64_t k) {
            if (maxK == std::numeric_limits<uint64_t>::max()) {
                // not initialized at all.
                // Create some data structures.
                for(uint64_t obs = 0; obs < pomdp.getNrObservations(); ++obs) {
                    actionSelectionVars.push_back(std::vector<storm::expressions::Variable>());
                    actionSelectionVarExpressions.push_back(std::vector<storm::expressions::Expression>());
                }

                // Fill the states-per-observation mapping,
                // declare the reachability variables,
                // declare the path variables.
                for(uint64_t stateId = 0; stateId < pomdp.getNumberOfStates(); ++stateId) {
                    pathVars.push_back(std::vector<storm::expressions::Expression>());
                    for (uint64_t i = 0; i < k; ++i) {
                        pathVars.back().push_back(expressionManager->declareBooleanVariable("P-"+std::to_string(stateId)+"-"+std::to_string(i)).getExpression());
                    }
                    reachVars.push_back(expressionManager->declareBooleanVariable("C-" + std::to_string(stateId)));
                    reachVarExpressions.push_back(reachVars.back().getExpression());
                    continuationVars.push_back(expressionManager->declareBooleanVariable("D-" + std::to_string(stateId)));
                    continuationVarExpressions.push_back(continuationVars.back().getExpression());
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

            // TODO: Update found schedulers if k is increased.
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

                    if(options.isExportSATSet()) {
                        STORM_LOG_DEBUG("Export SMT Solver Call (" <<iterations << ")");
                        std::string filepath = options.getExportSATCallsPath() + "call_" + std::to_string(iterations) + ".smt2";
                        std::ofstream filestream;
                        storm::utility::openFile(filepath, filestream);
                        filestream << smtSolver->getSmtLibString() << std::endl;
                        storm::utility::closeFile(filestream);
                    }

                    STORM_LOG_DEBUG("Call to SMT Solver (" <<iterations << ")");
                    auto result = smtSolver->check();
                    uint64_t i = 0;

                    if (result == storm::solver::SmtSolver::CheckResult::Unknown) {
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "SMT solver yielded an unexpected result");
                    } else if (result == storm::solver::SmtSolver::CheckResult::Unsat) {
                        STORM_LOG_DEBUG("Unsatisfiable!");
                        break;
                    }

                    STORM_LOG_DEBUG("Satisfying assignment: ");
                    STORM_LOG_TRACE(smtSolver->getModelAsValuation().toString(true));
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
                    for (auto rv : continuationVars) {
                        if (model->getBooleanValue(rv)) {
                            smtSolver->add(rv.getExpression());
                            observationsAfterSwitch.set(pomdp.getObservation(i));
                        }
                        ++i;
                    }

                    if (options.computeTraceOutput()) {
                        detail::printRelevantInfoFromModel(model, reachVars, continuationVars);
                    }

                    // TODO do not repush everyting to the solver.
                    std::vector<storm::expressions::Expression> schedulerSoFar;
                    uint64_t obs = 0;
                    for (auto const &actionSelectionVarsForObs : actionSelectionVars) {
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

                    if(options.computeTraceOutput()) {
                        // generates debug output, but here we only want it for trace level.
                        // For consistency, all output on debug level.
                        STORM_LOG_DEBUG("the scheduler so far: ");
                        scheduler.printForObservations(observations,observationsAfterSwitch);
                    }

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

                if(options.computeDebugOutput()) {
                    printCoveredStates(remainingstates);
                    // generates info output, but here we only want it for debug level.
                    // For consistency, all output on info level.
                    STORM_LOG_DEBUG("the scheduler: ");
                    scheduler.printForObservations(observations,observationsAfterSwitch);
                }

                std::vector<storm::expressions::Expression> remainingExpressions;
                for (auto index : remainingstates) {
                    remainingExpressions.push_back(reachVarExpressions[index]);
                }

                for (uint64_t observation = 0; observation < pomdp.getNrObservations(); ++observation) {
                    storm::storage::BitVector update = storm::storage::BitVector(statesPerObservation[observation].size());
                    uint64_t i = 0;
                    for (uint64_t state : statesPerObservation[observation]) {
                        if (!remainingstates.get(state)) {
                            update.set(i);
                        }
                    }
                    winningRegion.update(observation, update);
                    ++i;
                }

                smtSolver->add(storm::expressions::disjunction(remainingExpressions));

                uint64_t obs = 0;
                for (auto const &statesForObservation : statesPerObservation) {
                    if (observations.get(obs)) {
                        STORM_LOG_DEBUG("We have a new policy ( " << finalSchedulers.size() << " ) for states with observation " << obs << ".");
                        assert(schedulerForObs.size() > obs);
                        schedulerForObs[obs].push_back(finalSchedulers.size());
                        STORM_LOG_DEBUG("We now have "  << schedulerForObs[obs].size() << " policies for states with observation " << obs);

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
        void MemlessStrategySearchQualitative<ValueType>::printCoveredStates(storm::storage::BitVector const &remaining) const {

            STORM_LOG_DEBUG("states that are okay");
            for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                if (!remaining.get(state)) {
                    std::cout << " " << state;
                }
            }
            std::cout << std::endl;

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
