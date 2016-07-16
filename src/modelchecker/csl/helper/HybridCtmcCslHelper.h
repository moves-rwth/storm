#ifndef STORM_MODELCHECKER_HYBRID_CTMC_CSL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_HYBRID_CTMC_CSL_MODELCHECKER_HELPER_H_

#include <memory>

#include "src/models/symbolic/Ctmc.h"

#include "src/modelchecker/results/CheckResult.h"

#include "src/solver/LinearEquationSolver.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class HybridCtmcCslHelper {
            public:
                typedef typename storm::models::symbolic::Model<DdType, ValueType>::RewardModelType RewardModelType;

                static std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, double lowerBound, double upperBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, RewardModelType const& rewardModel, double timeBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeCumulativeRewards(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, RewardModelType const& rewardModel, double timeBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeUntilProbabilities(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeReachabilityRewards(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeNextProbabilities(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& nextStates);

                /*!
                 * Converts the given rate-matrix into a time-abstract probability matrix.
                 *
                 * @param model The symbolic model.
                 * @param rateMatrix The rate matrix.
                 * @param exitRateVector The exit rate vector of the model.
                 * @return The probability matrix.
                 */
                static storm::dd::Add<DdType, ValueType> computeProbabilityMatrix(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector);
                
                /*!
                 * Computes the matrix representing the transitions of the uniformized CTMC.
                 *
                 * @param model The symbolic model.
                 * @param transitionMatrix The matrix to uniformize.
                 * @param exitRateVector The exit rate vector.
                 * @param maybeStates The states that need to be considered.
                 * @param uniformizationRate The rate to be used for uniformization.
                 * @return The uniformized matrix.
                 */
                static storm::dd::Add<DdType, ValueType> computeUniformizedMatrix(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& maybeStates, ValueType uniformizationRate);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_HYBRID_CTMC_CSL_MODELCHECKER_HELPER_H_ */