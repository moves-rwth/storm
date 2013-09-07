#ifndef STORM_SETTINGS_ARGUMENTBUILDER_H_
#define STORM_SETTINGS_ARGUMENTBUILDER_H_

#include <iostream>
#include <sstream>
#include <list>
#include <utility>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

#include "ArgumentType.h"
#include "ArgumentTypeInferationHelper.h"
#include "ArgumentBase.h"
#include "Argument.h"
#include "ArgumentValidators.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/exceptions/IllegalArgumentTypeException.h"

namespace storm {
	namespace settings {

		class ArgumentBuilder {
		public:
			~ArgumentBuilder() {}

			/*
				Preparation Functions for all ArgumentType's

				Resets all internal attributes
			*/
			static ArgumentBuilder createStringArgument(std::string const& argumentName, std::string const& argumentDescription) {
				ArgumentBuilder ab(ArgumentType::String, argumentName, argumentDescription);
				return ab;
			}

			static ArgumentBuilder createIntegerArgument(std::string const& argumentName, std::string const& argumentDescription) {
				ArgumentBuilder ab(ArgumentType::Integer, argumentName, argumentDescription);
				return ab;
			}

			static ArgumentBuilder createUnsignedIntegerArgument(std::string const& argumentName, std::string const& argumentDescription) {
				ArgumentBuilder ab(ArgumentType::UnsignedInteger, argumentName, argumentDescription);
				return ab;
			}

			static ArgumentBuilder createDoubleArgument(std::string const& argumentName, std::string const& argumentDescription) {
				ArgumentBuilder ab(ArgumentType::Double, argumentName, argumentDescription);
				return ab;
			}

			static ArgumentBuilder createBooleanArgument(std::string const& argumentName, std::string const& argumentDescription) {
				ArgumentBuilder ab(ArgumentType::Boolean, argumentName, argumentDescription);
				return ab;
			}

			ArgumentBuilder& setName(std::string const& newName) {
				this->argumentName = newName;
				return *this;
			}

			ArgumentBuilder& setDescription(std::string const& newDescription) {
				this->argumentDescription = newDescription;
				return *this;
			}

			ArgumentBuilder& setIsOptional(bool isOptional) {
				this->isOptional = isOptional;
				return *this;
			}
#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define MACROaddValidationFunction(funcName, funcType) 	ArgumentBuilder& PPCAT(addValidationFunction, funcName) (storm::settings::Argument< funcType >::userValidationFunction_t userValidationFunction) { \
				if (this->argumentType != ArgumentType::funcName) { \
					throw storm::exceptions::IllegalFunctionCallException() << "Error: You tried adding a Validation-Function for a \"" << ArgumentTypeHelper::toString(ArgumentType::funcName) << "\" Argument, but this Argument is configured to be of Type \"" << ArgumentTypeHelper::toString(this->argumentType) << "\"."; \
				} \
				( PPCAT(this->userValidationFunction_, funcName) ).push_back(userValidationFunction); \
				std::string errorMessageTarget = ""; \
				if (this->hasDefaultValue && !this->validateDefaultForEach(errorMessageTarget)) { \
					throw storm::exceptions::IllegalArgumentValueException() << "Error: You tried adding a Validation-Function for an Argument which has a Default Value set which is rejected by this Validation-Function:\r\n" << errorMessageTarget; \
				} \
				return *this; \
			}

			MACROaddValidationFunction(String, std::string)
			MACROaddValidationFunction(Integer, int_fast64_t)
			MACROaddValidationFunction(UnsignedInteger, uint_fast64_t)
			MACROaddValidationFunction(Double, double)
			MACROaddValidationFunction(Boolean, bool)


#define MACROsetDefaultValue(funcName, funcType) ArgumentBuilder& PPCAT(setDefaultValue, funcName) (funcType const& defaultValue) { \
				if (this->argumentType != ArgumentType::funcName) { \
					throw storm::exceptions::IllegalFunctionCallException() << "Error: You tried adding a default Value for a \"" << ArgumentTypeHelper::toString(ArgumentType::String) << "\" Argument, but this Argument is configured to be of Type \"" << ArgumentTypeHelper::toString(this->argumentType) << "\"."; \
				} \
				PPCAT(this->defaultValue_, funcName) = defaultValue; \
				std::string errorMessageTarget = ""; \
				if (!this->validateDefaultForEach(errorMessageTarget)) { \
					throw storm::exceptions::IllegalArgumentValueException() << "Error: You tried adding a default Value for an Argument, but a Validation Function rejected it:\r\n" << errorMessageTarget; \
				} \
				this->hasDefaultValue = true; \
				return *this; \
			}

			MACROsetDefaultValue(String, std::string)
			MACROsetDefaultValue(Integer, int_fast64_t)
			MACROsetDefaultValue(UnsignedInteger, uint_fast64_t)
			MACROsetDefaultValue(Double, double)
			MACROsetDefaultValue(Boolean, bool)

			ArgumentBase* build() {
				if (this->hasBeenBuild) {
					// LOG
					throw storm::exceptions::IllegalFunctionCallException() << "Error: Called build() on an instance of ArgumentBuilder which has already build an Instance.";
				}	
				this->hasBeenBuild = true;
				switch (this->argumentType) {
					case ArgumentType::String: {
						if (this->hasDefaultValue) {
							return dynamic_cast<ArgumentBase*>(new Argument<std::string>(this->argumentName, this->argumentDescription, userValidationFunction_String, this->isOptional, this->defaultValue_String));
						} else {
							return dynamic_cast<ArgumentBase*>(new Argument<std::string>(this->argumentName, this->argumentDescription, userValidationFunction_String, this->isOptional));
						}
						break;
											   }
					case ArgumentType::Integer:
						if (this->hasDefaultValue) {
							return dynamic_cast<ArgumentBase*>(new Argument<int_fast64_t>(this->argumentName, this->argumentDescription, userValidationFunction_Integer, this->isOptional, this->defaultValue_Integer));
						} else {
							return dynamic_cast<ArgumentBase*>(new Argument<int_fast64_t>(this->argumentName, this->argumentDescription, userValidationFunction_Integer, this->isOptional));
						}
						break;
					case ArgumentType::UnsignedInteger:
						if (this->hasDefaultValue) {
							return dynamic_cast<ArgumentBase*>(new Argument<uint_fast64_t>(this->argumentName, this->argumentDescription, userValidationFunction_UnsignedInteger, this->isOptional, this->defaultValue_UnsignedInteger));
						} else {
							return dynamic_cast<ArgumentBase*>(new Argument<uint_fast64_t>(this->argumentName, this->argumentDescription, userValidationFunction_UnsignedInteger, this->isOptional));
						}
						break;
					case ArgumentType::Double:
						if (this->hasDefaultValue) {
							return dynamic_cast<ArgumentBase*>(new Argument<double>(this->argumentName, this->argumentDescription, userValidationFunction_Double, this->isOptional, this->defaultValue_Double));
						} else {
							return dynamic_cast<ArgumentBase*>(new Argument<double>(this->argumentName, this->argumentDescription, userValidationFunction_Double, this->isOptional));
						}
						break;
					case ArgumentType::Boolean:
						if (this->hasDefaultValue) {
							return dynamic_cast<ArgumentBase*>(new Argument<bool>(this->argumentName, this->argumentDescription, userValidationFunction_Boolean, this->isOptional, this->defaultValue_Boolean));
						} else {
							return dynamic_cast<ArgumentBase*>(new Argument<bool>(this->argumentName, this->argumentDescription, userValidationFunction_Boolean, this->isOptional));
						}
						break;
					default:
						// LOG
						throw storm::exceptions::InternalTypeErrorException() << "Error: Missing Case in ArgumentBuilder's switch/case Code.";
				}
			}
		private:
			ArgumentBuilder(ArgumentType argumentType, std::string const& argumentName, std::string const& argumentDescription) : hasBeenBuild(false), argumentType(argumentType), argumentName(argumentName), argumentDescription(argumentDescription), isOptional(false), hasDefaultValue(false) {
				//
			}
			ArgumentBuilder(ArgumentBuilder& other) : hasBeenBuild(other.hasBeenBuild), argumentType(other.argumentType), argumentName(other.argumentName), argumentDescription(other.argumentDescription), isOptional(other.isOptional),
				defaultValue_String(other.defaultValue_String), defaultValue_Integer(other.defaultValue_Integer), defaultValue_UnsignedInteger(other.defaultValue_UnsignedInteger), defaultValue_Double(other.defaultValue_Double), defaultValue_Boolean(other.defaultValue_Boolean),
				hasDefaultValue(other.hasDefaultValue) {
				// Copy all userFunctions
				for (auto i = 0; i < userValidationFunction_String.size(); ++i) {
					this->userValidationFunction_String.push_back(storm::settings::Argument<std::string>::userValidationFunction_t(other.userValidationFunction_String.at(i)));
				}
				for (auto i = 0; i < userValidationFunction_Integer.size(); ++i) {
					this->userValidationFunction_Integer.push_back(storm::settings::Argument<int_fast64_t>::userValidationFunction_t(other.userValidationFunction_Integer.at(i)));
				}
				for (auto i = 0; i < userValidationFunction_UnsignedInteger.size(); ++i) {
					this->userValidationFunction_UnsignedInteger.push_back(storm::settings::Argument<uint_fast64_t>::userValidationFunction_t(other.userValidationFunction_UnsignedInteger.at(i)));
				}
				for (auto i = 0; i < userValidationFunction_Double.size(); ++i) {
					this->userValidationFunction_Double.push_back(storm::settings::Argument<double>::userValidationFunction_t(other.userValidationFunction_Double.at(i)));
				}
				for (auto i = 0; i < userValidationFunction_Boolean.size(); ++i) {
					this->userValidationFunction_Boolean.push_back(storm::settings::Argument<bool>::userValidationFunction_t(other.userValidationFunction_Boolean.at(i)));
				}
			}

			bool hasBeenBuild;

			ArgumentType argumentType;

			std::string argumentName;
			std::string argumentDescription;

			/*
				enum class ArgumentType {
					Invalid, String, Integer, UnsignedInteger, Double, Boolean
				};
			*/

			std::vector<storm::settings::Argument<std::string>::userValidationFunction_t> userValidationFunction_String;
			std::vector<storm::settings::Argument<int_fast64_t>::userValidationFunction_t> userValidationFunction_Integer;
			std::vector<storm::settings::Argument<uint_fast64_t>::userValidationFunction_t> userValidationFunction_UnsignedInteger;
			std::vector<storm::settings::Argument<double>::userValidationFunction_t> userValidationFunction_Double;
			std::vector<storm::settings::Argument<bool>::userValidationFunction_t> userValidationFunction_Boolean;

			bool isOptional;
			
			std::string defaultValue_String;
			int_fast64_t defaultValue_Integer;
			uint_fast64_t defaultValue_UnsignedInteger;
			double defaultValue_Double;
			bool defaultValue_Boolean;

			bool hasDefaultValue;

			bool validateDefaultForEach(std::string& errorMessageTarget) {
				bool result = true;

				switch (this->argumentType) {
					case ArgumentType::String:
						for (auto lambda: this->userValidationFunction_String) {
							result = result && lambda(this->defaultValue_String, errorMessageTarget);
						}
						break;
					case ArgumentType::Integer:
						for (auto lambda: this->userValidationFunction_Integer) {
							result = result && lambda(this->defaultValue_Integer, errorMessageTarget);
						}
						break;
					case ArgumentType::UnsignedInteger:
						for (auto lambda: this->userValidationFunction_UnsignedInteger) {
							result = result && lambda(this->defaultValue_UnsignedInteger, errorMessageTarget);
						}
						break;
					case ArgumentType::Double:
						for (auto lambda: this->userValidationFunction_Double) {
							result = result && lambda(this->defaultValue_Double, errorMessageTarget);
						}
						break;
					case ArgumentType::Boolean:
						for (auto lambda: this->userValidationFunction_Boolean) {
							result = result && lambda(this->defaultValue_Boolean, errorMessageTarget);
						}
						break;
					default:
						// LOG
						throw storm::exceptions::InternalTypeErrorException() << "Error: Missing Case in ArgumentBuilder's switch/case Code.";
				}
				
				return result;
			}
		};
	}
}

#endif // STORM_SETTINGS_ARGUMENTBUILDER_H_