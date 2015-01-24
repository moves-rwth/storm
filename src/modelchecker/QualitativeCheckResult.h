#ifndef STORM_MODELCHECKER_QUALITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_QUALITATIVECHECKRESULT_H_

#include "src/modelchecker/CheckResult.h"

namespace storm {
    namespace modelchecker {
        class QualitativeCheckResult : public CheckResult {
        public:
            virtual bool isQualitative() const override;
        };
    }
}

#endif /* STORM_MODELCHECKER_QUALITATIVECHECKRESULT_H_ */