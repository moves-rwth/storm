#ifndef MRMC_DTMC_ATOMIC_PROPOSITION_H_
#define MRMC_DTMC_ATOMIC_PROPOSITION_H_

#include <exception>
#include <cmath>
#include "boost/integer/integer_mask.hpp"

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

#include "src/vector/bitvector.h"


namespace mrmc {

namespace dtmc {

//! An atomic proposition for DTMCs with a constant number of nodes
/*! 
	A dense vector representing a single atomic proposition for all nodes of a DTMC.
 */
class AtomicProposition {
 public:

	//! Constructor
	 /*!
		\param nodeCount Amount of nodes that the DTMC has to label
	 */
	AtomicProposition(uint_fast32_t nodeCount) : nodes(nodeCount) {
		//
	}

	~AtomicProposition() {
		//
	}

	bool hasNodeLabel(uint_fast32_t nodeId) {
		return nodes.get(nodeId);
	}

	void addLabelToNode(uint_fast32_t nodeId) {
		nodes.set(nodeId, true);
	}
	
 private:
	/*! BitVector containing the boolean bits for each node */
	mrmc::vector::BitVector nodes;
};

} // namespace dtmc

} // namespace mrmc

#endif // MRMC_DTMC_ATOMIC_PROPOSITION_H_
