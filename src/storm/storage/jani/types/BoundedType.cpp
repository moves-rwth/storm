#include "storm/storage/jani/types/BoundedType.h"

namespace storm {
    namespace jani {
        BoundedType::BoundedType(const ElementType &type, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& expression upperBound) : JaniType(), type(type), lowerBound(lowerBound), upperBound(upperBound) {
            STORM_LOG_ASSERT(isIntegerType() || isRealType(), "Wrong type for BoundedType, expecting int or real");
            // Intentionally left empty
        }

        bool BoundedType::isBoundedType() const {
            return true;
        }

        bool BoundedType::isIntegerType() const {
            return type == ElementType::Int;
        }

        bool BoundedType::isRealType() const {
            return type == ElementType::Real;
        }

        void BoundedType::setLowerBound(storm::expressions::Expression const& expression) {
            this->lowerBound = expression;
        }

        void BoundedType::setUpperBound(storm::expressions::Expression const& expression) {
            this->upperBound = expression;
        }

        storm::expressions::Expression const& BoundedType::getLowerBound() {
            return this->lowerBound;
        }

        storm::expressions::Expression const& BoundedType::getUpperBound() {
            return this->upperBound;
        }

        std::string BoundedType::getStringRepresentation() const {
            switch (type) {
                case ElementType::Real:
                    return "bounded real";
                case ElementType::Int:
                    return "bounded int";
            }
        }
    }
}