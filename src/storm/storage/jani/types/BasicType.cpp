#include "storm/storage/jani/types/BasicType.h"

namespace storm {
    namespace jani {
        BasicType::BasicType(const Type &type) : JaniType(), type(type) {
            // Intentionally left empty
        }

        bool BasicType::isBasicType() const {
            return true;
        }

        BasicType::Type const& BasicType::get() const {
            return type;
        }
        
        bool BasicType::isBooleanType() const {
            return type == Type::Bool;
        }

        bool BasicType::isIntegerType() const {
            return type == Type::Int;
        }

        bool BasicType::isRealType() const {
            return type == Type::Real;
        }

        bool BasicType::isNumericalType() const {
            return isIntegerType() || isRealType();
        }

        std::string BasicType::getStringRepresentation() const {
            switch (type) {
                case Type::Bool:
                    return "bool";
                case Type::Int:
                    return "int";
                case Type::Real:
                    return "real";
            }
        }
        
        std::unique_ptr<JaniType> BasicType::clone() const {
            return std::make_unique<BasicType>(*this);
        }
        
    }
}