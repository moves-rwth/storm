#include "src/modelchecker/results/QualitativeCheckResult.h"

namespace storm {
    namespace modelchecker {
        bool QualitativeCheckResult::isQualitative() const {
            return true;
        }
    }
}