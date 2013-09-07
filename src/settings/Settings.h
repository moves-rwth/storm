#ifndef STORM_SETTINGS_SETTINGS_H_
#define STORM_SETTINGS_SETTINGS_H_

#include <iostream>
#include <sstream>
#include <list>
#include <utility>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>


#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBase.h"
#include "src/settings/Argument.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/ArgumentType.h"
#include "src/settings/ArgumentTypeInferationHelper.h"

// Exceptions that should be catched when performing a parsing run
#include "src/exceptions/OptionParserException.h"

namespace storm {

/*!
 *	@brief Contains Settings class and associated methods.
 *
 *	The settings namespace contains the Settings class some friend methods like instance().
 */
namespace settings {
	class Settings;

	typedef std::function<bool (Settings*)> ModuleRegistrationFunction_t;

	typedef bool (*stringValidationFunction_t)(const std::string);
	typedef bool (*integerValidationFunction_t)(const int_fast64_t);
	typedef bool (*unsignedIntegerValidationFunction_t)(const uint_fast64_t);
	typedef bool (*doubleValidationFunction_t)(const double);
	typedef bool (*booleanValidationFunction_t)(const bool);

	typedef std::pair<std::string, std::string> stringPair_t;
	typedef std::pair<bool, std::string> fromStringAssignmentResult_t;

	class Destroyer;

	/*!
	 *	@brief	Wrapper around boost::program_options to handle configuration options.
	 *
	 *	This class uses boost::program_options to read options from the
	 *	commandline and additionally load options from a file.
	 *
	 *	It is meant to be used as a singleton. Call 
	 *	@code storm::settings::newInstance(argc, argv, filename) @endcode
	 *	to initialize it and obtain an instance for the first time.
	 *	Afterwards, use
	 *	@code storm::settings::instance() @endcode
	 *
	 *	This class can be customized by other parts of the software using
	 *	option modules. An option module can be anything that implements the
	 *	interface specified by registerModule().
	 */
	class Settings {
		public:

			static bool registerNewModule(ModuleRegistrationFunction_t registrationFunction);
			
			/*!
			 * This parses the command line of the application and matches it to all prior registered options
			 * @throws OptionParserException
			 */
			static void parse(int const argc, char const * const argv[]);

			std::vector<std::shared_ptr<Option>> const& getOptions() const {
				return this->optionPointers;
			}

			// PUBLIC INTERFACE OF OPTIONSACCUMULATOR (now internal)
			/*!
			* Returns true IFF an option with the specified longName exists.
			*/
			bool containsOptionByLongName(std::string const& longName) const {
				return this->containsLongName(longName);
			}

			/*!
			* Returns true IFF an option with the specified shortName exists.
			*/
			bool containsOptionByShortName(std::string const& shortName) const {
				return this->containsLongName(shortName);
			}

			/*!
			* Returns a reference to the Option with the specified longName.
			* Throws an Exception of Type IllegalArgumentException if there is no such Option.
			*/
			Option const& getOptionByLongName(std::string const& longName) const {
				return this->getByLongName(longName);
			}

			/*!
			* Returns a reference to the Option with the specified shortName.
			* Throws an Exception of Type IllegalArgumentException if there is no such Option.
			*/
			Option const& getOptionByShortName(std::string const& shortName) const {
				return this->getByShortName(shortName);
			}
			
			/*!
			 * Adds the given option to the set of known options.
			 * Unifying with existing options is done automatically.
			 * Ownership of the Option is handed over when calling this function!
			 * Returns a reference to the settings instance
			 * @throws OptionUnificationException
			 */
			Settings& addOption(Option* option);

			/*!
			 * Returns true iff there is an Option with the specified longName and it has been set
			 * @return bool true if the option exists and has been set
			 * @throws InvalidArgumentException
			 */
			bool isSet(std::string const& longName) const {
				return this->getByLongName(longName).getHasOptionBeenSet();
			}

			/*!
			 * Sets the Option with the specified longName
			 * This function requires the Option to have no arguments
			 * This is for TESTING only and should not be used outside of the testing code!
			 * @throws InvalidArgumentException
			 */
			void set(std::string const& longName) const {
				return this->getByLongName(longName).setHasOptionBeenSet();
			}

			/*
			 * This generated a list of all registered options and their arguments together with descriptions and defaults.
			 * @return A std::string containing the help text, delimited by \n
			 */
			std::string getHelpText() const;

			static Settings* getInstance();
			friend class Destroyer;
		private:
			/*!
			 *	@brief	Private constructor.
			 *
			 *	This constructor is private, as noone should be able to create
			 *	an instance manually, one should always use the
			 *	newInstance() method.
			 */
			Settings() {
				//
			}
			
			/*!
			 *	@brief	Private destructor.
			 *
			 *	This destructor should be private, as noone should be able to destroy a singleton.
			 *	The object is automatically destroyed when the program terminates by the destroyer.
			 */
			virtual ~Settings() {
				this->instance = nullptr;
			}

			void parseCommandLine(int const argc, char const * const argv[]);

			/*!
			* The map holding the information regarding registered options and their types
			*/
			std::unordered_map<std::string, std::shared_ptr<Option>> options;

			/*!
			* The vector holding a pointer to all options
			*/
			std::vector<std::shared_ptr<Option>> optionPointers;

			/*!
			* The map holding the information regarding registered options and their short names
			*/
			std::unordered_map<std::string, std::string> shortNames;

			/*!
			 *	@brief	actual instance of this class.
			 */
			static Settings* instance;
 			
 			/*!
 			 *	@brief	Destroyer object.
 			 */
 			static Destroyer destroyer;

			// Helper functions
			stringPair_t splitOptionString(std::string const& option);
			bool hasAssignment(std::string const& option);
			void handleAssignment(std::string const& longOptionName, std::vector<std::string> arguments);
			std::vector<std::string> argvToStringArray(int const argc, char const * const argv[]);
			std::vector<bool> scanForOptions(std::vector<std::string> const& arguments);
			bool checkArgumentSyntaxForOption(std::string const& argvString);

			/*!
			* Returns true IFF this accumulator contains an option with the specified longName.
			*/
			bool containsLongName(std::string const& longName) const {
				return (this->options.find(storm::utility::StringHelper::stringToLower(longName)) != this->options.end());
			}

			/*!
			* Returns true IFF this accumulator contains an option with the specified shortName.
			*/
			bool containsShortName(std::string const& shortName) const {
				return (this->shortNames.find(storm::utility::StringHelper::stringToLower(shortName)) != this->shortNames.end());
			}

			/*!
			* Returns a reference to the Option with the specified longName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			*/
			Option& getByLongName(std::string const& longName) const {
				auto longNameIterator = this->options.find(storm::utility::StringHelper::stringToLower(longName));
				if (longNameIterator == this->options.end()) {
					throw storm::exceptions::IllegalArgumentException() << "This Accumulator does not contain an Option named \"" << longName << "\"!";
				}
				return *longNameIterator->second.get();
			}

			/*!
			* Returns a pointer to the Option with the specified longName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			* @throws InvalidArgumentException
			*/
			Option* getPtrByLongName(std::string const& longName) const {
				auto longNameIterator = this->options.find(storm::utility::StringHelper::stringToLower(longName));
				if (longNameIterator == this->options.end()) {
					throw storm::exceptions::IllegalArgumentException() << "This Accumulator does not contain an Option named \"" << longName << "\"!";
				}
				return longNameIterator->second.get();
			}

			/*!
			* Returns a reference to the Option with the specified shortName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			*/
			Option& getByShortName(std::string const& shortName) const {
				auto shortNameIterator = this->shortNames.find(storm::utility::StringHelper::stringToLower(shortName));
				if (shortNameIterator == this->shortNames.end()) {
					throw storm::exceptions::IllegalArgumentException() << "This Accumulator does not contain an Option with ShortName \"" << shortName << "\"!";
				}
				return *(this->options.find(shortNameIterator->second)->second.get());
			}

			/*!
			* Returns a pointer to the Option with the specified shortName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			*/
			Option* getPtrByShortName(std::string const& shortName) const {
				auto shortNameIterator = this->shortNames.find(storm::utility::StringHelper::stringToLower(shortName));
				if (shortNameIterator == this->shortNames.end()) {
					throw storm::exceptions::IllegalArgumentException() << "This Accumulator does not contain an Option with ShortName \"" << shortName << "\"!";
				}
				return this->options.find(shortNameIterator->second)->second.get();
			}
	};

	/*!
	 *	@brief	Destroyer class for singleton object of Settings.
	 *
	 *	The sole purpose of this class is to clean up the singleton object
	 *	instance of Settings. The Settings class has a static member of this
	 *	Destroyer type that gets cleaned up when the program terminates. In
	 *	it's destructor, this object will remove the Settings instance.
	 */
	class Destroyer {
		public:
			Destroyer(): settingsInstance(nullptr) {
				this->settingsInstance = storm::settings::Settings::getInstance();
			}

			/*!
			 *	@brief	Destructor.
			 *
			 *	Free Settings::inst.
			 */
			virtual ~Destroyer() {
				if (this->settingsInstance != nullptr) {
					std::cout << "Destroying Settings Instance..." << std::endl;
					this->settingsInstance->instance = nullptr;
					// The C++11 Method of Singleton deletes its instance on its own
					//delete this->settingsInstance;
					this->settingsInstance = nullptr;
				}
			}
		private:
			storm::settings::Settings* settingsInstance;
	};
	




} // namespace settings
} // namespace storm

#endif // 