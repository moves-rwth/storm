#pragma once

#include <cassert>
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
                    assert(false);
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
                    assert(false);
                    return false;
            }
        }
        
        
    }
}

