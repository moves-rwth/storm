/*
 * IRUtility.h
 *
 *  Created on: 05.10.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_IRUTILITY_H_
#define STORM_UTILITY_IRUTILITY_H_

#include <src/ir/IR.h>

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
            std::unique_ptr<storm::ir::expressions::BaseExpression> getWeakestPrecondition(std::unique_ptr<storm::ir::expressions::BaseExpression> const& booleanExpression, std::vector<storm::ir::Update> const& updates) {
                std::map<std::string, std::reference_wrapper<storm::ir::expressions::BaseExpression>> variableToExpressionMap;
                
                // Construct the full substitution we need to perform later.
                for (auto const& update : updates) {
                    for (auto const& variableAssignmentPair : update.getBooleanAssignments()) {
                        variableToExpressionMap.emplace(variableAssignmentPair.first, *variableAssignmentPair.second.getExpression());
                    }
                    for (auto const& variableAssignmentPair : update.getIntegerAssignments()) {
                        variableToExpressionMap.emplace(variableAssignmentPair.first, *variableAssignmentPair.second.getExpression());
                    }
                }
                
                // Copy the given expression and apply the substitution.
                return storm::ir::expressions::BaseExpression::substitute(booleanExpression->clone(), variableToExpressionMap);
            }
            
        } // namespace ir
    } // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_IRUTILITY_H_ */
