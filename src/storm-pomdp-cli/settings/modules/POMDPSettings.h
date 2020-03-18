#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"
#include "storm-pomdp/storage/PomdpMemory.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for POMDP model checking.
             */
            class POMDPSettings : public ModuleSettings {
            public:

                /*!
                 * Creates a new set of POMDP settings.
                 */
                POMDPSettings();

                virtual ~POMDPSettings() = default;
                
                bool isExportToParametricSet() const;
                std::string getExportToParametricFilename() const;
                
                bool isQualitativeReductionSet() const;

                bool isGridApproximationSet() const;
                bool isLimitExplorationSet() const;
                bool isAnalyzeUniqueObservationsSet() const;
                bool isMecReductionSet() const;
                bool isSelfloopReductionSet() const;
                bool isTransformSimpleSet() const;
                bool isTransformBinarySet() const;
                bool isMemlessSearchSet() const;
                bool isCheckFullyObservableSet() const;
                std::string getMemlessSearchMethod() const;
                std::string getFscApplicationTypeString() const;
                uint64_t getMemoryBound() const;

                uint64_t getGridResolution() const;
                double getExplorationThreshold() const;
                storm::storage::PomdpMemoryPattern getMemoryPattern() const;
                
                bool check() const override;
                void finalize() override;

                // The name of the module.
                static const std::string moduleName;

            private:

                
            };

        } // namespace modules
    } // namespace settings
} // namespace storm
