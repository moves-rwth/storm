/* 
 * File:   BooleanExpression.cpp
 * Author: Lukas Westhofen
 * 
 * Created on 11. April 2015, 17:44
 */

#include "src/storage/pgcl/BooleanExpression.h"
#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace pgcl {
        BooleanExpression::BooleanExpression(storm::expressions::Expression const& booleanExpression) :
            booleanExpression(booleanExpression) {
        }
        
        storm::expressions::Expression& BooleanExpression::getBooleanExpression() {
            return this->booleanExpression;
        }
    }
}

