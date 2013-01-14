/*
 * Vector.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_VECTOR_H_
#define STORM_UTILITY_VECTOR_H_

#include "Eigen/src/Core/Matrix.h"
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

} //namespace utility

} //namespace storm

#endif /* STORM_UTILITY_VECTOR_H_ */
