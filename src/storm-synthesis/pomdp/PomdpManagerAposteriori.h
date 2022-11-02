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

        private:

            // original POMDP
            storm::models::sparse::Pomdp<ValueType> const& pomdp;
            
        };
    }
}