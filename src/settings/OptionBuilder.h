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
					LOG4CPLUS_ERROR(logger, "OptionBuilder::addArgument: Illegal call to addArgument() on an instance of OptionBuilder that has already built an instance.");
					throw storm::exceptions::IllegalFunctionCallException() << "Illegal call to addArgument() on an instance of OptionBuilder that has already built an instance.";
				}

				if (newArgument->getArgumentType() == ArgumentType::Invalid) {
					LOG4CPLUS_ERROR(logger, "OptionBuilder::addArgument: Unable to add argument to option \"" << getLongName() << "\" because its type is invalid.");
					throw storm::exceptions::InternalTypeErrorException() << "Unable to add argument to option \"" << getLongName() << "\" because its type is invalid.";
				}
				
				if (!newArgument->getIsOptional() && (this->arguments.size() > 0) && (this->arguments.at(this->arguments.size() - 1).get()->getIsOptional())) {
					LOG4CPLUS_ERROR(logger, "OptionBuilder::addArgument: Unable to add a non-optional argument to option \"" << getLongName() << "\", because it already contains an optional argument.");
					throw storm::exceptions::IllegalArgumentException() << "Unable to add non-optional argument to option \"" << getLongName() << "\", because it already contains an optional argument.";
				}

				std::string lowerArgumentName = storm::utility::StringHelper::stringToLower(newArgument->getArgumentName());
				if (argumentNameSet.find(lowerArgumentName) != argumentNameSet.end()) {
					LOG4CPLUS_ERROR(logger, "OptionBuilder::addArgument: Unable to add argument with name \"" << newArgument->getArgumentName() << "\" to option \"" << getLongName() << "\", because it already contains an argument with the same name.");
					throw storm::exceptions::IllegalArgumentException() << "Unable to add argument with name \"" << newArgument->getArgumentName() << "\" to option \"" << getLongName() << "\", because it already contains an argument with the same name.";
				}
				argumentNameSet.insert(lowerArgumentName);

				this->arguments.push_back(std::shared_ptr<ArgumentBase>(argumentPtr));

				return *this;
			}

			Option* build() {
				if (this->isBuild) {
					LOG4CPLUS_ERROR(logger, "OptionBuilder::addArgument: Illegal call to build() on an instance of OptionBuilder that has already built an instance.");
					throw storm::exceptions::IllegalFunctionCallException() << "Illegal call to build() on an instance of OptionBuilder that has already built an instance.";
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