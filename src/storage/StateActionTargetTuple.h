
#ifndef STATEACTIONTARGETTUPLE_H
#define	STATEACTIONTARGETTUPLE_H

#include <memory>

namespace storm {
    namespace storage {
        struct StateActionTarget {
            uint_fast64_t state;
            uint_fast64_t action;
            uint_fast64_t target;
                
        };
            
        inline std::string to_string(StateActionTarget const& sat) {
            return std::to_string(sat.state) + "_" + std::to_string(sat.action) + "_" + std::to_string(sat.target);
        }

    }
}
            
namespace std {
    template<>
    struct hash<storm::storage::StateActionTarget> {
        bool operator()(storm::storage::StateActionTarget const& sat) const {
            return (sat.state ^ sat.target) << 3 | sat.action;
        }
    };
    
}


#endif	/* STATEACTIONTARGETTUPLE_H */

