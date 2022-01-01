#pragma once

#include "storm/environment/solver/SolverEnvironment.h"
#include "storm/solver/SolverSelectionOptions.h"

namespace storm {

class MultiplierEnvironment {
   public:
    MultiplierEnvironment();
    ~MultiplierEnvironment();

    storm::solver::MultiplierType const& getType() const;
    bool const& isTypeSetFromDefault() const;
    void setType(storm::solver::MultiplierType value, bool isSetFromDefault = false);

   private:
    storm::solver::MultiplierType type;
    bool typeSetFromDefault;
};
}  // namespace storm
