#include "src/modelchecker/csl/HybridCtmcCslModelChecker.h"
#include "src/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "src/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"

#include "src/storage/dd/CuddOdd.h"
#include "src/storage/dd/CuddDdManager.h"

#include "src/utility/macros.h"
#include "src/utility/graph.h"


#include "src/settings/modules/GeneralSettings.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "src/modelchecker/results/HybridQuantitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"



namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType DdType, class ValueType>
        HybridCtmcCslModelChecker<DdType, ValueType>::HybridCtmcCslModelChecker(storm::models::symbolic::Ctmc<DdType> const& model) : SymbolicPropositionalModelChecker<DdType>(model), linearEquationSolverFactory(new storm::utility::solver::LinearEquationSolverFactory<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        HybridCtmcCslModelChecker<DdType, ValueType>::HybridCtmcCslModelChecker(storm::models::symbolic::Ctmc<DdType> const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<DdType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        bool HybridCtmcCslModelChecker<DdType, ValueType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isCslStateFormula() || formula.isCslPathFormula() || formula.isRewardPathFormula();
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        storm::dd::Add<DdType> HybridCtmcCslModelChecker<DdType, ValueType>::computeProbabilityMatrix(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& rateMatrix, storm::dd::Add<DdType> const& exitRateVector) {
            return rateMatrix / exitRateVector;
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            return HybridDtmcPrctlModelChecker<DdType, ValueType>::computeUntilProbabilitiesHelper(this->getModel(), this->computeProbabilityMatrix(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector()), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), HybridDtmcPrctlModelChecker<DdType, ValueType>::computeNextProbabilitiesHelper(this->getModel(), this->computeProbabilityMatrix(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector()), subResult.getTruthValuesVector())));
        }

        template<storm::dd::DdType DdType, class ValueType>
        storm::models::symbolic::Ctmc<DdType> const& HybridCtmcCslModelChecker<DdType, ValueType>::getModel() const {
            return this->template getModelAs<storm::models::symbolic::Ctmc<DdType>>();
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        storm::dd::Add<DdType> HybridCtmcCslModelChecker<DdType, ValueType>::computeUniformizedMatrix(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Add<DdType> const& exitRateVector, storm::dd::Bdd<DdType> const& maybeStates, ValueType uniformizationRate) {
            STORM_LOG_DEBUG("Computing uniformized matrix using uniformization rate " << uniformizationRate << ".");
            STORM_LOG_DEBUG("Keeping " << maybeStates.getNonZeroCount() << " rows.");
            
            // Cut all non-maybe rows/columns from the transition matrix.
            storm::dd::Add<DdType> uniformizedMatrix = transitionMatrix * maybeStates.toAdd() * maybeStates.swapVariables(model.getRowColumnMetaVariablePairs()).toAdd();
            
            // Now perform the uniformization.
            uniformizedMatrix = uniformizedMatrix / model.getManager().getConstant(uniformizationRate);
            storm::dd::Add<DdType> diagonal = model.getRowColumnIdentity() * maybeStates.toAdd();
            storm::dd::Add<DdType> diagonalOffset = diagonal;
            diagonalOffset -= diagonal * (exitRateVector / model.getManager().getConstant(uniformizationRate));
            uniformizedMatrix += diagonalOffset;
            
            return uniformizedMatrix;
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();

            boost::optional<storm::dd::Add<DdType>> modifiedStateRewardVector;
            if (this->getModel().hasStateRewards()) {
                modifiedStateRewardVector = this->getModel().getStateRewardVector() / this->getModel().getExitRateVector();
            }
            
            return HybridDtmcPrctlModelChecker<DdType, ValueType>::computeReachabilityRewardsHelper(this->getModel(), this->computeProbabilityMatrix(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector()), modifiedStateRewardVector, this->getModel().getOptionalTransitionRewardMatrix(), subResult.getTruthValuesVector(), *linearEquationSolverFactory, qualitative);
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            double lowerBound = 0;
            double upperBound = 0;
            if (!pathFormula.hasDiscreteTimeBound()) {
                std::pair<double, double> const& intervalBounds =  pathFormula.getIntervalBounds();
                lowerBound = intervalBounds.first;
                upperBound = intervalBounds.second;
            } else {
                upperBound = pathFormula.getDiscreteTimeBound();
            }
            
            return this->computeBoundedUntilProbabilitiesHelper(leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), this->getModel().getExitRateVector(), qualitative, lowerBound, upperBound);
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeBoundedUntilProbabilitiesHelper(storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, storm::dd::Add<DdType> const& exitRates, bool qualitative, double lowerBound, double upperBound) const {
            // If the time bounds are [0, inf], we rather call untimed reachability.
            storm::utility::ConstantsComparator<ValueType> comparator;
            if (comparator.isZero(lowerBound) && comparator.isInfinity(upperBound)) {
                return HybridDtmcPrctlModelChecker<DdType, ValueType>::computeUntilProbabilitiesHelper(this->getModel(), this->computeProbabilityMatrix(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector()), phiStates, psiStates, qualitative, *this->linearEquationSolverFactory);
            }
            
            // From this point on, we know that we have to solve a more complicated problem [t, t'] with either t != 0
            // or t' != inf.
            
            // If we identify the states that have probability 0 of reaching the target states, we can exclude them from the
            // further computations.
            storm::dd::Bdd<DdType> statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(this->getModel(), this->getModel().getTransitionMatrix().notZero(), phiStates, psiStates);
            STORM_LOG_INFO("Found " << statesWithProbabilityGreater0.getNonZeroCount() << " states with probability greater 0.");
            storm::dd::Bdd<DdType> statesWithProbabilityGreater0NonPsi = statesWithProbabilityGreater0 && !psiStates;
            STORM_LOG_INFO("Found " << statesWithProbabilityGreater0NonPsi.getNonZeroCount() << " 'maybe' states.");
            
            if (!statesWithProbabilityGreater0NonPsi.isZero()) {
                if (comparator.isZero(upperBound)) {
                    // In this case, the interval is of the form [0, 0].
                    return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), psiStates.toAdd()));
                } else {
                    if (comparator.isZero(lowerBound)) {
                        // In this case, the interval is of the form [0, t].
                        // Note that this excludes [0, inf] since this is untimed reachability and we considered this case earlier.
                        
                        // Find the maximal rate of all 'maybe' states to take it as the uniformization rate.
                        ValueType uniformizationRate =  1.02 * (statesWithProbabilityGreater0NonPsi.toAdd() * exitRates).getMax();
                        STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                        
                        // Compute the uniformized matrix.
                        storm::dd::Add<DdType> uniformizedMatrix = this->computeUniformizedMatrix(this->getModel(), this->getModel().getTransitionMatrix(), exitRates, statesWithProbabilityGreater0NonPsi, uniformizationRate);
                        
                        // Compute the vector that is to be added as a compensation for removing the absorbing states.
                        storm::dd::Add<DdType> b = (statesWithProbabilityGreater0NonPsi.toAdd() * this->getModel().getTransitionMatrix() * psiStates.swapVariables(this->getModel().getRowColumnMetaVariablePairs()).toAdd()).sumAbstract(this->getModel().getColumnVariables()) / this->getModel().getManager().getConstant(uniformizationRate);
                        
                        // Create an ODD for the translation to an explicit representation.
                        storm::dd::Odd<DdType> odd(statesWithProbabilityGreater0NonPsi);
                        
                        // Convert the symbolic parts to their explicit representation.
                        storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                        std::vector<ValueType> explicitB = b.template toVector<ValueType>(odd);
                        
                        // Finally compute the transient probabilities.
                        std::vector<ValueType> values(statesWithProbabilityGreater0NonPsi.getNonZeroCount(), storm::utility::zero<ValueType>());
                        std::vector<ValueType> subresult = SparseCtmcCslModelChecker<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, &explicitB, upperBound, uniformizationRate, values, *this->linearEquationSolverFactory);

                        return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(),
                                                                                                      (psiStates || !statesWithProbabilityGreater0) && this->getModel().getReachableStates(),
                                                                                                      psiStates.toAdd(),
                                                                                                      statesWithProbabilityGreater0NonPsi,
                                                                                                      odd, subresult));
                    } else if (comparator.isInfinity(upperBound)) {
                        // In this case, the interval is of the form [t, inf] with t != 0.
                        
                        // Start by computing the (unbounded) reachability probabilities of reaching psi states while
                        // staying in phi states.
                        std::unique_ptr<CheckResult> unboundedResult = HybridDtmcPrctlModelChecker<DdType, ValueType>::computeUntilProbabilitiesHelper(this->getModel(), this->computeProbabilityMatrix(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector()), phiStates, psiStates, qualitative, *this->linearEquationSolverFactory);
                        
                        // Compute the set of relevant states.
                        storm::dd::Bdd<DdType> relevantStates = statesWithProbabilityGreater0 && phiStates;

                        // Filter the unbounded result such that it only contains values for the relevant states.
                        unboundedResult->filter(SymbolicQualitativeCheckResult<DdType>(this->getModel().getReachableStates(), relevantStates));

                        // Build an ODD for the relevant states.
                        storm::dd::Odd<DdType> odd(relevantStates);
                        
                        std::vector<ValueType> result;
                        if (unboundedResult->isHybridQuantitativeCheckResult()) {
                            std::unique_ptr<CheckResult> explicitUnboundedResult = unboundedResult->asHybridQuantitativeCheckResult<DdType>().toExplicitQuantitativeCheckResult();
                            result = std::move(explicitUnboundedResult->asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                        } else {
                            STORM_LOG_THROW(unboundedResult->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidStateException, "Expected check result of different type.");
                            result = unboundedResult->asSymbolicQuantitativeCheckResult<DdType>().getValueVector().template toVector<ValueType>(odd);
                        }
                        
                        // Determine the uniformization rate for the transient probability computation.
                        ValueType uniformizationRate = 1.02 * (relevantStates.toAdd() * exitRates).getMax();
                        
                        // Compute the uniformized matrix.
                        storm::dd::Add<DdType> uniformizedMatrix = this->computeUniformizedMatrix(this->getModel(), this->getModel().getTransitionMatrix(), exitRates,  relevantStates, uniformizationRate);
                        storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                        
                        // Compute the transient probabilities.
                        result = SparseCtmcCslModelChecker<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, result, *this->linearEquationSolverFactory);
                        
                        return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), !relevantStates && this->getModel().getReachableStates(), this->getModel().getManager().getAddZero(), relevantStates, odd, result));
                    } else {
                        // In this case, the interval is of the form [t, t'] with t != 0 and t' != inf.
                        
                        if (lowerBound != upperBound) {
                            // In this case, the interval is of the form [t, t'] with t != 0, t' != inf and t != t'.

                            // Find the maximal rate of all 'maybe' states to take it as the uniformization rate.
                            ValueType uniformizationRate =  1.02 * (statesWithProbabilityGreater0NonPsi.toAdd() * exitRates).getMax();
                            STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                        
                            // Compute the (first) uniformized matrix.
                            storm::dd::Add<DdType> uniformizedMatrix = this->computeUniformizedMatrix(this->getModel(), this->getModel().getTransitionMatrix(), exitRates,  statesWithProbabilityGreater0NonPsi, uniformizationRate);
                            
                            // Create the one-step vector.
                            storm::dd::Add<DdType> b = (statesWithProbabilityGreater0NonPsi.toAdd() * this->getModel().getTransitionMatrix() * psiStates.swapVariables(this->getModel().getRowColumnMetaVariablePairs()).toAdd()).sumAbstract(this->getModel().getColumnVariables()) / this->getModel().getManager().getConstant(uniformizationRate);
                            
                            // Build an ODD for the relevant states and translate the symbolic parts to their explicit representation.
                            storm::dd::Odd<DdType> odd = storm::dd::Odd<DdType>(statesWithProbabilityGreater0NonPsi);
                            storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                            std::vector<ValueType> explicitB = b.template toVector<ValueType>(odd);
                            
                            // Compute the transient probabilities.
                            std::vector<ValueType> values(statesWithProbabilityGreater0NonPsi.getNonZeroCount(), storm::utility::zero<ValueType>());
                            std::vector<ValueType> subResult = SparseCtmcCslModelChecker<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, &explicitB, upperBound - lowerBound, uniformizationRate, values, *this->linearEquationSolverFactory);

                            // Transform the explicit result to a hybrid check result, so we can easily convert it to
                            // a symbolic qualitative format.
                            HybridQuantitativeCheckResult<DdType> hybridResult(this->getModel().getReachableStates(), psiStates || (!statesWithProbabilityGreater0 && this->getModel().getReachableStates()),
                                                                               psiStates.toAdd(), statesWithProbabilityGreater0NonPsi, odd, subResult);
                            
                            // Compute the set of relevant states.
                            storm::dd::Bdd<DdType> relevantStates = statesWithProbabilityGreater0 && phiStates;
                            
                            // Filter the unbounded result such that it only contains values for the relevant states.
                            hybridResult.filter(SymbolicQualitativeCheckResult<DdType>(this->getModel().getReachableStates(), relevantStates));
                            
                            // Build an ODD for the relevant states.
                            odd = storm::dd::Odd<DdType>(relevantStates);
                            
                            std::unique_ptr<CheckResult> explicitResult = hybridResult.toExplicitQuantitativeCheckResult();
                            std::vector<ValueType> newSubresult = std::move(explicitResult->asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                            
                            // Then compute the transient probabilities of being in such a state after t time units. For this,
                            // we must re-uniformize the CTMC, so we need to compute the second uniformized matrix.
                            uniformizationRate =  1.02 * (relevantStates.toAdd() * exitRates).getMax();
                            STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                            
                            // If the lower and upper bounds coincide, we have only determined the relevant states at this
                            // point, but we still need to construct the starting vector.
                            if (lowerBound == upperBound) {
                                odd = storm::dd::Odd<DdType>(relevantStates);
                                newSubresult = psiStates.toAdd().template toVector<ValueType>(odd);
                            }
                            
                            // Finally, we compute the second set of transient probabilities.
                            uniformizedMatrix = this->computeUniformizedMatrix(this->getModel(), this->getModel().getTransitionMatrix(), exitRates,  relevantStates, uniformizationRate);
                            explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                            
                            newSubresult = SparseCtmcCslModelChecker<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult, *this->linearEquationSolverFactory);
                            
                            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), !relevantStates && this->getModel().getReachableStates(), this->getModel().getManager().getAddZero(), relevantStates, odd, newSubresult));
                        } else {
                            // In this case, the interval is of the form [t, t] with t != 0, t != inf.

                            // Build an ODD for the relevant states.
                            storm::dd::Odd<DdType> odd = storm::dd::Odd<DdType>(statesWithProbabilityGreater0);
                            
                            std::vector<ValueType> newSubresult = psiStates.toAdd().template toVector<ValueType>(odd);
                            
                            // Then compute the transient probabilities of being in such a state after t time units. For this,
                            // we must re-uniformize the CTMC, so we need to compute the second uniformized matrix.
                            ValueType uniformizationRate =  1.02 * (statesWithProbabilityGreater0.toAdd() * exitRates).getMax();
                            STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                            
                            // Finally, we compute the second set of transient probabilities.
                            storm::dd::Add<DdType> uniformizedMatrix = this->computeUniformizedMatrix(this->getModel(), this->getModel().getTransitionMatrix(), exitRates,  statesWithProbabilityGreater0, uniformizationRate);
                            storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                            
                            newSubresult = SparseCtmcCslModelChecker<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult, *this->linearEquationSolverFactory);
                            
                            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), !statesWithProbabilityGreater0 && this->getModel().getReachableStates(), this->getModel().getManager().getAddZero(), statesWithProbabilityGreater0, odd, newSubresult));
                        }
                    }
                }
            } else {
                return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), psiStates.toAdd()));
            }
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            return this->computeInstantaneousRewardsHelper(rewardPathFormula.getContinuousTimeBound());
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeInstantaneousRewardsHelper(double timeBound) const {
            // Only compute the result if the model has a state-based reward this->getModel().
            STORM_LOG_THROW(this->getModel().hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Create ODD for the translation.
            storm::dd::Odd<DdType> odd(this->getModel().getReachableStates());
            
            // Initialize result to state rewards of the this->getModel().
            std::vector<ValueType> result = this->getModel().getStateRewardVector().template toVector<ValueType>(odd);
            
            // If the time-bound is not zero, we need to perform a transient analysis.
            if (timeBound > 0) {
                ValueType uniformizationRate = 1.02 * this->getModel().getExitRateVector().getMax();
                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                
                storm::dd::Add<DdType> uniformizedMatrix = this->computeUniformizedMatrix(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(),  this->getModel().getReachableStates(), uniformizationRate);
                
                storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                result = SparseCtmcCslModelChecker<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, nullptr, timeBound, uniformizationRate, result, *this->linearEquationSolverFactory);
            }
            
            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), this->getModel().getManager().getBddZero(), this->getModel().getManager().getAddZero(), this->getModel().getReachableStates(), odd, result));
        }

        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            return this->computeCumulativeRewardsHelper(rewardPathFormula.getContinuousTimeBound());
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeCumulativeRewardsHelper(double timeBound) const {
            // Only compute the result if the model has a state-based reward this->getModel().
            STORM_LOG_THROW(this->getModel().hasStateRewards() || this->getModel().hasTransitionRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // If the time bound is zero, the result is the constant zero vector.
            if (timeBound == 0) {
                return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), this->getModel().getManager().getAddZero()));
            }
            
            // Otherwise, we need to perform some computations.
            
            // Start with the uniformization.
            ValueType uniformizationRate = 1.02 * this->getModel().getExitRateVector().getMax();
            STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
            
            // Create ODD for the translation.
            storm::dd::Odd<DdType> odd(this->getModel().getReachableStates());
            
            // Compute the uniformized matrix.
            storm::dd::Add<DdType> uniformizedMatrix = this->computeUniformizedMatrix(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(),  this->getModel().getReachableStates(), uniformizationRate);
            storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);

            // Then compute the state reward vector to use in the computation.
            storm::dd::Add<DdType> totalRewardVector = this->getModel().hasStateRewards() ? this->getModel().getStateRewardVector() : this->getModel().getManager().getAddZero();
            if (this->getModel().hasTransitionRewards()) {
                totalRewardVector += (this->getModel().getTransitionMatrix() * this->getModel().getTransitionRewardMatrix()).sumAbstract(this->getModel().getColumnVariables());
            }
            std::vector<ValueType> explicitTotalRewardVector = totalRewardVector.template toVector<ValueType>(odd);
            
            // Finally, compute the transient probabilities.
            std::vector<ValueType> result = SparseCtmcCslModelChecker<ValueType>::template computeTransientProbabilities<true>(explicitUniformizedMatrix, nullptr, timeBound, uniformizationRate, explicitTotalRewardVector, *this->linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), this->getModel().getManager().getBddZero(), this->getModel().getManager().getAddZero(), this->getModel().getReachableStates(), std::move(odd), std::move(result)));
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            storm::dd::Add<DdType> probabilityMatrix = this->computeProbabilityMatrix(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector());
            
            // Create ODD for the translation.
            storm::dd::Odd<DdType> odd(this->getModel().getReachableStates());
            
            storm::storage::SparseMatrix<ValueType> explicitProbabilityMatrix = probabilityMatrix.toMatrix(odd, odd);
            std::vector<ValueType> explicitExitRateVector = this->getModel().getExitRateVector().template toVector<ValueType>(odd);
            
            std::vector<ValueType> result = SparseCtmcCslModelChecker<ValueType>::computeLongRunAverageHelper(explicitProbabilityMatrix, subResult.getTruthValuesVector().toVector(odd), &explicitExitRateVector, qualitative, *this->linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), this->getModel().getManager().getBddZero(), this->getModel().getManager().getAddZero(), this->getModel().getReachableStates(), std::move(odd), std::move(result)));
        }
        
        // Explicitly instantiate the model checker.
        template class HybridCtmcCslModelChecker<storm::dd::DdType::CUDD, double>;
        
    } // namespace modelchecker
} // namespace storm