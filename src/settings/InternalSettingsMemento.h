#ifndef STORM_SETTINGS_INTERNALSETTINGMEMENTO_H_
#define STORM_SETTINGS_INTERNALSETTINGMEMENTO_H_

#include "src/settings/SettingsManager.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {
    namespace settings {
        
        /*!
         * NOTE: THIS CLASS IS FOR INTERNAL USE IN THE TESTS ONLY.
         *
         * If an option is required to be set when executing a test the test may instantiate an object of this class
         * while the test itself is executed. This object then ensures that the option has the requested value and
         * resets it to its previous value as soon as its destructor is called.
         */
        class InternalSettingMemento {
		public:
            /*!
             * Constructs a new memento for the specified option.
             *
             * @param moduleName The name of the module that registered the option.
             * @param longOptionName The long name of the option.
             * @param requiredHasBeenSetState A flag that indicates whether the setting is to be temporarily set to
             * true or false.
             */
			InternalOptionMemento(std::string const& moduleName, std::string const& longOptionName, bool requiredHasBeenSetState): optionName(longOptionName), stateBefore() {
				this->stateBefore = storm::settings::SettingsManager::manager().isSet(optionName);
				if (requiredHasBeenSetState) {
					storm::settings::SettingsManager::manager()..set(optionName);
				} else {
					storm::settings::SettingsManager::manager().unset(optionName);
				}
			}
			
            /*!
             * Destructs the memento object and resets the value of the option to its original state.
             */
			virtual ~InternalOptionMemento() {
				if (stateBefore) {
					storm::settings::SettingsManager::manager().set(optionName);
				} else {
					storm::settings::SettingsManager::manager().unset(optionName);
				}
			}
			
		private:
            // The long name of the option that was temporarily set.
			std::string const optionName;
            
            // The state of the option before it was set.
			bool stateBefore;
        };
        
    } // namespace settings
} // namespace storm

#endif // STORM_SETTINGS_INTERNALSETTINGMEMENTO_H_