#pragma once

#include "storm/environment/solver/SolverEnvironment.h"

#include "storm/adapters/RationalNumberAdapter.h"

namespace storm {
    
    class OviSolverEnvironment {
    public:
        
        OviSolverEnvironment();
        ~OviSolverEnvironment();
        
        storm::RationalNumber getPrecisionUpdateFactor() const;
        storm::RationalNumber getMaxVerificationIterationFactor() const;
        bool useRelevantValuesForPrecisionUpdate() const;
        storm::RationalNumber getUpperBoundGuessingFactor() const;
        uint64_t getUpperBoundOnlyIterations() const;
        
    private:
        storm::RationalNumber precisionUpdateFactor;
        storm::RationalNumber maxVerificationIterationFactor;
        bool relevantValuesForPrecisionUpdate;
        storm::RationalNumber upperBoundGuessingFactor;
        uint64_t upperBoundOnlyIterations;
    };
}

