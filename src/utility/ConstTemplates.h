/*
 * ConstTemplates.h
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

namespace storm {

namespace utility {

/*!
 * Returns a constant value of 0 that is fit to the type it is being written to.
 * As (at least) gcc has problems to use the correct template by the return value
 * only, the function gets a pointer as a parameter to infer the return type.
 *
 * @note
 * 	The template parameter is just inferred by the return type; GCC is not able to infer this
 * 	automatically, hence the type should always be stated explicitly (e.g. @c constGetZero<int>();)
 *
 * @return Value 0, fit to the return type
 */
template<typename _Scalar>
static inline _Scalar constGetZero() {
   return _Scalar(0);
}

/*! @cond TEMPLATE_SPECIALIZATION
 * (By default, the template specifications are not included in the documentation)
 */

/*!
 * Template specification for int_fast32_t
 * @return Value 0, fit to the type int_fast32_t
 */
template <>
inline int_fast32_t constGetZero() {
   return 0;
}

/*!
 * Template specification for uint_fast64_t
 * @return Value 0, fit to the type uint_fast64_t
 */
template <>
inline uint_fast64_t constGetZero() {
   return 0;
}

/*!
 * Template specification for double
 * @return Value 0.0, fit to the type double
 */
template <>
inline double constGetZero() {
   return 0.0;
}

/*! @endcond */

/*!
 * Returns a constant value of 1 that is fit to the type it is being written to.
 * As (at least) gcc has problems to use the correct template by the return value
 * only, the function gets a pointer as a parameter to infer the return type.
 *
 * @note
 * 	The template parameter is just inferred by the return type; GCC is not able to infer this
 * 	automatically, hence the type should always be stated explicitly (e.g. @c constGetOne<int>();)
 *
 * @return Value 1, fit to the return type
 */
template<typename _Scalar>
static inline _Scalar constGetOne() {
   return _Scalar(1);
}

/*! @cond TEMPLATE_SPECIALIZATION
 * (By default, the template specifications are not included in the documentation)
 */

/*!
 * Template specification for int_fast32_t
 * @return Value 1, fit to the type int_fast32_t
 */
template<>
inline int_fast32_t constGetOne() {
   return 1;
}

/*!
 * Template specification for uint_fast64_t
 * @return Value 1, fit to the type uint_fast61_t
 */
template<>
inline uint_fast64_t constGetOne() {
   return 1;
}

/*!
 * Template specification for double
 * @return Value 1.0, fit to the type double
 */
template<>
inline double constGetOne() {
   return 1.0;
}

/*! @endcond */

/*!
 * Returns a constant value of infinity that is fit to the type it is being written to.
 * As (at least) gcc has problems to use the correct template by the return value
 * only, the function gets a pointer as a parameter to infer the return type.
 *
 * @note
 * 	The template parameter is just inferred by the return type; GCC is not able to infer this
 * 	automatically, hence the type should always be stated explicitly (e.g. @c constGetOne<int>();)
 *
 * @return Value Infinity, fit to the return type
 */
template<typename _Scalar>
static inline _Scalar constGetInfinity() {
   return std::numeric_limits<_Scalar>::infinity();
}

/*! @cond TEMPLATE_SPECIALIZATION
 * (By default, the template specifications are not included in the documentation)
 */

/*!
 * Template specification for int_fast32_t
 * @return Value Infinity, fit to the type uint_fast32_t
 */
template<>
inline int_fast32_t constGetInfinity() {
	throw storm::exceptions::InvalidArgumentException() << "Integer has no infinity.";
	return std::numeric_limits<int_fast32_t>::max();
}

/*!
 * Template specification for uint_fast64_t
 * @return Value Infinity, fit to the type uint_fast64_t
 */
template<>
inline uint_fast64_t constGetInfinity() {
	throw storm::exceptions::InvalidArgumentException() << "Integer has no infinity.";
	return std::numeric_limits<int_fast64_t>::max();
}

/*!
 * Template specification for double
 * @return Value Infinity, fit to the type double
 */
template<>
inline double constGetInfinity() {
   return std::numeric_limits<double>::infinity();
}

/*! @endcond */

} //namespace utility

} //namespace storm


#endif /* STORM_UTILITY_CONSTTEMPLATES_H_ */
