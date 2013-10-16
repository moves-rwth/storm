#include "ConstIntegerExpressionGrammar.h"

namespace storm {
    namespace parser {
        namespace prism {
            
            
            ConstIntegerExpressionGrammar::ConstIntegerExpressionGrammar(std::shared_ptr<VariableState> const& state)
            : ConstIntegerExpressionGrammar::base_type(constantIntegerExpression), BaseGrammar(state) {
                
                constantIntegerExpression %= constantIntegerPlusExpression;
                constantIntegerExpression.name("constant integer expression");
                
                constantIntegerPlusExpression = constantIntegerMultExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> constantIntegerMultExpression)
                [qi::_val = phoenix::bind(&BaseGrammar::createIntPlus, this, qi::_val, qi::_a, qi::_1)];
                constantIntegerPlusExpression.name("constant integer expression");
                
                constantIntegerMultExpression %= constantAtomicIntegerExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> constantAtomicIntegerExpression)
                [qi::_val = phoenix::bind(&BaseGrammar::createIntMult, this, qi::_val, qi::_1)];
                constantIntegerMultExpression.name("constant integer expression");
                
                constantAtomicIntegerExpression %= (constantIntegerMinMaxExpression | constantIntegerFloorCeilExpression | qi::lit("(") >> constantIntegerExpression >> qi::lit(")") | integerConstantExpression);
                constantAtomicIntegerExpression.name("constant integer expression");
                
                constantIntegerMinMaxExpression = ((qi::lit("min")[qi::_a = true] | qi::lit("max")[qi::_a = false]) >> qi::lit("(") >> constantIntegerExpression >> qi::lit(",") >> constantIntegerExpression >> qi::lit(")"))[qi::_val = phoenix::bind(&BaseGrammar::createIntMinMax, this, qi::_a, qi::_1, qi::_2)];
                constantIntegerMinMaxExpression.name("integer min/max expression");
                
                constantIntegerFloorCeilExpression = ((qi::lit("floor")[qi::_a = true] | qi::lit("ceil")[qi::_a = false]) >> qi::lit("(") >> constantIntegerExpression >> qi::lit(")"))[qi::_val = phoenix::bind(&BaseGrammar::createIntFloorCeil, this, qi::_a, qi::_1)];
                constantIntegerFloorCeilExpression.name("integer floor/ceil expression");
                
                integerConstantExpression %= (this->state->integerConstants_ | integerLiteralExpression);
                integerConstantExpression.name("integer constant or literal");
                
                integerLiteralExpression = qi::int_[qi::_val = phoenix::bind(&BaseGrammar::createIntLiteral, this, qi::_1)];
                integerLiteralExpression.name("integer literal");
                
            }
            
        } // namespace prism
    } // namespace parser
} // namespace storm