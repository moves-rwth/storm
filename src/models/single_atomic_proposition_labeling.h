#ifndef MRMC_MODELS_SINGLE_ATOMIC_PROPOSITION_LABELING_H_
#define MRMC_MODELS_SINGLE_ATOMIC_PROPOSITION_LABELING_H_

#include "src/storage/BitVector.h"

namespace mrmc {

namespace models {

/*! 
 * This class represents the labeling of a state space with a single atomic
 * proposition.
 */
class SingleAtomicPropositionLabeling {
 public:

	//! Constructor
	/*!
	 * Constructs a labeling for the given number of states.
	 * @param state_count Amount of states for which this objects manages the
	 * labeling with one particular atomic proposition.
	 */
	SingleAtomicPropositionLabeling(uint_fast32_t state_count) : label_vector(state_count) {
		// intentionally left empty
	}

	//! Copy Constructor
	/*!
	 * Copy constructor. Performs a deep copy of the given
	 * SingleAtomicPropositionLabeling object.
	 * @param single_atomic_proposition_labeling The object to copy.
	 */
	SingleAtomicPropositionLabeling(const SingleAtomicPropositionLabeling& single_atomic_proposition_labeling)
		: label_vector(single_atomic_proposition_labeling.label_vector) {
		// intentionally left empty
	}

	/*!
	 * Checks whether the given state possesses the label.
	 * @param state_index The index of the state to check.
	 */
	bool hasLabel(uint_fast32_t state_index) {
		return label_vector.get(state_index);
	}

	/*!
	 * Adds the label to the given state.
	 * @param state_index The index of the state to label.
	 */
	void addLabelToState(uint_fast32_t state_index) {
		label_vector.set(state_index, true);
	}
	
	/*!
	 * Returns the number of states that are labeled.
	 * @return The number of states that are labeled.
	 */
	uint_fast64_t getNumberOfLabeledStates() {
		return label_vector.getNumberOfSetBits();
	}

	/*!
	 * Returns the size of the single atomic proposition labeling in memory
	 * measured in bytes.
	 * @return The size of the single atomic proposition labeling in memory
	 * measured in bytes.
	 */
	uint_fast64_t getSizeInMemory() {
		return sizeof(*this) + label_vector.getSizeInMemory();
	}

 private:

	/*!
	 * A bit vector storing for every state whether or not that state is
	 * labeled.
	 */
	mrmc::vector::BitVector label_vector;
};

} // namespace models

} // namespace mrmc

#endif // MRMC_MODELS_SINGLE_ATOMIC_PROPOSITION_LABELING_H_
