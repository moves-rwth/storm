#ifndef BASEEXCEPTION_H_
#define BASEEXCEPTION_H_

#include <exception>
#include <sstream>

namespace mrmc {
namespace exceptions {

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
		BaseException& operator<<(const T& var)
		{
			this->stream << var;
			return *this;
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

#endif // BASEEXCEPTION_H_
