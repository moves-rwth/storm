#pragma once

#include <optional>
#include "storm/environment/solver/SolverEnvironment.h"

#include "storm/adapters/RationalNumberAdapter.h"

namespace storm {

class OviSolverEnvironment {
   public:
    OviSolverEnvironment();
    ~OviSolverEnvironment() = default;

    std::optional<storm::RationalNumber> const& getUpperBoundGuessingFactor() const;

   private:
    std::optional<storm::RationalNumber> upperBoundGuessingFactor;
};
}  // namespace storm
