#include "storm/storage/jani/traverser/ArrayEliminator.h"


#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/storage/jani/expressions/JaniExpressionVisitor.h"

namespace storm {
    namespace jani {
        namespace detail {
/*
        

            class MaxArraySizeVisitor : public ExpressionVisitor, public JaniExpressionVisitor {
                virtual MaxArraySizeVisitor() = default;
                virtual ~MaxArraySizeVisitor() = default;
    
                std::size_t getMaxSize(Expression const& expression) {
                    return boost::any_cast<std::size_t> expression.accept(*this);
                }
     
                virtual boost::any visit(IfThenElseExpression const& expression, boost::any const& data) override {
                
                }
                
                virtual boost::any visit(BinaryBooleanFunctionExpression const& expression, boost::any const& data) override {
                
                }
                
                virtual boost::any visit(BinaryNumericalFunctionExpression const& expression, boost::any const& data) override {
                
                }
                
                virtual boost::any visit(BinaryRelationExpression const& expression, boost::any const& data) override {
                
                }
                
                virtual boost::any visit(VariableExpression const& expression, boost::any const& data) override {
                
                }
                
                virtual boost::any visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) override {
                
                }
                
                virtual boost::any visit(UnaryNumericalFunctionExpression const& expression, boost::any const& data) override {
                
                }
                
                virtual boost::any visit(BooleanLiteralExpression const& expression, boost::any const& data) override {
                
                }
                
                virtual boost::any visit(IntegerLiteralExpression const& expression, boost::any const& data) override {
                
                }
                
                virtual boost::any visit(RationalLiteralExpression const& expression, boost::any const& data) override {
                
                }
                
                
                virtual boost::any visit(ValueArrayExpression const& expression, boost::any const& data) override {
                
                }
                
                virtual boost::any visit(ConstructorArrayExpression const& expression, boost::any const& data) override {
                
                }
                
                virtual boost::any visit(ArrayAccessExpression const& expression, boost::any const& data) override {
                
                }
            };
        
            class ArrayVariableReplacer : public JaniTraverser {
            public:

                ArrayVariableReplacer() = default;
                virtual ~ArrayVariableReplacer() = default;
                std::unordered_map<storm::expressions::Variable, std::vector<reference_wrapper<storm::jani::Variable>> replace();

                virtual void traverse(Assignment const& assignment, boost::any const& data) const override;
                
            private:
                void std::unordered_map<storm::expressions::Variable, std::size_t>::getMaxSizes(Model& model);
                
            };
            
            class ArrayAccessLValueReplacer : public JaniTraverser {
            }
           */
        }
        
        ArrayEliminator::eliminate(Model& model) {
            //auto variableReplacements ArrayVariableReplacer().replace(model)
        
        }
    }
}

