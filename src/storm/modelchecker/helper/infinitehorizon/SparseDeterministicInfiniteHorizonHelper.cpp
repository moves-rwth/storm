#include "SparseDeterministicInfiniteHorizonHelper.h"

#include "storm/modelchecker/helper/infinitehorizon/internal/ComponentUtility.h"
#include "storm/modelchecker/helper/infinitehorizon/internal/LraViHelper.h"


#include "storm/storage/SparseMatrix.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/Scheduler.h"

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/Multiplier.h"
#include "storm/solver/LpSolver.h"

#include "storm/utility/SignalHandler.h"
#include "storm/utility/solver.h"
#include "storm/utility/vector.h"

#include "storm/environment/solver/LongRunAverageSolverEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/exceptions/UnmetRequirementException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
        
            template <typename ValueType>
            SparseDeterministicInfiniteHorizonHelper<ValueType>::SparseDeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix) : SparseInfiniteHorizonHelper<ValueType, false>(transitionMatrix) {
                // Intentionally left empty.
            }
            
            template <typename ValueType>
            SparseDeterministicInfiniteHorizonHelper<ValueType>::SparseDeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates) : SparseInfiniteHorizonHelper<ValueType, false>(transitionMatrix, exitRates) {
                // For the CTMC case we assert that the caller actually provided the probabilistic transitions
                STORM_LOG_ASSERT(this->_transitionMatrix.isProbabilistic(), "Non-probabilistic transitions");
            }
            
            template <typename ValueType>
            void SparseDeterministicInfiniteHorizonHelper<ValueType>::createDecomposition() {
                if (this->_longRunComponentDecomposition == nullptr) {
                    // The decomposition has not been provided or computed, yet.
                    this->_computedLongRunComponentDecomposition = std::make_unique<storm::storage::StronglyConnectedComponentDecomposition<ValueType>>(this->_transitionMatrix, storm::storage::StronglyConnectedComponentDecompositionOptions().onlyBottomSccs());
                    this->_longRunComponentDecomposition = this->_computedLongRunComponentDecomposition.get();
                }
            }

            template <typename ValueType>
            ValueType SparseDeterministicInfiniteHorizonHelper<ValueType>::computeLraForComponent(Environment const& env, ValueGetter const& stateRewardsGetter, ValueGetter const& actionRewardsGetter, storm::storage::StronglyConnectedComponent const& component) {
                // For deterministic models, we compute the LRA for a BSCC
                
                STORM_LOG_ASSERT(!this->isProduceSchedulerSet(), "Scheduler production enabled for deterministic model.");
                
                auto trivialResult = computeLraForTrivialBscc(env, stateRewardsGetter, actionRewardsGetter, component);
                if (trivialResult.first) {
                    return trivialResult.second;
                }
                
                // Solve nontrivial BSCC with the method specified  in the settings
                storm::solver::LraMethod method = env.solver().lra().getDetLraMethod();
                if ((storm::NumberTraits<ValueType>::IsExact || env.solver().isForceExact()) && env.solver().lra().isDetLraMethodSetFromDefault() && method == storm::solver::LraMethod::ValueIteration) {
                    method = storm::solver::LraMethod::GainBiasEquations;
                    STORM_LOG_INFO("Selecting " << storm::solver::toString(method) << " as the solution technique for long-run properties to guarantee exact results. If you want to override this, please explicitly specify a different LRA method.");
                } else if (env.solver().isForceSoundness() && env.solver().lra().isDetLraMethodSetFromDefault() && method != storm::solver::LraMethod::ValueIteration) {
                    method = storm::solver::LraMethod::ValueIteration;
                    STORM_LOG_INFO("Selecting " << storm::solver::toString(method) << " as the solution technique for long-run properties to guarantee sound results. If you want to override this, please explicitly specify a different LRA method.");
                }
                STORM_LOG_TRACE("Computing LRA for BSCC of size " << component.size() << " using '" << storm::solver::toString(method) << "'.");
                if (method == storm::solver::LraMethod::ValueIteration) {
                    return computeLraForBsccVi(env, stateRewardsGetter, actionRewardsGetter, component);
                }/* else if (method == storm::solver::LraMethod::LraDistributionEquations) {
                    // We only need the first element of the pair as the lra distribution is not relevant at this point.
                    return computeLongRunAveragesForBsccLraDistr<ValueType>(env, bscc, rateMatrix, valueGetter, exitRateVector).first;
                }
                STORM_LOG_WARN_COND(method == storm::solver::LraMethod::GainBiasEquations, "Unsupported lra method selected. Defaulting to " << storm::solver::toString(storm::solver::LraMethod::GainBiasEquations) << ".");
                // We don't need the bias values
                return computeLongRunAveragesForBsccGainBias<ValueType>(env, bscc, rateMatrix, valueGetter, exitRateVector).first;*/
            }
            
            template <typename ValueType>
            std::pair<bool, ValueType> SparseDeterministicInfiniteHorizonHelper<ValueType>::computeLraForTrivialBscc(Environment const& env, ValueGetter const& stateRewardsGetter, ValueGetter const& actionRewardsGetter, storm::storage::StronglyConnectedComponent const& component) {
                
                // For deterministic models, we can catch the case where all values are the same. This includes the special case where the BSCC consist only of just one state.
                bool first = true;
                ValueType val = storm::utility::zero<ValueType>();
                for (auto const& element : component) {
                    auto state = internal::getComponentElementState(element);
                    STORM_LOG_ASSERT(state == *internal::getComponentElementChoicesBegin(element), "Unexpected choice index at state " << state << " of deterministic model.");
                    ValueType curr = stateRewardsGetter(state) + (this->isContinuousTime() ? (*this->_exitRates)[state] * actionRewardsGetter(state) : actionRewardsGetter(state));
                    if (first) {
                        first = false;
                    } else if (val != curr) {
                        return {false, storm::utility::zero<ValueType>()};
                    }
                }
                // All values are the same
                return {true, val};
            }
            
    
            template <typename ValueType>
            ValueType SparseDeterministicInfiniteHorizonHelper<ValueType>::computeLraForBsccVi(Environment const& env, ValueGetter const& stateRewardsGetter, ValueGetter const& actionRewardsGetter, storm::storage::StronglyConnectedComponent const& bscc) {

                // Collect parameters of the computation
                ValueType aperiodicFactor = storm::utility::convertNumber<ValueType>(env.solver().lra().getAperiodicFactor());
                
                // Now create a helper and perform the algorithm
                if (this->isContinuousTime()) {
                    // We assume a CTMC (with deterministic timed states and no instant states)
                    storm::modelchecker::helper::internal::LraViHelper<ValueType, storm::storage::StronglyConnectedComponent, storm::modelchecker::helper::internal::LraViTransitionsType::DetTsNoIs> viHelper(bscc, this->_transitionMatrix, aperiodicFactor, this->_markovianStates, this->_exitRates);
                    return viHelper.performValueIteration(env, stateRewardsGetter, actionRewardsGetter, this->_exitRates);
                } else {
                    // We assume a DTMC (with deterministic timed states and no instant states)
                    storm::modelchecker::helper::internal::LraViHelper<ValueType, storm::storage::StronglyConnectedComponent, storm::modelchecker::helper::internal::LraViTransitionsType::DetTsNoIs> viHelper(bscc, this->_transitionMatrix, aperiodicFactor);
                    return viHelper.performValueIteration(env, stateRewardsGetter, actionRewardsGetter);
                }
            }
            
            template <typename ValueType>
            std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> SparseDeterministicInfiniteHorizonHelper<ValueType>::buildSspMatrixVector(std::vector<ValueType> const& bsccLraValues, std::vector<uint64_t> const& inputStateToBsccIndexMap, storm::storage::BitVector const& statesNotInComponent, bool asEquationSystem) {
                
                // Create SSP Matrix.
                // In contrast to the version for nondeterministic models, we eliminate the auxiliary states representing each BSCC on the fly
                
                // Probability mass that would lead to a BSCC will be considered in the rhs of the equation system
                auto sspMatrix = this->_transitionMatrix.getSubmatrix(false, statesNotInComponent, statesNotInComponent, asEquationSystem);
                if (asEquationSystem) {
                    sspMatrix.convertToEquationSystem();
                }
                
                // Create the SSP right-hand-side
                std::vector<ValueType> rhs;
                rhs.reserve(sspMatrix.getRowCount());
                for (auto const& state : statesNotInComponent) {
                    ValueType stateValue = storm::utility::zero<ValueType>();
                    for (auto const& transition : this->_transitionMatrix.getRow(state)) {
                        if (!statesNotInComponent.get(transition.getColumn())) {
                            // This transition leads to a BSCC!
                            stateValue += transition.getValue() * bsccLraValues[inputStateToBsccIndexMap[transition.getColumn()]];
                        }
                    }
                    rhs.push_back(std::move(stateValue));
                }
                
                return std::make_pair(std::move(sspMatrix), std::move(rhs));
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseDeterministicInfiniteHorizonHelper<ValueType>::buildAndSolveSsp(Environment const& env, std::vector<ValueType> const& componentLraValues) {
                STORM_LOG_ASSERT(this->_longRunComponentDecomposition != nullptr, "Decomposition not computed, yet.");
                
                // For fast transition rewriting, we build a mapping from the input state indices to the state indices of a new transition matrix
                // which redirects all transitions leading to a former BSCC state to a new (imaginary) auxiliary state.
                // Each auxiliary state gets assigned the value of that BSCC and we compute expected rewards (aka stochastic shortest path, SSP) on that new system.
                // For efficiency reasons, we actually build the system where the auxiliary states are already eliminated.
                
                // First gather the states that are part of a component
                // and create a mapping from states that lie in a component to the corresponding component index.
                storm::storage::BitVector statesInComponents(this->_transitionMatrix.getRowGroupCount());
                std::vector<uint64_t> stateIndexMap(this->_transitionMatrix.getRowGroupCount(), std::numeric_limits<uint64_t>::max());
                for (uint64_t currentComponentIndex = 0; currentComponentIndex < this->_longRunComponentDecomposition->size(); ++currentComponentIndex) {
                    for (auto const& element : (*this->_longRunComponentDecomposition)[currentComponentIndex]) {
                        uint64_t state = internal::getComponentElementState(element);
                        statesInComponents.set(state);
                        stateIndexMap[state] = currentComponentIndex;
                    }
                }
                // Map the non-component states to their index in the SSP. Note that the order of these states will be preserved.
                uint64_t numberOfNonComponentStates = 0;
                storm::storage::BitVector statesNotInComponent = ~statesInComponents;
                for (auto const& nonComponentState : statesNotInComponent) {
                    stateIndexMap[nonComponentState] = numberOfNonComponentStates;
                    ++numberOfNonComponentStates;
                }
                
                // The next step is to create the equation system solving the SSP (unless the whole system consists of BSCCs)
                std::vector<ValueType> sspValues;
                if (numberOfNonComponentStates > 0) {
                    storm::solver::GeneralLinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
                    bool isEqSysFormat = linearEquationSolverFactory.getEquationProblemFormat(env) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem;
                    auto sspMatrixVector = buildSspMatrixVector(componentLraValues, stateIndexMap, statesNotInComponent, isEqSysFormat);
                    std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(env, sspMatrixVector.first);
                    auto lowerUpperBounds = std::minmax_element(componentLraValues.begin(), componentLraValues.end());
                    solver->setBounds(*lowerUpperBounds.first, *lowerUpperBounds.second);
                    // Check solver requirements
                    auto requirements = solver->getRequirements(env);
                    STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UnmetRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
                    sspValues.assign(sspMatrixVector.first.getRowCount(), (*lowerUpperBounds.first + *lowerUpperBounds.second) / storm::utility::convertNumber<ValueType,uint64_t>(2));
                    solver->solveEquations(env, sspValues, sspMatrixVector.second);
                }
                
                // Prepare result vector.
                std::vector<ValueType> result(this->_transitionMatrix.getRowGroupCount());
                for (uint64_t state = 0; state < stateIndexMap.size(); ++state) {
                    if (statesNotInComponent.get(state)) {
                        result[state] = sspValues[stateIndexMap[state]];
                    } else {
                        result[state] = componentLraValues[stateIndexMap[state]];
                    }
                }
                return result;
            }
            
            template class SparseDeterministicInfiniteHorizonHelper<double>;
            template class SparseDeterministicInfiniteHorizonHelper<storm::RationalNumber>;
            
        }
    }
}