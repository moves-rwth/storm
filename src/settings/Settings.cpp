#include "Settings.h"

#include <cstring>
#include <cctype>
#include <mutex>

#include "src/exceptions/OptionParserException.h"

// Static Inits
storm::settings::Settings* storm::settings::Settings::instance = nullptr;
storm::settings::Destroyer storm::settings::Settings::destroyer;

/*!
 *	@brief	Create new instance.
 *
 * 	Creates a new Settings instance and passes the arguments to the constructor of Settings.
 *
 *	@param argc should be argc passed to main function
 *	@param argv should be argv passed to main function
 *	@param filename either NULL or name of config file
 *	@return The new instance of Settings.
 */
void storm::settings::Settings::parse(int const argc, char const * const argv[]) {
	storm::settings::Settings* myInstance = storm::settings::Settings::getInstance();
	myInstance->parseCommandLine(argc, argv);
}

bool storm::settings::Settings::hasAssignment(std::string const& option) {
	return (option.find_first_of('=', 0) != std::string::npos);
}

storm::settings::stringPair_t storm::settings::Settings::splitOptionString(std::string const& option) {
	size_t splitLocation = option.find_first_of('=', 0);
	if (splitLocation == std::string::npos) {
		// Second half is empty
		return std::make_pair(option, "");
	} else if (splitLocation + 1 >= option.length()) {
		// Remove the = character
		return std::make_pair(option.substr(0, option.size() - 1), "");
	}
	return std::make_pair(option.substr(0, splitLocation), option.substr(splitLocation + 1, std::string::npos));
}

void storm::settings::Settings::handleAssignment(std::string const& longOptionName, std::vector<std::string> arguments) {
	std::string optionName = storm::utility::StringHelper::stringToLower(longOptionName);

	Option* option = this->optionsAccumulator->getPtrByLongName(optionName);
	
	// Mark as Set
	option->setHasOptionBeenSet();
	
	uint_fast64_t givenArgsCount = arguments.size();

	if (givenArgsCount > option->getArgumentCount()) {
		throw storm::exceptions::OptionParserException() << "Could not parse Arguments for Option \"" << longOptionName << "\": " << arguments.size() << " Arguments given, but max. " << option->getArgumentCount() << " Arguments expected.";
	}

	for (uint_fast64_t i = 0; i < option->getArgumentCount(); ++i) {
		if (i < givenArgsCount) {
			storm::settings::fromStringAssignmentResult_t assignmentResult = option->getArgument(i).fromStringValue(arguments.at(i));
			if (!assignmentResult.first) {
				// LOG
				throw storm::exceptions::OptionParserException() << "Could not parse Arguments for Option \"" << longOptionName << "\": Argument " << option->getArgument(i).getArgumentName() << " rejected the given Value \"" << arguments.at(i) << "\" with Message:\r\n" << assignmentResult.second;
			}
		} else {
			// There is no given value for this argument, only optional
			if (!option->getArgument(i).getIsOptional()) {
				// LOG
				throw storm::exceptions::OptionParserException() << "Could not parse Arguments for Option \"" << longOptionName << "\": " << arguments.size() << " Arguments given, but more Arguments were expected.";
			} else {
				option->getArgument(i).setFromDefaultValue();
			}
		}
	}
}

std::vector<std::string> storm::settings::Settings::argvToStringArray(int const argc, char const * const argv[]) {
	// Ignore argv[0], it contains the program path and name
	std::vector<std::string> result;
	for (int i = 1; i < argc; ++i) {
		result.push_back(std::string(argv[i]));
	}
	return result;
}

bool storm::settings::Settings::checkArgumentSyntaxForOption(std::string const& argvString) {
	if (argvString.size() < 2) {
		return false;
	}

	if ((argvString.at(0) != '-') || ((argvString.at(1) != '-') && !isalpha(argvString.at(1)))) {
		return false;
	}

	for (auto i = 2; i < argvString.size(); ++i) {
		if (!isalpha(argvString.at(i))) {
			return false;
		}
	}
	return true;
}

std::vector<bool> storm::settings::Settings::scanForOptions(std::vector<std::string> const& arguments) {
	std::vector<bool> result;
	result.reserve(arguments.size());
	for (auto it = arguments.cbegin(); it != arguments.cend(); ++it) {
		result.push_back(checkArgumentSyntaxForOption(*it));
	}
	return result;
}

void storm::settings::Settings::parseCommandLine(int const argc, char const * const argv[]) {
	std::cout << "Parsing " << argc << " arguments." << std::endl;	
	
	std::vector<std::string> stringArgv = argvToStringArray(argc, argv);
	std::vector<bool> optionPositions = scanForOptions(stringArgv);

	bool optionActive = false;
	std::string longOptionName;
	std::vector<std::string> argCache;
	for (auto i = 0; i <= stringArgv.size(); ++i) {
		if (i == stringArgv.size()) {
			if (optionActive) {
				this->handleAssignment(longOptionName, argCache);
				argCache.clear();
			}
			break;
		} else if (optionPositions.at(i)) {
			if (optionActive) {
				this->handleAssignment(longOptionName, argCache);
				argCache.clear();
			}

			std::string const& nextOption = stringArgv.at(i);
			if (nextOption.at(0) == '-' && nextOption.at(1) != '-') {
				// Short Option
				std::string nextShortOptionName = storm::utility::StringHelper::stringToLower(nextOption.substr(1, nextOption.size() - 1));
				if (!this->optionsAccumulator->containsShortName(nextShortOptionName)) {
					// LOG
					throw storm::exceptions::OptionParserException() << "Found an unknown ShortName for an Option: \"" << nextShortOptionName << "\".";
				} else {
					longOptionName = this->optionsAccumulator->getByShortName(nextShortOptionName).getLongName();
					optionActive = true;
				}
			} else {
				// Long Option
				std::string nextLongOptionName = storm::utility::StringHelper::stringToLower(nextOption.substr(2, nextOption.size() - 2));
				if (!this->optionsAccumulator->containsLongName(nextLongOptionName)) {
					// LOG
					throw storm::exceptions::OptionParserException() << "Found an unknown LongName for an Option: \"" << nextLongOptionName << "\".";
				} else {
					longOptionName = this->optionsAccumulator->getByLongName(nextLongOptionName).getLongName();
					optionActive = true;
				}
			}
		} else if (optionActive) {
			// Next argument for an Option found
			argCache.push_back(stringArgv.at(i));
		} else {
			// No Option active and this is no option.
			// LOG
			throw storm::exceptions::OptionParserException() << "Found a stray argument while parsing a given configuration: \"" << stringArgv.at(i) << "\" is neither a known Option nor preceeded by an Option.";
		}
	}

	for (auto it = this->optionsAccumulator->options.cbegin(); it != this->optionsAccumulator->options.cend(); ++it) {
		if (!it->second.get()->getHasOptionBeenSet()) {
			if (it->second.get()->getIsRequired()) {
				throw storm::exceptions::OptionParserException() << "Option \"" << it->second.get()->getLongName() << "\" is marked as required, but was not set!";
			} else {
				// Set defaults on optional values
				for (auto i = 0; i < it->second.get()->getArgumentCount(); ++i) {
					it->second.get()->getArgument(i).setFromDefaultValue();
				}
			}
		}
	}
}

bool storm::settings::Settings::registerNewModule(ModuleRegistrationFunction_t registrationFunction) {
	Settings* myInstance = Settings::getInstance();
	return registrationFunction(myInstance->optionsAccumulator);
}

storm::settings::Settings* storm::settings::Settings::getInstance() {
	// Usually, this would require double-checked locking.
	// But since C++11, this is the way to go:
	static storm::settings::Settings pInstance;

	return &pInstance;
}

void storm::settings::Settings::addOptions(OptionsAccumulator const& options) {
	this->optionsAccumulator->join(options);
}