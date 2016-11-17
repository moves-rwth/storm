#include "src/storm/modelchecker/results/ParetoCurveCheckResult.h"

#include "src/storm/adapters/CarlAdapter.h"
#include "src/storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/storm/utility/macros.h"
#include "src/storm/utility/vector.h"

#include "src/storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        ParetoCurveCheckResult<ValueType>::ParetoCurveCheckResult() {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        ParetoCurveCheckResult<ValueType>::ParetoCurveCheckResult(storm::storage::sparse::state_type const& state, std::vector<point_type> const& points, polytope_type const& underApproximation, polytope_type const& overApproximation) : state(state), points(points), underApproximation(underApproximation), overApproximation(overApproximation) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        ParetoCurveCheckResult<ValueType>::ParetoCurveCheckResult(storm::storage::sparse::state_type const& state, std::vector<point_type>&& points, polytope_type&& underApproximation, polytope_type&& overApproximation) : state(state), points(points), underApproximation(underApproximation), overApproximation(overApproximation) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool ParetoCurveCheckResult<ValueType>::isParetoCurveCheckResult() const {
            return true;
        }
        
        template<typename ValueType>
        void ParetoCurveCheckResult<ValueType>::filter(QualitativeCheckResult const& filter) {
            STORM_LOG_THROW(filter.isExplicitQualitativeCheckResult(), storm::exceptions::InvalidOperationException, "Cannot filter explicit check result with non-explicit filter.");
            STORM_LOG_THROW(filter.isResultForAllStates(), storm::exceptions::InvalidOperationException, "Cannot filter check result with non-complete filter.");
            ExplicitQualitativeCheckResult const& explicitFilter = filter.asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult::vector_type const& filterTruthValues = explicitFilter.getTruthValuesVector();
            
            STORM_LOG_THROW(filterTruthValues.getNumberOfSetBits() == 1 && filterTruthValues.get(state), storm::exceptions::InvalidOperationException, "The check result fails to contain some results referred to by the filter.");
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type const& ParetoCurveCheckResult<ValueType>:: getState() const {
            return state;
        }
        
        template<typename ValueType>
        std::vector<typename ParetoCurveCheckResult<ValueType>::point_type> const& ParetoCurveCheckResult<ValueType>::getPoints() const {
            return points;
        }
        
        template<typename ValueType>
        typename ParetoCurveCheckResult<ValueType>::polytope_type const& ParetoCurveCheckResult<ValueType>::getUnderApproximation() const {
            return underApproximation;
        }
        
        template<typename ValueType>
        typename ParetoCurveCheckResult<ValueType>::polytope_type const& ParetoCurveCheckResult<ValueType>::getOverApproximation() const {
            return overApproximation;
        }
        
        
        template<typename ValueType>
        std::ostream& ParetoCurveCheckResult<ValueType>::writeToStream(std::ostream& out) const {
            out << std::endl;
            out << "Underapproximation of achievable values: " << underApproximation->toString() << std::endl;
            out << "Overapproximation of achievable values: " << overApproximation->toString() << std::endl;
            out << points.size() << " pareto optimal points found (Note that these points are safe, i.e., contained in the underapproximation, but there is no guarantee for optimality):" << std::endl;
            for(auto const& p : points) {
                out << "   (";
                for(auto it = p.begin(); it != p.end(); ++it){
                    if(it != p.begin()){
                        out << ", ";
                    }
                    out << std::setw(10) << *it;
                }
                out << " )" << std::endl;
            }
            return out;
        }
        
        template class ParetoCurveCheckResult<double>;
        
#ifdef STORM_HAVE_CARL
        template class ParetoCurveCheckResult<storm::RationalNumber>;
#endif
    }
}
