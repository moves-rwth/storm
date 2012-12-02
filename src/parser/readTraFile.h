#ifndef READTRAFILE_H_
#define READTRAFILE_H_

#include "src/storage/SquareSparseMatrix.h"

namespace mrmc {
namespace parser {
	
/*!
 *	@brief	Load transition system from file and return initialized
 *	StaticSparseMatrix object.
 */
mrmc::storage::SquareSparseMatrix<double> * readTraFile(const char * filename);
		
} // namespace parser
} // namespace mrmc

#endif /* READTRAFILE_H_ */
