#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {
class PrismExportSettings : public ModuleSettings {
   public:
    /*!
     * Creates a new PrismExport setting
     */
    PrismExportSettings();

    bool isExportFlattenedSet() const;
    bool isSimplifySet() const;

    bool check() const override;
    void finalize() override;

    static const std::string moduleName;

   private:
    static const std::string exportFlattenOptionName;
    static const std::string exportSimplifyOptionName;
};
}  // namespace modules
}  // namespace settings
}  // namespace storm
