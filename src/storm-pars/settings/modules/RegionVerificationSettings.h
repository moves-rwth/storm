#pragma once

#include "storm-pars/modelchecker/region/RegionCheckEngine.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm::settings::modules {

class RegionVerificationSettings : public ModuleSettings {
   public:
    RegionVerificationSettings();

    bool isSplittingThresholdSet() const;

    int getSplittingThreshold() const;

    /*!
     * Retrieves which type of region check should be performed
     */
    storm::modelchecker::RegionCheckEngine getRegionCheckEngine() const;

    const static std::string moduleName;
};

}  // namespace storm::settings::modules