#pragma once

#include <cstdint>
#include <string>
#include <boost/optional.hpp>
#include "storm/utility/macros.h"

namespace storm {
    namespace jani {
        class JaniType {
            class ArrayType;
            class BasicType;
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
                virtual JaniType const& getChildType() const;


//                BasicType& asBasicType();
//                BasicType const& asBasicType() const;
//                BoundedType& asBoundedType();
//                BoundedType const& asBoundedType() const;
//                ArrayType& asArrayType();
//                ArrayType const& asArrayType() const;
//                ClockType& asClockType();
//                ClockType const& asClockType() const;
//                ContinuousType& asContinuousType();
//                ContinuousType const& asContinuousType() const;

            private:

        };
    }
}
