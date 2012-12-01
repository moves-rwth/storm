#ifndef READTRAFILE_H_
#define READTRAFILE_H_

#include "src/sparse/static_sparse_matrix.h"

namespace mrmc {
namespace parser {
	
/*!
 *	@brief	Load transition system from file and return initialized
 *	StaticSparseMatrix object.
 */
mrmc::sparse::StaticSparseMatrix<double> * readTraFile(const char * filename);
		
} // namespace parser
} // namespace mrmc

#endif /* READTRAFILE_H_ */