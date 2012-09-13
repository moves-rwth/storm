/*
 * read_lab_file.h
 *
 *  Created on: 10.09.2012
 *      Author: thomas
 */

#ifndef READ_LAB_FILE_H_
#define READ_LAB_FILE_H_

#include "src/dtmc/labelling.h"


namespace mrmc {

namespace parser {

mrmc::dtmc::labelling * read_lab_file(int node_count, const char * filename);

}
}

#endif /* READ_LAB_FILE_H_ */
