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

#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/OptionUnificationException.h"

namespace storm {
	namespace settings {

        // Forward-declare settings manager class.
        class SettingsManager;
        
        /*!
         * This class represents one command-line option.
         */
		class Option {
		public:
            // Declare settings manager class as friend.
			friend class SettingsManager;

            /*!
             * Creates an option with the given parameters.
             *
             * @param moduleName The module to which this option belongs.
             * @param longOptionName The long option name.
             * @param optionDescription The description of the option.
             * @param isOptionRequired Sets whether the option is required to appear.
             * @param requireModulePrefix A flag that indicates whether this option requires to be prefixed with the
             * module name.
             * @param optionArguments The arguments of the option.
             */
			Option(std::string const& moduleName, std::string const& longOptionName, std::string const& optionDescription, bool isOptionRequired, bool requireModulePrefix, std::vector<std::shared_ptr<ArgumentBase>> const& optionArguments = std::vector<std::shared_ptr<ArgumentBase>>()) : Option(moduleName, longOptionName, "", false, optionDescription, isOptionRequired, requireModulePrefix, optionArguments) {
				// Intentionally left empty.
			}
            
            /*!
             * Creates an option with the given parameters.
             *
             * @param moduleName The module to which this option belongs.
             * @param longOptionName The long option name.
             * @param shortOptionName The short option name.
             * @param optionDescription The description of the option.
             * @param isOptionRequired Sets whether the option is required to appear.
             * @param requireModulePrefix A flag that indicates whether this option requires to be prefixed with the
             * module name.
             * @param optionArguments The arguments of the option.
             */
            Option(std::string const& moduleName, std::string const& longOptionName, std::string const& shortOptionName, std::string const& optionDescription, bool isOptionRequired, bool requireModulePrefix, std::vector<std::shared_ptr<ArgumentBase>> const& optionArguments = std::vector<std::shared_ptr<ArgumentBase>>()) : Option(moduleName, longOptionName, shortOptionName, true, optionDescription, isOptionRequired, requireModulePrefix, optionArguments) {
                // Intentionally left empty.
            }

            /*!
             * Checks whether the given option is compatible with the current one. If not, an exception is thrown.
             *
             * @param other The other option with which to check compatibility.
             * @return True iff the given argument is compatible with the current one.
             */
			bool isCompatibleWith(Option const& other) {
                LOG_THROW(this->getArgumentCount() == other.getArgumentCount(), storm::exceptions::OptionUnificationException, "Unable to unify two options, because their argument count differs.");

				for(size_t i = 0; i != this->arguments.size(); i++) {
                    ArgumentBase const& firstArgument = this->getArgument(i);
                    ArgumentBase const& secondArgument = other.getArgument(i);

                    LOG_THROW(firstArgument.getType() == secondArgument.getType(), storm::exceptions::OptionUnificationException, "Unable to unify two options, because their arguments are incompatible.");

					switch (firstArgument.getType()) {
						case ArgumentType::String:
							static_cast<storm::settings::Argument<std::string> const&>(firstArgument).isCompatibleWith(static_cast<storm::settings::Argument<std::string> const&>(secondArgument));
							break;
						case ArgumentType::Integer:
							static_cast<storm::settings::Argument<int_fast64_t> const&>(firstArgument).isCompatibleWith(static_cast<storm::settings::Argument<int_fast64_t> const&>(secondArgument));
							break;
						case ArgumentType::UnsignedInteger:
							static_cast<storm::settings::Argument<uint_fast64_t> const&>(firstArgument).isCompatibleWith(static_cast<storm::settings::Argument<uint_fast64_t> const&>(secondArgument));
							break;
						case ArgumentType::Double:
							static_cast<storm::settings::Argument<double> const&>(firstArgument).isCompatibleWith(static_cast<storm::settings::Argument<double> const&>(secondArgument));
							break;
						case ArgumentType::Boolean:
							static_cast<storm::settings::Argument<bool> const&>(firstArgument).isCompatibleWith(static_cast<storm::settings::Argument<bool> const&>(secondArgument));
							break;
					}
				}
                return true;
			}

            /*!
             * Retrieves the argument count this option expects.
             *
             * @return The argument count of this option.
             */
			uint_fast64_t getArgumentCount() const {
				return this->arguments.size();
			}

            /*!
             * Retrieves the i-th argument of this option.
             *
             * @param argumentIndex The index of the argument to retrieve.
             * @return The i-th argument of this option.
             */
			ArgumentBase const& getArgument(uint_fast64_t argumentIndex) const {
                LOG_THROW(argumentIndex < this->getArgumentCount(), storm::exceptions::IllegalArgumentException, "Index of argument is out of bounds.");
				return *this->arguments.at(argumentIndex);
			}

			/*!
             * Returns a reference to the argument with the specified long name.
             *
             * @param argumentName The name of the argument to retrieve.
             * @return The argument with the given name.
             */
			ArgumentBase const& getArgumentByName(std::string const& argumentName) const {
				auto argumentIterator = this->argumentNameMap.find(argumentName);
                LOG_THROW(argumentIterator != this->argumentNameMap.end(), storm::exceptions::IllegalArgumentException, "Unable to retrieve argument with unknown name " << argumentName << ".");
				return *argumentIterator->second;
			}

            /*!
             * Retrieves the long name of this option.
             *
             * @return The long name of this option.
             */
			std::string const& getLongName() const {
				return this->longName;
			}

            /*!
             * Retrieves whether this option has a short name.
             *
             * @return True iff the option has a short name.
             */
            bool getHasShortName() const {
                return this->hasShortName;
            }
            
            /*!
             * Retrieves the short name of this option.
             *
             * @return The short name of this option.
             */
			std::string const& getShortName() const {
				return this->shortName;
			}

            /*!
             * Retrieves the description of the option.
             *
             * @return The description of the option.
             */
			std::string const& getDescription() const {
				return this->description;
			}

            /*!
             * Retrieves the name of the module to which this option belongs.
             *
             * @return The name of the module to which this option belongs.
             */
			std::string const& getModuleName() const {
				return this->moduleName;
			}

            /*!
             * Retrieves whether the option is required.
             *
             * @return True iff the option is required.
             */
			bool getIsRequired() const {
				return this->isRequired;
			}

            /*!
             * Retrieves whether the option has been set.
             *
             * @return True iff the option has been set.
             */
			bool getHasOptionBeenSet() const {
				return this->hasBeenSet;
			}

		private:
            // The long name of the option.
			std::string longName;
            
            // A flag that indicates whether the option has a short name.
            bool hasShortName;
            
            // The short name of the option if any is set and an empty string otherwise.
			std::string shortName;
            
            // The description of the option.
			std::string description;
            
            // The name of the module to which this option belongs.
			std::string moduleName;

            // A flag that indicates whether this option is required to appear.
			bool isRequired;
            
            // A flag that indicates whether this option is required to be prefixed with the module name.
            bool requireModulePrefix;
            
            // A flag that indicates whether this option has been set.
			bool hasBeenSet;

            // The arguments of this option (possibly empty).
			std::vector<std::shared_ptr<ArgumentBase>> arguments;

            // A mapping from argument names of this option to the actual arguments.
			std::unordered_map<std::string, std::shared_ptr<ArgumentBase>> argumentNameMap;

            /*!
             * Creates an option with the given parameters.
             *
             * @param moduleName The module to which this option belongs.
             * @param longOptionName The long option name.
             * @param shortOptionName The short option name.
             * @param hasShortOptionName A flag that indicates whether this option has a short name.
             * @param optionDescription The description of the option.
             * @param isOptionRequired Sets whether the option is required to appear.
             * @param requireModulePrefix A flag that indicates whether this option requires to be prefixed with the
             * module name.
             * @param optionArguments The arguments of the option.
             */
            Option(std::string const& moduleName, std::string const& longOptionName, std::string const& shortOptionName, bool hasShortOptionName, std::string const& optionDescription, bool isOptionRequired, bool requireModulePrefix, std::vector<std::shared_ptr<ArgumentBase>> const& optionArguments = std::vector<std::shared_ptr<ArgumentBase>>()) : longName(longOptionName), hasShortName(hasShortOptionName), shortName(shortOptionName), description(optionDescription), moduleName(moduleName), isRequired(isOptionRequired), requireModulePrefix(requireModulePrefix), hasBeenSet(false), arguments(), argumentNameMap() {

                // First, do some sanity checks.
                LOG_THROW(!longName.empty(), storm::exceptions::IllegalArgumentException, "Unable to construct option with empty name.");
                LOG_THROW(!moduleName.empty(), storm::exceptions::IllegalArgumentException, "Unable to construct option with empty module name.");
                
                bool longNameContainsNonAlpha = std::find_if(longName.begin(), longName.end(), [](char c) { return !std::isalpha(c); }) != longName.end();
                LOG_THROW(!longNameContainsNonAlpha, storm::exceptions::IllegalArgumentException, "Unable to construct option with illegal name.");
                
                bool shortNameContainsNonAlpha = std::find_if(shortName.begin(), shortName.end(), [](char c) { return !std::isalpha(c); }) != shortName.end();
                LOG_THROW(!shortNameContainsNonAlpha, storm::exceptions::IllegalArgumentException, "Unable to construct option with illegal name.");

                // Then index all arguments.
                for (auto const& argument : arguments) {
                    argumentNameMap.emplace(argument->getName(), argument);
                }
            }
            
            /*!
             * Sets the flag that marks the option as being (un)set.
             *
             * @param newValue The new status of the flag.
             */
            void setHasOptionBeenSet(bool newValue = true) {
                this->hasBeenSet = newValue;
            }
		};
	}
}

#endif // STORM_SETTINGS_OPTION_H_