/*
 * vector.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef VECTOR_H_
#define VECTOR_H_

namespace mrmc {

namespace utility {

template<class T>
void setVectorValues(std::vector<T>* vector, const mrmc::storage::BitVector& positions, const std::vector<T> values) {
	uint_fast64_t oldPosition = 0;
	for (auto position : positions) {
		(*vector)[position] = values[oldPosition++];
	}
}

template<class T>
void setVectorValues(std::vector<T>* vector, const mrmc::storage::BitVector& positions, T value) {
	for (auto position : positions) {
		(*vector)[position] = value;
	}
}

} //namespace utility

} //namespace mrmc

#endif /* VECTOR_H_ */
