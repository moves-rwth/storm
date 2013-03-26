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

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

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
void selectVectorValues(std::vector<T>* vector, const storm::storage::BitVector& positions, const std::vector<uint_fast64_t>& rowMapping, std::vector<T> const& values) {
	uint_fast64_t oldPosition = 0;
	for (auto position : positions) {
		for (uint_fast64_t i = rowMapping[position]; i < rowMapping[position + 1]; ++i) {
			(*vector)[oldPosition++] = values[i];
		}
	}
}

template<class T>
void selectVectorValuesRepeatedly(std::vector<T>* vector, const storm::storage::BitVector& positions, const std::vector<uint_fast64_t>& rowMapping, std::vector<T> const& values) {
	uint_fast64_t oldPosition = 0;
	for (auto position : positions) {
		for (uint_fast64_t i = rowMapping[position]; i < rowMapping[position + 1]; ++i) {
			(*vector)[oldPosition++] = values[position];
		}
	}
}

template<class T>
void subtractFromConstantOneVector(std::vector<T>* vector) {
	for (auto it = vector->begin(); it != vector->end(); ++it) {
		*it = storm::utility::constGetOne<T>() - *it;
	}
}

template<class T>
void addVectors(std::vector<T>& target, std::vector<T> const& summand) {
	if (target.size() != target.size()) {
		LOG4CPLUS_ERROR(logger, "Lengths of vectors does not match and makes operation impossible.");
		throw storm::exceptions::InvalidArgumentException() << "Length of vectors does not match and makes operation impossible.";
	}

	for (uint_fast64_t i = 0; i < target.size(); ++i) {
		target[i] += summand[i];
	}
}

template<class T>
void addVectors(std::vector<uint_fast64_t> const& states, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<T>& original, std::vector<T> const& summand) {
	for (auto stateIt = states.cbegin(), stateIte = states.cend(); stateIt != stateIte; ++stateIt) {
		for (auto rowIt = nondeterministicChoiceIndices[*stateIt], rowIte = nondeterministicChoiceIndices[*stateIt + 1]; rowIt != rowIte; ++rowIt) {
			original[rowIt] += summand[rowIt];
		}
	}
}

template<class T>
void reduceVectorMin(std::vector<T> const& source, std::vector<T>* target, std::vector<uint_fast64_t> const& filter) {
	uint_fast64_t currentSourceRow = 0;
	uint_fast64_t currentTargetRow = -1;
	for (auto it = source.cbegin(); it != source.cend(); ++it, ++currentSourceRow) {
		// Check whether we have considered all source rows for the current target row.
		if (filter[currentTargetRow + 1] <= currentSourceRow || currentSourceRow == 0) {
			++currentTargetRow;
			(*target)[currentTargetRow] = source[currentSourceRow];
			continue;
		}

		// We have to minimize the value, so only overwrite the current value if the
		// value is actually lower.
		if (*it < (*target)[currentTargetRow]) {
			(*target)[currentTargetRow] = *it;
		}
	}
}

template<class T>
void reduceVectorMin(std::vector<T> const& source, std::vector<T>* target, std::vector<uint_fast64_t> const& scc, std::vector<uint_fast64_t> const& filter) {
	for (auto stateIt = scc.cbegin(); stateIt != scc.cend(); ++stateIt) {
		(*target)[*stateIt] = source[filter[*stateIt]];

		for (auto row = filter[*stateIt] + 1; row < filter[*stateIt + 1]; ++row) {
			// We have to minimize the value, so only overwrite the current value if the
			// value is actually lower.
			if (source[row] < (*target)[*stateIt]) {
				(*target)[*stateIt] = source[row];
			}
		}
	}
}

template<class T>
void reduceVectorMax(std::vector<T> const& source, std::vector<T>* target, std::vector<uint_fast64_t> const& filter) {
	uint_fast64_t currentSourceRow = 0;
	uint_fast64_t currentTargetRow = -1;
	for (auto it = source.cbegin(); it != source.cend(); ++it, ++currentSourceRow) {
		// Check whether we have considered all source rows for the current target row.
		if (filter[currentTargetRow + 1] <= currentSourceRow || currentSourceRow == 0) {
			++currentTargetRow;
			(*target)[currentTargetRow] = source[currentSourceRow];
			continue;
		}

		// We have to maximize the value, so only overwrite the current value if the
		// value is actually greater.
		if (*it > (*target)[currentTargetRow]) {
			(*target)[currentTargetRow] = *it;
		}
	}
}

template<class T>
void reduceVectorMax(std::vector<T> const& source, std::vector<T>* target, std::vector<uint_fast64_t> const& scc, std::vector<uint_fast64_t> const& filter) {
	for (auto stateIt = scc.cbegin(); stateIt != scc.cend(); ++stateIt) {
		(*target)[*stateIt] = source[filter[*stateIt]];

		for (auto row = filter[*stateIt] + 1; row < filter[*stateIt + 1]; ++row) {
			// We have to maximize the value, so only overwrite the current value if the
			// value is actually lower.
			if (source[row] > (*target)[*stateIt]) {
				(*target)[*stateIt] = source[row];
			}
		}
	}
}

template<class T>
bool equalModuloPrecision(std::vector<T> const& vectorLeft, std::vector<T> const& vectorRight, T precision, bool relativeError) {
	if (vectorLeft.size() != vectorRight.size()) {
		LOG4CPLUS_ERROR(logger, "Lengths of vectors does not match and makes comparison impossible.");
		throw storm::exceptions::InvalidArgumentException() << "Length of vectors does not match and makes comparison impossible.";
	}

	for (uint_fast64_t i = 0; i < vectorLeft.size(); ++i) {
		if (relativeError) {
			if (std::abs(vectorLeft[i] - vectorRight[i])/vectorRight[i] > precision) return false;
		} else {
			if (std::abs(vectorLeft[i] - vectorRight[i]) > precision) return false;
		}
	}

	return true;
}

template<class T>
bool equalModuloPrecision(std::vector<T> const& vectorLeft, std::vector<T> const& vectorRight, std::vector<uint_fast64_t> const& scc, T precision, bool relativeError) {
	if (vectorLeft.size() != vectorRight.size()) {
		LOG4CPLUS_ERROR(logger, "Lengths of vectors does not match and makes comparison impossible.");
		throw storm::exceptions::InvalidArgumentException() << "Length of vectors does not match and makes comparison impossible.";
	}

	for (uint_fast64_t state : scc) {
		if (relativeError) {
			if (std::abs(vectorLeft[state] - vectorRight[state])/vectorRight[state] > precision) return false;
		} else {
			if (std::abs(vectorLeft[state] - vectorRight[state]) > precision) return false;
		}
	}

	return true;
}

} //namespace utility

} //namespace storm

#endif /* STORM_UTILITY_VECTOR_H_ */
