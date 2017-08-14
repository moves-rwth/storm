#include "storm/storage/dd/bisimulation/QuotientExtractor.h"

#include <boost/container/flat_map.hpp>

#include "storm/storage/dd/DdManager.h"

#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Ctmc.h"
#include "storm/models/symbolic/Mdp.h"
#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/storage/dd/bisimulation/PreservationInformation.h"

#include "storm/storage/dd/cudd/utility.h"
#include "storm/storage/dd/sylvan/utility.h"

#include "storm/settings/SettingsManager.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm/storage/SparseMatrix.h"
#include "storm/storage/BitVector.h"

#include <sparsepp/spp.h>

namespace storm {
    namespace dd {
        namespace bisimulation {

            template<storm::dd::DdType DdType, typename ValueType>
            class InternalSparseQuotientExtractor;

            template<storm::dd::DdType DdType, typename ValueType>
            class InternalSparseQuotientExtractorBase {
            public:
                InternalSparseQuotientExtractorBase(storm::dd::DdManager<DdType> const& manager, std::set<storm::expressions::Variable> const& stateVariables, std::set<storm::expressions::Variable> const& nondeterminismVariables) : manager(manager) {
                    
                    // Initialize cubes.
                    stateVariablesCube = manager.getBddOne();
                    allSourceVariablesCube = manager.getBddOne();
                    nondeterminismVariablesCube = manager.getBddOne();
                    
                    // Create cubes.
                    for (auto const& variable : stateVariables) {
                        auto const& ddMetaVariable = manager.getMetaVariable(variable);
                        std::vector<std::pair<uint64_t, uint64_t>> indicesAndLevels = ddMetaVariable.getIndicesAndLevels();
                        sourceVariablesIndicesAndLevels.insert(sourceVariablesIndicesAndLevels.end(), indicesAndLevels.begin(), indicesAndLevels.end());
                        
                        allSourceVariablesCube &= ddMetaVariable.getCube();
                        stateVariablesCube &= ddMetaVariable.getCube();
                    }
                    for (auto const& variable : nondeterminismVariables) {
                        auto const& ddMetaVariable = manager.getMetaVariable(variable);
                        allSourceVariablesCube &= ddMetaVariable.getCube();
                        nondeterminismVariablesCube &= ddMetaVariable.getCube();
                        
                        std::vector<std::pair<uint64_t, uint64_t>> indicesAndLevels = ddMetaVariable.getIndicesAndLevels();
                        nondeterminismVariablesIndicesAndLevels.insert(nondeterminismVariablesIndicesAndLevels.end(), indicesAndLevels.begin(), indicesAndLevels.end());
                    }
                    
                    // Sort the indices by their levels.
                    std::sort(sourceVariablesIndicesAndLevels.begin(), sourceVariablesIndicesAndLevels.end(), [] (std::pair<uint64_t, uint64_t> const& a, std::pair<uint64_t, uint64_t> const& b) { return a.second < b.second; } );
                    std::sort(nondeterminismVariablesIndicesAndLevels.begin(), nondeterminismVariablesIndicesAndLevels.end(), [] (std::pair<uint64_t, uint64_t> const& a, std::pair<uint64_t, uint64_t> const& b) { return a.second < b.second; } );
                }

            protected:
                storm::storage::SparseMatrix<ValueType> createMatrixFromEntries(Partition<DdType, ValueType> const& partition) {
                    if (!deterministicEntries.empty()) {
                        return createMatrixFromDeterministicEntries(partition);
                    } else {
                        return createMatrixFromNondeterministicEntries(partition);
                    }
                }
                
                storm::storage::SparseMatrix<ValueType> createMatrixFromNondeterministicEntries(Partition<DdType, ValueType> const& partition) {
                    bool nontrivialRowGrouping = false;
                    uint64_t numberOfChoices = 0;
                    for (auto& group : nondeterministicEntries) {
                        for (auto& choice : group) {
                            auto& row = choice.second;
                            std::sort(row.begin(), row.end(),
                                      [] (storm::storage::MatrixEntry<uint_fast64_t, ValueType> const& a, storm::storage::MatrixEntry<uint_fast64_t, ValueType> const& b) {
                                          return a.getColumn() < b.getColumn();
                                      });
                            ++numberOfChoices;
                        }
                        nontrivialRowGrouping |= group.size() > 1;
                    }
                    
                    storm::storage::SparseMatrixBuilder<ValueType> builder(numberOfChoices, partition.getNumberOfBlocks(), 0, false, nontrivialRowGrouping);
                    uint64_t rowCounter = 0;
                    for (auto& group : nondeterministicEntries) {
                        for (auto& choice : group) {
                            auto& row = choice.second;
                            for (auto const& entry : row) {
                                builder.addNextValue(rowCounter, entry.getColumn(), entry.getValue());
                            }
                        
                            // Free storage for row.
                            row.clear();
                            row.shrink_to_fit();
                        
                            ++rowCounter;
                        }
                        
                        group.clear();
                        
                        if (nontrivialRowGrouping) {
                            builder.newRowGroup(rowCounter);
                        }
                    }
                    nondeterministicEntries.clear();
                    nondeterministicEntries.shrink_to_fit();
                    return builder.build();
                }

                storm::storage::SparseMatrix<ValueType> createMatrixFromDeterministicEntries(Partition<DdType, ValueType> const& partition) {
                    for (auto& row : deterministicEntries) {
                        std::sort(row.begin(), row.end(),
                                  [] (storm::storage::MatrixEntry<uint_fast64_t, ValueType> const& a, storm::storage::MatrixEntry<uint_fast64_t, ValueType> const& b) {
                                      return a.getColumn() < b.getColumn();
                                  });
                    }
                    
                    storm::storage::SparseMatrixBuilder<ValueType> builder(partition.getNumberOfBlocks(), partition.getNumberOfBlocks());
                    uint64_t rowCounter = 0;
                    for (auto& row : deterministicEntries) {
                        for (auto const& entry : row) {
                            builder.addNextValue(rowCounter, entry.getColumn(), entry.getValue());
                        }
                        
                        // Free storage for row.
                        row.clear();
                        row.shrink_to_fit();
                        
                        ++rowCounter;
                    }
                    
                    deterministicEntries.clear();
                    deterministicEntries.shrink_to_fit();
                    
                    return builder.build();
                }
                
                void addMatrixEntry(storm::storage::BitVector const& nondeterminismEncoding, uint64_t sourceBlockIndex, uint64_t targetBlockIndex, ValueType const& value) {
                    if (nondeterminismVariablesIndicesAndLevels.empty()) {
                        this->deterministicEntries[sourceBlockIndex].emplace_back(targetBlockIndex, value);
                    } else {
                        this->nondeterministicEntries[sourceBlockIndex][nondeterminismEncoding].emplace_back(targetBlockIndex, value);
                    }
                }
                
                void reserveMatrixEntries(uint64_t numberOfStates) {
                    if (nondeterminismVariablesIndicesAndLevels.empty()) {
                        this->deterministicEntries.resize(numberOfStates);
                    } else {
                        this->nondeterministicEntries.resize(numberOfStates);
                    }
                }

                // The manager responsible for the DDs.
                storm::dd::DdManager<DdType> const& manager;
                
                // The indices and levels of all state variables.
                std::vector<std::pair<uint64_t, uint64_t>> sourceVariablesIndicesAndLevels;

                // The indices and levels of all state variables.
                std::vector<std::pair<uint64_t, uint64_t>> nondeterminismVariablesIndicesAndLevels;

                // Useful cubes needed in the translation.
                storm::dd::Bdd<DdType> stateVariablesCube;
                storm::dd::Bdd<DdType> allSourceVariablesCube;
                storm::dd::Bdd<DdType> nondeterminismVariablesCube;

                // A hash map that stores the unique source state representative for each source block index.
                spp::sparse_hash_map<uint64_t, std::unique_ptr<storm::storage::BitVector>> uniqueSourceRepresentative;

                // The entries of the matrix that is built if the model is deterministic (DTMC, CTMC).
                std::vector<std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>> deterministicEntries;

                // The entries of the matrix that is built if the model is nondeterministic (MDP).
                std::vector<boost::container::flat_map<storm::storage::BitVector, std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>>> nondeterministicEntries;
            };
            
            template<typename ValueType>
            class InternalSparseQuotientExtractor<storm::dd::DdType::CUDD, ValueType> : public InternalSparseQuotientExtractorBase<storm::dd::DdType::CUDD, ValueType> {
            public:
                InternalSparseQuotientExtractor(storm::dd::DdManager<storm::dd::DdType::CUDD> const& manager, std::set<storm::expressions::Variable> const& stateVariables, std::set<storm::expressions::Variable> const& nondeterminismVariables) : InternalSparseQuotientExtractorBase<storm::dd::DdType::CUDD, ValueType>(manager, stateVariables, nondeterminismVariables), ddman(this->manager.getInternalDdManager().getCuddManager().getManager()) {
                    // Intentionally left empty.
                }
                
                storm::storage::SparseMatrix<ValueType> extractTransitionMatrix(storm::dd::Add<storm::dd::DdType::CUDD, ValueType> const& transitionMatrix, Partition<storm::dd::DdType::CUDD, ValueType> const& partition) {
                    STORM_LOG_ASSERT(partition.storedAsAdd(), "Expected partition stored as ADD.");
                    
                    // Create the number of rows necessary for the matrix.
                    this->reserveMatrixEntries(partition.getNumberOfBlocks());
                    STORM_LOG_TRACE("Partition has " << partition.getNumberOfStates() << " states in " << partition.getNumberOfBlocks() << " blocks.");
                    
                    storm::storage::BitVector stateEncoding(this->sourceVariablesIndicesAndLevels.size());
                    storm::storage::BitVector nondeterminismEncoding(this->nondeterminismVariablesIndicesAndLevels.size());
                    extractTransitionMatrixRec(transitionMatrix.getInternalAdd().getCuddDdNode(), partition.asAdd().getInternalAdd().getCuddDdNode(), partition.asAdd().getInternalAdd().getCuddDdNode(), 0, stateEncoding, nondeterminismEncoding);
                    
                    return this->createMatrixFromEntries(partition);
                }
                
                storm::storage::BitVector extractStates(storm::dd::Bdd<storm::dd::DdType::CUDD> const& states, Partition<storm::dd::DdType::CUDD, ValueType> const& partition) {
                    STORM_LOG_ASSERT(partition.storedAsAdd(), "Expected partition stored as ADD.");

                    storm::storage::BitVector result(partition.getNumberOfBlocks());
                    extractStatesRec(states.getInternalBdd().getCuddDdNode(), partition.asAdd().getInternalAdd().getCuddDdNode(), this->stateVariablesCube.getInternalBdd().getCuddDdNode(), result);
                    
                    return result;
                }
                
            private:
                uint64_t decodeBlockIndex(DdNode* blockEncoding) {
                    std::unique_ptr<uint64_t>& blockCacheEntry = blockDecodeCache[blockEncoding];
                    if (blockCacheEntry) {
                        return *blockCacheEntry;
                    }
                
//                    FILE* fp = fopen("block.dot" , "w");
//                    Cudd_DumpDot(ddman, 1, &blockEncoding, nullptr, nullptr, fp);
//                    fclose(fp);
                    
                    uint64_t result = 0;
                    uint64_t offset = 0;
                    while (blockEncoding != Cudd_ReadOne(ddman)) {
                        DdNode* then = Cudd_T(blockEncoding);
                        if (then != Cudd_ReadZero(ddman)) {
                            blockEncoding = then;
                            result |= 1ull << offset;
                        } else {
                            blockEncoding = Cudd_E(blockEncoding);
                        }
                        ++offset;
                    }
                    
                    blockCacheEntry.reset(new uint64_t(result));
                    
                    return result;
                }
                
                void extractStatesRec(DdNode* statesNode, DdNode* partitionNode, DdNode* stateVariablesNode, storm::storage::BitVector& result) {
                    if (statesNode == Cudd_ReadLogicZero(ddman)) {
                        return;
                    }
                    
                    bool skippedBoth = true;
                    DdNode* tStates;
                    DdNode* eStates;
                    DdNode* tPartition;
                    DdNode* ePartition;
                    bool negate = false;
                    while (skippedBoth && !Cudd_IsConstant(stateVariablesNode)) {
                        if (Cudd_NodeReadIndex(statesNode) == Cudd_NodeReadIndex(stateVariablesNode)) {
                            tStates = Cudd_T(statesNode);
                            eStates = Cudd_E(statesNode);
                            negate = Cudd_IsComplement(statesNode);
                            skippedBoth = false;
                        } else {
                            tStates = eStates = statesNode;
                        }
                        
                        if (Cudd_NodeReadIndex(partitionNode) == Cudd_NodeReadIndex(stateVariablesNode) + 1) {
                            tPartition = Cudd_T(partitionNode);
                            ePartition = Cudd_E(partitionNode);
                            skippedBoth = false;
                        } else {
                            tPartition = ePartition = partitionNode;
                        }
                        
                        if (skippedBoth) {
                            stateVariablesNode = Cudd_T(stateVariablesNode);
                        }
                    }
                    
                    if (Cudd_IsConstant(stateVariablesNode)) {
                        // If there is no more state variables, it means that we arrived at a block encoding in which there is a state in the state set.
                        result.set(decodeBlockIndex(partitionNode));
                        return;
                    } else {
                        // Otherwise, we need to recursively descend.
                        extractStatesRec(negate ? Cudd_Not(tStates) : tStates, tPartition, Cudd_T(stateVariablesNode), result);
                        extractStatesRec(negate ? Cudd_Not(eStates) : eStates, ePartition, Cudd_T(stateVariablesNode), result);
                    }
                }
                
                void extractTransitionMatrixRec(DdNode* transitionMatrixNode, DdNode* sourcePartitionNode, DdNode* targetPartitionNode, uint64_t sourceStateEncodingIndex, storm::storage::BitVector& sourceStateEncoding, storm::storage::BitVector const& nondeterminismEncoding, ValueType const& factor = 1) {
                    // For the empty DD, we do not need to add any entries. Note that the partition nodes cannot be zero
                    // as all states of the model have to be contained.
                    if (transitionMatrixNode == Cudd_ReadZero(ddman)) {
                        return;
                    }

                    // If we have moved through all source variables, we must have arrived at a target block encoding.
                    if (sourceStateEncodingIndex == sourceStateEncoding.size()) {
                        // Decode the source block.
                        uint64_t sourceBlockIndex = decodeBlockIndex(sourcePartitionNode);
                        
                        std::unique_ptr<storm::storage::BitVector>& sourceRepresentative = this->uniqueSourceRepresentative[sourceBlockIndex];
                        if (sourceRepresentative && *sourceRepresentative != sourceStateEncoding) {
                            // In this case, we have picked a different representative and must not record any entries now.
                            return;
                        }
                        
                        // Otherwise, we record the new representative.
                        sourceRepresentative.reset(new storm::storage::BitVector(sourceStateEncoding));
                        
                        // Decode the target block and add entry to matrix.
                        uint64_t targetBlockIndex = decodeBlockIndex(targetPartitionNode);
                        this->addMatrixEntry(nondeterminismEncoding, sourceBlockIndex, targetBlockIndex, factor * Cudd_V(transitionMatrixNode));
                    } else {
                        // Determine the levels in the DDs.
                        uint64_t transitionMatrixVariable = Cudd_NodeReadIndex(transitionMatrixNode);
                        uint64_t sourcePartitionVariable = Cudd_NodeReadIndex(sourcePartitionNode) - 1;
                        uint64_t targetPartitionVariable = Cudd_NodeReadIndex(targetPartitionNode) - 1;
                        
                        // Move through transition matrix.
                        bool skippedSourceInMatrix = false;
                        bool skippedTargetTInMatrix = false;
                        bool skippedTargetEInMatrix = false;
                        DdNode* tt = transitionMatrixNode;
                        DdNode* te = transitionMatrixNode;
                        DdNode* et = transitionMatrixNode;
                        DdNode* ee = transitionMatrixNode;
                        STORM_LOG_ASSERT(transitionMatrixVariable >= this->sourceVariablesIndicesAndLevels[sourceStateEncodingIndex].first, "Illegal top variable of transition matrix.");
                        if (transitionMatrixVariable == this->sourceVariablesIndicesAndLevels[sourceStateEncodingIndex].first) {
                            DdNode* t = Cudd_T(transitionMatrixNode);
                            uint64_t tVariable = Cudd_NodeReadIndex(t);
                            if (tVariable == this->sourceVariablesIndicesAndLevels[sourceStateEncodingIndex].first + 1) {
                                tt = Cudd_T(t);
                                te = Cudd_E(t);
                            } else {
                                tt = te = t;
                                skippedTargetTInMatrix = true;
                            }
                            
                            DdNode* e = Cudd_E(transitionMatrixNode);
                            uint64_t eVariable = Cudd_NodeReadIndex(e);
                            if (eVariable == this->sourceVariablesIndicesAndLevels[sourceStateEncodingIndex].first + 1) {
                                et = Cudd_T(e);
                                ee = Cudd_E(e);
                            } else {
                                et = ee = e;
                                skippedTargetEInMatrix = true;
                            }
                        } else {
                            skippedSourceInMatrix = true;
                            if (transitionMatrixVariable == this->sourceVariablesIndicesAndLevels[sourceStateEncodingIndex].first + 1) {
                                tt = et = Cudd_T(transitionMatrixNode);
                                te = ee = Cudd_E(transitionMatrixNode);
                            } else {
                                tt = te = et = ee = transitionMatrixNode;
                                skippedTargetTInMatrix = skippedTargetEInMatrix = true;
                            }
                        }
                        
                        // Move through partition (for source state).
                        bool skippedInSourcePartition = false;
                        DdNode* sourceT;
                        DdNode* sourceE;
                        STORM_LOG_ASSERT(sourcePartitionVariable >= this->sourceVariablesIndicesAndLevels[sourceStateEncodingIndex].first, "Illegal top variable of source partition.");
                        if (sourcePartitionVariable == this->sourceVariablesIndicesAndLevels[sourceStateEncodingIndex].first) {
                            sourceT = Cudd_T(sourcePartitionNode);
                            sourceE = Cudd_E(sourcePartitionNode);
                        } else {
                            sourceT = sourceE = sourcePartitionNode;
                            skippedInSourcePartition = true;
                        }
                        
                        // Move through partition (for target state).
                        bool skippedInTargetPartition = false;
                        DdNode* targetT;
                        DdNode* targetE;
                        STORM_LOG_ASSERT(targetPartitionVariable >= this->sourceVariablesIndicesAndLevels[sourceStateEncodingIndex].first, "Illegal top variable of source partition.");
                        if (targetPartitionVariable == this->sourceVariablesIndicesAndLevels[sourceStateEncodingIndex].first) {
                            targetT = Cudd_T(targetPartitionNode);
                            targetE = Cudd_E(targetPartitionNode);
                        } else {
                            targetT = targetE = targetPartitionNode;
                            skippedInTargetPartition = true;
                        }
                        
                        // If we skipped the variable in the source partition, we only have to choose one of the two representatives.
                        if (!skippedInSourcePartition) {
                            sourceStateEncoding.set(sourceStateEncodingIndex, true);
                            if (!skippedInTargetPartition) {
                                extractTransitionMatrixRec(tt, sourceT, targetT, sourceStateEncodingIndex + 1, sourceStateEncoding, nondeterminismEncoding, factor);
                            }
                            extractTransitionMatrixRec(te, sourceT, targetE, sourceStateEncodingIndex + 1, sourceStateEncoding, nondeterminismEncoding, skippedTargetTInMatrix && skippedInTargetPartition ? 2 * factor : factor);
                        }
                        
                        sourceStateEncoding.set(sourceStateEncodingIndex, false);
                        // If we skipped the variable in the target partition, just count the one representative twice.
                        if (!skippedInTargetPartition) {
                            extractTransitionMatrixRec(et, sourceE, targetT, sourceStateEncodingIndex + 1, sourceStateEncoding, nondeterminismEncoding, factor);
                        }
                        extractTransitionMatrixRec(ee, sourceE, targetE, sourceStateEncodingIndex + 1, sourceStateEncoding, nondeterminismEncoding, skippedTargetEInMatrix && skippedInTargetPartition ? 2 * factor : factor);
                    }
                }

                ::DdManager* ddman;
                
                spp::sparse_hash_map<DdNode const*, std::unique_ptr<uint64_t>> blockDecodeCache;
            };

            template<typename ValueType>
            class InternalSparseQuotientExtractor<storm::dd::DdType::Sylvan, ValueType> : public InternalSparseQuotientExtractorBase<storm::dd::DdType::Sylvan, ValueType> {
            public:
                InternalSparseQuotientExtractor(storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager, std::set<storm::expressions::Variable> const& stateVariables, std::set<storm::expressions::Variable> const& nondeterminismVariables) : InternalSparseQuotientExtractorBase<storm::dd::DdType::Sylvan, ValueType>(manager, stateVariables, nondeterminismVariables) {
                    // Intentionally left empty.
                }
                
                storm::storage::SparseMatrix<ValueType> extractTransitionMatrix(storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> const& transitionMatrix, Partition<storm::dd::DdType::Sylvan, ValueType> const& partition) {
                    STORM_LOG_ASSERT(partition.storedAsBdd(), "Expected partition stored as BDD.");
                    
                    // Create the number of rows necessary for the matrix.
                    this->reserveMatrixEntries(partition.getNumberOfBlocks());

                    storm::storage::BitVector stateEncoding(this->sourceVariablesIndicesAndLevels.size());
                    storm::storage::BitVector nondeterminismEncoding;
                    extractTransitionMatrixRec(transitionMatrix.getInternalAdd().getSylvanMtbdd().GetMTBDD(), partition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), partition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), 0, stateEncoding, nondeterminismEncoding);
                    
                    return this->createMatrixFromEntries(partition);
                }
                
                storm::storage::BitVector extractStates(storm::dd::Bdd<storm::dd::DdType::Sylvan> const& states, Partition<storm::dd::DdType::Sylvan, ValueType> const& partition) {
                    STORM_LOG_ASSERT(partition.storedAsBdd(), "Expected partition stored as BDD.");
                    
                    storm::storage::BitVector result(partition.getNumberOfBlocks());
                    extractStatesRec(states.getInternalBdd().getSylvanBdd().GetBDD(), partition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), this->stateVariablesCube.getInternalBdd().getSylvanBdd().GetBDD(), result);
                    
                    return result;
                }
                
            private:
                uint64_t decodeBlockIndex(BDD blockEncoding) {
                    std::unique_ptr<uint64_t>& blockCacheEntry = blockDecodeCache[blockEncoding];
                    if (blockCacheEntry) {
                        return *blockCacheEntry;
                    }
                    
                    uint64_t result = 0;
                    uint64_t offset = 0;
                    while (blockEncoding != sylvan_true) {
                        if (sylvan_high(blockEncoding) != sylvan_false) {
                            blockEncoding = sylvan_high(blockEncoding);
                            result |= 1ull << offset;
                        } else {
                            blockEncoding = sylvan_low(blockEncoding);
                        }
                        ++offset;
                    }
                    
                    blockCacheEntry.reset(new uint64_t(result));
                    
                    return result;
                }
                
                void extractStatesRec(BDD statesNode, BDD partitionNode, BDD stateVariablesNode, storm::storage::BitVector& result) {
                    if (statesNode == sylvan_false) {
                        return;
                    }
                    
                    bool skippedBoth = true;
                    BDD tStates;
                    BDD eStates;
                    BDD tPartition;
                    BDD ePartition;
                    while (skippedBoth && !sylvan_isconst(stateVariablesNode)) {
                        if (sylvan_var(statesNode) == sylvan_var(stateVariablesNode)) {
                            tStates = sylvan_high(statesNode);
                            eStates = sylvan_low(statesNode);
                            skippedBoth = false;
                        } else {
                            tStates = eStates = statesNode;
                        }
                        
                        if (sylvan_var(partitionNode) == sylvan_var(stateVariablesNode) + 1) {
                            tPartition = sylvan_high(partitionNode);
                            ePartition = sylvan_low(partitionNode);
                            skippedBoth = false;
                        } else {
                            tPartition = ePartition = partitionNode;
                        }
                        
                        if (skippedBoth) {
                            stateVariablesNode = sylvan_high(stateVariablesNode);
                        }
                    }
                    
                    if (sylvan_isconst(stateVariablesNode)) {
                        // If there is no more state variables, it means that we arrived at a block encoding in which there is a state in the state set.
                        result.set(decodeBlockIndex(partitionNode));
                        return;
                    } else {
                        // Otherwise, we need to recursively descend.
                        extractStatesRec(tStates, tPartition, sylvan_high(stateVariablesNode), result);
                        extractStatesRec(eStates, ePartition, sylvan_high(stateVariablesNode), result);
                    }
                }
                
                void extractTransitionMatrixRec(MTBDD transitionMatrixNode, BDD sourcePartitionNode, BDD targetPartitionNode, uint64_t currentIndex, storm::storage::BitVector& sourceState, storm::storage::BitVector const& nondeterminismEncoding, ValueType const& factor = storm::utility::one<ValueType>()) {
                    // For the empty DD, we do not need to add any entries. Note that the partition nodes cannot be zero
                    // as all states of the model have to be contained.
                    if (mtbdd_iszero(transitionMatrixNode)) {
                        return;
                    }
                    
                    // If we have moved through all source variables, we must have arrived at a target block encoding.
                    if (currentIndex == sourceState.size()) {
                        // Decode the source block.
                        uint64_t sourceBlockIndex = decodeBlockIndex(sourcePartitionNode);
                        
                        std::unique_ptr<storm::storage::BitVector>& sourceRepresentative = this->uniqueSourceRepresentative[sourceBlockIndex];
                        if (sourceRepresentative && *sourceRepresentative != sourceState) {
                            // In this case, we have picked a different representative and must not record any entries now.
                            return;
                        }
                        
                        // Otherwise, we record the new representative.
                        sourceRepresentative.reset(new storm::storage::BitVector(sourceState));
                        
                        // Decode the target block and add matrix entry.
                        uint64_t targetBlockIndex = decodeBlockIndex(targetPartitionNode);
                        this->addMatrixEntry(nondeterminismEncoding, sourceBlockIndex, targetBlockIndex, factor * storm::dd::InternalAdd<storm::dd::DdType::Sylvan, ValueType>::getValue(transitionMatrixNode));
                    } else {
                        // Determine the levels in the DDs.
                        uint64_t transitionMatrixVariable = sylvan_isconst(transitionMatrixNode) ? 0xffffffff : sylvan_var(transitionMatrixNode);
                        uint64_t sourcePartitionVariable = sylvan_var(sourcePartitionNode) - 1;
                        uint64_t targetPartitionVariable = sylvan_var(targetPartitionNode) - 1;
                        
                        // Move through transition matrix.
                        bool skippedSourceInMatrix = false;
                        bool skippedTargetTInMatrix = false;
                        bool skippedTargetEInMatrix = false;
                        MTBDD tt = transitionMatrixNode;
                        MTBDD te = transitionMatrixNode;
                        MTBDD et = transitionMatrixNode;
                        MTBDD ee = transitionMatrixNode;
                        if (transitionMatrixVariable == this->sourceVariablesIndicesAndLevels[currentIndex].first) {
                            MTBDD t = sylvan_high(transitionMatrixNode);
                            MTBDD e = sylvan_low(transitionMatrixNode);
                            
                            uint64_t tVariable = sylvan_isconst(t) ? 0xffffffff : sylvan_var(t);
                            if (tVariable == this->sourceVariablesIndicesAndLevels[currentIndex].first + 1) {
                                tt = sylvan_high(t);
                                te = sylvan_low(t);
                            } else {
                                tt = te = t;
                                skippedTargetTInMatrix = true;
                            }
                            
                            uint64_t eVariable = sylvan_isconst(e) ? 0xffffffff : sylvan_var(e);
                            if (eVariable == this->sourceVariablesIndicesAndLevels[currentIndex].first + 1) {
                                et = sylvan_high(e);
                                ee = sylvan_low(e);
                            } else {
                                et = ee = e;
                                skippedTargetEInMatrix = true;
                            }
                        } else {
                            skippedSourceInMatrix = true;
                            if (transitionMatrixVariable == this->sourceVariablesIndicesAndLevels[currentIndex].first + 1) {
                                tt = et = sylvan_high(transitionMatrixNode);
                                te = ee = sylvan_low(transitionMatrixNode);
                            } else {
                                tt = te = et = ee = transitionMatrixNode;
                                skippedTargetTInMatrix = skippedTargetEInMatrix = true;
                            }
                        }
                        
                        // Move through partition (for source state).
                        bool skippedInSourcePartition = false;
                        MTBDD sourceT;
                        MTBDD sourceE;
                        if (sourcePartitionVariable == this->sourceVariablesIndicesAndLevels[currentIndex].first) {
                            sourceT = sylvan_high(sourcePartitionNode);
                            sourceE = sylvan_low(sourcePartitionNode);
                        } else {
                            sourceT = sourceE = sourcePartitionNode;
                            skippedInSourcePartition = true;
                        }
                        
                        // Move through partition (for target state).
                        bool skippedInTargetPartition = false;
                        MTBDD targetT;
                        MTBDD targetE;
                        if (targetPartitionVariable == this->sourceVariablesIndicesAndLevels[currentIndex].first) {
                            targetT = sylvan_high(targetPartitionNode);
                            targetE = sylvan_low(targetPartitionNode);
                        } else {
                            targetT = targetE = targetPartitionNode;
                            skippedInTargetPartition = true;
                        }
                        
                        // If we skipped the variable in the source partition, we only have to choose one of the two representatives.
                        if (!skippedInSourcePartition) {
                            sourceState.set(currentIndex, true);
                            // If we skipped the variable in the target partition, just count the one representative twice.
                            if (!skippedInTargetPartition) {
                                extractTransitionMatrixRec(tt, sourceT, targetT, currentIndex + 1, sourceState, nondeterminismEncoding, factor);
                            }
                            extractTransitionMatrixRec(te, sourceT, targetE, currentIndex + 1, sourceState, nondeterminismEncoding, skippedTargetTInMatrix && skippedInTargetPartition ? 2 * factor : factor);
                        }
                        
                        sourceState.set(currentIndex, false);
                        // If we skipped the variable in the target partition, just count the one representative twice.
                        if (!skippedInTargetPartition) {
                            extractTransitionMatrixRec(et, sourceE, targetT, currentIndex + 1, sourceState, nondeterminismEncoding, factor);
                        }
                        extractTransitionMatrixRec(ee, sourceE, targetE, currentIndex + 1, sourceState, nondeterminismEncoding, skippedTargetEInMatrix && skippedInTargetPartition ? 2 * factor : factor);
                    }
                }
                
                spp::sparse_hash_map<BDD, std::unique_ptr<uint64_t>> blockDecodeCache;
            };

            template<storm::dd::DdType DdType, typename ValueType>
            QuotientExtractor<DdType, ValueType>::QuotientExtractor() : useRepresentatives(false) {
                auto const& settings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
                this->useRepresentatives = settings.isUseRepresentativesSet();
                this->quotientFormat = settings.getQuotientFormat();
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::Model<ValueType>> QuotientExtractor<DdType, ValueType>::extract(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation) {
                auto start = std::chrono::high_resolution_clock::now();
                std::shared_ptr<storm::models::Model<ValueType>> result;
                if (quotientFormat == storm::settings::modules::BisimulationSettings::QuotientFormat::Sparse) {
                    result = extractSparseQuotient(model, partition, preservationInformation);
                } else {
                    result = extractDdQuotient(model, partition, preservationInformation);
                }
                auto end = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Quotient extraction completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");
                
                STORM_LOG_THROW(result, storm::exceptions::NotSupportedException, "Quotient could not be extracted.");
                
                return result;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::sparse::Model<ValueType>> QuotientExtractor<DdType, ValueType>::extractSparseQuotient(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation) {
                InternalSparseQuotientExtractor<DdType, ValueType> sparseExtractor(model.getManager(), model.getRowVariables(), model.getNondeterminismVariables());
                auto states = partition.getStates().swapVariables(model.getRowColumnMetaVariablePairs());
                
                auto start = std::chrono::high_resolution_clock::now();
                storm::storage::SparseMatrix<ValueType> quotientTransitionMatrix = sparseExtractor.extractTransitionMatrix(model.getTransitionMatrix(), partition);
                auto end = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Quotient transition matrix extracted in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");
                
                start = std::chrono::high_resolution_clock::now();
                storm::models::sparse::StateLabeling quotientStateLabeling(partition.getNumberOfBlocks());
                quotientStateLabeling.addLabel("init", sparseExtractor.extractStates(model.getInitialStates(), partition));
                quotientStateLabeling.addLabel("deadlock", sparseExtractor.extractStates(model.getDeadlockStates(), partition));
                
                for (auto const& label : preservationInformation.getLabels()) {
                    quotientStateLabeling.addLabel(label, sparseExtractor.extractStates(model.getStates(label), partition));
                }
                for (auto const& expression : preservationInformation.getExpressions()) {
                    std::stringstream stream;
                    stream << expression;
                    std::string expressionAsString = stream.str();
                    
                    if (quotientStateLabeling.containsLabel(expressionAsString)) {
                        STORM_LOG_WARN("Duplicate label '" << expressionAsString << "', dropping second label definition.");
                    } else {
                        quotientStateLabeling.addLabel(stream.str(), sparseExtractor.extractStates(model.getStates(expression), partition));
                    }
                }
                end = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Quotient labels extracted in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");

                std::shared_ptr<storm::models::sparse::Model<ValueType>> result;
                if (model.getType() == storm::models::ModelType::Dtmc) {
                    result = std::make_shared<storm::models::sparse::Dtmc<ValueType>>(std::move(quotientTransitionMatrix), std::move(quotientStateLabeling));
                } else if (model.getType() == storm::models::ModelType::Ctmc) {
                    result = std::make_shared<storm::models::sparse::Ctmc<ValueType>>(std::move(quotientTransitionMatrix), std::move(quotientStateLabeling));
                }
                
                return result;
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> QuotientExtractor<DdType, ValueType>::extractDdQuotient(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation) {
                return extractQuotientUsingBlockVariables(model, partition, preservationInformation);
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> QuotientExtractor<DdType, ValueType>::extractQuotientUsingBlockVariables(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation) {
                auto modelType = model.getType();
                
                bool useRepresentativesForThisExtraction = this->useRepresentatives;
                if (modelType == storm::models::ModelType::Dtmc || modelType == storm::models::ModelType::Ctmc || modelType == storm::models::ModelType::Mdp) {
                    if (modelType == storm::models::ModelType::Mdp) {
                        STORM_LOG_WARN_COND(!useRepresentativesForThisExtraction, "Using representatives is unsupported for MDPs, falling back to regular extraction.");
                        useRepresentativesForThisExtraction = false;
                    }
                    
                    // Sanity checks.
                    STORM_LOG_ASSERT(partition.getNumberOfStates() == model.getNumberOfStates(), "Mismatching partition size.");
                    STORM_LOG_ASSERT(partition.getStates().renameVariables(model.getColumnVariables(), model.getRowVariables()) == model.getReachableStates(), "Mismatching partition.");
                    
                    std::set<storm::expressions::Variable> blockVariableSet = {partition.getBlockVariable()};
                    std::set<storm::expressions::Variable> blockPrimeVariableSet = {partition.getPrimedBlockVariable()};
                    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> blockMetaVariablePairs = {std::make_pair(partition.getBlockVariable(), partition.getPrimedBlockVariable())};
                    
                    storm::dd::Bdd<DdType> partitionAsBdd = partition.storedAsBdd() ? partition.asBdd() : partition.asAdd().notZero();
                    if (useRepresentativesForThisExtraction) {
                        storm::dd::Bdd<DdType> partitionAsBddOverPrimedBlockVariable = partitionAsBdd.renameVariables(blockVariableSet, blockPrimeVariableSet);
                        storm::dd::Bdd<DdType> representativePartition = partitionAsBddOverPrimedBlockVariable.existsAbstractRepresentative(model.getColumnVariables()).renameVariables(model.getColumnVariables(), blockVariableSet);
                        partitionAsBdd = (representativePartition && partitionAsBddOverPrimedBlockVariable).existsAbstract(blockPrimeVariableSet);
                    }
                    
                    auto start = std::chrono::high_resolution_clock::now();
                    storm::dd::Bdd<DdType> partitionAsBddOverRowVariables = partitionAsBdd.renameVariables(model.getColumnVariables(), model.getRowVariables());
                    storm::dd::Bdd<DdType> reachableStates = partitionAsBdd.existsAbstract(model.getColumnVariables());
                    storm::dd::Bdd<DdType> initialStates = (model.getInitialStates() && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables());
                    
                    std::map<std::string, storm::dd::Bdd<DdType>> preservedLabelBdds;
                    for (auto const& label : preservationInformation.getLabels()) {
                        preservedLabelBdds.emplace(label, (model.getStates(label) && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables()));
                    }
                    for (auto const& expression : preservationInformation.getExpressions()) {
                        std::stringstream stream;
                        stream << expression;
                        std::string expressionAsString = stream.str();
                        
                        auto it = preservedLabelBdds.find(expressionAsString);
                        if (it != preservedLabelBdds.end()) {
                            STORM_LOG_WARN("Duplicate label '" << expressionAsString << "', dropping second label definition.");
                        } else {
                            preservedLabelBdds.emplace(stream.str(), (model.getStates(expression) && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables()));
                        }
                    }
                    auto end = std::chrono::high_resolution_clock::now();
                    STORM_LOG_TRACE("Quotient labels extracted in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");

                    storm::dd::Add<DdType, ValueType> partitionAsAdd = partitionAsBdd.template toAdd<ValueType>();
                    start = std::chrono::high_resolution_clock::now();
                    storm::dd::Add<DdType, ValueType> quotientTransitionMatrix = model.getTransitionMatrix().multiplyMatrix(partitionAsAdd.renameVariables(blockVariableSet, blockPrimeVariableSet), model.getColumnVariables());
                    
                    // Pick a representative from each block.
                    partitionAsBdd = partitionAsBdd.existsAbstractRepresentative(model.getColumnVariables());
                    partitionAsAdd = partitionAsBdd.template toAdd<ValueType>();
                    
                    quotientTransitionMatrix = quotientTransitionMatrix.multiplyMatrix(partitionAsAdd.renameVariables(model.getColumnVariables(), model.getRowVariables()), model.getRowVariables());
                    end = std::chrono::high_resolution_clock::now();
                    
                    // Check quotient matrix for sanity.
                    STORM_LOG_ASSERT(quotientTransitionMatrix.greater(storm::utility::one<ValueType>()).isZero(), "Illegal entries in quotient matrix.");
                    STORM_LOG_ASSERT(quotientTransitionMatrix.sumAbstract(blockPrimeVariableSet).equalModuloPrecision(quotientTransitionMatrix.notZero().existsAbstract(blockPrimeVariableSet).template toAdd<ValueType>(), ValueType(1e-6)), "Illegal non-probabilistic matrix.");
                    
                    STORM_LOG_TRACE("Quotient transition matrix extracted in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");

                    storm::dd::Bdd<DdType> quotientTransitionMatrixBdd = quotientTransitionMatrix.notZero();
                    storm::dd::Bdd<DdType> deadlockStates = !quotientTransitionMatrixBdd.existsAbstract(blockPrimeVariableSet) && reachableStates;
                    
                    if (modelType == storm::models::ModelType::Dtmc) {
                        return std::shared_ptr<storm::models::symbolic::Dtmc<DdType, ValueType>>(new storm::models::symbolic::Dtmc<DdType, ValueType>(model.getManager().asSharedPointer(), reachableStates, initialStates, deadlockStates, quotientTransitionMatrix, blockVariableSet, blockPrimeVariableSet, blockMetaVariablePairs, preservedLabelBdds, {}));
                    } else if (modelType == storm::models::ModelType::Ctmc) {
                        return std::shared_ptr<storm::models::symbolic::Ctmc<DdType, ValueType>>(new storm::models::symbolic::Ctmc<DdType, ValueType>(model.getManager().asSharedPointer(), reachableStates, initialStates, deadlockStates, quotientTransitionMatrix, blockVariableSet, blockPrimeVariableSet, blockMetaVariablePairs, preservedLabelBdds, {}));
                    } else if (modelType == storm::models::ModelType::Mdp) {
                        return std::shared_ptr<storm::models::symbolic::Mdp<DdType, ValueType>>(new storm::models::symbolic::Mdp<DdType, ValueType>(model.getManager().asSharedPointer(), reachableStates, initialStates, deadlockStates, quotientTransitionMatrix, blockVariableSet, blockPrimeVariableSet, blockMetaVariablePairs, model.getNondeterminismVariables(), preservedLabelBdds, {}));
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unsupported quotient type.");
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Cannot extract quotient for this model type.");
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> QuotientExtractor<DdType, ValueType>::extractQuotientUsingOriginalVariables(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation) {
                auto modelType = model.getType();
                
                if (modelType == storm::models::ModelType::Dtmc || modelType == storm::models::ModelType::Ctmc) {
                    std::set<storm::expressions::Variable> blockVariableSet = {partition.getBlockVariable()};
                    std::set<storm::expressions::Variable> blockPrimeVariableSet = {partition.getPrimedBlockVariable()};
                    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> blockMetaVariablePairs = {std::make_pair(partition.getBlockVariable(), partition.getPrimedBlockVariable())};
                    
                    storm::dd::Add<DdType, ValueType> partitionAsAdd = partition.storedAsBdd() ? partition.asBdd().template toAdd<ValueType>() : partition.asAdd();
                    storm::dd::Add<DdType, ValueType> quotientTransitionMatrix = model.getTransitionMatrix().multiplyMatrix(partitionAsAdd, model.getColumnVariables());
                    quotientTransitionMatrix = quotientTransitionMatrix.renameVariables(blockVariableSet, model.getColumnVariables());
                    partitionAsAdd = partitionAsAdd / partitionAsAdd.sumAbstract(model.getColumnVariables());
                    quotientTransitionMatrix = quotientTransitionMatrix.multiplyMatrix(partitionAsAdd, model.getRowVariables());
                    quotientTransitionMatrix = quotientTransitionMatrix.renameVariables(blockVariableSet, model.getRowVariables());
                    storm::dd::Bdd<DdType> quotientTransitionMatrixBdd = quotientTransitionMatrix.notZero();
                    
                    // Check quotient matrix for sanity.
                    STORM_LOG_ASSERT(quotientTransitionMatrix.greater(storm::utility::one<ValueType>()).isZero(), "Illegal entries in quotient matrix.");
                    STORM_LOG_ASSERT(quotientTransitionMatrix.sumAbstract(blockPrimeVariableSet).equalModuloPrecision(quotientTransitionMatrix.notZero().existsAbstract(blockPrimeVariableSet).template toAdd<ValueType>(), ValueType(1e-6)), "Illegal non-probabilistic matrix.");
                    
                    storm::dd::Bdd<DdType> partitionAsBdd = partition.storedAsBdd() ? partition.asBdd() : partition.asAdd().notZero();
                    storm::dd::Bdd<DdType> partitionAsBddOverRowVariables = partitionAsBdd.renameVariables(model.getColumnVariables(), model.getRowVariables());
                    storm::dd::Bdd<DdType> reachableStates = partitionAsBdd.existsAbstract(model.getColumnVariables()).renameVariables(blockVariableSet, model.getRowVariables());
                    storm::dd::Bdd<DdType> initialStates = (model.getInitialStates() && partitionAsBdd.renameVariables(model.getColumnVariables(), model.getRowVariables())).existsAbstract(model.getRowVariables()).renameVariables(blockVariableSet, model.getRowVariables());
                    storm::dd::Bdd<DdType> deadlockStates = !quotientTransitionMatrixBdd.existsAbstract(model.getColumnVariables()) && reachableStates;
                    
                    std::map<std::string, storm::dd::Bdd<DdType>> preservedLabelBdds;
                    for (auto const& label : preservationInformation.getLabels()) {
                        preservedLabelBdds.emplace(label, (model.getStates(label) && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables()));
                    }
                    for (auto const& expression : preservationInformation.getExpressions()) {
                        std::stringstream stream;
                        stream << expression;
                        std::string expressionAsString = stream.str();
                        
                        auto it = preservedLabelBdds.find(expressionAsString);
                        if (it != preservedLabelBdds.end()) {
                            STORM_LOG_WARN("Duplicate label '" << expressionAsString << "', dropping second label definition.");
                        } else {
                            preservedLabelBdds.emplace(stream.str(), (model.getStates(expression) && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables()));
                        }
                    }
                    
                    if (modelType == storm::models::ModelType::Dtmc) {
                        return std::shared_ptr<storm::models::symbolic::Dtmc<DdType, ValueType>>(new storm::models::symbolic::Dtmc<DdType, ValueType>(model.getManager().asSharedPointer(), reachableStates, initialStates, deadlockStates, quotientTransitionMatrix, model.getRowVariables(), model.getColumnVariables(), model.getRowColumnMetaVariablePairs(), preservedLabelBdds, {}));
                    } else {
                        return std::shared_ptr<storm::models::symbolic::Ctmc<DdType, ValueType>>(new storm::models::symbolic::Ctmc<DdType, ValueType>(model.getManager().asSharedPointer(), reachableStates, initialStates, deadlockStates, quotientTransitionMatrix, blockVariableSet, blockPrimeVariableSet, blockMetaVariablePairs, preservedLabelBdds, {}));
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Cannot exctract quotient for this model type.");
                }
            }
            
            template class QuotientExtractor<storm::dd::DdType::CUDD, double>;
            
            template class QuotientExtractor<storm::dd::DdType::Sylvan, double>;
            template class QuotientExtractor<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class QuotientExtractor<storm::dd::DdType::Sylvan, storm::RationalFunction>;
            
        }
    }
}
