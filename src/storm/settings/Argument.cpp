
#include "Argument.h"

#include "src/storm/exceptions/IllegalArgumentException.h"
#include "src/storm/exceptions/IllegalArgumentValueException.h"
#include "src/storm/exceptions/IllegalFunctionCallException.h"
#include "src/storm/settings/ArgumentTypeInferationHelper.h"
#include "src/storm/utility/macros.h"


namespace storm {
	namespace settings {
      
        template<typename T>
			Argument<T>::Argument(std::string const& name, std::string const& description, std::vector<userValidationFunction_t> const& validationFunctions): ArgumentBase(name, description), argumentValue(), argumentType(inferToEnumType<T>()), validationFunctions(validationFunctions), isOptional(false), defaultValue(), hasDefaultValue(false) {
                // Intentionally left empty.
			}
            
           template<typename T>
            Argument<T>::Argument(std::string const& name, std::string const& description, std::vector<userValidationFunction_t> const& validationFunctions, bool isOptional, T defaultValue): ArgumentBase(name, description), argumentValue(), argumentType(inferToEnumType<T>()), validationFunctions(validationFunctions), isOptional(isOptional), defaultValue(), hasDefaultValue(true) {
                this->setDefaultValue(defaultValue);
            }
            
           template<typename T>
		    bool Argument<T>::getIsOptional() const  {
                return this->isOptional;
            }
            
			template<typename T>
            bool Argument<T>::setFromStringValue(std::string const& fromStringValue)  {
				bool conversionOk = false;
				T newValue = ArgumentBase::convertFromString<T>(fromStringValue, conversionOk);
				if (!conversionOk) {
                    return false;
				}
				return this->setFromTypeValue(newValue);
			}
            
			template<typename T>
            bool Argument<T>::setFromTypeValue(T const& newValue, bool hasBeenSet) {
				if (!this->validate(newValue)) {
                    return false;
				}
				this->argumentValue = newValue;
				this->hasBeenSet = hasBeenSet;
				return true;
			}
            
			template<typename T>
            ArgumentType Argument<T>::getType() const  {
				return this->argumentType;
			}
            
          
			        
			template<typename T>
            T const& Argument<T>::getArgumentValue() const {
                STORM_LOG_THROW(this->getHasBeenSet() || this->getHasDefaultValue(), storm::exceptions::IllegalFunctionCallException, "Unable to retrieve value of argument '" << this->getName() << "', because it was neither set nor specifies a default value.");
                if (this->getHasBeenSet()) {
                    return this->argumentValue;
                } else {
                    return this->defaultValue;
                }
			}
            
			template<typename T>
            bool Argument<T>::getHasDefaultValue() const  {
				return this->hasDefaultValue;
			}
            
			template<typename T>
            void Argument<T>::setFromDefaultValue()  {
                STORM_LOG_THROW(this->hasDefaultValue, storm::exceptions::IllegalFunctionCallException, "Unable to set value from default value, because the argument has none.");
				bool result = this->setFromTypeValue(this->defaultValue, false);
                STORM_LOG_THROW(result, storm::exceptions::IllegalArgumentValueException, "Unable to assign default value to argument, because it was rejected.");
			}
            
			template<typename T>
            std::string Argument<T>::getValueAsString() const  {
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
			
			template<typename T>
            int_fast64_t Argument<T>::getValueAsInteger() const  {
				switch (this->argumentType) {
					case ArgumentType::Integer:
						return inferToInteger(ArgumentType::Integer, this->getArgumentValue());
					default: STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "Unable to retrieve argument value as integer."); break;
                }
            }
            
            
            template<typename T>
            uint_fast64_t Argument<T>::getValueAsUnsignedInteger() const  {
                switch (this->argumentType) {
                    case ArgumentType::UnsignedInteger:
                        return inferToUnsignedInteger(ArgumentType::UnsignedInteger, this->getArgumentValue());
                    default: STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "Unable to retrieve argument value as unsigned integer."); break;
                }
            }
            
            
            template<typename T>
            double Argument<T>::getValueAsDouble() const  {
                switch (this->argumentType) {
                    case ArgumentType::Double:
                        return inferToDouble(ArgumentType::Double, this->getArgumentValue());
                    default: STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "Unable to retrieve argument value as double."); break;
                }
            }
            
            template<typename T>
            bool Argument<T>::getValueAsBoolean() const  {
                switch (this->argumentType) {
                    case ArgumentType::Boolean:
                        return inferToBoolean(ArgumentType::Boolean, this->getArgumentValue());
                    default: STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "Unable to retrieve argument value as boolean."); break;
                }
            }
    
           
            template<typename T>
            void Argument<T>::setDefaultValue(T const& newDefault) {
                STORM_LOG_THROW(this->validate(newDefault), storm::exceptions::IllegalArgumentValueException, "The default value for the argument did not pass all validation functions.");
                this->defaultValue = newDefault;
                this->hasDefaultValue = true;
            }
                        
            
            template<typename T>
            bool Argument<T>::validate(T const& value) const {
                bool result = true;
                for (auto const& lambda : validationFunctions) {
                    result = result && lambda(value);
                }
                return result;
            }
            
            template class Argument<std::string>;
            template class Argument<int_fast64_t>;
            template class Argument<uint_fast64_t>;
            template class Argument<double>;
            template class Argument<bool>;
    }
}
        
