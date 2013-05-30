/*
 * vector.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_VECTOR_H_
#define STORM_UTILITY_VECTOR_H_

#include "Eigen/Core"
#include "ConstTemplates.h"
#include <iostream>
#include <algorithm>
#include <functional>

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

namespace utility {
    
namespace vector {

/*!
 * Sets the provided values at the provided positions in the given vector.
 *
 * @param vector The vector in which the values are to be set.
 * @param positions The positions at which the values are to be set.
 * @param values The values that are to be set.
 */
template<class T>
void setVectorValues(std::vector<T>& vector, storm::storage::BitVector const& positions, std::vector<T> const& values) {
	uint_fast64_t oldPosition = 0;
	for (auto position : positions) {
		vector[position] = values[oldPosition++];
	}
}

/*!
 * Sets the provided value at the provided positions in the given vector.
 *
 * @param vector The vector in which the value is to be set.
 * @param positions The positions at which the value is to be set.
 * @param value The value that is to be set.
 */
template<class T>
void setVectorValues(std::vector<T>& vector, storm::storage::BitVector const& positions, T value) {
	for (auto position : positions) {
		vector[position] = value;
	}
}

/*!
 * Sets the provided value at the provided positions in the given vector.
 *
 * @param vector The vector in which the value is to be set.
 * @param positions The positions at which the value is to be set.
 * @param value The value that is to be set.
 */
template<class T>
void setVectorValues(Eigen::Matrix<T, -1, 1, 0, -1, 1>& eigenVector, storm::storage::BitVector const& positions, T value) {
	for (auto position : positions) {
		eigenVector(position, 0) = value;
	}
}

/*!
 * Selects the elements from a vector at the specified positions and writes them consecutively into another vector.
 * @param vector The vector into which the selected elements are to be written.
 * @param positions The positions at which to select the elements from the values vector.
 * @param values The vector from which to select the elements.
 */
template<class T>
void selectVectorValues(std::vector<T>& vector, storm::storage::BitVector const& positions, std::vector<T> const& values) {
	uint_fast64_t oldPosition = 0;
	for (auto position : positions) {
		vector[oldPosition++] = values[position];
	}
}

/*!
 * Selects groups of elements from a vector at the specified positions and writes them consecutively into another vector.
 *
 * @param vector The vector into which the selected elements are to be written.
 * @param positions The positions of the groups of elements that are to be selected.
 * @param rowGrouping A vector that specifies the begin and end of each group of elements in the values vector.
 * @param values The vector from which to select groups of elements.
 */
template<class T>
void selectVectorValues(std::vector<T>& vector, storm::storage::BitVector const& positions, std::vector<uint_fast64_t> const& rowGrouping, std::vector<T> const& values) {
	uint_fast64_t oldPosition = 0;
	for (auto position : positions) {
		for (uint_fast64_t i = rowGrouping[position]; i < rowGrouping[position + 1]; ++i) {
			vector[oldPosition++] = values[i];
		}
	}
}

/*!
 * Selects values from a vector at the specified positions and writes them into another vector as often as given by
 * the size of the corresponding group of elements.
 *
 * @param vector The vector into which the selected elements are written.
 * @param positions The positions at which to select the values.
 * @param rowGrouping A vector that specifies the begin and end of each group of elements in the values vector. This
 * implicitly defines the number of times any element is written to the output vector.
 */
template<class T>
void selectVectorValuesRepeatedly(std::vector<T>& vector, storm::storage::BitVector const& positions, std::vector<uint_fast64_t> const& rowGrouping, std::vector<T> const& values) {
	uint_fast64_t oldPosition = 0;
	for (auto position : positions) {
		for (uint_fast64_t i = rowGrouping[position]; i < rowGrouping[position + 1]; ++i) {
			vector[oldPosition++] = values[position];
		}
	}
}

/*!
 * Subtracts the given vector from the constant one-vector and writes the result to the input vector.
 *
 * @param vector The vector that is to be subtracted from the constant one-vector.
 */
template<class T>
void subtractFromConstantOneVector(std::vector<T>& vector) {
	for (auto element : vector) {
		element = storm::utility::constGetOne<T>() - element;
	}
}
    
/*!
 * Adds the two given vectors and writes the result into the first operand.
 *
 * @param target The first summand and target vector.
 * @param summand The second summand.
 */
template<class T>
void addVectorsInPlace(std::vector<T>& target, std::vector<T> const& summand) {
    if (target.size() != summand.size()) {
        LOG4CPLUS_ERROR(logger, "Lengths of vectors do not match, which makes operation impossible.");
        throw storm::exceptions::InvalidArgumentException() << "Length of vectors do not match, which makes operation impossible.";
    }
    
    std::transform(target.begin(), target.end(), summand.begin(), target.begin(), std::plus<T>());
}

/*!
 * Reduces the given source vector by selecting an element according to the given filter out of each row group.
 *
 * @param source The source vector which is to be reduced.
 * @param target The target vector into which a single element from each row group is written.
 * @param rowGrouping A vector that specifies the begin and end of each group of elements in the values vector.
 * @param filter A function that compares two elements v1 and v2 according to some filter criterion. This function must
 * return true iff v1 is supposed to be taken instead of v2.
 */
template<class T>
void reduceVector(std::vector<T> const& source, std::vector<T>& target, std::vector<uint_fast64_t> const& rowGrouping, std::function<bool (T const&, T const&)> filter) {
    uint_fast64_t currentSourceRow = 0;
    uint_fast64_t currentTargetRow = -1;
    for (auto it = source.cbegin(), ite = source.cend(); it != ite; ++it, ++currentSourceRow) {
        // Check whether we have considered all source rows for the current target row.
        if (rowGrouping[currentTargetRow + 1] <= currentSourceRow || currentSourceRow == 0) {
            ++currentTargetRow;
            target[currentTargetRow] = source[currentSourceRow];
            continue;
        }
            
        // We have to minimize the value, so only overwrite the current value if the
        // value is actually lower.
        if (filter(*it, target[currentTargetRow])) {
            target[currentTargetRow] = *it;
        }
    }
}

/*!
 * Reduces the given source vector by selecting the smallest element out of each row group.
 *
 * @param source The source vector which is to be reduced.
 * @param target The target vector into which a single element from each row group is written.
 * @param rowGrouping A vector that specifies the begin and end of each group of elements in the values vector.
 */
template<class T>
void reduceVectorMin(std::vector<T> const& source, std::vector<T>& target, std::vector<uint_fast64_t> const& rowGrouping) {
	reduceVector<T>(source, target, rowGrouping, [] (T const& val1, T const& val2) -> bool { return val1 < val2; });
}

/*!
 * Reduces the given source vector by selecting the largest element out of each row group.
 *
 * @param source The source vector which is to be reduced.
 * @param target The target vector into which a single element from each row group is written.
 * @param rowGrouping A vector that specifies the begin and end of each group of elements in the values vector.
 */
template<class T>
void reduceVectorMax(std::vector<T> const& source, std::vector<T>& target, std::vector<uint_fast64_t> const& rowGrouping) {
    reduceVector<T>(source, target, rowGrouping, [] (T const& val1, T const& val2) -> bool { return val1 > val2; });
}

/*!
 * Compares the given elements and determines whether they are equal modulo the given precision. The provided flag
 * additionaly specifies whether the error is computed in relative or absolute terms.
 *
 * @param val1 The first value to compare.
 * @param val2 The second value to compare.
 * @param precision The precision up to which the elements are compared.
 * @param relativeError If set, the error is computed relative to the second value.
 * @return True iff the elements are considered equal.
 */
template<class T>
bool equalModuloPrecision(T const& val1, T const& val2, T precision, bool relativeError = true) {
    if (relativeError) {
        if (std::abs(val1 - val2)/val2 > precision) return false;
    } else {
        if (std::abs(val1 - val2) > precision) return false;
    }
    return true;
}
    
/*!
 * Compares the two vectors and determines whether they are equal modulo the provided precision. Depending on whether the
 * flag is set, the difference between the vectors is computed relative to the value or in absolute terms.
 *
 * @param vectorLeft The first vector of the comparison.
 * @param vectorRight The second vector of the comparison.
 * @param precision The precision up to which the vectors are to be checked for equality.
 * @param relativeError If set, the difference between the vectors is computed relative to the value or in absolute terms.
 */
template<class T>
bool equalModuloPrecision(std::vector<T> const& vectorLeft, std::vector<T> const& vectorRight, T precision, bool relativeError) {
	if (vectorLeft.size() != vectorRight.size()) {
		LOG4CPLUS_ERROR(logger, "Lengths of vectors do not match, which makes comparison impossible.");
		throw storm::exceptions::InvalidArgumentException() << "Lengths of vectors do not match, which makes comparison impossible.";
	}

	for (uint_fast64_t i = 0; i < vectorLeft.size(); ++i) {
		if (!equalModuloPrecision(vectorLeft[i], vectorRight[i], precision, relativeError)) {
            return false;
        }
	}

	return true;
}

/*!
 * Compares the two vectors at the specified positions and determines whether they are equal modulo the provided
 * precision. Depending on whether the flag is set, the difference between the vectors is computed relative to the value
 * or in absolute terms.
 *
 * @param vectorLeft The first vector of the comparison.
 * @param vectorRight The second vector of the comparison.
 * @param precision The precision up to which the vectors are to be checked for equality.
 * @param positions A vector representing a set of positions at which the vectors are compared.
 * @param relativeError If set, the difference between the vectors is computed relative to the value or in absolute terms.
 */
template<class T>
bool equalModuloPrecision(std::vector<T> const& vectorLeft, std::vector<T> const& vectorRight, std::vector<uint_fast64_t> const& positions, T precision, bool relativeError) {
	if (vectorLeft.size() != vectorRight.size()) {
		LOG4CPLUS_ERROR(logger, "Lengths of vectors do not match, which makes comparison impossible.");
		throw storm::exceptions::InvalidArgumentException() << "Lengths of vectors do not match, which makes comparison impossible.";
	}

	for (uint_fast64_t position : positions) {
		if (!equalModuloPrecision(vectorLeft[position], vectorRight[position], precision, relativeError)) {
            return false;
        }
	}

	return true;
}
    
} // namespace vector

} // namespace utility

} // namespace storm

#endif /* STORM_UTILITY_VECTOR_H_ */
