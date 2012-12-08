#ifndef MRMC_EXCEPTIONS_NO_CONVERGENCE_H_
#define MRMC_EXCEPTIONS_NO_CONVERGENCE_H_

#include <exception>

namespace mrmc {
namespace exceptions {

//!This exception is thrown when an iterative solver failed to converge with the given maxIterations
class NoConvergence : public std::exception
{
 public:
/* The Visual C++-Version of the exception class has constructors accepting
 * a char*-constant; The GCC version does not have these
 *
 * As the "extended" constructor is used in the sparse matrix code, a dummy
 * constructor is used under linux (which will ignore the parameter)
 */
#ifdef _WIN32
   NoConvergence() : exception("::mrmc::NoConvergence"){
	   iterations = -1;
	   maxIterations = -1;
   }
   NoConvergence(const char * const s, int iterations, int maxIterations): exception(s) {
	   this->iterations = iterations;
	   this->maxIterations = maxIterations;
   }
#else
   NoConvergence() : exception() {
	   iterations = -1;
	   maxIterations = -1;
   }
   NoConvergence(const char * const s, int iterations, int maxIterations): exception() {
	   this->iterations = iterations;
	   this->maxIterations = maxIterations;
   }

#endif
   virtual const char* what() const throw()
      {  return "mrmc::NoConvergence";  }

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

#endif // MRMC_EXCEPTIONS_NO_CONVERGENCE_H_
