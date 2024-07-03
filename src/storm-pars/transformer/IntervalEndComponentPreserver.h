#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "adapters/RationalFunctionForward.h"
#include "adapters/RationalNumberForward.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm-pars/analysis/Order.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm-pars/utility/parametric.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"

namespace storm {
namespace transformer {

template<typename ParametricType>
class IntervalEndComponentPreserver {
   public:
      IntervalEndComponentPreserver(storm::storage::SparseMatrix<ParametricType> const& originalMatrix, std::vector<ParametricType> const& originalVector);

      void specifyAssignment(storm::storage::SparseMatrix<Interval> const& matrix, std::vector<Interval> const& vector);

      // Returns the resulting matrix. Should only be called AFTER specifying an assigment
      storm::storage::SparseMatrix<Interval> const& getMatrix() const;

      // Returns the resulting vector. Should only be called AFTER specifying an assigment
      std::vector<Interval> const& getVector() const;

   private:
      storm::storage::SparseMatrix<Interval> matrix;  // The resulting matrix;
      std::vector<typename storm::storage::SparseMatrix<Interval>::iterator> matrixAssignment;  // Connection of matrix entries with placeholders
      
      std::vector<Interval> vector;
      std::vector<typename std::vector<Interval>::iterator> vectorAssignment;  // Connection of vector entries with placeholders
      std::vector<Interval> vectorPlaceholders;

      storage::BitVector considered;
};
}  // namespace transformer
}  // namespace storm
