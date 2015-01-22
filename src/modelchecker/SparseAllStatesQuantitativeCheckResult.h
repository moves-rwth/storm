#ifndef STORM_MODELCHECKER_SPARSEALLSTATESQUANTITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_SPARSEALLSTATESQUANTITATIVECHECKRESULT_H_

#include <vector>

#include "src/modelchecker/CheckResult.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        class SparseAllStatesQuantitativeCheckResult : public CheckResult {
        public:
            /*!
             * Constructs a check result with the provided values.
             *
             * @param values The values of the result.
             */
            SparseAllStatesQuantitativeCheckResult(std::vector<ValueType>&& values) : values(std::move(values)) {
                // Intentionally left empty.
            }
            
            SparseAllStatesQuantitativeCheckResult(SparseAllStatesQuantitativeCheckResult const& other) = default;
            SparseAllStatesQuantitativeCheckResult& operator=(SparseAllStatesQuantitativeCheckResult const& other) = default;
#ifndef WINDOWS
            SparseAllStatesQuantitativeCheckResult(SparseAllStatesQuantitativeCheckResult&& other) = default;
            SparseAllStatesQuantitativeCheckResult& operator=(SparseAllStatesQuantitativeCheckResult&& other) = default;
#endif
            
        private:
            // The values of the quantitative check result.
            std::vector<ValueType> values;
        };
    }
}

#endif /* STORM_MODELCHECKER_SPARSEALLSTATESQUANTITATIVECHECKRESULT_H_ */