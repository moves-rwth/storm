#ifndef STORM_SETTINGS_MODULES_NATIVEEQUATIONSOLVERSETTINGS_H_
#define STORM_SETTINGS_MODULES_NATIVEEQUATIONSOLVERSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for the native equation solver.
             */
            class NativeEquationSolverSettings : public ModuleSettings {
            public:
                // An enumeration of all available techniques for solving linear equations.
                enum class LinearEquationTechnique { Jacobi };
                
                // An enumeration of all available convergence criteria.
                enum class ConvergenceCriterion { Absolute, Relative };
                
                NativeEquationSolverSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves the technique that is to be used for solving systems of linear equations.
                 *
                 * @return The technique to use.
                 */
                LinearEquationTechnique getLinearEquationSystemTechnique() const;
                
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
                 * Retrieves the selected convergence criterion.
                 *
                 * @return The selected convergence criterion.
                 */
                ConvergenceCriterion getConvergenceCriterion() const;
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                // Define the string names of the options as constants.
                static const std::string techniqueOptionName;
                static const std::string maximalIterationsOptionName;
                static const std::string maximalIterationsOptionShortName;
                static const std::string precisionOptionName;
                static const std::string absoluteOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_NATIVEEQUATIONSOLVERSETTINGS_H_ */
