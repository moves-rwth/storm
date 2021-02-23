#include "storm/storage/jani/types/ArrayType.h"

namespace storm {
    namespace jani {
        ArrayType::ArrayType(JaniType const* childType) : JaniType(), childType(childType){
            // Intentionally left empty
        }

        bool ArrayType::isArrayType() const {
            return true;
        }

        bool ArrayType::isBoundedType() const {
            return childType->isBoundedType();
        }

        JaniType const* ArrayType::getChildType() const {
            return childType;
        }

        std::string ArrayType::getStringRepresentation() const {
            return "array[" + getChildType()->getStringRepresentation() + "]";
        }
    }
}