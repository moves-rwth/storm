#ifndef STORM_SETTINGS_ARGUMENTTYPE_H_
#define STORM_SETTINGS_ARGUMENTTYPE_H_

#include <iostream>

#include "src/exceptions/ExceptionMacros.h"

namespace storm {
	namespace settings {
        
        /*!
         * This enum captures all possible types for arguments.
         */
		enum class ArgumentType {
			String, Integer, UnsignedInteger, Double, Boolean
		};

        std::ostream& operator<<(std::ostream& out, ArgumentType& argumentType) {
            switch (argumentType) {
                case ArgumentType::String: out << "string"; break;
                case ArgumentType::Integer: out << "integer"; break;
                case ArgumentType::UnsignedInteger: out << "unsigned integer"; break;
                case ArgumentType::Double: out << "double"; break;
                case ArgumentType::Boolean: out << "boolean"; break;
            }
            
            return out;
        }
        
	} // namespace settings
} // namespace storm

#endif // STORM_SETTINGS_ARGUMENTTYPE_H_