#pragma once

#include <deque>
#include <list>
#include <queue>
#include <vector>

#include "storm-dft/storage/OrderDFTElementsById.h"

namespace storm::dft {
namespace storage {

// Forward declarations
namespace elements {

template<typename ValueType>
class DFTGate;
template<typename ValueType>
class DFTElement;
template<typename ValueType>
class DFTRestriction;

}  // namespace elements

template<typename ValueType>
class DFTStateSpaceGenerationQueues {
    using DFTElementPointer = std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>>;
    using DFTElementVector = std::vector<DFTElementPointer>;
    using DFTGatePointer = std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>>;
    using DFTGateVector = std::vector<DFTGatePointer>;
    using DFTRestrictionPointer = std::shared_ptr<storm::dft::storage::elements::DFTRestriction<ValueType>>;
    using DFTRestrictionVector = std::vector<DFTRestrictionPointer>;

    std::priority_queue<DFTGatePointer, DFTGateVector, OrderElementsByRank<ValueType>> failurePropagation;
    DFTGateVector failsafePropagation;
    DFTElementVector dontcarePropagation;
    DFTElementVector activatePropagation;
    DFTRestrictionVector restrictionChecks;

   public:
    void propagateFailure(DFTGatePointer const& elem) {
        failurePropagation.push(elem);
    }

    bool failurePropagationDone() const {
        return failurePropagation.empty();
    }

    DFTGatePointer nextFailurePropagation() {
        DFTGatePointer next = failurePropagation.top();
        failurePropagation.pop();
        return next;
    }

    bool restrictionChecksDone() const {
        return restrictionChecks.empty();
    }

    DFTRestrictionPointer nextRestrictionCheck() {
        STORM_LOG_ASSERT(!restrictionChecksDone(), "All restriction checks done already.");
        DFTRestrictionPointer next = restrictionChecks.back();
        restrictionChecks.pop_back();
        return next;
    }

    void checkRestrictionLater(DFTRestrictionPointer const& restr) {
        restrictionChecks.push_back(restr);
    }

    bool failsafePropagationDone() const {
        return failsafePropagation.empty();
    }

    void propagateFailsafe(DFTGatePointer const& gate) {
        failsafePropagation.push_back(gate);
    }

    DFTGatePointer nextFailsafePropagation() {
        DFTGatePointer next = failsafePropagation.back();
        failsafePropagation.pop_back();
        return next;
    }

    bool dontCarePropagationDone() const {
        return dontcarePropagation.empty();
    }

    void propagateDontCare(DFTElementPointer const& elem) {
        dontcarePropagation.push_back(elem);
    }

    void propagateDontCare(DFTElementVector const& elems) {
        dontcarePropagation.insert(dontcarePropagation.end(), elems.begin(), elems.end());
    }

    DFTElementPointer nextDontCarePropagation() {
        DFTElementPointer next = dontcarePropagation.back();
        dontcarePropagation.pop_back();
        return next;
    }
};

}  // namespace storage
}  // namespace storm::dft