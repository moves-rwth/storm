#pragma once

#include "storm-pars/modelchecker/region/RegionModelChecker.h"
#include "storm-pars/modelchecker/region/SparseParameterLiftingModelChecker.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm/utility/NumberTraits.h"

namespace storm {
    namespace modelchecker {
            
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        class ValidatingSparseParameterLiftingModelChecker : public RegionModelChecker<typename SparseModelType::ValueType> {
            static_assert(storm::NumberTraits<PreciseType>::IsExact, "Specified type for exact computations is not exact.");

        public:
            ValidatingSparseParameterLiftingModelChecker();
            virtual ~ValidatingSparseParameterLiftingModelChecker();
            
            virtual bool canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) const override;

            /*!
             * Analyzes the given region by means of parameter lifting.
             * We first apply unsound solution methods (standard value iteratio with doubles) and then validate the obtained result
             * by means of exact and soud methods.
             */
            virtual RegionResult analyzeRegion(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, RegionResultHypothesis const& hypothesis = RegionResultHypothesis::Unknown, RegionResult const& initialResult = RegionResult::Unknown, bool sampleVerticesOfRegion = false, std::shared_ptr<storm::analysis::LocalMonotonicityResult<typename RegionModelChecker<typename SparseModelType::ValueType>::VariableType>> localMonotonicityResult = nullptr) override;

        protected:
            
            virtual SparseParameterLiftingModelChecker<SparseModelType, ImpreciseType>& getImpreciseChecker() = 0;
            virtual SparseParameterLiftingModelChecker<SparseModelType, ImpreciseType> const& getImpreciseChecker() const = 0;
            virtual SparseParameterLiftingModelChecker<SparseModelType, PreciseType>& getPreciseChecker() = 0;
            virtual SparseParameterLiftingModelChecker<SparseModelType, PreciseType> const& getPreciseChecker() const = 0;
            
            virtual void applyHintsToPreciseChecker() = 0;

        private:
            
            // Information for statistics
            uint_fast64_t numOfWrongRegions;
            
        };
    }
}
