#pragma once

#include "storm/settings/modules/ModuleSettings.h"
#include "storm-dft/builder/DftExplorationHeuristic.h"
#include "storm-config.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for DFT model checking.
             */
            class FaultTreeSettings : public ModuleSettings {
            public:

                /*!
                 * Creates a new set of DFT settings.
                 */
                FaultTreeSettings();

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
                 * Retrieves whether the option to allow Dont Care propagation for relevant events is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isAllowDCForRelevantEvents() const;

                /*!
                 * Retrieves whether the option to give relevant events is set.
                 *
                 * @return True iff the option was set.
                 */
                bool areRelevantEventsSet() const;

                /*!
                 * Retrieves the relevant events which should be present throughout the analysis.
                 *
                 * @return The list of relevant events.
                 */
                std::vector<std::string> getRelevantEvents() const;

                /*!
                 * Retrieves whether the labels for claimings should be added in the Markov chain.
                 *
                 * @return True iff the option was set.
                 */
                bool isAddLabelsClaiming() const;

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
                 * Retrieves whether the option to set a maximal exploration depth is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isMaxDepthSet() const;

                /*!
                 * Retrieves the maximal exploration depth.
                 *
                 * @return The maximal exploration depth.
                 */
                uint_fast64_t getMaxDepth() const;

                /*!
                 * Retrieves whether the non-determinism should be avoided by always taking the first possible dependency.
                 *
                 * @return True iff the option was set.
                 */
                bool isTakeFirstDependency() const;

                /*!
                 * Retrieves whether the DFT should be transformed to contain at most one constantly failed BE.
                 *
                 * @return True iff the option was set.
                  */
                bool isUniqueFailedBE() const;

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
                static const std::string noSymmetryReductionOptionName;
                static const std::string noSymmetryReductionOptionShortName;
                static const std::string modularisationOptionName;
                static const std::string disableDCOptionName;
                static const std::string allowDCRelevantOptionName;
                static const std::string relevantEventsOptionName;
                static const std::string addLabelsClaimingOptionName;
                static const std::string approximationErrorOptionName;
                static const std::string approximationErrorOptionShortName;
                static const std::string approximationHeuristicOptionName;
                static const std::string maxDepthOptionName;
                static const std::string firstDependencyOptionName;
                static const std::string uniqueFailedBEOptionName;
#ifdef STORM_HAVE_Z3
                static const std::string solveWithSmtOptionName;
#endif

            };

        } // namespace modules
    } // namespace settings
} // namespace storm
