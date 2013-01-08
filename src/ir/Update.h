/*
 * Update.h
 *
 *  Created on: 06.01.2013
 *      Author: chris
 */

#ifndef UPDATE_H_
#define UPDATE_H_

#include "IR.h"

namespace storm {

namespace ir {

class Update {
public:
	std::shared_ptr<storm::ir::expressions::BaseExpression> likelihoodExpression;
	std::vector<storm::ir::Assignment> assignments;
};

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::Update,
    (std::shared_ptr<storm::ir::expressions::BaseExpression>, likelihoodExpression)
    (std::vector<storm::ir::Assignment>, assignments)
)

#endif /* UPDATE_H_ */
