#ifndef STORM_MODELCHECKER_SPARSEALLSTATESQUALITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_SPARSEALLSTATESQUALITATIVECHECKRESULT_H_

#include "src/modelchecker/CheckResult.h"
#include "src/storage/BitVector.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        class SparseAllStatesQualitativeCheckResult : public CheckResult {
        public:
            /*!
             * Constructs a check result with the provided truth values.
             *
             * @param truthValues The truth values of the result.
             */
            SparseAllStatesQualitativeCheckResult(storm::storage::BitVector&& truthValues) : truthValues(std::move(truthValues)) {
                // Intentionally left empty.
            }
            
            SparseAllStatesQualitativeCheckResult(SparseAllStatesQualitativeCheckResult const& other) = default;
            SparseAllStatesQualitativeCheckResult& operator=(SparseAllStatesQualitativeCheckResult const& other) = default;
#ifndef WINDOWS
            SparseAllStatesQualitativeCheckResult(SparseAllStatesQualitativeCheckResult&& other) = default;
            SparseAllStatesQualitativeCheckResult& operator=(SparseAllStatesQualitativeCheckResult&& other) = default;
#endif
            
        private:
            // The values of the quantitative check result.
            storm::storage::BitVector truthValues;
        };
    }
}

#endif /* STORM_MODELCHECKER_SPARSEALLSTATESQUALITATIVECHECKRESULT_H_ */