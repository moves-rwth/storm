#include "storm-pomdp/transformer/ObservationTraceUnfolder.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/utility/ConstantsComparator.h"

#undef _VERBOSE_OBSERVATION_UNFOLDING

namespace storm {
namespace pomdp {
template<typename ValueType>
ObservationTraceUnfolder<ValueType>::ObservationTraceUnfolder(storm::models::sparse::Pomdp<ValueType> const& model, std::vector<ValueType> const& risk,
                                                              std::shared_ptr<storm::expressions::ExpressionManager>& exprManager,
                                                              ObservationTraceUnfolderOptions const& options)
    : model(model), risk(risk), exprManager(exprManager), options(options) {
    statesPerObservation = std::vector<storm::storage::BitVector>(model.getNrObservations() + 1, storm::storage::BitVector(model.getNumberOfStates()));
    for (uint64_t state = 0; state < model.getNumberOfStates(); ++state) {
        statesPerObservation[model.getObservation(state)].set(state, true);
    }
    svvar = exprManager->declareFreshIntegerVariable(false, "_s");
    tsvar = exprManager->declareFreshIntegerVariable(false, "_t");
}

template<typename ValueType>
std::shared_ptr<storm::models::sparse::Mdp<ValueType>> ObservationTraceUnfolder<ValueType>::transform(const std::vector<uint32_t>& observations) {
    std::vector<uint32_t> modifiedObservations = observations;
    // First observation should be special.
    // This just makes the algorithm simpler because we do not treat the first step as a special case later.
    // We overwrite the observation with a non-existing obs z*
    modifiedObservations[0] = model.getNrObservations();

    storm::storage::BitVector initialStates = model.getInitialStates();
    storm::storage::BitVector actualInitialStates = initialStates;
    for (uint64_t state : initialStates) {
        if (model.getObservation(state) != observations[0]) {
            actualInitialStates.set(state, false);
        }
    }
    STORM_LOG_THROW(actualInitialStates.getNumberOfSetBits() == 1, storm::exceptions::InvalidArgumentException,
                    "Must have unique initial state matching the observation");
    // For this z* that only exists in the initial state, we now also define the states for this observation.
    statesPerObservation[model.getNrObservations()] = actualInitialStates;

#ifdef _VERBOSE_OBSERVATION_UNFOLDING
    std::cout << "build valution builder..\n";
#endif
    storm::storage::sparse::StateValuationsBuilder svbuilder;
    svbuilder.addVariable(svvar);
    svbuilder.addVariable(tsvar);

    // TODO: Do we need this as ordered maps? Is it better to make them unordered?
    std::map<uint64_t, uint64_t> unfoldedToOld;
    std::map<uint64_t, uint64_t> unfoldedToOldNextStep;
    std::map<uint64_t, uint64_t> oldToUnfolded;

#ifdef _VERBOSE_OBSERVATION_UNFOLDING
    std::cout << "start buildiing matrix...\n";
#endif

    uint64_t newStateIndex = 0;
    // TODO do not add violated state if we do rejection sampling.
    uint64_t violatedState = newStateIndex;
    ++newStateIndex;
    // Add this initial state state:
    uint64_t initialState = newStateIndex;
    ++newStateIndex;

    unfoldedToOldNextStep[initialState] = actualInitialStates.getNextSetIndex(0);

    uint64_t resetDestination = options.rejectionSampling ? initialState : violatedState;  // Should be initial state for the standard semantics.
    storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, true, true);

    // TODO only add this state if it is actually reachable / rejection sampling
    // the violated state is a sink state
    transitionMatrixBuilder.newRowGroup(violatedState);
    transitionMatrixBuilder.addNextValue(violatedState, violatedState, storm::utility::one<ValueType>());
    svbuilder.addState(violatedState, {}, {-1, -1});

    // Now we are starting to build the MDP from the initial state onwards.
    uint64_t newRowGroupStart = initialState;
    uint64_t newRowCount = initialState;

    // Notice that we are going to use a special last step
    for (uint64_t step = 0; step < observations.size() - 1; ++step) {
        oldToUnfolded.clear();
        unfoldedToOld = unfoldedToOldNextStep;
        unfoldedToOldNextStep.clear();

        for (auto const& unfoldedToOldEntry : unfoldedToOld) {
            transitionMatrixBuilder.newRowGroup(newRowGroupStart);
#ifdef _VERBOSE_OBSERVATION_UNFOLDING
            std::cout << "\tconsider new state " << unfoldedToOldEntry.first << '\n';
#endif
            assert(step == 0 || newRowCount == transitionMatrixBuilder.getLastRow() + 1);
            svbuilder.addState(unfoldedToOldEntry.first, {}, {static_cast<int64_t>(unfoldedToOldEntry.second), static_cast<int64_t>(step)});
            uint64_t oldRowIndexStart = model.getNondeterministicChoiceIndices()[unfoldedToOldEntry.second];
            uint64_t oldRowIndexEnd = model.getNondeterministicChoiceIndices()[unfoldedToOldEntry.second + 1];

            for (uint64_t oldRowIndex = oldRowIndexStart; oldRowIndex != oldRowIndexEnd; oldRowIndex++) {
#ifdef _VERBOSE_OBSERVATION_UNFOLDING
                std::cout << "\t\tconsider old action " << oldRowIndex << '\n';
                std::cout << "\t\tconsider new row nr " << newRowCount << '\n';
#endif

                ValueType resetProb = storm::utility::zero<ValueType>();
                // We first find the reset probability
                for (auto const& oldRowEntry : model.getTransitionMatrix().getRow(oldRowIndex)) {
                    if (model.getObservation(oldRowEntry.getColumn()) != observations[step + 1]) {
                        resetProb += oldRowEntry.getValue();
                    }
                }
#ifdef _VERBOSE_OBSERVATION_UNFOLDING
                std::cout << "\t\t\t add reset with probability " << resetProb << '\n';
#endif

                // Add the resets
                if (resetProb != storm::utility::zero<ValueType>()) {
                    transitionMatrixBuilder.addNextValue(newRowCount, resetDestination, resetProb);
                }
#ifdef _VERBOSE_OBSERVATION_UNFOLDING
                std::cout << "\t\t\t add other transitions...\n";
#endif

                // Now, we build the outgoing transitions.
                for (auto const& oldRowEntry : model.getTransitionMatrix().getRow(oldRowIndex)) {
                    if (model.getObservation(oldRowEntry.getColumn()) != observations[step + 1]) {
                        continue;  // already handled.
                    }
                    uint64_t column = 0;

                    auto entryIt = oldToUnfolded.find(oldRowEntry.getColumn());
                    if (entryIt == oldToUnfolded.end()) {
                        column = newStateIndex;
                        oldToUnfolded[oldRowEntry.getColumn()] = column;
                        unfoldedToOldNextStep[column] = oldRowEntry.getColumn();
                        newStateIndex++;
                    } else {
                        column = entryIt->second;
                    }
#ifdef _VERBOSE_OBSERVATION_UNFOLDING
                    std::cout << "\t\t\t\t transition to " << column << "with probability " << oldRowEntry.getValue() << '\n';
#endif
                    transitionMatrixBuilder.addNextValue(newRowCount, column, oldRowEntry.getValue());
                }
                newRowCount++;
            }
            newRowGroupStart = transitionMatrixBuilder.getLastRow() + 1;
        }
    }
    // Now, take care of the last step.
    uint64_t sinkState = newStateIndex;
    uint64_t targetState = newStateIndex + 1;
    [[maybe_unused]] auto cc = storm::utility::ConstantsComparator<ValueType>();
    for (auto const& unfoldedToOldEntry : unfoldedToOldNextStep) {
        svbuilder.addState(unfoldedToOldEntry.first, {}, {static_cast<int64_t>(unfoldedToOldEntry.second), static_cast<int64_t>(observations.size() - 1)});

        transitionMatrixBuilder.newRowGroup(newRowGroupStart);
        STORM_LOG_ASSERT(risk.size() > unfoldedToOldEntry.second, "Must be a state");
        STORM_LOG_ASSERT(!cc.isLess(storm::utility::one<ValueType>(), risk[unfoldedToOldEntry.second]), "Risk must be a probability");
        STORM_LOG_ASSERT(!cc.isLess(risk[unfoldedToOldEntry.second], storm::utility::zero<ValueType>()), "Risk must be a probability");
        if (!storm::utility::isOne(risk[unfoldedToOldEntry.second])) {
            transitionMatrixBuilder.addNextValue(newRowGroupStart, sinkState, storm::utility::one<ValueType>() - risk[unfoldedToOldEntry.second]);
        }
        if (!storm::utility::isZero(risk[unfoldedToOldEntry.second])) {
            transitionMatrixBuilder.addNextValue(newRowGroupStart, targetState, risk[unfoldedToOldEntry.second]);
        }
        newRowGroupStart++;
    }
    // sink state
    transitionMatrixBuilder.newRowGroup(newRowGroupStart);
    transitionMatrixBuilder.addNextValue(newRowGroupStart, sinkState, storm::utility::one<ValueType>());
    svbuilder.addState(sinkState, {}, {-1, -1});

    newRowGroupStart++;
    transitionMatrixBuilder.newRowGroup(newRowGroupStart);
    // target state
    transitionMatrixBuilder.addNextValue(newRowGroupStart, targetState, storm::utility::one<ValueType>());
    svbuilder.addState(targetState, {}, {-1, -1});
#ifdef _VERBOSE_OBSERVATION_UNFOLDING
    std::cout << "build matrix...\n";
#endif

    storm::storage::sparse::ModelComponents<ValueType> components;
    components.transitionMatrix = transitionMatrixBuilder.build();
#ifdef _VERBOSE_OBSERVATION_UNFOLDING
    std::cout << components.transitionMatrix << '\n';
#endif
    STORM_LOG_ASSERT(components.transitionMatrix.getRowGroupCount() == targetState + 2,
                     "Expect row group count (" << components.transitionMatrix.getRowGroupCount() << ") one more as target state index " << targetState << ")");

    storm::models::sparse::StateLabeling labeling(components.transitionMatrix.getRowGroupCount());
    labeling.addLabel("_goal");
    labeling.addLabelToState("_goal", targetState);
    labeling.addLabel("_violated");
    labeling.addLabelToState("_violated", violatedState);
    labeling.addLabel("_end");
    labeling.addLabelToState("_end", sinkState);
    labeling.addLabelToState("_end", targetState);
    labeling.addLabel("init");
    labeling.addLabelToState("init", initialState);
    components.stateLabeling = labeling;
    components.stateValuations = svbuilder.build();
    return std::make_shared<storm::models::sparse::Mdp<ValueType>>(std::move(components));
}

template<typename ValueType>
std::shared_ptr<storm::models::sparse::Mdp<ValueType>> ObservationTraceUnfolder<ValueType>::extend(uint32_t observation) {
    traceSoFar.push_back(observation);
    return transform(traceSoFar);
}

template<typename ValueType>
void ObservationTraceUnfolder<ValueType>::reset(uint32_t observation) {
    traceSoFar = {observation};
}

template<typename ValueType>
bool ObservationTraceUnfolder<ValueType>::isRejectionSamplingSet() const {
    return options.rejectionSampling;
}

template class ObservationTraceUnfolder<double>;
template class ObservationTraceUnfolder<storm::RationalNumber>;
template class ObservationTraceUnfolder<storm::RationalFunction>;
}  // namespace pomdp
}  // namespace storm
