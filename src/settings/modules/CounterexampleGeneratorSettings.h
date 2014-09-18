#ifndef STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_
#define STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for counterexample generation.
             */
            class CounterexampleGeneratorSettings : public ModuleSettings {
            public:
                CounterexampleGeneratorSettings(storm::settings::SettingsManager& settingsManager);
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_ */
