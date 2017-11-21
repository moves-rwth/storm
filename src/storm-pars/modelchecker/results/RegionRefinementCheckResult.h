#pragma once

#include <vector>

#include "storm-pars/modelchecker/results/RegionCheckResult.h"
#include "storm-pars/modelchecker/region/RegionResult.h"
#include "storm-pars/storage/ParameterRegion.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        class RegionRefinementCheckResult : public RegionCheckResult<ValueType> {
        public:

            RegionRefinementCheckResult(std::vector<std::pair<storm::storage::ParameterRegion<ValueType>, storm::modelchecker::RegionResult>> const& regionResults, storm::storage::ParameterRegion<ValueType> const& parameterSpace);
            RegionRefinementCheckResult(std::vector<std::pair<storm::storage::ParameterRegion<ValueType>, storm::modelchecker::RegionResult>>&& regionResults, storm::storage::ParameterRegion<ValueType>&& parameterSpace);
            virtual ~RegionRefinementCheckResult() = default;

            virtual bool isRegionRefinementCheckResult() const override;
            
            storm::storage::ParameterRegion<ValueType> const& getParameterSpace() const;

            virtual std::ostream& writeIllustrationToStream(std::ostream& out) const override;

            virtual std::unique_ptr<CheckResult> clone() const override;

        protected:
            storm::storage::ParameterRegion<ValueType> parameterSpace;
        };
    }
}
