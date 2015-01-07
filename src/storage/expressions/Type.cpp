#include "src/storage/expressions/Type.h"

#include <sstream>

#include "src/storage/expressions/ExpressionManager.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        
        BaseType::BaseType() {
            // Intentionally left empty.
        }
        
        bool BaseType::operator==(BaseType const& other) const {
            return this->getMask() == other.getMask();
        }
        
        bool BaseType::isBooleanType() const {
            return false;
        }
        
        bool BooleanType::isBooleanType() const {
            return true;
        }
        
        bool BaseType::isIntegerType() const {
            return false;
        }
        
        bool IntegerType::isIntegerType() const {
            return true;
        }
        
        bool BaseType::isBoundedIntegerType() const {
            return false;
        }
        
        bool BoundedIntegerType::isBoundedIntegerType() const {
            return true;
        }
        
        bool BaseType::isRationalType() const {
            return false;
        }
        
        bool RationalType::isRationalType() const {
            return true;
        }
        
        uint64_t BooleanType::getMask() const {
            return BooleanType::mask;
        }
        
        std::string BooleanType::getStringRepresentation() const {
            return "bool";
        }

        uint64_t IntegerType::getMask() const {
            return IntegerType::mask;
        }

        std::string IntegerType::getStringRepresentation() const {
            return "int";
        }
        
        BoundedIntegerType::BoundedIntegerType(std::size_t width) : width(width) {
            // Intentionally left empty.
        }
        
        uint64_t BoundedIntegerType::getMask() const {
            return BoundedIntegerType::mask;
        }
        
        std::string BoundedIntegerType::getStringRepresentation() const {
            return "int[" + std::to_string(width) + "]";
        }

        std::size_t BoundedIntegerType::getWidth() const {
            return width;
        }
        
        bool BoundedIntegerType::operator==(BaseType const& other) const {
            return this->getMask() == other.getMask() && this->width == static_cast<BoundedIntegerType const&>(other).width;
        }

        uint64_t RationalType::getMask() const {
            return RationalType::mask;
        }
        
        std::string RationalType::getStringRepresentation() const {
            return "rational";
        }
        
        Type::Type() : manager(nullptr), innerType(nullptr) {
            // Intentionally left empty.
        }
        
        Type::Type(std::shared_ptr<ExpressionManager const> const& manager, std::shared_ptr<BaseType> const& innerType) : manager(manager), innerType(innerType) {
            // Intentionally left empty.
        }
        
        bool Type::operator==(Type const& other) const {
            return *this->innerType == *other.innerType;
        }
        
        uint64_t Type::getMask() const {
            return this->innerType->getMask();
        }
        
        std::string Type::getStringRepresentation() const {
            return this->innerType->getStringRepresentation();
        }
        
        bool Type::isNumericalType() const {
            return this->isIntegerType() || this->isRationalType();
        }
        
        bool Type::isIntegerType() const {
            return this->isUnboundedIntegerType() || this->isBoundedIntegerType();
        }
        
        bool Type::isBooleanType() const {
            return this->innerType->isBooleanType();
        }
        
        bool Type::isUnboundedIntegerType() const {
            return this->innerType->isIntegerType();
        }
        
        bool Type::isBoundedIntegerType() const {
            return this->innerType->isBoundedIntegerType();
        }
        
        std::size_t Type::getWidth() const {
            return dynamic_cast<BoundedIntegerType const&>(*this->innerType).getWidth();
        }

        bool Type::isRationalType() const {
            return this->innerType->isRationalType();
        }
        
        storm::expressions::ExpressionManager const& Type::getManager() const {
            return *manager;
        }
        
        Type Type::plusMinusTimes(Type const& other) const {
            STORM_LOG_THROW(this->isNumericalType() && other.isNumericalType(), storm::exceptions::InvalidTypeException, "Operator requires numerical operands.");
            if (this->isRationalType() || other.isRationalType()) {
                return this->getManager().getRationalType();
            }
            return getManager().getIntegerType();
        }
        
        Type Type::minus() const {
            STORM_LOG_THROW(this->isNumericalType(), storm::exceptions::InvalidTypeException, "Operator requires numerical operand.");
            return *this;
        }
        
        Type Type::divide(Type const& other) const {
            STORM_LOG_THROW(this->isNumericalType() && other.isNumericalType(), storm::exceptions::InvalidTypeException, "Operator requires numerical operands.");
            if (this->isRationalType() || other.isRationalType()) {
                return this->getManager().getRationalType();
            }
            return this->getManager().getIntegerType();
        }
        
        Type Type::power(Type const& other) const {
            STORM_LOG_THROW(this->isNumericalType() && other.isNumericalType(), storm::exceptions::InvalidTypeException, "Operator requires numerical operands.");
            if (this->isRationalType() || other.isRationalType()) {
                return getManager().getRationalType();
            }
            return this->getManager().getIntegerType();
        }
        
        Type Type::logicalConnective(Type const& other) const {
            STORM_LOG_THROW(this->isBooleanType() && other.isBooleanType(), storm::exceptions::InvalidTypeException, "Operator requires boolean operands.");
            return *this;
        }
        
        Type Type::logicalConnective() const {
            STORM_LOG_THROW(this->isBooleanType(), storm::exceptions::InvalidTypeException, "Operator requires boolean operand.");
            return *this;
        }
        
        Type Type::numericalComparison(Type const& other) const {
            STORM_LOG_THROW(this->isNumericalType() && other.isNumericalType(), storm::exceptions::InvalidTypeException, "Operator requires numerical operands.");
            return this->getManager().getBooleanType();
        }
        
        Type Type::ite(Type const& thenType, Type const& elseType) const {
            STORM_LOG_THROW(this->isBooleanType(), storm::exceptions::InvalidTypeException, "Operator requires boolean condition.");
            if (thenType == elseType) {
                return thenType;
            } else {
                STORM_LOG_THROW(thenType.isNumericalType() == elseType.isNumericalType(), storm::exceptions::InvalidTypeException, "Operator 'ite' requires proper types.");
                if (thenType.isRationalType() || elseType.isRationalType()) {
                    return this->getManager().getRationalType();
                } else {
                    return this->getManager().getIntegerType();
                }
            }
            return thenType;
        }
        
        Type Type::floorCeil() const {
            STORM_LOG_THROW(this->isNumericalType(), storm::exceptions::InvalidTypeException, "Operator requires rational operand.");
            return this->getManager().getIntegerType();
        }
        
        Type Type::minimumMaximum(Type const& other) const {
            STORM_LOG_THROW(this->isNumericalType() && other.isNumericalType(), storm::exceptions::InvalidTypeException, "Operator requires numerical operands.");
            if (this->isRationalType() || other.isRationalType()) {
                return this->getManager().getRationalType();
            }
            return this->getManager().getIntegerType();
        }
        
        std::ostream& operator<<(std::ostream& stream, Type const& type) {
            stream << type.getStringRepresentation();
            return stream;
        }
    }
}