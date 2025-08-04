#include "storm/solver/multiplier/Multiplier.h"

#include "storm-config.h"

#include "storm/storage/SparseMatrix.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/adapters/RationalNumberAdapter.h"

#include "storm/environment/solver/MultiplierEnvironment.h"
#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/NotImplementedException.h"

#include "storm/solver/multiplier/NativeMultiplier.h"

#include "storm/solver/SolverSelectionOptions.h"
#include "storm/solver/multiplier/ViOperatorMultiplier.h"
#include "storm/utility/ProgressMeasurement.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/macros.h"

namespace storm {
namespace solver {

template<typename ValueType>
Multiplier<ValueType>::Multiplier(storm::storage::SparseMatrix<ValueType> const& matrix) : matrix(matrix) {
    // Intentionally left empty.
}

template<typename ValueType>
void Multiplier<ValueType>::clearCache() const {
    cachedVector.reset();
}

template<typename ValueType>
void Multiplier<ValueType>::multiplyAndReduce(Environment const& env, OptimizationDirection const& dir, std::vector<ValueType> const& x,
                                              std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices) const {
    multiplyAndReduce(env, dir, this->matrix.getRowGroupIndices(), x, b, result, choices);
}

template<typename ValueType>
void Multiplier<ValueType>::multiplyAndReduceGaussSeidel(Environment const& env, OptimizationDirection const& dir, std::vector<ValueType>& x,
                                                         std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices, bool backwards) const {
    multiplyAndReduceGaussSeidel(env, dir, this->matrix.getRowGroupIndices(), x, b, choices, backwards);
}

template<typename ValueType>
void Multiplier<ValueType>::repeatedMultiply(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint64_t n) const {
    storm::utility::ProgressMeasurement progress("multiplications");
    progress.setMaxCount(n);
    progress.startNewMeasurement(0);
    for (uint64_t i = 0; i < n; ++i) {
        progress.updateProgress(i);
        multiply(env, x, b, x);
        if (storm::utility::resources::isTerminate()) {
            STORM_LOG_WARN("Aborting after " << i << " of " << n << " multiplications.");
            break;
        }
    }
}

template<typename ValueType>
void Multiplier<ValueType>::repeatedMultiplyAndReduce(Environment const& env, OptimizationDirection const& dir, std::vector<ValueType>& x,
                                                      std::vector<ValueType> const* b, uint64_t n) const {
    storm::utility::ProgressMeasurement progress("multiplications");
    progress.setMaxCount(n);
    progress.startNewMeasurement(0);
    for (uint64_t i = 0; i < n; ++i) {
        multiplyAndReduce(env, dir, x, b, x);
        if (storm::utility::resources::isTerminate()) {
            STORM_LOG_WARN("Aborting after " << i << " of " << n << " multiplications");
            break;
        }
    }
}

template<typename ValueType>
void Multiplier<ValueType>::repeatedMultiplyAndReduceWithFactor(Environment const& env, OptimizationDirection const& dir, std::vector<ValueType>& x,
                                                                std::vector<ValueType> const* b, uint64_t n, ValueType factor) const {
    storm::utility::ProgressMeasurement progress("multiplications");
    progress.setMaxCount(n);
    progress.startNewMeasurement(0);
    for (uint64_t i = 0; i < n; ++i) {
        std::transform(x.begin(), x.end(), x.begin(), [factor](ValueType& c) { return c * factor; });
        multiplyAndReduce(env, dir, x, b, x);
        if (storm::utility::resources::isTerminate()) {
            STORM_LOG_WARN("Aborting after " << i << " of " << n << " multiplications");
            break;
        }
    }
}

template<typename ValueType>
void Multiplier<ValueType>::repeatedMultiplyWithFactor(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint64_t n,
                                                       ValueType factor) const {
    storm::utility::ProgressMeasurement progress("multiplications");
    progress.setMaxCount(n);
    progress.startNewMeasurement(0);
    for (uint64_t i = 0; i < n; ++i) {
        std::transform(x.begin(), x.end(), x.begin(), [factor](ValueType& c) { return c * factor; });
        multiply(env, x, b, x);
        if (storm::utility::resources::isTerminate()) {
            STORM_LOG_WARN("Aborting after " << i << " of " << n << " multiplications");
            break;
        }
    }
}

template<typename ValueType>

std::vector<ValueType>& Multiplier<ValueType>::provideCachedVector(uint64_t size) const {
    if (this->cachedVector) {
        this->cachedVector->resize(size);
    } else {
        this->cachedVector = std::make_unique<std::vector<ValueType>>(size);
    }
    return *this->cachedVector;
}

template<typename ValueType>
std::unique_ptr<Multiplier<ValueType>> MultiplierFactory<ValueType>::create(Environment const& env, storm::storage::SparseMatrix<ValueType> const& matrix) {
    auto type = env.solver().multiplier().getType();

    // Adjust the type if the ValueType is not supported
    if (type == MultiplierType::ViOperator && (std::is_same_v<ValueType, storm::RationalFunction> || std::is_same_v<ValueType, storm::Interval>)) {
        STORM_LOG_INFO("Switching multiplier type from 'vioperator' to 'native' because the given ValueType is not supported by the VI Operator multiplier.");
        type = MultiplierType::Native;
    }

    switch (type) {
        case MultiplierType::ViOperator:
            if constexpr (std::is_same_v<ValueType, storm::RationalFunction> || std::is_same_v<ValueType, storm::Interval>) {
                throw storm::exceptions::NotImplementedException() << "VI Operator multiplier not supported with given value type.";
            }
            if (matrix.hasTrivialRowGrouping()) {
                return std::make_unique<ViOperatorMultiplier<ValueType, true>>(matrix);
            } else {
                return std::make_unique<ViOperatorMultiplier<ValueType, false>>(matrix);
            }
        case MultiplierType::Native:
            return std::make_unique<NativeMultiplier<ValueType>>(matrix);
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Unknown MultiplierType");
}

template class Multiplier<double>;
template class MultiplierFactory<double>;
template class Multiplier<storm::RationalNumber>;
template class MultiplierFactory<storm::RationalNumber>;
template class Multiplier<storm::RationalFunction>;
template class MultiplierFactory<storm::RationalFunction>;
template class Multiplier<storm::Interval>;
template class MultiplierFactory<storm::Interval>;

}  // namespace solver
}  // namespace storm
