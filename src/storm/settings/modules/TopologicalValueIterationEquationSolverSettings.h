#ifndef STORM_SETTINGS_MODULES_TOPOLOGICALVALUEITERATIONSETTINGS_H_
#define STORM_SETTINGS_MODULES_TOPOLOGICALVALUEITERATIONSETTINGS_H_

#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for topological value iteration.
             */
			class TopologicalValueIterationEquationSolverSettings : public ModuleSettings {
            public:

				// An enumeration of all available convergence criteria.
				enum class ConvergenceCriterion { Absolute, Relative };

                /*!
                 * Creates a new set of topological value iteration settings.
                 */
				TopologicalValueIterationEquationSolverSettings();

                
                /*!
                 * Retrieves whether the maximal iteration count has been set.
                 *
                 * @return True iff the maximal iteration count has been set.
                 */
                bool isMaximalIterationCountSet() const;
                
                /*!
                 * Retrieves the maximal number of iterations to perform until giving up on converging.
                 *
                 * @return The maximal iteration count.
                 */
                uint_fast64_t getMaximalIterationCount() const;
                
                /*!
                 * Retrieves whether the precision has been set.
                 *
                 * @return True iff the precision has been set.
                 */
                bool isPrecisionSet() const;
                
                /*!
                 * Retrieves the precision that is used for detecting convergence.
                 *
                 * @return The precision to use for detecting convergence.
                 */
                double getPrecision() const;

				/*!
				* Retrieves whether the convergence criterion has been set.
				*
				* @return True iff the convergence criterion has been set.
				*/
				bool isConvergenceCriterionSet() const;

				/*!
				* Retrieves the selected convergence criterion.
				*
				* @return The selected convergence criterion.
				*/
				ConvergenceCriterion getConvergenceCriterion() const;
                
                bool check() const override;
                
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

#endif /* STORM_SETTINGS_MODULES_TOPOLOGICALVALUEITERATIONSETTINGS_H_ */
