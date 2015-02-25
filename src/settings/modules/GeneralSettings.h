#ifndef STORM_SETTINGS_MODULES_GENERALSETTINGS_H_
#define STORM_SETTINGS_MODULES_GENERALSETTINGS_H_

#include "storm-config.h"
#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the general settings.
             */
            class GeneralSettings : public ModuleSettings {
            public:                
                // An enumeration of all engines.
                enum class Engine { Sparse, Dd };
                
                // An enumeration of all available LP solvers.
                enum class LpSolver { Gurobi, glpk };
                
                // An enumeration of all available equation solvers.
                enum class EquationSolver { Gmmxx, Native };
                
                /*!
                 * Creates a new set of general settings that is managed by the given manager.
                 *
                 * @param settingsManager The responsible manager.
                 */
                GeneralSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves whether the help option was set.
                 *
                 * @return True if the help option was set.
                 */
                bool isHelpSet() const;
                
				/*!
                 * Retrieves whether the version option was set.
                 *
                 * @return True if the version option was set.
                 */
                bool isVersionSet() const;
				
                /*!
                 * Retrieves the name of the module for which to show the help or "all" to indicate that the full help
                 * needs to be shown.
                 *
                 * @return The name of the module for which to show the help or "all".
                 */
                std::string getHelpModuleName() const;
                
                /*!
                 * Retrieves whether the verbose option was set.
                 *
                 * @return True if the verbose option was set.
                 */
                bool isVerboseSet() const;
                
                /*!
                 * Retrieves the precision to use for numerical operations.
                 *
                 * @return The precision to use for numerical operations.
                 */
                double getPrecision() const;
                
                /*!
                 * Retrieves whether the export-to-dot option was set.
                 *
                 * @return True if the export-to-dot option was set.
                 */
                bool isExportDotSet() const;
                
                /*!
                 * Retrieves the name in which to write the model in dot format, if the export-to-dot option was set.
                 *
                 * @return The name of the file in which to write the exported model.
                 */
                std::string getExportDotFilename() const;
                
                /*!
                 * Retrieves whether the config option was set.
                 *
                 * @return True if the config option was set.
                 */
                bool isConfigSet() const;
                
                /*!
                 * Retrieves the name of the file that is to be scanned for settings.
                 *
                 * @return The name of the file that is to be scanned for settings.
                 */
                std::string getConfigFilename() const;
                
                /*!
                 * Retrieves whether the explicit option was set.
                 *
                 * @return True if the explicit option was set.
                 */
                bool isExplicitSet() const;
                
                /*!
                 * Retrieves the name of the file that contains the transitions if the model was given using the explicit
                 * option.
                 *
                 * @return The name of the file that contains the transitions.
                 */
                std::string getTransitionFilename() const;

                /*!
                 * Retrieves the name of the file that contains the state labeling if the model was given using the
                 * explicit option.
                 *
                 * @return The name of the file that contains the state labeling.
                 */
                std::string getLabelingFilename() const;
                
                /*!
                 * Retrieves whether the symbolic option was set.
                 *
                 * @return True if the symbolic option was set.
                 */
                bool isSymbolicSet() const;
                
                /*!
                 * Retrieves the name of the file that contains the symbolic model specification if the model was given
                 * using the symbolic option.
                 *
                 * @return The name of the file that contains the symbolic model specification.
                 */
                std::string getSymbolicModelFilename() const;
                
                /*!
                 * Retrieves whether the name of a reward model was passed to the symbolic option.
                 *
                 * @return True iff the name of a reward model was passed to the symbolic option.
                 */
                bool isSymbolicRewardModelNameSet() const;
                
                /*!
                 * Retrieves the name of the reward model if one was set using the symbolic option.
                 *
                 * @return The name of the selected reward model.
                 */
                std::string getSymbolicRewardModelName() const;

                /*!
                 * Retrieves whether the property option was set.
                 *
                 * @return True if the property option was set.
                 */
                bool isPropertySet() const;
                
                /*!
                 * Retrieves the property specified with the property option.
                 *
                 * @return The property specified with the property option.
                 */
                std::string getProperty() const;
                
                /*!
                 * Retrieves whether a property file was set.
                 *
                 * @return True iff a property was set.
                 */
                bool isPropertyFileSet() const;
                
                /*!
                 * Retrieves the name of the file that contains the properties to be checked on the model.
                 *
                 * @return The name of the file that contains the properties to be checked on the model.
                 */
                std::string getPropertiesFilename() const;

                /*!
                 * Retrieves whether the transition reward option was set.
                 *
                 * @return True if the transition reward option was set.
                 */
                bool isTransitionRewardsSet() const;

                /*!
                 * Retrieves the name of the file that contains the transition rewards if the model was given using the
                 * explicit option.
                 *
                 * @return The name of the file that contains the transition rewards.
                 */
                std::string getTransitionRewardsFilename() const;
                
                /*!
                 * Retrieves whether the state reward option was set.
                 *
                 * @return True if the state reward option was set.
                 */
                bool isStateRewardsSet() const;
                
                /*!
                 * Retrieves the name of the file that contains the state rewards if the model was given using the
                 * explicit option.
                 *
                 * @return The name of the file that contains the state rewards.
                 */
                std::string getStateRewardsFilename() const;
                
                /*!
                 * Retrieves whether the counterexample option was set.
                 *
                 * @return True if the counterexample option was set.
                 */
                bool isCounterexampleSet() const;

                /*!
                 * Retrieves the name of the file to which the counterexample is to be written if the counterexample
                 * option was set.
                 *
                 * @return The name of the file to which the counterexample is to be written.
                 */
                std::string getCounterexampleFilename() const;
                
                /*!
                 * Retrieves whether the dont-fix-deadlocks option was set.
                 *
                 * @return True if the dont-fix-deadlocks option was set.
                 */
                bool isDontFixDeadlocksSet() const;

                /*!
                 * Overrides the option to not fix deadlocks by setting it to the specified value. As soon as the
                 * returned memento goes out of scope, the original value is restored.
                 *
                 * @param stateToSet The value that is to be set for the fix-deadlocks option.
                 * @return The memento that will eventually restore the original value.
                 */
                std::unique_ptr<storm::settings::SettingMemento> overrideDontFixDeadlocksSet(bool stateToSet);
                
                /*!
                 * Retrieves whether the timeout option was set.
                 *
                 * @return True if the timeout option was set.
                 */
                bool isTimeoutSet() const;
                
                /*!
                 * Retrieves the time after which the computation has to be aborted in case the timeout option was set.
                 *
                 * @return The number of seconds after which to timeout.
                 */
                uint_fast64_t getTimeoutInSeconds() const;

                /*!
                 * Retrieves the selected equation solver.
                 *
                 * @return The selected convergence criterion.
                 */
                EquationSolver getEquationSolver() const;
                
                /*!
                 * Retrieves the selected LP solver.
                 *
                 * @return The selected LP solver.
                 */
                LpSolver getLpSolver() const;
                
                /*!
                 * Retrieves whether the export-to-dot option was set.
                 *
                 * @return True if the export-to-dot option was set.
                 */
                bool isConstantsSet() const;
                
                /*!
                 * Retrieves the string that defines the constants of a symbolic model (given via the symbolic option).
                 *
                 * @return The string that defines the constants of a symbolic model.
                 */
                std::string getConstantDefinitionString() const;
                
                /*!
                 * Retrieves whether statistics are to be shown for counterexample generation.
                 *
                 * @return True iff statistics are to be shown for counterexample generation.
                 */
                bool isShowStatisticsSet() const;
                
                /*!
                 * Retrieves whether the option to perform bisimulation minimization is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isBisimulationSet() const;

                /*!
                 * Retrieves whether the option to use CUDA is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isCudaSet() const;
                
                /*!
                 * Retrieves the selected engine.
                 *
                 * @return The selecte engine.
                 */
                Engine getEngine() const;
                
#ifdef STORM_HAVE_CARL
                /*!
                 * Retrieves whether the option enabling parametric model checking is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isParametricSet() const;
#endif
                
                bool check() const override;

                // The name of the module.
                static const std::string moduleName;

            private:
                // Define the string names of the options as constants.
                static const std::string helpOptionName;
                static const std::string helpOptionShortName;
				static const std::string versionOptionName;
                static const std::string verboseOptionName;
                static const std::string verboseOptionShortName;
                static const std::string precisionOptionName;
                static const std::string precisionOptionShortName;
                static const std::string exportDotOptionName;
                static const std::string configOptionName;
                static const std::string configOptionShortName;
                static const std::string explicitOptionName;
                static const std::string explicitOptionShortName;
                static const std::string symbolicOptionName;
                static const std::string symbolicOptionShortName;
                static const std::string propertyOptionName;
                static const std::string propertyFileOptionName;
                static const std::string transitionRewardsOptionName;
                static const std::string stateRewardsOptionName;
                static const std::string counterexampleOptionName;
                static const std::string counterexampleOptionShortName;
                static const std::string dontFixDeadlockOptionName;
                static const std::string dontFixDeadlockOptionShortName;
                static const std::string timeoutOptionName;
                static const std::string timeoutOptionShortName;
                static const std::string eqSolverOptionName;
                static const std::string lpSolverOptionName;
                static const std::string constantsOptionName;
                static const std::string constantsOptionShortName;
                static const std::string statisticsOptionName;
                static const std::string statisticsOptionShortName;
                static const std::string bisimulationOptionName;
                static const std::string bisimulationOptionShortName;
                static const std::string engineOptionName;
                static const std::string engineOptionShortName;
                static const std::string cudaOptionName;
                
#ifdef STORM_HAVE_CARL
                static const std::string parametricOptionName;
#endif
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_GENERALSETTINGS_H_ */
