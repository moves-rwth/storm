#pragma once

#include "storm-pars/utility/FeasibilitySynthesisTask.h"
#include "storm/settings/modules/ModuleSettings.h"
#include "storm/solver/OptimizationDirection.h"

namespace storm::settings::modules {
/*!
 * This class represents the settings for parametric model checking.
 */
class FeasibilitySettings : public ModuleSettings {
   public:
    FeasibilitySettings();

    /*!
     * Get the feasibility method to be used.
     */
    storm::pars::FeasibilityMethod getFeasibilityMethod() const;

    /*!
     * Retrieves whether an optimal value guarantee has been provided.
     */
    bool hasOptimalValueGuaranteeBeenSet() const;
    /*!
     * Retrieves the precision for the extremal value
     */
    double getOptimalValueGuarantee() const;

    /*!
     * Should the guarantee for the optimal value be absolute or relative.
     */
    bool isAbsolutePrecisionSet() const;

    /*!
     * Retrieves whether an extremal value is to be computed
     */
    bool isParameterDirectionSet() const;

    /*!
     * Retrieves whether to minimize or maximize over parameters
     */
    storm::solver::OptimizationDirection getParameterDirection() const;

    const static std::string moduleName;
};

}  // namespace storm::settings::modules