#pragma once

#include <memory>

#include "storm-permissive/analysis/PermissiveSchedulerPenalty.h"
#include "storm-permissive/analysis/PermissiveSchedulers.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/storage/BitVector.h"

namespace storm {
namespace ps {

template<typename RM>
class PermissiveSchedulerComputation {
   protected:
    storm::models::sparse::Mdp<double, RM> const& mdp;
    storm::storage::BitVector const& mGoals;
    storm::storage::BitVector const& mSinks;
    PermissiveSchedulerPenalties mPenalties;

   public:
    PermissiveSchedulerComputation(storm::models::sparse::Mdp<double, RM> const& mdp, storm::storage::BitVector const& goalstates,
                                   storm::storage::BitVector const& sinkstates)
        : mdp(mdp), mGoals(goalstates), mSinks(sinkstates) {}

    virtual ~PermissiveSchedulerComputation() = default;

    virtual void calculatePermissiveScheduler(bool lowerBound, double boundary) = 0;

    void setPenalties(PermissiveSchedulerPenalties penalties) {
        mPenalties = penalties;
    }

    PermissiveSchedulerPenalties const& getPenalties() const {
        return mPenalties;
    }

    PermissiveSchedulerPenalties& getPenalties() {
        return mPenalties;
    }

    virtual bool foundSolution() const = 0;

    virtual SubMDPPermissiveScheduler<RM> getScheduler() const = 0;
};

}  // namespace ps
}  // namespace storm
