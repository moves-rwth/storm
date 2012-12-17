#ifndef MRMC_EXCEPTIONS_NOCONVERGENCEEXCEPTION_H_
#define MRMC_EXCEPTIONS_NOCONVERGENCEEXCEPTION_H_

#include <exception>

namespace mrmc {
namespace exceptions {

//!This exception is thrown when an iterative solver failed to converge with the given maxIterations
class NoConvergenceException : public std::exception
{
 public:
/* The Visual C++-Version of the exception class has constructors accepting
 * a char*-constant; The GCC version does not have these
 *
 * As the "extended" constructor is used in the sparse matrix code, a dummy
 * constructor is used under linux (which will ignore the parameter)
 */
#ifdef _WIN32
   NoConvergenceException() : exception("::mrmc::exceptions::NoConvergenceException"){
	   iterations = -1;
	   maxIterations = -1;
   }
   NoConvergenceException(const char * const s, int iterations, int maxIterations): exception(s) {
	   this->iterations = iterations;
	   this->maxIterations = maxIterations;
   }
#else
   NoConvergenceException() : exception() {
	   iterations = -1;
	   maxIterations = -1;
   }
   NoConvergenceException(const char * const s, int iterations, int maxIterations): exception() {
	   this->iterations = iterations;
	   this->maxIterations = maxIterations;
   }

#endif
   virtual const char* what() const throw()
      {  return "mrmc::exceptions::NoConvergenceException";  }

   int getIterationCount() const {
	   return iterations;
   }
   int getMaxIterationCount() const {
	   return maxIterations;
   }
 private:
	 int iterations;
	 int maxIterations;
};

} // namespace exceptions
} // namespace mrmc

#endif // MRMC_EXCEPTIONS_NOCONVERGENCEEXCEPTION_H_
