#ifndef MRMC_EXCEPTIONS_BASEEXCEPTION_H_
#define MRMC_EXCEPTIONS_BASEEXCEPTION_H_

#include <exception>
#include <sstream>

namespace mrmc {
namespace exceptions {

template<typename E>
class BaseException : public std::exception
{
	public:
		BaseException() : exception() {}
		BaseException(const BaseException& cp)
			: exception(cp), stream(cp.stream.str())
		{
		}

		BaseException(const char* cstr) {
			stream << cstr;
		}
		
		~BaseException() throw() { }
		
		template<class T>
		E& operator<<(const T& var)
		{
			this->stream << var;
			return * dynamic_cast<E*>(this);
		}
		
		virtual const char* what() const throw()
		{
			return this->stream.str().c_str();
		}
	
	private:
		std::stringstream stream;
};

} // namespace exceptions
} // namespace mrmc

/* Macro to generate descendant exception classes.
 * As all classes are nearly the same, this makes changing common features much easier.
 */
#define MRMC_EXCEPTION_DEFINE_NEW(exception_name) class exception_name : public BaseException<exception_name> { \
public: \
	exception_name() : BaseException() { \
	} \
	exception_name(const char* cstr) : BaseException(cstr) { \
	} \
	exception_name(const exception_name& cp) : BaseException(cp) { \
	} \
};


#endif // MRMC_EXCEPTIONS_BASEEXCEPTION_H_
