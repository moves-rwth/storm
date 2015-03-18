#include "src/modelchecker/csl/SparseCtmcCslModelChecker.h"

#include <vector>

#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"
#include "src/utility/solver.h"

#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        template<class ValueType>
        SparseCtmcCslModelChecker<ValueType>::SparseCtmcCslModelChecker(storm::models::Ctmc<ValueType> const& model) : SparsePropositionalModelChecker<ValueType>(model), linearEquationSolver(storm::utility::solver::getLinearEquationSolver<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<class ValueType>
        SparseCtmcCslModelChecker<ValueType>::SparseCtmcCslModelChecker(storm::models::Ctmc<ValueType> const& model, std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>&& linearEquationSolver) : SparsePropositionalModelChecker<ValueType>(model), linearEquationSolver(std::move(linearEquationSolver)) {
            // Intentionally left empty.
        }
        
        template<class ValueType>
        bool SparseCtmcCslModelChecker<ValueType>::canHandle(storm::logic::Formula const& formula) const {
            // FIXME: refine.
            return formula.isCslStateFormula() || formula.isCslPathFormula() || formula.isRewardPathFormula();
        }
        
        template<class ValueType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<ValueType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(pathFormula.isIntervalBounded(), storm::exceptions::InvalidPropertyException, "Cannot treat non-interval bounded until.");
            
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();;
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::pair<double, double> const& intervalBounds =  pathFormula.getIntervalBounds();
            std::unique_ptr<CheckResult> result = std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(this->computeBoundedUntilProbabilitiesHelper(leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), intervalBounds.first, intervalBounds.second)));
            
            return result;
        }
        
        template<class ValueType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<ValueType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> result = SparseDtmcPrctlModelChecker<ValueType>::computeNextProbabilitiesHelper(this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), *this->linearEquationSolver);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
        }
        
        template<class ValueType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<ValueType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> result = SparseDtmcPrctlModelChecker<ValueType>::computeUntilProbabilitiesHelper(this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, *this->linearEquationSolver);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
        }
        
//        template<class ValueType>
//        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<ValueType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
//            
//        }
        
        template<class ValueType>
        storm::models::Ctmc<ValueType> const& SparseCtmcCslModelChecker<ValueType>::getModel() const {
            return this->template getModelAs<storm::models::Ctmc<ValueType>>();
        }
        
        template<class ValueType>
        std::vector<ValueType> SparseCtmcCslModelChecker<ValueType>::computeBoundedUntilProbabilitiesHelper(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, double lowerBound, double upperBound) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "not yet implemented, sorry");
        }
        
//        template<class ValueType>
//        std::vector<ValueType> SparseCtmcCslModelChecker<ValueType>::computeReachabilityRewardsHelper(storm::storage::BitVector const& targetStates, bool qualitative) const {
//            
//        }
        
    } // namespace modelchecker
} // namespace storm