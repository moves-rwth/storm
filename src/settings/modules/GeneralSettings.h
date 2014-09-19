#ifndef STORM_SETTINGS_MODULES_GENERALSETTINGS_H_
#define STORM_SETTINGS_MODULES_GENERALSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the general settings.
             */
            class GeneralSettings : public ModuleSettings {
            public:
                GeneralSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves whether the help option was set.
                 *
                 * @return True if the help option was set.
                 */
                bool isHelpSet() const;
                
                /*!
                 * Retrieves whether the verbose option was set.
                 *
                 * @return True if the verbose option was set.
                 */
                bool isVerboseSet() const;
                
                /*!
                 * Retrieves whether the export-to-dot option was set.
                 *
                 * @return True if the export-to-dot option was set.
                 */
                bool isExportDotSet() const;
                
                /*!
                 * Retrieves whether the config option was set.
                 *
                 * @return True if the config option was set.
                 */
                bool isConfigSet() const;
                
                /*!
                 * Retrieves whether the explicit option was set.
                 *
                 * @return True if the explicit option was set.
                 */
                bool isExplicitSet() const;

                /*!
                 * Retrieves whether the symbolic option was set.
                 *
                 * @return True if the symbolic option was set.
                 */
                bool isSymbolicSet() const;

                /*!
                 * Retrieves whether the pctl option was set.
                 *
                 * @return True if the pctl option was set.
                 */
                bool isPctlSet() const;

                /*!
                 * Retrieves whether the csl option was set.
                 *
                 * @return True if the csl option was set.
                 */
                bool isCslSet() const;

                /*!
                 * Retrieves whether the ltl option was set.
                 *
                 * @return True if the ltl option was set.
                 */
                bool isLtlSet() const;

                /*!
                 * Retrieves whether the transition reward option was set.
                 *
                 * @return True if the transition reward option was set.
                 */
                bool isTransitionRewardsSet() const;

                /*!
                 * Retrieves whether the state reward option was set.
                 *
                 * @return True if the state reward option was set.
                 */
                bool isStateRewardsSet() const;
                
                /*!
                 * Retrieves whether the counterexample option was set.
                 *
                 * @return True if the counterexample option was set.
                 */
                bool isCounterexampleSet() const;

                /*!
                 * Retrieves whether the fix-deadlocks option was set.
                 *
                 * @return True if the fix-deadlocks option was set.
                 */
                bool isFixDeadlocksSet() const;

                /*!
                 * Retrieves whether the timeout option was set.
                 *
                 * @return True if the timeout option was set.
                 */
                bool isTimeoutSet() const;

                /*!
                 * Retrieves whether the eqsolver option was set.
                 *
                 * @return True if the eqsolver option was set.
                 */
                bool isEqSolverSet() const;

                /*!
                 * Retrieves whether the export-to-dot option was set.
                 *
                 * @return True if the export-to-dot option was set.
                 */
                bool isLpSolverSet() const;

                /*!
                 * Retrieves whether the export-to-dot option was set.
                 *
                 * @return True if the export-to-dot option was set.
                 */
                bool isConstantsSet() const;
                
            private:
                // Define the string names of the options as constants.
                static const std::string moduleName;
                static const std::string helpOptionName;
                static const std::string helpOptionShortName;
                static const std::string verboseOptionName;
                static const std::string verboseOptionShortName;
                static const std::string exportDotOptionName;
                static const std::string configOptionName;
                static const std::string configOptionShortName;
                static const std::string explicitOptionName;
                static const std::string explicitOptionShortName;
                static const std::string symbolicOptionName;
                static const std::string symbolicOptionShortName;
                static const std::string pctlOptionName;
                static const std::string cslOptionName;
                static const std::string ltlOptionName;
                static const std::string transitionRewardsOptionName;
                static const std::string stateRewardsOptionName;
                static const std::string counterexampleOptionName;
                static const std::string fixDeadlockOptionName;
                static const std::string fixDeadlockOptionShortName;
                static const std::string timeoutOptionName;
                static const std::string timeoutOptionShortName;
                static const std::string eqSolverOptionName;
                static const std::string lpSolverOptionName;
                static const std::string constantsOptionName;
                static const std::string constantsOptionShortName;
                
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_GENERALSETTINGS_H_ */
