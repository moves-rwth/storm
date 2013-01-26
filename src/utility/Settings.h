/*
 * Settings.h
 *
 *  Created on: 22.11.2012
 *      Author: Gereon Kremer
 */

#ifndef STORM_SETTINGS_SETTINGS_H_
#define STORM_SETTINGS_SETTINGS_H_

#include <iostream>
#include <sstream>
#include <list>
#include <utility>
#include <memory>
#include <boost/program_options.hpp>
#include "src/exceptions/InvalidSettingsException.h"

namespace storm {

/*!
 *	@brief Contains Settings class and associated methods.
 *
 *	The settings namespace contains the Settings class some friend methods like instance().
 */
namespace settings {
	
	namespace bpo = boost::program_options;
	
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
		
			/*!
			 *	@brief	Get value of a generic option.
			 */
			template <typename T>
			inline const T& get( std::string const & name) const {
				if (this->vm.count(name) == 0) throw storm::exceptions::InvalidSettingsException() << "Could not read option " << name << ".";
				return this->vm[name].as<T>();
			}
		
			/*!
			 *	@brief	Get value of string option.
			 */
			inline const std::string& getString(std::string const & name) const {
				return this->get<std::string>(name);
			}
		
			/*!
			 *	@brief	Check if an option is set.
			 */
			inline const bool isSet(std::string const & name) const {
				return this->vm.count(name) > 0;
			}

			/*!
			 *	@brief	Register a new module.
			 *
			 *	A new settings module can be registered via
			 *	@code
			 *	storm::settings::Settings::registerModule<storm::ModuleClass>();
			 *	@endcode
			 *	This has to be done before any parsing takes place, i.e. before newInstance() is called.
			 *
			 *	This function implicitly defines the following interface for any SettingsModule:
			 *	@code
			 *	static std::string getModuleName();
			 *	static std::pair< std::string, std::string > getOptionTrigger();
			 *	static void putOptions(boost::program_options::options_description*);
			 *	@endcode
			 *
			 *	The semantic is the following:
			 *	If the trigger <a,b> is true, i.e. if option a is set to b,
			 *	the options_description object will be added to the internal
			 *	option object.
			 *
			 *	Furthermore, it will generate the option specified by the
			 *	trigger.
			 *
			 *	The functions could look like this:
			 *	@code
			 *	static std::string getModuleName() { return "Backend A"; }
			 *	static std::pair<std::string, std::string> getOptionTrigger() {
			 *		return std::pair<std::string, std::string>("backend", "a");
			 *	}
			 *	static void putOptions(boost::program_options::options_description* desc) {
			 *		desc->add_options()("foo", "bar");
			 *	}
			 *	@endcode
			 */
			template <typename T>
			static void registerModule() {
				// Get trigger values.
				std::pair< std::string, std::string > trigger = T::getOptionTrigger();
				// Build description name.
				std::stringstream str;
				str << "Options for " << T::getModuleName() << " (" << trigger.first << " = " << trigger.second << ")";
				std::shared_ptr<bpo::options_description> desc = std::shared_ptr<bpo::options_description>(new bpo::options_description(str.str()));
				// Put options into description.
				T::putOptions(desc.get());
				// Store module.
				Settings::modules[ trigger ] = desc;
			}
			
			friend std::ostream& help(std::ostream& os);
			friend std::ostream& helpConfigfile(std::ostream& os);
			friend Settings* instance();
			friend Settings* newInstance(int const argc, char const * const argv[], char const * const filename, bool const sloppy = false);
			friend Destroyer;

		private:
			/*!
			 *	@brief	Private constructor.
			 *
			 *	This constructor is private, as noone should be able to create
			 *	an instance manually, one should always use the
			 *	newInstance() method.
			 */
			Settings(int const argc, char const * const argv[], char const * const filename, bool const sloppy);
			
			/*!
			 *	@brief	Private destructor.
			 *
			 *	This destructor should be private, as noone should be able to destroy a singleton.
			 *	The object is automatically destroyed when the program terminates by the destroyer.
			 */
			~Settings() {}
			
			/*!
			 *	@brief	Initialize options_description object.
			 */
			void initDescriptions();
			
			/*!
			 *	@brief	Perform first parser run
			 */
			void firstRun(int const argc, char const * const argv[], char const * const filename);
			
			/*!
			 *	@brief	Perform second parser run.
			 */
			void secondRun(int const argc, char const * const argv[], char const * const filename);
			
			/*!
			 *	@brief	Option description for positional arguments on command line.
			 */
			bpo::positional_options_description positional;
			
			/*!
			 *	@brief	Collecting option descriptions.
			 */
			static std::unique_ptr<bpo::options_description> desc;

			/*!
			 *	@brief	Contains option descriptions for all modules.
			 */
			static std::map< std::pair< std::string, std::string >, std::shared_ptr<bpo::options_description> > modules;
			
			/*!
			 *	@brief	option mapping.
			 */
			bpo::variables_map vm;
			
			/*!
			 *	@brief name of binary
			 */
			static std::string binaryName;
			
			/*!
			 *	@brief	actual instance of this class.
			 */
 			static Settings* inst;
 			
 			/*!
 			 *	@brief	Destroyer object.
 			 */
 			static Destroyer destroyer;
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
			/*!
			 *	@brief	Destructor.
			 *
			 *	Free Settings::inst.
			 */
			~Destroyer() {
				if (Settings::inst != nullptr) {
					delete Settings::inst;
				}
			}
	};
	
	/*!
	 *	@brief	Print usage help.
	 */
	std::ostream& help(std::ostream& os);
	
	/*!
	 *	@brief	Return current instance.
	 *
	 *	@return The current instance of Settings created by newInstance().
	 */
	inline Settings* instance() {
		return Settings::inst;
	}
	
	/*!
	 *	@brief	Create new instance.
	 *
	 *	Creates a new Settings instance and passes the arguments to the constructor of Settings.
	 *
	 *	@param argc should be argc passed to main function
	 *	@param argv should be argv passed to main function
	 *	@param filename either NULL or name of config file
	 *	@return The new instance of Settings.
	 */
	inline Settings* newInstance(int const argc, char const * const argv[], char const * const filename, bool const sloppy) {
		if (Settings::inst != nullptr) delete Settings::inst;
		Settings::inst = new Settings(argc, argv, filename, sloppy);
		return Settings::inst;
	}
		
} // namespace settings
} // namespace storm

#endif // STORM_SETTINGS_SETTINGS_H_
