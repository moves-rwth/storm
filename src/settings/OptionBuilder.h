/*
 * OptionBuilder.h
 *
 *  Created on: 11.08.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_SETTINGS_OPTIONBUILDER_H_
#define STORM_SETTINGS_OPTIONBUILDER_H_

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_set>

#include "ArgumentType.h"
#include "ArgumentBase.h"
#include "Option.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/IllegalFunctionCallException.h"

namespace storm {
	namespace settings {

		class OptionBuilder {
		public:
			OptionBuilder(std::string const& newOptionModuleName, std::string const& newOptionLongName, std::string const& newOptionShortName, std::string const& newOptionDescription): longName(newOptionLongName), shortName(newOptionShortName), description(newOptionDescription), moduleName(newOptionModuleName), isRequired(false), isBuild(false) {}

			~OptionBuilder() {}

			OptionBuilder& setLongName(std::string const& newLongName) {
				this->longName = newLongName;
				
				return *this;
			}

			std::string const& getLongName() const {
				return this->longName;
			}

			OptionBuilder& setShortName(std::string const& newShortName) {
				this->shortName = newShortName;
				
				return *this;
			}

			std::string const& getShortName() const {
				return this->shortName;
			}

			OptionBuilder& setDescription(std::string const& newDescription) {
				this->description = newDescription;
				
				return *this;
			}

			std::string const& getDescription() const {
				return this->description;
			}

			OptionBuilder& setModuleName(std::string const& newModuleName) {
				this->moduleName = newModuleName;
				
				return *this;
			}

			std::string const& getModuleName() const {
				return this->moduleName;
			}

			OptionBuilder& setIsRequired(bool newIsRequired) {
				this->isRequired = newIsRequired;
				
				return *this;
			}

			bool getIsRequired() const {
				return this->isRequired;
			}

			OptionBuilder& addArgument(ArgumentBase* newArgument) {
				// For automatic management of newArgument's lifetime
				std::shared_ptr<ArgumentBase> argumentPtr(newArgument);
				if (this->isBuild) {
					LOG4CPLUS_ERROR(logger, "OptionBuilder::addArgument: Called addArgument() on an instance of OptionBuilder which has already build an Instance.");
					throw storm::exceptions::IllegalFunctionCallException() << "Called addArgument() on an instance of OptionBuilder which has already build an Instance.";
				}

				if (newArgument->getArgumentType() == ArgumentType::Invalid) {
					LOG4CPLUS_ERROR(logger, "OptionBuilder::addArgument: Could not add Argument to Option \"" << getLongName() << "\" because its Type is Invalid!");
					throw storm::exceptions::InternalTypeErrorException() << "Could not add Argument to Option \"" << getLongName() << "\" because its Type is Invalid!";
				}
				
				if (!newArgument->getIsOptional() && (this->arguments.size() > 0) && (this->arguments.at(this->arguments.size() - 1).get()->getIsOptional())) {
					LOG4CPLUS_ERROR(logger, "OptionBuilder::addArgument: Could not add Non-Optional Argument to Option \"" << getLongName() << "\" because it already contains an optional argument! Please note that after an optional argument has been added only arguments which are also optional can be appended.");
					throw storm::exceptions::IllegalArgumentException() << "Could not add Non-Optional Argument to Option \"" << getLongName() << "\" because it already contains an optional argument! Please note that after an optional argument has been added only arguments which are also optional can be appended.";
				}

				std::string lowerArgumentName = storm::utility::StringHelper::stringToLower(newArgument->getArgumentName());
				if (argumentNameSet.find(lowerArgumentName) != argumentNameSet.end()) {
					LOG4CPLUS_ERROR(logger, "OptionBuilder::addArgument: Could not add Argument with Name \"" << newArgument->getArgumentName() << "\" to Option \"" << getLongName() << "\" because it already contains an argument with the same name! Please note that all argument names must be unique in its respective option.");
					throw storm::exceptions::IllegalArgumentException() << "Could not add Argument with Name \"" << newArgument->getArgumentName() << "\" to Option \"" << getLongName() << "\" because it already contains an argument with the same name! Please note that all argument names must be unique in its respective option.";
				}
				argumentNameSet.insert(lowerArgumentName);

				this->arguments.push_back(std::shared_ptr<ArgumentBase>(argumentPtr));

				return *this;
			}

			Option* build() {
				if (this->isBuild) {
					LOG4CPLUS_ERROR(logger, "OptionBuilder::addArgument: Called build() on an instance of OptionBuilder which has already build an Instance.");
					throw storm::exceptions::IllegalFunctionCallException() << "Called build() on an instance of OptionBuilder which has already build an Instance.";
				}

				this->isBuild = true;

				return new storm::settings::Option(this->moduleName, this->longName, this->shortName, this->description, this->isRequired, this->arguments);
			}
		private:
			std::string longName;
			std::string shortName;
			std::string description;
			std::string moduleName;

			bool isRequired;
			bool isBuild;

			std::vector<std::shared_ptr<ArgumentBase>> arguments;

			std::unordered_set<std::string> argumentNameSet;
		};
	}
}

#endif // STORM_SETTINGS_OPTIONBUILDER_H_