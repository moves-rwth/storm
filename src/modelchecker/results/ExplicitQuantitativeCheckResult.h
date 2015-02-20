#ifndef STORM_MODELCHECKER_EXPLICITQUANTITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_EXPLICITQUANTITATIVECHECKRESULT_H_

#include <vector>
#include <map>
#include <boost/variant.hpp>

#include "src/modelchecker/results/QuantitativeCheckResult.h"
#include "src/storage/sparse/StateType.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        class ExplicitQuantitativeCheckResult : public QuantitativeCheckResult<ValueType> {
        public:
            typedef std::vector<ValueType> vector_type;
            typedef std::map<storm::storage::sparse::state_type, ValueType> map_type;
            
            ExplicitQuantitativeCheckResult();
            ExplicitQuantitativeCheckResult(map_type const& values);
            ExplicitQuantitativeCheckResult(map_type&& values);
            ExplicitQuantitativeCheckResult(storm::storage::sparse::state_type const& state, ValueType const& value);
            ExplicitQuantitativeCheckResult(vector_type const& values);
            ExplicitQuantitativeCheckResult(vector_type&& values);
            
            ExplicitQuantitativeCheckResult(ExplicitQuantitativeCheckResult const& other) = default;
            ExplicitQuantitativeCheckResult& operator=(ExplicitQuantitativeCheckResult const& other) = default;
#ifndef WINDOWS
            ExplicitQuantitativeCheckResult(ExplicitQuantitativeCheckResult&& other) = default;
            ExplicitQuantitativeCheckResult& operator=(ExplicitQuantitativeCheckResult&& other) = default;
#endif
            
            ValueType& operator[](storm::storage::sparse::state_type state);
            ValueType const& operator[](storm::storage::sparse::state_type state) const;

            virtual std::unique_ptr<CheckResult> compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const override;
            
            virtual bool isExplicit() const override;
            virtual bool isResultForAllStates() const override;
            
            virtual bool isExplicitQuantitativeCheckResult() const override;
            
            vector_type const& getValueVector() const;
            map_type const& getValueMap() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            virtual std::ostream& writeToStream(std::ostream& out, storm::storage::BitVector const& filter) const override;
            
        private:
            // The values of the quantitative check result.
            boost::variant<vector_type, map_type> values;
        };
    }
}

#endif /* STORM_MODELCHECKER_EXPLICITQUANTITATIVECHECKRESULT_H_ */