/*
 * constants.h
 *
 *  Created on: 11.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_UTILITY_CONSTTEMPLATES_H_
#define STORM_UTILITY_CONSTTEMPLATES_H_

#ifdef max
#	undef max
#endif

#ifdef min
#	undef min
#endif

#include <limits>

#include "src/exceptions/InvalidArgumentException.h"
#include "src/storage/BitVector.h"
#include "src/storage/LabeledValues.h"

namespace storm {

namespace utility {

/*!
 * Returns a constant value of 0 that is fit to the type it is being written to.
 * As (at least) gcc has problems to use the correct template by the return value
 * only, the function gets a pointer as a parameter to infer the return type.
 *
 * @note
 * 	The template parameter is just inferred by the return type; GCC is not able to infer this
 * 	automatically, hence the type should always be stated explicitly (e.g. @c constantZero<int>();)
 *
 * @return Value 0, fit to the return type
 */
template<typename _Scalar>
static inline _Scalar constantZero() {
   return _Scalar(0);
}

/*! @cond TEMPLATE_SPECIALIZATION
 * (By default, the template specifications are not included in the documentation)
 */

/*!
 * Template specialization for int_fast32_t
 * @return Value 0, fit to the type int_fast32_t
 */
template <>
inline int_fast32_t constantZero() {
   return 0;
}

/*!
 * Template specialization for uint_fast64_t
 * @return Value 0, fit to the type uint_fast64_t
 */
template <>
inline uint_fast64_t constantZero() {
   return 0;
}

/*!
 * Template specialization for double
 * @return Value 0.0, fit to the type double
 */
template <>
inline double constantZero() {
   return 0.0;
}
    
/*!
 * Template specialization for LabeledValues.
 * @return A LabeledValues object that represents a value of 0.
 */
template<>
inline storm::storage::LabeledValues<double> constantZero() {
    return storm::storage::LabeledValues<double>(0.0);
}

/*! @endcond */

/*!
 * Returns a constant value of 1 that is fit to the type it is being written to.
 * As (at least) gcc has problems to use the correct template by the return value
 * only, the function gets a pointer as a parameter to infer the return type.
 *
 * @note
 * 	The template parameter is just inferred by the return type; GCC is not able to infer this
 * 	automatically, hence the type should always be stated explicitly (e.g. @c constantOne<int>();)
 *
 * @return Value 1, fit to the return type
 */
template<typename _Scalar>
static inline _Scalar constantOne() {
   return _Scalar(1);
}

/*! @cond TEMPLATE_SPECIALIZATION
 * (By default, the template specializations are not included in the documentation)
 */

/*!
 * Template specialization for int_fast32_t
 * @return Value 1, fit to the type int_fast32_t
 */
template<>
inline int_fast32_t constantOne() {
   return 1;
}

/*!
 * Template specialization for uint_fast64_t
 * @return Value 1, fit to the type uint_fast61_t
 */
template<>
inline uint_fast64_t constantOne() {
   return 1;
}

/*!
 * Template specialization for double
 * @return Value 1.0, fit to the type double
 */
template<>
inline double constantOne() {
   return 1.0;
}

/*!
 * Template specialization for LabeledValues.
 * @return A LabeledValues object that represents a value of 1.
 */
template<>
inline storm::storage::LabeledValues<double> constantOne() {
    return storm::storage::LabeledValues<double>(1.0);
}
    
/*! @endcond */

/*!
 * Returns a constant value of infinity that is fit to the type it is being written to.
 * As (at least) gcc has problems to use the correct template by the return value
 * only, the function gets a pointer as a parameter to infer the return type.
 *
 * @note
 * 	The template parameter is just inferred by the return type; GCC is not able to infer this
 * 	automatically, hence the type should always be stated explicitly (e.g. @c constantOne<int>();)
 *
 * @return Value Infinity, fit to the return type
 */
template<typename _Scalar>
static inline _Scalar constantInfinity() {
   return std::numeric_limits<_Scalar>::infinity();
}

/*! @cond TEMPLATE_SPECIALIZATION
 * (By default, the template specializations are not included in the documentation)
 */

/*!
 * Template specialization for int_fast32_t
 * @return Value Infinity, fit to the type uint_fast32_t
 */
template<>
inline int_fast32_t constantInfinity() {
	throw storm::exceptions::InvalidArgumentException() << "Integer has no infinity.";
	return std::numeric_limits<int_fast32_t>::max();
}

/*!
 * Template specialization for uint_fast64_t
 * @return Value Infinity, fit to the type uint_fast64_t
 */
template<>
inline uint_fast64_t constantInfinity() {
	throw storm::exceptions::InvalidArgumentException() << "Integer has no infinity.";
	return std::numeric_limits<uint_fast64_t>::max();
}

/*!
 * Template specialization for double
 * @return Value Infinity, fit to the type double
 */
template<>
inline double constantInfinity() {
   return std::numeric_limits<double>::infinity();
}

/*!
 * Template specialization for LabeledValues.
 * @return Value Infinity, fit to the type LabeledValues.
 */
template<>
inline storm::storage::LabeledValues<double> constantInfinity() {
    return storm::storage::LabeledValues<double>(std::numeric_limits<double>::infinity());
}
    
/*! @endcond */

    
} //namespace utility

} //namespace storm

#endif /* STORM_UTILITY_CONSTTEMPLATES_H_ */
