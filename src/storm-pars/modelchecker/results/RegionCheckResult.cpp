#include "storm-pars/modelchecker/results/RegionCheckResult.h"

#include <map>

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

namespace storm {
    namespace modelchecker {

        template<typename ValueType>
        RegionCheckResult<ValueType>::RegionCheckResult(std::vector<std::pair<storm::storage::ParameterRegion<ValueType>, storm::modelchecker::RegionResult>> const& regionResults) : regionResults(regionResults) {
            auto overallArea = storm::utility::zero<typename storm::storage::ParameterRegion<ValueType>::CoefficientType>();
            for (auto const& res : this->regionResults) {
                overallArea += res.first.area();
            }
            initFractions(overallArea);
        }
        
        template<typename ValueType>
        RegionCheckResult<ValueType>::RegionCheckResult(std::vector<std::pair<storm::storage::ParameterRegion<ValueType>, storm::modelchecker::RegionResult>>&& regionResults) : regionResults(std::move(regionResults)) {
            auto overallArea = storm::utility::zero<typename storm::storage::ParameterRegion<ValueType>::CoefficientType>();
            for (auto const& res : this->regionResults) {
                overallArea += res.first.area();
            }
            initFractions(overallArea);
        }

        template<typename ValueType>
        bool RegionCheckResult<ValueType>::isRegionCheckResult() const {
            return true;
        }
        
        template<typename ValueType>
        bool RegionCheckResult<ValueType>::isRegionRefinementCheckResult() const {
            return false;
        }
            
        template<typename ValueType>
        std::vector<std::pair<storm::storage::ParameterRegion<ValueType>, storm::modelchecker::RegionResult>> const& RegionCheckResult<ValueType>::getRegionResults() const {
            return regionResults;
        }
        
        template<typename ValueType>
        typename storm::storage::ParameterRegion<ValueType>::CoefficientType const& RegionCheckResult<ValueType>::getSatFraction() const {
            return satFraction;
        }
        
        template<typename ValueType>
        typename storm::storage::ParameterRegion<ValueType>::CoefficientType const& RegionCheckResult<ValueType>::getUnsatFraction() const {
            return unsatFraction;
        }

        template<typename ValueType>
        std::ostream& RegionCheckResult<ValueType>::writeToStream(std::ostream& out) const {
            writeCondensedToStream(out);
            out << std::endl << "Region results: " << std::endl;
            for (auto const& res : this->regionResults) {
                out << res.first.toString() << ": \t" << res.second << std::endl;
            }
        }
        
        template<typename ValueType>
        std::ostream& RegionCheckResult<ValueType>::writeCondensedToStream(std::ostream& out) const {
            auto oneHundred = storm::utility::convertNumber<typename storm::storage::ParameterRegion<ValueType>::CoefficientType>(100.0);
            auto one = storm::utility::convertNumber<typename storm::storage::ParameterRegion<ValueType>::CoefficientType>(1.0);
            out << "Fraction of satisfied area:   " << (satFraction * oneHundred) << std::endl;
            out << "Fraction of unsatisfied area: " << (unsatFraction * oneHundred) << std::endl;
            out << "Unknown fraction:             " << ((one - satFraction - unsatFraction) * oneHundred) << std::endl;
            out << "Total Number of regions:      " << regionResults.size() << std::endl;
            std::map<storm::modelchecker::RegionResult, uint_fast64_t> counters;
            for (auto const& res : this->regionResults) {
                ++counters[res.second];
            }
            for (auto const& counter : counters) {
                out << std::setw(28) << counter.first << ": " << counter.second << std::endl;
            }
        }

        template<typename ValueType>
        std::ostream& RegionCheckResult<ValueType>::writeIllustrationToStream(std::ostream& out) const {
            STORM_LOG_WARN("Writing illustration of region check result to a stream is not implemented.");
        }
  
        template<typename ValueType>
        void RegionCheckResult<ValueType>::initFractions(typename storm::storage::ParameterRegion<ValueType>::CoefficientType const& overallArea) {
            auto satArea = storm::utility::zero<typename storm::storage::ParameterRegion<ValueType>::CoefficientType>();
            auto unsatArea = storm::utility::zero<typename storm::storage::ParameterRegion<ValueType>::CoefficientType>();
            for (auto const& res : this->regionResults) {
                if (res.second == storm::modelchecker::RegionResult::AllSat) {
                    satArea += res.first.area();
                } else if (res.second == storm::modelchecker::RegionResult::AllViolated) {
                    unsatArea += res.first.area();
                }
            }
            satFraction = satArea / overallArea;
            unsatFraction = unsatArea / overallArea;
        }
        
        template<typename ValueType>
        void RegionCheckResult<ValueType>::filter(QualitativeCheckResult const& filter) {
            // Filtering has no effect as we only store the result w.r.t. a single state anyway.
            // Hence, this is intentionally left empty.
        }

        
#ifdef STORM_HAVE_CARL
        template class RegionCheckResult<storm::RationalFunction>;
#endif
    }
}
