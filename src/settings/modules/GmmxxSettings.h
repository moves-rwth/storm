#ifndef STORM_SETTINGS_MODULES_GMMXXSETTINGS_H_
#define STORM_SETTINGS_MODULES_GMMXXSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for gmm++.
             */
            class GmmxxSettings : public ModuleSettings {
            public:
                // An enumeration of all available techniques for solving linear equations.
                enum class LinearEquationTechnique { Bicgstab, Qmr, Gmres, Jacobi };

                // An enumeration of all available preconditioning techniques.
                enum class PreconditioningTechnique { Ilu, Diagonal, Ildlt, None };
                
                /*!
                 * Creates a new set of gmm++ settings that is managed by the given manager.
                 *
                 * @param settingsManager The responsible manager.
                 */
                GmmxxSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves the technique that is to be used for solving systems of linear equations.
                 *
                 * @return The technique to use.
                 */
                LinearEquationTechnique getLinearEquationSystemTechnique() const;
                
                /*!
                 * Retrieves the technique that is to be used for preconditioning solving systems of linear equations.
                 *
                 * @return The technique to use.
                 */
                PreconditioningTechnique getPreconditioningTechnique() const;
                
                /*!
                 * Retrieves the number of iterations after which restarted techniques are to be restarted.
                 *
                 * @return The number of iterations after which to restart.
                 */
                uint_fast64_t getRestartIterationCount() const;
                
                /*!
                 * Retrieves the maximal number of iterations to perform until giving up on converging.
                 *
                 * @return The maximal iteration count.
                 */
                uint_fast64_t getMaximalIterationCount() const;
                
                /*!
                 * Retrieves the precision that is used for detecting convergence.
                 *
                 * @return The precision to use for detecting convergence.
                 */
                double getPrecision() const;
                
                /*!
                 * Retrieves whether the absolute error is used for detecting convergence.
                 *
                 * @return True iff the absolute error is used convergence detection.
                 */
                bool useAbsoluteConvergenceCriterion() const;

                /*!
                 * Retrieves whether the relative error is used for detecting convergence.
                 *
                 * @return True iff the relative error is used convergence detection.
                 */
                bool useRelativeConvergenceCriterion() const;
                
            private:
                // Define the string names of the options as constants.
                static const std::string moduleName;
                static const std::string techniqueOptionName;
                static const std::string preconditionOptionName;
                static const std::string restartOptionName;
                static const std::string maximalIterationsOptionName;
                static const std::string maximalIterationsOptionShortName;
                static const std::string precisionOptionName;
                static const std::string absoluteOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_GMMXXSETTINGS_H_ */
