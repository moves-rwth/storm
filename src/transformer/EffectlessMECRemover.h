#ifndef STORM_TRANSFORMER_EFFECTLESSMECREMOVER_H
#define STORM_TRANSFORMER_EFFECTLESSMECREMOVER_H


#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/utility/graph.h"
#include "src/utility/vector.h"
#include "src/storage/MaximalEndComponentDecomposition.h"

namespace storm {
    namespace transformer {
        
        template <typename ValueType>
        class EffectlessMECRemover {
        public:

            struct EffectlessMECRemoverReturnType {
                // The resulting matrix
                storm::storage::SparseMatrix<ValueType> matrix;
                // The resulting vector
                std::vector<ValueType> vector;
                // Index mapping that gives for each row of the resulting matrix and vector the corresponding row in the original matrix and vector.
                // For the rows that lead to the newly introduced sink state, some row of the original matrix that stays inside the EC is given.
                // The selfloop of the newly introduced sink state has no entry, i.e., matrix.rowCount() == newToOldRowMapping.size() + 1.
                std::vector<uint_fast64_t> newToOldRowMapping;
                // Gives for each state (=rowGroup) of the original matrix and vector the corresponding state in the resulting matrix and vector.
                // if the given state does not exist in the result, the returned value will be numeric_limits<uint_fast64_t>::max()
                std::vector<uint_fast64_t> oldToNewStateMapping;
            };
            
            /*
             * Identifies maximal effectless end components and replaces them by a single state.
             *
             * A choice is effectless, iff the given vector is zero at the corresponding row.
             * An EC is effectless iff it only consideres effectless choices.
             * For each such EC that is not contained by another effectles EC, we add a new state and redirect all incoming and outgoing
             * transitions of the EC to (and from) this state.
             * If allowChoiceToSink is true for at least one state in an effectless EC, a choice that leads to a sink state is added to the new state.
             * States from which all reachable choices are effectless will be removed.
             *
             */
            static EffectlessMECRemoverReturnType transform(storm::storage::SparseMatrix<ValueType> const& originalMatrix, std::vector<ValueType> const& originalVector, storm::storage::BitVector const& allowChoiceToSink) {
                STORM_LOG_DEBUG("Invoked EffectlessMECRemover on matrix with " << originalMatrix.getRowGroupCount() << " row groups.");
                
                storm::storage::BitVector keptStates = computeStatesFromWhichNonEffectlessChoiceIsReachable(originalMatrix, originalVector);
                storm::storage::MaximalEndComponentDecomposition<ValueType> effectlessECs = computeEffectlessECs(originalMatrix, originalVector, keptStates);
                
                //further shrink the set of kept states by removing all states that are part of an effectless EC
                for (auto const& ec : effectlessECs) {
                     for (auto const& stateActionsPair : ec) {
                         keptStates.set(stateActionsPair.first, false);
                     }
                }
                STORM_LOG_DEBUG("Found " << effectlessECs.size() << " effectless End Components. Keeping " << keptStates.getNumberOfSetBits() << " of " << keptStates.size() << " original states.");
                
                EffectlessMECRemoverReturnType result;
                std::vector<uint_fast64_t> newRowGroupIndices;
                result.oldToNewStateMapping = std::vector<uint_fast64_t>(originalMatrix.getRowGroupCount(), std::numeric_limits<uint_fast64_t>::max());
                storm::storage::BitVector rowsToSinkState(originalMatrix.getRowCount(), false); // will be resized as soon as rowCount of resulting matrix is known
                
                for(auto keptState : keptStates) {
                    result.oldToNewStateMapping[keptState] = newRowGroupIndices.size(); // i.e., the current number of processed states
                    newRowGroupIndices.push_back(result.newToOldRowMapping.size());     // i.e., the current number of processed rows
                    for(uint_fast64_t oldRow = originalMatrix.getRowGroupIndices()[keptState]; oldRow < originalMatrix.getRowGroupIndices()[keptState + 1]; ++oldRow) {
                        result.newToOldRowMapping.push_back(oldRow);
                    }
                }
                for (auto const& ec : effectlessECs) {
                    newRowGroupIndices.push_back(result.newToOldRowMapping.size());
                    bool ecHasChoiceToSink = false;
                    for (auto const& stateActionsPair : ec) {
                        result.oldToNewStateMapping[stateActionsPair.first] = newRowGroupIndices.size()-1;
                        for(uint_fast64_t choice = 0; choice < originalMatrix.getRowGroupSize(stateActionsPair.first); ++choice) {
                            if(stateActionsPair.second.find(choice) == stateActionsPair.second.end()) {
                                result.newToOldRowMapping.push_back(originalMatrix.getRowGroupIndices()[stateActionsPair.first] + choice);
                            }
                        }
                        ecHasChoiceToSink |= allowChoiceToSink.get(stateActionsPair.first);
                    }
                    if(ecHasChoiceToSink) {
                        STORM_LOG_ASSERT(result.newToOldRowMapping.size() < originalMatrix.getRowCount(), "Didn't expect to see more rows in the reduced matrix than in the original one.");
                        rowsToSinkState.set(result.newToOldRowMapping.size(), true);
                        result.newToOldRowMapping.push_back(originalMatrix.getRowGroupIndices()[ec.begin()->first] + (*ec.begin()->second.begin()));
                    }
                }
                
                newRowGroupIndices.push_back(result.newToOldRowMapping.size());
                newRowGroupIndices.push_back(result.newToOldRowMapping.size()+1);
                rowsToSinkState.resize(result.newToOldRowMapping.size());
                
                result.matrix = buildTransformedMatrix(originalMatrix, newRowGroupIndices, result.newToOldRowMapping, result.oldToNewStateMapping, rowsToSinkState);
                result.vector = buildTransformedVector(originalVector, result.newToOldRowMapping);
              
                STORM_LOG_DEBUG("EffectlessMECRemover is done. Resulting matrix has " << result.matrix.getRowGroupCount() << " row groups.");
                return result;
            }
            
        private:
            
            static storm::storage::BitVector computeStatesFromWhichNonEffectlessChoiceIsReachable(storm::storage::SparseMatrix<ValueType> const& originalMatrix, std::vector<ValueType> const& originalVector) {
                storm::storage::BitVector statesWithNonEffectlessChoice(originalMatrix.getRowGroupCount(), false);
                for(uint_fast64_t rowGroup = 0; rowGroup < originalMatrix.getRowGroupCount(); ++rowGroup){
                    for(uint_fast64_t row = originalMatrix.getRowGroupIndices()[rowGroup]; row < originalMatrix.getRowGroupIndices()[rowGroup+1]; ++row) {
                        if(!storm::utility::isZero(originalVector[row])) {
                            statesWithNonEffectlessChoice.set(rowGroup);
                            break;
                        }
                    }
                }
                storm::storage::BitVector trueVector(originalMatrix.getRowGroupCount(), true);
                return storm::utility::graph::performProbGreater0E(originalMatrix, originalMatrix.getRowGroupIndices(), originalMatrix.transpose(true), trueVector, statesWithNonEffectlessChoice);
            }
            
            static storm::storage::MaximalEndComponentDecomposition<ValueType> computeEffectlessECs(storm::storage::SparseMatrix<ValueType> const& originalMatrix, std::vector<ValueType> const& originalVector, storm::storage::BitVector const& keptStates) {
                // Get an auxiliary matrix to identify the effectless end components.
                // This is done by redirecting choices that can never be part of an effectless EC to a sink state.
                // Such choices are either non-effectless choices or choices that lead to a state that is not in keptStates.
                storm::storage::BitVector effectlessChoices = storm::utility::vector::filter<ValueType>(originalVector, [&] (ValueType const& value) -> bool {return storm::utility::isZero(value); } );
                uint_fast64_t sinkState = originalMatrix.getRowGroupCount();
                storm::storage::SparseMatrixBuilder<ValueType> builder(originalMatrix.getRowCount() + 1, originalMatrix.getColumnCount() + 1, originalMatrix.getEntryCount() + 1, false, true,  originalMatrix.getRowGroupCount()+1);
                uint_fast64_t row = 0;
                for(uint_fast64_t rowGroup = 0; rowGroup < originalMatrix.getRowGroupCount(); ++rowGroup) {
                    builder.newRowGroup(row);
                    for (; row < originalMatrix.getRowGroupIndices()[rowGroup+1]; ++row ){
                        bool keepRow = effectlessChoices.get(row);
                        if(keepRow) { //Also check whether all successors are kept
                            for(auto const& entry : originalMatrix.getRow(row)){
                                keepRow &= keptStates.get(entry.getColumn());
                            }
                        }
                        if(keepRow) {
                            for(auto const& entry : originalMatrix.getRow(row)){
                                builder.addNextValue(row, entry.getColumn(), entry.getValue());
                            }
                        } else {
                            builder.addNextValue(row, sinkState, storm::utility::one<ValueType>());
                        }
                    }
                }
                builder.newRowGroup(row);
                builder.addNextValue(row, sinkState, storm::utility::one<ValueType>());
                storm::storage::SparseMatrix<ValueType> auxiliaryMatrix = builder.build();
                storm::storage::SparseMatrix<ValueType> backwardsTransitions = auxiliaryMatrix.transpose(true);
                storm::storage::BitVector sinkStateAsBitVector(auxiliaryMatrix.getRowGroupCount(), false);
                sinkStateAsBitVector.set(sinkState);
                storm::storage::BitVector subsystem = keptStates;
                subsystem.resize(keptStates.size() + 1, true);
                // The states for which sinkState is reachable under any scheduler can not be part of an EC
                subsystem &= ~(storm::utility::graph::performProbGreater0A(auxiliaryMatrix, auxiliaryMatrix.getRowGroupIndices(), backwardsTransitions, subsystem, sinkStateAsBitVector));
                return storm::storage::MaximalEndComponentDecomposition<ValueType>(auxiliaryMatrix, backwardsTransitions, subsystem);
            }
            
            static storm::storage::SparseMatrix<ValueType> buildTransformedMatrix(storm::storage::SparseMatrix<ValueType> const& originalMatrix,
                                                                                  std::vector<uint_fast64_t> const& newRowGroupIndices,
                                                                                  std::vector<uint_fast64_t> const& newToOldRowMapping,
                                                                                  std::vector<uint_fast64_t> const& oldToNewStateMapping,
                                                                                  storm::storage::BitVector const& rowsToSinkState) {
                
                uint_fast64_t numRowGroups = newRowGroupIndices.size()-1;
                uint_fast64_t sinkState = numRowGroups-1;
                storm::storage::SparseMatrixBuilder<ValueType> builder(newToOldRowMapping.size()+1, numRowGroups, 0, false, true, numRowGroups);
                // Insert all matrix entries except the selfloop of the sink state
                for(uint_fast64_t newRowGroup = 0; newRowGroup < numRowGroups-1; ++newRowGroup) {
                    uint_fast64_t newRow = newRowGroupIndices[newRowGroup];
                    builder.newRowGroup(newRow);
                    for(; newRow < newRowGroupIndices[newRowGroup+1]; ++newRow) {
                        if(rowsToSinkState.get(newRow)) {
                            builder.addNextValue(newRow, sinkState, storm::utility::one<ValueType>());
                        } else {
                            // Make sure that the entries for this row are inserted in the right order.
                            // Also, transitions to the same EC need to be merged. and transitions to states that are erased need to be ignored
                            std::map<uint_fast64_t, ValueType> sortedEntries;
                            for(auto const& entry : originalMatrix.getRow(newToOldRowMapping[newRow])){
                                uint_fast64_t newColumn = oldToNewStateMapping[entry.getColumn()];
                                if(newColumn < numRowGroups) {
                                    auto insertRes = sortedEntries.insert(std::make_pair(newColumn, entry.getValue()));
                                    if(!insertRes.second) {
                                        // We have already seen an entry with this column. ==> merge transitions
                                        insertRes.first->second += entry.getValue();
                                    }
                                }
                            }
                            for(auto const& sortedEntry : sortedEntries) {
                                builder.addNextValue(newRow, sortedEntry.first, sortedEntry.second);
                            }
                        }
                    }
                }
                //Now add the selfloop at the sink state
                builder.newRowGroup(newRowGroupIndices[sinkState]);
                builder.addNextValue(newRowGroupIndices[sinkState], sinkState, storm::utility::one<ValueType>());
                
                return builder.build();
            }
            
            static std::vector<ValueType> buildTransformedVector(std::vector<ValueType> const& originalVector, std::vector<uint_fast64_t> const& newToOldRowMapping) {
                std::vector<ValueType> v;
                v.reserve(newToOldRowMapping.size()+1);
                for(auto const& oldRow : newToOldRowMapping) {
                    v.push_back(originalVector[oldRow]);
                }
                // add entry for the sink state
                v.push_back(storm::utility::zero<ValueType>());
                return v;
            }
            
        };
    }
}
#endif // STORM_TRANSFORMER_EFFECTLESSMECREMOVER_H
