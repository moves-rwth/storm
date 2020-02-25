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
              
                double getPrecisionUpdateFactor() const;
                
                double getMaxVerificationIterationFactor() const;
                
                bool useRelevantValuesForPrecisionUpdate() const;
                
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                static const std::string precisionUpdateFactorOptionName;
                static const std::string maxVerificationIterationFactorOptionName;
                static const std::string useRelevantValuesForPrecisionUpdateOptionName;
            };
            
        }
    }
}
