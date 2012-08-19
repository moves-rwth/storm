#ifndef MRMC_EXCEPTIONS_INVALID_ARGUMENT_H_
#define MRMC_EXCEPTIONS_INVALID_ARGUMENT_H_

#include <exception>

namespace mrmc {

namespace exceptions {

//!This exception is thrown when a parameter is invalid in this context
class invalid_argument : public std::exception
{
 public:
/* The Visual C++-Version of the exception class has constructors accepting
 * a char*-constant; The GCC version does not have these
 *
 * As the "extended" constructor is used in the sparse matrix code, a dummy
 * constructor is used under linux (which will ignore the parameter)
 */
#ifdef _WIN32
   invalid_argument() : exception("::mrmc::invalid_argument"){}
   invalid_argument(const char * const s): exception(s) {}
#else
   invalid_argument() : exception() {}
   invalid_argument(const char * const s): exception() {}
#endif
   virtual const char* what() const throw()
      {  return "mrmc::invalid_argument";  }
};

} // namespace exceptions

} // namespace mrmc
#endif // MRMC_EXCEPTIONS_INVALID_ARGUMENT_H_
