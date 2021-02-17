#pragma once

#include "JaniType.h"

namespace storm {
    namespace jani {
        class BasicType : public JaniType {

        public:
            BasicType (ElementType const& type, bool bounded);
            virtual bool isBoundedType() const override;
            virtual bool isBooleanType() const override;
            virtual bool isIntegerType() const override;
            virtual bool isRealType() const override;
        private:
            ElementType type;
            bool bounded;
        };
    }
}
