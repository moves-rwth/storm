#pragma once

#include "JaniType.h"

namespace storm {
    namespace jani {
        class ArrayType : public JaniType {
        public:
            ArrayType(JaniType const* childType);
            bool isArrayType() const override;
            bool isBoundedType() const override;
            JaniType const* getChildType() const override;
            std::string getStringRepresentation() const override;

        private:
            JaniType const* childType;

        };
    }
}

