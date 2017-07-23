#include "storm/storage/dd/bisimulation/QuotientExtractor.h"

#include "storm/storage/dd/DdManager.h"

#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Ctmc.h"
#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/storage/dd/bisimulation/PreservationInformation.h"

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
                InternalSparseQuotientExtractorBase(storm::dd::DdManager<DdType> const& manager, std::set<storm::expressions::Variable> const& stateVariables) : manager(manager) {
                    for (auto const& variable : stateVariables) {
                        auto const& ddMetaVariable = manager.getMetaVariable(variable);
                        std::vector<std::pair<uint64_t, uint64_t>> indicesAndLevels = ddMetaVariable.getIndicesAndLevels();
                        stateVariablesIndicesAndLevels.insert(stateVariablesIndicesAndLevels.end(), indicesAndLevels.begin(), indicesAndLevels.end());
                    }
                    
                    // Sort the indices by their levels.
                    std::sort(stateVariablesIndicesAndLevels.begin(), stateVariablesIndicesAndLevels.end(), [] (std::pair<uint64_t, uint64_t> const& a, std::pair<uint64_t, uint64_t> const& b) { return a.second < b.second; } );
                }

            protected:
                storm::storage::SparseMatrix<ValueType> createMatrixFromEntries(Partition<DdType, ValueType> const& partition) {
                    for (auto& row : entries) {
                        std::sort(row.begin(), row.end(), [] (storm::storage::MatrixEntry<uint_fast64_t, ValueType> const& a, storm::storage::MatrixEntry<uint_fast64_t, ValueType> const& b) { return a.getColumn() < b.getColumn(); } );
                    }
                    
                    storm::storage::SparseMatrixBuilder<ValueType> builder(partition.getNumberOfBlocks(), partition.getNumberOfBlocks());
                    uint64_t rowCounter = 0;
                    for (auto& row : entries) {
                        for (auto const& entry : row) {
                            builder.addNextValue(rowCounter, entry.getColumn(), entry.getValue());
                        }
                        
                        // Free storage for row.
                        row.clear();
                        row.shrink_to_fit();
                        
                        ++rowCounter;
                    }
                    
                    return builder.build();
                }
                
                storm::dd::DdManager<DdType> const& manager;
                std::vector<std::pair<uint64_t, uint64_t>> stateVariablesIndicesAndLevels;
                std::vector<std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>> entries;
            };
            
            template<typename ValueType>
            class InternalSparseQuotientExtractor<storm::dd::DdType::CUDD, ValueType> : public InternalSparseQuotientExtractorBase<storm::dd::DdType::CUDD, ValueType> {
            public:
                InternalSparseQuotientExtractor(storm::dd::DdManager<storm::dd::DdType::CUDD> const& manager, std::set<storm::expressions::Variable> const& stateVariables) : InternalSparseQuotientExtractorBase<storm::dd::DdType::CUDD, ValueType>(manager, stateVariables), ddman(this->manager.getInternalDdManager().getCuddManager().getManager()) {
                    // Intentionally left empty.
                }
                
                storm::storage::SparseMatrix<ValueType> extractTransitionMatrix(storm::dd::Add<storm::dd::DdType::CUDD, ValueType> const& transitionMatrix, Partition<storm::dd::DdType::CUDD, ValueType> const& partition) {
                    STORM_LOG_ASSERT(partition.storedAsAdd(), "Expected partition stored as ADD.");
                    
                    // Create the number of rows necessary for the matrix.
                    this->entries.resize(partition.getNumberOfBlocks());
                    
                    storm::storage::BitVector encoding(this->stateVariablesIndicesAndLevels.size());
                    extractTransitionMatrixRec(transitionMatrix.getInternalAdd().getCuddDdNode(), partition.asAdd().getInternalAdd().getCuddDdNode(), partition.asAdd().getInternalAdd().getCuddDdNode(), 0, encoding);
                    
                    return this->createMatrixFromEntries(partition);
                }
                
                storm::storage::BitVector extractStates(storm::dd::Bdd<storm::dd::DdType::CUDD> const& states, Partition<storm::dd::DdType::CUDD, ValueType> const& partition) {
                    STORM_LOG_ASSERT(partition.storedAsAdd(), "Expected partition stored as ADD.");

                    storm::storage::BitVector result(partition.getNumberOfBlocks());
                    extractStatesRec(states.getInternalBdd().getCuddDdNode(), partition.asAdd().getInternalAdd().getCuddDdNode(), 0, result);
                    
                    return result;
                }
                
            private:
                uint64_t decodeBlockIndex(DdNode* blockEncoding) {
                    std::unique_ptr<uint64_t>& blockCacheEntry = blockDecodeCache[blockEncoding];
                    if (blockCacheEntry) {
                        return *blockCacheEntry;
                    }
                
                    uint64_t result = 0;
                    uint64_t offset = 0;
                    while (blockEncoding != Cudd_ReadOne(ddman)) {
                        if (Cudd_T(blockEncoding) != Cudd_ReadZero(ddman)) {
                            blockEncoding = Cudd_T(blockEncoding);
                            result |= 1ull << offset;
                        } else {
                            blockEncoding = Cudd_E(blockEncoding);
                        }
                        ++offset;
                    }
                    
                    blockCacheEntry.reset(new uint64_t(result));
                    
                    return result;
                }
                
                void extractStatesRec(DdNode* statesNode, DdNode* partitionNode, uint64_t offset, storm::storage::BitVector& result) {
                    if (statesNode == Cudd_ReadLogicZero(ddman)) {
                        return;
                    }
                    
                    // Determine the levels in the DDs.
                    uint64_t statesVariable = Cudd_NodeReadIndex(statesNode);
                    uint64_t partitionVariable = Cudd_NodeReadIndex(partitionNode) - 1;
                    
                    // See how many variables we skipped.
                    while (offset < this->stateVariablesIndicesAndLevels.size() && statesVariable != this->stateVariablesIndicesAndLevels[offset].first && partitionVariable != this->stateVariablesIndicesAndLevels[offset].first) {
                        ++offset;
                    }

                    if (offset == this->stateVariablesIndicesAndLevels.size()) {
                        result.set(decodeBlockIndex(partitionNode));
                        return;
                    }
                    
                    uint64_t topVariable;
                    if (statesVariable == this->stateVariablesIndicesAndLevels[offset].first) {
                        topVariable = statesVariable;
                    } else {
                        topVariable = partitionVariable;
                    }
                    
                    DdNode* tStates = statesNode;
                    DdNode* eStates = statesNode;
                    bool negate = false;
                    if (topVariable == statesVariable) {
                        tStates = Cudd_T(statesNode);
                        eStates = Cudd_E(statesNode);
                        negate = Cudd_IsComplement(statesNode);
                    }
                    
                    DdNode* tPartition = partitionNode;
                    DdNode* ePartition = partitionNode;
                    if (topVariable == partitionVariable) {
                        tPartition = Cudd_T(partitionNode);
                        ePartition = Cudd_E(partitionNode);
                    }
                    
                    extractStatesRec(negate ? Cudd_Not(tStates) : tStates, tPartition, offset, result);
                    extractStatesRec(negate ? Cudd_Not(eStates) : eStates, ePartition, offset, result);
                }
                
                void extractTransitionMatrixRec(DdNode* transitionMatrixNode, DdNode* sourcePartitionNode, DdNode* targetPartitionNode, uint64_t currentIndex, storm::storage::BitVector& sourceState) {
                    // For the empty DD, we do not need to add any entries. Note that the partition nodes cannot be zero
                    // as all states of the model have to be contained.
                    if (transitionMatrixNode == Cudd_ReadZero(ddman)) {
                        return;
                    }

                    // If we have moved through all source variables, we must have arrived at a constant.
                    if (currentIndex == sourceState.size()) {
                        // Decode the source block.
                        uint64_t sourceBlockIndex = decodeBlockIndex(sourcePartitionNode);
                        
                        std::unique_ptr<storm::storage::BitVector>& sourceRepresentative = uniqueSourceRepresentative[sourceBlockIndex];
                        if (sourceRepresentative && *sourceRepresentative != sourceState) {
                            // In this case, we have picked a different representative and must not record any entries now.
                            return;
                        }
                        
                        // Otherwise, we record the new representative.
                        sourceRepresentative.reset(new storm::storage::BitVector(sourceState));
                        
                        // Decode the target block.
                        uint64_t targetBlockIndex = decodeBlockIndex(targetPartitionNode);
                        
                        this->entries[sourceBlockIndex].emplace_back(targetBlockIndex, Cudd_V(transitionMatrixNode));
                    } else {
                        // Determine the levels in the DDs.
                        uint64_t transitionMatrixVariable = Cudd_NodeReadIndex(transitionMatrixNode);
                        uint64_t sourcePartitionVariable = Cudd_NodeReadIndex(sourcePartitionNode) - 1;
                        uint64_t targetPartitionVariable = Cudd_NodeReadIndex(targetPartitionNode) - 1;
                        
                        // Move through transition matrix.
                        DdNode* tt = transitionMatrixNode;
                        DdNode* te = transitionMatrixNode;
                        DdNode* et = transitionMatrixNode;
                        DdNode* ee = transitionMatrixNode;
                        if (transitionMatrixVariable == this->stateVariablesIndicesAndLevels[currentIndex].first) {
                            DdNode* t = Cudd_T(transitionMatrixNode);
                            DdNode* e = Cudd_E(transitionMatrixNode);
                            
                            uint64_t tVariable = Cudd_NodeReadIndex(t);
                            if (tVariable == this->stateVariablesIndicesAndLevels[currentIndex].first + 1) {
                                tt = Cudd_T(t);
                                te = Cudd_E(t);
                            } else {
                                tt = te = t;
                            }
                            
                            uint64_t eVariable = Cudd_NodeReadIndex(e);
                            if (eVariable == this->stateVariablesIndicesAndLevels[currentIndex].first + 1) {
                                et = Cudd_T(e);
                                ee = Cudd_E(e);
                            } else {
                                et = ee = e;
                            }
                        } else {
                            if (transitionMatrixVariable == this->stateVariablesIndicesAndLevels[currentIndex].first + 1) {
                                tt = Cudd_T(transitionMatrixNode);
                                te = Cudd_E(transitionMatrixNode);
                            } else {
                                tt = te = transitionMatrixNode;
                            }
                        }
                        
                        // Move through partition (for source state).
                        DdNode* sourceT;
                        DdNode* sourceE;
                        if (sourcePartitionVariable == this->stateVariablesIndicesAndLevels[currentIndex].first) {
                            sourceT = Cudd_T(sourcePartitionNode);
                            sourceE = Cudd_E(sourcePartitionNode);
                        } else {
                            sourceT = sourceE = sourcePartitionNode;
                        }
                        
                        // Move through partition (for target state).
                        DdNode* targetT;
                        DdNode* targetE;
                        if (targetPartitionVariable == this->stateVariablesIndicesAndLevels[currentIndex].first) {
                            targetT = Cudd_T(targetPartitionNode);
                            targetE = Cudd_E(targetPartitionNode);
                        } else {
                            targetT = targetE = targetPartitionNode;
                        }
                        
                        sourceState.set(currentIndex, true);
                        extractTransitionMatrixRec(tt, sourceT, targetT, currentIndex + 1, sourceState);
                        extractTransitionMatrixRec(te, sourceT, targetE, currentIndex + 1, sourceState);
                        
                        sourceState.set(currentIndex, false);
                        extractTransitionMatrixRec(et, sourceE, targetT, currentIndex + 1, sourceState);
                        extractTransitionMatrixRec(ee, sourceE, targetE, currentIndex + 1, sourceState);
                    }
                }

                ::DdManager* ddman;
                
                spp::sparse_hash_map<uint64_t, std::unique_ptr<storm::storage::BitVector>> uniqueSourceRepresentative;
                spp::sparse_hash_map<DdNode const*, std::unique_ptr<uint64_t>> blockDecodeCache;
            };

            template<typename ValueType>
            class InternalSparseQuotientExtractor<storm::dd::DdType::Sylvan, ValueType> : public InternalSparseQuotientExtractorBase<storm::dd::DdType::Sylvan, ValueType> {
            public:
                InternalSparseQuotientExtractor(storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager, std::set<storm::expressions::Variable> const& stateVariables) : InternalSparseQuotientExtractorBase<storm::dd::DdType::Sylvan, ValueType>(manager, stateVariables) {
                    // Intentionally left empty.
                }
                
                storm::storage::SparseMatrix<ValueType> extractTransitionMatrix(storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> const& transitionMatrix, Partition<storm::dd::DdType::Sylvan, ValueType> const& partition) {
                    STORM_LOG_ASSERT(partition.storedAsBdd(), "Expected partition stored as BDD.");
                    
                    // Create the number of rows necessary for the matrix.
                    this->entries.resize(partition.getNumberOfBlocks());
                    
                    storm::storage::BitVector encoding(this->stateVariablesIndicesAndLevels.size());
                    extractTransitionMatrixRec(transitionMatrix.getInternalAdd().getSylvanMtbdd().GetMTBDD(), partition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), partition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), 0, encoding);
                    
                    return this->createMatrixFromEntries(partition);
                }
                
                storm::storage::BitVector extractStates(storm::dd::Bdd<storm::dd::DdType::Sylvan> const& states, Partition<storm::dd::DdType::Sylvan, ValueType> const& partition) {
                    STORM_LOG_ASSERT(partition.storedAsBdd(), "Expected partition stored as BDD.");
                    
                    storm::storage::BitVector result(partition.getNumberOfBlocks());
                    extractStatesRec(states.getInternalBdd().getSylvanBdd().GetBDD(), partition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), 0, result);
                    
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
                
                void extractStatesRec(BDD statesNode, BDD partitionNode, uint64_t offset, storm::storage::BitVector& result) {
                    if (statesNode == sylvan_false) {
                        return;
                    }
                    
                    // Determine the levels in the DDs.
                    uint64_t statesVariable = sylvan_isconst(statesNode) ? 0xffffffff : sylvan_var(statesNode);
                    uint64_t partitionVariable = sylvan_var(partitionNode) - 1;
                    
                    // See how many variables we skipped.
                    while (offset < this->stateVariablesIndicesAndLevels.size() && statesVariable != this->stateVariablesIndicesAndLevels[offset].first && partitionVariable != this->stateVariablesIndicesAndLevels[offset].first) {
                        ++offset;
                    }
                    
                    if (offset == this->stateVariablesIndicesAndLevels.size()) {
                        result.set(decodeBlockIndex(partitionNode));
                        return;
                    }
                    
                    uint64_t topVariable;
                    if (statesVariable == this->stateVariablesIndicesAndLevels[offset].first) {
                        topVariable = statesVariable;
                    } else {
                        topVariable = partitionVariable;
                    }
                    
                    BDD tStates = statesNode;
                    BDD eStates = statesNode;
                    if (topVariable == statesVariable) {
                        tStates = sylvan_high(statesNode);
                        eStates = sylvan_low(statesNode);
                    }
                    
                    BDD tPartition = partitionNode;
                    BDD ePartition = partitionNode;
                    if (topVariable == partitionVariable) {
                        tPartition = sylvan_high(partitionNode);
                        ePartition = sylvan_low(partitionNode);
                    }
                    
                    extractStatesRec(tStates, tPartition, offset, result);
                    extractStatesRec(eStates, ePartition, offset, result);
                }
                
                void extractTransitionMatrixRec(MTBDD transitionMatrixNode, BDD sourcePartitionNode, BDD targetPartitionNode, uint64_t currentIndex, storm::storage::BitVector& sourceState) {
                    // For the empty DD, we do not need to add any entries. Note that the partition nodes cannot be zero
                    // as all states of the model have to be contained.
                    if (mtbdd_iszero(transitionMatrixNode)) {
                        return;
                    }
                    
                    // If we have moved through all source variables, we must have arrived at a constant.
                    if (currentIndex == sourceState.size()) {
                        // Decode the source block.
                        uint64_t sourceBlockIndex = decodeBlockIndex(sourcePartitionNode);
                        
                        std::unique_ptr<storm::storage::BitVector>& sourceRepresentative = uniqueSourceRepresentative[sourceBlockIndex];
                        if (sourceRepresentative && *sourceRepresentative != sourceState) {
                            // In this case, we have picked a different representative and must not record any entries now.
                            return;
                        }
                        
                        // Otherwise, we record the new representative.
                        sourceRepresentative.reset(new storm::storage::BitVector(sourceState));
                        
                        // Decode the target block.
                        uint64_t targetBlockIndex = decodeBlockIndex(targetPartitionNode);
                        
                        this->entries[sourceBlockIndex].emplace_back(targetBlockIndex, storm::dd::InternalAdd<storm::dd::DdType::Sylvan, ValueType>::getValue(transitionMatrixNode));
                    } else {
                        // Determine the levels in the DDs.
                        uint64_t transitionMatrixVariable = sylvan_isconst(transitionMatrixNode) ? 0xffffffff : sylvan_var(transitionMatrixNode);
                        uint64_t sourcePartitionVariable = sylvan_var(sourcePartitionNode) - 1;
                        uint64_t targetPartitionVariable = sylvan_var(targetPartitionNode) - 1;
                        
                        // Move through transition matrix.
                        MTBDD tt = transitionMatrixNode;
                        MTBDD te = transitionMatrixNode;
                        MTBDD et = transitionMatrixNode;
                        MTBDD ee = transitionMatrixNode;
                        if (transitionMatrixVariable == this->stateVariablesIndicesAndLevels[currentIndex].first) {
                            MTBDD t = sylvan_high(transitionMatrixNode);
                            MTBDD e = sylvan_low(transitionMatrixNode);
                            
                            uint64_t tVariable = sylvan_isconst(t) ? 0xffffffff : sylvan_var(t);
                            if (tVariable == this->stateVariablesIndicesAndLevels[currentIndex].first + 1) {
                                tt = sylvan_high(t);
                                te = sylvan_low(t);
                            } else {
                                tt = te = t;
                            }
                            
                            uint64_t eVariable = sylvan_isconst(e) ? 0xffffffff : sylvan_var(e);
                            if (eVariable == this->stateVariablesIndicesAndLevels[currentIndex].first + 1) {
                                et = sylvan_high(e);
                                ee = sylvan_low(e);
                            } else {
                                et = ee = e;
                            }
                        } else {
                            if (transitionMatrixVariable == this->stateVariablesIndicesAndLevels[currentIndex].first + 1) {
                                tt = sylvan_high(transitionMatrixNode);
                                te = sylvan_low(transitionMatrixNode);
                            } else {
                                tt = te = transitionMatrixNode;
                            }
                        }
                        
                        // Move through partition (for source state).
                        MTBDD sourceT;
                        MTBDD sourceE;
                        if (sourcePartitionVariable == this->stateVariablesIndicesAndLevels[currentIndex].first) {
                            sourceT = sylvan_high(sourcePartitionNode);
                            sourceE = sylvan_low(sourcePartitionNode);
                        } else {
                            sourceT = sourceE = sourcePartitionNode;
                        }
                        
                        // Move through partition (for target state).
                        MTBDD targetT;
                        MTBDD targetE;
                        if (targetPartitionVariable == this->stateVariablesIndicesAndLevels[currentIndex].first) {
                            targetT = sylvan_high(targetPartitionNode);
                            targetE = sylvan_low(targetPartitionNode);
                        } else {
                            targetT = targetE = targetPartitionNode;
                        }
                        
                        sourceState.set(currentIndex, true);
                        extractTransitionMatrixRec(tt, sourceT, targetT, currentIndex + 1, sourceState);
                        extractTransitionMatrixRec(te, sourceT, targetE, currentIndex + 1, sourceState);
                        
                        sourceState.set(currentIndex, false);
                        extractTransitionMatrixRec(et, sourceE, targetT, currentIndex + 1, sourceState);
                        extractTransitionMatrixRec(ee, sourceE, targetE, currentIndex + 1, sourceState);
                    }
                }
                
                spp::sparse_hash_map<uint64_t, std::unique_ptr<storm::storage::BitVector>> uniqueSourceRepresentative;
                spp::sparse_hash_map<BDD, std::unique_ptr<uint64_t>> blockDecodeCache;
            };

            template<storm::dd::DdType DdType, typename ValueType>
            QuotientExtractor<DdType, ValueType>::QuotientExtractor() : useRepresentatives(false) {
                auto const& settings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
                this->useRepresentatives = settings.isUseRepresentativesSet();
                this->quotientFormat = settings.getQuotientFormat();
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::Model<ValueType>> QuotientExtractor<DdType, ValueType>::extract(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition) {
                auto start = std::chrono::high_resolution_clock::now();
                std::shared_ptr<storm::models::Model<ValueType>> result;
                if (quotientFormat == storm::settings::modules::BisimulationSettings::QuotientFormat::Sparse) {
                    result = extractSparseQuotient(model, partition);
                } else {
                    result = extractDdQuotient(model, partition);
                }
                auto end = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Quotient extraction completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");
                
                STORM_LOG_THROW(result, storm::exceptions::NotSupportedException, "Quotient could not be extracted.");
                
                return result;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::sparse::Model<ValueType>> QuotientExtractor<DdType, ValueType>::extractSparseQuotient(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition) {
                InternalSparseQuotientExtractor<DdType, ValueType> sparseExtractor(model.getManager(), model.getRowVariables());
                storm::storage::SparseMatrix<ValueType> quotientTransitionMatrix = sparseExtractor.extractTransitionMatrix(model.getTransitionMatrix(), partition);
                
                storm::models::sparse::StateLabeling quotientStateLabeling(partition.getNumberOfBlocks());
                quotientStateLabeling.addLabel("init", sparseExtractor.extractStates(model.getInitialStates(), partition));
                quotientStateLabeling.addLabel("deadlock", sparseExtractor.extractStates(model.getDeadlockStates(), partition));
                
                for (auto const& label : partition.getPreservationInformation().getLabels()) {
                    quotientStateLabeling.addLabel(label, sparseExtractor.extractStates(model.getStates(label), partition));
                }
                for (auto const& expression : partition.getPreservationInformation().getExpressions()) {
                    std::stringstream stream;
                    stream << expression;
                    std::string expressionAsString = stream.str();
                    
                    if (quotientStateLabeling.containsLabel(expressionAsString)) {
                        STORM_LOG_WARN("Duplicate label '" << expressionAsString << "', dropping second label definition.");
                    } else {
                        quotientStateLabeling.addLabel(stream.str(), sparseExtractor.extractStates(model.getStates(expression), partition));
                    }
                }

                std::shared_ptr<storm::models::sparse::Model<ValueType>> result;
                if (model.getType() == storm::models::ModelType::Dtmc) {
                    result = std::make_shared<storm::models::sparse::Dtmc<ValueType>>(std::move(quotientTransitionMatrix), std::move(quotientStateLabeling));
                } else if (model.getType() == storm::models::ModelType::Ctmc) {
                    result = std::make_shared<storm::models::sparse::Ctmc<ValueType>>(std::move(quotientTransitionMatrix), std::move(quotientStateLabeling));
                }
                
                return result;
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> QuotientExtractor<DdType, ValueType>::extractDdQuotient(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition) {
                return extractQuotientUsingBlockVariables(model, partition);
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> QuotientExtractor<DdType, ValueType>::extractQuotientUsingBlockVariables(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition) {
                auto modelType = model.getType();
                
                if (modelType == storm::models::ModelType::Dtmc || modelType == storm::models::ModelType::Ctmc) {
                    std::set<storm::expressions::Variable> blockVariableSet = {partition.getBlockVariable()};
                    std::set<storm::expressions::Variable> blockPrimeVariableSet = {partition.getPrimedBlockVariable()};
                    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> blockMetaVariablePairs = {std::make_pair(partition.getBlockVariable(), partition.getPrimedBlockVariable())};
                    
                    storm::dd::Bdd<DdType> partitionAsBdd = partition.storedAsBdd() ? partition.asBdd() : partition.asAdd().notZero();
                    if (useRepresentatives) {
                        storm::dd::Bdd<DdType> partitionAsBddOverPrimedBlockVariable = partitionAsBdd.renameVariables(blockVariableSet, blockPrimeVariableSet);
                        storm::dd::Bdd<DdType> representativePartition = partitionAsBddOverPrimedBlockVariable.existsAbstractRepresentative(model.getColumnVariables()).renameVariables(model.getColumnVariables(), blockVariableSet);
                        partitionAsBdd = (representativePartition && partitionAsBddOverPrimedBlockVariable).existsAbstract(blockPrimeVariableSet);
                    }
                    
                    storm::dd::Add<DdType, ValueType> partitionAsAdd = partitionAsBdd.template toAdd<ValueType>();
                    auto start = std::chrono::high_resolution_clock::now();
                    storm::dd::Add<DdType, ValueType> quotientTransitionMatrix = model.getTransitionMatrix().multiplyMatrix(partitionAsAdd.renameVariables(blockVariableSet, blockPrimeVariableSet), model.getColumnVariables());
                    quotientTransitionMatrix = quotientTransitionMatrix.multiplyMatrix(partitionAsAdd.renameVariables(model.getColumnVariables(), model.getRowVariables()), model.getRowVariables());
                    auto end = std::chrono::high_resolution_clock::now();
                    STORM_LOG_TRACE("Quotient transition matrix extracted in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");
                    storm::dd::Bdd<DdType> quotientTransitionMatrixBdd = quotientTransitionMatrix.notZero();
                    
                    start = std::chrono::high_resolution_clock::now();
                    storm::dd::Bdd<DdType> partitionAsBddOverRowVariables = partitionAsBdd.renameVariables(model.getColumnVariables(), model.getRowVariables());
                    storm::dd::Bdd<DdType> reachableStates = partitionAsBdd.existsAbstract(model.getColumnVariables());
                    storm::dd::Bdd<DdType> initialStates = (model.getInitialStates() && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables());
                    storm::dd::Bdd<DdType> deadlockStates = !quotientTransitionMatrixBdd.existsAbstract(blockPrimeVariableSet) && reachableStates;
                    
                    std::map<std::string, storm::dd::Bdd<DdType>> preservedLabelBdds;
                    for (auto const& label : partition.getPreservationInformation().getLabels()) {
                        preservedLabelBdds.emplace(label, (model.getStates(label) && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables()));
                    }
                    for (auto const& expression : partition.getPreservationInformation().getExpressions()) {
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
                    end = std::chrono::high_resolution_clock::now();
                    STORM_LOG_TRACE("Quotient labels extracted in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");

                    if (modelType == storm::models::ModelType::Dtmc) {
                        return std::shared_ptr<storm::models::symbolic::Dtmc<DdType, ValueType>>(new storm::models::symbolic::Dtmc<DdType, ValueType>(model.getManager().asSharedPointer(), reachableStates, initialStates, deadlockStates, quotientTransitionMatrix, blockVariableSet, blockPrimeVariableSet, blockMetaVariablePairs, preservedLabelBdds, {}));
                    } else {
                        return std::shared_ptr<storm::models::symbolic::Ctmc<DdType, ValueType>>(new storm::models::symbolic::Ctmc<DdType, ValueType>(model.getManager().asSharedPointer(), reachableStates, initialStates, deadlockStates, quotientTransitionMatrix, blockVariableSet, blockPrimeVariableSet, blockMetaVariablePairs, preservedLabelBdds, {}));
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Cannot exctract quotient for this model type.");
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> QuotientExtractor<DdType, ValueType>::extractQuotientUsingOriginalVariables(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition) {
                auto modelType = model.getType();
                
                if (modelType == storm::models::ModelType::Dtmc || modelType == storm::models::ModelType::Ctmc) {
                    std::set<storm::expressions::Variable> blockVariableSet = {partition.getBlockVariable()};
                    std::set<storm::expressions::Variable> blockPrimeVariableSet = {partition.getPrimedBlockVariable()};
                    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> blockMetaVariablePairs = {std::make_pair(partition.getBlockVariable(), partition.getPrimedBlockVariable())};
                    
                    storm::dd::Add<DdType, ValueType> partitionAsAdd = partition.storedAsBdd() ? partition.asBdd().template toAdd<ValueType>() : partition.asAdd();
                    storm::dd::Add<DdType, ValueType> quotientTransitionMatrix = model.getTransitionMatrix().multiplyMatrix(partitionAsAdd, model.getColumnVariables());
                    quotientTransitionMatrix = quotientTransitionMatrix.renameVariables(blockVariableSet, model.getColumnVariables());
                    quotientTransitionMatrix = quotientTransitionMatrix.multiplyMatrix(partitionAsAdd, model.getRowVariables());
                    quotientTransitionMatrix = quotientTransitionMatrix.renameVariables(blockVariableSet, model.getRowVariables());
                    storm::dd::Bdd<DdType> quotientTransitionMatrixBdd = quotientTransitionMatrix.notZero();
                    
                    storm::dd::Bdd<DdType> partitionAsBdd = partition.storedAsBdd() ? partition.asBdd() : partition.asAdd().notZero();
                    storm::dd::Bdd<DdType> partitionAsBddOverRowVariables = partitionAsBdd.renameVariables(model.getColumnVariables(), model.getRowVariables());
                    storm::dd::Bdd<DdType> reachableStates = partitionAsBdd.existsAbstract(model.getColumnVariables()).renameVariables(blockVariableSet, model.getRowVariables());
                    storm::dd::Bdd<DdType> initialStates = (model.getInitialStates() && partitionAsBdd.renameVariables(model.getColumnVariables(), model.getRowVariables())).existsAbstract(model.getRowVariables()).renameVariables(blockVariableSet, model.getRowVariables());
                    storm::dd::Bdd<DdType> deadlockStates = !quotientTransitionMatrixBdd.existsAbstract(model.getColumnVariables()) && reachableStates;
                    
                    std::map<std::string, storm::dd::Bdd<DdType>> preservedLabelBdds;
                    for (auto const& label : partition.getPreservationInformation().getLabels()) {
                        preservedLabelBdds.emplace(label, (model.getStates(label) && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables()));
                    }
                    for (auto const& expression : partition.getPreservationInformation().getExpressions()) {
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
