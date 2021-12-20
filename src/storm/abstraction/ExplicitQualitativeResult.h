#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/QualitativeResult.h"

namespace storm {
namespace storage {
class BitVector;
}

namespace abstraction {

class ExplicitQualitativeGameResult;

class ExplicitQualitativeResult : public QualitativeResult {
   public:
    virtual ~ExplicitQualitativeResult() = default;

    ExplicitQualitativeGameResult& asExplicitQualitativeGameResult();
    ExplicitQualitativeGameResult const& asExplicitQualitativeGameResult() const;

    virtual storm::storage::BitVector const& getStates() const = 0;
};

}  // namespace abstraction
}  // namespace storm
