#include "storm/exceptions/BaseException.h"

namespace storm {
    namespace exceptions {
        BaseException::BaseException() : exception() {
            // Intentionally left empty.
        }
        
        BaseException::BaseException(BaseException const& other) : exception(other), stream(other.stream.str()) {
            // Intentionally left empty.
        }
        
        BaseException::BaseException(char const* cstr) {
            stream << cstr;
        }

		BaseException::~BaseException() {
			// Intentionally left empty.
		}
        
        const char* BaseException::what() const NOEXCEPT {
            errorString = this->stream.str();
            return errorString.c_str();
        }
    }
}
