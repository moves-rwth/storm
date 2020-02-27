#include "storm/environment/solver/OviSolverEnvironment.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/OviSolverSettings.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

namespace storm {
    
    OviSolverEnvironment::OviSolverEnvironment() {
        auto const& oviSettings = storm::settings::getModule<storm::settings::modules::OviSolverSettings>();
        precisionUpdateFactor = storm::utility::convertNumber<storm::RationalNumber>(oviSettings.getPrecisionUpdateFactor());
        maxVerificationIterationFactor = storm::utility::convertNumber<storm::RationalNumber>(oviSettings.getMaxVerificationIterationFactor());
        relevantValuesForPrecisionUpdate = oviSettings.useRelevantValuesForPrecisionUpdate();
        upperBoundGuessingFactor = storm::utility::convertNumber<storm::RationalNumber>(oviSettings.getUpperBoundGuessingFactor());
        upperBoundOnlyIterations = oviSettings.getUpperBoundOnlyIterations();
    }
    
    OviSolverEnvironment::~OviSolverEnvironment() {
        // Intentionally left empty
    }
    
    storm::RationalNumber OviSolverEnvironment::getPrecisionUpdateFactor() const {
        return precisionUpdateFactor;
    }
    
    storm::RationalNumber OviSolverEnvironment::getMaxVerificationIterationFactor() const {
        return maxVerificationIterationFactor;
    }
    
    bool OviSolverEnvironment::useRelevantValuesForPrecisionUpdate() const {
        return relevantValuesForPrecisionUpdate;
    }

    storm::RationalNumber OviSolverEnvironment::getUpperBoundGuessingFactor() const {
        return maxVerificationIterationFactor;
    }

    uint64_t OviSolverEnvironment::getUpperBoundOnlyIterations() const {
        return upperBoundOnlyIterations;
    }
    
}
