#ifndef STORM_SETTINGS_ARGUMENTBASE_H_
#define STORM_SETTINGS_ARGUMENTBASE_H_

#include <iostream>
#include <string>

#include "ArgumentType.h"
#include "src/utility/StringHelper.h"

namespace storm {
	namespace settings {

		typedef std::pair<bool, std::string> assignmentResult_t;

		class ArgumentBase {
		public:
			ArgumentBase(std::string const& argumentName, std::string const& argumentDescription, bool isOptional) : isOptional(isOptional), hasBeenSet(false), argumentName(argumentName), argumentDescription(argumentDescription) {}
			virtual ~ArgumentBase() {
				std::cout << "Destructing an ArgumentBase." << std::endl;
			}
			virtual ArgumentType getArgumentType() const = 0;

			virtual bool getIsOptional() const {
				return this->isOptional;
			}

			std::string const& getArgumentName() const {
				return this->argumentName;
			}

			std::string const& getArgumentDescription() const {
				return this->argumentDescription;
			}

			virtual bool getHasDefaultValue() const = 0;
			virtual bool getHasBeenSet() const {
				return this->hasBeenSet;
			}

			virtual void setFromDefaultValue() = 0;
			virtual assignmentResult_t fromStringValue(std::string const& fromStringValue) = 0;
			virtual ArgumentBase* clone() const = 0;

			virtual std::string getValueAsString() const = 0;
			virtual int_fast64_t getValueAsInteger() const = 0;
			virtual uint_fast64_t getValueAsUnsignedInteger() const = 0;
			virtual double getValueAsDouble() const = 0;
			virtual bool getValueAsBoolean() const = 0;
		protected:
			bool isOptional;
			bool hasBeenSet;

			std::string argumentName;
			std::string argumentDescription;

			class ArgumentHelper {
			public:
				template <typename S>
				static S convertFromString(std::string const& s, bool* ok = nullptr);
			private:
				ArgumentHelper() {}
				ArgumentHelper(ArgumentHelper& other) {}
				~ArgumentHelper() {}
			};
		};

		template <typename S> S ArgumentBase::ArgumentHelper::convertFromString(std::string const& s, bool* ok) {
			std::istringstream stream(s);
			S t;
			if (ok != nullptr) {
				*ok = (stream >> t) && (stream >> std::ws).eof();
			} else {
				stream >> t;
			}
			return t;
		}

		template <> inline bool ArgumentBase::ArgumentHelper::convertFromString<bool>(std::string const& s, bool* ok) {
			static const std::string lowerTrueString = "true";
			static const std::string lowerFalseString = "false";
			static const std::string lowerYesString = "yes";
			static const std::string lowerNoString = "no";

			std::string lowerInput = storm::utility::StringHelper::stringToLower(s);

			if (s.compare(lowerTrueString) == 0 || s.compare(lowerYesString) == 0) {
				if (ok != nullptr) {
					*ok = true;
				}
				return true;
			} else if (s.compare(lowerFalseString) == 0 || s.compare(lowerNoString) == 0) {
				if (ok != nullptr) {
					*ok = true;
				}
				return false;
			}

			std::istringstream stream(s);
			bool t;
			if (ok != nullptr) {
				*ok = (stream >> t) && (stream >> std::ws).eof();
			} else {
				stream >> t;
			}
			return t;
		}
	}
}

#endif // STORM_SETTINGS_ARGUMENTBASE_H_