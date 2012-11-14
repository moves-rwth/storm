/*
 * dtmc.h
 *
 *  Created on: 14.11.2012
 *      Author: Christian Dehnert
 */

#ifndef DTMC_H_
#define DTMC_H_

#include "labeling.h"
#include "src/sparse/static_sparse_matrix.h"

namespace mrmc {

namespace models {

/*! This class represents a discrete-time Markov chain (DTMC) whose states are
 * labeled with atomic propositions.
 */
class Dtmc {

public:
	//! Constructor
	/*!
		\param probability_matrix The transition probability function of the DTMC given by a matrix.
		\param state_labeling The labeling that assigns a set of atomic propositions to each state.
	*/
	Dtmc(mrmc::sparse::StaticSparseMatrix* probability_matrix,
			mrmc::models::Labeling* state_labeling) {
		this->probability_matrix = probability_matrix;
		this->state_labeling = state_labeling;
	}

	//! Copy Constructor
	/*!
		Copy Constructor. Creates an exact copy of the source DTMC. Modification of either DTMC does not affect the other.
		@param dtmc A reference to the DTMC that is to be copied.
	 */
	Dtmc(const Dtmc &dtmc) : probability_matrix(dtmc.probability_matrix), state_labeling(dtmc.state_labeling) {
		// intentionally left empty
	}

private:

	mrmc::sparse::StaticSparseMatrix* probability_matrix;
	mrmc::models::Labeling* state_labeling;

};

} //namespace models

} //namespace mrmc

#endif /* DTMC_H_ */
