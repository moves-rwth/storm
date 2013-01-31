#ifndef STORM_EXCEPTIONS_BASEEXCEPTION_H_
#define STORM_EXCEPTIONS_BASEEXCEPTION_H_

#include <exception>
#include <sstream>

namespace storm {
namespace exceptions {

template<typename E>
class BaseException : public std::exception {
	public:
		BaseException() : exception() {}
		BaseException(const BaseException& cp)
			: exception(cp), stream(cp.stream.str()) {
		}

		BaseException(const char* cstr) {
			stream << cstr;
		}
		
		~BaseException() throw() { }
		
		template<class T>
		E& operator<<(const T& var) {
			this->stream << var;
			return * dynamic_cast<E*>(this);
		}
		
		virtual const char* what() const throw() {
			std::string errorString = this->stream.str();
			char* result = new char[errorString.size() + 1];
			result[errorString.size()] = '\0';
			std::copy(errorString.begin(), errorString.end(), result);
			return result;
		}
	
	private:
		std::stringstream stream;
};

} // namespace exceptions
} // namespace storm

/* Macro to generate descendant exception classes.
 * As all classes are nearly the same, this makes changing common features much easier.
 */
#define STORM_EXCEPTION_DEFINE_NEW(exception_name) class exception_name : public BaseException<exception_name> { \
public: \
	exception_name() : BaseException() { \
	} \
	exception_name(const char* cstr) : BaseException(cstr) { \
	} \
	exception_name(const exception_name& cp) : BaseException(cp) { \
	} \
};


#endif // STORM_EXCEPTIONS_BASEEXCEPTION_H_
