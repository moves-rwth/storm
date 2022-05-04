#pragma once

#include <memory>

#include "storm-pars/analysis/Order.h"
#include "storm-pars/analysis/OrderExtender.h"
#include "storm-pars/analysis/LocalMonotonicityResult.h"
#include "storm-pars/modelchecker/results/RegionCheckResult.h"
#include "storm-pars/modelchecker/results/RegionRefinementCheckResult.h"
#include "storm-pars/modelchecker/region/RegionResult.h"
#include "storm-pars/modelchecker/region/RegionResultHypothesis.h"
#include "storm-pars/storage/ParameterRegion.h"

#include "storm/models/ModelBase.h"
#include "storm/modelchecker/CheckTask.h"

namespace storm {
    
    class Environment;
    
    namespace modelchecker{
        
        template<typename ParametricType>
        class RegionModelChecker {
        public:
            
            typedef typename storm::storage::ParameterRegion<ParametricType>::CoefficientType CoefficientType;
            typedef typename storm::storage::ParameterRegion<ParametricType>::VariableType VariableType;

            RegionModelChecker();
            virtual ~RegionModelChecker() = default;
            
            virtual bool canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, ParametricType> const& checkTask) const = 0;
            virtual void specify(Environment const& env, std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, ParametricType> const& checkTask, bool generateRegionSplitEstimates, bool allowModelSimplifications = true) = 0;

            
            /*!
             * Analyzes the given region.
             * @param hypothesis if not 'unknown', the region checker only tries to show the hypothesis
             * @param initialResult encodes what is already known about this region
             * @param sampleVerticesOfRegion enables sampling of the vertices of the region in cases where AllSat/AllViolated could not be shown.
             */
            virtual RegionResult analyzeRegion(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, RegionResultHypothesis const& hypothesis = RegionResultHypothesis::Unknown, RegionResult const& initialResult = RegionResult::Unknown, bool sampleVerticesOfRegion = false, std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult = nullptr) = 0;
            
             /*!
             * Analyzes the given regions.
             * @param hypothesis if not 'unknown', we only try to show the hypothesis for each region

             * If supported by this model checker, it is possible to sample the vertices of the regions whenever AllSat/AllViolated could not be shown.
             */
            std::unique_ptr<storm::modelchecker::RegionCheckResult<ParametricType>> analyzeRegions(Environment const& env, std::vector<storm::storage::ParameterRegion<ParametricType>> const& regions, std::vector<RegionResultHypothesis> const& hypotheses, bool sampleVerticesOfRegion = false) ;

            virtual ParametricType getBoundAtInitState(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dirForParameters);
            
            /*!
             * Iteratively refines the region until the region analysis yields a conclusive result (AllSat or AllViolated).
             * @param region the considered region
             * @param coverageThreshold if given, the refinement stops as soon as the fraction of the area of the subregions with inconclusive result is less then this threshold
             * @param depthThreshold if given, the refinement stops at the given depth. depth=0 means no refinement.
             * @param hypothesis if not 'unknown', it is only checked whether the hypothesis holds within the given region.
             * @param monThresh if given, determines at which depth to start using monotonicity
             */
            std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ParametricType>> performRegionRefinement(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, boost::optional<ParametricType> const& coverageThreshold, boost::optional<uint64_t> depthThreshold = boost::none, RegionResultHypothesis const& hypothesis = RegionResultHypothesis::Unknown, uint64_t monThresh = 0);

            // TODO: documentation
            /*!
             * Finds the extremal value within the given region and with the given precision.
             * The returned value v corresponds to the value at the returned valuation.
             * The actual maximum (minimum) lies in the interval [v, v+precision] ([v-precision, v])
             */
            virtual std::pair<ParametricType, typename storm::storage::ParameterRegion<ParametricType>::Valuation> computeExtremalValue(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dir, ParametricType const& precision, bool absolutePrecision);
            virtual bool checkExtremalValue(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dir, ParametricType const& precision, bool absolutePrecision, ParametricType const& valueToCheck);

            /*!
             * Returns true if region split estimation (a) was enabled when model and check task have been specified and (b) is supported by this region model checker.
             */
            virtual bool isRegionSplitEstimateSupported() const;
            
            /*!
             * Returns an estimate of the benefit of splitting the last checked region with respect to each parameter. This method should only be called if region split estimation is supported and enabled.
             * If a parameter is assigned a high value, we should prefer splitting with respect to this parameter.
             */
            virtual std::map<VariableType, double> getRegionSplitEstimate() const;

            virtual std::shared_ptr<storm::analysis::Order> extendOrder(std::shared_ptr<storm::analysis::Order> order, storm::storage::ParameterRegion<ParametricType> region);

            virtual void setConstantEntries(std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult);

            bool isUseMonotonicitySet() const;
            bool isUseBoundsSet();
            bool isOnlyGlobalSet();

            void setUseMonotonicity(bool monotonicity = true);
            void setUseBounds(bool bounds = true);
            void setUseOnlyGlobal(bool global = true);

            void setMonotoneParameters(std::pair<std::set<typename storm::storage::ParameterRegion<ParametricType>::VariableType>, std::set<typename storm::storage::ParameterRegion<ParametricType>::VariableType>> monotoneParameters);

        private:
            bool useMonotonicity = false;
            bool useOnlyGlobal = false;
            bool useBounds = false;

        protected:

            uint_fast64_t numberOfRegionsKnownThroughMonotonicity;
            boost::optional<std::set<typename storm::storage::ParameterRegion<ParametricType>::VariableType>> monotoneIncrParameters;
            boost::optional<std::set<typename storm::storage::ParameterRegion<ParametricType>::VariableType>> monotoneDecrParameters;

            virtual void extendLocalMonotonicityResult(storm::storage::ParameterRegion<ParametricType> const& region, std::shared_ptr<storm::analysis::Order> order, std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult);

            virtual void splitSmart(storm::storage::ParameterRegion<ParametricType> &region, std::vector<storm::storage::ParameterRegion<ParametricType>> &regionVector, storm::analysis::MonotonicityResult<VariableType> &monRes, bool splitForExtremum) const;

        };

    } //namespace modelchecker
} //namespace storm
