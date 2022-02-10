#include "storm/abstraction/ExplicitQualitativeResult.h"

#include "storm/abstraction/ExplicitQualitativeGameResult.h"

namespace storm {
namespace abstraction {

ExplicitQualitativeGameResult& ExplicitQualitativeResult::asExplicitQualitativeGameResult() {
    return static_cast<ExplicitQualitativeGameResult&>(*this);
}

ExplicitQualitativeGameResult const& ExplicitQualitativeResult::asExplicitQualitativeGameResult() const {
    return static_cast<ExplicitQualitativeGameResult const&>(*this);
}

}  // namespace abstraction
}  // namespace storm
