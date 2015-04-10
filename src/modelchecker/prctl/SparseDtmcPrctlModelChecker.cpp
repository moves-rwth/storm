#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"

#include <vector>
#include <memory>

#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/storage/StronglyConnectedComponentDecomposition.h"

#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        SparseDtmcPrctlModelChecker<ValueType>::SparseDtmcPrctlModelChecker(storm::models::Dtmc<ValueType> const& model, std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>&& linearEquationSolver) : SparsePropositionalModelChecker<ValueType>(model), linearEquationSolver(std::move(linearEquationSolver)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        SparseDtmcPrctlModelChecker<ValueType>::SparseDtmcPrctlModelChecker(storm::models::Dtmc<ValueType> const& model) : SparsePropositionalModelChecker<ValueType>(model), linearEquationSolver(storm::utility::solver::getLinearEquationSolver<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool SparseDtmcPrctlModelChecker<ValueType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isPctlStateFormula() || formula.isPctlPathFormula() || formula.isRewardPathFormula();
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeBoundedUntilProbabilitiesHelper(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound) const {
            std::vector<ValueType> result(this->getModel().getNumberOfStates(), storm::utility::zero<ValueType>());
            
            // If we identify the states that have probability 0 of reaching the target states, we can exclude them in the further analysis.
            storm::storage::BitVector statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(this->getModel().getBackwardTransitions(), phiStates, psiStates, true, stepBound);
            STORM_LOG_INFO("Found " << statesWithProbabilityGreater0.getNumberOfSetBits() << " 'maybe' states.");
            
            if (!statesWithProbabilityGreater0.empty()) {
                // We can eliminate the rows and columns from the original transition probability matrix that have probability 0.
                storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(true, statesWithProbabilityGreater0, statesWithProbabilityGreater0, true);
                
                // Compute the new set of target states in the reduced system.
                storm::storage::BitVector rightStatesInReducedSystem = psiStates % statesWithProbabilityGreater0;
                
                // Make all rows absorbing that satisfy the second sub-formula.
                submatrix.makeRowsAbsorbing(rightStatesInReducedSystem);
                
                // Create the vector with which to multiply.
                std::vector<ValueType> subresult(statesWithProbabilityGreater0.getNumberOfSetBits());
                storm::utility::vector::setVectorValues(subresult, rightStatesInReducedSystem, storm::utility::one<ValueType>());
                
                // Perform the matrix vector multiplication as often as required by the formula bound.
                STORM_LOG_THROW(linearEquationSolver != nullptr, storm::exceptions::InvalidStateException, "No valid linear equation solver available.");
                this->linearEquationSolver->performMatrixVectorMultiplication(submatrix, subresult, nullptr, stepBound);
                
                // Set the values of the resulting vector accordingly.
                storm::utility::vector::setVectorValues(result, statesWithProbabilityGreater0, subresult);
                storm::utility::vector::setVectorValues<ValueType>(result, ~statesWithProbabilityGreater0, storm::utility::zero<ValueType>());
            }
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();;
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::unique_ptr<CheckResult> result = std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeBoundedUntilProbabilitiesHelper(leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getUpperBound())));
            
            return result;
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeNextProbabilitiesHelper(storm::storage::BitVector const& nextStates) {
            // Create the vector with which to multiply and initialize it correctly.
            std::vector<ValueType> result(this->getModel().getNumberOfStates());
            storm::utility::vector::setVectorValues(result, nextStates, storm::utility::one<ValueType>());
            
            // Perform one single matrix-vector multiplication.
            STORM_LOG_THROW(linearEquationSolver != nullptr, storm::exceptions::InvalidStateException, "No valid linear equation solver available.");
            this->linearEquationSolver->performMatrixVectorMultiplication(this->getModel().getTransitionMatrix(), result);
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeNextProbabilitiesHelper(subResult.getTruthValuesVector())));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeUntilProbabilitiesHelper(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative) const {
            // We need to identify the states which have to be taken out of the matrix, i.e.
            // all states that have probability 0 and 1 of satisfying the until-formula.
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(this->getModel(), phiStates, psiStates);
            storm::storage::BitVector statesWithProbability0 = std::move(statesWithProbability01.first);
            storm::storage::BitVector statesWithProbability1 = std::move(statesWithProbability01.second);
            
            // Perform some logging.
            storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
            STORM_LOG_INFO("Found " << statesWithProbability0.getNumberOfSetBits() << " 'no' states.");
            STORM_LOG_INFO("Found " << statesWithProbability1.getNumberOfSetBits() << " 'yes' states.");
            STORM_LOG_INFO("Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
            
            // Create resulting vector.
            std::vector<ValueType> result(this->getModel().getNumberOfStates());
            
            // Check whether we need to compute exact probabilities for some states.
            if (qualitative) {
                // Set the values for all maybe-states to 0.5 to indicate that their probability values are neither 0 nor 1.
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, ValueType(0.5));
            } else {
                if (!maybeStates.empty()) {
                    // In this case we have have to compute the probabilities.
                    
                    // We can eliminate the rows and columns from the original transition probability matrix.
                    storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(true, maybeStates, maybeStates, true);
                    
                    // Converting the matrix from the fixpoint notation to the form needed for the equation
                    // system. That is, we go from x = A*x + b to (I-A)x = b.
                    submatrix.convertToEquationSystem();
                    
                    // Initialize the x vector with 0.5 for each element. This is the initial guess for
                    // the iterative solvers. It should be safe as for all 'maybe' states we know that the
                    // probability is strictly larger than 0.
                    std::vector<ValueType> x(maybeStates.getNumberOfSetBits(), ValueType(0.5));
                    
                    // Prepare the right-hand side of the equation system. For entry i this corresponds to
                    // the accumulated probability of going from state i to some 'yes' state.
                    std::vector<ValueType> b = this->getModel().getTransitionMatrix().getConstrainedRowSumVector(maybeStates, statesWithProbability1);
                    
                    // Now solve the created system of linear equations.
                    STORM_LOG_THROW(linearEquationSolver != nullptr, storm::exceptions::InvalidStateException, "No valid linear equation solver available.");
                    this->linearEquationSolver->solveEquationSystem(submatrix, x, b);
                    
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
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();;
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();;
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeUntilProbabilitiesHelper(leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative)));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeCumulativeRewardsHelper(uint_fast64_t stepBound) const {
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
            
            // Perform the matrix vector multiplication as often as required by the formula bound.
            STORM_LOG_THROW(linearEquationSolver != nullptr, storm::exceptions::InvalidStateException, "No valid linear equation solver available.");
            this->linearEquationSolver->performMatrixVectorMultiplication(this->getModel().getTransitionMatrix(), result, &totalRewardVector, stepBound);
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeCumulativeRewardsHelper(rewardPathFormula.getStepBound())));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeInstantaneousRewardsHelper(uint_fast64_t stepCount) const {
            // Only compute the result if the model has a state-based reward this->getModel().
            STORM_LOG_THROW(this->getModel().hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Initialize result to state rewards of the this->getModel().
            std::vector<ValueType> result(this->getModel().getStateRewardVector());
            
            // Perform the matrix vector multiplication as often as required by the formula bound.
            STORM_LOG_THROW(linearEquationSolver != nullptr, storm::exceptions::InvalidStateException, "No valid linear equation solver available.");
            this->linearEquationSolver->performMatrixVectorMultiplication(this->getModel().getTransitionMatrix(), result, nullptr, stepCount);
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeInstantaneousRewardsHelper(rewardPathFormula.getStepCount())));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeReachabilityRewardsHelper(storm::storage::BitVector const& targetStates, bool qualitative) const {
            // Only compute the result if the model has at least one reward this->getModel().
            STORM_LOG_THROW(this->getModel().hasStateRewards() || this->getModel().hasTransitionRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Determine which states have a reward of infinity by definition.
            storm::storage::BitVector trueStates(this->getModel().getNumberOfStates(), true);
            storm::storage::BitVector infinityStates = storm::utility::graph::performProb1(this->getModel().getBackwardTransitions(), trueStates, targetStates);
            infinityStates.complement();
            storm::storage::BitVector maybeStates = ~targetStates & ~infinityStates;
            STORM_LOG_INFO("Found " << infinityStates.getNumberOfSetBits() << " 'infinity' states.");
            STORM_LOG_INFO("Found " << targetStates.getNumberOfSetBits() << " 'target' states.");
            STORM_LOG_INFO("Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
            
            // Create resulting vector.
            std::vector<ValueType> result(this->getModel().getNumberOfStates());
            
            // Check whether we need to compute exact rewards for some states.
            if (qualitative) {
                // Set the values for all maybe-states to 1 to indicate that their reward values
                // are neither 0 nor infinity.
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, storm::utility::one<ValueType>());
            } else {
                // In this case we have to compute the reward values for the remaining states.
                // We can eliminate the rows and columns from the original transition probability matrix.
                storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(true, maybeStates, maybeStates, true);
                
                // Converting the matrix from the fixpoint notation to the form needed for the equation
                // system. That is, we go from x = A*x + b to (I-A)x = b.
                submatrix.convertToEquationSystem();
                
                // Initialize the x vector with 1 for each element. This is the initial guess for
                // the iterative solvers.
                std::vector<ValueType> x(submatrix.getColumnCount(), storm::utility::one<ValueType>());
                
                // Prepare the right-hand side of the equation system.
                std::vector<ValueType> b(submatrix.getRowCount());
                if (this->getModel().hasTransitionRewards()) {
                    // If a transition-based reward model is available, we initialize the right-hand
                    // side to the vector resulting from summing the rows of the pointwise product
                    // of the transition probability matrix and the transition reward matrix.
                    std::vector<ValueType> pointwiseProductRowSumVector = this->getModel().getTransitionMatrix().getPointwiseProductRowSumVector(this->getModel().getTransitionRewardMatrix());
                    storm::utility::vector::selectVectorValues(b, maybeStates, pointwiseProductRowSumVector);
                    
                    if (this->getModel().hasStateRewards()) {
                        // If a state-based reward model is also available, we need to add this vector
                        // as well. As the state reward vector contains entries not just for the states
                        // that we still consider (i.e. maybeStates), we need to extract these values
                        // first.
                        std::vector<ValueType> subStateRewards(b.size());
                        storm::utility::vector::selectVectorValues(subStateRewards, maybeStates, this->getModel().getStateRewardVector());
                        storm::utility::vector::addVectorsInPlace(b, subStateRewards);
                    }
                } else {
                    // If only a state-based reward model is  available, we take this vector as the
                    // right-hand side. As the state reward vector contains entries not just for the
                    // states that we still consider (i.e. maybeStates), we need to extract these values
                    // first.
                    storm::utility::vector::selectVectorValues(b, maybeStates, this->getModel().getStateRewardVector());
                }
                
                // Now solve the resulting equation system.
                STORM_LOG_THROW(linearEquationSolver != nullptr, storm::exceptions::InvalidStateException, "No valid linear equation solver available.");
                this->linearEquationSolver->solveEquationSystem(submatrix, x, b);
                
                // Set values of resulting vector according to result.
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
            }
            
            // Set values of resulting vector that are known exactly.
            storm::utility::vector::setVectorValues(result, targetStates, storm::utility::zero<ValueType>());
            storm::utility::vector::setVectorValues(result, infinityStates, storm::utility::infinity<ValueType>());
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeReachabilityRewardsHelper(subResult.getTruthValuesVector(), qualitative)));
        }
        
		template<typename ValueType>
		std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeLongRunAverageHelper(storm::storage::BitVector const& psiStates, bool qualitative) const {
			// If there are no goal states, we avoid the computation and directly return zero.
			auto numOfStates = this->getModel().getNumberOfStates();
			if (psiStates.empty()) {
				return std::vector<ValueType>(numOfStates, storm::utility::zero<ValueType>());
			}

			// Likewise, if all bits are set, we can avoid the computation and set.
			if ((~psiStates).empty()) {
				return std::vector<ValueType>(numOfStates, storm::utility::one<ValueType>());
			}

			// Start by decomposing the DTMC into its BSCCs.
			storm::storage::StronglyConnectedComponentDecomposition<double> bsccDecomposition(this->getModelAs<storm::models::AbstractModel<ValueType>>(), false, true);

			// Get some data members for convenience.
			typename storm::storage::SparseMatrix<ValueType> const& transitionMatrix = this->getModel().getTransitionMatrix();

			// First we check which states are in BSCCs. We use this later to speed up reachability analysis
			storm::storage::BitVector statesNotInBsccs(numOfStates, true);

			for (uint_fast64_t currentBsccIndex = 0; currentBsccIndex < bsccDecomposition.size(); ++currentBsccIndex) {
				storm::storage::StronglyConnectedComponent const& bscc = bsccDecomposition[currentBsccIndex];

				// Gather information for later use.
				for (auto const& state : bscc) {
					statesNotInBsccs.set(state, false);
				}
			}

			// Prepare result vector.
			std::vector<ValueType> result(numOfStates);

			//now we calculate the long run average for each BSCC in isolation and compute the weighted contribution of the BSCC to the LRA value of all states
			for (uint_fast64_t currentBsccIndex = 0; currentBsccIndex < bsccDecomposition.size(); ++currentBsccIndex) {
				storm::storage::StronglyConnectedComponent const& bscc = bsccDecomposition[currentBsccIndex];

				storm::storage::BitVector statesInThisBsccs(numOfStates);
				for (auto const& state : bscc) {
					statesInThisBsccs.set(state);
				}

				ValueType lraForThisBscc = this->computeLraForBSCC(transitionMatrix, psiStates, bscc);

				//the LRA value of a BSCC contributes to the LRA value of a state with the probability of reaching that BSCC from that state
				//thus we add Prob(Eventually statesInThisBscc) * lraForThisBscc to the result vector

				//the reachability probabilities will be zero in other BSCCs, thus we can set the left operand of the until formula to statesNotInBsccs as an optimization
				std::vector<ValueType> reachProbThisBscc = this->computeUntilProbabilitiesHelper(statesNotInBsccs, statesInThisBsccs, false);
				
				storm::utility::vector::scaleVectorInPlace<ValueType>(reachProbThisBscc, lraForThisBscc);
				storm::utility::vector::addVectorsInPlace<ValueType>(result, reachProbThisBscc);
			}

			return result;
		}

		template<typename ValueType>
		std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) {
			std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
			ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();

			return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeLongRunAverageHelper(subResult.getTruthValuesVector(), qualitative)));
		}


		template<typename ValueType>
		ValueType SparseDtmcPrctlModelChecker<ValueType>::computeLraForBSCC(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& psiStates, storm::storage::StronglyConnectedComponent const& bscc) {
			//if size is one we can immediately derive the result
			if (bscc.size() == 1){
				if (psiStates.get(*(bscc.begin()))) {
					return storm::utility::one<ValueType>();
				} else{
					return storm::utility::zero<ValueType>();
				}
			}
			std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = storm::utility::solver::getLinearEquationSolver<ValueType>();

			storm::storage::BitVector subsystem = storm::storage::BitVector(transitionMatrix.getRowCount());
			subsystem.set(bscc.begin(), bscc.end());

			//we now have to solve ((P')^t - I) * x = 0, where P' is the submatrix of transitionMatrix,
			// ^t means transose, and I is the identity matrix.
			
			storm::storage::SparseMatrix<ValueType> subsystemMatrix = transitionMatrix.getSubmatrix(false, subsystem, subsystem, true);
			subsystemMatrix = subsystemMatrix.transpose();

			// subtract 1 on the diagonal and at the same time add a row with all ones to enforce that the result of the equation system is unique
			storm::storage::SparseMatrixBuilder<ValueType> equationSystemBuilder(subsystemMatrix.getRowCount() + 1, subsystemMatrix.getColumnCount(), subsystemMatrix.getEntryCount() + subsystemMatrix.getColumnCount());
			ValueType one = storm::utility::one<ValueType>();
			ValueType zero = storm::utility::zero<ValueType>();
			bool foundDiagonalElement = false;
			for (uint_fast64_t row = 0; row < subsystemMatrix.getRowCount(); ++row) {
				for (auto& entry : subsystemMatrix.getRowGroup(row)) {
					if (entry.getColumn() == row) {
						equationSystemBuilder.addNextValue(row, entry.getColumn(), entry.getValue() - one);
						foundDiagonalElement = true;
					} else {
						equationSystemBuilder.addNextValue(row, entry.getColumn(), entry.getValue());
					}
				}

				// Throw an exception if a row did not have an element on the diagonal.
				STORM_LOG_THROW(foundDiagonalElement, storm::exceptions::InvalidOperationException, "Internal Error, no diagonal entry found.");
			}
			for (uint_fast64_t column = 0; column + 1 < subsystemMatrix.getColumnCount(); ++column) {
				equationSystemBuilder.addNextValue(subsystemMatrix.getRowCount(), column, one);
			}
			equationSystemBuilder.addNextValue(subsystemMatrix.getRowCount(), subsystemMatrix.getColumnCount() - 1, zero);
			subsystemMatrix = equationSystemBuilder.build();

			// create x and b for the equation system. setting the last entry of b to one enforces the sum over the unique solution vector is one
			std::vector<ValueType> steadyStateDist(subsystemMatrix.getRowCount(), zero);
			std::vector<ValueType> b(subsystemMatrix.getRowCount(), zero);
			b[subsystemMatrix.getRowCount() - 1] = one;

			solver->solveEquationSystem(subsystemMatrix, steadyStateDist, b);

			//remove the last entry of the vector as it was just there to enforce the unique solution
			steadyStateDist.pop_back();
			
			//calculate the fraction we spend in psi states on the long run
			std::vector<ValueType> steadyStateForPsiStates(transitionMatrix.getRowCount() - 1, zero);
			storm::utility::vector::setVectorValues(steadyStateForPsiStates, psiStates & subsystem, steadyStateDist);

			ValueType result = zero;

			for (auto value : steadyStateForPsiStates) {
				result += value;
			}

			return result;
		}

        template<typename ValueType>
        storm::models::Dtmc<ValueType> const& SparseDtmcPrctlModelChecker<ValueType>::getModel() const {
            return this->template getModelAs<storm::models::Dtmc<ValueType>>();
        }
        
        template class SparseDtmcPrctlModelChecker<double>;
    }
}