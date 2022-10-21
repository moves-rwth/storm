#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {

/*!
 * This class represents the settings for POMDP to Julia export
 */
class ToJuliaSettings : public ModuleSettings {
   public:

    ToJuliaSettings();

    virtual ~ToJuliaSettings() = default;

    bool isExportToJuliaSet() const;
    std::string getExportToJuliaFilename() const;

    bool isDiscountFactorSet() const;
    double getDiscountFactor() const;

    bool check() const override;
    void finalize() override;

    // The name of the module.
    static const std::string moduleName;
};

} // namespace modules
} // namespace settings
} // namespace storm

