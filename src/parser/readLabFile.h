#ifndef READLABFILE_H_
#define READLABFILE_H_

#include "src/models/AtomicPropositionsLabeling.h"
#include "boost/integer/integer_mask.hpp"


namespace mrmc {
namespace parser {

/*!
 *	@brief Load label file and return initialized AtomicPropositionsLabeling object.
 */
mrmc::models::AtomicPropositionsLabeling * readLabFile(uint_fast64_t node_count, const char * filename);

} // namespace parser
} // namespace mrmc

#endif /* READLABFILE_H_ */