#pragma once

#include "storm/utility/macros.h"

namespace storm::dft {
namespace storage {

enum class DFTElementState { Operational = 0, Failed = 2, Failsafe = 1, DontCare = 3 };

inline std::ostream& operator<<(std::ostream& os, DFTElementState st) {
    switch (st) {
        case DFTElementState::Operational:
            return os << "Operational";
        case DFTElementState::Failed:
            return os << "Failed";
        case DFTElementState::Failsafe:
            return os << "Failsafe";
        case DFTElementState::DontCare:
            return os << "Don't Care";
        default:
            STORM_LOG_ASSERT(false, "Element state not known.");
            return os;
    }
}

inline char toChar(DFTElementState st) {
    switch (st) {
        case DFTElementState::Operational:
            return 'O';
        case DFTElementState::Failed:
            return 'F';
        case DFTElementState::Failsafe:
            return 'S';
        case DFTElementState::DontCare:
            return '-';
        default:
            STORM_LOG_ASSERT(false, "Element state not known.");
            return ' ';
    }
}

enum class DFTDependencyState { Passive = 0, Unsuccessful = 1, Successful = 2, DontCare = 3 };

inline std::ostream& operator<<(std::ostream& os, DFTDependencyState st) {
    switch (st) {
        case DFTDependencyState::Passive:
            return os << "Passive";
        case DFTDependencyState::Successful:
            return os << "Successful";
        case DFTDependencyState::Unsuccessful:
            return os << "Unsuccessful";
        case DFTDependencyState::DontCare:
            return os << "Don't Care";
        default:
            STORM_LOG_ASSERT(false, "Element state not known.");
            return os;
    }
}

inline char toChar(DFTDependencyState st) {
    switch (st) {
        case DFTDependencyState::Passive:
            return 'P';
        case DFTDependencyState::Successful:
            return 'S';
        case DFTDependencyState::Unsuccessful:
            return 'U';
        case DFTDependencyState::DontCare:
            return '-';
        default:
            STORM_LOG_ASSERT(false, "Element state not known.");
            return ' ';
    }
}

}  // namespace storage
}  // namespace storm::dft
