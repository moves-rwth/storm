#include <functional>
#include <limits>

#include "storm/solver/IterativeMinMaxLinearEquationSolver.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/OviSolverEnvironment.h"

#include "storm/exceptions/InvalidEnvironmentException.h"
#include "storm/exceptions/UnmetRequirementException.h"
#include "storm/solver/helper/GuessingValueIterationHelper.h"
#include "storm/solver/helper/IntervalterationHelper.h"
#include "storm/solver/helper/OptimisticValueIterationHelper.h"
#include "storm/solver/helper/RationalSearchHelper.h"
#include "storm/solver/helper/SchedulerTrackingHelper.h"
#include "storm/solver/helper/SoundValueIterationHelper.h"
#include "storm/solver/helper/ValueIterationHelper.h"
#include "storm/utility/ConstantsComparator.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

namespace storm {
namespace solver {

template<typename ValueType, typename SolutionType>
IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::IterativeMinMaxLinearEquationSolver() : linearEquationSolverFactory(nullptr) {
    STORM_LOG_ASSERT(static_cast<bool>(std::is_same_v<storm::Interval, ValueType>),
                     "Only for interval models");  // This constructor is only meant for intervals where we can not pass a good factory yet.
}

template<typename ValueType, typename SolutionType>
IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::IterativeMinMaxLinearEquationSolver(
    std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory)
    : linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
    // Intentionally left empty
}

template<typename ValueType, typename SolutionType>
IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::IterativeMinMaxLinearEquationSolver(
    storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory)
    : StandardMinMaxLinearEquationSolver<ValueType, SolutionType>(A), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
    // Intentionally left empty.
}

template<typename ValueType, typename SolutionType>
IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::IterativeMinMaxLinearEquationSolver(
    storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory)
    : StandardMinMaxLinearEquationSolver<ValueType, SolutionType>(std::move(A)), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
    // Intentionally left empty.
}

template<typename ValueType, typename SolutionType>
MinMaxMethod IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::getMethod(Environment const& env, bool isExactMode) const {
    // Adjust the method if none was specified and we want exact or sound computations.
    auto method = env.solver().minMax().getMethod();

    if (isExactMode && method != MinMaxMethod::PolicyIteration && method != MinMaxMethod::RationalSearch && method != MinMaxMethod::ViToPi) {
        if (env.solver().minMax().isMethodSetFromDefault()) {
            STORM_LOG_INFO(
                "Selecting 'Policy iteration' as the solution technique to guarantee exact results. If you want to override this, please explicitly specify a "
                "different method.");
            method = MinMaxMethod::PolicyIteration;
        } else {
            STORM_LOG_WARN("The selected solution method " << toString(method) << " does not guarantee exact results.");
        }
    } else if (env.solver().isForceSoundness() && method != MinMaxMethod::SoundValueIteration && method != MinMaxMethod::IntervalIteration &&
               method != MinMaxMethod::PolicyIteration && method != MinMaxMethod::RationalSearch && method != MinMaxMethod::OptimisticValueIteration &&
               method != MinMaxMethod::GuessingValueIteration) {
        if (env.solver().minMax().isMethodSetFromDefault()) {
            method = MinMaxMethod::OptimisticValueIteration;
            STORM_LOG_INFO(
                "Selecting '"
                << toString(method)
                << "' as the solution technique to guarantee sound results. If you want to override this, please explicitly specify a different method.");
        } else {
            STORM_LOG_WARN("The selected solution method does not guarantee sound results.");
        }
    }
    STORM_LOG_THROW(method == MinMaxMethod::ValueIteration || method == MinMaxMethod::PolicyIteration || method == MinMaxMethod::RationalSearch ||
                        method == MinMaxMethod::SoundValueIteration || method == MinMaxMethod::IntervalIteration ||
                        method == MinMaxMethod::OptimisticValueIteration || method == MinMaxMethod::GuessingValueIteration || method == MinMaxMethod::ViToPi,
                    storm::exceptions::InvalidEnvironmentException, "This solver does not support the selected method '" << toString(method) << "'.");
    return method;
}

template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::internalSolveEquations(Environment const& env, OptimizationDirection dir,
                                                                                          std::vector<SolutionType>& x, std::vector<ValueType> const& b) const {
    bool result = false;
    switch (getMethod(env, storm::NumberTraits<ValueType>::IsExact || env.solver().isForceExact())) {
        case MinMaxMethod::ValueIteration:
            result = solveEquationsValueIteration(env, dir, x, b);
            break;
        case MinMaxMethod::OptimisticValueIteration:
            result = solveEquationsOptimisticValueIteration(env, dir, x, b);
            break;
        case MinMaxMethod::GuessingValueIteration:
            result = solveEquationsGuessingValueIteration(env, dir, x, b);
            break;
        case MinMaxMethod::PolicyIteration:
            result = solveEquationsPolicyIteration(env, dir, x, b);
            break;
        case MinMaxMethod::RationalSearch:
            result = solveEquationsRationalSearch(env, dir, x, b);
            break;
        case MinMaxMethod::IntervalIteration:
            result = solveEquationsIntervalIteration(env, dir, x, b);
            break;
        case MinMaxMethod::SoundValueIteration:
            result = solveEquationsSoundValueIteration(env, dir, x, b);
            break;
        case MinMaxMethod::ViToPi:
            result = solveEquationsViToPi(env, dir, x, b);
            break;
        default:
            STORM_LOG_THROW(false, storm::exceptions::InvalidEnvironmentException, "This solver does not implement the selected solution method");
    }

    return result;
}

template<typename ValueType, typename SolutionType>
void IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::setUpViOperator() const {
    if (!viOperator) {
        viOperator = std::make_shared<helper::ValueIterationOperator<ValueType, false, SolutionType>>();
        viOperator->setMatrixBackwards(*this->A);
    }
    if (this->choiceFixedForRowGroup) {
        // Ignore those rows that are not selected
        assert(this->initialScheduler);
        auto callback = [&](uint64_t groupIndex, uint64_t localRowIndex) {
            return this->choiceFixedForRowGroup->get(groupIndex) && this->initialScheduler->at(groupIndex) != localRowIndex;
        };
        viOperator->setIgnoredRows(true, callback);
    }
}

template<typename ValueType, typename SolutionType>
void IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::extractScheduler(std::vector<SolutionType>& x, std::vector<ValueType> const& b,
                                                                                    OptimizationDirection const& dir, bool updateX, bool robust) const {
    // Make sure that storage for scheduler choices is available
    if (!this->schedulerChoices) {
        this->schedulerChoices = std::vector<uint64_t>(x.size(), 0);
    } else {
        this->schedulerChoices->resize(x.size(), 0);
    }
    // Set the correct choices.
    STORM_LOG_WARN_COND(viOperator, "Expected VI operator to be initialized for scheduler extraction. Initializing now, but this is inefficient.");
    if (!viOperator) {
        setUpViOperator();
    }
    storm::solver::helper::SchedulerTrackingHelper<ValueType, SolutionType> schedHelper(viOperator);
    schedHelper.computeScheduler(x, b, dir, *this->schedulerChoices, robust, updateX ? &x : nullptr);
}

template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::solveInducedEquationSystem(
    Environment const& env, std::unique_ptr<LinearEquationSolver<ValueType>>& linearEquationSolver, std::vector<uint64_t> const& scheduler,
    std::vector<SolutionType>& x, std::vector<ValueType>& subB, std::vector<ValueType> const& originalB) const {
    if constexpr (std::is_same_v<ValueType, storm::Interval>) {
        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "We did not implement solving induced equation systems for interval-based models.");
        // Implementing this requires linear equation systems with different value types and solution types (or some appropriate casting)
        return false;
    } else {
        STORM_LOG_ASSERT(subB.size() == x.size(), "Sizes of subB and x do not coincide.");
        STORM_LOG_ASSERT(this->linearEquationSolverFactory != nullptr, "Wrong constructor was called.");

        // Resolve the nondeterminism according to the given scheduler.
        bool convertToEquationSystem = this->linearEquationSolverFactory->getEquationProblemFormat(env) == LinearEquationSolverProblemFormat::EquationSystem;
        storm::storage::SparseMatrix<ValueType> submatrix;

        submatrix = this->A->selectRowsFromRowGroups(scheduler, convertToEquationSystem);
        if (convertToEquationSystem) {
            submatrix.convertToEquationSystem();
        }
        storm::utility::vector::selectVectorValues<ValueType>(subB, scheduler, this->A->getRowGroupIndices(), originalB);

        // Check whether the linear equation solver is already initialized
        if (!linearEquationSolver) {
            // Initialize the equation solver
            linearEquationSolver = this->linearEquationSolverFactory->create(env, std::move(submatrix));
            linearEquationSolver->setBoundsFromOtherSolver(*this);
            linearEquationSolver->setCachingEnabled(true);
        } else {
            // If the equation solver is already initialized, it suffices to update the matrix
            linearEquationSolver->setMatrix(std::move(submatrix));
        }
        // Solve the equation system for the 'DTMC' and return true upon success
        return linearEquationSolver->solveEquations(env, x, subB);
    }
}

template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::solveEquationsPolicyIteration(Environment const& env, OptimizationDirection dir,
                                                                                                 std::vector<SolutionType>& x,
                                                                                                 std::vector<ValueType> const& b) const {
    std::vector<storm::storage::sparse::state_type> scheduler =
        this->hasInitialScheduler() ? this->getInitialScheduler() : std::vector<storm::storage::sparse::state_type>(this->A->getRowGroupCount());
    return performPolicyIteration(env, dir, x, b, std::move(scheduler));
}

template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::performPolicyIteration(
    Environment const& env, OptimizationDirection dir, std::vector<SolutionType>& x, std::vector<ValueType> const& b,
    std::vector<storm::storage::sparse::state_type>&& initialPolicy) const {
    if constexpr (std::is_same_v<ValueType, storm::Interval>) {
        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "We did not implement policy iteration for interval-based models.");
        return false;
    } else {
        std::vector<storm::storage::sparse::state_type> scheduler = std::move(initialPolicy);
        // Get a vector for storing the right-hand side of the inner equation system.
        if (!auxiliaryRowGroupVector) {
            auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
        }
        std::vector<ValueType>& subB = *auxiliaryRowGroupVector;

        // The solver that we will use throughout the procedure.
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver;
        // The linear equation solver should be at least as precise as this solver
        std::unique_ptr<storm::Environment> environmentOfSolverStorage;
        auto precOfSolver = env.solver().getPrecisionOfLinearEquationSolver(env.solver().getLinearEquationSolverType());
        if (!storm::NumberTraits<ValueType>::IsExact) {
            bool changePrecision = precOfSolver.first && precOfSolver.first.get() > env.solver().minMax().getPrecision();
            bool changeRelative = precOfSolver.second && !precOfSolver.second.get() && env.solver().minMax().getRelativeTerminationCriterion();
            if (changePrecision || changeRelative) {
                environmentOfSolverStorage = std::make_unique<storm::Environment>(env);
                boost::optional<storm::RationalNumber> newPrecision;
                boost::optional<bool> newRelative;
                if (changePrecision) {
                    newPrecision = env.solver().minMax().getPrecision();
                }
                if (changeRelative) {
                    newRelative = true;
                }
                environmentOfSolverStorage->solver().setLinearEquationSolverPrecision(newPrecision, newRelative);
            }
        }
        storm::Environment const& environmentOfSolver = environmentOfSolverStorage ? *environmentOfSolverStorage : env;

        SolverStatus status = SolverStatus::InProgress;
        uint64_t iterations = 0;
        this->startMeasureProgress();
        do {
            // Solve the equation system for the 'DTMC'.
            solveInducedEquationSystem(environmentOfSolver, solver, scheduler, x, subB, b);

            // Go through the multiplication result and see whether we can improve any of the choices.
            bool schedulerImproved = false;
            // Group refers to the state number
            for (uint_fast64_t group = 0; group < this->A->getRowGroupCount(); ++group) {
                if (!this->choiceFixedForRowGroup || !this->choiceFixedForRowGroup.get()[group]) {
                    //  Only update when the choice is not fixed
                    uint_fast64_t currentChoice = scheduler[group];
                    for (uint_fast64_t choice = this->A->getRowGroupIndices()[group]; choice < this->A->getRowGroupIndices()[group + 1]; ++choice) {
                        // If the choice is the currently selected one, we can skip it.
                        if (choice - this->A->getRowGroupIndices()[group] == currentChoice) {
                            continue;
                        }

                        // Create the value of the choice.
                        ValueType choiceValue = storm::utility::zero<ValueType>();
                        for (auto const& entry : this->A->getRow(choice)) {
                            choiceValue += entry.getValue() * x[entry.getColumn()];
                        }
                        choiceValue += b[choice];

                        // If the value is strictly better than the solution of the inner system, we need to improve the scheduler.
                        // TODO: If the underlying solver is not precise, this might run forever (i.e. when a state has two choices where the (exact) values are
                        // equal). only changing the scheduler if the values are not equal (modulo precision) would make this unsound.
                        if (valueImproved(dir, x[group], choiceValue)) {
                            schedulerImproved = true;
                            scheduler[group] = choice - this->A->getRowGroupIndices()[group];
                            x[group] = std::move(choiceValue);
                        }
                    }
                }
            }

            // If the scheduler did not improve, we are done.
            if (!schedulerImproved) {
                status = SolverStatus::Converged;
            }

            // Update environment variables.
            ++iterations;
            status =
                this->updateStatus(status, x, dir == storm::OptimizationDirection::Minimize ? SolverGuarantee::GreaterOrEqual : SolverGuarantee::LessOrEqual,
                                   iterations, env.solver().minMax().getMaximalNumberOfIterations());

            // Potentially show progress.
            this->showProgressIterative(iterations);
        } while (status == SolverStatus::InProgress);

        STORM_LOG_INFO("Number of iterations: " << iterations);
        this->reportStatus(status, iterations);

        // If requested, we store the scheduler for retrieval.
        if (this->isTrackSchedulerSet()) {
            this->schedulerChoices = std::move(scheduler);
        }

        if (!this->isCachingEnabled()) {
            clearCache();
        }

        return status == SolverStatus::Converged || status == SolverStatus::TerminatedEarly;
    }
}

template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::valueImproved(OptimizationDirection dir, ValueType const& value1,
                                                                                 ValueType const& value2) const {
    if (dir == OptimizationDirection::Minimize) {
        return value2 < value1;
    } else {
        return value2 > value1;
    }
}

template<typename ValueType, typename SolutionType>
MinMaxLinearEquationSolverRequirements IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::getRequirements(
    Environment const& env, boost::optional<storm::solver::OptimizationDirection> const& direction, bool const& hasInitialScheduler) const {
    auto method = getMethod(env, storm::NumberTraits<ValueType>::IsExact || env.solver().isForceExact());

    // Check whether a linear equation solver is needed and potentially start with its requirements
    bool needsLinEqSolver = false;
    needsLinEqSolver |= method == MinMaxMethod::PolicyIteration;
    needsLinEqSolver |= method == MinMaxMethod::ValueIteration && (this->hasInitialScheduler() || hasInitialScheduler);
    needsLinEqSolver |= method == MinMaxMethod::ViToPi;

    MinMaxLinearEquationSolverRequirements requirements;
    if constexpr (std::is_same_v<ValueType, storm::Interval>) {
        STORM_LOG_ASSERT(!needsLinEqSolver, "Intervals should not require a linear equation solver.");
        // nothing to be done;
    } else if (needsLinEqSolver) {
        requirements = MinMaxLinearEquationSolverRequirements(this->linearEquationSolverFactory->getRequirements(env));
    } else {
        // nothing to be done.
    }

    if (method == MinMaxMethod::ValueIteration) {
        if (!this->hasUniqueSolution()) {  // Traditional value iteration has no requirements if the solution is unique.
            // Computing a scheduler is only possible if the solution is unique
            if (env.solver().minMax().isForceRequireUnique() || this->isTrackSchedulerSet()) {
                requirements.requireUniqueSolution();
            } else {
                // As we want the smallest (largest) solution for maximizing (minimizing) equation systems, we have to approach the solution from below (above).
                if (!direction || direction.get() == OptimizationDirection::Maximize) {
                    requirements.requireLowerBounds();
                }
                if (!direction || direction.get() == OptimizationDirection::Minimize) {
                    requirements.requireUpperBounds();
                }
            }
        }
    } else if (method == MinMaxMethod::OptimisticValueIteration) {
        // OptimisticValueIteration always requires lower bounds and a unique solution.
        if (!this->hasUniqueSolution()) {
            requirements.requireUniqueSolution();
        }
        requirements.requireLowerBounds();

    } else if (method == MinMaxMethod::GuessingValueIteration) {
        // Guessing value iteration requires a unique solution and lower+upper bounds
        if (!this->hasUniqueSolution()) {
            requirements.requireUniqueSolution();
        }
        requirements.requireBounds();
    } else if (method == MinMaxMethod::IntervalIteration) {
        // Interval iteration requires a unique solution and lower+upper bounds
        if (!this->hasUniqueSolution()) {
            requirements.requireUniqueSolution();
        }
        requirements.requireBounds();
    } else if (method == MinMaxMethod::RationalSearch) {
        // Rational search needs to approach the solution from below.
        requirements.requireLowerBounds();
        // The solution needs to be unique in case of minimizing or in cases where we want a scheduler.
        if (!this->hasUniqueSolution()) {
            // RationalSearch guesses and verifies a fixpoint and terminates once a fixpoint is found. To ensure that the guessed fixpoint is the
            // correct one, we enforce uniqueness.
            requirements.requireUniqueSolution();
        }
    } else if (method == MinMaxMethod::PolicyIteration) {
        // The initial scheduler shall not select an end component
        if (!this->hasUniqueSolution() && env.solver().minMax().isForceRequireUnique()) {
            requirements.requireUniqueSolution();
        }
        if (!this->hasNoEndComponents() && !this->hasInitialScheduler()) {
            requirements.requireValidInitialScheduler();
        }
    } else if (method == MinMaxMethod::SoundValueIteration) {
        if (!this->hasUniqueSolution()) {
            requirements.requireUniqueSolution();
        }
        requirements.requireBounds(false);
    } else if (method == MinMaxMethod::ViToPi) {
        // Since we want to use value iteration to extract an initial scheduler, the solution has to be unique.
        if (!this->hasUniqueSolution()) {
            requirements.requireUniqueSolution();
        }
    } else {
        STORM_LOG_THROW(false, storm::exceptions::InvalidEnvironmentException, "Unsupported technique for iterative MinMax linear equation solver.");
    }
    return requirements;
}

template<typename ValueType, typename SolutionType>
ValueType computeMaxAbsDiff(std::vector<ValueType> const& allValues, storm::storage::BitVector const& relevantValues, std::vector<ValueType> const& oldValues) {
    ValueType result = storm::utility::zero<ValueType>();
    auto oldValueIt = oldValues.begin();
    for (auto value : relevantValues) {
        result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allValues[value] - *oldValueIt));
        ++oldValueIt;
    }
    return result;
}

template<typename ValueType, typename SolutionType>
ValueType computeMaxAbsDiff(std::vector<ValueType> const& allOldValues, std::vector<ValueType> const& allNewValues,
                            storm::storage::BitVector const& relevantValues) {
    ValueType result = storm::utility::zero<ValueType>();
    for (auto value : relevantValues) {
        result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allNewValues[value] - allOldValues[value]));
    }
    return result;
}

template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::solveEquationsOptimisticValueIteration(Environment const& env, OptimizationDirection dir,
                                                                                                          std::vector<SolutionType>& x,
                                                                                                          std::vector<ValueType> const& b) const {
    if constexpr (std::is_same_v<ValueType, storm::Interval>) {
        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "We did not implement optimistic value iteration for interval-based models.");
        return false;
    } else {
        if (!storm::utility::vector::hasNonZeroEntry(b)) {
            // If all entries are zero, OVI might run in an endless loop. However, the result is easy in this case.
            x.assign(x.size(), storm::utility::zero<SolutionType>());
            if (this->isTrackSchedulerSet()) {
                this->schedulerChoices = std::vector<uint_fast64_t>(x.size(), 0);
            }
            return true;
        }

        setUpViOperator();

        helper::OptimisticValueIterationHelper<ValueType, false> oviHelper(viOperator);
        auto prec = storm::utility::convertNumber<ValueType>(env.solver().minMax().getPrecision());
        std::optional<ValueType> lowerBound, upperBound;
        if (this->hasLowerBound()) {
            lowerBound = this->getLowerBound(true);
        }
        if (this->hasUpperBound()) {
            upperBound = this->getUpperBound(true);
        }
        uint64_t numIterations{0};
        auto oviCallback = [&](SolverStatus const& current, std::vector<ValueType> const& v) {
            this->showProgressIterative(numIterations);
            return this->updateStatus(current, v, SolverGuarantee::LessOrEqual, numIterations, env.solver().minMax().getMaximalNumberOfIterations());
        };
        this->createLowerBoundsVector(x);
        std::optional<ValueType> guessingFactor;
        if (env.solver().ovi().getUpperBoundGuessingFactor()) {
            guessingFactor = storm::utility::convertNumber<ValueType>(*env.solver().ovi().getUpperBoundGuessingFactor());
        }
        this->startMeasureProgress();
        auto status = oviHelper.OVI(x, b, numIterations, env.solver().minMax().getRelativeTerminationCriterion(), prec, dir, guessingFactor, lowerBound,
                                    upperBound, oviCallback);
        this->reportStatus(status, numIterations);

        // If requested, we store the scheduler for retrieval.
        if (this->isTrackSchedulerSet()) {
            this->extractScheduler(x, b, dir, this->isUncertaintyRobust());
        }

        if (!this->isCachingEnabled()) {
            clearCache();
        }

        return status == SolverStatus::Converged || status == SolverStatus::TerminatedEarly;
    }
}

template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::solveEquationsGuessingValueIteration(Environment const& env, OptimizationDirection dir,
                                                                                                        std::vector<SolutionType>& x,
                                                                                                        std::vector<ValueType> const& b) const {
    if constexpr (std::is_same_v<ValueType, storm::Interval>) {
        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "We did not implement guessing value iteration for interval-based models.");
        return false;
    } else {
        setUpViOperator();

        auto& lowerX = x;
        auto upperX = std::make_unique<std::vector<SolutionType>>(x.size());

        storm::solver::helper::GuessingValueIterationHelper<ValueType, false> helper(viOperator, *this->A);

        // x has to start with a lower bound.
        this->createLowerBoundsVector(lowerX);
        this->createUpperBoundsVector(*upperX);

        auto statusIters = helper.solveEquations(lowerX, *upperX, b, storm::utility::convertNumber<ValueType>(env.solver().minMax().getPrecision()),
                                                 env.solver().minMax().getMaximalNumberOfIterations(), dir);
        auto two = storm::utility::convertNumber<ValueType>(2.0);
        storm::utility::vector::applyPointwise<ValueType, ValueType, ValueType>(
            lowerX, *upperX, x, [&two](ValueType const& a, ValueType const& b) -> ValueType { return (a + b) / two; });

        this->reportStatus(statusIters.first, statusIters.second);

        // If requested, we store the scheduler for retrieval.
        if (this->isTrackSchedulerSet()) {
            this->schedulerChoices = std::vector<uint_fast64_t>(this->A->getRowGroupCount());
            this->A->multiplyAndReduce(dir, this->A->getRowGroupIndices(), x, &b, *auxiliaryRowGroupVector.get(), &this->schedulerChoices.get());
        }

        if (!this->isCachingEnabled()) {
            clearCache();
        }

        return statusIters.first == SolverStatus::Converged || statusIters.first == SolverStatus::TerminatedEarly;
    }
}

template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::solveEquationsValueIteration(Environment const& env, OptimizationDirection dir,
                                                                                                std::vector<SolutionType>& x,
                                                                                                std::vector<ValueType> const& b) const {
    setUpViOperator();
    // By default, we can not provide any guarantee
    SolverGuarantee guarantee = SolverGuarantee::None;

    if (this->hasInitialScheduler()) {
        if (!auxiliaryRowGroupVector) {
            auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
        }
        // Solve the equation system induced by the initial scheduler.
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> linEqSolver;
        // The linear equation solver should be at least as precise as this solver
        std::unique_ptr<storm::Environment> environmentOfSolverStorage;
        auto precOfSolver = env.solver().getPrecisionOfLinearEquationSolver(env.solver().getLinearEquationSolverType());
        if (!storm::NumberTraits<ValueType>::IsExact) {
            bool changePrecision = precOfSolver.first && precOfSolver.first.get() > env.solver().minMax().getPrecision();
            bool changeRelative = precOfSolver.second && !precOfSolver.second.get() && env.solver().minMax().getRelativeTerminationCriterion();
            if (changePrecision || changeRelative) {
                environmentOfSolverStorage = std::make_unique<storm::Environment>(env);
                boost::optional<storm::RationalNumber> newPrecision;
                boost::optional<bool> newRelative;
                if (changePrecision) {
                    newPrecision = env.solver().minMax().getPrecision();
                }
                if (changeRelative) {
                    newRelative = true;
                }
                environmentOfSolverStorage->solver().setLinearEquationSolverPrecision(newPrecision, newRelative);
            }
        }
        storm::Environment const& environmentOfSolver = environmentOfSolverStorage ? *environmentOfSolverStorage : env;

        solveInducedEquationSystem(environmentOfSolver, linEqSolver, this->getInitialScheduler(), x, *auxiliaryRowGroupVector, b);
        // If we were given an initial scheduler and are maximizing (minimizing), our current solution becomes
        // always less-or-equal (greater-or-equal) than the actual solution.
        guarantee = maximize(dir) ? SolverGuarantee::LessOrEqual : SolverGuarantee::GreaterOrEqual;
    } else if (!this->hasUniqueSolution()) {
        if (maximize(dir)) {
            this->createLowerBoundsVector(x);
            guarantee = SolverGuarantee::LessOrEqual;
        } else {
            this->createUpperBoundsVector(x);
            guarantee = SolverGuarantee::GreaterOrEqual;
        }
    } else if (this->hasCustomTerminationCondition()) {
        if (this->getTerminationCondition().requiresGuarantee(SolverGuarantee::LessOrEqual) && this->hasLowerBound()) {
            this->createLowerBoundsVector(x);
            guarantee = SolverGuarantee::LessOrEqual;
        } else if (this->getTerminationCondition().requiresGuarantee(SolverGuarantee::GreaterOrEqual) && this->hasUpperBound()) {
            this->createUpperBoundsVector(x);
            guarantee = SolverGuarantee::GreaterOrEqual;
        }
    }

    storm::solver::helper::ValueIterationHelper<ValueType, false, SolutionType> viHelper(viOperator);
    uint64_t numIterations{0};
    auto viCallback = [&](SolverStatus const& current) {
        this->showProgressIterative(numIterations);
        return this->updateStatus(current, x, guarantee, numIterations, env.solver().minMax().getMaximalNumberOfIterations());
    };
    this->startMeasureProgress();
    auto status = viHelper.VI(x, b, numIterations, env.solver().minMax().getRelativeTerminationCriterion(),
                              storm::utility::convertNumber<SolutionType>(env.solver().minMax().getPrecision()), dir, viCallback,
                              env.solver().minMax().getMultiplicationStyle(), this->isUncertaintyRobust());
    this->reportStatus(status, numIterations);

    // If requested, we store the scheduler for retrieval.
    if (this->isTrackSchedulerSet()) {
        this->extractScheduler(x, b, dir, this->isUncertaintyRobust());
    }

    if (!this->isCachingEnabled()) {
        clearCache();
    }

    return status == SolverStatus::Converged || status == SolverStatus::TerminatedEarly;
}

template<typename ValueType, typename SolutionType>
void preserveOldRelevantValues(std::vector<ValueType> const& allValues, storm::storage::BitVector const& relevantValues, std::vector<ValueType>& oldValues) {
    storm::utility::vector::selectVectorValues(oldValues, relevantValues, allValues);
}

/*!
 * This version of value iteration is sound, because it approaches the solution from below and above. This
 * technique is due to Haddad and Monmege (Interval iteration algorithm for MDPs and IMDPs, TCS 2017) and was
 * extended to rewards by Baier, Klein, Leuschner, Parker and Wunderlich (Ensuring the Reliability of Your
 * Model Checker: Interval Iteration for Markov Decision Processes, CAV 2017).
 */
template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::solveEquationsIntervalIteration(Environment const& env, OptimizationDirection dir,
                                                                                                   std::vector<SolutionType>& x,
                                                                                                   std::vector<ValueType> const& b) const {
    if constexpr (std::is_same_v<ValueType, storm::Interval>) {
        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "We did not implement intervaliteration for interval-based models");
        return false;
    } else {
        setUpViOperator();
        helper::IntervalIterationHelper<ValueType, false> iiHelper(viOperator);
        auto prec = storm::utility::convertNumber<ValueType>(env.solver().minMax().getPrecision());
        auto lowerBoundsCallback = [&](std::vector<SolutionType>& vector) { this->createLowerBoundsVector(vector); };
        auto upperBoundsCallback = [&](std::vector<SolutionType>& vector) { this->createUpperBoundsVector(vector); };

        uint64_t numIterations{0};
        auto iiCallback = [&](helper::IIData<ValueType> const& data) {
            this->showProgressIterative(numIterations);
            bool terminateEarly = this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(data.x, SolverGuarantee::LessOrEqual) &&
                                  this->getTerminationCondition().terminateNow(data.y, SolverGuarantee::GreaterOrEqual);
            return this->updateStatus(data.status, terminateEarly, numIterations, env.solver().minMax().getMaximalNumberOfIterations());
        };
        std::optional<storm::storage::BitVector> optionalRelevantValues;
        if (this->hasRelevantValues()) {
            optionalRelevantValues = this->getRelevantValues();
        }
        this->startMeasureProgress();
        auto status = iiHelper.II(x, b, numIterations, env.solver().minMax().getRelativeTerminationCriterion(), prec, lowerBoundsCallback, upperBoundsCallback,
                                  dir, iiCallback, optionalRelevantValues);
        this->reportStatus(status, numIterations);

        // If requested, we store the scheduler for retrieval.
        if (this->isTrackSchedulerSet()) {
            this->extractScheduler(x, b, dir, this->isUncertaintyRobust());
        }

        if (!this->isCachingEnabled()) {
            clearCache();
        }

        return status == SolverStatus::Converged || status == SolverStatus::TerminatedEarly;
    }
}

template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::solveEquationsSoundValueIteration(Environment const& env, OptimizationDirection dir,
                                                                                                     std::vector<SolutionType>& x,
                                                                                                     std::vector<ValueType> const& b) const {
    if constexpr (std::is_same_v<ValueType, storm::Interval>) {
        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "SoundVI does not handle interval-based models");
        return false;
    } else {
        // Prepare the solution vectors and the helper.
        assert(x.size() == this->A->getRowGroupCount());

        std::optional<ValueType> lowerBound, upperBound;
        if (this->hasLowerBound()) {
            lowerBound = this->getLowerBound(true);
        }
        if (this->hasUpperBound()) {
            upperBound = this->getUpperBound(true);
        }

        setUpViOperator();

        auto precision = storm::utility::convertNumber<ValueType>(env.solver().minMax().getPrecision());
        uint64_t numIterations{0};
        auto sviCallback = [&](typename helper::SoundValueIterationHelper<ValueType, false>::SVIData const& current) {
            this->showProgressIterative(numIterations);
            return this->updateStatus(current.status,
                                      this->hasCustomTerminationCondition() && current.checkCustomTerminationCondition(this->getTerminationCondition()),
                                      numIterations, env.solver().minMax().getMaximalNumberOfIterations());
        };
        this->startMeasureProgress();
        helper::SoundValueIterationHelper<ValueType, false> sviHelper(viOperator);
        std::optional<storm::storage::BitVector> optionalRelevantValues;
        if (this->hasRelevantValues()) {
            optionalRelevantValues = this->getRelevantValues();
        }
        auto status = sviHelper.SVI(x, b, numIterations, env.solver().minMax().getRelativeTerminationCriterion(), precision, dir, lowerBound, upperBound,
                                    sviCallback, optionalRelevantValues);

        // If requested, we store the scheduler for retrieval.
        if (this->isTrackSchedulerSet()) {
            this->extractScheduler(x, b, dir, this->isUncertaintyRobust());
        }

        this->reportStatus(status, numIterations);

        if (!this->isCachingEnabled()) {
            clearCache();
        }

        return status == SolverStatus::Converged || status == SolverStatus::TerminatedEarly;
    }
}

template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::solveEquationsViToPi(Environment const& env, OptimizationDirection dir,
                                                                                        std::vector<SolutionType>& x, std::vector<ValueType> const& b) const {
    if constexpr (std::is_same_v<ValueType, storm::Interval>) {
        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "ViToPi does not handle interval-based models");
        return false;
    }
    // First create an (inprecise) vi solver to get a good initial strategy for the (potentially precise) policy iteration solver.
    std::vector<storm::storage::sparse::state_type> initialSched;
    {
        Environment viEnv = env;
        viEnv.solver().minMax().setMethod(MinMaxMethod::ValueIteration);
        viEnv.solver().setForceExact(false);
        viEnv.solver().setForceSoundness(false);
        auto impreciseSolver = GeneralMinMaxLinearEquationSolverFactory<double>().create(viEnv, this->A->template toValueType<double>());
        impreciseSolver->setHasUniqueSolution(this->hasUniqueSolution());
        impreciseSolver->setTrackScheduler(true);
        if (this->hasInitialScheduler()) {
            auto initSched = this->getInitialScheduler();
            impreciseSolver->setInitialScheduler(std::move(initSched));
        }
        auto impreciseSolverReq = impreciseSolver->getRequirements(viEnv, dir);
        STORM_LOG_THROW(!impreciseSolverReq.hasEnabledCriticalRequirement(), storm::exceptions::UnmetRequirementException,
                        "The value-iteration based solver has an unmet requirement: " << impreciseSolverReq.getEnabledRequirementsAsString());
        impreciseSolver->setRequirementsChecked(true);
        auto xVi = storm::utility::vector::convertNumericVector<double>(x);
        auto bVi = storm::utility::vector::convertNumericVector<double>(b);
        impreciseSolver->solveEquations(viEnv, dir, xVi, bVi);
        initialSched = impreciseSolver->getSchedulerChoices();
    }
    STORM_LOG_INFO("Found initial policy using Value Iteration. Starting Policy iteration now.");
    return performPolicyIteration(env, dir, x, b, std::move(initialSched));
}

template<typename ValueType, typename SolutionType>
bool IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::solveEquationsRationalSearch(Environment const& env, OptimizationDirection dir,
                                                                                                std::vector<SolutionType>& x,
                                                                                                std::vector<ValueType> const& b) const {
    if constexpr (std::is_same_v<ValueType, storm::Interval>) {
        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Rational search does not handle interval-based models");
        return false;
    } else {
        // Set up two value iteration operators. One for exact and one for imprecise computations
        setUpViOperator();
        std::shared_ptr<helper::ValueIterationOperator<storm::RationalNumber, false>> exactOp;
        std::shared_ptr<helper::ValueIterationOperator<double, false>> impreciseOp;
        std::function<bool(uint64_t, uint64_t)> fixedChoicesCallback;
        if (this->choiceFixedForRowGroup) {
            // Ignore those rows that are not selected
            assert(this->initialScheduler);
            fixedChoicesCallback = [&](uint64_t groupIndex, uint64_t localRowIndex) {
                return this->choiceFixedForRowGroup->get(groupIndex) && this->initialScheduler->at(groupIndex) != localRowIndex;
            };
        }

        if constexpr (std::is_same_v<ValueType, storm::RationalNumber>) {
            exactOp = viOperator;
            impreciseOp = std::make_shared<helper::ValueIterationOperator<double, false>>();
            impreciseOp->setMatrixBackwards(this->A->template toValueType<double>(), &this->A->getRowGroupIndices());
            if (this->choiceFixedForRowGroup) {
                impreciseOp->setIgnoredRows(true, fixedChoicesCallback);
            }
        } else if constexpr (std::is_same_v<ValueType, double>) {
            impreciseOp = viOperator;
            exactOp = std::make_shared<helper::ValueIterationOperator<storm::RationalNumber, false>>();
            exactOp->setMatrixBackwards(this->A->template toValueType<storm::RationalNumber>(), &this->A->getRowGroupIndices());
            if (this->choiceFixedForRowGroup) {
                exactOp->setIgnoredRows(true, fixedChoicesCallback);
            }
        }

        storm::solver::helper::RationalSearchHelper<ValueType, storm::RationalNumber, double, false> rsHelper(exactOp, impreciseOp);
        uint64_t numIterations{0};
        auto rsCallback = [&](SolverStatus const& current) {
            this->showProgressIterative(numIterations);
            return this->updateStatus(current, x, SolverGuarantee::None, numIterations, env.solver().minMax().getMaximalNumberOfIterations());
        };
        this->startMeasureProgress();
        auto status = rsHelper.RS(x, b, numIterations, storm::utility::convertNumber<ValueType>(env.solver().minMax().getPrecision()), dir, rsCallback);

        this->reportStatus(status, numIterations);

        // If requested, we store the scheduler for retrieval.
        if (this->isTrackSchedulerSet()) {
            this->extractScheduler(x, b, dir, this->isUncertaintyRobust());
        }

        if (!this->isCachingEnabled()) {
            clearCache();
        }

        return status == SolverStatus::Converged || status == SolverStatus::TerminatedEarly;
    }
}

template<typename ValueType, typename SolutionType>
void IterativeMinMaxLinearEquationSolver<ValueType, SolutionType>::clearCache() const {
    auxiliaryRowGroupVector.reset();
    viOperator.reset();
    StandardMinMaxLinearEquationSolver<ValueType, SolutionType>::clearCache();
}

template class IterativeMinMaxLinearEquationSolver<double, double>;
template class IterativeMinMaxLinearEquationSolver<storm::RationalNumber, storm::RationalNumber>;
template class IterativeMinMaxLinearEquationSolver<storm::Interval, double>;

}  // namespace solver
}  // namespace storm
