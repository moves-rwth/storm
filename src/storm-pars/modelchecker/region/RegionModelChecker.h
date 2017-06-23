#pragma once

#include <memory>

#include "storm-pars/modelchecker/results/RegionCheckResult.h"
#include "storm-pars/modelchecker/results/RegionRefinementCheckResult.h"
#include "storm-pars/modelchecker/region/RegionResult.h"
#include "storm-pars/storage/ParameterRegion.h"

#include "storm/modelchecker/CheckTask.h"

namespace storm {
    namespace modelchecker{
        
        template<typename ParametricType>
        class RegionModelChecker {
        public:
            
            typedef typename storm::storage::ParameterRegion<ParametricType>::CoefficientType CoefficientType;

            RegionModelChecker();
            virtual ~RegionModelChecker() = default;
            
            virtual bool canHandle(CheckTask<storm::logic::Formula, ParametricType> const& checkTask) const = 0;

            virtual void specifyFormula(CheckTask<storm::logic::Formula, ParametricType> const& checkTask) = 0;
            
            /*!
             * Analyzes the given region.
             * An initial region result can be given to simplify the analysis (e.g. if the initial result is ExistsSat, we do not check for AllViolated).
             * If supported by this model checker, it is possible to sample the vertices of the region whenever AllSat/AllViolated could not be shown.
             */
            virtual RegionResult analyzeRegion(storm::storage::ParameterRegion<ParametricType> const& region, RegionResult const& initialResult = RegionResult::Unknown, bool sampleVerticesOfRegion = false) = 0;
            
             /*!
             * Analyzes the given regions.
             * If supported by this model checker, it is possible to sample the vertices of the regions whenever AllSat/AllViolated could not be shown.
             */
            std::unique_ptr<storm::modelchecker::RegionCheckResult<ParametricType>> analyzeRegions(std::vector<storm::storage::ParameterRegion<ParametricType>> const& regions, bool sampleVerticesOfRegion = false) ;
            
            /*!
             * Iteratively refines the region until the region analysis yields a conclusive result (AllSat or AllViolated).
             * The refinement stops as soon as the fraction of the area of the subregions with inconclusive result is less then the given threshold
             */
            std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ParametricType>> performRegionRefinement(storm::storage::ParameterRegion<ParametricType> const& region, ParametricType const& threshold);
            
        };

    } //namespace modelchecker
} //namespace storm
