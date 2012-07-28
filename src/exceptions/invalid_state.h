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
   invalid_state() : exception("::mrmc::invalid_state"){}
   invalid_state(const char * const s): exception(s) {}
   virtual const char* what() const throw()
      {  return "mrmc::invalid_state";  }
};

} // namespace exceptions

} // namespace mrmc

#endif // MRMC_EXCEPTIONS_INVALID_STATE_H_