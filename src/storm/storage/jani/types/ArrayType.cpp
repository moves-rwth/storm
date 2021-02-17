#include "storm/storage/jani/types/ArrayType.h"

namespace storm {
    namespace jani {
        ArrayType::ArrayType(JaniType const& childType) : JaniType(), childType(childType){
            // Intentionally left empty
        }

        bool ArrayType::isArrayType() const {
            return true;
        }

        JaniType const& ArrayType::getChildType() const {
            return childType;
        }
    }
}