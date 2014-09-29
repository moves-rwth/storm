#ifndef STORM_SETTINGS_ARGUMENT_H_
#define STORM_SETTINGS_ARGUMENT_H_

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <utility>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>

#include "src/settings/ArgumentBase.h"
#include "src/settings/ArgumentType.h"
#include "src/settings/ArgumentTypeInferationHelper.h"
#include "src/utility/macros.h"
#include "src/exceptions/ArgumentUnificationException.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/IllegalArgumentValueException.h"
#include "src/exceptions/IllegalFunctionCallException.h"

namespace storm {
	namespace settings {
        
        /*!
         * This class subclasses the argument base to actually implement the pure virtual functions. This construction
         * is necessary so that it becomes easy to store a vector of arguments later despite variing template types, by
         * keeping a vector of pointers to the base class.
         */
		template<typename T>
		class Argument : public ArgumentBase {
		public:
            // Introduce shortcuts for validation functions.
			typedef std::function<bool (T const&)> userValidationFunction_t;
            
            /*!
             * Creates a new argument with the given parameters.
             *
             * @param name The name of the argument.
             * @param description The description of the argument.
             * @param validationFunctions A vector of validation functions that are to be executed upon assigning a value
             * to this argument.
             * @param isOptional A flag indicating whether the argument is optional.
             */
			Argument(std::string const& name, std::string const& description, std::vector<userValidationFunction_t> const& validationFunctions): ArgumentBase(name, description), argumentValue(), argumentType(inferToEnumType<T>()), validationFunctions(validationFunctions), isOptional(false), defaultValue(), hasDefaultValue(false) {
                // Intentionally left empty.
			}
            
            /*!
             * Creates a new argument with the given parameters.
             *
             * @param name The name of the argument.
             * @param description The description of the argument.
             * @param validationFunctions A vector of validation functions that are to be executed upon assigning a value
             * to this argument.
             * @param isOptional A flag indicating whether the argument is optional.
             */
			Argument(std::string const& name, std::string const& description, std::vector<userValidationFunction_t> const& validationFunctions, bool isOptional, T defaultValue): ArgumentBase(name, description), argumentValue(), argumentType(inferToEnumType<T>()), validationFunctions(validationFunctions), isOptional(isOptional), defaultValue(), hasDefaultValue(true) {
                this->setDefaultValue(defaultValue);
            }
            
            virtual bool getIsOptional() const override {
                return this->isOptional;
            }
            
			bool setFromStringValue(std::string const& fromStringValue) override {
				bool conversionOk = false;
				T newValue = ArgumentBase::convertFromString<T>(fromStringValue, conversionOk);
				if (!conversionOk) {
                    return false;
				}
				return this->setFromTypeValue(newValue);
			}
            
			bool setFromTypeValue(T const& newValue) {
				if (!this->validate(newValue)) {
                    return false;
				}
				this->argumentValue = newValue;
				this->hasBeenSet = true;
				return true;
			}
            
			virtual ArgumentType getType() const override {
				return this->argumentType;
			}
            
            /*!
             * Checks whether the given argument is compatible with the current one. If not, an exception is thrown.
             *
             * @param other The other argument with which to check compatibility.
             * @return True iff the given argument is compatible with the current one.
             */
			template <typename S>
			bool isCompatibleWith(Argument<S> const& other) const {
                STORM_LOG_THROW(this->getType() == other.getType(), storm::exceptions::ArgumentUnificationException, "Unable to unify the arguments " << this->getName() << " and " << other.getName() << ", because they have different types.");
                STORM_LOG_THROW(this->getIsOptional() == other.getIsOptional(), storm::exceptions::ArgumentUnificationException, "Unable to unify the arguments '" << this->getName() << "' and '" << other.getName() << "', because one of them is optional and the other one is not.");
                STORM_LOG_THROW(this->getHasDefaultValue() == other.getHasDefaultValue(), storm::exceptions::ArgumentUnificationException, "Unable to unify the arguments " << this->getName() << " and " << other.getName() << ", because one of them has a default value and the other one does not.");
                return true;
			}
            
            /*!
             * Retrieves the value of the argument if any has been set. Otherwise, an exception is thrown.
             *
             * @return The value of the argument.
             */
			T const& getArgumentValue() const {
                STORM_LOG_THROW(this->getHasBeenSet() || this->getHasDefaultValue(), storm::exceptions::IllegalFunctionCallException, "Unable to retrieve value of argument, because it was neither set nor specifies a default value.");
                if (this->getHasBeenSet()) {
                    return this->argumentValue;
                } else {
                    return this->defaultValue;
                }
			}
            
			virtual bool getHasDefaultValue() const override {
				return this->hasDefaultValue;
			}
            
			void setFromDefaultValue() override {
                STORM_LOG_THROW(this->hasDefaultValue, storm::exceptions::IllegalFunctionCallException, "Unable to set value from default value, because the argument has none.");
				bool result = this->setFromTypeValue(this->defaultValue);
                STORM_LOG_THROW(result, storm::exceptions::IllegalArgumentValueException, "Unable to assign default value to argument, because it was rejected.");
			}
            
			virtual std::string getValueAsString() const override {
				switch (this->argumentType) {
					case ArgumentType::String:
						return inferToString(ArgumentType::String, this->getArgumentValue());
					case ArgumentType::Boolean: {
						bool iValue = inferToBoolean(ArgumentType::Boolean, this->getArgumentValue());
						if (iValue) {
							return "true";
						} else {
							return "false";
						}
					}
					default: return ArgumentBase::convertToString(this->argumentValue);
				}
			}
			
			virtual int_fast64_t getValueAsInteger() const override {
				switch (this->argumentType) {
					case ArgumentType::Integer:
						return inferToInteger(ArgumentType::Integer, this->getArgumentValue());
					default: STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "Unable to retrieve argument value as integer."); break;
                }
            }
            
            
            virtual uint_fast64_t getValueAsUnsignedInteger() const override {
                switch (this->argumentType) {
                    case ArgumentType::UnsignedInteger:
                        return inferToUnsignedInteger(ArgumentType::UnsignedInteger, this->getArgumentValue());
                    default: STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "Unable to retrieve argument value as unsigned integer."); break;
                }
            }
            
            
            virtual double getValueAsDouble() const override {
                switch (this->argumentType) {
                    case ArgumentType::Double:
                        return inferToDouble(ArgumentType::Double, this->getArgumentValue());
                    default: STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "Unable to retrieve argument value as double."); break;
                }
            }
            
            
            virtual bool getValueAsBoolean() const override {
                switch (this->argumentType) {
                    case ArgumentType::Boolean:
                        return inferToBoolean(ArgumentType::Boolean, this->getArgumentValue());
                    default: STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "Unable to retrieve argument value as boolean."); break;
                }
            }
            
        private:
            // The value of the argument (in case it has been set).
            T argumentValue;
            
            // The type of the argument.
            ArgumentType argumentType;
            
            // The validation functions that were registered for this argument.
            std::vector<userValidationFunction_t> validationFunctions;
            
            // A flag indicating whether this argument is optional.
            bool isOptional;
            
            // The default value for the argument (in case one has been provided).
            T defaultValue;
            
            // A flag indicating whether a default value has been provided.
            bool hasDefaultValue;
            
            /*!
             * Sets the default value of the argument to the provided value.
             *
             * @param newDefault The new default value of the argument.
             */
            void setDefaultValue(T const& newDefault) {
                STORM_LOG_THROW(this->validate(newDefault), storm::exceptions::IllegalArgumentValueException, "The default value for the argument did not pass all validation functions.");
                this->defaultValue = newDefault;
                this->hasDefaultValue = true;
            }
                        
            /*!
             * Applies all validation functions to the given value and therefore checks the validity of a value for this
             * argument.
             *
             * @param value The value that is to be validated.
             * @return True iff the value passed all validation functions successfully.
             */
            bool validate(T const& value) const {
                bool result = true;
                for (auto const& lambda : validationFunctions) {
                    result = result && lambda(value);
                }
                return result;
            }
            
        };
    }
}

#endif // STORM_SETTINGS_ARGUMENT_H_