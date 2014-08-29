/*
 * ComparisonType.h
 *
 *  Created on: 17.04.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_COMPARISONTYPE_H_
#define STORM_FORMULA_COMPARISONTYPE_H_

namespace storm {
namespace properties {

/*!
 * An enum representing the greater- and less-than operators in both
 * the strict (<, >) and the non strict (<=, >=) variant.
 * It is mainly used to represent upper and lower bounds.
 */
enum ComparisonType { LESS, LESS_EQUAL, GREATER, GREATER_EQUAL };

}
}


#endif /* STORM_FORMULA_COMPARISONTYPE_H_ */
