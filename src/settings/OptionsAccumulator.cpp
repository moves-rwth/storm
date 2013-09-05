#include "src/settings/OptionsAccumulator.h"

/*!
* The map holding the information regarding registered options and their types
*/
//std::unordered_map<std::string, std::shared_ptr<Option>> options;

/*!
* The map holding the information regarding registered options and their short names
*/
//std::unordered_map<std::string, std::string> shortNames;

storm::settings::OptionsAccumulator& storm::settings::OptionsAccumulator::addOption(Option* option) {
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
		this->shortNames.insert(std::make_pair(lowerShortName, lowerLongName));
	} else {
		// This will fail if the shortNames are not identical, so no additional checks here.
		longNameIterator->second.get()->unify(*option);
	}

	return *this;
}

void storm::settings::OptionsAccumulator::join(storm::settings::OptionsAccumulator const& rhs) {
	for (auto it = rhs.options.begin(); it != rhs.options.end(); ++it) {
		// Clone all Options
		this->addOption(it->second.get()->clone());
	}
}