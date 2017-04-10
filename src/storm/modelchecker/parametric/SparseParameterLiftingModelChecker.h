#pragma once

#include "storm/logic/Formulas.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/storage/ParameterRegion.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/utility/parametric.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {
            
            /*!
             * Class to approximatively check a formula on a parametric model for all parameter valuations within a region
             * It is assumed that all considered valuations are graph-preserving and well defined, i.e.,
             * * all non-const transition probabilities evaluate to some non-zero value
             * * the sum of all outgoing transitions is one
             */
            template <typename SparseModelType, typename ConstantType>
            class SparseParameterLiftingModelChecker {
            public:
                SparseParameterLiftingModelChecker(SparseModelType const& parametricModel);
                virtual ~SparseParameterLiftingModelChecker() = default;
                
                virtual bool canHandle(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) const = 0;
                
                void specifyFormula(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask);
    
                /*!
                 * Checks the specified formula on the given region by applying parameter lifting (Parameter choices are lifted to nondeterministic choices)
                 * This yields a (sound) approximative model checking result.

                 * @param region the region on which parameter lifting is applied
                 * @param dirForParameters  The optimization direction for the parameter choices. If this is, e.g., minimize, then the returned result will be a lower bound for all results induced by the parameter evaluations inside the region.
                 */
                std::unique_ptr<CheckResult> check(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters);
                
            protected:
                
                virtual void specifyBoundedUntilFormula(CheckTask<storm::logic::BoundedUntilFormula, ConstantType> const& checkTask);
                virtual void specifyUntilFormula(CheckTask<storm::logic::UntilFormula, ConstantType> const& checkTask);
                virtual void specifyReachabilityProbabilityFormula(CheckTask<storm::logic::EventuallyFormula, ConstantType> const& checkTask);
                virtual void specifyReachabilityRewardFormula(CheckTask<storm::logic::EventuallyFormula, ConstantType> const& checkTask);
                virtual void specifyCumulativeRewardFormula(CheckTask<storm::logic::CumulativeRewardFormula, ConstantType> const& checkTask);
                
                virtual std::unique_ptr<CheckResult> computeQuantitativeValues(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters) = 0;
                
                virtual void reset() = 0;
                
                SparseModelType const& parametricModel;
                std::unique_ptr<CheckTask<storm::logic::Formula, ConstantType>> currentCheckTask;
              
            private:
                // store the current formula. Note that currentCheckTask only stores a reference to the formula.
                std::shared_ptr<storm::logic::Formula const> currentFormula;
            };
        }
    }
}
