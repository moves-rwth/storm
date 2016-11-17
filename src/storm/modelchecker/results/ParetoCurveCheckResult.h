#ifndef STORM_MODELCHECKER_PARETOCURVECHECKRESULT_H_
#define STORM_MODELCHECKER_PARETOCURVECHECKRESULT_H_

#include <vector>

#include "src/modelchecker/results/CheckResult.h"
#include "src/utility/OsDetection.h"
#include "src/storage/sparse/StateType.h"
#include "src/storage/geometry/Polytope.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        class ParetoCurveCheckResult : public CheckResult {
        public:
            typedef std::vector<ValueType> point_type;
            typedef std::shared_ptr<storm::storage::geometry::Polytope<ValueType>> polytope_type;
            
            ParetoCurveCheckResult();
            ParetoCurveCheckResult(storm::storage::sparse::state_type const& state, std::vector<point_type> const& points, polytope_type const& underApproximation, polytope_type const& overApproximation);
            ParetoCurveCheckResult(storm::storage::sparse::state_type const& state, std::vector<point_type>&& points, polytope_type&& underApproximation, polytope_type&& overApproximation);
            
            ParetoCurveCheckResult(ParetoCurveCheckResult const& other) = default;
            ParetoCurveCheckResult& operator=(ParetoCurveCheckResult const& other) = default;
            ParetoCurveCheckResult(ParetoCurveCheckResult&& other) = default;
            ParetoCurveCheckResult& operator=(ParetoCurveCheckResult&& other) = default;
            virtual ~ParetoCurveCheckResult() = default;
            
            virtual bool isParetoCurveCheckResult() const override;
            
            virtual void filter(QualitativeCheckResult const& filter) override;
            
            storm::storage::sparse::state_type const& getState() const;
            std::vector<point_type> const& getPoints() const;
            polytope_type const& getUnderApproximation() const;
            polytope_type const& getOverApproximation() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;

        private:
            // The state of the checked model to which the result applies
            storm::storage::sparse::state_type state;
            
            // The pareto optimal points that have been found.
            std::vector<point_type> points;
            
            // An underapproximation of the set of achievable values
            polytope_type underApproximation;
            
            // An overapproximation of the set of achievable values
            polytope_type overApproximation;
        };
    }
}

#endif /* STORM_MODELCHECKER_PARETOCURVECHECKRESULT_H_ */
