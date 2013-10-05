/*
 * BinaryRelationExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_BINARYRELATIONEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_BINARYRELATIONEXPRESSION_H_

#include "src/ir/expressions/BinaryExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a binary relation expression of boolean type.
             */
            class BinaryRelationExpression : public BinaryExpression {
            public:
                /*!
                 * An enum type specifying the different relations applicable.
                 */
                enum RelationType {EQUAL, NOT_EQUAL, LESS, LESS_OR_EQUAL, GREATER, GREATER_OR_EQUAL};
                
                /*!
                 * Creates a binary relation expression tree node with the given children and relation type.
                 *
                 * @param left The left child of the binary expression.
                 * @param right The right child of the binary expression.
                 * @param relationType The type of the relation associated with this node.
                 */
                BinaryRelationExpression(std::shared_ptr<BaseExpression> const& left, std::shared_ptr<BaseExpression> const& right, RelationType relationType);
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param binaryRelationExpression The expression to copy.
                 */
                BinaryRelationExpression(BinaryRelationExpression const& binaryRelationExpression);
                
                virtual std::shared_ptr<BaseExpression> clone() const override;
                
                virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const override;
                
                virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                /*!
                 * Retrieves the relation that is associated with this node.
                 *
                 * @param The relation that is associated with this node.
                 */
                RelationType getRelationType() const;
                
                virtual void accept(ExpressionVisitor* visitor) override;
                
                virtual std::string toString() const override;
                
            private:
                // The relation operator associated with this node.
                RelationType relationType;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BINARYRELATIONEXPRESSION_H_ */
