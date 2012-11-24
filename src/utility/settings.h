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
namespace settings {

	namespace bpo = boost::program_options;
	/*!
	 *	@brief	Simple wrapper around boost::program_options to handle configuration options.
	 *
	 *	This class uses boost::program_options to read options from the
	 *	commandline and additionally load options from a file.
	 */
	class Settings
	{
		public:
		
		/*!
		 *	@brief Constructor of Settings file
		 *
		 *	The constructor fills the option descriptions, parses the
		 *	command line and the config file and puts the option values to
		 *	our option mapping.
		 *
		 *	If a configfile is set in the commandline, we load this one.
		 *	Otherwise, if filename is not NULL, we load this one. Otherwise,
		 *	we load no config file.
		 *
		 *	@param argc should be argc passed to main function
		 *	@param argv should be argv passed to main function
		 *	@param filename	either NULL or name of config file
		 */
		Settings(const int argc, const char* argv[], const char* filename)
			: configfile("Config Options"), generic("Generic Options"), commandline("Commandline Options")
		{
			/*
			 *	fill option descriptions
			 */
			this->commandline.add_options()
				("help", "produce help message")
				("help-config", "produce help message about config file")
				("configfile", bpo::value<std::string>(), "name of config file")
			;
			this->generic.add_options()
				("trafile", bpo::value<std::string>()->required(), "name of the .tra file")
				("labfile", bpo::value<std::string>()->required(), "name of the .lab file")
			;
			this->configfile.add_options()
			;
			
			this->positional.add("trafile", 1);
			this->positional.add("labfile", 1);
			
			/*
			 *	construct option descriptions for commandline and config file
			 */
			this->cli.add(this->commandline).add(generic);
			this->conf.add(this->configfile).add(generic);
			
			try
			{
				/*
				 *	load command line
				 */
				bpo::store(bpo::command_line_parser(argc, argv).options(this->cli).positional(this->positional).run(), this->vm);
				/*
				 *	load config file if specified
				 */
				if (this->vm.count("configfile"))
				{
					bpo::store(bpo::parse_config_file<char>(this->vm["configfile"].as<std::string>().c_str(), this->conf), this->vm, true);
				}
				else if (filename != NULL)
				{
					bpo::store(bpo::parse_config_file<char>(filename, this->conf), this->vm, true);
				}
				bpo::notify(this->vm);
			}
			/*
			 *	catch errors...
			 */
			catch (bpo::reading_file e)
			{
				std::cout << "Could not read config file " << filename << std::endl;
			}
			catch (bpo::required_option e)
			{
				if (! (this->vm.count("help") || this->vm.count("help-config")))
				{
					std::cout << e.what() << std::endl;
					throw mrmc::exceptions::InvalidSettings();
				}
			}
			catch (bpo::error e)
			{
				std::cout << "Some error occurred: " << e.what() << std::endl;
			}
		}
		
		/*!
		 *	@brief Get option descriptions for commandline options
		 */
		bpo::options_description& getHelpForCommandline()
		{
			return this->cli;
		}

		/*!
		 *	@brief Get option descriptions for config file options
		 */
		bpo::options_description& getHelpForConfigfile()
		{
			return this->conf;
		}
		
		/*!
		 *	@brief Get value of string argument
		 */
		const std::string& getString(const std::string &name) const
		{
			return this->vm[name].as<std::string>();
		}
		
		const bool isSet(const std::string &name) const
		{
			return this->vm.count(name) > 0;
		}

		private:
			/*!
			 *	@brief option descriptions
			 */
			bpo::options_description configfile;
			bpo::options_description generic;
			bpo::options_description commandline;
			bpo::positional_options_description positional;
			
			/*!
			 *	@brief collecing option descriptions
			 *
			 *	The options for command line and config file are collected
			 *	here
			 */
			bpo::options_description cli;
			bpo::options_description conf;
			
			/*!
			 *	@brief	option mapping
			 */
			bpo::variables_map vm;
		
	};

} // namespace parser
} // namespace mrmc

#endif // SETTINGS_H_
