#pragma once

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            class JitBuilderSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new JitBuilder settings object.
                 */
                JitBuilderSettings();
                
//                bool isNoJitSet() const;

                bool isDoctorSet() const;
                
                bool check() const override;
                void finalize() override;
                
                static const std::string moduleName;
                
            private:
                static const std::string noJitOptionName;
                static const std::string doctorOptionName;
            };
            
        }
    }
}
