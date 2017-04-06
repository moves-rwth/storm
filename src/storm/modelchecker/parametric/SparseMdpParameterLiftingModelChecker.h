#pragma once

#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "storm/modelchecker/parametric/SparseParameterLiftingModelChecker.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/sparse/StateType.h"
#include "storm/solver/GameSolver.h"
#include "storm/transformer/ParameterLifter.h"
#include "storm/storage/TotalScheduler.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {
            
            template <typename SparseModelType, typename ConstantType>
            class SparseMdpParameterLiftingModelChecker : public SparseParameterLiftingModelChecker<SparseModelType, ConstantType> {
            public:
                SparseMdpParameterLiftingModelChecker(SparseModelType const& parametricModel);
                SparseMdpParameterLiftingModelChecker(SparseModelType const& parametricModel, std::unique_ptr<storm::solver::GameSolverFactory<ConstantType>>&& solverFactory);
                
                virtual bool canHandle(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) const override;
                
                boost::optional<storm::storage::TotalScheduler>& getCurrentMinScheduler();
                boost::optional<storm::storage::TotalScheduler>& getCurrentMaxScheduler();
                boost::optional<storm::storage::TotalScheduler>& getCurrentPlayer1Scheduler();
                
            protected:
                
                virtual void specifyBoundedUntilFormula(CheckTask<storm::logic::BoundedUntilFormula, ConstantType> const& checkTask) override;
                virtual void specifyUntilFormula(CheckTask<storm::logic::UntilFormula, ConstantType> const& checkTask) override;
                virtual void specifyReachabilityRewardFormula(CheckTask<storm::logic::EventuallyFormula, ConstantType> const& checkTask) override;
                virtual void specifyCumulativeRewardFormula(CheckTask<storm::logic::CumulativeRewardFormula, ConstantType> const& checkTask) override;
                
                virtual std::unique_ptr<CheckResult> computeQuantitativeValues(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters) override;
                
                virtual void reset() override;
                

            private:
                void computePlayer1Matrix();
                
                storm::storage::BitVector maybeStates;
                std::vector<ConstantType> resultsForNonMaybeStates;
                boost::optional<uint_fast64_t> stepBound;
                
                storm::storage::SparseMatrix<storm::storage::sparse::state_type> player1Matrix;
                std::unique_ptr<storm::transformer::ParameterLifter<typename SparseModelType::ValueType, ConstantType>> parameterLifter;
                std::unique_ptr<storm::solver::GameSolverFactory<ConstantType>> solverFactory;
                
                // Results from the most recent solver call.
                boost::optional<storm::storage::TotalScheduler> minSched, maxSched;
                boost::optional<storm::storage::TotalScheduler> player1Sched;
                std::vector<ConstantType> x;
                boost::optional<ConstantType> lowerResultBound, upperResultBound;
                bool applyPreviousResultAsHint;
            };
        }
    }
}
