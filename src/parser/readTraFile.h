/*
 * read_tra_file.h
 *
 *  Created on: 15.08.2012
 *      Author: Thomas Heinemann
 */

#pragma once

#include "src/sparse/static_sparse_matrix.h"

namespace mrmc {
	namespace parser {
	
		mrmc::sparse::StaticSparseMatrix<double> * readTraFile(const char * filename);
		
	}
}

