#pragma once

#include <cassert>
namespace storm {
    namespace storage {

        enum class DFTElementType : int {AND = 0, COUNTING = 1, OR = 2, VOT = 3, BE = 4, CONSTF = 5, CONSTS = 6, PAND = 7, SPARE = 8, POR = 9, PDEP = 10, SEQAND = 11};

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
                case DFTElementType::SEQAND:
                    return false;
                default:
                    assert(false);
            }
        }
        
        
        inline bool isGateType(DFTElementType const& tp) {
            switch(tp) {
                case DFTElementType::AND:
                case DFTElementType::COUNTING:
                case DFTElementType::OR:
                case DFTElementType::VOT:
                case DFTElementType::PAND:
                case DFTElementType::SPARE:
                case DFTElementType::POR:
                case DFTElementType::SEQAND:
                    return true;
                case DFTElementType::BE:
                case DFTElementType::CONSTF:
                case DFTElementType::CONSTS:
                case DFTElementType::PDEP:
                    return false;
                default:
                    assert(false);
            }
        }

    }
}

