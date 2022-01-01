#pragma once

#include "storm/solver/stateelimination/EliminatorBase.h"

namespace storm {
namespace solver {
namespace stateelimination {

template<typename ValueType>
class EquationSystemEliminator : public EliminatorBase<ValueType, ScalingMode::Divide> {
   public:
    EquationSystemEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& matrix, storm::storage::FlexibleSparseMatrix<ValueType>& transposedMatrix);
};

}  // namespace stateelimination
}  // namespace solver
}  // namespace storm
