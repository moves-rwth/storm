#ifndef STORM_MODELCHECKER_HYBRIDCTMCCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_HYBRIDCTMCCSLMODELCHECKER_H_

#include "src/modelchecker/propositional/SymbolicPropositionalModelChecker.h"
#include "src/models/symbolic/Ctmc.h"
#include "src/utility/solver.h"
#include "src/solver/LinearEquationSolver.h"

namespace storm {
    namespace modelchecker {
        
        template<storm::dd::DdType DdType, class ValueType>
        class HybridCtmcCslModelChecker : public SymbolicPropositionalModelChecker<DdType> {
        public:
            explicit HybridCtmcCslModelChecker(storm::models::symbolic::Ctmc<DdType> const& model);
            explicit HybridCtmcCslModelChecker(storm::models::symbolic::Ctmc<DdType> const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;

        protected:
            storm::models::symbolic::Ctmc<DdType> const& getModel() const override;
            
        private:
            /*!
             * Converts the given rate-matrix into a time-abstract probability matrix.
             *
             * @param model The symbolic model.
             * @param rateMatrix The rate matrix.
             * @param exitRateVector The exit rate vector of the model.
             * @return The probability matrix.
             */
            static storm::dd::Add<DdType> computeProbabilityMatrix(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& rateMatrix, storm::dd::Add<DdType> const& exitRateVector);

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
            static storm::dd::Add<DdType> computeUniformizedMatrix(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Add<DdType> const& exitRateVector, storm::dd::Bdd<DdType> const& maybeStates, ValueType uniformizationRate);
            
            // The methods that perform the actual checking.
            std::unique_ptr<CheckResult> computeBoundedUntilProbabilitiesHelper(storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, storm::dd::Add<DdType> const& exitRates, bool qualitative, double lowerBound, double upperBound) const;
            std::unique_ptr<CheckResult> computeInstantaneousRewardsHelper(double timeBound) const;
            std::unique_ptr<CheckResult> computeCumulativeRewardsHelper(double timeBound) const;
            
            // An object that is used for solving linear equations and performing matrix-vector multiplication.
            std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_HYBRIDCTMCCSLMODELCHECKER_H_ */
