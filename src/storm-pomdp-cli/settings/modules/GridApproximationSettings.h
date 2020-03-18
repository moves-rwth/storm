#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for POMDP model checking.
             */
            class GridApproximationSettings : public ModuleSettings {
            public:

                /*!
                 * Creates a new set of POMDP settings.
                 */
                GridApproximationSettings();

                virtual ~GridApproximationSettings() = default;
                
                bool isRefineSet() const;
                double getRefinementPrecision() const;
                uint64_t getGridResolution() const;
                double getExplorationThreshold() const;
                bool isNumericPrecisionSetFromDefault() const;
                double getNumericPrecision() const;
                bool isCacheSimplicesSet() const;
                
                // The name of the module.
                static const std::string moduleName;

            private:

                
            };

        } // namespace modules
    } // namespace settings
} // namespace storm
