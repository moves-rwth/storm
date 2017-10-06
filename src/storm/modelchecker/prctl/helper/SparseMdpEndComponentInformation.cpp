#include "storm/modelchecker/prctl/helper/SparseMdpEndComponentInformation.h"

#include "storm/storage/BitVector.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
        
            template<typename ValueType>
            SparseMdpEndComponentInformation<ValueType>::SparseMdpEndComponentInformation(storm::storage::MaximalEndComponentDecomposition<ValueType> const& endComponentDecomposition, storm::storage::BitVector const& maybeStates) : NOT_IN_EC(std::numeric_limits<uint64_t>::max()), eliminatedEndComponents(false), numberOfMaybeStatesInEc(0), numberOfMaybeStatesNotInEc(0), numberOfEc(endComponentDecomposition.size()) {
                
                // (1) Compute how many maybe states there are before each other maybe state.
                maybeStatesBefore = maybeStates.getNumberOfSetBitsBeforeIndices();
                
                // (2) Create mapping from maybe states to their MEC. If they are not contained in an MEC, their value
                // is set to a special constant.
                maybeStateToEc.resize(maybeStates.getNumberOfSetBits(), NOT_IN_EC);
                uint64_t mecIndex = 0;
                for (auto const& mec : endComponentDecomposition) {
                    for (auto const& stateActions : mec) {
                        maybeStateToEc[maybeStatesBefore[stateActions.first]] = mecIndex;
                        ++numberOfMaybeStatesInEc;
                    }
                    ++mecIndex;
                }
                
                // (3) Compute number of states not in MECs.
                numberOfMaybeStatesNotInEc = maybeStateToEc.size() - numberOfMaybeStatesInEc;
            }

            template<typename ValueType>
            bool SparseMdpEndComponentInformation<ValueType>::isMaybeStateInEc(uint64_t maybeState) const {
                return maybeStateToEc[maybeState] != NOT_IN_EC;
            }
            
            template<typename ValueType>
            bool SparseMdpEndComponentInformation<ValueType>::isStateInEc(uint64_t state) const {
                return maybeStateToEc[maybeStatesBefore[state]] != NOT_IN_EC;
            }
            
            template<typename ValueType>
            std::vector<uint64_t> SparseMdpEndComponentInformation<ValueType>::getNumberOfMaybeStatesNotInEcBeforeIndices() const {
                std::vector<uint64_t> result(maybeStateToEc.size());
                
                uint64_t count = 0;
                auto resultIt = result.begin();
                for (auto const& e : maybeStateToEc) {
                    *resultIt = count;
                    if (e != NOT_IN_EC) {
                        ++count;
                    }
                    ++resultIt;
                }
                
                return result;
            }
            
            template<typename ValueType>
            uint64_t SparseMdpEndComponentInformation<ValueType>::getNumberOfMaybeStatesNotInEc() const {
                return numberOfMaybeStatesNotInEc;
            }
            
            template<typename ValueType>
            uint64_t SparseMdpEndComponentInformation<ValueType>::getEc(uint64_t state) const {
                return maybeStateToEc[maybeStatesBefore[state]];
            }
            
            template<typename ValueType>
            uint64_t SparseMdpEndComponentInformation<ValueType>::getRowGroupAfterElimination(uint64_t state) const {
                return numberOfMaybeStatesNotInEc + getEc(state);
            }
            
            template<typename ValueType>
            bool SparseMdpEndComponentInformation<ValueType>::getEliminatedEndComponents() const {
                return eliminatedEndComponents;
            }
            
            template<typename ValueType>
            uint64_t SparseMdpEndComponentInformation<ValueType>::getNotInEcMarker() const {
                return NOT_IN_EC;
            }

            template<typename ValueType>
            SparseMdpEndComponentInformation<ValueType> SparseMdpEndComponentInformation<ValueType>::eliminateEndComponents(storm::storage::MaximalEndComponentDecomposition<ValueType> const& endComponentDecomposition, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& maybeStates, storm::storage::BitVector const* sumColumns, storm::storage::BitVector const* selectedChoices, std::vector<ValueType> const* summand, storm::storage::SparseMatrix<ValueType>& submatrix, std::vector<ValueType>* columnSumVector, std::vector<ValueType>* summandResultVector) {
                
                SparseMdpEndComponentInformation<ValueType> result(endComponentDecomposition, maybeStates);
                
                // (1) Compute the number of maybe states not in ECs before any other maybe state.
                std::vector<uint64_t> maybeStatesNotInEcBefore = result.getNumberOfMaybeStatesNotInEcBeforeIndices();
                uint64_t numberOfStates = result.numberOfMaybeStatesNotInEc + result.numberOfEc;
                
                STORM_LOG_TRACE("Found " << numberOfStates << " states, " << result.numberOfMaybeStatesNotInEc << " not in ECs, " << result.numberOfMaybeStatesInEc << " in ECs and " << result.numberOfEc << " ECs.");
                
                // Prepare builder and vector storage.
                storm::storage::SparseMatrixBuilder<ValueType> builder(0, numberOfStates, 0, true, true, numberOfStates);
                STORM_LOG_ASSERT((sumColumns && columnSumVector) || (!sumColumns && !columnSumVector), "Expecting a bit vector for which columns to sum iff there is a column sum result vector.");
                if (columnSumVector) {
                    columnSumVector->resize(numberOfStates);
                }
                STORM_LOG_ASSERT((summand && summandResultVector) || (!summand && !summandResultVector), "Expecting summand iff there is a summand result vector.");
                if (summandResultVector) {
                    summandResultVector->resize(numberOfStates);
                }
                std::vector<std::pair<uint64_t, ValueType>> ecValuePairs;
                
                // (2) Create the parts of the submatrix and vector b that belong to states not contained in ECs.
                uint64_t currentRow = 0;
                for (auto state : maybeStates) {
                    if (!result.isStateInEc(state)) {
                        builder.newRowGroup(currentRow);
                        for (uint64_t row = transitionMatrix.getRowGroupIndices()[state], endRow = transitionMatrix.getRowGroupIndices()[state + 1]; row < endRow; ++row) {
                            // If the choices is not in the selected ones, drop it.
                            if (selectedChoices && !selectedChoices->get(row)) {
                                continue;
                            }
                            
                            ecValuePairs.clear();
                            
                            if (summand) {
                                (*summandResultVector)[currentRow] += (*summand)[row];
                            }
                            for (auto const& e : transitionMatrix.getRow(row)) {
                                if (sumColumns && sumColumns->get(e.getColumn())) {
                                    (*columnSumVector)[currentRow] += e.getValue();
                                } else if (maybeStates.get(e.getColumn())) {
                                    // If the target state of the transition is not contained in an EC, we can just add the entry.
                                    if (result.isStateInEc(e.getColumn())) {
                                        builder.addNextValue(currentRow, maybeStatesNotInEcBefore[result.maybeStatesBefore[e.getColumn()]], e.getValue());
                                    } else {
                                        // Otherwise, we store the information that the state can go to a certain EC.
                                        ecValuePairs.emplace_back(result.getEc(e.getColumn()), e.getValue());
                                    }
                                }
                            }
                            
                            if (!ecValuePairs.empty()) {
                                std::sort(ecValuePairs.begin(), ecValuePairs.end());
                                
                                for (auto const& e : ecValuePairs) {
                                    builder.addNextValue(currentRow, result.numberOfMaybeStatesNotInEc + e.first, e.second);
                                }
                            }
                            
                            ++currentRow;
                        }
                    }
                }
                
                // (3) Create the parts of the submatrix and vector b that belong to states contained in ECs.
                for (auto const& mec : endComponentDecomposition) {
                    builder.newRowGroup(currentRow);
                    
                    for (auto const& stateActions : mec) {
                        uint64_t const& state = stateActions.first;
                        for (uint64_t row = transitionMatrix.getRowGroupIndices()[state], endRow = transitionMatrix.getRowGroupIndices()[state + 1]; row < endRow; ++row) {
                            // If the choice is contained in the MEC, drop it.
                            if (stateActions.second.find(row) != stateActions.second.end()) {
                                continue;
                            }
                            
                            // If the choices is not in the selected ones, drop it.
                            if (selectedChoices && !selectedChoices->get(row)) {
                                continue;
                            }
                            
                            ecValuePairs.clear();
                            
                            if (summand) {
                                (*summandResultVector)[currentRow] += (*summand)[row];
                            }
                            for (auto const& e : transitionMatrix.getRow(row)) {
                                if (sumColumns && sumColumns->get(e.getColumn())) {
                                    (*columnSumVector)[currentRow] += e.getValue();
                                } else if (maybeStates.get(e.getColumn())) {
                                    // If the target state of the transition is not contained in an EC, we can just add the entry.
                                    if (result.isStateInEc(e.getColumn())) {
                                        builder.addNextValue(currentRow, maybeStatesNotInEcBefore[result.maybeStatesBefore[e.getColumn()]], e.getValue());
                                    } else {
                                        // Otherwise, we store the information that the state can go to a certain EC.
                                        ecValuePairs.emplace_back(result.getEc(e.getColumn()), e.getValue());
                                    }
                                }
                            }
                            
                            if (!ecValuePairs.empty()) {
                                std::sort(ecValuePairs.begin(), ecValuePairs.end());
                                
                                for (auto const& e : ecValuePairs) {
                                    builder.addNextValue(currentRow, result.getNumberOfMaybeStatesNotInEc() + e.first, e.second);
                                }
                            }
                            
                            ++currentRow;
                        }
                    }
                }
                
                submatrix = builder.build(currentRow);
                return result;
            }
            
            template<typename ValueType>
            SparseMdpEndComponentInformation<ValueType> SparseMdpEndComponentInformation<ValueType>::eliminateEndComponents(storm::storage::MaximalEndComponentDecomposition<ValueType> const& endComponentDecomposition, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType>& rhsVector, storm::storage::BitVector const& maybeStates, storm::storage::SparseMatrix<ValueType>& submatrix, std::vector<ValueType>& subvector) {
                
                SparseMdpEndComponentInformation<ValueType> result(endComponentDecomposition, maybeStates);
                
                // (1) Compute the number of maybe states not in ECs before any other maybe state.
                std::vector<uint64_t> maybeStatesNotInEcBefore = result.getNumberOfMaybeStatesNotInEcBeforeIndices();
                uint64_t numberOfStates = result.numberOfMaybeStatesNotInEc + result.numberOfEc;
                
                STORM_LOG_TRACE("Found " << numberOfStates << " states, " << result.numberOfMaybeStatesNotInEc << " not in ECs, " << result.numberOfMaybeStatesInEc << " in ECs and " << result.numberOfEc << " ECs.");
                
                // Prepare builder and vector storage.
                storm::storage::SparseMatrixBuilder<ValueType> builder(0, numberOfStates, 0, true, true, numberOfStates);
                subvector.resize(numberOfStates);
                
                std::vector<std::pair<uint64_t, ValueType>> ecValuePairs;
                
                // (2) Create the parts of the submatrix and vector b that belong to states not contained in ECs.
                uint64_t currentRow = 0;
                for (auto state : maybeStates) {
                    if (!result.isStateInEc(state)) {
                        builder.newRowGroup(currentRow);
                        for (uint64_t row = transitionMatrix.getRowGroupIndices()[state], endRow = transitionMatrix.getRowGroupIndices()[state + 1]; row < endRow; ++row) {
                            // Copy over the entry of the vector.
                            subvector[currentRow] = rhsVector[row];
                            
                            ecValuePairs.clear();
                            for (auto const& e : transitionMatrix.getRow(row)) {
                                if (maybeStates.get(e.getColumn())) {
                                    // If the target state of the transition is not contained in an EC, we can just add the entry.
                                    if (result.isStateInEc(e.getColumn())) {
                                        builder.addNextValue(currentRow, maybeStatesNotInEcBefore[result.maybeStatesBefore[e.getColumn()]], e.getValue());
                                    } else {
                                        // Otherwise, we store the information that the state can go to a certain EC.
                                        ecValuePairs.emplace_back(result.getEc(e.getColumn()), e.getValue());
                                    }
                                }
                            }
                            
                            if (!ecValuePairs.empty()) {
                                std::sort(ecValuePairs.begin(), ecValuePairs.end());
                                
                                for (auto const& e : ecValuePairs) {
                                    builder.addNextValue(currentRow, result.numberOfMaybeStatesNotInEc + e.first, e.second);
                                }
                            }
                            
                            ++currentRow;
                        }
                    }
                }
                
                // (3) Create the parts of the submatrix and vector b that belong to states contained in ECs.
                for (auto const& mec : endComponentDecomposition) {
                    builder.newRowGroup(currentRow);
                    
                    for (auto const& stateActions : mec) {
                        uint64_t const& state = stateActions.first;
                        for (uint64_t row = transitionMatrix.getRowGroupIndices()[state], endRow = transitionMatrix.getRowGroupIndices()[state + 1]; row < endRow; ++row) {
                            // If the choice is contained in the MEC, drop it.
                            if (stateActions.second.find(row) != stateActions.second.end()) {
                                continue;
                            }
                            
                            // Copy over the entry of the vector.
                            subvector[currentRow] = rhsVector[row];
                            
                            ecValuePairs.clear();
                            for (auto const& e : transitionMatrix.getRow(row)) {
                                if (maybeStates.get(e.getColumn())) {
                                    // If the target state of the transition is not contained in an EC, we can just add the entry.
                                    if (result.isStateInEc(e.getColumn())) {
                                        builder.addNextValue(currentRow, maybeStatesNotInEcBefore[result.maybeStatesBefore[e.getColumn()]], e.getValue());
                                    } else {
                                        // Otherwise, we store the information that the state can go to a certain EC.
                                        ecValuePairs.emplace_back(result.getEc(e.getColumn()), e.getValue());
                                    }
                                }
                            }
                            
                            if (!ecValuePairs.empty()) {
                                std::sort(ecValuePairs.begin(), ecValuePairs.end());
                                
                                for (auto const& e : ecValuePairs) {
                                    builder.addNextValue(currentRow, result.getNumberOfMaybeStatesNotInEc() + e.first, e.second);
                                }
                            }
                            
                            ++currentRow;
                        }
                    }
                }
                
                submatrix = builder.build();
                return result;
            }
            
            template<typename ValueType>
            void SparseMdpEndComponentInformation<ValueType>::setValues(std::vector<ValueType>& result, storm::storage::BitVector const& maybeStates, std::vector<ValueType> const& fromResult) {
                auto notInEcResultIt = result.begin();
                for (auto state : maybeStates) {
                    if (this->isStateInEc(state)) {
                        result[state] = result[this->getRowGroupAfterElimination(state)];
                    } else {
                        result[state] = *notInEcResultIt;
                        ++notInEcResultIt;
                    }
                }
                STORM_LOG_ASSERT(notInEcResultIt == result.begin() + this->getNumberOfMaybeStatesNotInEc(), "Mismatching iterators.");
            }
            
            template class SparseMdpEndComponentInformation<double>;
            
#ifdef STORM_HAVE_CARL
            template class SparseMdpEndComponentInformation<storm::RationalNumber>;
            template class SparseMdpEndComponentInformation<storm::RationalFunction>;
#endif
            
        }
    }
}
