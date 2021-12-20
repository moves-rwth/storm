#pragma once

#include <set>
#include <string>
#include <vector>

#include "storm/storage/BitVector.h"

namespace storm {
namespace modelchecker {
namespace helper {
namespace rewardbounded {

class CostLimit {
   public:
    CostLimit(uint64_t const& costLimit);
    bool isInfinity() const;
    uint64_t const& get() const;
    uint64_t& get();
    bool operator<(CostLimit const& other) const;
    bool operator==(CostLimit const& other) const;
    static CostLimit infinity();

   private:
    uint64_t value;
};
typedef std::vector<CostLimit> CostLimits;

class CostLimitClosure {
   public:
    struct CostLimitsCompare {
        bool operator()(CostLimits const& lhs, CostLimits const& rhs) const;
    };
    typedef std::set<CostLimits, CostLimitsCompare> GeneratorType;

    explicit CostLimitClosure(storm::storage::BitVector const& downwardDimensions);
    bool insert(CostLimits const& costLimits);
    bool contains(CostLimits const& costLimits) const;
    bool containsUpwardClosure(CostLimits const& costLimits) const;
    bool dominates(CostLimits const& lhs, CostLimits const& rhs) const;
    bool empty() const;  // True if there is no point in this
    bool full() const;   // True if all points lie in this.
    std::vector<CostLimits> getDominatingCostLimits(CostLimits const& costLimits) const;
    GeneratorType const& getGenerator() const;
    uint64_t dimension() const;

    /*!
     * Returns true if the union of the two closures is full, i.e., contains every point.
     */
    static bool unionFull(CostLimitClosure const& first, CostLimitClosure const& second);

   private:
    /// The dimensions that are downwards closed, i.e., if x is in the closure, then also all y <= x
    storm::storage::BitVector downwardDimensions;
    GeneratorType generator;
};
}  // namespace rewardbounded
}  // namespace helper
}  // namespace modelchecker
}  // namespace storm