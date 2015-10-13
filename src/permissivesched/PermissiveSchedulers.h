
#ifndef PERMISSIVESCHEDULERS_H
#define	PERMISSIVESCHEDULERS_H

#include "../logic/ProbabilityOperatorFormula.h"
#include "../models/sparse/Mdp.h"
#include "../models/sparse/StandardRewardModel.h"


namespace storm {
    namespace ps {

        class PermissiveScheduler {
        public:
            virtual ~PermissiveScheduler() = default;
        };

        template<typename RM= storm::models::sparse::StandardRewardModel<double>>
        class SubMDPPermissiveScheduler : public PermissiveScheduler {
            storm::models::sparse::Mdp<double, RM> const &mdp;
            storm::storage::BitVector enabledChoices;
        public:
            virtual ~SubMDPPermissiveScheduler() = default;

            SubMDPPermissiveScheduler(SubMDPPermissiveScheduler &&) = default;

            SubMDPPermissiveScheduler(SubMDPPermissiveScheduler const &) = delete;

            SubMDPPermissiveScheduler(storm::models::sparse::Mdp<double, RM> const &refmdp, bool allEnabled) :
                    PermissiveScheduler(), mdp(refmdp), enabledChoices(refmdp.getNumberOfChoices(), allEnabled) {
                // Intentionally left empty.
            }

            void disable(uint_fast64_t choiceIndex) {
                assert(choiceIndex < enabledChoices.size());
                enabledChoices.set(choiceIndex, false);
            }


            storm::models::sparse::Mdp<double, RM> apply() const {
                return mdp.restrictChoices(enabledChoices);
            }


        };

        template<typename RM = storm::models::sparse::StandardRewardModel<double>>
        boost::optional<SubMDPPermissiveScheduler<RM>> computePermissiveSchedulerViaMILP(storm::models::sparse::Mdp<double, RM> const& mdp, storm::logic::ProbabilityOperatorFormula const &safeProp);
        
        template<typename RM>
        boost::optional<SubMDPPermissiveScheduler<RM>> computePermissiveSchedulerViaSMT(storm::models::sparse::Mdp<double, RM> const& mdp, storm::logic::ProbabilityOperatorFormula const& safeProp);
    }
}


#endif	/* PERMISSIVESCHEDULERS_H */

