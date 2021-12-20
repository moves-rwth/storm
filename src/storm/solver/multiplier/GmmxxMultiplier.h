#pragma once

#include "Multiplier.h"

#include "storm/adapters/GmmxxAdapter.h"

#include "storm-config.h"

namespace storm {

namespace storage {
template<typename ValueType>
class SparseMatrix;
}

namespace solver {

template<typename ValueType>
class GmmxxMultiplier : public Multiplier<ValueType> {
   public:
    GmmxxMultiplier(storm::storage::SparseMatrix<ValueType> const& matrix);
    virtual ~GmmxxMultiplier() = default;

    virtual void multiply(Environment const& env, std::vector<ValueType> const& x, std::vector<ValueType> const* b,
                          std::vector<ValueType>& result) const override;
    virtual void multiplyGaussSeidel(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const* b, bool backwards = true) const override;
    virtual void multiplyAndReduce(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices,
                                   std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result,
                                   std::vector<uint_fast64_t>* choices = nullptr) const override;
    virtual void multiplyAndReduceGaussSeidel(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices,
                                              std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices = nullptr,
                                              bool backwards = true) const override;
    virtual void multiplyRow(uint64_t const& rowIndex, std::vector<ValueType> const& x, ValueType& value) const override;
    virtual void clearCache() const override;

   private:
    void initialize() const;

    bool parallelize(Environment const& env) const;

    void multAdd(std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const;
    void multAddParallel(std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const;
    void multAddReduceParallel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType> const& x,
                               std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices = nullptr) const;
    void multAddReduceHelper(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType> const& x,
                             std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices = nullptr,
                             bool backwards = true) const;

    template<typename Compare, bool backwards = true>
    void multAddReduceHelper(std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType> const& x, std::vector<ValueType> const* b,
                             std::vector<ValueType>& result, std::vector<uint64_t>* choices = nullptr) const;

    mutable gmm::csr_matrix<ValueType> gmmMatrix;
};

}  // namespace solver
}  // namespace storm
