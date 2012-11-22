/*
 * settings.cpp
 *
 *  Created on: 22.11.2012
 *      Author: Gereon Kremer
 */

#include "src/utility/settings.h"

//#include <iostream>
//#include <boost/program_options.hpp>
//#include "src/exceptions/InvalidSettings.h"

namespace mrmc {
namespace settings {

namespace bpo = boost::program_options;

bpo::options_description mrmc::settings::Settings::configfile("Config Options");
bpo::options_description mrmc::settings::Settings::generic("Generic Options");
bpo::options_description mrmc::settings::Settings::commandline("Commandline Options");
bpo::positional_options_description mrmc::settings::Settings::positional;

bpo::options_description mrmc::settings::Settings::cli;
bpo::options_description mrmc::settings::Settings::conf;

mrmc::settings::Settings* mrmc::settings::Settings::inst = NULL;

/*!
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
Settings::Settings(const int argc, const char* argv[], const char* filename)
{
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
 *	Creates a new instance if necessary. When the instance is actually
 *	created for the first time, the internal options_description objects are
 *	initialized.
 *	
 *	If this function was already called and another instance is   
 *	already present, the existing instance will be returned. In this
 *	case, better use the routine mrmc::settings::instance().
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
 *	@return instance of Settings
 */
Settings* Settings::instance(const int argc, const char* argv[], const char* filename)
{
	if (Settings::inst == NULL) {
		/*
		 *	fill option descriptions
		 */
		Settings::commandline.add_options()
			("help", "produce help message")
			("help-config", "produce help message about config file")
			("configfile", bpo::value<std::string>(), "name of config file")
		;
		Settings::generic.add_options()
			("trafile", bpo::value<std::string>()->required(), "name of the .tra file")
			("labfile", bpo::value<std::string>()->required(), "name of the .lab file")
		;
		Settings::configfile.add_options()
		;

		/*
		 *	construct option descriptions for commandline and config file
		 */
		Settings::cli.add(Settings::commandline).add(generic);
		Settings::conf.add(Settings::configfile).add(generic);
	
		/*
		 *	Take care of positional arguments
		 */
		Settings::positional.add("trafile", 1);
		Settings::positional.add("labfile", 1);
		
		/*
		 *	Actually create new instance
		 */
		Settings::inst = new Settings(argc, argv, filename);	        
	}
	return Settings::inst;
}

/*!
 *	Print a short general usage information consisting of the positional
 *	options and the list of available command line options.
 *
 *	Use it like this:
 *	@code std::cout << mrmc::settings::help; @endcode
 */
std::ostream& help(std::ostream& os)
{
	os << "Usage: <binary> [options] <transition file> <label file>" << std::endl;
	os << Settings::cli << std::endl;;
	return os;
}

/*!
 *	Print a list of available options for the config file.
 *
 *	Use it like this:
 *	@code std::cout << mrmc::settings::helpConfigfile; @endcode
 */
std::ostream& helpConfigfile(std::ostream& os)
{
	os << Settings::conf << std::endl;;
	return os;
}

/*!
 *	@return current instance.
 */
Settings* instance()
{
	return Settings::inst;
}
    
} // namespace settings
} // namespace mrmc