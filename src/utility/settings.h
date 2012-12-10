/*
 * settings.h
 *
 *  Created on: 22.11.2012
 *      Author: Gereon Kremer
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <iostream>
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
	/*
	 *	Sorry for very long comment at this point (for the class), but all
	 *	methods are private, hence there is no other place to explain the
	 *	inner workings of this class that are necessary to understand the
	 *	callback concept...
	 */
	/*!
	 *	@brief	Simple wrapper around boost::program_options to handle configuration options.
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
	 *	callbacks. There are three types of callbacks: register,
	 *	intermediate and checker.
	 *
	 *	The (private) constructor will start with filling the internal
	 *	options_description object. There are a few generic options like
	 *	--help or --verbose. Then if calls all register callbacks that may
	 *	add more options.
	 *
	 *	Then, it will start with a sloppy parsing run, allowing unregistered 
	 *	options and ignoring further constraints from the options_description 
	 *	objects.
	 *
	 *	After that, it will call all intermediate callbacks. They can
	 *	inspect the options from the first run and add more options, e.g.
	 *	enable more options for a specific component that has been enabled.
	 *
	 *	Using the new options_description objects, the constructor performs
	 *	a second run. This time, it will not allow unregistered options and
	 *	will check for required and positional arguments.
	 *
	 *	Finally, all checker callbacks will be called. They can check the
	 *	final options for more complex requirements. If any of those checker
	 *	callbacks returns false, a InvalidSettings exception will be thrown.
	 */
	class Settings
	{
		public:
		
			/*!
			 *	@brief Get value of a generic option.
			 */
			template <typename T>
			const T& get(const std::string &name) const {
				if (this->vm.count(name) == 0) throw mrmc::exceptions::InvalidSettings();
				return this->vm[name].as<T>();
			}
		
			/*!
			 *	@brief Get value of string option
			 */
			const std::string& getString(const std::string &name) const {
				return this->get<std::string>(name);
			}
		
			/*!
			 *	@brief Check if an option is set
			 */
			const bool isSet(const std::string &name) const {
				return this->vm.count(name) > 0;
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
			 *	@brief	Option descriptions for config file.
			 */
			bpo::options_description configfile;
			/*!
			 *	@brief	Option descriptions for config file and command line.
			 */
			bpo::options_description generic;
			/*!
			 *	@brief	Option descriptions for command line.
			 */
			bpo::options_description commandline;
			/*!
			 *	@brief	Option description for positional arguments on command line.
			 */
			bpo::positional_options_description positional;
			
			/*!
			 *	@brief	Collecting option descriptions for command line.
			 */
			static std::unique_ptr<bpo::options_description> cli;
			/*!
			 *	@brief	Collecting option descriptions for config file.
			 */
			static std::unique_ptr<bpo::options_description> conf;
			
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
	 *	@brief	Print help for config file options.
	 */
	std::ostream& helpConfigfile(std::ostream& os);
	
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
	
	/*!
	 *	@brief	Function type for functions registering new options.
	 */
	typedef void(*RegisterCallback)(bpo::options_description&);
	
	/*!
	 *	@brief	Function type for functions changing the parser state
	 *	between the first and second run.
	 */
	typedef void(*IntermediateCallback)(bpo::options_description*, bpo::variables_map&);
	
	/*!
	 *	@brief	Function type for function checking constraints on settings.
	 */
	typedef bool(*CheckerCallback)(bpo::variables_map&);
	
	/*!
	 *	@brief	This enums specifies the three types of options.
	 */
	enum CallbackType {
		//! Option can be set in config file
		CB_CONFIG,
		//! Option can be set on command line
		CB_CLI, 
		//! Option can be set in config file and command line
		CB_GENERIC
	};
	
	/*!
	 *	@brief This class handles callbacks for registering new options and
	 *	checking constraints on them afterwards.
	 *
	 *	As it should never be used directly, but only through the Register
	 *	class, it does not provide any public methods.
	 *
	 *	This class is also a singleton (like Settings) and is implemented much
	 *	simpler as we don't need any custom initialization code.
	 */
	class Callbacks
	{
		public:
			inline void put(const CallbackType type, const RegisterCallback ptr)
			{
				if (this->disabled) throw mrmc::exceptions::InvalidSettings();
				this->registerList.push_back(std::pair<CallbackType, RegisterCallback>(type, ptr));
			}
			inline void put(const CallbackType type, const IntermediateCallback ptr)
			{
				if (this->disabled) throw mrmc::exceptions::InvalidSettings();
				this->intermediateList.push_back(std::pair<CallbackType, IntermediateCallback>(type, ptr));
			}
			inline void put(const CheckerCallback ptr)
			{
				if (this->disabled) throw mrmc::exceptions::InvalidSettings();
				this->checkerList.push_back(ptr);
			}
			
		private:
			/*!
			 *	@brief	Stores register callbacks.
			 */
			std::list<std::pair<CallbackType, RegisterCallback>> registerList;
			
			/*!
			 *	@brief	Stores intermediate callbacks.
			 */
			std::list<std::pair<CallbackType, IntermediateCallback>> intermediateList;
			
			/*!
			 *	@brief	Stores check callbacks.
			 */
			std::list<CheckerCallback> checkerList;
			
			/*!
			 *	@brief	Stores if we already loaded the settings.
			 */
			bool disabled;
			
			/*!
			 *	@brief Private constructor.
			 */
			Callbacks() : disabled(false) {}
			/*!
			 *	@brief Private copy constructor.
			 */
			Callbacks(const Callbacks&) {}
			/*!
			 *	@brief Private destructor.
			 */
			~Callbacks() {}			

			/*!
			 *	@brief Returns current instance to create singleton.
			 *	@return current instance
			 */
			inline static Callbacks* instance()
			{
				static Callbacks inst;
				return &inst;
			}
		
		/*!
		 *	@brief Register class needs access to lists.
		 */
		friend class Register;
		
		/*!
		 *	@brief Settings class need access to lists.
		 */
		friend class Settings;
	};
	
	/*!
	 *	@brief Wrapper class to allow for registering callbacks during
	 *	static initialization.
	 *
	 *	To use this class, use the following includes:
	 *	@code
	 *	#include "src/utility/settings.h"
	 *	#include <boost/program_options.hpp>
	 *	namespace bpo = boost::program_options;	
	 *	@endcode
	 */
	class Register
	{
		public:
			/*!
			 *	@brief Registers given function as register callback.
			 *
			 *	This constructor registers a callback routine that might add
			 *	new options for the Settings class. It should be used like
			 *	this:
			 *	@code
			 *	void register(bpo::options_description& desc) {
			 *		// do something with desc here
			 *	}
			 *	mrmc::settings::Register reg(mrmc::settings::CB_CLI, &register);
			 *	@endcode
			 *	This code should be executed during static initialization, i.e.
			 *	it should be somewhere in the cpp-file.
			 */
			Register(const CallbackType type, const RegisterCallback ptr)
			{
				mrmc::settings::Callbacks::instance()->put(type, ptr);
			}
			
			/*!
			 *	@brief Registers given function as intermediate callback.
			 * 
			 *	This constructor registers a callback routine that can check
			 *	the option assignment after the first run and change the
			 *	options description before the second run.
			 *	It should be used like this:
			 *	@code
			 *	void intermediate(bpo::options_description& desc, bpo::variables_map& map) {
			 *		// check contents of map and maybe change desc
			 *	}
			 *	mrmc::settings::Register reg(mrmc::settings::CB_CLI, &intermediate);
			 *	@endcode
			 *	This code should be executed during static initialization, i.e.
			 *	it should be somewhere in the cpp-file.
			 */
			Register(const CallbackType type, const IntermediateCallback ptr)
			{
				mrmc::settings::Callbacks::instance()->put(type, ptr);
			}

			/*!
			 *	@brief Registers given function as check callback.
			 * 
			 *	This constructor registers a callback routine that can check
			 *	the option assignment after the Settings class has loaded
			 *	them. It should be used like this:
			 *	@code
			 *	void check(bpo::variables_map& map) {
			 *		// check contents of map
			 *	}
			 *	mrmc::settings::Register reg(&check);
			 *	@endcode
			 *	This code should be executed during static initialization, i.e.
			 *	it should be somewhere in the cpp-file.
			 */
			Register(const CheckerCallback ptr)
			{
				mrmc::settings::Callbacks::instance()->put(ptr);
			}
	};
	
} // namespace settings
} // namespace mrmc

#endif // SETTINGS_H_
