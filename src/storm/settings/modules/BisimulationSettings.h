#ifndef STORM_SETTINGS_MODULES_BISIMULATIONSETTINGS_H_
#define STORM_SETTINGS_MODULES_BISIMULATIONSETTINGS_H_

#include "storm/settings/modules/ModuleSettings.h"

#include "storm/storage/dd/bisimulation/SignatureMode.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the bisimulation settings.
             */
            class BisimulationSettings : public ModuleSettings {
            public:
                // An enumeration of all available bisimulation types.
                enum class BisimulationType { Strong, Weak };
                
                enum class QuotientFormat { Sparse, Dd };
                
                enum class ReuseMode { None, BlockNumbers };
                
                /*!
                 * Creates a new set of bisimulation settings.
                 */
                BisimulationSettings();
                
                /*!
                 * Retrieves whether strong bisimulation is to be used.
                 *
                 * @return True iff strong bisimulation is to be used.
                 */
                bool isStrongBisimulationSet() const;

                /*!
                 * Retrieves whether weak bisimulation is to be used.
                 *
                 * @return True iff weak bisimulation is to be used.
                 */
                bool isWeakBisimulationSet() const;

                /*!
                 * Retrieves the format in which the quotient is to be extracted.
                 * NOTE: only applies to DD-based bisimulation.
                 */
                QuotientFormat getQuotientFormat() const;
                
                /*!
                 * Retrieves whether representatives for blocks are to be used instead of the block numbers.
                 * NOTE: only applies to DD-based bisimulation.
                 */
                bool isUseRepresentativesSet() const;
                
                /*!
                 * Retrieves the mode to compute signatures.
                 */
                storm::dd::bisimulation::SignatureMode getSignatureMode() const;
                
                /*!
                 * Retrieves the selected reuse mode.
                 */
                ReuseMode getReuseMode() const;
                
                virtual bool check() const override;
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                // Define the string names of the options as constants.
                static const std::string typeOptionName;
                static const std::string representativeOptionName;
                static const std::string quotientFormatOptionName;
                static const std::string signatureModeOptionName;
                static const std::string reuseOptionName;
            };
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_BISIMULATIONSETTINGS_H_ */
