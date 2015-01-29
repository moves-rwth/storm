#include "src/modelchecker/QualitativeCheckResult.h"

namespace storm {
    namespace modelchecker {
        bool QualitativeCheckResult::isQualitative() const {
            return true;
        }
    }
}