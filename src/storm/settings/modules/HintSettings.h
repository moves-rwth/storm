#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {

/*!
 * This class represents the model transformer settings
 */
class HintSettings : public ModuleSettings {
   public:
    /*!
     * Creates a new set of transformer settings.
     */
    HintSettings();

    /*!
     * Retrieves whether the option that estimates the number of states is set.
     */
    bool isNumberStatesSet() const;

    uint64_t getNumberStates() const;

    bool check() const override;

    void finalize() override;

    // The name of the module.
    static const std::string moduleName;
};

}  // namespace modules
}  // namespace settings
}  // namespace storm
