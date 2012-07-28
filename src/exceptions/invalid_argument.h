#ifndef MRMC_EXCEPTIONS_INVALID_ARGUMENT_H_
#define MRMC_EXCEPTIONS_INVALID_ARGUMENT_H_

#include <exception>

namespace mrmc {

namespace exceptions {

//!This exception is thrown when a parameter is invalid in this context
class invalid_argument : public std::exception
{
 public:
   invalid_argument() : exception("::mrmc::invalid_argument"){}
   invalid_argument(const char * const s): exception(s) {}
   virtual const char* what() const throw()
      {  return "mrmc::invalid_argument";  }
};

} // namespace exceptions

} // namespace mrmc
#endif // MRMC_EXCEPTIONS_INVALID_ARGUMENT_H_