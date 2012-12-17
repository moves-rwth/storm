#ifndef MRMC_EXCEPTIONS_INVALIDSETTINGSEXCEPTION_H_
#define MRMC_EXCEPTIONS_INVALIDSETTINGSEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace mrmc {
namespace exceptions {

class InvalidSettingsException : public BaseException<InvalidSettingsException> {
public:
	InvalidSettingsException() {
	}
	InvalidSettingsException(const char* cstr) : BaseException(cstr) {
	}
	InvalidSettingsException(const InvalidSettingsException& cp) : BaseException(cp) {
	}
};

} // namespace exceptions
} // namespace mrmc

#endif // MRMC_EXCEPTIONS_INVALIDSETTINGSEXCEPTION_H_
