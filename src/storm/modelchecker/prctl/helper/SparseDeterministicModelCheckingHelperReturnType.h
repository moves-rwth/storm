#pragma once

#include <utility>
#include <vector>

#include "storm/solver/SolutionBounds.h"

namespace storm {
namespace modelchecker {
namespace helper {

/*!
 * The outcome of a quantitative model checking query on a sparse deterministic model, i.e. a DTMC or a CTMC.
 * Unlike its MDP counterpart it carries no scheduler, as such a model has no nondeterminism to resolve.
 */
template<typename ValueType>
struct SparseDeterministicModelCheckingHelperReturnType {
    SparseDeterministicModelCheckingHelperReturnType(SparseDeterministicModelCheckingHelperReturnType const&) = delete;
    SparseDeterministicModelCheckingHelperReturnType(SparseDeterministicModelCheckingHelperReturnType&&) = default;
    SparseDeterministicModelCheckingHelperReturnType& operator=(SparseDeterministicModelCheckingHelperReturnType&&) = default;

    explicit SparseDeterministicModelCheckingHelperReturnType(std::vector<ValueType>&& values) : values(std::move(values)) {
        // Intentionally left empty.
    }

    virtual ~SparseDeterministicModelCheckingHelperReturnType() {
        // Intentionally left empty.
    }

    // The values computed for the states.
    std::vector<ValueType> values;

    storm::solver::SolutionBounds<ValueType> solutionBounds;
};

}  // namespace helper
}  // namespace modelchecker
}  // namespace storm
