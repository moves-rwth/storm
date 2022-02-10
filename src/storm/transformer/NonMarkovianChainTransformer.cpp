#include "NonMarkovianChainTransformer.h"

#include <queue>

#include <storm/solver/stateelimination/NondeterministicModelStateEliminator.h>
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/InvalidModelException.h"
#include "storm/logic/Formulas.h"
#include "storm/logic/FragmentSpecification.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/storage/FlexibleSparseMatrix.h"
#include "storm/storage/sparse/ModelComponents.h"
#include "storm/utility/constants.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

namespace storm {
namespace transformer {

template<typename ValueType, typename RewardModelType>
std::shared_ptr<models::sparse::Model<ValueType, RewardModelType>> NonMarkovianChainTransformer<ValueType, RewardModelType>::eliminateNonmarkovianStates(
    std::shared_ptr<models::sparse::MarkovAutomaton<ValueType, RewardModelType>> ma, EliminationLabelBehavior labelBehavior) {
    STORM_LOG_WARN_COND(labelBehavior == EliminationLabelBehavior::KeepLabels,
                        "Labels are not preserved! Results may be incorrect. Continue at your own caution.");
    STORM_LOG_THROW(ma->isClosed(), storm::exceptions::InvalidModelException, "MA should be closed first.");

    if (ma->getMarkovianStates().full()) {
        // Is already a CTMC
        storm::storage::sparse::ModelComponents<ValueType, RewardModelType> components(ma->getTransitionMatrix(), ma->getStateLabeling(), ma->getRewardModels(),
                                                                                       false);
        components.exitRates = ma->getExitRates();
        if (ma->hasChoiceLabeling()) {
            components.choiceLabeling = ma->getChoiceLabeling();
        }
        if (ma->hasStateValuations()) {
            components.stateValuations = ma->getStateValuations();
        }
        if (ma->hasChoiceOrigins()) {
            components.choiceOrigins = ma->getChoiceOrigins();
        }
        return std::make_shared<storm::models::sparse::Ctmc<ValueType, RewardModelType>>(std::move(components));
    }

    // Initialize
    storm::storage::FlexibleSparseMatrix<ValueType> flexibleMatrix(ma->getTransitionMatrix());
    storm::storage::FlexibleSparseMatrix<ValueType> flexibleBackwardTransitions(ma->getTransitionMatrix().transpose(), true);
    storm::models::sparse::StateLabeling stateLabeling = ma->getStateLabeling();
    // TODO: update reward models and choice labels according to kept states
    STORM_LOG_WARN_COND(ma->getRewardModels().empty(), "Reward models are not preserved in chain elimination.");
    std::unordered_map<std::string, RewardModelType> rewardModels;
    STORM_LOG_WARN_COND(!ma->hasChoiceLabeling(), "Choice labels are not preserved in chain elimination.");
    STORM_LOG_WARN_COND(!ma->hasStateValuations(), "State valuations are not preserved in chain elimination.");
    STORM_LOG_WARN_COND(!ma->hasChoiceOrigins(), "Choice origins are not preserved in chain elimination.");

    // Eliminate all probabilistic states by state elimination
    auto actionRewards = std::vector<ValueType>(ma->getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
    storm::solver::stateelimination::NondeterministicModelStateEliminator<ValueType> stateEliminator(flexibleMatrix, flexibleBackwardTransitions,
                                                                                                     actionRewards);
    storm::storage::BitVector keepStates(ma->getNumberOfStates(), true);

    for (uint_fast64_t state = 0; state < ma->getNumberOfStates(); ++state) {
        STORM_LOG_ASSERT(!ma->isHybridState(state), "State is hybrid.");
        if (ma->isProbabilisticState(state)) {
            // Only eliminate immediate states (and no Markovian states)
            if (ma->getNumberOfChoices(state) <= 1) {
                // Eliminate only if no non-determinism occurs
                STORM_LOG_ASSERT(ma->getNumberOfChoices(state) == 1, "State " << state << " has no choices.");
                STORM_LOG_ASSERT(flexibleMatrix.getRowGroupSize(state) == 1, "State " << state << " has too many rows.");

                if (labelBehavior == EliminationLabelBehavior::KeepLabels) {
                    // Only eliminate if eliminated state and all its successors have the same labels
                    bool sameLabels = true;
                    auto currLabels = stateLabeling.getLabelsOfState(state);
                    typename storm::storage::FlexibleSparseMatrix<ValueType>::row_type entriesInRow = flexibleMatrix.getRow(state, 0);  // Row group
                    for (auto entryIt = entriesInRow.begin(); entryIt != entriesInRow.end(); ++entryIt) {
                        if (currLabels != stateLabeling.getLabelsOfState(entryIt->getColumn())) {
                            STORM_LOG_TRACE("Do not eliminate state " << state << " because labels of state " << entryIt->getColumn() << " are different.");
                            sameLabels = false;
                            break;
                        }
                    }
                    if (!sameLabels) {
                        continue;
                    }
                } else {
                    // As helper for the labeling we create a bitvector representing all successor states
                    storm::storage::BitVector successors(stateLabeling.getNumberOfItems());
                    typename storm::storage::FlexibleSparseMatrix<ValueType>::row_type entriesInRow = flexibleMatrix.getRow(state, 0);  // Row group
                    for (auto entryIt = entriesInRow.begin(); entryIt != entriesInRow.end(); ++entryIt) {
                        successors.set(entryIt->getColumn());
                    }

                    if (labelBehavior == EliminationLabelBehavior::MergeLabels) {
                        // Add labels from eliminated state to successors
                        for (std::string const& label : stateLabeling.getLabelsOfState(state)) {
                            storm::storage::BitVector states = stateLabeling.getStates(label);
                            // Add successor states for this label as well
                            stateLabeling.setStates(label, states | successors);
                        }
                    } else {
                        STORM_LOG_ASSERT(labelBehavior == EliminationLabelBehavior::DeleteLabels, "Unknown label behavior.");
                        if (stateLabeling.getStateHasLabel("init", state)) {
                            // Add "init" label of eliminated state to successor states
                            storm::storage::BitVector states = stateLabeling.getStates("init");
                            // Add successor states for this label as well
                            stateLabeling.setStates("init", states | successors);
                        }
                    }
                }

                // Eliminate this probabilistic state
                stateEliminator.eliminateState(state, true);
                keepStates.set(state, false);
            }
        }
    }

    // Create the new matrix
    auto keptRows = ma->getTransitionMatrix().getRowFilter(keepStates);
    storm::storage::SparseMatrix<ValueType> matrix = flexibleMatrix.createSparseMatrix(keptRows, keepStates);

    // TODO: obtain the reward model for the resulting system

    // Prepare model components
    storm::storage::BitVector markovianStates = ma->getMarkovianStates() % keepStates;
    storm::models::sparse::StateLabeling labeling = stateLabeling.getSubLabeling(keepStates);
    storm::storage::sparse::ModelComponents<ValueType, RewardModelType> components(matrix, labeling, ma->getRewardModels(), false, markovianStates);
    std::vector<ValueType> exitRates(markovianStates.size());
    storm::utility::vector::selectVectorValues(exitRates, keepStates, ma->getExitRates());
    components.exitRates = exitRates;

    // Build transformed model
    auto model = std::make_shared<storm::models::sparse::MarkovAutomaton<ValueType, RewardModelType>>(std::move(components));
    if (model->isConvertibleToCtmc()) {
        return model->convertToCtmc();
    } else {
        return model;
    }
}

template<typename ValueType, typename RewardModelType>
bool NonMarkovianChainTransformer<ValueType, RewardModelType>::preservesFormula(storm::logic::Formula const& formula) {
    storm::logic::FragmentSpecification fragment = storm::logic::propositional();

    fragment.setProbabilityOperatorsAllowed(true);
    fragment.setGloballyFormulasAllowed(true);
    fragment.setReachabilityProbabilityFormulasAllowed(true);
    fragment.setUntilFormulasAllowed(true);
    fragment.setBoundedUntilFormulasAllowed(true);
    fragment.setTimeBoundedUntilFormulasAllowed(true);

    fragment.setNextFormulasAllowed(false);
    fragment.setStepBoundedUntilFormulasAllowed(false);

    return formula.isInFragment(fragment);
}

template<typename ValueType, typename RewardModelType>
std::vector<std::shared_ptr<storm::logic::Formula const>> NonMarkovianChainTransformer<ValueType, RewardModelType>::checkAndTransformFormulas(
    std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
    std::vector<std::shared_ptr<storm::logic::Formula const>> result;

    for (auto const& f : formulas) {
        if (preservesFormula(*f)) {
            result.push_back(f);
        } else {
            STORM_LOG_WARN("Non-Markovian chain elimination does not preserve formula " << *f);
        }
    }
    return result;
}

template class NonMarkovianChainTransformer<double>;

template class NonMarkovianChainTransformer<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;

#ifdef STORM_HAVE_CARL
template class NonMarkovianChainTransformer<storm::RationalFunction>;

template class NonMarkovianChainTransformer<storm::RationalNumber>;

#endif

}  // namespace transformer
}  // namespace storm
