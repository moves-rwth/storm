/*
 * wrong_file_format.h
 *
 *  Created on: 16.08.2012
 *      Author: Thomas Heinemann
 */

#ifndef WRONG_FILE_FORMAT_H_
#define WRONG_FILE_FORMAT_H_

#include <exception>

namespace mrmc {

namespace exceptions {

class wrong_file_format : public std::exception {
   public:
#ifdef _WIN32
      wrong_file_format() : exception("::mrmc::wrong_file_format"){};
      wrong_file_format(const char * const s): exception(s) {};
#else
      wrong_file_format() {};
      wrong_file_format(const char * const s): exception() {};
#endif
      virtual const char* what() const throw(){
         {  return "mrmc::wrong_file_format";  }
      }
};

} //namespace exceptions

} //namespace mrmc

#endif /* WRONG_FILE_FORMAT_H_ */
