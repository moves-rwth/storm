#pragma once

#include "JaniType.h"

namespace storm {
    namespace jani {
        class ArrayType : public JaniType {
        public:
            ArrayType(JaniType const& childType);
            bool isArrayType() const override;
            JaniType const& getChildType() const override;

        private:
            JaniType childType;

        };
    }
}

