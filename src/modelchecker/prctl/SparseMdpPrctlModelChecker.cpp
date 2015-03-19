#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"

#include <vector>
#include <memory>

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/solver/LpSolver.h"

#include "src/exceptions/InvalidPropertyException.h"
#include "src/storage/expressions/Expressions.h"

#include "src/storage/MaximalEndComponentDecomposition.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        SparseMdpPrctlModelChecker<ValueType>::SparseMdpPrctlModelChecker(storm::models::Mdp<ValueType> const& model) : SparsePropositionalModelChecker<ValueType>(model), nondeterministicLinearEquationSolver(storm::utility::solver::getNondeterministicLinearEquationSolver<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        SparseMdpPrctlModelChecker<ValueType>::SparseMdpPrctlModelChecker(storm::models::Mdp<ValueType> const& model, std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministicLinearEquationSolver) : SparsePropositionalModelChecker<ValueType>(model), nondeterministicLinearEquationSolver(nondeterministicLinearEquationSolver) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool SparseMdpPrctlModelChecker<ValueType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isPctlStateFormula() || formula.isPctlPathFormula() || formula.isRewardPathFormula();
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeBoundedUntilProbabilitiesHelper(bool minimize, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound) const {
            std::vector<ValueType> result(this->getModel().getNumberOfStates(), storm::utility::zero<ValueType>());
            
            // Determine the states that have 0 probability of reaching the target states.
            storm::storage::BitVector statesWithProbabilityGreater0;
            if (minimize) {
                statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0A(this->getModel().getTransitionMatrix(), this->getModel().getTransitionMatrix().getRowGroupIndices(), this->getModel().getBackwardTransitions(), phiStates, psiStates, true, stepBound);
            } else {
                statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0E(this->getModel().getTransitionMatrix(), this->getModel().getTransitionMatrix().getRowGroupIndices(), this->getModel().getBackwardTransitions(), phiStates, psiStates, true, stepBound);
            }
            STORM_LOG_INFO("Found " << statesWithProbabilityGreater0.getNumberOfSetBits() << " 'maybe' states.");
            
            if (!statesWithProbabilityGreater0.empty()) {
                // We can eliminate the rows and columns from the original transition probability matrix that have probability 0.
                storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(true, statesWithProbabilityGreater0, statesWithProbabilityGreater0, false);
                
                // Compute the new set of target states in the reduced system.
                storm::storage::BitVector rightStatesInReducedSystem = psiStates % statesWithProbabilityGreater0;
                
                // Make all rows absorbing that satisfy the second sub-formula.
                submatrix.makeRowGroupsAbsorbing(rightStatesInReducedSystem);
                
                // Create the vector with which to multiply.
                std::vector<ValueType> subresult(statesWithProbabilityGreater0.getNumberOfSetBits());
                storm::utility::vector::setVectorValues(subresult, rightStatesInReducedSystem, storm::utility::one<ValueType>());
            
                STORM_LOG_THROW(nondeterministicLinearEquationSolver != nullptr, storm::exceptions::InvalidStateException, "No valid equation solver available.");
                this->nondeterministicLinearEquationSolver->performMatrixVectorMultiplication(minimize, submatrix, subresult, nullptr, stepBound);
                
                // Set the values of the resulting vector accordingly.
                storm::utility::vector::setVectorValues(result, statesWithProbabilityGreater0, subresult);
                storm::utility::vector::setVectorValues(result, ~statesWithProbabilityGreater0, storm::utility::zero<ValueType>());
            }
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::unique_ptr<CheckResult> result = std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeBoundedUntilProbabilitiesHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getUpperBound())));
            return result;
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeNextProbabilitiesHelper(bool minimize, storm::storage::BitVector const& nextStates) {
            // Create the vector with which to multiply and initialize it correctly.
            std::vector<ValueType> result(this->getModel().getNumberOfStates());
            storm::utility::vector::setVectorValues(result, nextStates, storm::utility::one<ValueType>());
            
            STORM_LOG_THROW(nondeterministicLinearEquationSolver != nullptr, storm::exceptions::InvalidStateException, "No valid equation solver available.");
            this->nondeterministicLinearEquationSolver->performMatrixVectorMultiplication(minimize, this->getModel().getTransitionMatrix(), result);
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeNextProbabilitiesHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, subResult.getTruthValuesVector())));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeUntilProbabilitiesHelper(bool minimize, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative) const {
            return computeUntilProbabilitiesHelper(minimize, this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), phiStates, psiStates, nondeterministicLinearEquationSolver, qualitative);
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeUntilProbabilitiesHelper(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministicLinearEquationSolver, bool qualitative) {
            size_t numberOfStates = phiStates.size();
            
            // We need to identify the states which have to be taken out of the matrix, i.e.
            // all states that have probability 0 and 1 of satisfying the until-formula.
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01;
            if (minimize) {
                statesWithProbability01 = storm::utility::graph::performProb01Min(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, phiStates, psiStates);
            } else {
                statesWithProbability01 = storm::utility::graph::performProb01Max(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, phiStates, psiStates);
            }
            storm::storage::BitVector statesWithProbability0 = std::move(statesWithProbability01.first);
            storm::storage::BitVector statesWithProbability1 = std::move(statesWithProbability01.second);
            storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
            LOG4CPLUS_INFO(logger, "Found " << statesWithProbability0.getNumberOfSetBits() << " 'no' states.");
            LOG4CPLUS_INFO(logger, "Found " << statesWithProbability1.getNumberOfSetBits() << " 'yes' states.");
            LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
            
            // Create resulting vector.
            std::vector<ValueType> result(numberOfStates);
            
            // Check whether we need to compute exact probabilities for some states.
            if (qualitative) {
                // Set the values for all maybe-states to 0.5 to indicate that their probability values are neither 0 nor 1.
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, ValueType(0.5));
            } else {
                if (!maybeStates.empty()) {
                    // In this case we have have to compute the probabilities.

                    // First, we can eliminate the rows and columns from the original transition probability matrix for states
                    // whose probabilities are already known.
                    storm::storage::SparseMatrix<ValueType> submatrix = transitionMatrix.getSubmatrix(true, maybeStates, maybeStates, false);
                    
                    // Prepare the right-hand side of the equation system. For entry i this corresponds to
                    // the accumulated probability of going from state i to some 'yes' state.
                    std::vector<ValueType> b = transitionMatrix.getConstrainedRowGroupSumVector(maybeStates, statesWithProbability1);
                    
                    // Create vector for results for maybe states.
                    std::vector<ValueType> x(maybeStates.getNumberOfSetBits());
                    
                    // Solve the corresponding system of equations.
                    nondeterministicLinearEquationSolver->solveEquationSystem(minimize, submatrix, x, b);
                    
                    // Set values of resulting vector according to result.
                    storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
                }
            }
            
            // Set values of resulting vector that are known exactly.
            storm::utility::vector::setVectorValues<ValueType>(result, statesWithProbability0, storm::utility::zero<ValueType>());
            storm::utility::vector::setVectorValues<ValueType>(result, statesWithProbability1, storm::utility::one<ValueType>());
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(SparseMdpPrctlModelChecker<ValueType>::computeUntilProbabilitiesHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), nondeterministicLinearEquationSolver, qualitative)));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeCumulativeRewardsHelper(bool minimize, uint_fast64_t stepBound) const {
            // Only compute the result if the model has at least one reward this->getModel().
            STORM_LOG_THROW(this->getModel().hasStateRewards() || this->getModel().hasTransitionRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Compute the reward vector to add in each step based on the available reward models.
            std::vector<ValueType> totalRewardVector;
            if (this->getModel().hasTransitionRewards()) {
                totalRewardVector = this->getModel().getTransitionMatrix().getPointwiseProductRowSumVector(this->getModel().getTransitionRewardMatrix());
                if (this->getModel().hasStateRewards()) {
                    storm::utility::vector::addVectorsInPlace(totalRewardVector, this->getModel().getStateRewardVector());
                }
            } else {
                totalRewardVector = std::vector<ValueType>(this->getModel().getStateRewardVector());
            }
            
            // Initialize result to either the state rewards of the model or the null vector.
            std::vector<ValueType> result;
            if (this->getModel().hasStateRewards()) {
                result = std::vector<ValueType>(this->getModel().getStateRewardVector());
            } else {
                result.resize(this->getModel().getNumberOfStates());
            }
            
            this->nondeterministicLinearEquationSolver->performMatrixVectorMultiplication(minimize, this->getModel().getTransitionMatrix(), result, &totalRewardVector, stepBound);
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic.");
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeCumulativeRewardsHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, rewardPathFormula.getStepBound())));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeInstantaneousRewardsHelper(bool minimize, uint_fast64_t stepCount) const {
            // Only compute the result if the model has a state-based reward this->getModel().
            STORM_LOG_THROW(this->getModel().hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Initialize result to state rewards of the this->getModel().
            std::vector<ValueType> result(this->getModel().getStateRewardVector());
            
            STORM_LOG_THROW(nondeterministicLinearEquationSolver != nullptr, storm::exceptions::InvalidStateException, "No valid linear equation solver available.");
            this->nondeterministicLinearEquationSolver->performMatrixVectorMultiplication(minimize, this->getModel().getTransitionMatrix(), result, nullptr, stepCount);
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic.");
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeInstantaneousRewardsHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, rewardPathFormula.getStepCount())));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeReachabilityRewardsHelper(bool minimize, storm::storage::BitVector const& targetStates, bool qualitative) const {
            // Only compute the result if the model has at least one reward this->getModel().
            STORM_LOG_THROW(this->getModel().hasStateRewards() || this->getModel().hasTransitionRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Determine which states have a reward of infinity by definition.
            storm::storage::BitVector infinityStates;
            storm::storage::BitVector trueStates(this->getModel().getNumberOfStates(), true);
            if (minimize) {
                infinityStates = std::move(storm::utility::graph::performProb1A(this->getModel().getTransitionMatrix(), this->getModel().getTransitionMatrix().getRowGroupIndices(), this->getModel().getBackwardTransitions(), trueStates, targetStates));
            } else {
                infinityStates = std::move(storm::utility::graph::performProb1E(this->getModel().getTransitionMatrix(), this->getModel().getTransitionMatrix().getRowGroupIndices(), this->getModel().getBackwardTransitions(), trueStates, targetStates));
            }
            infinityStates.complement();
            storm::storage::BitVector maybeStates = ~targetStates & ~infinityStates;
            LOG4CPLUS_INFO(logger, "Found " << infinityStates.getNumberOfSetBits() << " 'infinity' states.");
            LOG4CPLUS_INFO(logger, "Found " << targetStates.getNumberOfSetBits() << " 'target' states.");
            LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
            
            // Create resulting vector.
            std::vector<ValueType> result(this->getModel().getNumberOfStates());
            
            // Check whether we need to compute exact rewards for some states.
            if (this->getModel().getInitialStates().isDisjointFrom(maybeStates)) {
                LOG4CPLUS_INFO(logger, "The rewards for the initial states were determined in a preprocessing step."
                               << " No exact rewards were computed.");
                // Set the values for all maybe-states to 1 to indicate that their reward values
                // are neither 0 nor infinity.
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, storm::utility::one<ValueType>());
            } else {
                // In this case we have to compute the reward values for the remaining states.
                
                // We can eliminate the rows and columns from the original transition probability matrix for states
                // whose reward values are already known.
                storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(true, maybeStates, maybeStates, false);
                
                // Prepare the right-hand side of the equation system. For entry i this corresponds to
                // the accumulated probability of going from state i to some 'yes' state.
                std::vector<ValueType> b(submatrix.getRowCount());
                
                if (this->getModel().hasTransitionRewards()) {
                    // If a transition-based reward model is available, we initialize the right-hand
                    // side to the vector resulting from summing the rows of the pointwise product
                    // of the transition probability matrix and the transition reward matrix.
                    std::vector<ValueType> pointwiseProductRowSumVector = this->getModel().getTransitionMatrix().getPointwiseProductRowSumVector(this->getModel().getTransitionRewardMatrix());
                    storm::utility::vector::selectVectorValues(b, maybeStates, this->getModel().getTransitionMatrix().getRowGroupIndices(), pointwiseProductRowSumVector);
                    
                    if (this->getModel().hasStateRewards()) {
                        // If a state-based reward model is also available, we need to add this vector
                        // as well. As the state reward vector contains entries not just for the states
                        // that we still consider (i.e. maybeStates), we need to extract these values
                        // first.
                        std::vector<ValueType> subStateRewards(b.size());
                        storm::utility::vector::selectVectorValuesRepeatedly(subStateRewards, maybeStates, this->getModel().getTransitionMatrix().getRowGroupIndices(), this->getModel().getStateRewardVector());
                        storm::utility::vector::addVectorsInPlace(b, subStateRewards);
                    }
                } else {
                    // If only a state-based reward model is  available, we take this vector as the
                    // right-hand side. As the state reward vector contains entries not just for the
                    // states that we still consider (i.e. maybeStates), we need to extract these values
                    // first.
                    storm::utility::vector::selectVectorValuesRepeatedly(b, maybeStates, this->getModel().getTransitionMatrix().getRowGroupIndices(), this->getModel().getStateRewardVector());
                }
                
                // Create vector for results for maybe states.
                std::vector<ValueType> x(maybeStates.getNumberOfSetBits());
                
                // Solve the corresponding system of equations.
                this->nondeterministicLinearEquationSolver->solveEquationSystem(minimize, submatrix, x, b);
                
                // Set values of resulting vector according to result.
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
            }
            
            // Set values of resulting vector that are known exactly.
            storm::utility::vector::setVectorValues(result, targetStates, storm::utility::zero<ValueType>());
            storm::utility::vector::setVectorValues(result, infinityStates, storm::utility::infinity<ValueType>());
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeReachabilityRewardsHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, subResult.getTruthValuesVector(), qualitative)));
        }
        

		template<typename ValueType>
		std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeLongRunAverageHelper(bool minimize, storm::storage::BitVector const& psiStates, bool qualitative) const {
			// If there are no goal states, we avoid the computation and directly return zero.
			auto numOfStates = this->getModel().getNumberOfStates();
			if (psiStates.empty()) {
				return std::vector<ValueType>(numOfStates, storm::utility::zero<ValueType>());
			}

			// Likewise, if all bits are set, we can avoid the computation and set.
			if ((~psiStates).empty()) {
				return std::vector<ValueType>(numOfStates, storm::utility::one<ValueType>());
			}

			// Start by decomposing the MDP into its MECs.
			storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition(this->getModelAs<storm::models::AbstractNondeterministicModel<ValueType>>());

			// Get some data members for convenience.
			typename storm::storage::SparseMatrix<ValueType> const& transitionMatrix = this->getModel().getTransitionMatrix();

			// Now start with compute the long-run average for all end components in isolation.
			std::vector<ValueType> lraValuesForEndComponents;

			// While doing so, we already gather some information for the following steps.
			std::vector<uint_fast64_t> stateToMecIndexMap(numOfStates);
			storm::storage::BitVector statesInMecs(numOfStates);

			for (uint_fast64_t currentMecIndex = 0; currentMecIndex < mecDecomposition.size(); ++currentMecIndex) {
				storm::storage::MaximalEndComponent const& mec = mecDecomposition[currentMecIndex];

				// Gather information for later use.
				for (auto const& stateChoicesPair : mec) {
					uint_fast64_t state = stateChoicesPair.first;

					statesInMecs.set(state);
					stateToMecIndexMap[state] = currentMecIndex;
				}

				// Compute the LRA value for the current MEC.
				lraValuesForEndComponents.push_back(this->computeLraForMaximalEndComponent(minimize, transitionMatrix, psiStates, mec));
			}

			// For fast transition rewriting, we build some auxiliary data structures.
			storm::storage::BitVector statesNotContainedInAnyMec = ~statesInMecs;
			
			// Prepare result vector.
			std::vector<ValueType> result(numOfStates);

			//Set the values for all states in MECs.
			for (auto state : statesInMecs) {
				result[state] = lraValuesForEndComponents[stateToMecIndexMap[state]];
			}

			//for all states not in any mec set the result to the minimal/maximal value of the reachable MECs
			//there might be a more efficient way to do this...
			for (auto state : statesNotContainedInAnyMec){

				//calculate what result values the reachable states in MECs have
				storm::storage::BitVector currentState(numOfStates);
				currentState.set(state);
				storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(
					transitionMatrix, currentState, storm::storage::BitVector(numOfStates, true), statesInMecs
					);

				storm::storage::BitVector reachableMecStates = statesInMecs & reachableStates;
				std::vector<ValueType> reachableResults(reachableMecStates.getNumberOfSetBits());
				storm::utility::vector::selectVectorValues(reachableResults, reachableMecStates, result);

				//now select the minimal/maximal element
				if (minimize){
					result[state] = *std::min_element(reachableResults.begin(), reachableResults.end());
				} else {
					result[state] = *std::max_element(reachableResults.begin(), reachableResults.end());
				}
			}

			return result;
		}

		template<typename ValueType>
		std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
			STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
			
			std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
			ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
			
			return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeLongRunAverageHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, subResult.getTruthValuesVector(), qualitative)));
		}

		template<typename ValueType>
		ValueType SparseMdpPrctlModelChecker<ValueType>::computeLraForMaximalEndComponent(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& psiStates, storm::storage::MaximalEndComponent const& mec) {
			std::shared_ptr<storm::solver::LpSolver> solver = storm::utility::solver::getLpSolver("LRA for MEC");
			solver->setModelSense(minimize ? storm::solver::LpSolver::ModelSense::Maximize : storm::solver::LpSolver::ModelSense::Minimize);

			//// First, we need to create the variables for the problem.
			std::map<uint_fast64_t, storm::expressions::Variable> stateToVariableMap;
			for (auto const& stateChoicesPair : mec) {
				std::string variableName = "h" + std::to_string(stateChoicesPair.first);
				stateToVariableMap[stateChoicesPair.first] = solver->addUnboundedContinuousVariable(variableName);
			}
			storm::expressions::Variable lambda = solver->addUnboundedContinuousVariable("L", 1);
			solver->update();

			// Now we encode the problem as constraints.
			for (auto const& stateChoicesPair : mec) {
				uint_fast64_t state = stateChoicesPair.first;

				// Now, based on the type of the state, create a suitable constraint.
				for (auto choice : stateChoicesPair.second) {
					storm::expressions::Expression constraint = solver->getConstant(1);
					ValueType w = 0;

					for (auto element : transitionMatrix.getRow(choice)) {
						constraint = constraint + stateToVariableMap.at(element.getColumn()) * solver->getConstant(element.getValue());
						if (psiStates.get(element.getColumn())) {
							w += element.getValue();
						}
					}
					constraint = constraint - solver->getConstant(w) * lambda;

					if (minimize) {
						constraint = stateToVariableMap.at(state) <= constraint;
					} else {
						constraint = stateToVariableMap.at(state) >= constraint;
					}
					solver->addConstraint("state" + std::to_string(state) + "," + std::to_string(choice), constraint);
				}
			}

			solver->optimize();
			return solver->getContinuousValue(lambda);
		}

        template<typename ValueType>
        storm::models::Mdp<ValueType> const& SparseMdpPrctlModelChecker<ValueType>::getModel() const {
            return this->template getModelAs<storm::models::Mdp<ValueType>>();
        }
                
        template class SparseMdpPrctlModelChecker<double>;
    }
}