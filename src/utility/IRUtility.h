/*
 * IRUtility.h
 *
 *  Created on: 05.10.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_IRUTILITY_H_
#define STORM_UTILITY_IRUTILITY_H_

#include <IR.h>

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {
    namespace utility {
        namespace ir {

            /*!
             * Computes the weakest precondition of the given boolean expression wrt. the given updates. The weakest
             * precondition is the most liberal expression that must hold in order to satisfy the given boolean
             * expression after performing the updates. The updates must be disjoint in the sense that they must not
             * assign an expression to a variable twice or more.
             *
             * @param expression The expression for which to build the weakest precondition.
             * @param update The update with respect to which to compute the weakest precondition.
             */
            std::shared_ptr<storm::ir::expressions::BaseExpression> getWeakestPrecondition(std::shared_ptr<storm::ir::expressions::BaseExpression> booleanExpression, std::vector<storm::ir::Update> const& updates) {
                std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> variableToExpressionMap;
                
                // Construct the full substitution we need to perform later.
                for (auto const& update : updates) {
                    for (uint_fast64_t assignmentIndex = 0; assignmentIndex < update.getNumberOfAssignments(); ++assignmentIndex) {
                        storm::ir::Assignment const& update.getAssignment(assignmentIndex);
                        
                        variableToExpressionMap[assignment.getVariableName()] = assignment.getExpression();
                    }
                }
                
                // Copy the given expression and apply the substitution.
                std::shared_ptr<storm::ir::expressions::BaseExpression> copiedExpression = booleanExpression->clone();
                copiedExpression->substitute(variableToExpressionMap);
                
                return copiedExpression;
            }
            
        } // namespace ir
    } // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_IRUTILITY_H_ */
