#ifndef STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/models/sparse/Ctmc.h"
#include "src/solver/LinearEquationSolver.h"

namespace storm {
    namespace modelchecker {
        
        template<class ValueType>
        class SparseCtmcCslModelChecker : public SparsePropositionalModelChecker<ValueType> {
        public:
            explicit SparseCtmcCslModelChecker(storm::models::sparse::Ctmc<ValueType> const& model);
            explicit SparseCtmcCslModelChecker(storm::models::sparse::Ctmc<ValueType> const& model, std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>&& linearEquationSolver);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;

        protected:
            storm::models::sparse::Ctmc<ValueType> const& getModel() const override;
            
        private:
            // The methods that perform the actual checking.
            std::vector<ValueType> computeBoundedUntilProbabilitiesHelper(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<ValueType> const& exitRates, bool qualitative, double lowerBound, double upperBound) const;
            static std::vector<ValueType> computeUntilProbabilitiesHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::LinearEquationSolver<ValueType> const& linearEquationSolver);
            std::vector<ValueType> computeInstantaneousRewardsHelper(double timeBound) const;
            std::vector<ValueType> computeCumulativeRewardsHelper(double timeBound) const;
            std::vector<ValueType> computeReachabilityRewardsHelper(storm::storage::BitVector const& targetStates, bool qualitative) const;

            /*!
             * Computes the matrix representing the transitions of the uniformized CTMC.
             *
             * @param transitionMatrix The matrix to uniformize.
             * @param maybeStates The states that need to be considered.
             * @param absorbingStates The states that need to be made absorbing.
             * @param uniformizationRate The rate to be used for uniformization.
             * @param exitRates The exit rates of all states.
             * @return The uniformized matrix.
             */
            static storm::storage::SparseMatrix<ValueType> computeUniformizedMatrix(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& maybeStates, storm::storage::BitVector const& absorbingStates, ValueType uniformizationRate, std::vector<ValueType> const& exitRates);
            
            /*!
             * Computes the transient probabilities for lambda time steps.
             *
             * @param uniformizedMatrix The uniformized transition matrix.
             * @param timeBound The time bound to use.
             * @param uniformizationRate The used uniformization rate.
             * @param values A vector mapping each state to an initial probability.
             * @param linearEquationSolver The linear equation solver to use.
             * @param useMixedPoissonProbabilities If set to true, instead of taking the poisson probabilities,  mixed
             * poisson probabilities are used.
             * @return The vector of transient probabilities.
             */
            template<bool useMixedPoissonProbabilities = false>
            std::vector<ValueType> computeTransientProbabilities(storm::storage::SparseMatrix<ValueType> const& uniformizedMatrix, ValueType timeBound, ValueType uniformizationRate, std::vector<ValueType> values, storm::solver::LinearEquationSolver<ValueType> const& linearEquationSolver) const;
            
            /*!
             * Converts the given rate-matrix into a time-abstract probability matrix.
             *
             * @param rateMatrix The rate matrix.
             * @param exitRates The exit rate vector.
             */
            static storm::storage::SparseMatrix<ValueType> computeProbabilityMatrix(storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& exitRates);
            
            // An object that is used for solving linear equations and performing matrix-vector multiplication.
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> linearEquationSolver;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_ */
