#pragma once

#include "storm-pars/modelchecker/region/ValidatingSparseParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/SparseMdpParameterLiftingModelChecker.h"

namespace storm {
    namespace modelchecker {
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        class ValidatingSparseMdpParameterLiftingModelChecker : public ValidatingSparseParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType> {
        public:
            ValidatingSparseMdpParameterLiftingModelChecker();
            virtual ~ValidatingSparseMdpParameterLiftingModelChecker() = default;

            virtual void specify(Environment const& env, std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask, bool generateRegionSplitEstimates = false, bool allowModelSimplifications = true) override;

        protected:
            virtual SparseParameterLiftingModelChecker<SparseModelType, ImpreciseType>& getImpreciseChecker() override;
            virtual SparseParameterLiftingModelChecker<SparseModelType, ImpreciseType> const& getImpreciseChecker() const override;
            virtual SparseParameterLiftingModelChecker<SparseModelType, PreciseType>& getPreciseChecker() override;
            virtual SparseParameterLiftingModelChecker<SparseModelType, PreciseType> const& getPreciseChecker() const override;
            
            virtual void applyHintsToPreciseChecker() override ;

        private:
            SparseMdpParameterLiftingModelChecker<SparseModelType, ImpreciseType> impreciseChecker;
            SparseMdpParameterLiftingModelChecker<SparseModelType, PreciseType> preciseChecker;
            
        };
    }
}
