#include <cstdint>
#include <string>
#include <boost/optional.hpp>

#include "storm/storage/expressions/Variable.h"
#include "storm/storage/jani/types/AllJaniTypes.h"

namespace storm {
    namespace jani {
        JaniType::JaniType() {
           // Intentionally left empty
        }

        bool JaniType::isBoundedType() const {
            return false;
        }

        bool JaniType::isBooleanType() const {
            return false;
        }

        bool JaniType::isIntegerType() const {
            return false;
        }

        bool JaniType::isRealType() const {
            return false;
        }

        bool JaniType::isArrayType() const {
            return false;
        }

        bool JaniType::isClockType() const {
            return false;
        }

        bool JaniType::isContinuousType() const {
            return false;
        }

        void BoundedType::setLowerBound(storm::expressions::Expression const& expression) {
            STORM_LOG_ASSERT(false, "Trying to set lowerbound for not bounded variable");
        }

        void BoundedType::setUpperBound(storm::expressions::Expression const& expression) {
            STORM_LOG_ASSERT(false, "Trying to set lowerbound for not bounded variable");
        }

        storm::expressions::Expression const& BoundedType::getLowerBound() {
            return storm::expressions::Expression();
        }

        storm::expressions::Expression const& BoundedType::getUpperBound() {
            return storm::expressions::Expression();
        }
//
//        BasicType& JaniType::asBasicType() {
//            return static_cast<BasicType&>(*this);
//        }
//
//        BasicType const& JaniType::asBasicType() const {
//            return static_cast<BasicType& const>(*this);
//        }
//
//        BoundedType& JaniType::asBoundedType() {
//            return static_cast<BoundedType&>(*this);
//        }
//
//        BoundedType const& JaniType::asBoundedType() const {
//            return static_cast<BoundedType& const>(*this);
//        }
//
//        ArrayType& JaniType::asArrayType() {
//            return static_cast<ArrayType&>(*this);
//        }
//
//        ArrayType const& JaniType::asArrayType() const {
//            return static_cast<ArrayType& const>(*this);
//        }
//
//        ClockType& JaniType::asClockType() {
//            return static_cast<ClockType&>(*this);
//        }
//
//        ClockType const& JaniType::asClockType() const {
//            return static_cast<ClockType& const>(*this);
//        }
//
//        ContinuousType& JaniType::asContinuousType() {
//            return static_cast<ContinuousType&>(*this);
//        }
//
//        ContinuousType const& JaniType::asContinuousType() const {
//            return static_cast<ContinuousType& const>(*this);
//        }

        JaniType const* JaniType::getChildType() const {
            assert (false);
            return this;
        }

        std::string JaniType::getStringRepresentation() const {
            return "";
        }

        std::ostream& operator<<(std::ostream& stream, JaniType const& type) {
            stream << type.getStringRepresentation();
            return stream;
        }
    }
}
