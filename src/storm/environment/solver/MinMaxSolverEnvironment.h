#pragma once

#include "storm/environment/solver/SolverEnvironment.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/solver/MultiplicationStyle.h"
#include "storm/solver/SolverSelectionOptions.h"

namespace storm {

class MinMaxSolverEnvironment {
   public:
    MinMaxSolverEnvironment();
    ~MinMaxSolverEnvironment();

    storm::solver::MinMaxMethod const& getMethod() const;
    bool const& isMethodSetFromDefault() const;
    void setMethod(storm::solver::MinMaxMethod value, bool isSetFromDefault = false);
    uint64_t const& getMaximalNumberOfIterations() const;
    void setMaximalNumberOfIterations(uint64_t value);
    storm::RationalNumber const& getPrecision() const;
    void setPrecision(storm::RationalNumber value);
    bool const& getRelativeTerminationCriterion() const;
    void setRelativeTerminationCriterion(bool value);
    storm::solver::MultiplicationStyle const& getMultiplicationStyle() const;
    void setMultiplicationStyle(storm::solver::MultiplicationStyle value);
    bool isForceRequireUnique() const;
    void setForceRequireUnique(bool value);

   private:
    storm::solver::MinMaxMethod minMaxMethod;
    bool methodSetFromDefault;
    uint64_t maxIterationCount;
    storm::RationalNumber precision;
    bool considerRelativeTerminationCriterion;
    storm::solver::MultiplicationStyle multiplicationStyle;
    bool forceRequireUnique;
};
}  // namespace storm
