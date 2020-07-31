#pragma once

#include "storm-pars/modelchecker/region/ValidatingSparseParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"

namespace storm {
    namespace modelchecker {
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        class ValidatingSparseDtmcParameterLiftingModelChecker : public ValidatingSparseParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType> {
        public:
            ValidatingSparseDtmcParameterLiftingModelChecker();
            virtual ~ValidatingSparseDtmcParameterLiftingModelChecker() = default;

            virtual void specify(Environment const& env, std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask, bool generateRegionSplitEstimates = false, bool allowModelSimplifications = true) override;

        protected:
            virtual SparseParameterLiftingModelChecker<SparseModelType, ImpreciseType>& getImpreciseChecker() override;
            virtual SparseParameterLiftingModelChecker<SparseModelType, ImpreciseType> const& getImpreciseChecker() const override;
            virtual SparseParameterLiftingModelChecker<SparseModelType, PreciseType>& getPreciseChecker() override;
            virtual SparseParameterLiftingModelChecker<SparseModelType, PreciseType> const& getPreciseChecker() const override;
            
            virtual void applyHintsToPreciseChecker() override ;

        private:
            SparseDtmcParameterLiftingModelChecker<SparseModelType, ImpreciseType> impreciseChecker;
            SparseDtmcParameterLiftingModelChecker<SparseModelType, PreciseType> preciseChecker;
            
        };
    }
}
