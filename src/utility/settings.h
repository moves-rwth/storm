/*
 * settings.h
 *
 *  Created on: 22.11.2012
 *      Author: Gereon Kremer
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <iostream>
#include <boost/program_options.hpp>
#include "src/exceptions/InvalidSettings.h"

namespace mrmc {

/*!
 *	The settings namespace contains the Settings class and all associated
 *	methods.
 */
namespace settings {
	
	namespace bpo = boost::program_options;
	/*!
	 *	@brief	Simple wrapper around boost::program_options to handle configuration options.
	 *
	 *	This class uses boost::program_options to read options from the
	 *	commandline and additionally load options from a file.
	 *
	 *	It is meant to be used as a singleton. Call 
	 *	@code mrmc::settings::Settings::instance(argc, argv, filename) @endcode
	 *	to initialize it and obtain an instance for the first time.
	 *	Afterwards, it is possible to use
	 *	@code mrmc::settings::instance() @endcode
	 */
	class Settings
	{
		public:
		
		/*!
		 *	@brief Get value of string option
		 */
		const std::string& getString(const std::string &name) const {
			return this->vm[name].as<std::string>();
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

		/*!
		 *	@brief Creates a new instance.
		 */
		static Settings* instance(const int argc, const char* argv[], const char* filename);

		private:
			/*!
			 *	@brief	Constructor.
			 */
			Settings(const int argc, const char* argv[], const char* filename);
		
			/*!
			 *	@brief Option descriptions.
			 */
			static bpo::options_description configfile;
			static bpo::options_description generic;
			static bpo::options_description commandline;
			static bpo::positional_options_description positional;
			
			/*!
			 *	@brief Collecting option descriptions.
			 *
			 *	The options for command line and config file are collected
			 *	here.
			 */
			static bpo::options_description cli;
			static bpo::options_description conf;
			
			/*!
			 *	@brief	option mapping.
			 */
			bpo::variables_map vm;
			
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
	 */
	Settings* instance();

} // namespace parser
} // namespace mrmc

#endif // SETTINGS_H_
