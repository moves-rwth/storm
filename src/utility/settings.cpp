/*
 * settings.cpp
 *
 *  Created on: 22.11.2012
 *      Author: Gereon Kremer
 */

#include "src/utility/settings.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace mrmc {
namespace settings {

namespace bpo = boost::program_options;

/*
 * static initializers
 */
bpo::options_description* mrmc::settings::Settings::cli = nullptr;
bpo::options_description* mrmc::settings::Settings::conf = nullptr;
std::string mrmc::settings::Settings::binaryName = "";
mrmc::settings::Settings* mrmc::settings::Settings::inst = nullptr;

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
 *	@param filename	either nullptr or name of config file
 */
Settings::Settings(const int argc, const char* argv[], const char* filename)
	: configfile("Config Options"), generic("Generic Options"), commandline("Commandline Options")
{
	Settings::binaryName = std::string(argv[0]);
	try
	{
		//! Initially fill description objects and call register callbacks
		this->initDescriptions();

		//! Take care of positional arguments
		Settings::positional.add("trafile", 1);
		Settings::positional.add("labfile", 1);
		
		//! Create and fill collecting options descriptions
		Settings::cli = new bpo::options_description();
		Settings::cli->add(Settings::commandline).add(generic);
		Settings::conf = new bpo::options_description();
		Settings::conf->add(Settings::configfile).add(generic);
		
		//! Perform first parse run and call intermediate callbacks
		this->firstRun(argc, argv, filename);
		
		//! Rebuild collecting options descriptions
		delete Settings::cli;
		Settings::cli = new bpo::options_description();
		Settings::cli->add(Settings::commandline).add(generic);

		delete Settings::conf;
		Settings::conf = new bpo::options_description();
		Settings::conf->add(Settings::configfile).add(generic);
		
		//! Stop if help is set
		if ((this->vm.count("help") > 0) || (this->vm.count("help-config") > 0))
		{
			return;
		}
		
		//! Perform second run and call checker callbacks
		this->secondRun(argc, argv, filename);
		
		//! Finalize parsed options, check for specified requirements
		bpo::notify(this->vm);
		LOG4CPLUS_DEBUG(logger, "Finished loading config.");
	}
	catch (bpo::reading_file e)
	{
		std::cerr << "Could not read config file " << filename << std::endl;
		LOG4CPLUS_ERROR(logger, "Could not read config file");
	}
	catch (bpo::required_option e)
	{
		std::cerr << "Required option: " << e.what() << std::endl;
		LOG4CPLUS_ERROR(logger, "Required option: " << e.what());
		throw mrmc::exceptions::InvalidSettings();
	}
	catch (bpo::validation_error e)
	{
		std::cerr << "Validation failed: " << e.what() << std::endl;
		LOG4CPLUS_ERROR(logger, "Validation failed: " << e.what());
		throw mrmc::exceptions::InvalidSettings();
	}
	catch (bpo::invalid_command_line_syntax e)
	{
		std::cerr << "Invalid command line syntax: " << e.what() << std::endl;
		LOG4CPLUS_ERROR(logger, "Invalid command line syntax: " << e.what());
		throw mrmc::exceptions::InvalidSettings();
	}
	catch (bpo::error e)
	{
		std::cerr << e.what() << std::endl;
		LOG4CPLUS_ERROR(logger, "Unknown error: " << e.what());
		throw mrmc::exceptions::InvalidSettings();
	}
}

/*!
 *	Initially fill options_description objects.
 *	First puts some generic options, then calls all register Callbacks.
 */
void Settings::initDescriptions()
{
	LOG4CPLUS_DEBUG(logger, "Initializing descriptions.");
	this->commandline.add_options()
		("help,h", "produce help message")
		("verbose,v", "be verbose")
		("help-config", "produce help message about config file")
		("configfile,c", bpo::value<std::string>(), "name of config file")
		("test-prctl", bpo::value<std::string>(), "name of prctl file")
	;
	this->generic.add_options()
		("trafile", bpo::value<std::string>()->required(), "name of the .tra file")
		("labfile", bpo::value<std::string>()->required(), "name of the .lab file")
	;
	this->configfile.add_options()
	;

	/*
	 *	Get Callbacks object, then iterate over and call all register callbacks.
	 */
	Callbacks* cb = mrmc::settings::Callbacks::getInstance();
	while (cb->registerList.size() > 0)
	{
		CallbackType type = cb->registerList.front().first;
		RegisterCallback fptr = cb->registerList.front().second;
		cb->registerList.pop_front();
		
		switch (type)
		{
			case CB_CONFIG:
				(*fptr)(this->configfile);
				break;
			case CB_CLI:
				(*fptr)(this->commandline);
				break;
			case CB_GENERIC:
				(*fptr)(this->generic);
				break;
		}
	}
}

/*!
 *	Perform a sloppy parsing run: parse command line and config file (if
 *	given), but allow for unregistered options, do not check requirements
 *	from options_description objects, do not check positional arguments.
 *
 *	Call all intermediate callbacks afterwards.
 */
void Settings::firstRun(const int argc, const char* argv[], const char* filename)
{
	LOG4CPLUS_DEBUG(logger, "Performing first run.");
	//! parse command line
	bpo::store(bpo::command_line_parser(argc, argv).options(*(Settings::cli)).allow_unregistered().run(), this->vm);

	/*
	 *	load config file if specified
	 */
	if (this->vm.count("configfile"))
	{
		bpo::store(bpo::parse_config_file<char>(this->vm["configfile"].as<std::string>().c_str(), *(Settings::conf)), this->vm, true);
	}
	else if (filename != NULL)
	{
		bpo::store(bpo::parse_config_file<char>(filename, *(Settings::conf)), this->vm, true);
	}
	
	/*
	 *	Call intermediate callbacks.
	 */
	Callbacks* cb = mrmc::settings::Callbacks::getInstance();
	while (cb->intermediateList.size() > 0)
	{
		CallbackType type = cb->intermediateList.front().first;
		IntermediateCallback fptr = cb->intermediateList.front().second;
		cb->intermediateList.pop_front();
		
		try
		{
			switch (type)
			{
				case CB_CONFIG:
					(*fptr)(&this->configfile, this->vm);
					break;
				case CB_CLI:
					(*fptr)(&this->commandline, this->vm);
					break;
				case CB_GENERIC:
					(*fptr)(&this->generic, this->vm);
					break;
			}
		}
		catch (boost::bad_any_cast e)
		{
			std::cerr << "An intermediate callback failed." << std::endl;
			LOG4CPLUS_ERROR(logger, "An intermediate callback failed.\n" << e.what());
		}
	}
}

/*!
 *	Perform the second parser run: parse command line and config file (if
 *	given) and check for unregistered options, requirements from
 *	options_description objects and positional arguments.
 *
 *	Call all checker callbacks afterwards.
 */
void Settings::secondRun(const int argc, const char* argv[], const char* filename)
{
	LOG4CPLUS_DEBUG(logger, "Performing second run.");
	//! Parse command line
	bpo::store(bpo::command_line_parser(argc, argv).options(*(Settings::cli)).positional(this->positional).run(), this->vm);
	/*
	 *	load config file if specified
	 */
	if (this->vm.count("configfile"))
	{
		bpo::store(bpo::parse_config_file<char>(this->vm["configfile"].as<std::string>().c_str(), *(Settings::conf)), this->vm, true);
	}
	else if (filename != NULL)
	{
		bpo::store(bpo::parse_config_file<char>(filename, *(Settings::conf)), this->vm, true);
	}
	
	
	/*
	 *	Call checker callbacks.
	 */
	Callbacks* cb = mrmc::settings::Callbacks::getInstance();
	while (cb->checkerList.size() > 0)
	{
		CheckerCallback fptr = cb->checkerList.front();
		cb->checkerList.pop_front();
		
		if (! (*fptr)(this->vm))
		{
			std::cerr << "Custom option checker failed." << std::endl;
			LOG4CPLUS_ERROR(logger, "A checker callback returned false.");
			throw mrmc::exceptions::InvalidSettings();
		}
	}
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
	os << "Usage: " << mrmc::settings::Settings::binaryName << " [options] <transition file> <label file>" << std::endl;
	os << *(mrmc::settings::Settings::cli) << std::endl;
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
	os << *(mrmc::settings::Settings::conf) << std::endl;;
	return os;
}

} // namespace settings
} // namespace mrmc
