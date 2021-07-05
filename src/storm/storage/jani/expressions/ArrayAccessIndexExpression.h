#pragma once

#include "storm/storage/expressions/BinaryExpression.h"

namespace storm {
    namespace expressions {
        /*!
         * Represents an access to an array.
         */
        class ArrayAccessIndexExpression : public BinaryExpression {
        public:
            
            ArrayAccessIndexExpression(ExpressionManager const& manager, Type const& type, std::shared_ptr<BaseExpression const> const& lhs, std::shared_ptr<BaseExpression const> const& rhs);
            ArrayAccessIndexExpression(ExpressionManager const& manager, Type const& type, std::shared_ptr<BaseExpression const> const& onehs);        // This one is for the last array index

            // Instantiate constructors and assignments with their default implementations.
            ArrayAccessIndexExpression(ArrayAccessIndexExpression const& other) = default;
            ArrayAccessIndexExpression& operator=(ArrayAccessIndexExpression const& other) = delete;
            ArrayAccessIndexExpression(ArrayAccessIndexExpression&&) = default;
            ArrayAccessIndexExpression& operator=(ArrayAccessIndexExpression&&) = delete;

            virtual ~ArrayAccessIndexExpression() = default;

            virtual std::shared_ptr<BaseExpression const> simplify() const override;
            virtual boost::any accept(ExpressionVisitor& visitor, boost::any const& data) const override;
            
        protected:
            virtual void printToStream(std::ostream& stream) const override;
            

        };
    }
}
