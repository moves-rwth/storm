/*
 * Option.h
 *
 *  Created on: 11.08.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_SETTINGS_OPTION_H_
#define STORM_SETTINGS_OPTION_H_

#include <iostream>
#include <string>
#include <cstdint>
#include <cctype>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_set>

#include "ArgumentType.h"
#include "ArgumentBase.h"
#include "Argument.h"

#include "src/utility/StringHelper.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/OptionUnificationException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
	namespace settings {

		class Settings;

		class Option {
		public:

			friend class storm::settings::Settings;

			Option(std::string const& moduleName, std::string const& longOptionName, std::string const& shortOptionName, std::string const& optionDescription, bool isOptionRequired)
				: longName(longOptionName), shortName(shortOptionName), description(optionDescription), moduleName(moduleName), isRequired(isOptionRequired), hasBeenSet(false) {
				validateFields();
			}

			Option(std::string const& moduleName, std::string const& longOptionName, std::string const& shortOptionName, std::string const& optionDescription, bool isOptionRequired, std::vector<std::shared_ptr<ArgumentBase>> const& optionArguments)
				: longName(longOptionName), shortName(shortOptionName), description(optionDescription), moduleName(moduleName), isRequired(isOptionRequired), hasBeenSet(false) {
				// Copy all Arguments
				this->arguments.reserve(optionArguments.size());
				for (uint_fast64_t i = 0; i < optionArguments.size(); ++i) {
					// Clone gives a deep copy
					this->arguments.push_back(std::shared_ptr<ArgumentBase>(optionArguments.at(i).get()->clone()));
				}
				
				isArgumentsVectorValid(this->arguments);

				validateFields();
			}

			Option(Option const& other): longName(other.longName), shortName(other.shortName), description(other.description), moduleName(other.moduleName), isRequired(other.isRequired), hasBeenSet(other.hasBeenSet) {
				// Copy all Arguments
				this->arguments.reserve(other.arguments.size());
				for (size_t i = 0; i < other.arguments.size(); ++i) {
					// Clone gives a deep copy
					this->arguments.push_back(std::shared_ptr<ArgumentBase>(other.arguments.at(i).get()->clone()));
				}

				isArgumentsVectorValid(this->arguments);

				validateFields();
			}

			~Option() {
				//std::cout << "Destructing an Option." << std::endl;

				this->arguments.clear();
				this->argumentNameMap.clear();
			}

			Option* clone() const {
				return new Option(*this);
			}

			void unify(Option& other) {
				if (this->getLongName().compare(other.getLongName()) != 0) {
					LOG4CPLUS_ERROR(logger, "Option::unify: Could not unify Option \"" << getLongName() << "\" because the Names are different (\"" << getLongName() << "\" vs. \"" << other.getLongName() << "\")!");
					throw storm::exceptions::OptionUnificationException() << "Could not unify Option \"" << getLongName() << "\" because the Names are different (\"" << getLongName() << "\" vs. \"" << other.getLongName() << "\")!";
				}
				if (this->getShortName().compare(other.getShortName()) != 0) {
					LOG4CPLUS_ERROR(logger, "Option::unify: Could not unify Option \"" << getLongName() << "\" because the Shortnames are different (\"" << getShortName() << "\" vs. \"" << other.getShortName() << "\")!");
					throw storm::exceptions::OptionUnificationException() << "Could not unify Option \"" << getLongName() << "\" because the Shortnames are different (\"" << getShortName() << "\" vs. \"" << other.getShortName() << "\")!";
				}

				if (this->getArgumentCount() != other.getArgumentCount()) {
					LOG4CPLUS_ERROR(logger, "Option::unify: Could not unify Option \"" << getLongName() << "\" because the Argument Counts are different!");
					throw storm::exceptions::OptionUnificationException() << "Could not unify Option \"" << getLongName() << "\" because the Argument Counts are different!";
				}
				for(size_t i = 0; i != this->arguments.size(); i++) {
					ArgumentBase* A = this->arguments.at(i).get();
					ArgumentBase* B = other.arguments.at(i).get();

					if (A->getArgumentType() != B->getArgumentType()) {
						LOG4CPLUS_ERROR(logger, "Option::unify: Could not unify Option \"" << getLongName() << "\" because the Argument Types at Index " << i << " are different!");
						throw storm::exceptions::OptionUnificationException() << "Could not unify Option \"" << getLongName() << "\" because the Argument Types at Index " << i << " are different!";
					}

					switch (A->getArgumentType()) {
						case ArgumentType::String:
							static_cast<storm::settings::Argument<std::string>*>(A)->unify(*static_cast<storm::settings::Argument<std::string>*>(B));
							break;
						case ArgumentType::Integer:
							static_cast<storm::settings::Argument<int_fast64_t>*>(A)->unify(*static_cast<storm::settings::Argument<int_fast64_t>*>(B));
							break;
						case ArgumentType::UnsignedInteger:
							static_cast<storm::settings::Argument<uint_fast64_t>*>(A)->unify(*static_cast<storm::settings::Argument<uint_fast64_t>*>(B));
							break;
						case ArgumentType::Double:
							static_cast<storm::settings::Argument<double>*>(A)->unify(*static_cast<storm::settings::Argument<double>*>(B));
							break;
						case ArgumentType::Boolean:
							static_cast<storm::settings::Argument<bool>*>(A)->unify(*static_cast<storm::settings::Argument<bool>*>(B));
							break;
						default:
							LOG4CPLUS_ERROR(logger, "Option::unify: Missing Case in ArgumentBuilder's switch/case Code.");
							throw storm::exceptions::InternalTypeErrorException() << "Missing Case in ArgumentBuilder's switch/case Code.";
					}
				}

				if (this->getModuleName().compare(other.getModuleName()) != 0) {
					this->moduleName.append(", ").append(other.getModuleName());
				}
			}

			uint_fast64_t getArgumentCount() const {
				return this->arguments.size();
			}

			ArgumentBase& getArgument(uint_fast64_t argumentIndex) const {
				if (argumentIndex >= getArgumentCount()) {
					LOG4CPLUS_ERROR(logger, "Option::getArgument: argumentIndex out of bounds!");
					throw storm::exceptions::IllegalArgumentException() << "Option::getArgument(): argumentIndex out of bounds!";
				}
				return *this->arguments.at(argumentIndex).get();
			}

			/*!
			* Returns a reference to the Argument with the specified longName.
			* Throws an Exception of Type IllegalArgumentException if there is no such Option.
			*/
			ArgumentBase const& getArgumentByName(std::string const& argumentName) const {
				auto argumentIterator = this->argumentNameMap.find(storm::utility::StringHelper::stringToLower(argumentName));

				if (argumentIterator == this->argumentNameMap.end()) {
					LOG4CPLUS_ERROR(logger, "Option::getArgumentByName: The Option \"" << this->getLongName() << "\" does not contain an Argument with Name \"" << argumentName << "\"!");
					throw storm::exceptions::IllegalArgumentException() << "The Option \"" << this->getLongName() << "\" does not contain an Argument with Name \"" << argumentName << "\"!";
				}

				return *argumentIterator->second.get();
			}

			std::string const& getLongName() const {
				return this->longName;
			}

			std::string const& getShortName() const {
				return this->shortName;
			}

			std::string const& getDescription() const {
				return this->description;
			}

			std::string const& getModuleName() const {
				return this->moduleName;
			}

			bool getIsRequired() const {
				return this->isRequired;
			}

			bool getHasOptionBeenSet() const {
				return this->hasBeenSet;
			}

			void setHasOptionBeenSet() {
				this->hasBeenSet = true;
			}
		private:
			std::string longName;
			std::string shortName;
			std::string description;
			std::string moduleName;

			bool isRequired;
			bool hasBeenSet;

			std::vector<std::shared_ptr<ArgumentBase>> arguments;

			std::unordered_map<std::string, std::shared_ptr<ArgumentBase>> argumentNameMap;

			void validateFields() const {
				if (longName.empty()) {
					LOG4CPLUS_ERROR(logger, "Option::validateFields: Tried constructing an Option with an empty longName field!");
					throw storm::exceptions::IllegalArgumentException() << "Tried constructing an Option with an empty longName field!";
				}

				if (moduleName.empty()) {
					LOG4CPLUS_ERROR(logger, "Option::validateFields: Tried constructing an Option with an empty moduleName field!");
					throw storm::exceptions::IllegalArgumentException() << "Tried constructing an Option with an empty moduleName field!";
				}

				bool longNameContainsNonAlpha = std::find_if(longName.begin(), longName.end(), [](char c) { return !std::isalpha(c); }) != longName.end();
				bool shortNameContainsNonAlpha = std::find_if(shortName.begin(), shortName.end(), [](char c) { return !std::isalpha(c); }) != shortName.end();

				if (longNameContainsNonAlpha) {
					LOG4CPLUS_ERROR(logger, "Option::validateFields: Tried constructing an Option with a longName that contains non-alpha characters!");
					throw storm::exceptions::IllegalArgumentException() << "Tried constructing an Option with a longName that contains non-alpha characters!";
				}
				if (shortNameContainsNonAlpha) {
					LOG4CPLUS_ERROR(logger, "Option::validateFields: Tried constructing an Option with a shortName that contains non-alpha characters!");
					throw storm::exceptions::IllegalArgumentException() << "Tried constructing an Option with a shortName that contains non-alpha characters!";
				}
			}

			bool isArgumentsVectorValid(std::vector<std::shared_ptr<ArgumentBase>> const& arguments) {
				bool lastEntryWasOptional = false;
				std::unordered_set<std::string> argumentNameSet;
				for (auto i = arguments.begin(); i != arguments.end(); ++i) {
					bool isCurrentArgumentOptional = i->get()->getIsOptional();
					//if (!this->isRequired && !i->get()->getHasDefaultValue()) {
						// LOG
					//	throw storm::exceptions::IllegalArgumentException() << "Error: The Argument Vector specified for Option \"" << getLongName() << "\" is invalid!\nIt contains an argument without a default value, but the containing option is optional and therefor requires all arguments to provide default values.";
					//}

					if (!isCurrentArgumentOptional && lastEntryWasOptional) {
						LOG4CPLUS_ERROR(logger, "Option::isArgumentsVectorValid: The Argument Vector specified for Option \"" << getLongName() << "\" is invalid! It contains a non-optional argument AFTER an optional argument.");
						throw storm::exceptions::IllegalArgumentException() << "The Argument Vector specified for Option \"" << getLongName() << "\" is invalid! It contains a non-optional argument AFTER an optional argument.";
					}
					std::string lowerArgumentName = storm::utility::StringHelper::stringToLower(i->get()->getArgumentName());
					if (argumentNameSet.find(lowerArgumentName) != argumentNameSet.end()) {
						LOG4CPLUS_ERROR(logger, "Option::isArgumentsVectorValid: The Argument Vector specified for Option \"" << getLongName() << "\" is invalid!\nIt contains two arguments with the same name.");
						throw storm::exceptions::IllegalArgumentException() << "The Argument Vector specified for Option \"" << getLongName() << "\" is invalid!\nIt contains two arguments with the same name.";
					}
					argumentNameSet.insert(lowerArgumentName);

					// This copies the Name to the Name Lookup Map
					argumentNameMap.insert(std::make_pair(lowerArgumentName, std::shared_ptr<ArgumentBase>(*i)));
					lastEntryWasOptional = isCurrentArgumentOptional;
				}
				return true;
			}
		};
	}
}

#endif // STORM_SETTINGS_OPTION_H_