#include "storm/solver/SolveGoal.h"

#include <memory>

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/adapters/RationalNumberAdapter.h"

#include "storm/modelchecker/CheckTask.h"

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/utility/solver.h"

namespace storm {
namespace storage {
template<typename ValueType>
class SparseMatrix;
}

namespace solver {

template<typename ValueType>
SolveGoal<ValueType>::SolveGoal() {
    // Intentionally left empty.
}

template<typename ValueType>
SolveGoal<ValueType>::SolveGoal(bool minimize) : optimizationDirection(minimize ? OptimizationDirection::Minimize : OptimizationDirection::Maximize) {
    // Intentionally left empty.
}

template<typename ValueType>
SolveGoal<ValueType>::SolveGoal(OptimizationDirection optimizationDirection) : optimizationDirection(optimizationDirection) {
    // Intentionally left empty.
}

template<typename ValueType>
SolveGoal<ValueType>::SolveGoal(OptimizationDirection optimizationDirection, storm::logic::ComparisonType boundComparisonType, ValueType const& boundThreshold,
                                storm::storage::BitVector const& relevantValues)
    : optimizationDirection(optimizationDirection), comparisonType(boundComparisonType), threshold(boundThreshold), relevantValueVector(relevantValues) {
    // Intentionally left empty.
}

template<typename ValueType>
SolveGoal<ValueType>::SolveGoal(OptimizationDirection optimizationDirection, storm::storage::BitVector const& relevantValues)
    : optimizationDirection(optimizationDirection), relevantValueVector(relevantValues) {
    // Intentionally left empty.
}

template<typename ValueType>
bool SolveGoal<ValueType>::hasDirection() const {
    return static_cast<bool>(optimizationDirection);
}

template<typename ValueType>
void SolveGoal<ValueType>::oneMinus() {
    if (optimizationDirection) {
        if (optimizationDirection == storm::solver::OptimizationDirection::Minimize) {
            optimizationDirection = storm::solver::OptimizationDirection::Maximize;
        } else {
            optimizationDirection = storm::solver::OptimizationDirection::Minimize;
        }
    }
    if (threshold) {
        this->threshold = storm::utility::one<ValueType>() - this->threshold.get();
    }
    if (comparisonType) {
        switch (comparisonType.get()) {
            case storm::logic::ComparisonType::Less:
                comparisonType = storm::logic::ComparisonType::GreaterEqual;
                break;
            case storm::logic::ComparisonType::LessEqual:
                comparisonType = storm::logic::ComparisonType::Greater;
                break;
            case storm::logic::ComparisonType::Greater:
                comparisonType = storm::logic::ComparisonType::LessEqual;
                break;
            case storm::logic::ComparisonType::GreaterEqual:
                comparisonType = storm::logic::ComparisonType::Less;
                break;
        }
    }
}

template<typename ValueType>
bool SolveGoal<ValueType>::minimize() const {
    return optimizationDirection == OptimizationDirection::Minimize;
}

template<typename ValueType>
OptimizationDirection SolveGoal<ValueType>::direction() const {
    return optimizationDirection.get();
}

template<typename ValueType>
bool SolveGoal<ValueType>::isBounded() const {
    return comparisonType && threshold && relevantValueVector;
}

template<typename ValueType>
bool SolveGoal<ValueType>::boundIsALowerBound() const {
    return (comparisonType.get() == storm::logic::ComparisonType::Greater || comparisonType.get() == storm::logic::ComparisonType::GreaterEqual);
}

template<typename ValueType>
bool SolveGoal<ValueType>::boundIsStrict() const {
    return (comparisonType.get() == storm::logic::ComparisonType::Greater || comparisonType.get() == storm::logic::ComparisonType::Less);
}

template<typename ValueType>
ValueType const& SolveGoal<ValueType>::thresholdValue() const {
    return threshold.get();
}

template<typename ValueType>
bool SolveGoal<ValueType>::hasRelevantValues() const {
    return static_cast<bool>(relevantValueVector);
}

template<typename ValueType>
storm::storage::BitVector const& SolveGoal<ValueType>::relevantValues() const {
    return relevantValueVector.get();
}

template<typename ValueType>
storm::storage::BitVector& SolveGoal<ValueType>::relevantValues() {
    return relevantValueVector.get();
}

template<typename ValueType>
void SolveGoal<ValueType>::restrictRelevantValues(storm::storage::BitVector const& filter) {
    if (relevantValueVector) {
        relevantValueVector = relevantValueVector.get() % filter;
    }
}

template<typename ValueType>
void SolveGoal<ValueType>::setRelevantValues(storm::storage::BitVector&& values) {
    relevantValueVector = std::move(values);
}

template class SolveGoal<double>;

#ifdef STORM_HAVE_CARL
template class SolveGoal<storm::RationalNumber>;
template class SolveGoal<storm::RationalFunction>;
#endif
}  // namespace solver
}  // namespace storm
