#pragma once

#include <cstdint>
#include <string>
#include <boost/optional.hpp>
#include "storm/utility/macros.h"
#include <iostream>
#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace jani {
        class JaniType {
            class ArrayType;
            class BasicType;
            class BoundedType;
            class ClockType;
            class ContinuousType;


            // TODO: what to do for delete
            public:
                enum class ElementType {Bool, Int, Real};
                JaniType();
                virtual bool isBoundedType() const;
                virtual bool isBooleanType() const;
                virtual bool isIntegerType() const;
                virtual bool isRealType() const;
                virtual bool isArrayType() const;
                virtual bool isClockType() const;
                virtual bool isContinuousType() const;
                virtual JaniType* getChildType() const;
                // TODO: fix the asXXXType things, such that this becomes superfluous
                virtual void setLowerBound(storm::expressions::Expression const& expression);
                virtual void setUpperBound(storm::expressions::Expression const& expression);
                virtual storm::expressions::Expression const& getLowerBound() const;
                virtual storm::expressions::Expression const& getUpperBound() const;
            /*!
             * Retrieves a string representation of the type.
             *
             * @return A string representation of the type.
             */
                virtual std::string getStringRepresentation() const;

                friend std::ostream& operator<<(std::ostream& stream, JaniType const& type);


            private:

        };
    }
}
