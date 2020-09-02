#include "storm/exceptions/InvalidArgumentException.h"
#include "storm-pomdp/transformer/ObservationTraceUnfolder.h"


namespace storm {
    namespace pomdp {
        template<typename ValueType>
        ObservationTraceUnfolder<ValueType>::ObservationTraceUnfolder(storm::models::sparse::Pomdp<ValueType> const& model) : model(model) {
            statesPerObservation = std::vector<storm::storage::BitVector>(model.getNrObservations(), storm::storage::BitVector(model.getNumberOfStates()));
            for (uint64_t state = 0; state < model.getNumberOfStates(); ++state) {
                statesPerObservation[model.getObservation(state)].set(state, true);
            }
        }

        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Mdp<ValueType>> ObservationTraceUnfolder<ValueType>::transform(
                const std::vector<uint32_t> &observations, std::vector<ValueType> const& risk) {
            std::vector<uint32_t> modifiedObservations = observations;
            // First observation should be special.
            // This just makes the algorithm simpler because we do not treat the first step as a special case later.
            modifiedObservations[0] = model.getNrObservations();

            storm::storage::BitVector initialStates = model.getInitialStates();
            storm::storage::BitVector actualInitialStates = initialStates;
            for (uint64_t state : initialStates) {
                if (model.getObservation(state) != observations[0]) {
                    actualInitialStates.set(state, false);
                }
            }
            STORM_LOG_THROW(actualInitialStates.getNumberOfSetBits() == 1, storm::exceptions::InvalidArgumentException, "Must have unique initial state matching the observation");
            //
            statesPerObservation.resize(model.getNrObservations() + 1);
            statesPerObservation[model.getNrObservations()] = actualInitialStates;


            std::map<uint64_t,uint64_t> unfoldedToOld;
            std::map<uint64_t,uint64_t> unfoldedToOldNextStep;
            std::map<uint64_t,uint64_t> oldToUnfolded;

            // Add this initial state state:
            unfoldedToOldNextStep[0] = actualInitialStates.getNextSetIndex(0);

            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0,0,0,true,true);
            uint64_t newStateIndex = 1;
            uint64_t newRowGroupStart = 0;
            uint64_t newRowCount = 0;
            // Notice that we are going to use a special last step

            for (uint64_t step = 0; step < observations.size() - 1; ++step) {
                std::cout << "step " << step  << std::endl;
                oldToUnfolded.clear();
                unfoldedToOld = unfoldedToOldNextStep;
                unfoldedToOldNextStep.clear();

                for (auto const& unfoldedToOldEntry : unfoldedToOld) {
                    transitionMatrixBuilder.newRowGroup(newRowGroupStart);
                    std::cout << "\tconsider new state " << unfoldedToOldEntry.first << std::endl;
                    assert(step == 0 || newRowCount == transitionMatrixBuilder.getLastRow() + 1);
                    uint64_t oldRowIndexStart = model.getNondeterministicChoiceIndices()[unfoldedToOldEntry.second];
                    uint64_t oldRowIndexEnd = model.getNondeterministicChoiceIndices()[unfoldedToOldEntry.second+1];

                    for (uint64_t oldRowIndex = oldRowIndexStart; oldRowIndex != oldRowIndexEnd; oldRowIndex++) {
                        std::cout << "\t\tconsider old action " << oldRowIndex << std::endl;
                        std::cout << "\t\tconsider new row nr " << newRowCount << std::endl;

                        ValueType resetProb = storm::utility::zero<ValueType>();
                        // We first find the reset probability
                        for (auto const &oldRowEntry : model.getTransitionMatrix().getRow(oldRowIndex)) {
                            if (model.getObservation(oldRowEntry.getColumn()) != observations[step + 1]) {
                                resetProb += oldRowEntry.getValue();
                            }
                        }
                        std::cout << "\t\t\t add reset" << std::endl;

                        // Add the resets
                        if (resetProb != storm::utility::zero<ValueType>()) {
                            transitionMatrixBuilder.addNextValue(newRowCount, 0, resetProb);
                        }

                        std::cout << "\t\t\t add other transitions..." << std::endl;

                        // Now, we build the outgoing transitions.
                        for (auto const &oldRowEntry : model.getTransitionMatrix().getRow(oldRowIndex)) {
                            if (model.getObservation(oldRowEntry.getColumn()) != observations[step + 1]) {
                                continue;// already handled.
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
                            std::cout << "\t\t\t\t transition to " << column << std::endl;
                            transitionMatrixBuilder.addNextValue(newRowCount, column,
                                                                   oldRowEntry.getValue());
                        }
                        newRowCount++;
                    }

                    newRowGroupStart = transitionMatrixBuilder.getLastRow() + 1;

                }
            }
            std::cout << "Adding last step..." << std::endl;
            // Now, take care of the last step.
            uint64_t sinkState = newStateIndex;
            uint64_t targetState = newStateIndex + 1;
            for (auto const& unfoldedToOldEntry : unfoldedToOldNextStep) {
                transitionMatrixBuilder.newRowGroup(newRowGroupStart);
                if (!storm::utility::isZero(storm::utility::one<ValueType>() - risk[unfoldedToOldEntry.second])) {
                    transitionMatrixBuilder.addNextValue(newRowGroupStart, sinkState,
                                                         storm::utility::one<ValueType>() - risk[unfoldedToOldEntry.second]);
                }
                if (!storm::utility::isZero(risk[unfoldedToOldEntry.second])) {
                    transitionMatrixBuilder.addNextValue(newRowGroupStart, targetState,
                                                           risk[unfoldedToOldEntry.second]);
                }
                newRowGroupStart++;
            }
            // sink state
            transitionMatrixBuilder.newRowGroup(newRowGroupStart);
            transitionMatrixBuilder.addNextValue(newRowGroupStart, sinkState, storm::utility::one<ValueType>());
            newRowGroupStart++;
            transitionMatrixBuilder.newRowGroup(newRowGroupStart);
            // target state
            transitionMatrixBuilder.addNextValue(newRowGroupStart, targetState, storm::utility::one<ValueType>());



            storm::storage::sparse::ModelComponents<ValueType> components;
            components.transitionMatrix = transitionMatrixBuilder.build();
            std::cout << components.transitionMatrix << std::endl;

            STORM_LOG_ASSERT(components.transitionMatrix.getRowGroupCount() == targetState + 1, "Expect row group count (" << components.transitionMatrix.getRowGroupCount() << ") one more as target state index " << targetState << ")");

            storm::models::sparse::StateLabeling labeling(components.transitionMatrix.getRowGroupCount());
            labeling.addLabel("_goal");
            labeling.addLabelToState("_goal", targetState);
            labeling.addLabel("init");
            labeling.addLabelToState("init", 0);
            components.stateLabeling = labeling;
            return std::make_shared<storm::models::sparse::Mdp<ValueType>>(std::move(components));




        }

        template class ObservationTraceUnfolder<double>;
        template class ObservationTraceUnfolder<storm::RationalFunction>;

    }
}