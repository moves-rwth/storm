/*
 * DtmcParser.cpp
 *
 *  Created on: 19.12.2012
 *      Author: thomas
 */

#include "DtmcParser.h"
#include "DeterministicSparseTransitionParser.h"
#include "AtomicPropositionLabelingParser.h"

namespace mrmc {
namespace parser {

/*!
 * Parses a transition file and a labeling file and produces a DTMC out of them; a pointer to the dtmc
 * is saved in the field "dtmc"
 * Note that the labeling file may have at most as many nodes as the transition file!
 *
 * @param transitionSystemFile String containing the location of the transition file (....tra)
 * @param labelingFile String containing the location of the labeling file (....lab)
 */
DtmcParser::DtmcParser(std::string const & transitionSystemFile, std::string const & labelingFile) {
	mrmc::parser::DeterministicSparseTransitionParser tp(transitionSystemFile);
	uint_fast64_t nodeCount = tp.getMatrix()->getRowCount();

	mrmc::parser::AtomicPropositionLabelingParser lp(nodeCount, labelingFile);

	dtmc = std::shared_ptr<mrmc::models::Dtmc<double> >(new mrmc::models::Dtmc<double>(tp.getMatrix(), lp.getLabeling()));
}

} /* namespace parser */
} /* namespace mrmc */
