#ifndef MRMC_EXCEPTIONS_INVALID_SETTINGS_H_
#define MRMC_EXCEPTIONS_INVALID_SETTINGS_H_

#include <exception>

namespace mrmc {
namespace exceptions {

//!This exception is thrown when a memory request can't be
//!fulfilled.
class InvalidSettings : public std::exception
{
 public:
/* The Visual C++-Version of the exception class has constructors accepting
 * a char*-constant; The GCC version does not have these
 *
 * As the "extended" constructor is used in the sparse matrix code, a dummy
 * constructor is used under linux (which will ignore the parameter)
 */
#ifdef _WIN32
   InvalidSettings() : exception("::mrmc::InvalidSettings"){}
   InvalidSettings(const char * const s): exception(s) {}
#else
   InvalidSettings() : exception() {}
   InvalidSettings(const char * const s): exception() {}

#endif
   virtual const char* what() const throw()
      {  return "mrmc::InvalidSettings";  }
};

} // namespace exceptions
} // namespace mrmc

#endif // MRMC_EXCEPTIONS_INVALID_SETTINGS_H_
