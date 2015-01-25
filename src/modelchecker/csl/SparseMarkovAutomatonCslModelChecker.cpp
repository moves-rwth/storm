#include "src/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"

#include <utility>
#include <vector>

#include "src/utility/ConstantsComparator.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include "src/modelchecker/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/ExplicitQuantitativeCheckResult.h"

#include "src/solver/LpSolver.h"

#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        SparseMarkovAutomatonCslModelChecker<ValueType>::SparseMarkovAutomatonCslModelChecker(storm::models::MarkovAutomaton<ValueType> const& model, std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministicLinearEquationSolver) : model(model), nondeterministicLinearEquationSolver(nondeterministicLinearEquationSolver) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        SparseMarkovAutomatonCslModelChecker<ValueType>::SparseMarkovAutomatonCslModelChecker(storm::models::MarkovAutomaton<ValueType> const& model) : model(model), nondeterministicLinearEquationSolver(storm::utility::solver::getNondeterministicLinearEquationSolver<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool SparseMarkovAutomatonCslModelChecker<ValueType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isCslStateFormula() || formula.isCslPathFormula() || (formula.isRewardPathFormula() && formula.isReachabilityRewardFormula());
        }

        template<typename ValueType>
        void SparseMarkovAutomatonCslModelChecker<ValueType>::computeBoundedReachabilityProbabilities(bool min, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint_fast64_t numberOfSteps) {
            // Start by computing four sparse matrices:
            // * a matrix aMarkovian with all (discretized) transitions from Markovian non-goal states to all Markovian non-goal states.
            // * a matrix aMarkovianToProbabilistic with all (discretized) transitions from Markovian non-goal states to all probabilistic non-goal states.
            // * a matrix aProbabilistic with all (non-discretized) transitions from probabilistic non-goal states to other probabilistic non-goal states.
            // * a matrix aProbabilisticToMarkovian with all (non-discretized) transitions from probabilistic non-goal states to all Markovian non-goal states.
            typename storm::storage::SparseMatrix<ValueType> aMarkovian = transitionMatrix.getSubmatrix(true, markovianNonGoalStates, markovianNonGoalStates, true);
            typename storm::storage::SparseMatrix<ValueType> aMarkovianToProbabilistic = transitionMatrix.getSubmatrix(true, markovianNonGoalStates, probabilisticNonGoalStates);
            typename storm::storage::SparseMatrix<ValueType> aProbabilistic = transitionMatrix.getSubmatrix(true, probabilisticNonGoalStates, probabilisticNonGoalStates);
            typename storm::storage::SparseMatrix<ValueType> aProbabilisticToMarkovian = transitionMatrix.getSubmatrix(true, probabilisticNonGoalStates, markovianNonGoalStates);
            
            // The matrices with transitions from Markovian states need to be digitized.
            // Digitize aMarkovian. Based on whether the transition is a self-loop or not, we apply the two digitization rules.
            uint_fast64_t rowIndex = 0;
            for (auto state : markovianNonGoalStates) {
                for (auto& element : aMarkovian.getRow(rowIndex)) {
                    ValueType eTerm = std::exp(-exitRates[state] * delta);
                    if (element.getColumn() == rowIndex) {
                        element.setValue((storm::utility::one<ValueType>() - eTerm) * element.getValue() + eTerm);
                    } else {
                        element.setValue((storm::utility::one<ValueType>() - eTerm) * element.getValue());
                    }
                }
                ++rowIndex;
            }
            
            // Digitize aMarkovianToProbabilistic. As there are no self-loops in this case, we only need to apply the digitization formula for regular successors.
            rowIndex = 0;
            for (auto state : markovianNonGoalStates) {
                for (auto& element : aMarkovianToProbabilistic.getRow(rowIndex)) {
                    element.setValue((1 - std::exp(-exitRates[state] * delta)) * element.getValue());
                }
                ++rowIndex;
            }
            
            // Initialize the two vectors that hold the variable one-step probabilities to all target states for probabilistic and Markovian (non-goal) states.
            std::vector<ValueType> bProbabilistic(aProbabilistic.getRowCount());
            std::vector<ValueType> bMarkovian(markovianNonGoalStates.getNumberOfSetBits());
            
            // Compute the two fixed right-hand side vectors, one for Markovian states and one for the probabilistic ones.
            std::vector<ValueType> bProbabilisticFixed = transitionMatrix.getConstrainedRowSumVector(probabilisticNonGoalStates, goalStates);
            std::vector<ValueType> bMarkovianFixed;
            bMarkovianFixed.reserve(markovianNonGoalStates.getNumberOfSetBits());
            for (auto state : markovianNonGoalStates) {
                bMarkovianFixed.push_back(storm::utility::zero<ValueType>());
                
                for (auto& element : transitionMatrix.getRowGroup(state)) {
                    if (goalStates.get(element.getColumn())) {
                        bMarkovianFixed.back() += (1 - std::exp(-exitRates[state] * delta)) * element.getValue();
                    }
                }
            }
            
            std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministiclinearEquationSolver = storm::utility::solver::getNondeterministicLinearEquationSolver<ValueType>();
            
            // Perform the actual value iteration
            // * loop until the step bound has been reached
            // * in the loop:
            // *    perform value iteration using A_PSwG, v_PS and the vector b where b = (A * 1_G)|PS + A_PStoMS * v_MS
            //      and 1_G being the characteristic vector for all goal states.
            // *    perform one timed-step using v_MS := A_MSwG * v_MS + A_MStoPS * v_PS + (A * 1_G)|MS
            std::vector<ValueType> markovianNonGoalValuesSwap(markovianNonGoalValues);
            std::vector<ValueType> multiplicationResultScratchMemory(aProbabilistic.getRowCount());
            std::vector<ValueType> aProbabilisticScratchMemory(probabilisticNonGoalValues.size());
            for (uint_fast64_t currentStep = 0; currentStep < numberOfSteps; ++currentStep) {
                // Start by (re-)computing bProbabilistic = bProbabilisticFixed + aProbabilisticToMarkovian * vMarkovian.
                aProbabilisticToMarkovian.multiplyWithVector(markovianNonGoalValues, bProbabilistic);
                storm::utility::vector::addVectorsInPlace(bProbabilistic, bProbabilisticFixed);
                
                // Now perform the inner value iteration for probabilistic states.
                nondeterministiclinearEquationSolver->solveEquationSystem(min, aProbabilistic, probabilisticNonGoalValues, bProbabilistic, &multiplicationResultScratchMemory, &aProbabilisticScratchMemory);
                
                // (Re-)compute bMarkovian = bMarkovianFixed + aMarkovianToProbabilistic * vProbabilistic.
                aMarkovianToProbabilistic.multiplyWithVector(probabilisticNonGoalValues, bMarkovian);
                storm::utility::vector::addVectorsInPlace(bMarkovian, bMarkovianFixed);
                aMarkovian.multiplyWithVector(markovianNonGoalValues, markovianNonGoalValuesSwap);
                std::swap(markovianNonGoalValues, markovianNonGoalValuesSwap);
                storm::utility::vector::addVectorsInPlace(markovianNonGoalValues, bMarkovian);
            }
            
            // After the loop, perform one more step of the value iteration for PS states.
            aProbabilisticToMarkovian.multiplyWithVector(markovianNonGoalValues, bProbabilistic);
            storm::utility::vector::addVectorsInPlace(bProbabilistic, bProbabilisticFixed);
            nondeterministiclinearEquationSolver->solveEquationSystem(min, aProbabilistic, probabilisticNonGoalValues, bProbabilistic, &multiplicationResultScratchMemory, &aProbabilisticScratchMemory);
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMarkovAutomatonCslModelChecker<ValueType>::computeBoundedUntilProbabilitiesHelper(bool minimize, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair) const {
            // 'Unpack' the bounds to make them more easily accessible.
            double lowerBound = boundsPair.first;
            double upperBound = boundsPair.second;
            
            // Get some data fields for convenient access.
            typename storm::storage::SparseMatrix<ValueType> const& transitionMatrix = model.getTransitionMatrix();
            std::vector<ValueType> const& exitRates = model.getExitRates();
            storm::storage::BitVector const& markovianStates = model.getMarkovianStates();
            
            // (1) Compute the accuracy we need to achieve the required error bound.
            ValueType maxExitRate = model.getMaximalExitRate();
            ValueType delta = (2 * storm::settings::generalSettings().getPrecision()) / (upperBound * maxExitRate * maxExitRate);
            
            // (2) Compute the number of steps we need to make for the interval.
            uint_fast64_t numberOfSteps = static_cast<uint_fast64_t>(std::ceil((upperBound - lowerBound) / delta));
            STORM_LOG_INFO("Performing " << numberOfSteps << " iterations (delta=" << delta << ") for interval [" << lowerBound << ", " << upperBound << "]." << std::endl);
            
            // (3) Compute the non-goal states and initialize two vectors
            // * vProbabilistic holds the probability values of probabilistic non-goal states.
            // * vMarkovian holds the probability values of Markovian non-goal states.
            storm::storage::BitVector const& markovianNonGoalStates = markovianStates & ~psiStates;
            storm::storage::BitVector const& probabilisticNonGoalStates = ~markovianStates & ~psiStates;
            std::vector<ValueType> vProbabilistic(probabilisticNonGoalStates.getNumberOfSetBits());
            std::vector<ValueType> vMarkovian(markovianNonGoalStates.getNumberOfSetBits());
            
            computeBoundedReachabilityProbabilities(minimize, transitionMatrix, exitRates, markovianStates, psiStates, markovianNonGoalStates, probabilisticNonGoalStates, vMarkovian, vProbabilistic, delta, numberOfSteps);
            
            // (4) If the lower bound of interval was non-zero, we need to take the current values as the starting values for a subsequent value iteration.
            if (lowerBound != storm::utility::zero<ValueType>()) {
                std::vector<ValueType> vAllProbabilistic((~markovianStates).getNumberOfSetBits());
                std::vector<ValueType> vAllMarkovian(markovianStates.getNumberOfSetBits());
                
                // Create the starting value vectors for the next value iteration based on the results of the previous one.
                storm::utility::vector::setVectorValues<ValueType>(vAllProbabilistic, psiStates % ~markovianStates, storm::utility::one<ValueType>());
                storm::utility::vector::setVectorValues<ValueType>(vAllProbabilistic, ~psiStates % ~markovianStates, vProbabilistic);
                storm::utility::vector::setVectorValues<ValueType>(vAllMarkovian, psiStates % markovianStates, storm::utility::one<ValueType>());
                storm::utility::vector::setVectorValues<ValueType>(vAllMarkovian, ~psiStates % markovianStates, vMarkovian);
                
                // Compute the number of steps to reach the target interval.
                numberOfSteps = static_cast<uint_fast64_t>(std::ceil(lowerBound / delta));
                STORM_LOG_INFO("Performing " << numberOfSteps << " iterations (delta=" << delta << ") for interval [0, " << lowerBound << "]." << std::endl);
                
                // Compute the bounded reachability for interval [0, b-a].
                computeBoundedReachabilityProbabilities(minimize, transitionMatrix, exitRates, markovianStates, storm::storage::BitVector(model.getNumberOfStates()), markovianStates, ~markovianStates, vAllMarkovian, vAllProbabilistic, delta, numberOfSteps);
                
                // Create the result vector out of vAllProbabilistic and vAllMarkovian and return it.
                std::vector<ValueType> result(model.getNumberOfStates());
                storm::utility::vector::setVectorValues(result, ~markovianStates, vAllProbabilistic);
                storm::utility::vector::setVectorValues(result, markovianStates, vAllMarkovian);
                
                return result;
            } else {
                // Create the result vector out of 1_G, vProbabilistic and vMarkovian and return it.
                std::vector<ValueType> result(model.getNumberOfStates());
                storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::one<ValueType>());
                storm::utility::vector::setVectorValues(result, probabilisticNonGoalStates, vProbabilistic);
                storm::utility::vector::setVectorValues(result, markovianNonGoalStates, vMarkovian);
                return result;
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMarkovAutomatonCslModelChecker<ValueType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(pathFormula.getLeftSubformula().isTrueFormula(), storm::exceptions::NotImplementedException, "Only bounded properties of the form 'true U[t1, t2] phi' are currently supported.");
            STORM_LOG_THROW(model.isClosed(), storm::exceptions::InvalidArgumentException, "Unable to compute time-bounded reachability probilities in non-closed Markov automaton.");
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult& rightResult = dynamic_cast<ExplicitQualitativeCheckResult&>(*rightResultPointer);
            std::unique_ptr<CheckResult> result = std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeBoundedUntilProbabilitiesHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, rightResult.getTruthValues(), pathFormula.getIntervalBounds())));
            return result;
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMarkovAutomatonCslModelChecker<ValueType>::computeUntilProbabilitiesHelper(bool minimize, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative) const {
            return storm::modelchecker::SparseMdpPrctlModelChecker<ValueType>::computeUntilProbabilitiesHelper(minimize, model.getTransitionMatrix(), model.getBackwardTransitions(), phiStates, psiStates, nondeterministicLinearEquationSolver, qualitative);
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMarkovAutomatonCslModelChecker<ValueType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult& leftResult = dynamic_cast<ExplicitQualitativeCheckResult&>(*leftResultPointer);
            ExplicitQualitativeCheckResult& rightResult = dynamic_cast<ExplicitQualitativeCheckResult&>(*rightResultPointer);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeUntilProbabilitiesHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, leftResult.getTruthValues(), rightResult.getTruthValues(), qualitative)));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMarkovAutomatonCslModelChecker<ValueType>::computeReachabilityRewardsHelper(bool minimize, storm::storage::BitVector const& targetStates, bool qualitative) const {

        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMarkovAutomatonCslModelChecker<ValueType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            ExplicitQualitativeCheckResult& subResult = dynamic_cast<ExplicitQualitativeCheckResult&>(*subResultPointer);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeReachabilityRewardsHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, subResult.getTruthValues(), qualitative)));
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMarkovAutomatonCslModelChecker<ValueType>::checkBooleanLiteralFormula(storm::logic::BooleanLiteralFormula const& stateFormula) {
            if (stateFormula.isTrueFormula()) {
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(storm::storage::BitVector(model.getNumberOfStates(), true)));
            } else {
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(storm::storage::BitVector(model.getNumberOfStates())));
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMarkovAutomatonCslModelChecker<ValueType>::checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula) {
            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(model.getLabeledStates(stateFormula.getLabel())));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMarkovAutomatonCslModelChecker<ValueType>::checkLongRunAverage(bool min, storm::storage::BitVector const& goalStates) const {
            // Check whether the automaton is closed.
            if (!model.isClosed()) {
                throw storm::exceptions::InvalidArgumentException() << "Unable to compute long-run average on non-closed Markov automaton.";
            }
            
            // If there are no goal states, we avoid the computation and directly return zero.
            if (goalStates.empty()) {
                return std::vector<ValueType>(model.getNumberOfStates(), storm::utility::zero<ValueType>());
            }
            
            // Likewise, if all bits are set, we can avoid the computation and set.
            if ((~goalStates).empty()) {
                return std::vector<ValueType>(model.getNumberOfStates(), storm::utility::one<ValueType>());
            }
            
            // Start by decomposing the Markov automaton into its MECs.
            storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition(model);
            
            // Get some data members for convenience.
            typename storm::storage::SparseMatrix<ValueType> const& transitionMatrix = model.getTransitionMatrix();
            std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();
            
            // Now start with compute the long-run average for all end components in isolation.
            std::vector<ValueType> lraValuesForEndComponents;
            
            // While doing so, we already gather some information for the following steps.
            std::vector<uint_fast64_t> stateToMecIndexMap(model.getNumberOfStates());
            storm::storage::BitVector statesInMecs(model.getNumberOfStates());
            
            for (uint_fast64_t currentMecIndex = 0; currentMecIndex < mecDecomposition.size(); ++currentMecIndex) {
                storm::storage::MaximalEndComponent const& mec = mecDecomposition[currentMecIndex];
                
                // Gather information for later use.
                for (auto const& stateChoicesPair : mec) {
                    uint_fast64_t state = stateChoicesPair.first;
                    
                    statesInMecs.set(state);
                    stateToMecIndexMap[state] = currentMecIndex;
                }
                
                // Compute the LRA value for the current MEC.
                lraValuesForEndComponents.push_back(this->computeLraForMaximalEndComponent(min, transitionMatrix, nondeterministicChoiceIndices, model.getMarkovianStates(), model.getExitRates(), goalStates, mec, currentMecIndex));
            }
            
            // For fast transition rewriting, we build some auxiliary data structures.
            storm::storage::BitVector statesNotContainedInAnyMec = ~statesInMecs;
            uint_fast64_t firstAuxiliaryStateIndex = statesNotContainedInAnyMec.getNumberOfSetBits();
            uint_fast64_t lastStateNotInMecs = 0;
            uint_fast64_t numberOfStatesNotInMecs = 0;
            std::vector<uint_fast64_t> statesNotInMecsBeforeIndex;
            statesNotInMecsBeforeIndex.reserve(model.getNumberOfStates());
            for (auto state : statesNotContainedInAnyMec) {
                while (lastStateNotInMecs <= state) {
                    statesNotInMecsBeforeIndex.push_back(numberOfStatesNotInMecs);
                    ++lastStateNotInMecs;
                }
                ++numberOfStatesNotInMecs;
            }
            
            // Finally, we are ready to create the SSP matrix and right-hand side of the SSP.
            std::vector<ValueType> b;
            typename storm::storage::SparseMatrixBuilder<ValueType> sspMatrixBuilder(0, 0, 0, false, true, numberOfStatesNotInMecs + mecDecomposition.size());
            
            // If the source state is not contained in any MEC, we copy its choices (and perform the necessary modifications).
            uint_fast64_t currentChoice = 0;
            for (auto state : statesNotContainedInAnyMec) {
                sspMatrixBuilder.newRowGroup(currentChoice);
                
                for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice, ++currentChoice) {
                    std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                    b.push_back(storm::utility::zero<ValueType>());
                    
                    for (auto element : transitionMatrix.getRow(choice)) {
                        if (statesNotContainedInAnyMec.get(element.getColumn())) {
                            // If the target state is not contained in an MEC, we can copy over the entry.
                            sspMatrixBuilder.addNextValue(currentChoice, statesNotInMecsBeforeIndex[element.getColumn()], element.getValue());
                        } else {
                            // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                            // so that we are able to write the cumulative probability to the MEC into the matrix.
                            auxiliaryStateToProbabilityMap[stateToMecIndexMap[element.getColumn()]] += element.getValue();
                        }
                    }
                    
                    // Now insert all (cumulative) probability values that target an MEC.
                    for (uint_fast64_t mecIndex = 0; mecIndex < auxiliaryStateToProbabilityMap.size(); ++mecIndex) {
                        if (auxiliaryStateToProbabilityMap[mecIndex] != 0) {
                            sspMatrixBuilder.addNextValue(currentChoice, firstAuxiliaryStateIndex + mecIndex, auxiliaryStateToProbabilityMap[mecIndex]);
                        }
                    }
                }
            }
            
            // Now we are ready to construct the choices for the auxiliary states.
            for (uint_fast64_t mecIndex = 0; mecIndex < mecDecomposition.size(); ++mecIndex) {
                storm::storage::MaximalEndComponent const& mec = mecDecomposition[mecIndex];
                sspMatrixBuilder.newRowGroup(currentChoice);
                
                for (auto const& stateChoicesPair : mec) {
                    uint_fast64_t state = stateChoicesPair.first;
                    boost::container::flat_set<uint_fast64_t> const& choicesInMec = stateChoicesPair.second;
                    
                    for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                        std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                        
                        // If the choice is not contained in the MEC itself, we have to add a similar distribution to the auxiliary state.
                        if (choicesInMec.find(choice) == choicesInMec.end()) {
                            b.push_back(storm::utility::zero<ValueType>());
                            
                            for (auto element : transitionMatrix.getRow(choice)) {
                                if (statesNotContainedInAnyMec.get(element.getColumn())) {
                                    // If the target state is not contained in an MEC, we can copy over the entry.
                                    sspMatrixBuilder.addNextValue(currentChoice, statesNotInMecsBeforeIndex[element.getColumn()], element.getValue());
                                } else {
                                    // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                                    // so that we are able to write the cumulative probability to the MEC into the matrix.
                                    auxiliaryStateToProbabilityMap[stateToMecIndexMap[element.getColumn()]] += element.getValue();
                                }
                            }
                            
                            // Now insert all (cumulative) probability values that target an MEC.
                            for (uint_fast64_t targetMecIndex = 0; targetMecIndex < auxiliaryStateToProbabilityMap.size(); ++targetMecIndex) {
                                if (auxiliaryStateToProbabilityMap[targetMecIndex] != 0) {
                                    // If the target MEC is the same as the current one, instead of adding a transition, we need to add the weighted reward
                                    // to the right-hand side vector of the SSP.
                                    if (mecIndex == targetMecIndex) {
                                        b.back() += auxiliaryStateToProbabilityMap[mecIndex] * lraValuesForEndComponents[mecIndex];
                                    } else {
                                        // Otherwise, we add a transition to the auxiliary state that is associated with the target MEC.
                                        sspMatrixBuilder.addNextValue(currentChoice, firstAuxiliaryStateIndex + targetMecIndex, auxiliaryStateToProbabilityMap[targetMecIndex]);
                                    }
                                }
                            }
                            
                            ++currentChoice;
                        }
                    }
                }
                
                // For each auxiliary state, there is the option to achieve the reward value of the LRA associated with the MEC.
                ++currentChoice;
                b.push_back(lraValuesForEndComponents[mecIndex]);
            }
            
            // Finalize the matrix and solve the corresponding system of equations.
            storm::storage::SparseMatrix<ValueType> sspMatrix = sspMatrixBuilder.build(currentChoice);
            
            std::vector<ValueType> x(numberOfStatesNotInMecs + mecDecomposition.size());
            nondeterministicLinearEquationSolver->solveEquationSystem(min, sspMatrix, x, b);
            
            // Prepare result vector.
            std::vector<ValueType> result(model.getNumberOfStates());
            
            // Set the values for states not contained in MECs.
            storm::utility::vector::setVectorValues(result, statesNotContainedInAnyMec, x);
            
            // Set the values for all states in MECs.
            for (auto state : statesInMecs) {
                result[state] = lraValuesForEndComponents[stateToMecIndexMap[state]];
            }
            
            return result;
        }
        
        template<typename ValueType>
        ValueType SparseMarkovAutomatonCslModelChecker<ValueType>::computeLraForMaximalEndComponent(bool min, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, storm::storage::BitVector const& markovianStates, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::MaximalEndComponent const& mec, uint_fast64_t mecIndex) {
            std::shared_ptr<storm::solver::LpSolver> solver = storm::utility::solver::getLpSolver("LRA for MEC");
            solver->setModelSense(min ? storm::solver::LpSolver::ModelSense::Maximize : storm::solver::LpSolver::ModelSense::Minimize);
            
            // First, we need to create the variables for the problem.
            std::map<uint_fast64_t, storm::expressions::Variable> stateToVariableMap;
            for (auto const& stateChoicesPair : mec) {
                std::string variableName = "x" + std::to_string(stateChoicesPair.first);
                stateToVariableMap[stateChoicesPair.first] = solver->addUnboundedContinuousVariable(variableName);
            }
            storm::expressions::Variable k = solver->addUnboundedContinuousVariable("k", 1);
            solver->update();
            
            // Now we encode the problem as constraints.
            for (auto const& stateChoicesPair : mec) {
                uint_fast64_t state = stateChoicesPair.first;
                
                // Now, based on the type of the state, create a suitable constraint.
                if (markovianStates.get(state)) {
                    storm::expressions::Expression constraint = stateToVariableMap.at(state);
                    
                    for (auto element : transitionMatrix.getRow(nondeterministicChoiceIndices[state])) {
                        constraint = constraint - stateToVariableMap.at(element.getColumn()) * solver->getConstant(element.getValue());
                    }
                    
                    constraint = constraint + solver->getConstant(storm::utility::one<ValueType>() / exitRates[state]) * k;
                    storm::expressions::Expression rightHandSide = goalStates.get(state) ? solver->getConstant(storm::utility::one<ValueType>() / exitRates[state]) : solver->getConstant(0);
                    if (min) {
                        constraint = constraint <= rightHandSide;
                    } else {
                        constraint = constraint >= rightHandSide;
                    }
                    solver->addConstraint("state" + std::to_string(state), constraint);
                } else {
                    // For probabilistic states, we want to add the constraint x_s <= sum P(s, a, s') * x_s' where a is the current action
                    // and the sum ranges over all states s'.
                    for (auto choice : stateChoicesPair.second) {
                        storm::expressions::Expression constraint = stateToVariableMap.at(state);
                        
                        for (auto element : transitionMatrix.getRow(choice)) {
                            constraint = constraint - stateToVariableMap.at(element.getColumn()) * solver->getConstant(element.getValue());
                        }
                        
                        storm::expressions::Expression rightHandSide = solver->getConstant(storm::utility::zero<ValueType>());
                        if (min) {
                            constraint = constraint <= rightHandSide;
                        } else {
                            constraint = constraint >= rightHandSide;
                        }
                        solver->addConstraint("state" + std::to_string(state), constraint);
                    }
                }
            }
            
            solver->optimize();
            return solver->getContinuousValue(k);
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMarkovAutomatonCslModelChecker<ValueType>::checkExpectedTime(bool minimize, storm::storage::BitVector const& goalStates) const {
            // Reduce the problem of computing the expected time to computing expected rewards where the rewards
            // for all probabilistic states are zero and the reward values of Markovian states is 1.
            std::vector<ValueType> rewardValues(model.getNumberOfStates(), storm::utility::zero<ValueType>());
            storm::utility::vector::setVectorValues(rewardValues, model.getMarkovianStates(), storm::utility::one<ValueType>());
            return this->computeExpectedRewards(minimize, goalStates, rewardValues);
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMarkovAutomatonCslModelChecker<ValueType>::computeExpectedRewards(bool minimize, storm::storage::BitVector const& goalStates, std::vector<ValueType> const& stateRewards) const {
            // Check whether the automaton is closed.
            if (!model.isClosed()) {
                throw storm::exceptions::InvalidArgumentException() << "Unable to compute expected time on non-closed Markov automaton.";
            }
            
            // First, we need to check which states have infinite expected time (by definition).
            storm::storage::BitVector infinityStates;
            if (minimize) {
                // If we need to compute the minimum expected times, we have to set the values of those states to infinity that, under all schedulers,
                // reach a bottom SCC without a goal state.
                
                // So we start by computing all bottom SCCs without goal states.
                storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition(model, ~goalStates, true, true);
                
                // Now form the union of all these SCCs.
                storm::storage::BitVector unionOfNonGoalBSccs(model.getNumberOfStates());
                for (auto const& scc : sccDecomposition) {
                    for (auto state : scc) {
                        unionOfNonGoalBSccs.set(state);
                    }
                }
                
                // Finally, if this union is non-empty, compute the states such that all schedulers reach some state of the union.
                if (!unionOfNonGoalBSccs.empty()) {
                    infinityStates = storm::utility::graph::performProbGreater0A(model.getTransitionMatrix(), model.getNondeterministicChoiceIndices(), model.getBackwardTransitions(), storm::storage::BitVector(model.getNumberOfStates(), true), unionOfNonGoalBSccs);
                } else {
                    // Otherwise, we have no infinity states.
                    infinityStates = storm::storage::BitVector(model.getNumberOfStates());
                }
            } else {
                // If we maximize the property, the expected time of a state is infinite, if an end-component without any goal state is reachable.
                
                // So we start by computing all MECs that have no goal state.
                storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition(model, ~goalStates);
                
                // Now we form the union of all states in these end components.
                storm::storage::BitVector unionOfNonGoalMaximalEndComponents(model.getNumberOfStates());
                for (auto const& mec : mecDecomposition) {
                    for (auto const& stateActionPair : mec) {
                        unionOfNonGoalMaximalEndComponents.set(stateActionPair.first);
                    }
                }
                
                if (!unionOfNonGoalMaximalEndComponents.empty()) {
                    // Now we need to check for which states there exists a scheduler that reaches one of the previously computed states.
                    infinityStates = storm::utility::graph::performProbGreater0E(model.getTransitionMatrix(), model.getNondeterministicChoiceIndices(), model.getBackwardTransitions(), storm::storage::BitVector(model.getNumberOfStates(), true), unionOfNonGoalMaximalEndComponents);
                } else {
                    // Otherwise, we have no infinity states.
                    infinityStates = storm::storage::BitVector(model.getNumberOfStates());
                }
            }
            
            // Now we identify the states for which values need to be computed.
            storm::storage::BitVector maybeStates = ~(goalStates | infinityStates);
            
            // Then, we can eliminate the rows and columns for all states whose values are already known to be 0.
            std::vector<ValueType> x(maybeStates.getNumberOfSetBits());
            storm::storage::SparseMatrix<ValueType> submatrix = model.getTransitionMatrix().getSubmatrix(true, maybeStates, maybeStates);
            
            // Now prepare the expected reward values for all states so they can be used as the right-hand side of the equation system.
            std::vector<ValueType> rewardValues(stateRewards);
            for (auto state : model.getMarkovianStates()) {
                rewardValues[state] = rewardValues[state] / model.getExitRates()[state];
            }
            
            // Finally, prepare the actual right-hand side.
            std::vector<ValueType> b(submatrix.getRowCount());
            storm::utility::vector::selectVectorValuesRepeatedly(b, maybeStates, model.getNondeterministicChoiceIndices(), rewardValues);
            
            // Solve the corresponding system of equations.
            std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministiclinearEquationSolver = storm::utility::solver::getNondeterministicLinearEquationSolver<ValueType>();
            nondeterministiclinearEquationSolver->solveEquationSystem(minimize, submatrix, x, b);
            
            // Create resulting vector.
            std::vector<ValueType> result(model.getNumberOfStates());
            
            // Set values of resulting vector according to previous result and return the result.
            storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
            storm::utility::vector::setVectorValues(result, goalStates, storm::utility::zero<ValueType>());
            storm::utility::vector::setVectorValues(result, infinityStates, storm::utility::infinity<ValueType>());
            
            return result;
        }
        
        template class SparseMarkovAutomatonCslModelChecker<double>;
    }
}