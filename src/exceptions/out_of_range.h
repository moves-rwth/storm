#ifndef MRMC_EXCEPTIONS_OUT_OF_RANGE_H_
#define MRMC_EXCEPTIONS_OUT_OF_RANGE_H_

#include <exception>

namespace mrmc {

namespace exceptions {

//!This exception is thrown when a parameter is not in the range of valid values
class out_of_range : public std::exception
{
 public:
/* The Visual C++-Version of the exception class has constructors accepting
 * a char*-constant; The GCC version does not have these
 *
 * As the "extended" constructor is used in the sparse matrix code, a dummy
 * constructor is used under linux (which will ignore the parameter)
 */
#ifdef _WIN32
   out_of_range() : exception("::mrmc::out_of_range"){}
   out_of_range(const char * const s): exception(s) {}
#else
   out_of_range() : exception() {}
   out_of_range(const char * const s): exception() {}
#endif
   virtual const char* what() const throw()
      {  return "mrmc::out_of_range";  }
};

} // namespace exceptions

} // namespace mrmc
#endif // MRMC_EXCEPTIONS_OUT_OF_RANGE_H_
