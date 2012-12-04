#ifndef READLABFILE_H_
#define READLABFILE_H_

#include "src/models/AtomicPropositionsLabeling.h"
#include "boost/integer/integer_mask.hpp"

#include "src/parser/parser.h"

namespace mrmc {
namespace parser {

/*!
 *	@brief Load label file and return initialized AtomicPropositionsLabeling object.
 *
 *	Note that this class creates a new AtomicPropositionsLabeling object that can
 *	be accessed via getLabeling(). However, it will not delete this object!
 */
class LabParser : Parser
{
	public:
		LabParser(uint_fast64_t node_count, const char* filename);

		mrmc::models::AtomicPropositionsLabeling* getLabeling()
		{
			return this->labeling;
		}
	
	private:
		mrmc::models::AtomicPropositionsLabeling* labeling;
};

} // namespace parser
} // namespace mrmc

#endif /* READLABFILE_H_ */
