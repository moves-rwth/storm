#pragma once


#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"
#include "storm/builder/ExplorationOrder.h"

namespace storm {
    namespace settings {
        namespace modules {
            class BuildSettings : public ModuleSettings {

            public:

                /*!
                 * Creates a new set of core settings.
                 */
                BuildSettings();
                /*!
                 * Retrieves whether the option to use the JIT builder is set.
                 *
                 * @return True iff the JIT builder is to be used.
                 */
                bool isJitSet() const;

                /*!
                 * Retrieves whether the model exploration order was set.
                 *
                 * @return True if the model exploration option was set.
                 */
                bool isExplorationOrderSet() const;

                /*!
                 * Retrieves whether to perform additional checks during model exploration (e.g. out-of-bounds, etc.).
                 *
                 * @return True if additional checks are to be performed.
                 */
                bool isExplorationChecksSet() const;

                /*!
                 * Retrieves the exploration order if it was set.
                 *
                 * @return The chosen exploration order.
                 */
                storm::builder::ExplorationOrder getExplorationOrder() const;

                /*!
                 * Retrieves whether the PRISM compatibility mode was enabled.
                 *
                 * @return True iff the PRISM compatibility mode was enabled.
                 */
                bool isPrismCompatibilityEnabled() const;

                /**
                 * Retrieves whether no model should be build at all, in case one just want to translate models or parse a file.
                 */
                bool isNoBuildModelSet() const;

                /*!
                 * Retrieves whether the full model should be build, that is, the model including all labels and rewards.
                 *
                 * @return true iff the full model should be build.
                 */
                bool isBuildFullModelSet() const;

                /*!
                 * Retrieves whether the choice labels should be build
                 * @return
                 */
                bool isBuildChoiceLabelsSet() const;

                /*!
                 * Retrieves whether the choice labels should be build
                 * @return
                 */
                bool isBuildStateValuationsSet() const;

                /*!
                 * Retrieves whether out of bounds state should be added
                 * @return
                 */
                bool isBuildOutOfBoundsStateSet() const;
                
                /*!
                 * Retrieves the number of bits that should be used to represent unbounded integer variables
                 * @return
                 */
                uint64_t getBitsForUnboundedVariables() const;


                // The name of the module.
                static const std::string moduleName;
            };
        }
    }
}