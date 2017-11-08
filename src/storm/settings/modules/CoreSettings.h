#ifndef STORM_SETTINGS_MODULES_CoreSettings_H_
#define STORM_SETTINGS_MODULES_CoreSettings_H_

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

#include "storm/builder/ExplorationOrder.h"

namespace storm {
    namespace solver {
        enum class EquationSolverType;
        enum class LpSolverType;
        enum class MinMaxMethod;
        enum class SmtSolverType;
    }
    
    namespace dd {
        enum class DdType;
    }

    namespace settings {
        namespace modules {

            /*!
             * This class represents the core settings.
             */
            class CoreSettings : public ModuleSettings {
            public:
                // An enumeration of all engines.
                enum class Engine {
                    Sparse, Hybrid, Dd, Exploration, AbstractionRefinement
                };

                /*!
                 * Creates a new set of core settings.
                 */
                CoreSettings();

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
                 * Retrieves the selected equation solver.
                 *
                 * @return The selected convergence criterion.
                 */
                storm::solver::EquationSolverType getEquationSolver() const;

                /*!
                 * Retrieves whether a equation solver has been set.
                 *
                 * @return True iff an equation solver has been set.
                 */
                bool isEquationSolverSet() const;

                /*!
                 * Retrieves whether the equation solver has been set from its default value.
                 *
                 * @return True iff it has been set from its default value.
                 */
                bool isEquationSolverSetFromDefaultValue() const;
                
                /*!
                 * Retrieves the selected LP solver.
                 *
                 * @return The selected LP solver.
                 */
                storm::solver::LpSolverType getLpSolver() const;

                /*!
                 * Retrieves the selected SMT solver.
                 *
                 * @return The selected SMT solver.
                 */
                storm::solver::SmtSolverType getSmtSolver() const;
                
                /*!
                 * Retrieves the selected library for DD-related operations.
                 *
                 * @return The selected library.
                 */
                storm::dd::DdType getDdLibraryType() const;
                
                /*!
                 * Retrieves whether the selected DD library is set from its default value.
                 *
                 * @return True iff if it is set from its default value.
                 */
                bool isDdLibraryTypeSetFromDefaultValue() const;
                
                /*!
                 * Retrieves whether statistics are to be shown
                 *
                 * @return True iff statistics are to be shown
                 */
                bool isShowStatisticsSet() const;

                /*!
                 * Retrieves whether the option to use Intel TBB is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isUseIntelTbbSet() const;

                /*!
                 * Retrieves whether the option to use CUDA is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isUseCudaSet() const;

                /*!
                 * Retrieves the selected engine.
                 *
                 * @return The selected engine.
                 */
                Engine getEngine() const;

                /*!
                 * Sets the engine for further usage.
                 */
                void setEngine(Engine);

                bool check() const override;
                void finalize() override;

                // The name of the module.
                static const std::string moduleName;

            private:
                Engine engine;

                // Define the string names of the options as constants.
                static const std::string counterexampleOptionName;
                static const std::string counterexampleOptionShortName;
                static const std::string dontFixDeadlockOptionName;
                static const std::string dontFixDeadlockOptionShortName;
                static const std::string eqSolverOptionName;
                static const std::string lpSolverOptionName;
                static const std::string smtSolverOptionName;
                static const std::string statisticsOptionName;
                static const std::string statisticsOptionShortName;
                static const std::string engineOptionName;
                static const std::string engineOptionShortName;
                static const std::string ddLibraryOptionName;
                static const std::string intelTbbOptionName;
                static const std::string intelTbbOptionShortName;
                static const std::string cudaOptionName;
            };

        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_CoreSettings_H_ */
