#ifndef MRMC_EXCEPTIONS_OUT_OF_RANGE_H_
#define MRMC_EXCEPTIONS_OUT_OF_RANGE_H_

#include <exception>

namespace mrmc {

namespace exceptions {

//!This exception is thrown when a parameter is not in the range of valid values
class out_of_range : public std::exception
{
 public:
   out_of_range() : exception("::mrmc::out_of_range"){}
   out_of_range(const char * const s): exception(s) {}
   virtual const char* what() const throw()
      {  return "mrmc::out_of_range";  }
};

} // namespace exceptions

} // namespace mrmc
#endif // MRMC_EXCEPTIONS_OUT_OF_RANGE_H_