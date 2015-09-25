#ifndef PERMISSIVESCHEDULERCOMPUTATION_H
#define	PERMISSIVESCHEDULERCOMPUTATION_H

#include <memory>

#include "../storage/BitVector.h"
#include "../models/sparse/Mdp.h"
#include "PermissiveSchedulerPenalty.h"
#include "PermissiveSchedulers.h"

namespace storm {
    namespace ps {
        class PermissiveSchedulerComputation {
        protected:
            std::shared_ptr<storm::models::sparse::Mdp<double>> mdp;
            storm::storage::BitVector const& mGoals;
            storm::storage::BitVector const& mSinks;
            PermissiveSchedulerPenalties mPenalties;
            
        public:
            
            PermissiveSchedulerComputation(std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::storage::BitVector const& goalstates, storm::storage::BitVector const& sinkstates)
                : mdp(mdp), mGoals(goalstates), mSinks(sinkstates)
            {
                
            }
            
                    
            virtual void calculatePermissiveScheduler(bool lowerBound, double boundary) = 0;
            
            void setPenalties(PermissiveSchedulerPenalties penalties) {
                mPenalties = penalties;
            }
            
            PermissiveSchedulerPenalties const& getPenalties() const {
                return mPenalties;
            }
            
            PermissiveSchedulerPenalties & getPenalties() {
                return mPenalties;
            }
            
            virtual bool foundSolution() const = 0;
            
            virtual SubMDPPermissiveScheduler getScheduler() const = 0;
            
       
        };
        
    }
}


#endif	/* PERMISSIVESCHEDULERCOMPUTATION_H */

