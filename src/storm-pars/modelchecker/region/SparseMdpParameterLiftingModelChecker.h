#pragma once

#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "storm-pars/transformer/ParameterLifter.h"
#include "storm-pars/modelchecker/region/SparseParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/instantiation/SparseMdpInstantiationModelChecker.h"

#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/sparse/StateType.h"
#include "storm/solver/GameSolver.h"
#include "storm/storage/Scheduler.h"

namespace storm {
    namespace modelchecker {
        
        template <typename SparseModelType, typename ConstantType>
        class SparseMdpParameterLiftingModelChecker : public SparseParameterLiftingModelChecker<SparseModelType, ConstantType> {
        public:
            SparseMdpParameterLiftingModelChecker();
            SparseMdpParameterLiftingModelChecker(std::unique_ptr<storm::solver::GameSolverFactory<ConstantType>>&& solverFactory);
            virtual ~SparseMdpParameterLiftingModelChecker() = default;
            
            virtual bool canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) const override;
            virtual void specify(Environment const& env, std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask,  bool generateRegionSplitEstimates = false, bool allowModelSimplification = true) override;
            void specify_internal(Environment const &env,
                                  std::shared_ptr<SparseModelType> parametricModel,
                                  CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const &checkTask,
                                  bool skipModelSimplification);

            boost::optional<storm::storage::Scheduler<ConstantType>> getCurrentMinScheduler();
            boost::optional<storm::storage::Scheduler<ConstantType>> getCurrentMaxScheduler();
            boost::optional<storm::storage::Scheduler<ConstantType>> getCurrentPlayer1Scheduler();
                
        protected:
                
            virtual void specifyBoundedUntilFormula(const CheckTask <storm::logic::BoundedUntilFormula, ConstantType> &checkTask) override;
            virtual void specifyUntilFormula(Environment const& env, CheckTask<storm::logic::UntilFormula, ConstantType> const& checkTask) override;
            virtual void specifyReachabilityRewardFormula(Environment const& env, CheckTask<storm::logic::EventuallyFormula, ConstantType> const& checkTask) override;
            virtual void specifyCumulativeRewardFormula(const CheckTask <storm::logic::CumulativeRewardFormula, ConstantType> &checkTask) override;
                
            virtual storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>& getInstantiationChecker() override;

            virtual std::unique_ptr<CheckResult> computeQuantitativeValues(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters, std::shared_ptr<storm::analysis::LocalMonotonicityResult<typename RegionModelChecker<typename SparseModelType::ValueType>::VariableType>> localMonotonicityResult = nullptr) override;
                
            virtual void reset() override;
                

        private:
            void computePlayer1Matrix(boost::optional<storm::storage::BitVector> const& selectedRows = boost::none);
            
            storm::storage::BitVector maybeStates;
            std::vector<ConstantType> resultsForNonMaybeStates;
            boost::optional<uint_fast64_t> stepBound;
                
            std::unique_ptr<storm::modelchecker::SparseMdpInstantiationModelChecker<SparseModelType, ConstantType>> instantiationChecker;
                
            storm::storage::SparseMatrix<storm::storage::sparse::state_type> player1Matrix;
            std::unique_ptr<storm::transformer::ParameterLifter<typename SparseModelType::ValueType, ConstantType>> parameterLifter;
            std::unique_ptr<storm::solver::GameSolverFactory<ConstantType>> solverFactory;
                
            // Results from the most recent solver call.
            boost::optional<std::vector<uint_fast64_t>> minSchedChoices, maxSchedChoices;
            boost::optional<std::vector<uint_fast64_t>> player1SchedChoices;
            std::vector<ConstantType> x;
            boost::optional<ConstantType> lowerResultBound, upperResultBound;
            bool applyPreviousResultAsHint;
        };
    }
}
