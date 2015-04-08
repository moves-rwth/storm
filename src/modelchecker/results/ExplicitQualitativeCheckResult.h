#ifndef STORM_MODELCHECKER_EXPLICITQUALITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_EXPLICITQUALITATIVECHECKRESULT_H_

#include <map>
#include <functional>
#include <boost/variant.hpp>

#include "src/modelchecker/results/QualitativeCheckResult.h"
#include "src/storage/sparse/StateType.h"
#include "src/storage/BitVector.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace modelchecker {
        class ExplicitQualitativeCheckResult : public QualitativeCheckResult {
        public:
            typedef storm::storage::BitVector vector_type;
            typedef std::map<storm::storage::sparse::state_type, bool> map_type;
            
            ExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult(map_type const& map);
            ExplicitQualitativeCheckResult(map_type&& map);
            ExplicitQualitativeCheckResult(storm::storage::sparse::state_type state, bool value);
            ExplicitQualitativeCheckResult(vector_type const& truthValues);
            ExplicitQualitativeCheckResult(vector_type&& truthValues);
            
            ExplicitQualitativeCheckResult(ExplicitQualitativeCheckResult const& other) = default;
            ExplicitQualitativeCheckResult& operator=(ExplicitQualitativeCheckResult const& other) = default;
#ifndef WINDOWS
            ExplicitQualitativeCheckResult(ExplicitQualitativeCheckResult&& other) = default;
            ExplicitQualitativeCheckResult& operator=(ExplicitQualitativeCheckResult&& other) = default;
#endif
            
            bool operator[](storm::storage::sparse::state_type index) const;
            void setValue(storm::storage::sparse::state_type, bool value);
            
            virtual bool isExplicit() const override;
            virtual bool isResultForAllStates() const override;
            
            virtual bool isExplicitQualitativeCheckResult() const override;
            
            virtual QualitativeCheckResult& operator&=(QualitativeCheckResult const& other) override;
            virtual QualitativeCheckResult& operator|=(QualitativeCheckResult const& other) override;
            virtual void complement() override;

            vector_type const& getTruthValuesVector() const;
            map_type const& getTruthValuesVectorMap() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            virtual void filter(QualitativeCheckResult const& filter) override;

        private:
            static void performLogicalOperation(ExplicitQualitativeCheckResult& first, QualitativeCheckResult const& second, std::function<bool (bool, bool)> const& function);
            
            // The values of the quantitative check result.
            boost::variant<vector_type, map_type> truthValues;
        };
    }
}

#endif /* STORM_MODELCHECKER_EXPLICITQUALITATIVECHECKRESULT_H_ */