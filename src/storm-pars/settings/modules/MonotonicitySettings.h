#ifndef STORM_SETTINGS_MODULES_MONOTONICITYSETTINGS_H_
#define STORM_SETTINGS_MODULES_MONOTONICITYSETTINGS_H_

#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for monotonicity checking.
             */
            class MonotonicitySettings : public ModuleSettings {
            public:

                /*!
                 * Creates a new set of monotonicity checking settings.
                 */
                MonotonicitySettings();

                /*!
                 * Retrieves whether monotonicity analysis should be applied.
                 */
                bool isMonotonicityAnalysisSet() const;

                /*!
                 * Retrieves whether SCCs in the monotonicity analysis should be eliminated.
                 */
                bool isSccEliminationSet() const;

                /*!
                 * Retrieves whether assumptions in monotonicity analysis should be validated
                 */
                bool isValidateAssumptionsSet() const;

                /*!
                 * Retrieves the number of samples used for sampling in the monotonicity analysis
                 */
                uint_fast64_t getNumberOfSamples() const;

                /*!
				 * Retrieves the precision for the extremal value
				 */
                double getMonotonicityAnalysisPrecision() const;

                const static std::string moduleName;

            private:
                const static std::string monotonicityAnalysis;
                const static std::string sccElimination;
                const static std::string validateAssumptions;
                const static std::string samplesMonotonicityAnalysis;
                const static std::string precision;
            };

        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_MONOTONICITYSETTINGS_H_ */
