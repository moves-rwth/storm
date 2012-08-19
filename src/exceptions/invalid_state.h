#ifndef MRMC_EXCEPTIONS_INVALID_STATE_H_
#define MRMC_EXCEPTIONS_INVALID_STATE_H_

#include <exception>

namespace mrmc {

namespace exceptions {

//!This exception is thrown when a memory request can't be
//!fulfilled.
class invalid_state : public std::exception
{
 public:
/* The Visual C++-Version of the exception class has constructors accepting
 * a char*-constant; The GCC version does not have these
 *
 * As the "extended" constructor is used in the sparse matrix code, a dummy
 * constructor is used under linux (which will ignore the parameter)
 */
#ifdef _WIN32
   invalid_state() : exception("::mrmc::invalid_state"){}
   invalid_state(const char * const s): exception(s) {}
#else
   invalid_state() : exception() {}
   invalid_state(const char * const s): exception() {}

#endif
   virtual const char* what() const throw()
      {  return "mrmc::invalid_state";  }
};

} // namespace exceptions

} // namespace mrmc

#endif // MRMC_EXCEPTIONS_INVALID_STATE_H_
