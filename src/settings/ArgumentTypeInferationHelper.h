#ifndef STORM_SETTINGS_ARGUMENTTYPEINFERATIONHELPER_H_
#define STORM_SETTINGS_ARGUMENTTYPEINFERATIONHELPER_H_

#include <cstdint>
#include <string>

#include "src/settings/ArgumentType.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InternalTypeErrorException.h"

namespace storm {
	namespace settings {
        /*!
         * This class serves as a helper class to infer the types of arguments.
         */
		class ArgumentTypeInferation {
		public:
			// Specialized function template that infers the Type of T to our local enum
            /*!
             * This function infers the type in our enum of possible types from the template parameter.
             *
             * @return The argument type that has been inferred.
             */
			template <typename T>
			static ArgumentType inferToEnumType();

			// Specialized function templates that allow casting the given value to the correct type. If the conversion
            // fails, an exception is thrown.
			template <typename T> static std::string const& inferToString(ArgumentType const& argumentType, T const& value);
			template <typename T> static int_fast64_t inferToInteger(ArgumentType const& argumentType, T const& value);
			template <typename T> static uint_fast64_t inferToUnsignedInteger(ArgumentType const& argumentType, T const& value);
			template <typename T> static double inferToDouble(ArgumentType const& argumentType, T const& value);
			template <typename T> static bool inferToBoolean(ArgumentType const& argumentType, T const& value);
		};
	} // namespace settings
} // namespace storm

#endif // STORM_SETTINGS_ARGUMENTTYPEINFERATIONHELPER_H_