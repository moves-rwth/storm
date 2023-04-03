#include "SparseDeterministicInfiniteHorizonHelper.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/modelchecker/helper/indefinitehorizon/visitingtimes/SparseDeterministicVisitingTimesHelper.h"
#include "storm/modelchecker/helper/infinitehorizon/internal/ComponentUtility.h"
#include "storm/modelchecker/helper/infinitehorizon/internal/LraViHelper.h"
#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"

#include "storm/storage/Scheduler.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"

#include "storm/solver/LinearEquationSolver.h"

#include "storm/utility/SignalHandler.h"
#include "storm/utility/solver.h"
#include "storm/utility/vector.h"

#include "storm/environment/solver/LongRunAverageSolverEnvironment.h"
#include "storm/environment/solver/TopologicalSolverEnvironment.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnmetRequirementException.h"

namespace storm {
namespace modelchecker {
namespace helper {

template<typename ValueType>
SparseDeterministicInfiniteHorizonHelper<ValueType>::SparseDeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix)
    : SparseInfiniteHorizonHelper<ValueType, false>(transitionMatrix) {
    // Intentionally left empty.
}

template<typename ValueType>
SparseDeterministicInfiniteHorizonHelper<ValueType>::SparseDeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                                              std::vector<ValueType> const& exitRates)
    : SparseInfiniteHorizonHelper<ValueType, false>(transitionMatrix, exitRates) {
    // For the CTMC case we assert that the caller actually provided the probabilistic transitions
    STORM_LOG_ASSERT(this->_transitionMatrix.isProbabilistic(), "Non-probabilistic transitions");
}

template<typename ValueType>
void SparseDeterministicInfiniteHorizonHelper<ValueType>::createDecomposition() {
    if (this->_longRunComponentDecomposition == nullptr) {
        // The decomposition has not been provided or computed, yet.
        this->_computedLongRunComponentDecomposition = std::make_unique<storm::storage::StronglyConnectedComponentDecomposition<ValueType>>(
            this->_transitionMatrix, storm::storage::StronglyConnectedComponentDecompositionOptions().onlyBottomSccs());
        this->_longRunComponentDecomposition = this->_computedLongRunComponentDecomposition.get();
    }
}

template<typename ValueType>
ValueType SparseDeterministicInfiniteHorizonHelper<ValueType>::computeLraForComponent(Environment const& env, ValueGetter const& stateValueGetter,
                                                                                      ValueGetter const& actionValueGetter,
                                                                                      storm::storage::StronglyConnectedComponent const& component) {
    // For deterministic models, we compute the LRA for a BSCC

    STORM_LOG_ASSERT(!this->isProduceSchedulerSet(), "Scheduler production enabled for deterministic model.");

    auto trivialResult = computeLraForTrivialBscc(env, stateValueGetter, actionValueGetter, component);
    if (trivialResult.first) {
        return trivialResult.second;
    }

    // Solve nontrivial BSCC with the method specified  in the settings
    storm::solver::LraMethod method = env.solver().lra().getDetLraMethod();
    if ((storm::NumberTraits<ValueType>::IsExact || env.solver().isForceExact()) && env.solver().lra().isDetLraMethodSetFromDefault() &&
        method == storm::solver::LraMethod::ValueIteration) {
        method = storm::solver::LraMethod::GainBiasEquations;
        STORM_LOG_INFO("Selecting " << storm::solver::toString(method)
                                    << " as the solution technique for long-run properties to guarantee exact results. If you want to override this, please "
                                       "explicitly specify a different LRA method.");
    } else if (env.solver().isForceSoundness() && env.solver().lra().isDetLraMethodSetFromDefault() && method != storm::solver::LraMethod::ValueIteration) {
        method = storm::solver::LraMethod::ValueIteration;
        STORM_LOG_INFO("Selecting " << storm::solver::toString(method)
                                    << " as the solution technique for long-run properties to guarantee sound results. If you want to override this, please "
                                       "explicitly specify a different LRA method.");
    }
    STORM_LOG_TRACE("Computing LRA for BSCC of size " << component.size() << " using '" << storm::solver::toString(method) << "'.");
    if (method == storm::solver::LraMethod::ValueIteration) {
        return computeLraForBsccVi(env, stateValueGetter, actionValueGetter, component);
    } else if (method == storm::solver::LraMethod::LraDistributionEquations) {
        // We only need the first element of the pair as the lra distribution is not relevant at this point.
        return computeLraForBsccSteadyStateDistr(env, stateValueGetter, actionValueGetter, component).first;
    }
    STORM_LOG_WARN_COND(method == storm::solver::LraMethod::GainBiasEquations,
                        "Unsupported lra method selected. Defaulting to " << storm::solver::toString(storm::solver::LraMethod::GainBiasEquations) << ".");
    // We don't need the bias values
    return computeLraForBsccGainBias(env, stateValueGetter, actionValueGetter, component).first;
}

template<typename ValueType>
std::pair<bool, ValueType> SparseDeterministicInfiniteHorizonHelper<ValueType>::computeLraForTrivialBscc(
    Environment const& env, ValueGetter const& stateValueGetter, ValueGetter const& actionValueGetter,
    storm::storage::StronglyConnectedComponent const& component) {
    // For deterministic models, we can catch the case where all values are the same. This includes the special case where the BSCC consist only of just one
    // state.
    bool first = true;
    ValueType val = storm::utility::zero<ValueType>();
    for (auto const& element : component) {
        auto state = internal::getComponentElementState(element);
        STORM_LOG_ASSERT(state == *internal::getComponentElementChoicesBegin(element),
                         "Unexpected choice index at state " << state << " of deterministic model.");
        ValueType curr =
            stateValueGetter(state) + (this->isContinuousTime() ? (*this->_exitRates)[state] * actionValueGetter(state) : actionValueGetter(state));
        if (first) {
            val = curr;
            first = false;
        } else if (val != curr) {
            return {false, storm::utility::zero<ValueType>()};
        }
    }
    // All values are the same
    return {true, val};
}

template<>
storm::RationalFunction SparseDeterministicInfiniteHorizonHelper<storm::RationalFunction>::computeLraForBsccVi(
    Environment const& env, ValueGetter const& stateValueGetter, ValueGetter const& actionValueGetter, storm::storage::StronglyConnectedComponent const& bscc) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The requested Method for LRA computation is not supported for parametric models.");
}
template<typename ValueType>
ValueType SparseDeterministicInfiniteHorizonHelper<ValueType>::computeLraForBsccVi(Environment const& env, ValueGetter const& stateValueGetter,
                                                                                   ValueGetter const& actionValueGetter,
                                                                                   storm::storage::StronglyConnectedComponent const& bscc) {
    // Collect parameters of the computation
    ValueType aperiodicFactor = storm::utility::convertNumber<ValueType>(env.solver().lra().getAperiodicFactor());

    // Now create a helper and perform the algorithm
    if (this->isContinuousTime()) {
        // We assume a CTMC (with deterministic timed states and no instant states)
        storm::modelchecker::helper::internal::LraViHelper<ValueType, storm::storage::StronglyConnectedComponent,
                                                           storm::modelchecker::helper::internal::LraViTransitionsType::DetTsNoIs>
            viHelper(bscc, this->_transitionMatrix, aperiodicFactor, this->_markovianStates, this->_exitRates);
        return viHelper.performValueIteration(env, stateValueGetter, actionValueGetter, this->_exitRates);
    } else {
        // We assume a DTMC (with deterministic timed states and no instant states)
        storm::modelchecker::helper::internal::LraViHelper<ValueType, storm::storage::StronglyConnectedComponent,
                                                           storm::modelchecker::helper::internal::LraViTransitionsType::DetTsNoIs>
            viHelper(bscc, this->_transitionMatrix, aperiodicFactor);
        return viHelper.performValueIteration(env, stateValueGetter, actionValueGetter);
    }
}

template<typename ValueType>
std::pair<ValueType, std::vector<ValueType>> SparseDeterministicInfiniteHorizonHelper<ValueType>::computeLraForBsccGainBias(
    Environment const& env, ValueGetter const& stateValuesGetter, ValueGetter const& actionValuesGetter,
    storm::storage::StronglyConnectedComponent const& bscc) {
    // we want that the returned vector is sorted as the bscc. So let's assert that the bscc is sorted ascendingly.
    STORM_LOG_ASSERT(std::is_sorted(bscc.begin(), bscc.end()), "Expected that bsccs are sorted.");

    // We build the equation system as in Line 3 of Algorithm 3 from
    // Kretinsky, Meggendorfer: Efficient Strategy Iteration for Mean Payoff in Markov Decision Processes (ATVA 2017)
    // https://doi.org/10.1007/978-3-319-68167-2_25
    // The first variable corresponds to the gain of the bscc whereas the subsequent variables yield the bias for each state s_1, s_2, ....
    // No bias variable for s_0 is needed since it is always set to zero, yielding an nxn equation system matrix
    // To make this work for CTMC, we could uniformize the model. This preserves LRA and ensures that we can compute the
    // LRA as for a DTMC (the soujourn time in each state is the same). If we then multiply the equations with the uniformization rate,
    // the uniformization rate cancels out. Hence, we obtain the equation system below.

    // Get a mapping from global state indices to local ones.
    std::unordered_map<uint64_t, uint64_t> toLocalIndexMap;
    uint64_t localIndex = 0;
    for (auto const& globalIndex : bscc) {
        toLocalIndexMap[globalIndex] = localIndex;
        ++localIndex;
    }

    // Prepare an environment for the underlying equation solver
    auto subEnv = env;
    if (subEnv.solver().getLinearEquationSolverType() == storm::solver::EquationSolverType::Topological) {
        // Topological solver does not make any sense since the BSCC is connected.
        subEnv.solver().setLinearEquationSolverType(subEnv.solver().topological().getUnderlyingEquationSolverType(),
                                                    subEnv.solver().topological().isUnderlyingEquationSolverTypeSetFromDefault());
    }
    subEnv.solver().setLinearEquationSolverPrecision(env.solver().lra().getPrecision(), env.solver().lra().getRelativeTerminationCriterion());

    // Build the equation system matrix and vector.
    storm::solver::GeneralLinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
    bool isEquationSystemFormat =
        linearEquationSolverFactory.getEquationProblemFormat(subEnv) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem;
    storm::storage::SparseMatrixBuilder<ValueType> builder(bscc.size(), bscc.size());
    std::vector<ValueType> eqSysVector;
    eqSysVector.reserve(bscc.size());
    // The first row asserts that the weighted bias variables and the reward at s_0 sum up to the gain
    uint64_t row = 0;
    ValueType entryValue;
    for (auto const& globalState : bscc) {
        ValueType rateAtState = this->_exitRates ? (*this->_exitRates)[globalState] : storm::utility::one<ValueType>();
        // Coefficient for the gain variable
        if (isEquationSystemFormat) {
            // '1-0' in row 0 and -(-1) in other rows
            builder.addNextValue(row, 0, storm::utility::one<ValueType>());
        } else if (row > 0) {
            // No coeficient in row 0, othwerise substract the gain
            builder.addNextValue(row, 0, -storm::utility::one<ValueType>());
        }
        // Compute weighted sum over successor state. As this is a BSCC, each successor state will again be in the BSCC.
        if (row > 0) {
            if (isEquationSystemFormat) {
                builder.addDiagonalEntry(row, rateAtState);
            } else if (!storm::utility::isOne(rateAtState)) {
                builder.addDiagonalEntry(row, storm::utility::one<ValueType>() - rateAtState);
            }
        }
        for (auto const& entry : this->_transitionMatrix.getRow(globalState)) {
            uint64_t col = toLocalIndexMap[entry.getColumn()];
            if (col == 0) {
                // Skip transition to state_0. This corresponds to setting the bias of state_0 to zero
                continue;
            }
            entryValue = entry.getValue() * rateAtState;
            if (isEquationSystemFormat) {
                entryValue = -entryValue;
            }
            builder.addNextValue(row, col, entryValue);
        }
        eqSysVector.push_back(stateValuesGetter(globalState) + rateAtState * actionValuesGetter(globalState));
        ++row;
    }

    // Create a linear equation solver
    auto solver = linearEquationSolverFactory.create(subEnv, builder.build());
    // Check solver requirements.
    auto requirements = solver->getRequirements(subEnv);
    STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UnmetRequirementException,
                    "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
    // Todo: Find bounds on the bias variables. Just inserting the maximal value from the vector probably does not work.

    std::vector<ValueType> eqSysSol(bscc.size(), storm::utility::zero<ValueType>());
    // Take the mean of the rewards as an initial guess for the gain
    // eqSysSol.front() = std::accumulate(eqSysVector.begin(), eqSysVector.end(), storm::utility::zero<ValueType>()) / storm::utility::convertNumber<ValueType,
    // uint64_t>(bscc.size());
    solver->solveEquations(subEnv, eqSysSol, eqSysVector);

    ValueType gain = eqSysSol.front();
    // insert bias value for state 0
    eqSysSol.front() = storm::utility::zero<ValueType>();
    // Return the gain and the bias values
    return std::pair<ValueType, std::vector<ValueType>>(std::move(gain), std::move(eqSysSol));
}

template<typename ValueType>
std::vector<ValueType> SparseDeterministicInfiniteHorizonHelper<ValueType>::computeSteadyStateDistrForBscc(
    Environment const& env, storm::storage::StronglyConnectedComponent const& bscc) {
    // We want that the returned values are sorted properly. Let's assert that strongly connected components use a sorted container type
    STORM_LOG_ASSERT(std::is_sorted(bscc.begin(), bscc.end()), "Expected that bsccs are sorted.");

    // Let A be ab auxiliary Matrix with A[s,s] =  R(s,s) - r(s) & A[s,s'] = R(s,s') for s,s' in BSCC and s!=s'.
    // We build and solve the equation system for
    // x*A=0 &  x_0+...+x_n=1  <=>  A^t*x=0=x-x & x_0+...+x_n=1  <=> (1+A^t)*x = x & 1-x_0-...-x_n-1=x_n
    // Then, x[i] will be the fraction of the time we are in state i.

    // This method assumes that this BSCC consist of more than one state.
    // We therefore catch the (easy) case where the BSCC is a singleton.
    if (bscc.size() == 1) {
        return {storm::utility::one<ValueType>()};
    }

    // Prepare an environment for the underlying linear equation solver
    auto subEnv = env;
    if (subEnv.solver().getLinearEquationSolverType() == storm::solver::EquationSolverType::Topological) {
        // Topological solver does not make any sense since the BSCC is connected.
        subEnv.solver().setLinearEquationSolverType(subEnv.solver().topological().getUnderlyingEquationSolverType(),
                                                    subEnv.solver().topological().isUnderlyingEquationSolverTypeSetFromDefault());
    }
    subEnv.solver().setLinearEquationSolverPrecision(env.solver().lra().getPrecision(), env.solver().lra().getRelativeTerminationCriterion());
    STORM_LOG_WARN_COND(!subEnv.solver().isForceSoundness(),
                        "Sound computations are not properly implemented for this computation. You might get incorrect results.");

    // Get a mapping from global state indices to local ones as well as a bitvector containing states within the BSCC.
    std::unordered_map<uint64_t, uint64_t> toLocalIndexMap;
    storm::storage::BitVector bsccStates(this->_transitionMatrix.getRowCount(), false);
    uint64_t localIndex = 0;
    for (auto const& globalIndex : bscc) {
        bsccStates.set(globalIndex, true);
        toLocalIndexMap[globalIndex] = localIndex;
        ++localIndex;
    }

    // Build the auxiliary Matrix A.
    auto auxMatrix = this->_transitionMatrix.getSubmatrix(false, bsccStates, bsccStates, true);  // add diagonal entries!
    uint64_t row = 0;
    for (auto const& globalIndex : bscc) {
        ValueType rateAtState = this->_exitRates ? (*this->_exitRates)[globalIndex] : storm::utility::one<ValueType>();
        for (auto& entry : auxMatrix.getRow(row)) {
            if (entry.getColumn() == row) {
                // This value is non-zero since we have a BSCC with more than one state
                entry.setValue(rateAtState * (entry.getValue() - storm::utility::one<ValueType>()));
            } else if (this->isContinuousTime()) {
                entry.setValue(entry.getValue() * rateAtState);
            }
        }
        ++row;
    }
    assert(row == auxMatrix.getRowCount());

    // We need to consider A^t. This will not delete diagonal entries since they are non-zero.
    auxMatrix = auxMatrix.transpose();

    // Check whether we need the fixpoint characterization
    storm::solver::GeneralLinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
    bool isFixpointFormat = linearEquationSolverFactory.getEquationProblemFormat(subEnv) == storm::solver::LinearEquationSolverProblemFormat::FixedPointSystem;
    if (isFixpointFormat) {
        // Add a 1 on the diagonal
        for (row = 0; row < auxMatrix.getRowCount(); ++row) {
            for (auto& entry : auxMatrix.getRow(row)) {
                if (entry.getColumn() == row) {
                    entry.setValue(storm::utility::one<ValueType>() + entry.getValue());
                }
            }
        }
    }

    // We now build the equation system matrix.
    // We can drop the last row of A and add ones in this row instead to assert that the variables sum up to one
    // Phase 1: replace the existing entries of the last row with ones
    uint64_t col = 0;
    uint64_t lastRow = auxMatrix.getRowCount() - 1;
    for (auto& entry : auxMatrix.getRow(lastRow)) {
        entry.setColumn(col);
        if (isFixpointFormat) {
            if (col == lastRow) {
                entry.setValue(storm::utility::zero<ValueType>());
            } else {
                entry.setValue(-storm::utility::one<ValueType>());
            }
        } else {
            entry.setValue(storm::utility::one<ValueType>());
        }
        ++col;
    }
    storm::storage::SparseMatrixBuilder<ValueType> builder(std::move(auxMatrix));
    for (; col <= lastRow; ++col) {
        if (isFixpointFormat) {
            if (col != lastRow) {
                builder.addNextValue(lastRow, col, -storm::utility::one<ValueType>());
            }
        } else {
            builder.addNextValue(lastRow, col, storm::utility::one<ValueType>());
        }
    }

    std::vector<ValueType> bsccEquationSystemRightSide(bscc.size(), storm::utility::zero<ValueType>());
    bsccEquationSystemRightSide.back() = storm::utility::one<ValueType>();

    // Create a linear equation solver
    auto solver = linearEquationSolverFactory.create(subEnv, builder.build());
    solver->setBounds(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
    // Check solver requirements.
    auto requirements = solver->getRequirements(subEnv);
    requirements.clearLowerBounds();
    requirements.clearUpperBounds();
    STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UnmetRequirementException,
                    "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");

    std::vector<ValueType> steadyStateDistr(bscc.size(), storm::utility::one<ValueType>() / storm::utility::convertNumber<ValueType, uint64_t>(bscc.size()));
    solver->solveEquations(subEnv, steadyStateDistr, bsccEquationSystemRightSide);

    // As a last step, we normalize these values to counter numerical inaccuracies a bit.
    // This is only reasonable in non-exact mode.
    if (!subEnv.solver().isForceExact()) {
        ValueType sum = std::accumulate(steadyStateDistr.begin(), steadyStateDistr.end(), storm::utility::zero<ValueType>());
        storm::utility::vector::scaleVectorInPlace<ValueType, ValueType>(steadyStateDistr, storm::utility::one<ValueType>() / sum);
    }

    return steadyStateDistr;
}

template<typename ValueType>
std::pair<ValueType, std::vector<ValueType>> SparseDeterministicInfiniteHorizonHelper<ValueType>::computeLraForBsccSteadyStateDistr(
    Environment const& env, ValueGetter const& stateValuesGetter, ValueGetter const& actionValuesGetter,
    storm::storage::StronglyConnectedComponent const& bscc) {
    // Get the LRA distribution for the provided bscc.
    auto steadyStateDistr = computeSteadyStateDistrForBscc(env, bscc);

    // Calculate final LRA Value
    ValueType result = storm::utility::zero<ValueType>();
    auto solIt = steadyStateDistr.begin();
    for (auto const& globalState : bscc) {
        if (this->isContinuousTime()) {
            result += (*solIt) * (stateValuesGetter(globalState) + (*this->_exitRates)[globalState] * actionValuesGetter(globalState));
        } else {
            result += (*solIt) * (stateValuesGetter(globalState) + actionValuesGetter(globalState));
        }
        ++solIt;
    }
    assert(solIt == steadyStateDistr.end());

    return std::pair<ValueType, std::vector<ValueType>>(std::move(result), std::move(steadyStateDistr));
}

template<typename ValueType>
std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> SparseDeterministicInfiniteHorizonHelper<ValueType>::buildSspMatrixVector(
    std::vector<ValueType> const& bsccLraValues, std::vector<uint64_t> const& inputStateToBsccIndexMap, storm::storage::BitVector const& statesNotInComponent,
    bool asEquationSystem) {
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
    for (auto state : statesNotInComponent) {
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

template<typename ValueType>
std::vector<ValueType> SparseDeterministicInfiniteHorizonHelper<ValueType>::buildAndSolveSsp(Environment const& env,
                                                                                             std::vector<ValueType> const& componentLraValues) {
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
    for (auto nonComponentState : statesNotInComponent) {
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
        requirements.clearUpperBounds();
        requirements.clearLowerBounds();
        STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UnmetRequirementException,
                        "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
        sspValues.assign(sspMatrixVector.first.getRowCount(),
                         (*lowerUpperBounds.first + *lowerUpperBounds.second) / storm::utility::convertNumber<ValueType, uint64_t>(2));
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

template<typename ValueType>
std::vector<ValueType> SparseDeterministicInfiniteHorizonHelper<ValueType>::computeLongRunAverageStateDistribution(Environment const& env) {
    createDecomposition();
    STORM_LOG_THROW(this->_longRunComponentDecomposition->size() <= 1, storm::exceptions::InvalidOperationException, "");
    return computeLongRunAverageStateDistribution(env, [](uint64_t) { return storm::utility::zero<ValueType>(); });
}

template<typename ValueType>
std::vector<ValueType> SparseDeterministicInfiniteHorizonHelper<ValueType>::computeLongRunAverageStateDistribution(Environment const& env,
                                                                                                                   uint64_t const& initialState) {
    STORM_LOG_ASSERT(initialState < this->_transitionMatrix.getRowGroupCount(),
                     "Invlid initial state index: " << initialState << ". Have only " << this->_transitionMatrix.getRowGroupCount() << " states.");
    return computeLongRunAverageStateDistribution(env, [&initialState](uint64_t stateIndex) {
        return initialState == stateIndex ? storm::utility::one<ValueType>() : storm::utility::zero<ValueType>();
    });
}

template<typename ValueType>
std::vector<ValueType> SparseDeterministicInfiniteHorizonHelper<ValueType>::computeLongRunAverageStateDistribution(
    Environment const& env, ValueGetter const& initialDistributionGetter) {
    createDecomposition();

    // Compute for each BSCC get the probability with which we reach that BSCC
    auto bsccReachProbs = computeBsccReachabilityProbabilities(env, initialDistributionGetter);
    // We are now ready to compute the resulting lra distribution
    std::vector<ValueType> steadyStateDistr(this->_transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
    for (uint64_t currentComponentIndex = 0; currentComponentIndex < this->_longRunComponentDecomposition->size(); ++currentComponentIndex) {
        auto const& component = (*this->_longRunComponentDecomposition)[currentComponentIndex];
        // Compute distribution for current bscc
        auto bsccDistr = this->computeSteadyStateDistrForBscc(env, component);
        // Scale with probability to reach that bscc
        auto const& scalingFactor = bsccReachProbs[currentComponentIndex];
        if (!storm::utility::isOne(scalingFactor)) {
            storm::utility::vector::scaleVectorInPlace(bsccDistr, scalingFactor);
        }
        // Set the values in the result vector
        auto bsccDistrIt = bsccDistr.begin();
        for (auto const& element : component) {
            uint64_t state = internal::getComponentElementState(element);
            steadyStateDistr[state] = *bsccDistrIt;
            ++bsccDistrIt;
        }
        STORM_LOG_ASSERT(bsccDistrIt == bsccDistr.end(), "Unexpected number of entries in bscc distribution");
    }
    return steadyStateDistr;
}

template<typename ValueType>
std::vector<ValueType> computeUpperBoundsForExpectedVisitingTimes(storm::storage::SparseMatrix<ValueType> const& nonBsccMatrix,
                                                                  std::vector<ValueType> const& toBsccProbabilities) {
    return storm::modelchecker::helper::BaierUpperRewardBoundsComputer<ValueType>::computeUpperBoundOnExpectedVisitingTimes(nonBsccMatrix, toBsccProbabilities);
}

template<>
std::vector<storm::RationalFunction> computeUpperBoundsForExpectedVisitingTimes(storm::storage::SparseMatrix<storm::RationalFunction> const& nonBsccMatrix,
                                                                                std::vector<storm::RationalFunction> const& toBsccProbabilities) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                    "Computing upper bounds for expected visiting times over rational functions is not supported.");
}

template<typename ValueType>
std::vector<ValueType> SparseDeterministicInfiniteHorizonHelper<ValueType>::computeBsccReachabilityProbabilities(Environment const& env,
                                                                                                                 ValueGetter const& initialDistributionGetter) {
    STORM_LOG_ASSERT(this->_longRunComponentDecomposition != nullptr, "Decomposition not computed, yet.");
    uint64_t numBSCCs = this->_longRunComponentDecomposition->size();
    STORM_LOG_ASSERT(numBSCCs > 0, "Found 0 BSCCs in a Markov chain. This should not be possible.");

    // Compute for each BSCC get the probability with which we reach that BSCC
    std::vector<ValueType> bsccReachProbs(numBSCCs, storm::utility::zero<ValueType>());
    if (numBSCCs == 1) {
        bsccReachProbs.front() = storm::utility::one<ValueType>();
    } else {
        // Get the expected number of times we visit each non-BSCC state
        // We deliberately exclude the exit rates here as we want to make this computation on the induced DTMC to get the expected number of times
        // that a successor state is chosen probabilistically.
        auto visittimesHelper = SparseDeterministicVisitingTimesHelper<ValueType>(this->_transitionMatrix);
        this->createBackwardTransitions();
        visittimesHelper.provideBackwardTransitions(*this->_backwardTransitions);
        auto expVisitTimes = visittimesHelper.computeExpectedVisitingTimes(env, initialDistributionGetter);
        // Then use the expected visiting times to compute BSCC reachability probabilities
        storm::storage::BitVector nonBsccStates(this->_transitionMatrix.getRowCount(), true);
        for (uint64_t currentComponentIndex = 0; currentComponentIndex < this->_longRunComponentDecomposition->size(); ++currentComponentIndex) {
            for (auto const& element : (*this->_longRunComponentDecomposition)[currentComponentIndex]) {
                nonBsccStates.set(internal::getComponentElementState(element), false);
            }
        }
        for (uint64_t currentComponentIndex = 0; currentComponentIndex < this->_longRunComponentDecomposition->size(); ++currentComponentIndex) {
            auto& bsccVal = bsccReachProbs[currentComponentIndex];
            for (auto const& element : (*this->_longRunComponentDecomposition)[currentComponentIndex]) {
                uint64_t state = internal::getComponentElementState(element);
                bsccVal += initialDistributionGetter(state);
                for (auto const& pred : this->_backwardTransitions->getRow(state)) {
                    if (nonBsccStates.get(pred.getColumn())) {
                        bsccVal += pred.getValue() * expVisitTimes[pred.getColumn()];
                    }
                }
            }
        }
    }

    // As a last step, we normalize these values to counter numerical inaccuracies a bit.
    // This is only reasonable in non-exact mode.
    if (!env.solver().isForceExact()) {
        ValueType sum = std::accumulate(bsccReachProbs.begin(), bsccReachProbs.end(), storm::utility::zero<ValueType>());
        storm::utility::vector::scaleVectorInPlace<ValueType, ValueType>(bsccReachProbs, storm::utility::one<ValueType>() / sum);
    }
    return bsccReachProbs;
}

template class SparseDeterministicInfiniteHorizonHelper<double>;
template class SparseDeterministicInfiniteHorizonHelper<storm::RationalNumber>;
template class SparseDeterministicInfiniteHorizonHelper<storm::RationalFunction>;

}  // namespace helper
}  // namespace modelchecker
}  // namespace storm