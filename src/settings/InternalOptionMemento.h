#ifndef STORM_SETTINGS_INTERNALOPTIONMEMENTO_H_
#define STORM_SETTINGS_INTERNALOPTIONMEMENTO_H_

#include "src/settings/Settings.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {
namespace settings {

	/*!
	 *	@brief	THIS CLASS IS FOR INTERNAL USE IN THE TESTS ONLY
	 *
	 *
	 *	If an option is required to be set when executing a test
	 *	the test may instantiate an Object of this class while the
	 *	test itself is executed.
	 *
	 *	This class ensures that the option has the requested value
	 *	and resets it to its previous value as soon as its destructor
	 *	is called.
	 */
	class InternalOptionMemento {
		public:
			/*!
			 *	@brief The constructor.
			 *
			 *	@param 
			 */
			InternalOptionMemento(std::string const& longOptionName, bool requiredHasBeenSetState): instance(storm::settings::SettingsManager::getInstance()), optionName(longOptionName), stateRequired(requiredHasBeenSetState) {
				this->stateBefore = instance->isSet(optionName);
				if (stateRequired) {
					instance->set(optionName);
				} else {
					instance->unset(optionName);
				}
			}
			
			/*!
			 *	@brief	The destructor.
			 *
			 *	Resets the Options state to its previous state
			 */
			virtual ~InternalOptionMemento() {
				// Reset the state of the Option
				if (stateBefore) {
					instance->set(optionName);
				} else {
					instance->unset(optionName);
				}
				this->instance = nullptr;
			}


			
		private:
			storm::settings::SettingsManager* instance;
			std::string const optionName;
			bool stateBefore;
			bool stateRequired;

			InternalOptionMemento(InternalOptionMemento& other) {}
	};

} // namespace settings
} // namespace storm

#endif // STORM_SETTINGS_INTERNALOPTIONMEMENTO_H_