#include "src/modelchecker/csl/SparseCtmcCslModelChecker.h"

#include <vector>

#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"
#include "src/utility/solver.h"
#include "src/utility/numerical.h"

#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        template<class ValueType>
        SparseCtmcCslModelChecker<ValueType>::SparseCtmcCslModelChecker(storm::models::sparse::Ctmc<ValueType> const& model) : SparsePropositionalModelChecker<ValueType>(model), linearEquationSolver(storm::utility::solver::getLinearEquationSolver<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<class ValueType>
        SparseCtmcCslModelChecker<ValueType>::SparseCtmcCslModelChecker(storm::models::sparse::Ctmc<ValueType> const& model, std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>&& linearEquationSolver) : SparsePropositionalModelChecker<ValueType>(model), linearEquationSolver(std::move(linearEquationSolver)) {
            // Intentionally left empty.
        }
        
        template<class ValueType>
        bool SparseCtmcCslModelChecker<ValueType>::canHandle(storm::logic::Formula const& formula) const {
            // FIXME: refine.
            return formula.isCslStateFormula() || formula.isCslPathFormula() || formula.isRewardPathFormula();
        }
        
        template<class ValueType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<ValueType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(pathFormula.isIntervalBounded(), storm::exceptions::InvalidPropertyException, "Cannot treat non-interval bounded until.");
            
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();;
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::pair<double, double> const& intervalBounds =  pathFormula.getIntervalBounds();
            std::unique_ptr<CheckResult> result = std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeBoundedUntilProbabilitiesHelper(leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), this->getModel().getExitRateVector(), qualitative, intervalBounds.first, intervalBounds.second)));
            
            return result;
        }
        
        template<class ValueType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<ValueType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> result = SparseDtmcPrctlModelChecker<ValueType>::computeNextProbabilitiesHelper(this->computeProbabilityMatrix(this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector()), subResult.getTruthValuesVector(), *this->linearEquationSolver);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
        }
        
        template<class ValueType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<ValueType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeUntilProbabilitiesHelper(this->computeProbabilityMatrix(this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector()), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, *this->linearEquationSolver)));
        }
        
        template<class ValueType>
        storm::models::sparse::Ctmc<ValueType> const& SparseCtmcCslModelChecker<ValueType>::getModel() const {
            return this->template getModelAs<storm::models::sparse::Ctmc<ValueType>>();
        }
        
        template<class ValueType>
        std::vector<ValueType> SparseCtmcCslModelChecker<ValueType>::computeBoundedUntilProbabilitiesHelper(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<ValueType> const& exitRates, bool qualitative, double lowerBound, double upperBound) const {
            // If the time bounds are [0, inf], we rather call untimed reachability.
            storm::utility::ConstantsComparator<ValueType> comparator;
            if (comparator.isZero(lowerBound) && comparator.isInfinity(upperBound)) {
                return this->computeUntilProbabilitiesHelper(this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), phiStates, psiStates, qualitative, *this->linearEquationSolver);
            }

            // From this point on, we know that we have to solve a more complicated problem [t, t'] with either t != 0
            // or t' != inf.
            
            // Create the result vector.
            std::vector<ValueType> result(this->getModel().getNumberOfStates(), storm::utility::zero<ValueType>());
            
            // If we identify the states that have probability 0 of reaching the target states, we can exclude them from the
            // further computations.
            storm::storage::SparseMatrix<ValueType> backwardTransitions = this->getModel().getBackwardTransitions();
            storm::storage::BitVector statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(backwardTransitions, phiStates, psiStates);
            STORM_LOG_INFO("Found " << statesWithProbabilityGreater0.getNumberOfSetBits() << " states with probability greater 0.");
            storm::storage::BitVector statesWithProbabilityGreater0NonPsi = statesWithProbabilityGreater0 & ~psiStates;
            STORM_LOG_INFO("Found " << statesWithProbabilityGreater0NonPsi.getNumberOfSetBits() << " 'maybe' states.");
            
            if (!statesWithProbabilityGreater0NonPsi.empty()) {
                if (comparator.isZero(upperBound)) {
                    // In this case, the interval is of the form [0, 0].
                    storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::one<ValueType>());
                } else {
                    if (comparator.isZero(lowerBound)) {
                        // In this case, the interval is of the form [0, t].
                        // Note that this excludes [0, inf] since this is untimed reachability and we considered this case earlier.

                        // Find the maximal rate of all 'maybe' states to take it as the uniformization rate.
                        ValueType uniformizationRate = 0;
                        for (auto const& state : statesWithProbabilityGreater0NonPsi) {
                            uniformizationRate = std::max(uniformizationRate, exitRates[state]);
                        }
                        STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                        
                        // Compute the uniformized matrix.
                        storm::storage::SparseMatrix<ValueType> uniformizedMatrix = this->computeUniformizedMatrix(this->getModel().getTransitionMatrix(), storm::storage::BitVector(this->getModel().getNumberOfStates(), true), psiStates, uniformizationRate, exitRates);

                        // Finally compute the transient probabilities.
                        std::vector<ValueType> psiStateValues(statesWithProbabilityGreater0.getNumberOfSetBits(), storm::utility::zero<ValueType>());
                        storm::utility::vector::setVectorValues(psiStateValues, psiStates % statesWithProbabilityGreater0, storm::utility::one<ValueType>());
                        std::vector<ValueType> subresult = this->computeTransientProbabilities(uniformizedMatrix, uniformizationRate * upperBound, psiStateValues, *this->linearEquationSolver);
                        storm::utility::vector::setVectorValues(result, statesWithProbabilityGreater0, subresult);
                    } else if (comparator.isInfinity(upperBound)) {
                        // In this case, the interval is of the form [t, inf] with t != 0.
                        
                        // Start by computing the (unbounded) reachability probabilities of reaching psi states while
                        // staying in phi states.
                        result = this->computeUntilProbabilitiesHelper(this->getModel().getTransitionMatrix(), backwardTransitions, phiStates, psiStates, qualitative, *this->linearEquationSolver);
                        
                        ValueType uniformizationRate = 0;
                        for (auto const& state : statesWithProbabilityGreater0) {
                            uniformizationRate = std::max(uniformizationRate, exitRates[state]);
                        }
                        
                        // Compute the uniformized matrix.
                        storm::storage::SparseMatrix<ValueType> uniformizedMatrix = this->computeUniformizedMatrix(this->getModel().getTransitionMatrix(), statesWithProbabilityGreater0, storm::storage::BitVector(statesWithProbabilityGreater0.getNumberOfSetBits()), uniformizationRate, exitRates);
                        
                        // Finally compute the transient probabilities.
                        result = this->computeTransientProbabilities(uniformizedMatrix, uniformizationRate * lowerBound, result, *this->linearEquationSolver);
                    } else {
                        // In this case, the interval is of the form [t, t'] with t != 0 and t' != inf.
                        
                        // Find the maximal rate of all 'maybe' states to take it as the uniformization rate.
                        ValueType uniformizationRate = 0;
                        for (auto const& state : statesWithProbabilityGreater0NonPsi) {
                            uniformizationRate = std::max(uniformizationRate, exitRates[state]);
                        }
                        STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                        
                        // Compute the (first) uniformized matrix.
                        storm::storage::SparseMatrix<ValueType> uniformizedMatrix = this->computeUniformizedMatrix(this->getModel().getTransitionMatrix(), statesWithProbabilityGreater0, psiStates, uniformizationRate, exitRates);
                        
                        // Start by computing the transient probabilities of reaching a psi state in time t' - t.
                        std::vector<ValueType> psiStateValues(statesWithProbabilityGreater0.getNumberOfSetBits(), storm::utility::zero<ValueType>());
                        storm::utility::vector::setVectorValues(psiStateValues, psiStates % statesWithProbabilityGreater0, storm::utility::one<ValueType>());
                        std::vector<ValueType> subresult = this->computeTransientProbabilities(uniformizedMatrix, uniformizationRate * (upperBound - lowerBound), psiStateValues, *this->linearEquationSolver);
                        storm::utility::vector::setVectorValues(result, statesWithProbabilityGreater0, subresult);

                        // Then compute the transient probabilities of being in such a state after t time units. For this,
                        // we must re-uniformize the CTMC, so we need to compute the second uniformized matrix.
                        storm::storage::BitVector absorbingStates = ~phiStates;
                        uniformizationRate = 0;
                        for (auto const& state : ~absorbingStates) {
                            uniformizationRate = std::max(uniformizationRate, exitRates[state]);
                        }
                        
                        // Finally, we compute the second set of transient probabilities.
                        uniformizedMatrix = this->computeUniformizedMatrix(this->getModel().getTransitionMatrix(), storm::storage::BitVector(this->getModel().getNumberOfStates(), true), absorbingStates, uniformizationRate, exitRates);
                        result = this->computeTransientProbabilities(uniformizedMatrix, uniformizationRate * lowerBound, result, *this->linearEquationSolver);
                    }
                }
            }
            
            return result;
        }
        
        template<class ValueType>
        storm::storage::SparseMatrix<ValueType> SparseCtmcCslModelChecker<ValueType>::computeUniformizedMatrix(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& maybeStates, storm::storage::BitVector const& absorbingStates, ValueType uniformizationRate, std::vector<ValueType> const& exitRates) {
            // Create the submatrix that only contains the states with a positive probability (including the
            // psi states) and reserve space for elements on the diagonal.
            storm::storage::SparseMatrix<ValueType> uniformizedMatrix = transitionMatrix.getSubmatrix(false, maybeStates, maybeStates, true);
            
            // Make the appropriate states absorbing.
            uniformizedMatrix.makeRowsAbsorbing(absorbingStates % maybeStates);
            
            // Now we need to perform the actual uniformization. That is, all entries need to be divided by
            // the uniformization rate, and the diagonal needs to be set to the negative exit rate of the
            // state plus the self-loop rate and then increased by one.
            uint_fast64_t currentRow = 0;
            for (auto const& state : maybeStates) {
                for (auto& element : uniformizedMatrix.getRow(currentRow)) {
                    if (element.getColumn() == currentRow) {
                        if (absorbingStates.get(state)) {
                            // Nothing to do here, since the state has already been made absorbing.
                        } else {
                            element.setValue(-(exitRates[state] + element.getValue()) / uniformizationRate + storm::utility::one<ValueType>());
                        }
                    } else {
                        element.setValue(element.getValue() / uniformizationRate);
                    }
                }
                ++currentRow;
            }
        }
        
        template<class ValueType>
        std::vector<ValueType> SparseCtmcCslModelChecker<ValueType>::computeTransientProbabilities(storm::storage::SparseMatrix<ValueType> const& uniformizedMatrix, ValueType const& lambda, std::vector<ValueType> values, storm::solver::LinearEquationSolver<ValueType> const& linearEquationSolver) const {
            // Use Fox-Glynn to get the truncation points and the weights.
            std::tuple<uint_fast64_t, uint_fast64_t, ValueType, std::vector<ValueType>> foxGlynnResult = storm::utility::numerical::getFoxGlynnCutoff(lambda, 1e-300, 1e+300, storm::settings::generalSettings().getPrecision() / 8.0);
            
            // Scale the weights so they add up to one.
            for (auto& element : std::get<3>(foxGlynnResult)) {
                element /= std::get<2>(foxGlynnResult);
            }
            
            // Initialize result.
            std::vector<ValueType> result;
            if (std::get<0>(foxGlynnResult) == 0) {
                result = values;
                storm::utility::vector::scaleVectorInPlace(result, std::get<3>(foxGlynnResult)[0]);
            } else {
                result = std::vector<ValueType>(values.size());
            }
            std::vector<ValueType> multiplicationResult(result.size());
            
            // Perform the matrix-vector multiplications (without adding)
            if (std::get<0>(foxGlynnResult) > 1) {
                linearEquationSolver.performMatrixVectorMultiplication(uniformizedMatrix, values, &result, std::get<0>(foxGlynnResult) - 1, &multiplicationResult);
            }
            
            // For the indices that fall in between the truncation points, we need to perform the matrix-vector
            // multiplication, scale and add the result.
            ValueType weight = 0;
            std::function<ValueType(ValueType const&, ValueType const&)> addAndScale = [&weight] (ValueType const& a, ValueType const& b) { return a + weight * b; };
            for (uint_fast64_t index = std::get<0>(foxGlynnResult); index <= std::get<1>(foxGlynnResult); ++index) {
                linearEquationSolver.performMatrixVectorMultiplication(uniformizedMatrix, values, nullptr, 1, &multiplicationResult);
                
                ValueType weight = std::get<3>(foxGlynnResult)[index - std::get<0>(foxGlynnResult)];
                storm::utility::vector::applyPointwiseInPlace(result, values, addAndScale);
            }
            
            return result;
        }
        
        template<class ValueType>
        std::vector<ValueType> SparseCtmcCslModelChecker<ValueType>::computeUntilProbabilitiesHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::LinearEquationSolver<ValueType> const& linearEquationSolver) {
            return SparseDtmcPrctlModelChecker<ValueType>::computeUntilProbabilitiesHelper(transitionMatrix, backwardTransitions, phiStates, psiStates, qualitative, linearEquationSolver);
        }
        
        template<class ValueType>
        storm::storage::SparseMatrix<ValueType> SparseCtmcCslModelChecker<ValueType>::computeProbabilityMatrix(storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& exitRates) {
            // Turn the rates into probabilities by scaling each row with the exit rate of the state.
            storm::storage::SparseMatrix<ValueType> result(rateMatrix);
            for (uint_fast64_t row = 0; row < result.getRowCount(); ++row) {
                for (auto& entry : result.getRow(row)) {
                    entry.setValue(entry.getValue() / exitRates[row]);
                }
            }
            return result;
        }
        
        // Explicitly instantiate the model checker.
        template class SparseCtmcCslModelChecker<double>;
        
    } // namespace modelchecker
} // namespace storm