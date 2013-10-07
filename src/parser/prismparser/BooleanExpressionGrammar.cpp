#include "BooleanExpressionGrammar.h"

#include "IntegerExpressionGrammar.h"
#include "ConstBooleanExpressionGrammar.h"

namespace storm {
    namespace parser {
        namespace prism {
            
            BooleanExpressionGrammar::BooleanExpressionGrammar(std::shared_ptr<VariableState> const& state)
            : BooleanExpressionGrammar::base_type(booleanExpression), BaseGrammar(state) {
                
                booleanExpression %= orExpression;
                booleanExpression.name("boolean expression");
                
                orExpression = andExpression[qi::_val = qi::_1] >> *(qi::lit("|") >> andExpression)[qi::_val = phoenix::bind(&BaseGrammar::createOr, this, qi::_val, qi::_1)];
                orExpression.name("boolean expression");
                
                andExpression = notExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> notExpression)[qi::_val = phoenix::bind(&BaseGrammar::createAnd, this, qi::_val, qi::_1)];
                andExpression.name("boolean expression");
                
                notExpression = atomicBooleanExpression[qi::_val = qi::_1] | (qi::lit("!") >> atomicBooleanExpression)[qi::_val = phoenix::bind(&BaseGrammar::createNot, this, qi::_1)];
                notExpression.name("boolean expression");
                
                atomicBooleanExpression %= (relativeExpression | booleanVariableExpression | qi::lit("(") >> booleanExpression >> qi::lit(")") | ConstBooleanExpressionGrammar::instance(this->state));
                atomicBooleanExpression.name("boolean expression");
                
                relativeExpression = (IntegerExpressionGrammar::instance(this->state) >> relations_ >> IntegerExpressionGrammar::instance(this->state))[qi::_val = phoenix::bind(&BaseGrammar::createRelation, this, qi::_1, qi::_2, qi::_3)];
                relativeExpression.name("relative expression");
                
                booleanVariableExpression = IdentifierGrammar::instance(this->state)[qi::_val = phoenix::bind(&BaseGrammar::getBoolVariable, this, qi::_1)];
                booleanVariableExpression.name("boolean variable");
            }
            
            void BooleanExpressionGrammar::prepareSecondRun() {
                booleanVariableExpression %= this->state->booleanVariables_;
                booleanVariableExpression.name("boolean variable");
            }
            
        } // namespace prism
    } // namespace parser
} // namespace storm