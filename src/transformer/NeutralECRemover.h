#ifndef STORM_TRANSFORMER_NEUTRALECREMOVER_H
#define STORM_TRANSFORMER_NEUTRALECREMOVER_H


#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/utility/graph.h"
#include "src/utility/vector.h"
#include "src/storage/MaximalEndComponentDecomposition.h"

namespace storm {
    namespace transformer {
        
        template <typename ValueType>
        class NeutralECRemover {
        public:

            struct NeutralECRemoverReturnType {
                // The resulting matrix
                storm::storage::SparseMatrix<ValueType> matrix;
                // The resulting vector
                std::vector<ValueType> vector;
                // Index mapping that gives for each row of the resulting matrix and vector the corresponding row in the original matrix and vector.
                // For the empty rows added to EC states, some row of the original matrix that stays inside the EC is given.
                std::vector<uint_fast64_t> newToOldRowMapping;
                // Gives for each state (=rowGroup) of the original matrix and vector the corresponding state in the resulting matrix and vector.
                // if the given state does not exist in the result, the returned value will be std::numeric_limits<uint_fast64_t>::max(), i.e., an invalid index.
                std::vector<uint_fast64_t> oldToNewStateMapping;
            };
            
            /*
             * Identifies neutral end components and replaces them by a single state.
             *
             * A choice is neutral, iff the given vector is zero at the corresponding row.
             * An EC is neutral iff it only consideres neutral choices.
             * For each such EC that is not contained by another neutral EC, we add a new state and redirect all incoming and outgoing
             * transitions of the EC to (and from) this state.
             * If allowEmptyRow is true for at least one state in a neutral EC, an empty row is added to the new state (representing the choice to stay at the neutral EC forever).
             * States from which all reachable choices are neutral will be removed.
             *
             */
            static NeutralECRemoverReturnType transform(storm::storage::SparseMatrix<ValueType> const& originalMatrix, std::vector<ValueType> const& originalVector, storm::storage::BitVector const& allowEmptyRow) {
                STORM_LOG_DEBUG("Invoked NeutralECRemover on matrix with " << originalMatrix.getRowGroupCount() << " row groups.");
                
                storm::storage::BitVector keptStates = computeStatesFromWhichNonNeutralChoiceIsReachable(originalMatrix, originalVector);
                storm::storage::MaximalEndComponentDecomposition<ValueType> neutralECs = computeNeutralECs(originalMatrix, originalVector, keptStates);
                
                //further shrink the set of kept states by removing all states that are part of a neutral EC
                for (auto const& ec : neutralECs) {
                     for (auto const& stateActionsPair : ec) {
                         keptStates.set(stateActionsPair.first, false);
                     }
                }
                STORM_LOG_DEBUG("Found " << neutralECs.size() << " neutral End Components. Keeping " << keptStates.getNumberOfSetBits() << " of " << keptStates.size() << " original states.");
                
                NeutralECRemoverReturnType result;
                std::vector<uint_fast64_t> newRowGroupIndices;
                result.oldToNewStateMapping = std::vector<uint_fast64_t>(originalMatrix.getRowGroupCount(), std::numeric_limits<uint_fast64_t>::max());
                storm::storage::BitVector emptyRows(originalMatrix.getRowCount(), false); // will be resized as soon as the rowCount of the resulting matrix is known
                
                for(auto keptState : keptStates) {
                    result.oldToNewStateMapping[keptState] = newRowGroupIndices.size(); // i.e., the current number of processed states
                    newRowGroupIndices.push_back(result.newToOldRowMapping.size());     // i.e., the current number of processed rows
                    for(uint_fast64_t oldRow = originalMatrix.getRowGroupIndices()[keptState]; oldRow < originalMatrix.getRowGroupIndices()[keptState + 1]; ++oldRow) {
                        result.newToOldRowMapping.push_back(oldRow);
                    }
                }
                for (auto const& ec : neutralECs) {
                    newRowGroupIndices.push_back(result.newToOldRowMapping.size());
                    bool ecGetsEmptyRow = false;
                    for (auto const& stateActionsPair : ec) {
                        result.oldToNewStateMapping[stateActionsPair.first] = newRowGroupIndices.size()-1;
                        for(uint_fast64_t row = originalMatrix.getRowGroupIndices()[stateActionsPair.first]; row < originalMatrix.getRowGroupIndices()[stateActionsPair.first +1]; ++row) {
                            if(stateActionsPair.second.find(row) == stateActionsPair.second.end()) {
                                result.newToOldRowMapping.push_back(row);
                            }
                        }
                        ecGetsEmptyRow |= allowEmptyRow.get(stateActionsPair.first);
                    }
                    if(ecGetsEmptyRow) {
                        STORM_LOG_ASSERT(result.newToOldRowMapping.size() < originalMatrix.getRowCount(), "Didn't expect to see more rows in the reduced matrix than in the original one.");
                        emptyRows.set(result.newToOldRowMapping.size(), true);
                        result.newToOldRowMapping.push_back(originalMatrix.getRowGroupIndices()[ec.begin()->first] + (*ec.begin()->second.begin()));
                    }
                }
                newRowGroupIndices.push_back(result.newToOldRowMapping.size());
                emptyRows.resize(result.newToOldRowMapping.size());
                
                result.matrix = buildTransformedMatrix(originalMatrix, newRowGroupIndices, result.newToOldRowMapping, result.oldToNewStateMapping, emptyRows);
                result.vector = buildTransformedVector(originalVector, result.newToOldRowMapping);
                
                STORM_LOG_DEBUG("NeutralECRemover is done. Resulting  matrix has " << result.matrix.getRowGroupCount() << " row groups. Resulting Matrix: " << std::endl << result.matrix << std::endl << " resulting vector: " << result.vector );
                return result;
            }
            
        private:
            
            static storm::storage::BitVector computeStatesFromWhichNonNeutralChoiceIsReachable(storm::storage::SparseMatrix<ValueType> const& originalMatrix, std::vector<ValueType> const& originalVector) {
                storm::storage::BitVector statesWithNonNeutralChoice(originalMatrix.getRowGroupCount(), false);
                for(uint_fast64_t rowGroup = 0; rowGroup < originalMatrix.getRowGroupCount(); ++rowGroup){
                    for(uint_fast64_t row = originalMatrix.getRowGroupIndices()[rowGroup]; row < originalMatrix.getRowGroupIndices()[rowGroup+1]; ++row) {
                        if(!storm::utility::isZero(originalVector[row])) {
                            statesWithNonNeutralChoice.set(rowGroup);
                            break;
                        }
                    }
                }
                storm::storage::BitVector trueVector(originalMatrix.getRowGroupCount(), true);
                return storm::utility::graph::performProbGreater0E(originalMatrix, originalMatrix.getRowGroupIndices(), originalMatrix.transpose(true), trueVector, statesWithNonNeutralChoice);
            }
            
            static storm::storage::MaximalEndComponentDecomposition<ValueType> computeNeutralECs(storm::storage::SparseMatrix<ValueType> const& originalMatrix, std::vector<ValueType> const& originalVector, storm::storage::BitVector const& keptStates) {
                // Get an auxiliary matrix to identify the neutral end components.
                // This is done by redirecting choices that can never be part of a neutral EC to a sink state.
                // Such choices are either non-neutral choices or choices that lead to a state that is not in keptStates.
                storm::storage::BitVector neutralChoices = storm::utility::vector::filter<ValueType>(originalVector, [&] (ValueType const& value) -> bool {return storm::utility::isZero(value); } );
                uint_fast64_t sinkState = originalMatrix.getRowGroupCount();
                storm::storage::SparseMatrixBuilder<ValueType> builder(originalMatrix.getRowCount() + 1, originalMatrix.getColumnCount() + 1, originalMatrix.getEntryCount() + 1, false, true,  originalMatrix.getRowGroupCount()+1);
                uint_fast64_t row = 0;
                for(uint_fast64_t rowGroup = 0; rowGroup < originalMatrix.getRowGroupCount(); ++rowGroup) {
                    builder.newRowGroup(row);
                    for (; row < originalMatrix.getRowGroupIndices()[rowGroup+1]; ++row ){
                        bool keepRow = neutralChoices.get(row);
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
                storm::storage::SparseMatrix<ValueType> auxiliaryMatrix = builder.build(originalMatrix.getRowCount() + 1, originalMatrix.getColumnCount()+1, originalMatrix.getRowGroupCount() +1);
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
                                                                                  storm::storage::BitVector const& emptyRows) {
                
                uint_fast64_t numRowGroups = newRowGroupIndices.size()-1;
                uint_fast64_t newRow = 0;
                storm::storage::SparseMatrixBuilder<ValueType> builder(newToOldRowMapping.size(), numRowGroups, originalMatrix.getEntryCount(), false, true, numRowGroups);
                for(uint_fast64_t newRowGroup = 0; newRowGroup < numRowGroups; ++newRowGroup) {
                    builder.newRowGroup(newRow);
                    for(; newRow < newRowGroupIndices[newRowGroup+1]; ++newRow) {
                        if(!emptyRows.get(newRow)) {
                            // Make sure that the entries for this row are inserted in the right order.
                            // Also, transitions to the same EC need to be merged and transitions to states that are erased need to be ignored
                            std::map<uint_fast64_t, ValueType> sortedEntries;
                            for(auto const& entry : originalMatrix.getRow(newToOldRowMapping[newRow])){
                                uint_fast64_t newColumn = oldToNewStateMapping[entry.getColumn()];
                                if(newColumn < numRowGroups) {
                                    auto insertResult = sortedEntries.insert(std::make_pair(newColumn, entry.getValue()));
                                    if(!insertResult.second) {
                                        // We have already seen an entry with this column. ==> merge transitions
                                        insertResult.first->second += entry.getValue();
                                    }
                                }
                            }
                            for(auto const& sortedEntry : sortedEntries) {
                                builder.addNextValue(newRow, sortedEntry.first, sortedEntry.second);
                            }
                        }
                    }
                }
                return builder.build(newToOldRowMapping.size(), numRowGroups, numRowGroups);
            }
            
            static std::vector<ValueType> buildTransformedVector(std::vector<ValueType> const& originalVector, std::vector<uint_fast64_t> const& newToOldRowMapping) {
                std::vector<ValueType> v;
                v.reserve(newToOldRowMapping.size());
                for(auto const& oldRow : newToOldRowMapping) {
                    v.push_back(originalVector[oldRow]);
                }
                return v;
            }
            
        };
    }
}
#endif // STORM_TRANSFORMER_NEUTRALECREMOVER_H
