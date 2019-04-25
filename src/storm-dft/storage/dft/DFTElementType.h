#pragma  once

#include "storm/utility/macros.h"

namespace storm {
    namespace storage {

        /*!
         * Element types in a DFT.
         */
        enum class DFTElementType {
            BE_EXP, BE_CONST,
            AND, OR, VOT,
            BE,
            PAND,
            POR,
            SPARE,
            PDEP,
            SEQ,
            MUTEX
        };

        inline bool isGateType(DFTElementType const& type) {
            switch (type) {
                case DFTElementType::AND:
                case DFTElementType::OR:
                case DFTElementType::VOT:
                case DFTElementType::PAND:
                case DFTElementType::POR:
                case DFTElementType::SPARE:
                    return true;
                case DFTElementType::BE_EXP:
                case DFTElementType::BE_CONST:
                case DFTElementType::PDEP:
                case DFTElementType::SEQ:
                case DFTElementType::MUTEX:
                    return false;
                default:
                    STORM_LOG_ASSERT(false, "DFT type not known.");
                    return false;
            }
        }

        inline bool isStaticGateType(DFTElementType const& type) {
            if (!isGateType(type)) {
                return false;
            }
            switch (type) {
                case DFTElementType::AND:
                case DFTElementType::OR:
                case DFTElementType::VOT:
                    return true;
                case DFTElementType::PAND:
                case DFTElementType::POR:
                case DFTElementType::SPARE:
                    return false;
                default:
                    STORM_LOG_ASSERT(false, "DFT gate type not known.");
                    return false;
            }
        }

        inline std::string toString(DFTElementType const& tp) {
            switch (tp) {
                case DFTElementType::BE_EXP:
                    return "BE_EXP";
                case DFTElementType::BE_CONST:
                    return "BE_CONST";
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

        inline std::ostream& operator<<(std::ostream& os, DFTElementType const& type) {
            return os << toString(type);
        }

    }
}
