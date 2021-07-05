#include "storm/storage/jani/types/ContinuousType.h"

namespace storm {
    namespace jani {
        ContinuousType::ContinuousType() : JaniType() {
            // Intentionally left empty
        }

        bool ContinuousType::isContinuousType() const {
            return true;
        }

        std::string ContinuousType::getStringRepresentation() const {
            return "continuous";
        }
    }
}
