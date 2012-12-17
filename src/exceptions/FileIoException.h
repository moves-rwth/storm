/*
 * FileIoException.h
 *
 *  Created on: 16.08.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_EXCEPTIONS_FILEIOEXCEPTION_H_
#define MRMC_EXCEPTIONS_FILEIOEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace mrmc {

namespace exceptions {

class FileIoException : public BaseException<FileIoException> {
public:
	FileIoException() : BaseException() {
	}
	FileIoException(const char* cstr) : BaseException(cstr) {
	}
	FileIoException(const FileIoException& cp) : BaseException(cp) {
	}
};

}

}

#endif /* MRMC_EXCEPTIONS_FILEIOEXCEPTION_H_ */
