#pragma once
#include <memory>

#include "Multiplier.h"

#include "storm/solver/helper/ValueIterationOperator.h"

namespace storm {

namespace storage {
template<typename ValueType>
class SparseMatrix;
}

namespace solver {

template<typename ValueType, bool TrivialRowGrouping>
class ViOperatorMultiplier : public Multiplier<ValueType> {
   public:
    ViOperatorMultiplier(storm::storage::SparseMatrix<ValueType> const& matrix);
    virtual ~ViOperatorMultiplier() = default;

    virtual void multiply(Environment const& env, std::vector<ValueType> const& x, std::vector<ValueType> const* b,
                          std::vector<ValueType>& result) const override;
    virtual void multiplyGaussSeidel(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const* b, bool backwards = true) const override;
    virtual void multiplyAndReduce(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices,
                                   std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result,
                                   std::vector<uint_fast64_t>* choices = nullptr) const override;
    virtual void multiplyAndReduceGaussSeidel(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices,
                                              std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices = nullptr,
                                              bool backwards = true) const override;
    virtual void clearCache() const override;

   private:
    using ViOpT = storm::solver::helper::ValueIterationOperator<ValueType, TrivialRowGrouping>;

    ViOpT& initialize() const;
    ViOpT& initialize(bool backwards) const;

    // We store two operators, one for the forward and one for the backward gauss seidel iterations.
    // By default, the backward operator is chosen.
    mutable std::unique_ptr<ViOpT> viOperatorFwd, viOperatorBwd;
};

}  // namespace solver
}  // namespace storm
