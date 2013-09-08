/*
 * ArgumentTypeInferationHelper.h
 *
 *  Created on: 19.07.2013
 *      Author: Philipp Berger
 *  Static Lookup Helper that detects whether the given Template Type is valid.
 */

#ifndef STORM_SETTINGS_ARGUMENTTYPEINFERATIONHELPER_H_
#define STORM_SETTINGS_ARGUMENTTYPEINFERATIONHELPER_H_

#include <cstdint>
#include <string>

#include "ArgumentType.h"
#include "src/exceptions/InternalTypeErrorException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
	namespace settings {
		class ArgumentTypeInferation {
		public:
			// Specialized function template that infers the Type of T to our local enum
			template <typename T>
			static ArgumentType inferToEnumType();

			// Specialized function templates that allow casting using the Enum Class as Target
			template <typename T> static std::string	inferToString(ArgumentType argumentType, T value);
			template <typename T> static int_fast64_t	inferToInteger(ArgumentType argumentType, T value);
			template <typename T> static uint_fast64_t	inferToUnsignedInteger(ArgumentType argumentType, T value);
			template <typename T> static double			inferToDouble(ArgumentType argumentType, T value);
			template <typename T> static bool			inferToBoolean(ArgumentType argumentType, T value);

		private:
			ArgumentTypeInferation();
			~ArgumentTypeInferation();
		};

		/*
		 *	All functions related to the EnumType Inferation from the Template Parameter
		 */
		template <typename T> 
		ArgumentType ArgumentTypeInferation::inferToEnumType() {
			// "Missing Template Specialization Case in ArgumentTypeInferation"
			LOG4CPLUS_ERROR(logger, "ArgumentTypeInferation::inferToEnumType: Missing a Template Specialization Case in the ArgumentTypeInferationHelper! It seems you tried to use a new, non-standard Type as a Settings Parameter-Type!");
			throw storm::exceptions::InternalTypeErrorException() << "Missing a Template Specialization Case in the ArgumentTypeInferationHelper!\n" << "It seems you tried to use a new, non-standard Type as a Settings Parameter-Type!";

			return ArgumentType::Invalid;
		}

		template <> inline ArgumentType ArgumentTypeInferation::inferToEnumType<std::string>() {
			return ArgumentType::String;
		}
		template <> inline ArgumentType ArgumentTypeInferation::inferToEnumType<int_fast64_t>() {
			return ArgumentType::Integer;
		}
		template <> inline ArgumentType ArgumentTypeInferation::inferToEnumType<uint_fast64_t>() {
			return ArgumentType::UnsignedInteger;
		}
		template <> inline ArgumentType ArgumentTypeInferation::inferToEnumType<double>() {
			return ArgumentType::Double;
		}
		template <> inline ArgumentType ArgumentTypeInferation::inferToEnumType<bool>() {
			return ArgumentType::Boolean;
		}
		
		/*
		 *	All functions related to the conversion to std::string based on the Template and Enum Type
		 */
		template <typename T> 
		std::string ArgumentTypeInferation::inferToString(ArgumentType argumentType, T value) {
			LOG4CPLUS_ERROR(logger, "ArgumentTypeInferation::inferToString: inferToString was called on a non-string Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!");
			throw storm::exceptions::InternalTypeErrorException() << "inferToString was called on a non-string Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!";

			return std::string();
		}

		template <> inline std::string ArgumentTypeInferation::inferToString<std::string>(ArgumentType argumentType, std::string value) {
			if (argumentType != ArgumentType::String) {
				LOG4CPLUS_ERROR(logger, "ArgumentTypeInferation::inferToString: inferToString was called on a string Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!");
				throw storm::exceptions::InternalTypeErrorException() << "inferToString was called on a string Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!";
			}
			return value;
		}

		/*
		 *	All functions related to the conversion to int_fast64_t based on the Template and Enum Type
		 */
		template <typename T> 
		int_fast64_t ArgumentTypeInferation::inferToInteger(ArgumentType argumentType, T value) {
			LOG4CPLUS_ERROR(logger, "ArgumentTypeInferation::inferToInteger: inferToInteger was called on a non-int_fast64_t Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!");
			throw storm::exceptions::InternalTypeErrorException() << "inferToInteger was called on a non-int_fast64_t Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!";

			return 0;
		}

		template <> inline int_fast64_t ArgumentTypeInferation::inferToInteger<int_fast64_t>(ArgumentType argumentType, int_fast64_t value) {
			if (argumentType != ArgumentType::Integer) {
				LOG4CPLUS_ERROR(logger, "ArgumentTypeInferation::inferToInteger: inferToInteger was called on a int_fast64_t Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!");
				throw storm::exceptions::InternalTypeErrorException() << "inferToInteger was called on an int_fast64_t Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!";
			}
			return value;
		}

		/*
		 *	All functions related to the conversion to uint_fast64_t based on the Template and Enum Type
		 */
		template <typename T> 
		uint_fast64_t ArgumentTypeInferation::inferToUnsignedInteger(ArgumentType argumentType, T value) {
			LOG4CPLUS_ERROR(logger, "ArgumentTypeInferation::inferToUnsignedInteger: inferToUnsignedInteger was called on a non-uint_fast64_t Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!");
			throw storm::exceptions::InternalTypeErrorException() << "inferToUnsignedInteger was called on a non-uint_fast64_t Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!";

			return 0;
		}

		template <> inline uint_fast64_t ArgumentTypeInferation::inferToUnsignedInteger<uint_fast64_t>(ArgumentType argumentType, uint_fast64_t value) {
			if (argumentType != ArgumentType::UnsignedInteger) {
				LOG4CPLUS_ERROR(logger, "ArgumentTypeInferation::inferToUnsignedInteger: inferToUnsignedInteger was called on a uint_fast64_t Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!");
				throw storm::exceptions::InternalTypeErrorException() << "inferToUnsignedInteger was called on an uint_fast64_t Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!";
			}
			return value;
		}

		/*
		 *	All functions related to the conversion to double based on the Template and Enum Type
		 */
		template <typename T> 
		double ArgumentTypeInferation::inferToDouble(ArgumentType argumentType, T value) {
			LOG4CPLUS_ERROR(logger, "ArgumentTypeInferation::inferToDouble: inferToDouble was called on a non-double Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!");
			throw storm::exceptions::InternalTypeErrorException() << "inferToDouble was called on a non-double Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!";

			return 0.0;
		}

		template <> inline double ArgumentTypeInferation::inferToDouble<double>(ArgumentType argumentType, double value) {
			if (argumentType != ArgumentType::Double) {
				LOG4CPLUS_ERROR(logger, "ArgumentTypeInferation::inferToDouble: inferToDouble was called on a double Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!");
				throw storm::exceptions::InternalTypeErrorException() << "inferToDouble was called on a double Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!";
			}
			return value;
		}

		/*
		 *	All functions related to the conversion to bool based on the Template and Enum Type
		 */
		template <typename T> 
		bool ArgumentTypeInferation::inferToBoolean(ArgumentType argumentType, T value) {
			LOG4CPLUS_ERROR(logger, "ArgumentTypeInferation::inferToBoolean: inferToBoolean was called on a non-bool Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!");
			throw storm::exceptions::InternalTypeErrorException() << "inferToBoolean was called on a non-bool Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!";

			return false;
		}

		template <> inline bool ArgumentTypeInferation::inferToBoolean<bool>(ArgumentType argumentType, bool value) {
			if (argumentType != ArgumentType::Boolean) {
				LOG4CPLUS_ERROR(logger, "ArgumentTypeInferation::inferToBoolean: inferToBoolean was called on a bool Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!");
				throw storm::exceptions::InternalTypeErrorException() << "inferToBoolean was called on a bool Template Object to cast to " << ArgumentTypeHelper::toString(argumentType) << "!";
			}
			return value;
		}
	}
}

#endif // STORM_SETTINGS_ARGUMENTTYPEINFERATIONHELPER_H_