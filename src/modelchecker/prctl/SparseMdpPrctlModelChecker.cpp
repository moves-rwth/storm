#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"

#include <vector>
#include <memory>

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/solver/LpSolver.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/storage/expressions/Expressions.h"

#include "src/storage/MaximalEndComponentDecomposition.h"

namespace storm {
    namespace modelchecker {
        template<typename SparseMdpModelType>
        SparseMdpPrctlModelChecker<SparseMdpModelType>::SparseMdpPrctlModelChecker(SparseMdpModelType const& model) : SparsePropositionalModelChecker<SparseMdpModelType>(model), MinMaxLinearEquationSolverFactory(new storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<typename SparseMdpModelType>
        SparseMdpPrctlModelChecker<SparseMdpModelType>::SparseMdpPrctlModelChecker(SparseMdpModelType const& model, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>>&& MinMaxLinearEquationSolverFactory) : SparsePropositionalModelChecker<SparseMdpModelType>(model), MinMaxLinearEquationSolverFactory(std::move(MinMaxLinearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename SparseMdpModelType>
        bool SparseMdpPrctlModelChecker<SparseMdpModelType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isPctlStateFormula() || formula.isPctlPathFormula() || formula.isRewardPathFormula();
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::unique_ptr<CheckResult> result = std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeBoundedUntilProbabilitiesHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getDiscreteTimeBound())));
            return result;
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeNextProbabilitiesHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, subResult.getTruthValuesVector())));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(SparseMdpPrctlModelChecker<SparseMdpModelType>::computeUntilProbabilitiesHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), *MinMaxLinearEquationSolverFactory, qualitative)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeCumulativeRewardsHelper(rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), optimalityType.get() == storm::logic::OptimalityType::Minimize, rewardPathFormula.getDiscreteTimeBound())));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeInstantaneousRewardsHelper(rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), optimalityType.get() == storm::logic::OptimalityType::Minimize, rewardPathFormula.getDiscreteTimeBound())));
        }
                
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeReachabilityRewardsHelper(rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), optimalityType.get() == storm::logic::OptimalityType::Minimize, this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), subResult.getTruthValuesVector(), *this->MinMaxLinearEquationSolverFactory, qualitative)));
        }

		template<typename SparseMdpModelType>
		std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
			STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
			
			std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
			ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
			
			return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeLongRunAverageHelper(optimalityType.get() == storm::logic::OptimalityType::Minimize, subResult.getTruthValuesVector(), qualitative)));
		}
                
        template class SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>>;
    }
}