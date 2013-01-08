/*
 * Assignment.h
 *
 *  Created on: 06.01.2013
 *      Author: chris
 */

#ifndef ASSIGNMENT_H_
#define ASSIGNMENT_H_

#include "expressions/Expressions.h"
#include <boost/fusion/include/adapt_struct.hpp>

namespace storm {

namespace ir {

class Assignment {
public:

	std::string variableName;
	std::shared_ptr<storm::ir::expressions::BaseExpression> expression;
};

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::Assignment,
    (std::string, variableName)
    (std::shared_ptr<storm::ir::expressions::BaseExpression>, expression)
)

#endif /* ASSIGNMENT_H_ */
