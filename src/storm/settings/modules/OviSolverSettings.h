#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {

/*!
 * This class represents the settings for the optimistic value iteration solver.
 */
class OviSolverSettings : public ModuleSettings {
   public:
    OviSolverSettings();

    /*!
     * @return true if the parameter controlling how optimistic upper bounds are guessed has been set by the user
     */
    bool hasUpperBoundGuessingFactorBeenSet() const;

    /*!
     * @return the parameter controlling how optimistic upper bounds are guessed
     */
    double getUpperBoundGuessingFactor() const;

    // The name of the module.
    static const std::string moduleName;

   private:
    static const std::string upperBoundGuessingFactorOptionName;
};

}  // namespace modules
}  // namespace settings
}  // namespace storm
