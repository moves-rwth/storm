
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
        
        class SubMDPPermissiveScheduler  : public PermissiveScheduler {
            storm::models::sparse::Mdp<double> const& mdp;
            storm::storage::BitVector enabledChoices;
        public:
            virtual ~SubMDPPermissiveScheduler() = default;
            SubMDPPermissiveScheduler(SubMDPPermissiveScheduler&&) = default;
            SubMDPPermissiveScheduler(SubMDPPermissiveScheduler const&) = delete;

            SubMDPPermissiveScheduler(storm::models::sparse::Mdp<double> const& refmdp, bool allEnabled) :
                PermissiveScheduler(), mdp(refmdp), enabledChoices(refmdp.getNumberOfChoices(), allEnabled)
            {
                // Intentionally left empty.
            }

            void disable(uint_fast64_t choiceIndex) {
                assert(choiceIndex < enabledChoices.size());
                enabledChoices.set(choiceIndex, false);
            }


            storm::models::sparse::Mdp<double> apply() const {
                return mdp.restrictChoices(enabledChoices);
            }

            
        };
        
        boost::optional<SubMDPPermissiveScheduler> computePermissiveSchedulerViaMILP(std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::logic::ProbabilityOperatorFormula const& safeProp);
    }
}


#endif	/* PERMISSIVESCHEDULERS_H */

