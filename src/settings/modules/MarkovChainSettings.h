#ifndef STORM_SETTINGS_MODULES_MARKOVCHAINSETTINGS_H_
#define STORM_SETTINGS_MODULES_MARKOVCHAINSETTINGS_H_

#include "storm-config.h"
#include "src/settings/modules/ModuleSettings.h"

#include "src/builder/ExplorationOrder.h"

namespace storm {
    namespace solver {
        enum class EquationSolverType;
        enum class LpSolverType;
        enum class MinMaxTechnique;
        enum class SmtSolverType;
    }
    
    namespace dd {
        enum class DdType;
    }

    namespace settings {
        namespace modules {

            /*!
             * This class represents the markov chain settings.
             */
            class MarkovChainSettings : public ModuleSettings {
            public:
                // An enumeration of all engines.
                enum class Engine {
                    Sparse, Hybrid, Dd, AbstractionRefinement
                };

                /*!
                 * Creates a new set of markov chain settings.
                 */
                MarkovChainSettings();

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
                 * Retrieves whether the model exploration order was set.
                 *
                 * @return True if the model exploration option was set.
                 */
                bool isExplorationOrderSet() const;
                
                /*!
                 * Retrieves the exploration order if it was set.
                 *
                 * @return The chosen exploration order.
                 */
                storm::builder::ExplorationOrder getExplorationOrder() const;

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
                 * Retrieves whether the choice labeling option was set.
                 * 
                 * @return True iff the choice labeling option was set.
                 */
                bool isChoiceLabelingSet() const;

                /*!
                 * Retrieves the name of the file that contains the choice labeling
                 * if the model was given using the explicit option.
                 *
                 * @return The name of the file that contains the choice labeling.
                 */
                std::string getChoiceLabelingFilename() const;

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
                 * Overrides the option to enable the PRISM compatibility mode by setting it to the specified value. As
                 * soon as the returned memento goes out of scope, the original value is restored.
                 *
                 * @param stateToSet The value that is to be set for the option.
                 * @return The memento that will eventually restore the original value.
                 */
                std::unique_ptr<storm::settings::SettingMemento> overridePrismCompatibilityMode(bool stateToSet);

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
                 * Retrieves whether the option to use CUDA is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isCudaSet() const;

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

                /*!
                 * Retrieves whether the PRISM compatibility mode was enabled.
                 *
                 * @return True iff the PRISM compatibility mode was enabled.
                 */
                bool isPrismCompatibilityEnabled() const;

                /*!
                 * Retrieves whether a min/max equation solving technique has been set.
                 *
                 * @return True iff an equation solving technique has been set.
                 */
                bool isMinMaxEquationSolvingTechniqueSet() const;

                /*!
                 * Retrieves the selected min/max equation solving technique.
                 *
                 * @return The selected min/max equation solving technique.
                 */
                storm::solver::MinMaxTechnique getMinMaxEquationSolvingTechnique() const;


                bool check() const override;
                void finalize() override;

                // The name of the module.
                static const std::string moduleName;

            private:
                Engine engine;

                // Define the string names of the options as constants.
                static const std::string exportDotOptionName;
                static const std::string exportMatOptionName;
                static const std::string explicitOptionName;
                static const std::string explicitOptionShortName;
                static const std::string symbolicOptionName;
                static const std::string symbolicOptionShortName;
                static const std::string explorationOrderOptionName;
                static const std::string explorationOrderOptionShortName;
                static const std::string transitionRewardsOptionName;
                static const std::string stateRewardsOptionName;
                static const std::string choiceLabelingOptionName;
                static const std::string counterexampleOptionName;
                static const std::string counterexampleOptionShortName;
                static const std::string dontFixDeadlockOptionName;
                static const std::string dontFixDeadlockOptionShortName;
                static const std::string eqSolverOptionName;
                static const std::string lpSolverOptionName;
                static const std::string smtSolverOptionName;
                static const std::string constantsOptionName;
                static const std::string constantsOptionShortName;
                static const std::string statisticsOptionName;
                static const std::string statisticsOptionShortName;
                static const std::string engineOptionName;
                static const std::string engineOptionShortName;
                static const std::string ddLibraryOptionName;
                static const std::string cudaOptionName;
                static const std::string prismCompatibilityOptionName;
                static const std::string prismCompatibilityOptionShortName;
                static const std::string minMaxEquationSolvingTechniqueOptionName;
            };

        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_MARKOVCHAINSETTINGS_H_ */
