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

        void JaniType::setLowerBound(storm::expressions::Expression const& expression) {
            STORM_LOG_ASSERT(false, "Trying to set lowerbound for not bounded variable");
        }

        void JaniType::setUpperBound(storm::expressions::Expression const& expression) {
            STORM_LOG_ASSERT(false, "Trying to set lowerbound for not bounded variable");
        }

        storm::expressions::Expression const&  JaniType::getLowerBound() const{
            STORM_LOG_ASSERT(false, "Trying to get lowerbound for not bounded variable");
        }

        storm::expressions::Expression const& JaniType::getUpperBound() const {
            STORM_LOG_ASSERT(false, "Trying to get lowerbound for not bounded variable");
        }

        JaniType* JaniType::getChildType() const {
            assert (false);
            return nullptr;
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
