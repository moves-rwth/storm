#include "storm/transformer/AddUncertainty.h"
#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

namespace storm::transformer {

template<typename ValueType>
AddUncertainty<ValueType>::AddUncertainty(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& originalModel) : origModel(originalModel) {}

template<typename ValueType>
std::shared_ptr<storm::models::sparse::Model<Interval>> AddUncertainty<ValueType>::transform(double additiveUncertainty, double minimalTransitionProbability) {
    storage::SparseMatrixBuilder<storm::Interval> newMatrixBuilder =
        storage::SparseMatrixBuilder<storm::Interval>(origModel->getTransitionMatrix().getRowCount(), origModel->getTransitionMatrix().getColumnCount(),
                                                      origModel->getTransitionMatrix().getNonzeroEntryCount());
    for (uint64_t state = 0; state < origModel->getNumberOfStates(); ++state) {
        uint64_t startOfRowGroup;
        uint64_t endOfRowGroup;
        if (!origModel->getTransitionMatrix().hasTrivialRowGrouping()) {
            startOfRowGroup = origModel->getTransitionMatrix().getRowGroupIndices()[state];
            endOfRowGroup = origModel->getTransitionMatrix().getRowGroupIndices()[state + 1];
            newMatrixBuilder.newRowGroup(startOfRowGroup);
        } else {
            startOfRowGroup = state;
            endOfRowGroup = state + 1;
        }

        for (auto row : origModel->getTransitionMatrix().getRowGroupIndices(state)) {
            for (auto const& entry : origModel->getTransitionMatrix().getRow(row)) {
                newMatrixBuilder.addNextValue(row, entry.getColumn(), addUncertainty(entry.getValue(), additiveUncertainty, minimalTransitionProbability));
            }
        }
    }
    storm::storage::sparse::ModelComponents<storm::Interval> modelComponents(newMatrixBuilder.build(), origModel->getStateLabeling());
    std::unordered_map<std::string, models::sparse::StandardRewardModel<storm::Interval>> newRewardModels;
    for (auto const& rewModel : origModel->getRewardModels()) {
        auto const& oldRewModel = rewModel.second;
        std::optional<std::vector<storm::Interval>> stateRewards;
        std::optional<std::vector<storm::Interval>> stateActionRewards;
        if (oldRewModel.hasStateRewards()) {
            stateRewards = utility::vector::convertNumericVector<storm::Interval>(oldRewModel.getStateRewardVector());
        }
        if (oldRewModel.hasStateActionRewards()) {
            stateActionRewards = utility::vector::convertNumericVector<storm::Interval>(oldRewModel.getStateRewardVector());
        }
        STORM_LOG_THROW(oldRewModel.hasTransitionRewards(), exceptions::NotImplementedException, "Transition rewards are not supported.");
        models::sparse::StandardRewardModel<storm::Interval> newRewModel(stateRewards, stateActionRewards);
        newRewardModels.emplace(rewModel.first, std::move(newRewModel));
    }

    modelComponents.rewardModels = newRewardModels;
    modelComponents.stateValuations = origModel->getOptionalStateValuations();
    modelComponents.choiceLabeling = origModel->getOptionalChoiceLabeling();
    modelComponents.choiceOrigins = origModel->getOptionalChoiceOrigins();

    switch (origModel->getType()) {
        case storm::models::ModelType::Dtmc:
            return std::make_shared<storm::models::sparse::Dtmc<storm::Interval>>(std::move(modelComponents));
        case storm::models::ModelType::Mdp:
            return std::make_shared<storm::models::sparse::Mdp<storm::Interval>>(std::move(modelComponents));
        default:
            STORM_LOG_THROW(false, exceptions::NotImplementedException, "Only DTMC and MDP model types are currently supported.");
    }
}

template<typename ValueType>
storm::Interval AddUncertainty<ValueType>::addUncertainty(ValueType const& vt, double additiveUncertainty, double minimalValue) {
    if (utility::isOne(vt)) {
        return storm::Interval(1.0, 1.0);
    }
    double center = storm::utility::convertNumber<double>(vt);
    STORM_LOG_THROW(center >= minimalValue, storm::exceptions::InvalidArgumentException, "Transition probability is smaller than minimal value");
    double lowerBound = std::max(center - additiveUncertainty, minimalValue);
    double upperBound = std::min(center + additiveUncertainty, 1.0 - minimalValue);
    STORM_LOG_ASSERT(lowerBound > 0, "Lower bound must be strictly above zero.");
    STORM_LOG_ASSERT(upperBound < 1, "Upper bound must be strictly below one.");
    return storm::Interval(lowerBound, upperBound);
}

template class AddUncertainty<double>;
}  // namespace storm::transformer