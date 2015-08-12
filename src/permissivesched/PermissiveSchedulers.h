
#ifndef PERMISSIVESCHEDULERS_H
#define	PERMISSIVESCHEDULERS_H

#include "../logic/ProbabilityOperatorFormula.h"
#include "../models/sparse/Mdp.h"


namespace storm {
    namespace ps {
        
        class PermissiveScheduler {
            
        };
        
        class MemorylessDeterministicPermissiveScheduler  : public PermissiveScheduler {
            storm::storage::BitVector memdetschedulers;
        public:
            MemorylessDeterministicPermissiveScheduler(storm::storage::BitVector const& allowedPairs) : memdetschedulers(allowedPairs) {}
        };
        
        boost::optional<MemorylessDeterministicPermissiveScheduler> computePermissiveSchedulerViaMILP(std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::logic::ProbabilityOperatorFormula const& safeProp);
    }
}


#endif	/* PERMISSIVESCHEDULERS_H */

