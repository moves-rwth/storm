#include "storm/storage/jani/types/ClockType.h"

namespace storm {
    namespace jani {
        ClockType::ClockType() : JaniType() {
            // Intentionally left empty
        }

        bool ClockType::isClockType() const {
            return true;
        }

        std::string ClockType::getStringRepresentation() const {
            return "clock";
        }
    }
}