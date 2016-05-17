/*
 * helpers.h
 *
 *  Created on: 16 Apr 2016
 *      Author: harold
 */

#ifndef PYTHON_HELPERS_H_
#define PYTHON_HELPERS_H_

#include <sstream>
#include <string>

/**
 * Helper function to get a string out of the stream operator.
 * Used for __str__ functions.
 */
template<typename T>
std::string streamToString(T const & t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

// Be warned: Enabling something like this will break everything about Monomial,
// as to Python the shared_ptr (Arg) IS the Monomial
//  //PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

#endif /* PYTHON_HELPERS_H_ */
