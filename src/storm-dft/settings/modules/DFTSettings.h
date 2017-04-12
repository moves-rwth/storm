#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

#include "storm-dft/builder/DftExplorationHeuristic.h"

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
                 * Retrieves whether the dft file option for Json was set.
                 *
                 * @return True if the dft file option was set.
                 */
                bool isDftJsonFileSet() const;

                /*!
                 * Retrieves the name of the json file that contains the dft specification.
                 *
                 * @return The name of the json file that contains the dft specification.
                 */
                std::string getDftJsonFilename() const;

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
                 * Retrieves the relative error allowed for approximating the model checking result.
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
                 * Retrieves the timebound for the timebound property.
                 *
                 * @return The timebound.
                 */
                double getPropTimebound() const;

                /*!
                 * Retrieves whether the property timepoints should be used.
                 *
                 * @return True iff the option was set.
                 */
                bool usePropTimepoints() const;

                /*!
                 * Retrieves the settings for the timepoints property.
                 *
                 * @return The timepoints.
                 */
                std::vector<double> getPropTimepoints() const;

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
                 * Retrieves whether the non-determinism should be avoided by always taking the first possible dependency.
                 *
                 * @return True iff the option was set.
                 */
                bool isTakeFirstDependency() const;

                /*!
                 * Retrieves whether the DFT should be transformed into a GSPN.
                 *
                 * @return True iff the option was set.
                 */
                bool isTransformToGspn() const;

                /*!
                 * Retrieves whether the export to Json file option was set.
                 *
                 * @return True if the export to json file option was set.
                 */
                bool isExportToJson() const;

                /*!
                 * Retrieves the name of the json file to export to.
                 *
                 * @return The name of the json file to export to.
                 */
                std::string getExportJsonFilename() const;
                
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
                static const std::string dftJsonFileOptionName;
                static const std::string dftJsonFileOptionShortName;
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
                static const std::string propTimeboundOptionName;
                static const std::string propTimepointsOptionName;
                static const std::string minValueOptionName;
                static const std::string maxValueOptionName;
                static const std::string firstDependencyOptionName;
#ifdef STORM_HAVE_Z3
                static const std::string solveWithSmtOptionName;
#endif
                static const std::string transformToGspnOptionName;
                static const std::string exportToJsonOptionName;
                
            };

        } // namespace modules
    } // namespace settings
} // namespace storm
