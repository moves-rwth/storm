#include "storm/storage/jani/types/BasicType.h"

namespace storm {
    namespace jani {
        BasicType::BasicType(const ElementType &type) : JaniType(), type(type) {
            // Intentionally left empty
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

        std::string BasicType::getStringRepresentation() const {
            switch (type) {
                case ElementType::Real:
                    return "real";
                case ElementType::Bool:
                    return "bool";
                case ElementType::Int:
                    return "int";
            }
        }
    }
}