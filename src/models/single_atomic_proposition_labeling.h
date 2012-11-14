#ifndef MRMC_MODELS_SINGLE_ATOMIC_PROPOSITION_LABELING_H_
#define MRMC_MODELS_SINGLE_ATOMIC_PROPOSITION_LABELING_H_

#include "src/vector/bitvector.h"

namespace mrmc {

namespace models {

/*! 
 * This class represents the labeling of a state space with a single atomic proposition.
 * Internally, this is done by keeping a dense bit vector such that the i-th bit
 * in the vector is set to true if and only if the i-th state satisfies the
 * atomic proposition
 */
class SingleAtomicPropositionLabeling {
 public:

	//! Constructor
	 /*!
		\param nodeCount Amount of nodes that the DTMC has to label
	 */
	SingleAtomicPropositionLabeling(uint_fast32_t nodeCount) : nodes(nodeCount) {
		// intentionally left empty
	}

	//! Copy Constructor
	/*!
	 * Copy constructor. Performs a deep copy of this AtomicProposition object.
	 */
	SingleAtomicPropositionLabeling(const SingleAtomicPropositionLabeling& single_atomic_proposition_labeling) : nodes(single_atomic_proposition_labeling.nodes) {
		// intentionally left empty
	}

	~SingleAtomicPropositionLabeling() {
		// intentionally left empty
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

} // namespace models

} // namespace mrmc

#endif // MRMC_MODELS_SINGLE_ATOMIC_PROPOSITION_LABELING_H_
