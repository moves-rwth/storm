#pragma once

#include "storm/utility/macros.h"

namespace storm::dft {
namespace storage {
namespace elements {

/*!
 * Element types in a DFT.
 */
enum class DFTElementType { BE, AND, OR, VOT, PAND, POR, SPARE, PDEP, SEQ, MUTEX };

/*!
 * BE types in a DFT.
 */
enum class BEType { CONSTANT, PROBABILITY, EXPONENTIAL, ERLANG, WEIBULL, LOGNORMAL, SAMPLES };

inline std::string toString(DFTElementType const& type) {
    switch (type) {
        case DFTElementType::BE:
            return "BE";
        case DFTElementType::AND:
            return "AND";
        case DFTElementType::OR:
            return "OR";
        case DFTElementType::VOT:
            return "VOT";
        case DFTElementType::PAND:
            return "PAND";
        case DFTElementType::POR:
            return "POR";
        case DFTElementType::SPARE:
            return "SPARE";
        case DFTElementType::PDEP:
            return "PDEP";
        case DFTElementType::SEQ:
            return "SEQ";
        case DFTElementType::MUTEX:
            return "MUTEX";
        default:
            STORM_LOG_ASSERT(false, "DFT type not known.");
            return "";
    }
}

inline std::string toString(BEType const& type) {
    switch (type) {
        case BEType::CONSTANT:
            return "CONST";
        case BEType::PROBABILITY:
            return "PROBABILITY";
        case BEType::EXPONENTIAL:
            return "EXPONENTIAL";
        case BEType::ERLANG:
            return "ERLANG";
        case BEType::WEIBULL:
            return "WEIBULL";
        case BEType::LOGNORMAL:
            return "LOGNORMAL";
        case BEType::SAMPLES:
            return "SAMPLES";
        default:
            STORM_LOG_ASSERT(false, "BE type not known.");
            return "";
    }
}

inline std::ostream& operator<<(std::ostream& os, DFTElementType const& type) {
    return os << toString(type);
}

inline std::ostream& operator<<(std::ostream& os, BEType const& type) {
    return os << toString(type);
}

}  // namespace elements
}  // namespace storage
}  // namespace storm::dft
