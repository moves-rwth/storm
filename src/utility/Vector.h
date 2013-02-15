/*
 * Vector.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_VECTOR_H_
#define STORM_UTILITY_VECTOR_H_

#include "Eigen/Core"
#include "ConstTemplates.h"
#include <iostream>

namespace storm {

namespace utility {

template<class T>
void setVectorValues(std::vector<T>* vector, const storm::storage::BitVector& positions, std::vector<T> const& values) {
	uint_fast64_t oldPosition = 0;
	for (auto position : positions) {
		(*vector)[position] = values[oldPosition++];
	}
}

template<class T>
void setVectorValues(std::vector<T>* vector, const storm::storage::BitVector& positions, T value) {
	for (auto position : positions) {
		(*vector)[position] = value;
	}
}

template<class T>
void setVectorValues(Eigen::Matrix<T, -1, 1, 0, -1, 1>* eigenVector, const storm::storage::BitVector& positions, T value) {
	for (auto position : positions) {
		(*eigenVector)(position, 0) = value;
	}
}

template<class T>
void selectVectorValues(std::vector<T>* vector, const storm::storage::BitVector& positions, std::vector<T> const& values) {
	uint_fast64_t oldPosition = 0;
	for (auto position : positions) {
		(*vector)[oldPosition++] = values[position];
	}
}

template<class T>
void subtractFromConstantOneVector(std::vector<T>* vector) {
	for (auto it = vector->begin(); it != vector->end(); ++it) {
		*it = storm::utility::constGetOne<T>() - *it;
	}
}

template<class T>
void reduceVectorMin(std::vector<T> const& source, std::vector<T>* target, std::vector<uint_fast64_t> const& filter) {
	uint_fast64_t currentSourceRow = 0;
	uint_fast64_t currentTargetRow = 0;
	for (auto it = source->cbegin(); it != source->cend(); ++it, ++currentSourceRow) {
		// Check whether we have considered all from rows for the current to row.
		if (filter[currentTargetRow + 1] <= currentSourceRow) {
			++currentTargetRow;
			(*target)[currentTargetRow] = (*source)[currentSourceRow];
			continue;
		}


		// We have to minimize the value, so only overwrite the current value if the
		// value is actually lower.
		if (*it < (*target)[currentTargetRow]) {
			(*source)[currentTargetRow] = *it;
		}
	}
}

template<class T>
void reduceVectorMax(std::vector<T> const& source, std::vector<T>* target, std::vector<uint_fast64_t> const& filter) {
	uint_fast64_t currentSourceRow = 0;
	uint_fast64_t currentTargetRow = 0;
	for (auto it = source->cbegin(); it != source->cend(); ++it, ++currentSourceRow) {
		// Check whether we have considered all from rows for the current to row.
		if (filter[currentTargetRow + 1] <= currentSourceRow) {
			++currentTargetRow;
			(*target)[currentTargetRow] = (*source)[currentSourceRow];
			continue;
		}


		// We have to maximize the value, so only overwrite the current value if the
		// value is actually greater.
		if (*it > (*target)[currentTargetRow]) {
			(*source)[currentTargetRow] = *it;
		}
	}
}

} //namespace utility

} //namespace storm

#endif /* STORM_UTILITY_VECTOR_H_ */
