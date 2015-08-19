#ifndef STORM_SETTINGS_MODULES_REGIONSETTINGS_H_
#define STORM_SETTINGS_MODULES_REGIONSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for parametric region model checking.
             */
            class RegionSettings : public ModuleSettings {
            public:

                enum class ApproxMode {OFF, GUESSALLSAT, GUESSALLVIOLATED, TESTFIRST };
                enum class SampleMode {OFF, INSTANTIATE, EVALUATE };
                enum class SmtMode {OFF, FUNCTION, MODEL };

                /*!
                 * Creates a new set of parametric region model checking settings that is managed by the given manager.
                 *
                 * @param settingsManager The responsible manager.
                 */
                RegionSettings(storm::settings::SettingsManager& settingsManager);

                /*!
                 * Retrieves whether the regions are specified in a file.
                 * @return  True iff the regions are specified in a file.
                 */
                bool isRegionFileSet() const;
                
                /*!
                 * Returns the file in which the regions are specified.
                 */
                std::string getRegionFilePath() const;

                /*!
                 * Retrieves whether the regions are specified as cmd line parameter
                 * @return  True iff the regions are specified as cmd line parameter
                 */
                bool isRegionsSet() const;
                
                /*!
                 * Returns the regions that are specified as cmd line parameter
                 */
                std::string getRegionsFromCmdLine() const;
                
                /*!
                 * Returns the mode in which approximation should be used
                 */
                ApproxMode getApproxMode() const;
                
                /*!
                 * Returns whether to use approximation
                 */
                bool doApprox() const;
                
                /*!
                 * Returns the mode in which Sampling should be used
                 */
                SampleMode getSampleMode() const;
                
                /*!
                 * Returns whether to use Sampling
                 */
                bool doSample() const;
                
                /*!
                 * Returns the mode in which SMT solving should be used
                 */
                SmtMode getSmtMode() const;

                /*!
                 * Returns whether to use SMT Solving
                 */
                bool doSmt() const;
                
                bool check() const override;

                const static std::string moduleName;

            private:
                const static std::string regionfileOptionName;
                const static std::string regionsOptionName;
                const static std::string approxmodeOptionName;
                const static std::string samplemodeOptionName;
                const static std::string smtmodeOptionName;
            };

        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_REGIONSETTINGS_H_ */