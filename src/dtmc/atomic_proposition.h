#ifndef MRMC_DTMC_ATOMIC_PROPOSITION_H_
#define MRMC_DTMC_ATOMIC_PROPOSITION_H_

#include <exception>
#include <cmath>
#include "boost/integer/integer_mask.hpp"

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

#include "src/exceptions/invalid_state.h"
#include "src/exceptions/invalid_argument.h"
#include "src/exceptions/out_of_range.h"

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
	AtomicProposition(uint_fast32_t nodeCount) {
		node_count = nodeCount;
		node_array = new uint_fast64_t[(int) std::ceil(nodeCount / 64)]();
	}

	~AtomicProposition() {
		delete node_array;
	}

	bool hasNodeLabel(uint_fast32_t nodeId) {
		int bucket = static_cast<int>(std::floor(nodeId / 64));
		int bucket_place = nodeId % 64;
		// Taking the step with mask is crucial as we NEED a 64bit shift, not a 32bit one.
		uint_fast64_t mask = 1;
		mask = mask << bucket_place;
		return ((mask & node_array[bucket]) == mask);
	}

	void addLabelToNode(uint_fast32_t nodeId) {
		int bucket = static_cast<int>(std::floor(nodeId / 64));
		// Taking the step with mask is crucial as we NEED a 64bit shift, not a 32bit one.
		// MSVC: C4334, use 1i64 or cast to __int64.
		// result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
		uint_fast64_t mask = 1;
		mask = mask << (nodeId % 64);
		node_array[bucket] |= mask;
	}
	
 private:
	uint_fast32_t node_count;

	/*! Array containing the boolean bits for each node, 64bits/64nodes per element */
	uint_fast64_t* node_array;
};

} // namespace dtmc

} // namespace mrmc

#endif // MRMC_DTMC_ATOMIC_PROPOSITION_H_
