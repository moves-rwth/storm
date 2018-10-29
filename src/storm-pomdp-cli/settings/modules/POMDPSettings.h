#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"
#include "storm-pomdp/storage/PomdpMemory.h"

#include "storm-dft/builder/DftExplorationHeuristic.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for DFT model checking.
             */
            class POMDPSettings : public ModuleSettings {
            public:

                /*!
                 * Creates a new set of DFT settings.
                 */
                POMDPSettings();

                virtual ~POMDPSettings() = default;
                
                bool isExportToParametricSet() const;
                std::string getExportToParametricFilename() const;
                
                bool isQualitativeReductionSet() const;
                bool isAnalyzeUniqueObservationsSet() const;
                bool isMecReductionSet() const;
                bool isSelfloopReductionSet() const;
                bool isTransformSimpleSet() const;
                bool isTransformBinarySet() const;
                std::string getFscApplicationTypeString() const;
                uint64_t getMemoryBound() const;
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
