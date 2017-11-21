#ifndef STORM_MODELCHECKER_PARETOCURVECHECKRESULT_H_
#define STORM_MODELCHECKER_PARETOCURVECHECKRESULT_H_

#include <vector>

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/storage/geometry/Polytope.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        class ParetoCurveCheckResult : public CheckResult {
        public:
            typedef std::vector<ValueType> point_type;
            typedef std::shared_ptr<storm::storage::geometry::Polytope<ValueType>> polytope_type;

            ParetoCurveCheckResult();

            virtual bool isParetoCurveCheckResult() const override;
            
            std::vector<point_type> const& getPoints() const;
            polytope_type const& getUnderApproximation() const;
            polytope_type const& getOverApproximation() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;

        protected:
            ParetoCurveCheckResult(std::vector<point_type> const& points, polytope_type const& underApproximation, polytope_type const& overApproximation);
            ParetoCurveCheckResult(std::vector<point_type>&& points, polytope_type&& underApproximation, polytope_type&& overApproximation);

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
