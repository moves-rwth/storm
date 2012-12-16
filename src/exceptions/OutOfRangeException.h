#ifndef MRMC_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_
#define MRMC_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_

#include <exception>

namespace mrmc {

namespace exceptions {

//!This exception is thrown when a parameter is not in the range of valid values
class OutOfRangeException : public std::exception
{
 public:
/* The Visual C++-Version of the exception class has constructors accepting
 * a char*-constant; The GCC version does not have these
 *
 * As the "extended" constructor is used in the sparse matrix code, a dummy
 * constructor is used under linux (which will ignore the parameter)
 */
#ifdef _WIN32
   OutOfRangeException() : exception("::mrmc::OutOfRangeException"){}
   OutOfRangeException(const char * const s): exception(s) {}
#else
   OutOfRangeException() : exception() {}
   OutOfRangeException(const char * const s): exception() {}
#endif
   virtual const char* what() const throw()
      {  return "mrmc::OutOfRangeException";  }
};

} // namespace exceptions

} // namespace mrmc
#endif // MRMC_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_
