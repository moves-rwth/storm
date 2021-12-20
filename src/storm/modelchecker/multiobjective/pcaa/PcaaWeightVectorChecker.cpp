#include "storm/modelchecker/multiobjective/pcaa/PcaaWeightVectorChecker.h"

#include "storm/modelchecker/multiobjective/pcaa/RewardBoundedMdpPcaaWeightVectorChecker.h"
#include "storm/modelchecker/multiobjective/pcaa/StandardMaPcaaWeightVectorChecker.h"
#include "storm/modelchecker/multiobjective/pcaa/StandardMdpPcaaWeightVectorChecker.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace modelchecker {
namespace multiobjective {

template<typename ModelType>
PcaaWeightVectorChecker<ModelType>::PcaaWeightVectorChecker(std::vector<Objective<ValueType>> const& objectives)
    : objectives(objectives), weightedPrecision(storm::utility::zero<ValueType>()) {
    // Intentionally left empty
}

template<typename ModelType>
void PcaaWeightVectorChecker<ModelType>::setWeightedPrecision(ValueType const& value) {
    weightedPrecision = value;
}

template<typename ModelType>
typename PcaaWeightVectorChecker<ModelType>::ValueType const& PcaaWeightVectorChecker<ModelType>::getWeightedPrecision() const {
    return weightedPrecision;
}

template<typename ModelType>
storm::storage::Scheduler<typename PcaaWeightVectorChecker<ModelType>::ValueType> PcaaWeightVectorChecker<ModelType>::computeScheduler() const {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Scheduler generation is not supported in this setting.");
}

template<class SparseModelType>
boost::optional<typename SparseModelType::ValueType> PcaaWeightVectorChecker<SparseModelType>::computeWeightedResultBound(
    bool lower, std::vector<ValueType> const& weightVector, storm::storage::BitVector const& objectiveFilter) const {
    ValueType result = storm::utility::zero<ValueType>();
    for (auto objIndex : objectiveFilter) {
        boost::optional<ValueType> const& objBound = (lower == storm::solver::minimize(this->objectives[objIndex].formula->getOptimalityType()))
                                                         ? this->objectives[objIndex].upperResultBound
                                                         : this->objectives[objIndex].lowerResultBound;
        if (objBound) {
            if (storm::solver::minimize(this->objectives[objIndex].formula->getOptimalityType())) {
                result -= objBound.get() * weightVector[objIndex];
            } else {
                result += objBound.get() * weightVector[objIndex];
            }
        } else {
            // If there is an objective without the corresponding bound we can not give guarantees for the weighted sum
            return boost::none;
        }
    }
    return result;
}

template<typename ModelType>
template<typename VT, typename std::enable_if<std::is_same<ModelType, storm::models::sparse::Mdp<VT>>::value, int>::type>
std::unique_ptr<PcaaWeightVectorChecker<ModelType>> WeightVectorCheckerFactory<ModelType>::create(
    preprocessing::SparseMultiObjectivePreprocessorResult<ModelType> const& preprocessorResult) {
    if (preprocessorResult.containsOnlyTrivialObjectives()) {
        return std::make_unique<StandardMdpPcaaWeightVectorChecker<ModelType>>(preprocessorResult);
    } else {
        STORM_LOG_DEBUG("Query contains reward bounded formula");
        return std::make_unique<RewardBoundedMdpPcaaWeightVectorChecker<ModelType>>(preprocessorResult);
    }
}

template<typename ModelType>
template<typename VT, typename std::enable_if<std::is_same<ModelType, storm::models::sparse::MarkovAutomaton<VT>>::value, int>::type>
std::unique_ptr<PcaaWeightVectorChecker<ModelType>> WeightVectorCheckerFactory<ModelType>::create(
    preprocessing::SparseMultiObjectivePreprocessorResult<ModelType> const& preprocessorResult) {
    return std::make_unique<StandardMaPcaaWeightVectorChecker<ModelType>>(preprocessorResult);
}

template class PcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>;
template class PcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
template class PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>;
template class PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;

template class WeightVectorCheckerFactory<storm::models::sparse::Mdp<double>>;
template class WeightVectorCheckerFactory<storm::models::sparse::Mdp<storm::RationalNumber>>;
template class WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<double>>;
template class WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;

template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>> WeightVectorCheckerFactory<storm::models::sparse::Mdp<double>>::create(
    preprocessing::SparseMultiObjectivePreprocessorResult<storm::models::sparse::Mdp<double>> const& preprocessorResult);
template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>>
WeightVectorCheckerFactory<storm::models::sparse::Mdp<storm::RationalNumber>>::create(
    preprocessing::SparseMultiObjectivePreprocessorResult<storm::models::sparse::Mdp<storm::RationalNumber>> const& preprocessorResult);
template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>>
WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<double>>::create(
    preprocessing::SparseMultiObjectivePreprocessorResult<storm::models::sparse::MarkovAutomaton<double>> const& preprocessorResult);
template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>>
WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::create(
    preprocessing::SparseMultiObjectivePreprocessorResult<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>> const& preprocessorResult);

}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm
