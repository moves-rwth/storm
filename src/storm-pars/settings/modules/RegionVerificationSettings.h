#pragma once

#include "storm/settings/modules/ModuleSettings.h"
#include "storm-pars/modelchecker/region/RegionCheckEngine.h"

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


}