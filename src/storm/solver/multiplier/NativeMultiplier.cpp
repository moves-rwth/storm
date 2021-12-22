#include "NativeMultiplier.h"

#include "storm-config.h"

#include "storm/environment/solver/MultiplierEnvironment.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm/storage/SparseMatrix.h"

#include "storm/adapters/IntelTbbAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/adapters/RationalNumberAdapter.h"

#include "storm/utility/macros.h"

namespace storm {
namespace solver {

template<typename ValueType>
NativeMultiplier<ValueType>::NativeMultiplier(storm::storage::SparseMatrix<ValueType> const& matrix) : Multiplier<ValueType>(matrix) {
    // Intentionally left empty.
}

template<typename ValueType>
bool NativeMultiplier<ValueType>::parallelize(Environment const& env) const {
#ifdef STORM_HAVE_INTELTBB
    return storm::settings::getModule<storm::settings::modules::CoreSettings>().isUseIntelTbbSet();
#else
    return false;
#endif
}

template<typename ValueType>
void NativeMultiplier<ValueType>::multiply(Environment const& env, std::vector<ValueType> const& x, std::vector<ValueType> const* b,
                                           std::vector<ValueType>& result) const {
    std::vector<ValueType>* target = &result;
    if (&x == &result) {
        if (this->cachedVector) {
            this->cachedVector->resize(x.size());
        } else {
            this->cachedVector = std::make_unique<std::vector<ValueType>>(x.size());
        }
        target = this->cachedVector.get();
    }
    if (parallelize(env)) {
        multAddParallel(x, b, *target);
    } else {
        multAdd(x, b, *target);
    }
    if (&x == &result) {
        std::swap(result, *this->cachedVector);
    }
}

template<typename ValueType>
void NativeMultiplier<ValueType>::multiplyGaussSeidel(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const* b,
                                                      bool backwards) const {
    if (backwards) {
        this->matrix.multiplyWithVectorBackward(x, x, b);
    } else {
        this->matrix.multiplyWithVectorForward(x, x, b);
    }
}

template<typename ValueType>
void NativeMultiplier<ValueType>::multiplyAndReduce(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices,
                                                    std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result,
                                                    std::vector<uint_fast64_t>* choices) const {
    std::vector<ValueType>* target = &result;
    if (&x == &result) {
        if (this->cachedVector) {
            this->cachedVector->resize(x.size());
        } else {
            this->cachedVector = std::make_unique<std::vector<ValueType>>(x.size());
        }
        target = this->cachedVector.get();
    }
    if (parallelize(env)) {
        multAddReduceParallel(dir, rowGroupIndices, x, b, *target, choices);
    } else {
        multAddReduce(dir, rowGroupIndices, x, b, *target, choices);
    }
    if (&x == &result) {
        std::swap(result, *this->cachedVector);
    }
}

template<typename ValueType>
void NativeMultiplier<ValueType>::multiplyAndReduceGaussSeidel(Environment const& env, OptimizationDirection const& dir,
                                                               std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x,
                                                               std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices, bool backwards) const {
    if (backwards) {
        this->matrix.multiplyAndReduceBackward(dir, rowGroupIndices, x, b, x, choices);
    } else {
        this->matrix.multiplyAndReduceForward(dir, rowGroupIndices, x, b, x, choices);
    }
}

template<typename ValueType>
void NativeMultiplier<ValueType>::multiplyRow(uint64_t const& rowIndex, std::vector<ValueType> const& x, ValueType& value) const {
    for (auto const& entry : this->matrix.getRow(rowIndex)) {
        value += entry.getValue() * x[entry.getColumn()];
    }
}

template<typename ValueType>
void NativeMultiplier<ValueType>::multiplyRow2(uint64_t const& rowIndex, std::vector<ValueType> const& x1, ValueType& val1, std::vector<ValueType> const& x2,
                                               ValueType& val2) const {
    for (auto const& entry : this->matrix.getRow(rowIndex)) {
        val1 += entry.getValue() * x1[entry.getColumn()];
        val2 += entry.getValue() * x2[entry.getColumn()];
    }
}

template<typename ValueType>
void NativeMultiplier<ValueType>::multAdd(std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
    this->matrix.multiplyWithVector(x, result, b);
}

template<typename ValueType>
void NativeMultiplier<ValueType>::multAddReduce(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices,
                                                std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result,
                                                std::vector<uint64_t>* choices) const {
    this->matrix.multiplyAndReduce(dir, rowGroupIndices, x, b, result, choices);
}

template<typename ValueType>
void NativeMultiplier<ValueType>::multAddParallel(std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
#ifdef STORM_HAVE_INTELTBB
    this->matrix.multiplyWithVectorParallel(x, result, b);
#else
    STORM_LOG_WARN("Storm was built without support for Intel TBB, defaulting to sequential version.");
    multAdd(x, b, result);
#endif
}

template<typename ValueType>
void NativeMultiplier<ValueType>::multAddReduceParallel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices,
                                                        std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result,
                                                        std::vector<uint64_t>* choices) const {
#ifdef STORM_HAVE_INTELTBB
    this->matrix.multiplyAndReduceParallel(dir, rowGroupIndices, x, b, result, choices);
#else
    STORM_LOG_WARN("Storm was built without support for Intel TBB, defaulting to sequential version.");
    multAddReduce(dir, rowGroupIndices, x, b, result, choices);
#endif
}

template class NativeMultiplier<double>;
#ifdef STORM_HAVE_CARL
template class NativeMultiplier<storm::RationalNumber>;
template class NativeMultiplier<storm::RationalFunction>;
#endif

}  // namespace solver
}  // namespace storm
