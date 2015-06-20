#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"

#include <vector>
#include <memory>

#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"
#include "src/utility/solver.h"

#include "src/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/exceptions/InvalidStateException.h"

#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        SparseDtmcPrctlModelChecker<ValueType>::SparseDtmcPrctlModelChecker(storm::models::sparse::Dtmc<ValueType> const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : SparsePropositionalModelChecker<ValueType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        SparseDtmcPrctlModelChecker<ValueType>::SparseDtmcPrctlModelChecker(storm::models::sparse::Dtmc<ValueType> const& model) : SparsePropositionalModelChecker<ValueType>(model), linearEquationSolverFactory(new storm::utility::solver::LinearEquationSolverFactory<ValueType>()) {
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
            storm::storage::BitVector maybeStates = storm::utility::graph::performProbGreater0(this->getModel().getBackwardTransitions(), phiStates, psiStates, true, stepBound);
            maybeStates &= ~psiStates;
            STORM_LOG_INFO("Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
            
            if (!maybeStates.empty()) {
                // We can eliminate the rows and columns from the original transition probability matrix that have probability 0.
                storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(true, maybeStates, maybeStates, true);
                
                // Create the vector of one-step probabilities to go to target states.
                std::vector<ValueType> b = this->getModel().getTransitionMatrix().getConstrainedRowSumVector(maybeStates, psiStates);
                
                // Create the vector with which to multiply.
                std::vector<ValueType> subresult(maybeStates.getNumberOfSetBits());
                
                // Perform the matrix vector multiplication as often as required by the formula bound.
                STORM_LOG_THROW(linearEquationSolverFactory != nullptr, storm::exceptions::InvalidStateException, "No valid linear equation solver available.");
                std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory->create(submatrix);
                solver->performMatrixVectorMultiplication(subresult, &b, stepBound);
                
                // Set the values of the resulting vector accordingly.
                storm::utility::vector::setVectorValues(result, maybeStates, subresult);
            }
            storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::one<ValueType>());
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(pathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();;
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::unique_ptr<CheckResult> result = std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeBoundedUntilProbabilitiesHelper(leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getDiscreteTimeBound())));
            return result;
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeNextProbabilitiesHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& nextStates, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
            // Create the vector with which to multiply and initialize it correctly.
            std::vector<ValueType> result(transitionMatrix.getRowCount());
            storm::utility::vector::setVectorValues(result, nextStates, storm::utility::one<ValueType>());
            
            // Perform one single matrix-vector multiplication.
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(transitionMatrix);
            solver->performMatrixVectorMultiplication(result);
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeNextProbabilitiesHelper(this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), *this->linearEquationSolverFactory)));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeUntilProbabilitiesHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
            // We need to identify the states which have to be taken out of the matrix, i.e.
            // all states that have probability 0 and 1 of satisfying the until-formula.
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(backwardTransitions, phiStates, psiStates);
            storm::storage::BitVector statesWithProbability0 = std::move(statesWithProbability01.first);
            storm::storage::BitVector statesWithProbability1 = std::move(statesWithProbability01.second);
            
            // Perform some logging.
            storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
            STORM_LOG_INFO("Found " << statesWithProbability0.getNumberOfSetBits() << " 'no' states.");
            STORM_LOG_INFO("Found " << statesWithProbability1.getNumberOfSetBits() << " 'yes' states.");
            STORM_LOG_INFO("Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
            
            // Create resulting vector.
            std::vector<ValueType> result(transitionMatrix.getRowCount());
            
            // Check whether we need to compute exact probabilities for some states.
            if (qualitative) {
                // Set the values for all maybe-states to 0.5 to indicate that their probability values are neither 0 nor 1.
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, ValueType(0.5));
            } else {
                if (!maybeStates.empty()) {
                    // In this case we have have to compute the probabilities.
                    
                    // We can eliminate the rows and columns from the original transition probability matrix.
                    storm::storage::SparseMatrix<ValueType> submatrix = transitionMatrix.getSubmatrix(true, maybeStates, maybeStates, true);
                    
                    // Converting the matrix from the fixpoint notation to the form needed for the equation
                    // system. That is, we go from x = A*x + b to (I-A)x = b.
                    submatrix.convertToEquationSystem();
                    
                    // Initialize the x vector with 0.5 for each element. This is the initial guess for
                    // the iterative solvers. It should be safe as for all 'maybe' states we know that the
                    // probability is strictly larger than 0.
                    std::vector<ValueType> x(maybeStates.getNumberOfSetBits(), ValueType(0.5));
                    
                    // Prepare the right-hand side of the equation system. For entry i this corresponds to
                    // the accumulated probability of going from state i to some 'yes' state.
                    std::vector<ValueType> b = transitionMatrix.getConstrainedRowSumVector(maybeStates, statesWithProbability1);
                    
                    // Now solve the created system of linear equations.
                    std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(submatrix);
                    solver->solveEquationSystem(x, b);
                    
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
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeUntilProbabilitiesHelper(this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, *this->linearEquationSolverFactory)));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeCumulativeRewardsHelper(uint_fast64_t stepBound) const {
            // Only compute the result if the model has at least one reward model.
            STORM_LOG_THROW(this->getModel().hasStateRewards() || this->getModel().hasTransitionRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Compute the reward vector to add in each step based on the available reward models.
            std::vector<ValueType> totalRewardVector;
            if (this->getModel().hasTransitionRewards()) {
                totalRewardVector = this->getModel().getTransitionMatrix().getPointwiseProductRowSumVector(this->getModel().getTransitionRewardMatrix());
                if (this->getModel().hasStateRewards()) {
                    storm::utility::vector::addVectors(totalRewardVector, this->getModel().getStateRewardVector(), totalRewardVector);
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
            STORM_LOG_THROW(linearEquationSolverFactory != nullptr, storm::exceptions::InvalidStateException, "No valid linear equation solver available.");
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory->create(this->getModel().getTransitionMatrix());
            solver->performMatrixVectorMultiplication(result, &totalRewardVector, stepBound);
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeCumulativeRewardsHelper(rewardPathFormula.getDiscreteTimeBound())));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeInstantaneousRewardsHelper(uint_fast64_t stepCount) const {
            // Only compute the result if the model has a state-based reward this->getModel().
            STORM_LOG_THROW(this->getModel().hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Initialize result to state rewards of the model.
            std::vector<ValueType> result(this->getModel().getStateRewardVector());
            
            // Perform the matrix vector multiplication as often as required by the formula bound.
            STORM_LOG_THROW(linearEquationSolverFactory != nullptr, storm::exceptions::InvalidStateException, "No valid linear equation solver available.");
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory->create(this->getModel().getTransitionMatrix());
            solver->performMatrixVectorMultiplication(result, nullptr, stepCount);
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeInstantaneousRewardsHelper(rewardPathFormula.getDiscreteTimeBound())));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseDtmcPrctlModelChecker<ValueType>::computeReachabilityRewardsHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, boost::optional<std::vector<ValueType>> const& stateRewardVector, boost::optional<storm::storage::SparseMatrix<ValueType>> const& transitionRewardMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& targetStates, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory, bool qualitative) {
            // Only compute the result if the model has at least one reward model.
            STORM_LOG_THROW(stateRewardVector || transitionRewardMatrix, storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Determine which states have a reward of infinity by definition.
            storm::storage::BitVector trueStates(transitionMatrix.getRowCount(), true);
            storm::storage::BitVector infinityStates = storm::utility::graph::performProb1(backwardTransitions, trueStates, targetStates);
            infinityStates.complement();
            storm::storage::BitVector maybeStates = ~targetStates & ~infinityStates;
            STORM_LOG_INFO("Found " << infinityStates.getNumberOfSetBits() << " 'infinity' states.");
            STORM_LOG_INFO("Found " << targetStates.getNumberOfSetBits() << " 'target' states.");
            STORM_LOG_INFO("Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
            
            // Create resulting vector.
            std::vector<ValueType> result(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());
            
            // Check whether we need to compute exact rewards for some states.
            if (qualitative) {
                // Set the values for all maybe-states to 1 to indicate that their reward values
                // are neither 0 nor infinity.
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, storm::utility::one<ValueType>());
            } else {
                // In this case we have to compute the reward values for the remaining states.
                // We can eliminate the rows and columns from the original transition probability matrix.
                storm::storage::SparseMatrix<ValueType> submatrix = transitionMatrix.getSubmatrix(true, maybeStates, maybeStates, true);
                
                // Converting the matrix from the fixpoint notation to the form needed for the equation
                // system. That is, we go from x = A*x + b to (I-A)x = b.
                submatrix.convertToEquationSystem();
                                
                // Initialize the x vector with 1 for each element. This is the initial guess for
                // the iterative solvers.
                std::vector<ValueType> x(submatrix.getColumnCount(), storm::utility::one<ValueType>());
                
                // Prepare the right-hand side of the equation system.
                std::vector<ValueType> b(submatrix.getRowCount());
                if (transitionRewardMatrix) {
                    // If a transition-based reward model is available, we initialize the right-hand
                    // side to the vector resulting from summing the rows of the pointwise product
                    // of the transition probability matrix and the transition reward matrix.
                    std::vector<ValueType> pointwiseProductRowSumVector = transitionMatrix.getPointwiseProductRowSumVector(transitionRewardMatrix.get());
                    storm::utility::vector::selectVectorValues(b, maybeStates, pointwiseProductRowSumVector);
                    
                    if (stateRewardVector) {
                        // If a state-based reward model is also available, we need to add this vector
                        // as well. As the state reward vector contains entries not just for the states
                        // that we still consider (i.e. maybeStates), we need to extract these values
                        // first.
                        std::vector<ValueType> subStateRewards(b.size());
                        storm::utility::vector::selectVectorValues(subStateRewards, maybeStates, stateRewardVector.get());
                        storm::utility::vector::addVectors(b, subStateRewards, b);
                    }
                } else {
                    // If only a state-based reward model is  available, we take this vector as the
                    // right-hand side. As the state reward vector contains entries not just for the
                    // states that we still consider (i.e. maybeStates), we need to extract these values
                    // first.
                    storm::utility::vector::selectVectorValues(b, maybeStates, stateRewardVector.get());
                }
                
                // Now solve the resulting equation system.
                std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(submatrix);
                solver->solveEquationSystem(x, b);
                
                // Set values of resulting vector according to result.
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
            }
            
            // Set values of resulting vector that are known exactly.
            storm::utility::vector::setVectorValues(result, infinityStates, storm::utility::infinity<ValueType>());
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeReachabilityRewardsHelper(this->getModel().getTransitionMatrix(), this->getModel().getOptionalStateRewardVector(), this->getModel().getOptionalTransitionRewardMatrix(), this->getModel().getBackwardTransitions(), subResult.getTruthValuesVector(), *this->linearEquationSolverFactory, qualitative)));
        }

		template<typename ValueType>
		std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<ValueType>::computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
			std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
			ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();

            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(SparseCtmcCslModelChecker<ValueType>::computeLongRunAverageHelper(this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), nullptr, qualitative, *linearEquationSolverFactory)));
		}

        template<typename ValueType>
        storm::models::sparse::Dtmc<ValueType> const& SparseDtmcPrctlModelChecker<ValueType>::getModel() const {
            return this->template getModelAs<storm::models::sparse::Dtmc<ValueType>>();
        }
        
        template class SparseDtmcPrctlModelChecker<double>;
    }
}