
#ifndef PERMISSIVESCHEDULERS_H
#define	PERMISSIVESCHEDULERS_H

#include "../logic/ProbabilityOperatorFormula.h"
#include "../models/sparse/Mdp.h"


namespace storm {
    namespace ps {
        
        class PermissiveScheduler {
        public:
            virtual ~PermissiveScheduler() = default;
        };
        
        class MemorylessDeterministicPermissiveScheduler  : public PermissiveScheduler {
            storm::storage::BitVector memdetschedulers;
        public:
            virtual ~MemorylessDeterministicPermissiveScheduler() = default;
            MemorylessDeterministicPermissiveScheduler(MemorylessDeterministicPermissiveScheduler&&) = default;
            MemorylessDeterministicPermissiveScheduler(MemorylessDeterministicPermissiveScheduler const&) = delete;
            
            
            MemorylessDeterministicPermissiveScheduler(storm::storage::BitVector const& allowedPairs) : memdetschedulers(allowedPairs) {}
            MemorylessDeterministicPermissiveScheduler(storm::storage::BitVector && allowedPairs) : memdetschedulers(std::move(allowedPairs)) {}
            
            
        };
        
        boost::optional<MemorylessDeterministicPermissiveScheduler> computePermissiveSchedulerViaMILP(std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::logic::ProbabilityOperatorFormula const& safeProp);
    }
}


#endif	/* PERMISSIVESCHEDULERS_H */

