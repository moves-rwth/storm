/*
 * file_IO_exception.h
 *
 *  Created on: 16.08.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_EXCEPTIONS_FILE_IO_EXCEPTION_H_
#define MRMC_EXCEPTIONS_FILE_IO_EXCEPTION_H_

namespace mrmc {

namespace exceptions {

class file_IO_exception : public std::exception {
   public:
#ifdef _WIN32
      file_IO_exception() : exception("::mrmc::file_IO_exception"){};
      file_IO_exception(const char * const s): exception(s) {};
#else
      file_IO_exception() {};
      file_IO_exception(const char * const s): exception() {};
#endif
      virtual const char* what() const throw(){
         {  return "mrmc::file_IO_exception";  }
      }
};

}

}

#endif /* MRMC_EXCEPTIONS_FILE_IO_EXCEPTION_H_ */
