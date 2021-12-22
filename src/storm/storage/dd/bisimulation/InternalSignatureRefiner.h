#pragma once

#include "storm/storage/dd/DdType.h"

namespace storm {
namespace dd {
namespace bisimulation {

template<storm::dd::DdType DdType, typename ValueType>
class InternalSignatureRefiner;

struct InternalSignatureRefinerOptions {
    InternalSignatureRefinerOptions();
    InternalSignatureRefinerOptions(bool shiftStateVariables);

    bool shiftStateVariables;
    bool reuseBlockNumbers;
    bool createChangedStates;
};

class ReuseWrapper {
   public:
    ReuseWrapper();
    ReuseWrapper(bool value);

    bool isReused() const;
    void setReused();

   private:
    bool value;
};

}  // namespace bisimulation
}  // namespace dd
}  // namespace storm
