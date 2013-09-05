/*
 * OptionsAccumulator.h
 *
 *  Created on: 22.08.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_SETTINGS_OPTIONSACCUMULATOR_H_
#define STORM_SETTINGS_OPTIONSACCUMULATOR_H_

#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <memory>

#include "src/settings/Option.h"

namespace storm {
	namespace settings {
		class Settings;


		class OptionsAccumulator {
		public:
			OptionsAccumulator() {}
			~OptionsAccumulator() {
				//this->shortNames.clear();
				//this->options.clear();
			}

			OptionsAccumulator& addOption(Option* option);
			void join(OptionsAccumulator const& rhs);

			friend class storm::settings::Settings;
		private:
			/*!
			* The map holding the information regarding registered options and their types
			*/
			std::unordered_map<std::string, std::shared_ptr<Option>> options;

			/*!
			* The vector holding a pointer to all options
			*/
			std::vector<std::shared_ptr<Option>> optionPointers;

			/*!
			* The map holding the information regarding registered options and their short names
			*/
			std::unordered_map<std::string, std::string> shortNames;

			/*!
			* Returns true IFF this accumulator contains an option with the specified longName.
			*/
			bool containsLongName(std::string const& longName) {
				return (this->options.find(storm::utility::StringHelper::stringToLower(longName)) != this->options.end());
			}

			/*!
			* Returns true IFF this accumulator contains an option with the specified shortName.
			*/
			bool containsShortName(std::string const& shortName) {
				return (this->shortNames.find(storm::utility::StringHelper::stringToLower(shortName)) != this->shortNames.end());
			}

			/*!
			* Returns a reference to the Option with the specified longName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			*/
			Option& getByLongName(std::string const& longName) {
				auto longNameIterator = this->options.find(storm::utility::StringHelper::stringToLower(longName));
				if (longNameIterator == this->options.end()) {
					throw storm::exceptions::IllegalArgumentException() << "This Accumulator does not contain an Option named \"" << longName << "\"!";
				}
				return *longNameIterator->second.get();
			}

			/*!
			* Returns a pointer to the Option with the specified longName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			*/
			Option* getPtrByLongName(std::string const& longName) {
				auto longNameIterator = this->options.find(storm::utility::StringHelper::stringToLower(longName));
				if (longNameIterator == this->options.end()) {
					throw storm::exceptions::IllegalArgumentException() << "This Accumulator does not contain an Option named \"" << longName << "\"!";
				}
				return longNameIterator->second.get();
			}

			/*!
			* Returns a reference to the Option with the specified shortName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			*/
			Option& getByShortName(std::string const& shortName) {
				auto shortNameIterator = this->shortNames.find(storm::utility::StringHelper::stringToLower(shortName));
				if (shortNameIterator == this->shortNames.end()) {
					throw storm::exceptions::IllegalArgumentException() << "This Accumulator does not contain an Option with ShortName \"" << shortName << "\"!";
				}
				return *(this->options.find(shortNameIterator->second)->second.get());
			}

			/*!
			* Returns a pointer to the Option with the specified shortName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			*/
			Option* getPtrByShortName(std::string const& shortName) {
				auto shortNameIterator = this->shortNames.find(storm::utility::StringHelper::stringToLower(shortName));
				if (shortNameIterator == this->shortNames.end()) {
					throw storm::exceptions::IllegalArgumentException() << "This Accumulator does not contain an Option with ShortName \"" << shortName << "\"!";
				}
				return this->options.find(shortNameIterator->second)->second.get();
			}
		};
	}
}

#endif // STORM_SETTINGS_OPTIONSACCUMULATOR_H_