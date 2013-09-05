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
#include "src/settings/OptionsAccumulator.h"
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

	typedef std::function<bool (OptionsAccumulator*)> ModuleRegistrationFunction_t;

	typedef bool (*stringValidationFunction_t)(const std::string);
	typedef bool (*integerValidationFunction_t)(const int_fast64_t);
	typedef bool (*unsignedIntegerValidationFunction_t)(const uint_fast64_t);
	typedef bool (*doubleValidationFunction_t)(const double);
	typedef bool (*booleanValidationFunction_t)(const bool);

	typedef std::pair<std::string, std::string> stringPair_t;
	typedef std::pair<bool, std::string> fromStringAssignmentResult_t;

	/*
	typedef std::function<bool (std::string const)> stringValidationFunction_t;
	typedef std::function<bool (int_fast64_t const)> integerValidationFunction_t;
	typedef std::function<bool (uint_fast64_t const)> unsignedIntegerValidationFunction_t;
	typedef std::function<bool (double const)> doubleValidationFunction_t;
	typedef std::function<bool (bool const)> booleanValidationFunction_t;
	*/

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
			 * Parsing
			 */
			static void parse(int const argc, char const * const argv[]);

			void addOptions(OptionsAccumulator const& options);

			std::vector<std::shared_ptr<Option>> const& getOptions() const {
				return this->optionsAccumulator->optionPointers;
			}

			// COPY INTERFACE OF OPTIONSACCUMULATOR
			/*!
			* Returns true IFF an option with the specified longName exists.
			*/
			bool containsOptionByLongName(std::string const& longName) {
				return this->optionsAccumulator->containsLongName(longName);
			}

			/*!
			* Returns true IFF an option with the specified shortName exists.
			*/
			bool containsOptionByShortName(std::string const& shortName) {
				return this->optionsAccumulator->containsLongName(shortName);
			}

			/*!
			* Returns a reference to the Option with the specified longName.
			* Throws an Exception of Type IllegalArgumentException if there is no such Option.
			*/
			Option const& getOptionByLongName(std::string const& longName) {
				return this->optionsAccumulator->getByLongName(longName);
			}

			/*!
			* Returns a reference to the Option with the specified shortName.
			* Throws an Exception of Type IllegalArgumentException if there is no such Option.
			*/
			Option const& getOptionByShortName(std::string const& shortName) {
				return this->optionsAccumulator->getByShortName(shortName);
			}

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
			Settings(): optionsAccumulator(nullptr) {
				this->optionsAccumulator = new OptionsAccumulator();
			}
			
			/*!
			 *	@brief	Private destructor.
			 *
			 *	This destructor should be private, as noone should be able to destroy a singleton.
			 *	The object is automatically destroyed when the program terminates by the destroyer.
			 */
			virtual ~Settings() {
				delete this->optionsAccumulator;
				this->instance = nullptr;
			}

			void parseCommandLine(int const argc, char const * const argv[]);

			/*!
			 *	@brief	The registered options
			 */
			OptionsAccumulator* optionsAccumulator;

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