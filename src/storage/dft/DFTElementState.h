
#ifndef DFTELEMENTSTATE_H
#define	DFTELEMENTSTATE_H

namespace storm {
    namespace storage {
        enum class DFTElementState  {Operational = 0, Failed = 2, Failsafe = 1, DontCare = 3};
        
        inline std::ostream& operator<<(std::ostream& os, DFTElementState st) {
            switch(st) {
                case DFTElementState::Operational:
                    return os << "Operational";
                case DFTElementState::Failed:
                    return os << "Failed";
                case DFTElementState::Failsafe:
                    return os << "Failsafe";
                case DFTElementState::DontCare:
                    return os << "Don't Care";
            }
        }
    }
}


#endif	/* DFTELEMENTSTATE_H */

