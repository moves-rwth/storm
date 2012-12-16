/*
 * Settings.h
 *
 *  Created on: 22.11.2012
 *      Author: Gereon Kremer
 */

#ifndef MRMC_SETTINGS_SETTINGS_H_
#define MRMC_SETTINGS_SETTINGS_H_

#include <iostream>
#include <sstream>
#include <list>
#include <utility>
#include <memory>
#include <boost/program_options.hpp>
#include "src/exceptions/InvalidSettings.h"

namespace mrmc {

/*!
 *	@brief Contains Settings class and associated methods.
 *
 *	The settings namespace contains the Settings class some friend methods like instance().
 */
namespace settings {
	
	namespace bpo = boost::program_options;
	
	/*!
	 *	@brief	Wrapper around boost::program_options to handle configuration options.
	 *
	 *	This class uses boost::program_options to read options from the
	 *	commandline and additionally load options from a file.
	 *
	 *	It is meant to be used as a singleton. Call 
	 *	@code mrmc::settings::newInstance(argc, argv, filename) @endcode
	 *	to initialize it and obtain an instance for the first time.
	 *	Afterwards, use
	 *	@code mrmc::settings::instance() @endcode
	 *
	 *	This class can be customized by other parts of the software using
	 *	option modules. An option module can be anything that implements the
	 *	interface specified by registerModule().
	 */
	class Settings
	{
		public:
		
			/*!
			 *	@brief	Get value of a generic option.
			 */
			template <typename T>
			const T& get(const std::string &name) const {
				if (this->vm.count(name) == 0) throw mrmc::exceptions::InvalidSettings() << "Could not read option " << name << ".";
				return this->vm[name].as<T>();
			}
		
			/*!
			 *	@brief	Get value of string option
			 */
			const std::string& getString(const std::string &name) const {
				return this->get<std::string>(name);
			}
		
			/*!
			 *	@brief	Check if an option is set
			 */
			const bool isSet(const std::string &name) const {
				return this->vm.count(name) > 0;
			}
			
			/*!
			 *	@brief	Register a new module.
			 *
			 *	A new settings module can be registered via
			 *	@code
			 *	mrmc::settings::Settings::registerModule<mrmc::ModuleClass>();
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
			static void registerModule()
			{
				// get trigger
				std::pair< std::string, std::string > trigger = T::getOptionTrigger();
				// build description name
				std::stringstream str;
				str << T::getModuleName() << " (" << trigger.first << " = " << trigger.second << ")";
				std::shared_ptr<bpo::options_description> desc = std::shared_ptr<bpo::options_description>(new bpo::options_description(str.str()));
				// but options
				T::putOptions(desc.get());
				// store
				Settings::modules[ trigger ] = desc;
			}
	
			friend std::ostream& help(std::ostream& os);
			friend std::ostream& helpConfigfile(std::ostream& os);
			friend Settings* instance();
			friend Settings* newInstance(const int argc, const char* argv[], const char* filename);

		private:
			/*!
			 *	@brief	Constructor.
			 */
			Settings(const int argc, const char* argv[], const char* filename);
			
			/*!
			 *	@brief	Initialize options_description object.
			 */
			void initDescriptions();
			
			/*!
			 *	@brief	Perform first parser run
			 */
			void firstRun(const int argc, const char* argv[], const char* filename);
			
			/*!
			 *	@brief	Perform second parser run.
			 */
			void secondRun(const int argc, const char* argv[], const char* filename);
			
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
	inline Settings* instance()
	{
		return Settings::inst;
	}
	
	/*!
	 *	@brief	Create new instance.
	 *
	 *	Creates a new Settings instance and passes the arguments to the constructor of Settings.
	 *
	 *	@param argc should be argc passed to main function
	 *	@param argv should be argv passed to main function
	 *  @param filename either NULL or name of config file
	 *	@return The new instance of Settings.
	 */
	inline Settings* newInstance(const int argc, const char* argv[], const char* filename)
	{
		if (Settings::inst != nullptr) delete Settings::inst;
		Settings::inst = new Settings(argc, argv, filename);
		return Settings::inst;
	}
		
} // namespace settings
} // namespace mrmc

#endif // MRMC_SETTINGS_SETTINGS_H_
