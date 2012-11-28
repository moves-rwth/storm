#ifndef READLABFILE_H_
#define READLABFILE_H_

#include "src/models/atomic_propositions_labeling.h"


namespace mrmc {
namespace parser {

/*!
 *	@brief Load label file and return initialized AtomicPropositionsLabeling object.
 */
mrmc::models::AtomicPropositionsLabeling * readLabFile(int node_count, const char * filename);

} // namespace parser
} // namespace mrmc

#endif /* READLABFILE_H_ */