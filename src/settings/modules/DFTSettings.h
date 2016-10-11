#ifndef STORM_SETTINGS_MODULES_DFTSETTINGS_H_
#define STORM_SETTINGS_MODULES_DFTSETTINGS_H_

#include "storm-config.h"
#include "src/settings/modules/ModuleSettings.h"
#include "src/builder/DftExplorationHeuristic.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for DFT model checking.
             */
            class DFTSettings : public ModuleSettings {
            public:

                /*!
                 * Creates a new set of DFT settings.
                 */
                DFTSettings();
                
                /*!
                 * Retrieves whether the dft file option was set.
                 *
                 * @return True if the dft file option was set.
                 */
                bool isDftFileSet() const;
                
                /*!
                 * Retrieves the name of the file that contains the dft specification.
                 *
                 * @return The name of the file that contains the dft specification.
                 */
                std::string getDftFilename() const;

                /*!
                 * Retrieves whether the option to use symmetry reduction is set.
                 *
                 * @return True iff the option was set.
                 */
                bool useSymmetryReduction() const;
                
                /*!
                 * Retrieves whether the option to use modularisation is set.
                 *
                 * @return True iff the option was set.
                 */
                bool useModularisation() const;
                
                /*!
                 * Retrieves whether the option to disable Dont Care propagation is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isDisableDC() const;

                /*!
                 * Retrieves whether the option to compute an approximation is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isApproximationErrorSet() const;

                /*!
                 * Retrieves the error allowed for approximation the model checking result.
                 *
                 * @return The allowed errorbound.
                 */
                double getApproximationError() const;

                /*!
                 * Retrieves the heuristic used for approximation.
                 *
                 * @return The heuristic to use.
                 */
                storm::builder::ApproximationHeuristic getApproximationHeuristic() const;

                /*!
                 * Retrieves whether the property expected time should be used.
                 *
                 * @return True iff the option was set.
                 */
                bool usePropExpectedTime() const;
                
                /*!
                 * Retrieves whether the property probability should be used.
                 *
                 * @return True iff the option was set.
                 */
                bool usePropProbability() const;
                
                /*!
                 * Retrieves whether the property timebound should be used.
                 *
                 * @return True iff the option was set.
                 */
                bool usePropTimebound() const;
                
                /*!
                 * Retrieves whether the minimal value should be computed for non-determinism.
                 *
                 * @return True iff the option was set.
                 */
                bool isComputeMinimalValue() const;

                /*!
                 * Retrieves whether the maximal value should be computed for non-determinism.
                 *
                 * @return True iff the option was set.
                 */
                bool isComputeMaximalValue() const;
                
                /*!
                 * Retrieves the timebound for the timebound property.
                 *
                 * @return The timebound.
                 */
                double getPropTimebound() const;
                
#ifdef STORM_HAVE_Z3
                /*!
                 * Retrieves whether the DFT should be checked via SMT.
                 *
                 * @return True iff the option was set.
                 */
                bool solveWithSMT() const;
#endif
                
                bool check() const override;
                void finalize() override;

                // The name of the module.
                static const std::string moduleName;

            private:
               // Define the string names of the options as constants.
                static const std::string dftFileOptionName;
                static const std::string dftFileOptionShortName;
                static const std::string symmetryReductionOptionName;
                static const std::string symmetryReductionOptionShortName;
                static const std::string modularisationOptionName;
                static const std::string disableDCOptionName;
                static const std::string approximationErrorOptionName;
                static const std::string approximationErrorOptionShortName;
                static const std::string approximationHeuristicOptionName;
                static const std::string propExpectedTimeOptionName;
                static const std::string propExpectedTimeOptionShortName;
                static const std::string propProbabilityOptionName;
                static const std::string propTimeBoundOptionName;
                static const std::string minValueOptionName;
                static const std::string maxValueOptionName;
#ifdef STORM_HAVE_Z3
                static const std::string solveWithSmtOptionName;
#endif
                
            };

        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_DFTSETTINGS_H_ */
