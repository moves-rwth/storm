#pragma once

#include "storm/settings/modules/ModuleSettings.h"

namespace storm::settings::modules {
/*!
 * This class represents the settings for parametric model checking.
 */
class PartitionSettings : public ModuleSettings {
   public:
    PartitionSettings();
    /*!
     * Retrieves the threshold considered for iterative region refinement.
     * The refinement converges as soon as the fraction of unknown area falls below this threshold
     */
    double getCoverageThreshold() const;

    /*!
     * Retrieves whether a depth threshold has been set for refinement
     */
    bool isDepthLimitSet() const;

    /*!
     * Returns the depth threshold (if set).
     * It is illegal to call this method if no depth threshold has been set.
     */
    uint64_t getDepthLimit() const;

    /*!
     * Retrieves whether no illustration of the result should be printed.
     */
    bool isPrintNoIllustrationSet() const;

    /*!
     * Retrieves whether the full result should be printed
     */
    bool isPrintFullResultSet() const;

    const static std::string moduleName;
};
}  // namespace storm::settings::modules
