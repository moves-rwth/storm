/*
 * WrongFileFormatException.h
 *
 *  Created on: 16.08.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_EXCEPTIONS_WRONGFILEFORMAT_H_
#define MRMC_EXCEPTIONS_WRONGFILEFORMAT_H_

#include <exception>

namespace mrmc {

namespace exceptions {

class WrongFileFormatException : public std::exception {
   public:
#ifdef _WIN32
      WrongFileFormatException() : exception("::mrmc::WrongFileFormatException"){};
      WrongFileFormatException(const char * const s): exception(s) {};
#else
      WrongFileFormatException() {};
      WrongFileFormatException(const char * const s): exception() {};
#endif
      virtual const char* what() const throw(){
         {  return "mrmc::WrongFileFormatException";  }
      }
};

} //namespace exceptions

} //namespace mrmc

#endif /* MRMC_EXCEPTIONS_WRONGFILEFORMAT_H_ */
