/*
 * read_tra_file.h
 *
 *  Created on: 15.08.2012
 *      Author: Thomas Heinemann
 */

#ifndef READ_TRA_FILE_H_
#define READ_TRA_FILE_H_
#include "src/storage/SquareSparseMatrix.h"
namespace mrmc{
namespace parser {

mrmc::storage::SquareSparseMatrix<double> * read_tra_file(const char * filename);

}
}


#endif /* READ_TRA_FILE_H_ */
