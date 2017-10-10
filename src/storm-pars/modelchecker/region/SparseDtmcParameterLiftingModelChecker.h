#pragma once

#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "storm-pars/transformer/ParameterLifter.h"
#include "storm-pars/modelchecker/region/SparseParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/instantiation/SparseDtmcInstantiationModelChecker.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/Scheduler.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/logic/FragmentSpecification.h"

namespace storm {
    namespace modelchecker {
            
        template <typename SparseModelType, typename ConstantType>
        class SparseDtmcParameterLiftingModelChecker : public SparseParameterLiftingModelChecker<SparseModelType, ConstantType> {
        public:
            SparseDtmcParameterLiftingModelChecker();
            SparseDtmcParameterLiftingModelChecker(std::unique_ptr<storm::solver::MinMaxLinearEquationSolverFactory<ConstantType>>&& solverFactory);
            virtual ~SparseDtmcParameterLiftingModelChecker() = default;
            
            virtual bool canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) const override;
            virtual void specify(std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) override;
            void specify(std::shared_ptr<SparseModelType> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask, bool skipModelSimplification);
            
            boost::optional<storm::storage::Scheduler<ConstantType>> getCurrentMinScheduler();
            boost::optional<storm::storage::Scheduler<ConstantType>> getCurrentMaxScheduler();

        protected:
                
            virtual void specifyBoundedUntilFormula(CheckTask<storm::logic::BoundedUntilFormula, ConstantType> const& checkTask) override;
            virtual void specifyUntilFormula(CheckTask<storm::logic::UntilFormula, ConstantType> const& checkTask) override;
            virtual void specifyReachabilityRewardFormula(CheckTask<storm::logic::EventuallyFormula, ConstantType> const& checkTask) override;
            virtual void specifyCumulativeRewardFormula(CheckTask<storm::logic::CumulativeRewardFormula, ConstantType> const& checkTask) override;

            virtual storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>& getInstantiationChecker() override;
                
            virtual std::unique_ptr<CheckResult> computeQuantitativeValues(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters) override;
                
            virtual void reset() override;
                
        private:

            
            storm::storage::BitVector maybeStates;
            std::vector<ConstantType> resultsForNonMaybeStates;
            boost::optional<uint_fast64_t> stepBound;
            
            std::unique_ptr<storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>> instantiationChecker;
            
            std::unique_ptr<storm::transformer::ParameterLifter<typename SparseModelType::ValueType, ConstantType>> parameterLifter;
            std::unique_ptr<storm::solver::MinMaxLinearEquationSolverFactory<ConstantType>> solverFactory;
            bool solvingRequiresUpperRewardBounds;
            
            // Results from the most recent solver call.
            boost::optional<std::vector<uint_fast64_t>> minSchedChoices, maxSchedChoices;
            std::vector<ConstantType> x;
            boost::optional<ConstantType> lowerResultBound, upperResultBound;
        };
    }
}
