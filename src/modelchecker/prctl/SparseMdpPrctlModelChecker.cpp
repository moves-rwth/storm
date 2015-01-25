#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"

#include <vector>

#include "src/utility/ConstantsComparator.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include "src/modelchecker/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/ExplicitQuantitativeCheckResult.h"

#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        SparseMdpPrctlModelChecker<ValueType>::SparseMdpPrctlModelChecker(storm::models::Mdp<ValueType> const& model) : model(model), nondeterministicLinearEquationSolver(storm::utility::solver::getNondeterministicLinearEquationSolver<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        SparseMdpPrctlModelChecker<ValueType>::SparseMdpPrctlModelChecker(storm::models::Mdp<ValueType> const& model, std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministicLinearEquationSolver) : model(model), nondeterministicLinearEquationSolver(nondeterministicLinearEquationSolver) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool SparseMdpPrctlModelChecker<ValueType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isPctlStateFormula() || formula.isPctlPathFormula() || formula.isRewardPathFormula();
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeBoundedUntilProbabilitiesHelper(bool minimize, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound) {
            std::vector<ValueType> result(model.getNumberOfStates(), storm::utility::zero<ValueType>());
            
            // Determine the states that have 0 probability of reaching the target states.
            storm::storage::BitVector statesWithProbabilityGreater0;
            if (minimize) {
                statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0A(model.getTransitionMatrix(), model.getTransitionMatrix().getRowGroupIndices(), model.getBackwardTransitions(), phiStates, psiStates, true, stepBound);
            } else {
                statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0E(model.getTransitionMatrix(), model.getTransitionMatrix().getRowGroupIndices(), model.getBackwardTransitions(), phiStates, psiStates, true, stepBound);
            }
            STORM_LOG_INFO("Found " << statesWithProbabilityGreater0.getNumberOfSetBits() << " 'maybe' states.");
            
            if (!statesWithProbabilityGreater0.empty()) {
                // We can eliminate the rows and columns from the original transition probability matrix that have probability 0.
                storm::storage::SparseMatrix<ValueType> submatrix = model.getTransitionMatrix().getSubmatrix(true, statesWithProbabilityGreater0, statesWithProbabilityGreater0, false);
                
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
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult& leftResult = dynamic_cast<ExplicitQualitativeCheckResult&>(*leftResultPointer);
            ExplicitQualitativeCheckResult& rightResult = dynamic_cast<ExplicitQualitativeCheckResult&>(*rightResultPointer);
            std::unique_ptr<CheckResult> result = std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeBoundedUntilProbabilitiesHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, leftResult.getTruthValues(), rightResult.getTruthValues(), pathFormula.getUpperBound())));
            return result;
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeNextProbabilitiesHelper(bool minimize, storm::storage::BitVector const& nextStates) {
            // Create the vector with which to multiply and initialize it correctly.
            std::vector<ValueType> result(model.getNumberOfStates());
            storm::utility::vector::setVectorValues(result, nextStates, storm::utility::one<ValueType>());
            
            STORM_LOG_THROW(nondeterministicLinearEquationSolver != nullptr, storm::exceptions::InvalidStateException, "No valid equation solver available.");
            this->nondeterministicLinearEquationSolver->performMatrixVectorMultiplication(minimize, model.getTransitionMatrix(), result);
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult& subResult = dynamic_cast<ExplicitQualitativeCheckResult&>(*subResultPointer);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeNextProbabilitiesHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, subResult.getTruthValues())));
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
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult& leftResult = dynamic_cast<ExplicitQualitativeCheckResult&>(*leftResultPointer);
            ExplicitQualitativeCheckResult& rightResult = dynamic_cast<ExplicitQualitativeCheckResult&>(*rightResultPointer);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(SparseMdpPrctlModelChecker<ValueType>::computeUntilProbabilitiesHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, model.getTransitionMatrix(), model.getBackwardTransitions(), leftResult.getTruthValues(), rightResult.getTruthValues(), nondeterministicLinearEquationSolver, qualitative)));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeCumulativeRewardsHelper(bool minimize, uint_fast64_t stepBound) const {
            // Only compute the result if the model has at least one reward model.
            STORM_LOG_THROW(model.hasStateRewards() || model.hasTransitionRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Compute the reward vector to add in each step based on the available reward models.
            std::vector<ValueType> totalRewardVector;
            if (model.hasTransitionRewards()) {
                totalRewardVector = model.getTransitionMatrix().getPointwiseProductRowSumVector(model.getTransitionRewardMatrix());
                if (model.hasStateRewards()) {
                    storm::utility::vector::addVectorsInPlace(totalRewardVector, model.getStateRewardVector());
                }
            } else {
                totalRewardVector = std::vector<ValueType>(model.getStateRewardVector());
            }
            
            // Initialize result to either the state rewards of the model or the null vector.
            std::vector<ValueType> result;
            if (model.hasStateRewards()) {
                result = std::vector<ValueType>(model.getStateRewardVector());
            } else {
                result.resize(model.getNumberOfStates());
            }
            
            this->nondeterministicLinearEquationSolver->performMatrixVectorMultiplication(minimize, model.getTransitionMatrix(), result, &totalRewardVector, stepBound);
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeCumulativeRewardsHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, rewardPathFormula.getStepBound())));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeInstantaneousRewardsHelper(bool minimize, uint_fast64_t stepCount) const {
            // Only compute the result if the model has a state-based reward model.
            STORM_LOG_THROW(model.hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Initialize result to state rewards of the model.
            std::vector<ValueType> result(model.getStateRewardVector());
            
            STORM_LOG_THROW(nondeterministicLinearEquationSolver != nullptr, storm::exceptions::InvalidStateException, "No valid linear equation solver available.");
            this->nondeterministicLinearEquationSolver->performMatrixVectorMultiplication(minimize, model.getTransitionMatrix(), result, nullptr, stepCount);
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeInstantaneousRewardsHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, rewardPathFormula.getStepCount())));
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMdpPrctlModelChecker<ValueType>::computeReachabilityRewardsHelper(bool minimize, storm::storage::BitVector const& targetStates, bool qualitative) const {
            // Only compute the result if the model has at least one reward model.
            STORM_LOG_THROW(model.hasStateRewards() || model.hasTransitionRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Determine which states have a reward of infinity by definition.
            storm::storage::BitVector infinityStates;
            storm::storage::BitVector trueStates(model.getNumberOfStates(), true);
            if (minimize) {
                infinityStates = std::move(storm::utility::graph::performProb1A(model.getTransitionMatrix(), model.getTransitionMatrix().getRowGroupIndices(), model.getBackwardTransitions(), trueStates, targetStates));
            } else {
                infinityStates = std::move(storm::utility::graph::performProb1E(model.getTransitionMatrix(), model.getTransitionMatrix().getRowGroupIndices(), model.getBackwardTransitions(), trueStates, targetStates));
            }
            infinityStates.complement();
            storm::storage::BitVector maybeStates = ~targetStates & ~infinityStates;
            LOG4CPLUS_INFO(logger, "Found " << infinityStates.getNumberOfSetBits() << " 'infinity' states.");
            LOG4CPLUS_INFO(logger, "Found " << targetStates.getNumberOfSetBits() << " 'target' states.");
            LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
            
            // Create resulting vector.
            std::vector<ValueType> result(model.getNumberOfStates());
            
            // Check whether we need to compute exact rewards for some states.
            if (model.getInitialStates().isDisjointFrom(maybeStates)) {
                LOG4CPLUS_INFO(logger, "The rewards for the initial states were determined in a preprocessing step."
                               << " No exact rewards were computed.");
                // Set the values for all maybe-states to 1 to indicate that their reward values
                // are neither 0 nor infinity.
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, storm::utility::one<ValueType>());
            } else {
                // In this case we have to compute the reward values for the remaining states.
                
                // We can eliminate the rows and columns from the original transition probability matrix for states
                // whose reward values are already known.
                storm::storage::SparseMatrix<ValueType> submatrix = model.getTransitionMatrix().getSubmatrix(true, maybeStates, maybeStates, false);
                
                // Prepare the right-hand side of the equation system. For entry i this corresponds to
                // the accumulated probability of going from state i to some 'yes' state.
                std::vector<ValueType> b(submatrix.getRowCount());
                
                if (model.hasTransitionRewards()) {
                    // If a transition-based reward model is available, we initialize the right-hand
                    // side to the vector resulting from summing the rows of the pointwise product
                    // of the transition probability matrix and the transition reward matrix.
                    std::vector<ValueType> pointwiseProductRowSumVector = model.getTransitionMatrix().getPointwiseProductRowSumVector(model.getTransitionRewardMatrix());
                    storm::utility::vector::selectVectorValues(b, maybeStates, model.getTransitionMatrix().getRowGroupIndices(), pointwiseProductRowSumVector);
                    
                    if (model.hasStateRewards()) {
                        // If a state-based reward model is also available, we need to add this vector
                        // as well. As the state reward vector contains entries not just for the states
                        // that we still consider (i.e. maybeStates), we need to extract these values
                        // first.
                        std::vector<ValueType> subStateRewards(b.size());
                        storm::utility::vector::selectVectorValuesRepeatedly(subStateRewards, maybeStates, model.getTransitionMatrix().getRowGroupIndices(), model.getStateRewardVector());
                        storm::utility::vector::addVectorsInPlace(b, subStateRewards);
                    }
                } else {
                    // If only a state-based reward model is  available, we take this vector as the
                    // right-hand side. As the state reward vector contains entries not just for the
                    // states that we still consider (i.e. maybeStates), we need to extract these values
                    // first.
                    storm::utility::vector::selectVectorValuesRepeatedly(b, maybeStates, model.getTransitionMatrix().getRowGroupIndices(), model.getStateRewardVector());
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
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            ExplicitQualitativeCheckResult& subResult = dynamic_cast<ExplicitQualitativeCheckResult&>(*subResultPointer);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeReachabilityRewardsHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, subResult.getTruthValues(), qualitative)));
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::checkBooleanLiteralFormula(storm::logic::BooleanLiteralFormula const& stateFormula) {
            if (stateFormula.isTrueFormula()) {
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(storm::storage::BitVector(model.getNumberOfStates(), true)));
            } else {
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(storm::storage::BitVector(model.getNumberOfStates())));
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<ValueType>::checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula) {
            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(model.getLabeledStates(stateFormula.getLabel())));
        }
        
        template class SparseMdpPrctlModelChecker<double>;
    }
}