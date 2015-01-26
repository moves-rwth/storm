#ifndef STORM_SETTINGS_MODULES_SPARSEDTMCELIMINATIONMODELCHECKERSETTINGS_H_
#define STORM_SETTINGS_MODULES_SPARSEDTMCELIMINATIONMODELCHECKERSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for the elimination-based DTMC model checker.
             */
            class SparseDtmcEliminationModelCheckerSettings : public ModuleSettings {
            public:
                /*!
                 * An enum that contains all available state elimination orders.
                 */
                enum class EliminationOrder { Forward, ForwardReversed, Backward, BackwardReversed, Random };
				
                /*!
                 * An enum that contains all available techniques to solve parametric systems.
                 */
                enum class EliminationMethod { State, Scc, Hybrid};
                
                /*!
                 * Creates a new set of parametric model checking settings that is managed by the given manager.
                 *
                 * @param settingsManager The responsible manager.
                 */
                SparseDtmcEliminationModelCheckerSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves the selected elimination method.
                 *
                 * @return The selected elimination method.
                 */
                EliminationMethod getEliminationMethod() const;
                
                /*!
                 * Retrieves the selected elimination order.
                 *
                 * @return The selected elimination order.
                 */
                EliminationOrder getEliminationOrder() const;
                
                /*!
                 * Retrieves whether the option to eliminate entry states in the very end is set.
                 *
                 * @return True iff the option is set.
                 */
                bool isEliminateEntryStatesLastSet() const;
                
                /*!
                 * Retrieves the maximal size of an SCC on which state elimination is to be directly applied.
                 *
                 * @return The maximal size of an SCC on which state elimination is to be directly applied.
                 */
                uint_fast64_t getMaximalSccSize() const;
				
                const static std::string moduleName;
                
            private:
                const static std::string eliminationMethodOptionName;
                const static std::string eliminationOrderOptionName;
                const static std::string entryStatesLastOptionName;
                const static std::string maximalSccSizeOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_SPARSEDTMCELIMINATIONMODELCHECKERSETTINGS_H_ */