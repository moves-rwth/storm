/*
 * FileIoException.h
 *
 *  Created on: 16.08.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_EXCEPTIONS_FILEIOEXCEPTION_H_
#define MRMC_EXCEPTIONS_FILEIOEXCEPTION_H_

namespace mrmc {

namespace exceptions {

class FileIoException : public std::exception {
   public:
#ifdef _WIN32
      FileIoException() : exception("::mrmc::FileIoException"){};
      FileIoException(const char * const s): exception(s) {};
#else
      FileIoException() {};
      FileIoException(const char * const s): exception() {};
#endif
      virtual const char* what() const throw(){
         {  return "mrmc::FileIoException";  }
      }
};

}

}

#endif /* MRMC_EXCEPTIONS_FILEIOEXCEPTION_H_ */
