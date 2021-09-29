#include "storm/storage/jani/types/ArrayType.h"

namespace storm {
    namespace jani {
        ArrayType::ArrayType(JaniType const& baseType) : ArrayType(baseType.clone()) {
            // Intentionally left empty
        }
        
        ArrayType::ArrayType(std::unique_ptr<JaniType>&& baseType) : baseType(std::move(baseType)) {
            // Intentionally left empty
        }

        bool ArrayType::isArrayType() const {
            return true;
        }

        JaniType const& ArrayType::getBaseType() const {
            return *baseType;
        }

        std::string ArrayType::getStringRepresentation() const {
            return "array[" + getBaseType().getStringRepresentation() + "]";
        }
        
        void ArrayType::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            JaniType::substitute(substitution);
            baseType->substitute(substitution);
        }
        
        std::unique_ptr<JaniType> ArrayType::clone() const {
            return std::make_unique<ArrayType>(baseType->clone());
        }
        
    }
}