#ifndef STATEACTIONPAIR_H
#define	STATEACTIONPAIR_H

#include <memory>

namespace storm {
    namespace storage {
        class StateActionPair {
            std::pair<uint_fast64_t, uint_fast64_t> stateActionPair;
            
        public:
            StateActionPair(std::pair<uint_fast64_t, uint_fast64_t> const& sap) : stateActionPair(sap) {}
            StateActionPair(uint_fast64_t state, uint_fast64_t action) : stateActionPair(std::make_pair(state, action)) {}
            
            uint_fast64_t getState() const { 
                return stateActionPair.first;
            }
            
            uint_fast64_t getAction() const {
                return stateActionPair.second;
            }
            
            friend bool operator==(StateActionPair const& p1, StateActionPair const& p2) {
                return p1.stateActionPair == p2.stateActionPair;
            }
            
            friend bool operator!=(StateActionPair const& p1, StateActionPair const& p2) {
                return p1.stateActionPair != p2.stateActionPair;
            }
            
            
        };
    }
}

namespace std {
    template<>
    struct hash<storm::storage::StateActionPair> {
        size_t operator()(storm::storage::StateActionPair const& sap) {
            return (sap.getState() << 3 ^ sap.getAction());
        }
    };
	
}

#endif	/* STATEACTIONPAIR_H */

