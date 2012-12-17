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

#endif // MRMC_EXCEPTIONS_BASEEXCEPTION_H_
