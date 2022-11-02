#pragma once

#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Pomdp.h"

namespace storm {
    namespace synthesis {

        template<typename ValueType>
        class PomdpManagerAposteriori {

        public:
            
            PomdpManagerAposteriori(storm::models::sparse::Pomdp<ValueType> const& pomdp);

            // unfold memory model (a aposteriori memory update) into the POMDP
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> constructMdp();            

            // for each observation contains the number of allocated memory states (initially 1)
            std::vector<uint64_t> observation_memory_size;
            
            // set memory size to a selected observation
            void setObservationMemorySize(uint64_t obs, uint64_t memory_size);
            // set memory size to all observations
            void setGlobalMemorySize(uint64_t memory_size);

        private:

            // original POMDP
            storm::models::sparse::Pomdp<ValueType> const& pomdp;
            
        };
    }
}