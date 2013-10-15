#include "IntegerExpressionGrammar.h"

#include "IdentifierGrammars.h"
#include "ConstIntegerExpressionGrammar.h"

namespace storm {
    namespace parser {
        namespace prism {
            
            IntegerExpressionGrammar::IntegerExpressionGrammar(std::shared_ptr<VariableState> const& state)
            : IntegerExpressionGrammar::base_type(integerExpression), BaseGrammar(state) {
                
                integerExpression %= integerPlusExpression;
                integerExpression.name("integer expression");
                
                integerPlusExpression = integerMultExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> integerMultExpression)[qi::_val = phoenix::bind(&BaseGrammar::createIntPlus, this, qi::_val, qi::_a, qi::_1)];
                integerPlusExpression.name("integer expression");
                
                integerMultExpression %= atomicIntegerExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> atomicIntegerExpression[qi::_val = phoenix::bind(&BaseGrammar::createIntMult, this, qi::_val, qi::_1)]);
                integerMultExpression.name("integer expression");
                
                atomicIntegerExpression %= (integerMinMaxExpression | integerFloorCeilExpression | qi::lit("(") >> integerExpression >> qi::lit(")") | integerVariableExpression | ConstIntegerExpressionGrammar::instance(this->state));
                atomicIntegerExpression.name("integer expression");
                
                integerMinMaxExpression = ((qi::lit("min")[qi::_a = true] | qi::lit("max")[qi::_a = false]) >> qi::lit("(") >> integerExpression >> qi::lit(",") >> integerExpression >> qi::lit(")"))[qi::_val = phoenix::bind(&BaseGrammar::createIntMinMax, this, qi::_a, qi::_1, qi::_2)];
                integerMinMaxExpression.name("integer min/max expression");
                
                integerFloorCeilExpression = ((qi::lit("floor")[qi::_a = true] | qi::lit("ceil")[qi::_a = false]) >> qi::lit("(") >> integerExpression >> qi::lit(")"))[qi::_val = phoenix::bind(&BaseGrammar::createIntFloorCeil, this, qi::_a, qi::_1)];
                integerFloorCeilExpression.name("integer floor/ceil expression");
                
                integerVariableExpression = IdentifierGrammar::instance(this->state)[qi::_val = phoenix::bind(&BaseGrammar::getIntVariable, this, qi::_1)];
                integerVariableExpression.name("integer variable");
            }
            
            void IntegerExpressionGrammar::prepareSecondRun() {
                integerVariableExpression %= this->state->integerVariables_;
                integerVariableExpression.name("integer variable");
            }
            
        }
    }
}