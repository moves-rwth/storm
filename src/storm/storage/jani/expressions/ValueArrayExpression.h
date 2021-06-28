#pragma once

#include "storm/storage/jani/expressions/ArrayExpression.h"
#include <boost/optional.hpp>
namespace storm {
    namespace expressions {
        /*!
         * Represents an array with a given list of elements.
         */
        class ValueArrayExpression : public ArrayExpression {
        public:
            struct ValueArrayElements {
                boost::optional<std::vector<std::shared_ptr<BaseExpression const>>> elementsWithValue = boost::none;
                boost::optional<std::vector<std::shared_ptr<ValueArrayElements const>>> elementsOfElements = boost::none;
                std::vector<size_t> getSizes() const;
            };
            
            ValueArrayExpression(ExpressionManager const& manager, Type const& type);
            ValueArrayExpression(ExpressionManager const& manager, Type const& type, ValueArrayElements elements);

            // Instantiate constructors and assignments with their default implementations.
            ValueArrayExpression(ValueArrayExpression const& other) = default;
            ValueArrayExpression& operator=(ValueArrayExpression const& other) = delete;
            ValueArrayExpression(ValueArrayExpression&&) = default;
            ValueArrayExpression& operator=(ValueArrayExpression&&) = delete;

            virtual ~ValueArrayExpression() = default;

            virtual void gatherVariables(std::set<storm::expressions::Variable>& variables) const override;
            virtual bool containsVariables() const override;
            virtual std::shared_ptr<BaseExpression const> simplify() const override;
            virtual boost::any accept(ExpressionVisitor& visitor, boost::any const& data) const override;
            
            // Returns the size of the array
            virtual std::shared_ptr<BaseExpression const> size() const override;
            std::vector<size_t> getSizes() const;
            ValueArrayElements const& getElements() const;

            // Returns the element at position i
            virtual std::shared_ptr<BaseExpression const> at(std::vector<uint64_t>& i) const override;
            virtual std::shared_ptr<BaseExpression const> at(uint64_t i) const override;

        protected:
            virtual void printToStream(std::ostream& stream) const override;
            
        private:
            ValueArrayElements simplify(ValueArrayElements const& elements) const;
            bool containsVariables(ValueArrayElements const& elementsToCheck) const;
            size_t size(ValueArrayElements const& elementsToCheck) const;
            void gatherVariables(std::set<storm::expressions::Variable>& variables, ValueArrayElements const& elementsToCheck) const;
            void printToStream(std::ostream& stream, ValueArrayElements const& element) const;

            ValueArrayElements const elements;
        };
    }
}