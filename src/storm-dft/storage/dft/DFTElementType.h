#pragma  once

#include "storm/utility/macros.h"

namespace storm {
    namespace storage {

        enum class DFTElementType : int {AND = 0, OR = 2, VOT = 3, BE = 4, CONSTF = 5, CONSTS = 6, PAND = 7, SPARE = 8, POR = 9, PDEP = 10, SEQ = 11, MUTEX=12};

        inline bool isGateType(DFTElementType const& tp) {
            switch(tp) {
                case DFTElementType::AND:
                case DFTElementType::OR:
                case DFTElementType::VOT:
                case DFTElementType::PAND:
                case DFTElementType::SPARE:
                case DFTElementType::POR:
                    return true;
                case DFTElementType::SEQ:
                case DFTElementType::MUTEX:
                case DFTElementType::BE:
                case DFTElementType::CONSTF:
                case DFTElementType::CONSTS:
                case DFTElementType::PDEP:
                    return false;
                default:
                    STORM_LOG_ASSERT(false, "Dft type not known.");
                    return false;
            }
        }

        inline bool isStaticGateType(DFTElementType const& tp) {
            if(!isGateType(tp)) return false;
            switch(tp) {
                case DFTElementType::AND:
                case DFTElementType::OR:
                case DFTElementType::VOT:
                    return true;
                case DFTElementType::POR:
                case DFTElementType::SPARE:
                case DFTElementType::PAND:
                    return false;
                default:
                    STORM_LOG_ASSERT(false, "Dft gate type not known.");
                    return false;
            }
        }

        inline std::string toString(DFTElementType const& tp) {
            switch(tp) {
                case DFTElementType::BE:
                    return "BE";
                case DFTElementType::OR:
                    return "OR";
                case DFTElementType::AND:
                    return "AND";
                case DFTElementType::VOT:
                    return "VOT";
                case DFTElementType::POR:
                    return "POR";
                case DFTElementType::PAND:
                    return "PAND";
                case DFTElementType::SPARE:
                    return "SPARE";
                case DFTElementType::SEQ:
                    return "SEQ";
                case DFTElementType::MUTEX:
                    return "MUTEX";
                case DFTElementType::PDEP:
                    return "PDEP";
                default:
                    STORM_LOG_ASSERT(false, "Dft type not known.");
                    return "";
            }
        }

        inline std::ostream& operator<<(std::ostream& os, DFTElementType const& tp) {
            return os << toString(tp);
        }


        
    }
}
