#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"


namespace storm {
    namespace settings {
        namespace modules {
            class AigerExportSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new AigerExport setting
                 */
                AigerExportSettings();
                
                bool check() const override;
                void finalize() override;
                
                static const std::string moduleName;

            };
        }
    }
}
