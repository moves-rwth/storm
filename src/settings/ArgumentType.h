#ifndef STORM_SETTINGS_ARGUMENTTYPE_H_
#define STORM_SETTINGS_ARGUMENTTYPE_H_

#include "src/exceptions/InternalTypeErrorException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
	namespace settings {
		enum class ArgumentType {
			Invalid, String, Integer, UnsignedInteger, Double, Boolean
		};

		class ArgumentTypeHelper {
		public:
			static std::string const& toString(ArgumentType argumentType) {
				static std::string argumentTypeInvalid			= "Invalid";
				static std::string argumentTypeString			= "String";
				static std::string argumentTypeInteger			= "Integer";
				static std::string argumentTypeUnsignedInteger	= "UnsignedInteger";
				static std::string argumentTypeDouble			= "Double";
				static std::string argumentTypeBoolean			= "Boolean";

				switch (argumentType) {
				case ArgumentType::Invalid:
					return argumentTypeInvalid;
					break;
				case ArgumentType::String:
					return argumentTypeString;
					break;
				case ArgumentType::Integer:
					return argumentTypeInteger;
					break;
				case ArgumentType::UnsignedInteger:
					return argumentTypeUnsignedInteger;
					break;
				case ArgumentType::Double:
					return argumentTypeDouble;
					break;
				case ArgumentType::Boolean:
					return argumentTypeBoolean;
					break;
				default: {
					LOG4CPLUS_ERROR(logger, "ArgumentTypeHelper::toString: Missing case in ArgumentTypeHelper.");
					throw storm::exceptions::InternalTypeErrorException() << "Missing case in ArgumentTypeHelper.";
					}
				}
			}
		private:
			ArgumentTypeHelper() {}
			~ArgumentTypeHelper() {}
		};
	}
}

#endif // STORM_SETTINGS_ARGUMENTTYPE_H_