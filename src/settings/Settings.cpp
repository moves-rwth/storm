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

	Option* option = this->getPtrByLongName(optionName);
	
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
				if (!this->containsShortName(nextShortOptionName)) {
					// LOG
					throw storm::exceptions::OptionParserException() << "Found an unknown ShortName for an Option: \"" << nextShortOptionName << "\".";
				} else {
					longOptionName = this->getByShortName(nextShortOptionName).getLongName();
					optionActive = true;
				}
			} else {
				// Long Option
				std::string nextLongOptionName = storm::utility::StringHelper::stringToLower(nextOption.substr(2, nextOption.size() - 2));
				if (!this->containsLongName(nextLongOptionName)) {
					// LOG
					throw storm::exceptions::OptionParserException() << "Found an unknown LongName for an Option: \"" << nextLongOptionName << "\".";
				} else {
					longOptionName = this->getByLongName(nextLongOptionName).getLongName();
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

	for (auto it = this->options.cbegin(); it != this->options.cend(); ++it) {
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
	return registrationFunction(myInstance);
}

storm::settings::Settings* storm::settings::Settings::getInstance() {
	// Usually, this would require double-checked locking.
	// But since C++11, this is the way to go:
	static storm::settings::Settings pInstance;

	return &pInstance;
}

storm::settings::Settings& storm::settings::Settings::addOption(Option* option) {
	// For automatic management of option's lifetime
	std::shared_ptr<Option> optionPtr(option);
	
	std::string lowerLongName = storm::utility::StringHelper::stringToLower(option->getLongName());
	std::string lowerShortName = storm::utility::StringHelper::stringToLower(option->getShortName());
	
	auto longNameIterator = this->options.find(lowerLongName);
	auto shortNameIterator = this->shortNames.find(lowerShortName);

	if (longNameIterator == this->options.end()) {
		// Not found
		if (!(shortNameIterator == this->shortNames.end())) {
			// There exists an option which uses the same shortname
			// LOG
			throw storm::exceptions::OptionUnificationException() << "Error: The Option \"" << shortNameIterator->second << "\" from Module \"" << this->options.find(shortNameIterator->second)->second.get()->getModuleName() << "\" uses the same ShortName as the Option \"" << option->getLongName() << "\" from Module \"" << option->getModuleName() << "\"!";
		}
		
		// Copy Shared_ptr
		this->options.insert(std::make_pair(lowerLongName, std::shared_ptr<Option>(optionPtr)));
		this->optionPointers.push_back(std::shared_ptr<Option>(optionPtr));
		// Ignore Options with empty shortName
		if (!lowerShortName.empty()) {
			this->shortNames.insert(std::make_pair(lowerShortName, lowerLongName));
		}
	} else {
		// This will fail if the shortNames are not identical, so no additional checks here.
		longNameIterator->second.get()->unify(*option);
	}

	return *this;
}

std::string storm::settings::Settings::getHelpText() const {
	
	// Copy all option names into a vector and sort it
	std::vector<std::string> optionNames;
	optionNames.reserve(this->options.size());

	size_t longNameMaxSize = 0;
	size_t shortNameMaxSize = 0;
	size_t argumentNameMaxSize = 0;
	// Get the maximum size of the long and short Names and copy the long names for sorting
	std::for_each(this->options.cbegin(), this->options.cend(), [&] (std::pair<std::string, std::shared_ptr<storm::settings::Option>> const& it) -> void { 
		longNameMaxSize = std::max(longNameMaxSize, it.first.size()); 
		shortNameMaxSize = std::max(shortNameMaxSize, it.second.get()->getShortName().size()); 
		optionNames.push_back(it.first);
		std::for_each(it.second.get()->arguments.cbegin(), it.second.get()->arguments.cend(), [&] (std::shared_ptr<ArgumentBase> const& arg) -> void {
			argumentNameMaxSize = std::max(argumentNameMaxSize, arg.get()->getArgumentName().size());
		});
	});
	// Sort the long names
	std::sort(optionNames.begin(), optionNames.end(), [] (std::string const& a, std::string const& b) -> bool { return a.compare(b) < 0; });

	std::stringstream ss;
	
	/*
		Layout:
		--longName -shortName Description
			ArgumentName (ArgumentType) ArgumentDescription
	*/
	const std::string delimiter = "   ";

	for (auto it = optionNames.cbegin(); it != optionNames.cend(); ++it) {
		Option const& o = this->getByLongName(*it);
		std::string const& longName = o.getLongName();
		ss << delimiter << "--" << longName;
		// Fill up the remaining space after the long Name
		for (uint_fast64_t i = longName.size(); i < longNameMaxSize; ++i) {
			ss << " ";
		}
		std::string const& shortName = o.getShortName();
		ss << delimiter << "-" << shortName;
		// Fill up the remaining space after the short Name
		for (uint_fast64_t i = shortName.size(); i < shortNameMaxSize; ++i) {
			ss << " ";
		}
		ss << delimiter << o.getDescription() << std::endl;

		for (auto i = 0; i < o.getArgumentCount(); ++i) {
			ArgumentBase const& a = o.getArgument(i);
			std::string const& argumentName = a.getArgumentName();
			ss << delimiter << delimiter << "Arg " << (i+1) << ": " << argumentName;
			// Fill up the remaining space after the argument Name
			for (uint_fast64_t i = argumentName.size(); i < argumentNameMaxSize; ++i) {
				ss << " ";
			}
			ss << "(" << ArgumentTypeHelper::toString(a.getArgumentType()) << ")" << delimiter << a.getArgumentDescription() << std::endl;
		}
	}

	return ss.str();
}