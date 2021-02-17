#include "storm/storage/jani/types/BasicType.h"

namespace storm {
    namespace jani {
        BasicType::BasicType(const ElementType &type, bool bounded) : JaniType(), type(type), bounded(bounded) {
            // Intentionally left empty
        }

        bool BasicType::isBoundedType() const {
            return bounded;
        }

        bool BasicType::isBooleanType() const {
            return type == ElementType::Bool;
        }

        bool BasicType::isIntegerType() const {
            return type == ElementType::Int;
        }

        bool BasicType::isRealType() const {
            return type == ElementType::Real;
        }
    }
}