#include "storm/storage/dd/bisimulation/QuotientExtractor.h"

#include <numeric>

#include "storm/storage/dd/DdManager.h"

#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Ctmc.h"
#include "storm/models/symbolic/Mdp.h"
#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/Mdp.h"
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
            class InternalRepresentativeComputer;

            template<storm::dd::DdType DdType, typename ValueType>
            class InternalRepresentativeComputerBase {
            public:
                InternalRepresentativeComputerBase(Partition<DdType, ValueType> const& partition, std::set<storm::expressions::Variable> const& rowVariables, std::set<storm::expressions::Variable> const& columnVariables) : partition(partition), rowVariables(rowVariables), columnVariables(columnVariables) {
                    if (partition.storedAsAdd()) {
                        ddManager = &partition.asAdd().getDdManager();
                    } else {
                        ddManager = &partition.asBdd().getDdManager();
                    }
                    internalDdManager = &ddManager->getInternalDdManager();
                    
                    // Create state variables cube.
                    this->columnVariablesCube = ddManager->getBddOne();
                    for (auto const& var : columnVariables) {
                        auto const& metaVariable = ddManager->getMetaVariable(var);
                        this->columnVariablesCube &= metaVariable.getCube();
                    }
                }
                
            protected:
                storm::dd::DdManager<DdType> const* ddManager;
                storm::dd::InternalDdManager<DdType> const* internalDdManager;

                Partition<DdType, ValueType> const& partition;
                std::set<storm::expressions::Variable> const& rowVariables;
                std::set<storm::expressions::Variable> const& columnVariables;
                storm::dd::Bdd<DdType> columnVariablesCube;
            };

            template<typename ValueType>
            class InternalRepresentativeComputer<storm::dd::DdType::CUDD, ValueType> : public InternalRepresentativeComputerBase<storm::dd::DdType::CUDD, ValueType> {
            public:
                InternalRepresentativeComputer(Partition<storm::dd::DdType::CUDD, ValueType> const& partition, std::set<storm::expressions::Variable> const& rowVariables, std::set<storm::expressions::Variable> const& columnVariables) : InternalRepresentativeComputerBase<storm::dd::DdType::CUDD, ValueType>(partition, rowVariables, columnVariables) {
                    this->ddman = this->internalDdManager->getCuddManager().getManager();
                }
                
                storm::dd::Bdd<storm::dd::DdType::CUDD> getRepresentatives() {
                    return storm::dd::Bdd<storm::dd::DdType::CUDD>(*this->ddManager, storm::dd::InternalBdd<storm::dd::DdType::CUDD>(this->internalDdManager, cudd::BDD(this->internalDdManager->getCuddManager(), this->getRepresentativesRec(this->partition.asAdd().getInternalAdd().getCuddDdNode(), this->columnVariablesCube.getInternalBdd().getCuddDdNode()))), this->rowVariables);
                }
                
            private:
                DdNodePtr getRepresentativesRec(DdNodePtr partitionNode, DdNodePtr stateVariablesCube) {
                    if (partitionNode == Cudd_ReadZero(ddman)) {
                        return Cudd_ReadLogicZero(ddman);
                    }
                    
                    // If we visited the node before, there is no block that we still need to cover.
                    if (visitedNodes.find(partitionNode) != visitedNodes.end()) {
                        return Cudd_ReadLogicZero(ddman);
                    }
                    
                    // If we hit a block variable and have not yet terminated the DFS earlier, it means we have a new representative.
                    if (Cudd_IsConstant(stateVariablesCube)) {
                        visitedNodes.emplace(partitionNode, true);
                        return Cudd_ReadOne(ddman);
                    } else {
                        bool skipped = false;
                        DdNodePtr elsePartitionNode;
                        DdNodePtr thenPartitionNode;
                        if (Cudd_NodeReadIndex(partitionNode) == Cudd_NodeReadIndex(stateVariablesCube)) {
                            elsePartitionNode = Cudd_E(partitionNode);
                            thenPartitionNode = Cudd_T(partitionNode);
                        } else {
                            elsePartitionNode = thenPartitionNode = partitionNode;
                            skipped = true;
                        }
                        
                        if (!skipped) {
                            visitedNodes.emplace(partitionNode, true);
                        }
                        
                        // Otherwise, recursively proceed with DFS.
                        DdNodePtr elseResult = getRepresentativesRec(elsePartitionNode, Cudd_T(stateVariablesCube));
                        Cudd_Ref(elseResult);

                        DdNodePtr thenResult = nullptr;
                        if (!skipped) {
                            thenResult = getRepresentativesRec(thenPartitionNode, Cudd_T(stateVariablesCube));
                            Cudd_Ref(thenResult);
                            
                            if (thenResult == elseResult) {
                                Cudd_Deref(elseResult);
                                Cudd_Deref(thenResult);
                                return elseResult;
                            } else {
                                bool complement = Cudd_IsComplement(thenResult);
                                auto result = cuddUniqueInter(ddman, Cudd_NodeReadIndex(stateVariablesCube) - 1, Cudd_Regular(thenResult), complement ? Cudd_Not(elseResult) : elseResult);
                                Cudd_Deref(elseResult);
                                Cudd_Deref(thenResult);
                                return complement ? Cudd_Not(result) : result;
                            }
                        } else {
                            DdNodePtr result;
                            if (elseResult == Cudd_ReadLogicZero(ddman)) {
                                result = elseResult;
                            } else {
                                result = Cudd_Not(cuddUniqueInter(ddman, Cudd_NodeReadIndex(stateVariablesCube) - 1, Cudd_ReadOne(ddman), Cudd_Not(elseResult)));
                            }
                            Cudd_Deref(elseResult);
                            return result;
                        }
                    }
                }
                
                ::DdManager* ddman;
                spp::sparse_hash_map<DdNode const*, bool> visitedNodes;
            };

            template<typename ValueType>
            class InternalRepresentativeComputer<storm::dd::DdType::Sylvan, ValueType> : public InternalRepresentativeComputerBase<storm::dd::DdType::Sylvan, ValueType> {
            public:
                InternalRepresentativeComputer(Partition<storm::dd::DdType::Sylvan, ValueType> const& partition, std::set<storm::expressions::Variable> const& rowVariables, std::set<storm::expressions::Variable> const& columnVariables) : InternalRepresentativeComputerBase<storm::dd::DdType::Sylvan, ValueType>(partition, rowVariables, columnVariables) {
                    // Intentionally left empty.
                }
                
                storm::dd::Bdd<storm::dd::DdType::Sylvan> getRepresentatives() {
                    return storm::dd::Bdd<storm::dd::DdType::Sylvan>(*this->ddManager, storm::dd::InternalBdd<storm::dd::DdType::Sylvan>(this->internalDdManager, sylvan::Bdd(this->getRepresentativesRec(this->partition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), this->columnVariablesCube.getInternalBdd().getSylvanBdd().GetBDD()))), this->rowVariables);
                }

            private:
                BDD getRepresentativesRec(BDD partitionNode, BDD stateVariablesCube) {
                    if (partitionNode == sylvan_false) {
                        return sylvan_false;
                    }
                    
                    // If we visited the node before, there is no block that we still need to cover.
                    if (visitedNodes.find(partitionNode) != visitedNodes.end()) {
                        return sylvan_false;
                    }
                    
                    // If we hit a block variable and have not yet terminated the DFS earlier, it means we have a new representative.
                    if (sylvan_isconst(stateVariablesCube)) {
                        visitedNodes.emplace(partitionNode, true);
                        return sylvan_true;
                    } else {
                        bool skipped = false;
                        BDD elsePartitionNode;
                        BDD thenPartitionNode;
                        if (sylvan_var(partitionNode) == sylvan_var(stateVariablesCube)) {
                            elsePartitionNode = sylvan_low(partitionNode);
                            thenPartitionNode = sylvan_high(partitionNode);
                        } else {
                            elsePartitionNode = thenPartitionNode = partitionNode;
                            skipped = true;
                        }
                        
                        if (!skipped) {
                            visitedNodes.emplace(partitionNode, true);
                        }
                        
                        // Otherwise, recursively proceed with DFS.
                        BDD elseResult = getRepresentativesRec(elsePartitionNode, sylvan_high(stateVariablesCube));
                        mtbdd_refs_push(elseResult);
                        
                        BDD thenResult;
                        if (!skipped) {
                            thenResult = getRepresentativesRec(thenPartitionNode, sylvan_high(stateVariablesCube));
                            mtbdd_refs_push(thenResult);
                            
                            if (thenResult == elseResult) {
                                mtbdd_refs_pop(2);
                                return elseResult;
                            } else {
                                auto result = sylvan_makenode(sylvan_var(stateVariablesCube) - 1, elseResult, thenResult);
                                mtbdd_refs_pop(2);
                                return result;
                            }
                        } else {
                            BDD result;
                            if (elseResult == sylvan_false) {
                                result = elseResult;
                            } else {
                                result = sylvan_makenode(sylvan_var(stateVariablesCube) - 1, elseResult, sylvan_false);
                            }
                            mtbdd_refs_pop(1);
                            return result;
                        }
                    }
                }
                
                spp::sparse_hash_map<BDD, bool> visitedNodes;
            };

            template<storm::dd::DdType DdType, typename ValueType>
            class InternalSparseQuotientExtractor;

            template<storm::dd::DdType DdType, typename ValueType>
            class InternalSparseQuotientExtractorBase {
            public:
                InternalSparseQuotientExtractorBase(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition, storm::dd::Bdd<DdType> const& representatives) : manager(model.getManager()), isNondeterministic(false), partition(partition), representatives(representatives), matrixEntriesCreated(false) {
                    // Create cubes.
                    rowVariablesCube = manager.getBddOne();
                    for (auto const& variable : model.getRowVariables()) {
                        auto const& ddMetaVariable = manager.getMetaVariable(variable);
                        rowVariablesCube &= ddMetaVariable.getCube();
                    }
                    columnVariablesCube = manager.getBddOne();
                    for (auto const& variable : model.getColumnVariables()) {
                        auto const& ddMetaVariable = manager.getMetaVariable(variable);
                        columnVariablesCube &= ddMetaVariable.getCube();
                    }
                    nondeterminismVariablesCube = manager.getBddOne();
                    for (auto const& variable : model.getNondeterminismVariables()) {
                        auto const& ddMetaVariable = manager.getMetaVariable(variable);
                        nondeterminismVariablesCube &= ddMetaVariable.getCube();
                    }
                    allSourceVariablesCube = rowVariablesCube && nondeterminismVariablesCube;
                    isNondeterministic = !nondeterminismVariablesCube.isOne();
                    
                    // Create ODDs.
                    this->odd = representatives.createOdd();
                    if (this->isNondeterministic) {
                        this->nondeterminismOdd = (model.getQualitativeTransitionMatrix().existsAbstract(model.getColumnVariables()) && this->representatives).createOdd();
                    }
                }

                storm::dd::Odd const& getOdd() const {
                    return this->odd;
                }
                
            protected:
                storm::storage::SparseMatrix<ValueType> createMatrixFromEntries() {
                    for (auto& row : matrixEntries) {
                        std::sort(row.begin(), row.end(),
                                  [] (storm::storage::MatrixEntry<uint_fast64_t, ValueType> const& a, storm::storage::MatrixEntry<uint_fast64_t, ValueType> const& b) {
                                      return a.getColumn() < b.getColumn();
                                  });
                    }
                    
                    std::vector<uint64_t> rowPermutation(matrixEntries.size());
                    std::iota(rowPermutation.begin(), rowPermutation.end(), 0ull);
                    if (this->isNondeterministic) {
                        std::sort(rowPermutation.begin(), rowPermutation.end(), [this] (uint64_t first, uint64_t second) { return this->rowToState[first] < this->rowToState[second]; } );
                    }
                    
                    uint64_t rowCounter = 0;
                    uint64_t lastState = this->isNondeterministic ? rowToState[rowPermutation.front()] : 0;
                    storm::storage::SparseMatrixBuilder<ValueType> builder(matrixEntries.size(), partition.getNumberOfBlocks(), 0, true, this->isNondeterministic);
                    if (this->isNondeterministic) {
                        builder.newRowGroup(0);
                    }
                    for (auto& rowIdx : rowPermutation) {
                        // For nondeterministic models, open a new row group.
                        if (this->isNondeterministic && rowToState[rowIdx] != lastState) {
                            builder.newRowGroup(rowCounter);
                            lastState = rowToState[rowIdx];
                        }
                        
                        auto& row = matrixEntries[rowIdx];
                        for (auto const& entry : row) {
                            builder.addNextValue(rowCounter, entry.getColumn(), entry.getValue());
                        }
                        
                        // Free storage for row.
                        row.clear();
                        row.shrink_to_fit();
                        
                        ++rowCounter;
                    }
                    
                    matrixEntries.clear();
                    matrixEntries.shrink_to_fit();
                    
                    return builder.build();
                }

                void addMatrixEntry(uint64_t row, uint64_t column, ValueType const& value) {
                    this->matrixEntries[row].emplace_back(column, value);
                }
                
                void createMatrixEntryStorage() {
                    if (matrixEntriesCreated) {
                        matrixEntries.clear();
                        if (isNondeterministic) {
                            rowToState.clear();
                        }
                    }
                    matrixEntries.resize(this->isNondeterministic ? nondeterminismOdd.getTotalOffset() : odd.getTotalOffset());
                    if (isNondeterministic) {
                        rowToState.resize(matrixEntries.size());
                    }
                }
                
                void assignRowToState(uint64_t row, uint64_t state) {
                    rowToState[row] = state;
                }

                // The manager responsible for the DDs.
                storm::dd::DdManager<DdType> const& manager;
                
                // A flag that stores whether we need to take care of nondeterminism.
                bool isNondeterministic;
                
                // Useful cubes needed in the translation.
                storm::dd::Bdd<DdType> rowVariablesCube;
                storm::dd::Bdd<DdType> columnVariablesCube;
                storm::dd::Bdd<DdType> allSourceVariablesCube;
                storm::dd::Bdd<DdType> nondeterminismVariablesCube;
                
                // Information about the state partition.
                Partition<DdType, ValueType> partition;
                storm::dd::Bdd<DdType> representatives;
                storm::dd::Odd odd;
                storm::dd::Odd nondeterminismOdd;
                
                // A flag that stores whether the underlying storage for matrix entries has been created.
                bool matrixEntriesCreated;
                
                // The entries of the quotient matrix that is built.
                std::vector<std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>> matrixEntries;
                
                // A vector storing for each row which state it belongs to.
                std::vector<uint64_t> rowToState;
            };
            
            template<typename ValueType>
            class InternalSparseQuotientExtractor<storm::dd::DdType::CUDD, ValueType> : public InternalSparseQuotientExtractorBase<storm::dd::DdType::CUDD, ValueType> {
            public:
                InternalSparseQuotientExtractor(storm::models::symbolic::Model<storm::dd::DdType::CUDD, ValueType> const& model, Partition<storm::dd::DdType::CUDD, ValueType> const& partition, storm::dd::Bdd<storm::dd::DdType::CUDD> const& representatives) : InternalSparseQuotientExtractorBase<storm::dd::DdType::CUDD, ValueType>(model, partition, representatives), ddman(this->manager.getInternalDdManager().getCuddManager().getManager()) {

                    STORM_LOG_ASSERT(this->partition.storedAsAdd(), "Expected partition to be stored as an ADD.");
                    this->createBlockToOffsetMapping();
                }
                
                storm::storage::SparseMatrix<ValueType> extractTransitionMatrix(storm::dd::Add<storm::dd::DdType::CUDD, ValueType> const& transitionMatrix) {
                    // Create the number of rows necessary for the matrix.
                    STORM_LOG_TRACE("Partition has " << this->partition.getNumberOfStates() << " states in " << this->partition.getNumberOfBlocks() << " blocks.");
                    this->createMatrixEntryStorage();
                    extractTransitionMatrixRec(transitionMatrix.getInternalAdd().getCuddDdNode(), this->isNondeterministic ? this->nondeterminismOdd : this->odd, 0, this->partition.asAdd().getInternalAdd().getCuddDdNode(), this->representatives.getInternalBdd().getCuddDdNode(), this->allSourceVariablesCube.getInternalBdd().getCuddDdNode(), this->nondeterminismVariablesCube.getInternalBdd().getCuddDdNode(), this->isNondeterministic ? &this->odd : nullptr, 0);
                    return this->createMatrixFromEntries();
                }
                
            private:
                void createBlockToOffsetMapping() {
                    this->createBlockToOffsetMappingRec(this->partition.asAdd().getInternalAdd().getCuddDdNode(), this->representatives.getInternalBdd().getCuddDdNode(), this->rowVariablesCube.getInternalBdd().getCuddDdNode(), this->odd, 0);
                    STORM_LOG_ASSERT(blockToOffset.size() == this->partition.getNumberOfBlocks(), "Mismatching block-to-offset mapping: " << blockToOffset.size() << " vs. " << this->partition.getNumberOfBlocks() << ".");
                }
                
                void createBlockToOffsetMappingRec(DdNodePtr partitionNode, DdNodePtr representativesNode, DdNodePtr variables, storm::dd::Odd const& odd, uint64_t offset) {
                    STORM_LOG_ASSERT(partitionNode != Cudd_ReadZero(ddman) || representativesNode == Cudd_ReadLogicZero(ddman), "Expected representative to be zero if the partition is zero.");
                    if (representativesNode == Cudd_ReadLogicZero(ddman)) {
                        return;
                    }
                    
                    if (Cudd_IsConstant(variables)) {
                        STORM_LOG_ASSERT(odd.isTerminalNode(), "Expected terminal node.");
                        STORM_LOG_ASSERT(blockToOffset.find(partitionNode) == blockToOffset.end(), "Duplicate entry.");
                        blockToOffset[partitionNode] = offset;
                    } else {
                        STORM_LOG_ASSERT(!odd.isTerminalNode(), "Expected non-terminal node.");
                        DdNodePtr partitionT;
                        DdNodePtr partitionE;
                        if (Cudd_NodeReadIndex(partitionNode) == Cudd_NodeReadIndex(variables) + 1) {
                            partitionT = Cudd_T(partitionNode);
                            partitionE = Cudd_E(partitionNode);
                        } else {
                            partitionT = partitionE = partitionNode;
                        }
                        
                        DdNodePtr representativesT;
                        DdNodePtr representativesE;
                        if (Cudd_NodeReadIndex(representativesNode) == Cudd_NodeReadIndex(variables)) {
                            representativesT = Cudd_T(representativesNode);
                            representativesE = Cudd_E(representativesNode);
                        } else {
                            representativesT = representativesE = representativesNode;
                        }
                        
                        if (representativesT != representativesE && Cudd_IsComplement(representativesNode)) {
                            representativesE = Cudd_Not(representativesE);
                            representativesT = Cudd_Not(representativesT);
                        }
                        
                        createBlockToOffsetMappingRec(partitionE, representativesE, Cudd_T(variables), odd.getElseSuccessor(), offset);
                        createBlockToOffsetMappingRec(partitionT, representativesT, Cudd_T(variables), odd.getThenSuccessor(), offset + odd.getElseOffset());
                    }
                }
                
                void extractTransitionMatrixRec(DdNodePtr transitionMatrixNode, storm::dd::Odd const& sourceOdd, uint64_t sourceOffset, DdNodePtr targetPartitionNode, DdNodePtr representativesNode, DdNodePtr variables, DdNodePtr nondeterminismVariables, storm::dd::Odd const* stateOdd, uint64_t stateOffset) {
                    // For the empty DD, we do not need to add any entries. Note that the partition nodes cannot be zero
                    // as all states of the model have to be contained.
                    if (transitionMatrixNode == Cudd_ReadZero(ddman) || representativesNode == Cudd_ReadLogicZero(ddman)) {
                        return;
                    }

                    // If we have moved through all source variables, we must have arrived at a target block encoding.
                    if (Cudd_IsConstant(variables)) {
                        STORM_LOG_ASSERT(Cudd_IsConstant(transitionMatrixNode), "Expected constant node.");
                        this->addMatrixEntry(sourceOffset, blockToOffset.at(targetPartitionNode), Cudd_V(transitionMatrixNode));
                        if (stateOdd) {
                            this->assignRowToState(sourceOffset, stateOffset);
                        }
                    } else {
                        // Determine whether the next variable is a nondeterminism variable.
                        bool nextVariableIsNondeterminismVariable = !Cudd_IsConstant(nondeterminismVariables) && Cudd_NodeReadIndex(nondeterminismVariables) == Cudd_NodeReadIndex(variables);
                        
                        if (nextVariableIsNondeterminismVariable) {
                            DdNodePtr t;
                            DdNodePtr e;
                            
                            // Determine whether the variable was skipped in the matrix.
                            if (Cudd_NodeReadIndex(transitionMatrixNode) == Cudd_NodeReadIndex(variables)) {
                                t = Cudd_T(transitionMatrixNode);
                                e = Cudd_E(transitionMatrixNode);
                            } else {
                                t = e = transitionMatrixNode;
                            }
                            
                            STORM_LOG_ASSERT(stateOdd, "Expected separate state ODD.");
                            extractTransitionMatrixRec(e, sourceOdd.getElseSuccessor(), sourceOffset, targetPartitionNode, representativesNode, Cudd_T(variables), Cudd_T(nondeterminismVariables), stateOdd, stateOffset);
                            extractTransitionMatrixRec(t, sourceOdd.getThenSuccessor(), sourceOffset + sourceOdd.getElseOffset(), targetPartitionNode, representativesNode, Cudd_T(variables), Cudd_T(nondeterminismVariables), stateOdd, stateOffset);
                        } else {
                            DdNodePtr t;
                            DdNodePtr tt;
                            DdNodePtr te;
                            DdNodePtr e;
                            DdNodePtr et;
                            DdNodePtr ee;
                            if (Cudd_NodeReadIndex(transitionMatrixNode) == Cudd_NodeReadIndex(variables)) {
                                // Source node was not skipped in transition matrix.
                                t = Cudd_T(transitionMatrixNode);
                                e = Cudd_E(transitionMatrixNode);
                            } else {
                                t = e = transitionMatrixNode;
                            }
                            
                            if (Cudd_NodeReadIndex(t) == Cudd_NodeReadIndex(variables) + 1) {
                                // Target node was not skipped in transition matrix.
                                tt = Cudd_T(t);
                                te = Cudd_E(t);
                            } else {
                                // Target node was skipped in transition matrix.
                                tt = te = t;
                            }
                            if (t != e) {
                                if (Cudd_NodeReadIndex(e) == Cudd_NodeReadIndex(variables) + 1) {
                                    // Target node was not skipped in transition matrix.
                                    et = Cudd_T(e);
                                    ee = Cudd_E(e);
                                } else {
                                    // Target node was skipped in transition matrix.
                                    et = ee = e;
                                }
                            } else {
                                et = tt;
                                ee = te;
                            }
                            
                            DdNodePtr targetT;
                            DdNodePtr targetE;
                            if (Cudd_NodeReadIndex(targetPartitionNode) == Cudd_NodeReadIndex(variables) + 1) {
                                // Node was not skipped in target partition.
                                targetT = Cudd_T(targetPartitionNode);
                                targetE = Cudd_E(targetPartitionNode);
                            } else {
                                // Node was skipped in target partition.
                                targetT = targetE = targetPartitionNode;
                            }
                            
                            DdNodePtr representativesT;
                            DdNodePtr representativesE;
                            if (Cudd_NodeReadIndex(representativesNode) == Cudd_NodeReadIndex(variables)) {
                                // Node was not skipped in representatives.
                                representativesT = Cudd_T(representativesNode);
                                representativesE = Cudd_E(representativesNode);
                            } else {
                                // Node was skipped in representatives.
                                representativesT = representativesE = representativesNode;
                            }
                            
                            if (representativesT != representativesE && Cudd_IsComplement(representativesNode)) {
                                representativesT = Cudd_Not(representativesT);
                                representativesE = Cudd_Not(representativesE);
                            }
                            
                            extractTransitionMatrixRec(ee, sourceOdd.getElseSuccessor(), sourceOffset, targetE, representativesE, Cudd_T(variables), nondeterminismVariables, stateOdd ? &stateOdd->getElseSuccessor() : stateOdd, stateOffset);
                            extractTransitionMatrixRec(et, sourceOdd.getElseSuccessor(), sourceOffset, targetT, representativesE, Cudd_T(variables), nondeterminismVariables, stateOdd ? &stateOdd->getElseSuccessor() : stateOdd, stateOffset);
                            extractTransitionMatrixRec(te, sourceOdd.getThenSuccessor(), sourceOffset + sourceOdd.getElseOffset(), targetE, representativesT, Cudd_T(variables), nondeterminismVariables, stateOdd ? &stateOdd->getThenSuccessor() : stateOdd, stateOffset + (stateOdd ? stateOdd->getElseOffset() : 0));
                            extractTransitionMatrixRec(tt, sourceOdd.getThenSuccessor(), sourceOffset + sourceOdd.getElseOffset(), targetT, representativesT, Cudd_T(variables), nondeterminismVariables, stateOdd ? &stateOdd->getThenSuccessor() : stateOdd, stateOffset + (stateOdd ? stateOdd->getElseOffset() : 0));
                        }
                    }
                }

                ::DdManager* ddman;
                
                // A mapping from blocks (stored in terms of a DD node) to the offset of the corresponding block.
                spp::sparse_hash_map<DdNode const*, uint64_t> blockToOffset;
            };

            template<typename ValueType>
            class InternalSparseQuotientExtractor<storm::dd::DdType::Sylvan, ValueType> : public InternalSparseQuotientExtractorBase<storm::dd::DdType::Sylvan, ValueType> {
            public:
                InternalSparseQuotientExtractor(storm::models::symbolic::Model<storm::dd::DdType::Sylvan, ValueType> const& model, Partition<storm::dd::DdType::Sylvan, ValueType> const& partition, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& representatives) : InternalSparseQuotientExtractorBase<storm::dd::DdType::Sylvan, ValueType>(model, partition, representatives) {
                    
                    STORM_LOG_ASSERT(partition.storedAsBdd(), "Expected partition stored as BDD.");
                    this->createBlockToOffsetMapping();
                }
                
                storm::storage::SparseMatrix<ValueType> extractTransitionMatrix(storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> const& transitionMatrix) {
                    // Create the number of rows necessary for the matrix.
                    this->createMatrixEntryStorage();
                    extractTransitionMatrixRec(transitionMatrix.getInternalAdd().getSylvanMtbdd().GetMTBDD(), this->isNondeterministic ? this->nondeterminismOdd : this->odd, 0, this->partition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), this->representatives.getInternalBdd().getSylvanBdd().GetBDD(), this->allSourceVariablesCube.getInternalBdd().getSylvanBdd().GetBDD(), this->nondeterminismVariablesCube.getInternalBdd().getSylvanBdd().GetBDD(), this->isNondeterministic ? &this->odd : nullptr, 0);
                    return this->createMatrixFromEntries();
                }
                
            private:
                void createBlockToOffsetMapping() {
                    this->createBlockToOffsetMappingRec(this->partition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), this->representatives.getInternalBdd().getSylvanBdd().GetBDD(), this->rowVariablesCube.getInternalBdd().getSylvanBdd().GetBDD(), this->odd, 0);
                    STORM_LOG_ASSERT(blockToOffset.size() == this->partition.getNumberOfBlocks(), "Mismatching block-to-offset mapping: " << blockToOffset.size() << " vs. " << this->partition.getNumberOfBlocks() << ".");
                }
                
                void createBlockToOffsetMappingRec(BDD partitionNode, BDD representativesNode, BDD variables, storm::dd::Odd const& odd, uint64_t offset) {
                    STORM_LOG_ASSERT(partitionNode != sylvan_false || representativesNode == sylvan_false, "Expected representative to be zero if the partition is zero.");
                    if (representativesNode == sylvan_false) {
                        return;
                    }
                    
                    if (sylvan_isconst(variables)) {
                        STORM_LOG_ASSERT(odd.isTerminalNode(), "Expected terminal node.");
                        STORM_LOG_ASSERT(blockToOffset.find(partitionNode) == blockToOffset.end(), "Duplicate entry.");
                        blockToOffset[partitionNode] = offset;
                    } else {
                        STORM_LOG_ASSERT(!odd.isTerminalNode(), "Expected non-terminal node.");
                        BDD partitionT;
                        BDD partitionE;
                        if (sylvan_var(partitionNode) == sylvan_var(variables) + 1) {
                            partitionT = sylvan_high(partitionNode);
                            partitionE = sylvan_low(partitionNode);
                        } else {
                            partitionT = partitionE = partitionNode;
                        }
                        
                        BDD representativesT;
                        BDD representativesE;
                        if (sylvan_var(representativesNode) == sylvan_var(variables)) {
                            representativesT = sylvan_high(representativesNode);
                            representativesE = sylvan_low(representativesNode);
                        } else {
                            representativesT = representativesE = representativesNode;
                        }
                        
                        createBlockToOffsetMappingRec(partitionE, representativesE, sylvan_high(variables), odd.getElseSuccessor(), offset);
                        createBlockToOffsetMappingRec(partitionT, representativesT, sylvan_high(variables), odd.getThenSuccessor(), offset + odd.getElseOffset());
                    }
                }
                
                void extractTransitionMatrixRec(MTBDD transitionMatrixNode, storm::dd::Odd const& sourceOdd, uint64_t sourceOffset, BDD targetPartitionNode, BDD representativesNode, BDD variables, BDD nondeterminismVariables, storm::dd::Odd const* stateOdd, uint64_t stateOffset) {
                    // For the empty DD, we do not need to add any entries. Note that the partition nodes cannot be zero
                    // as all states of the model have to be contained.
                    if (mtbdd_iszero(transitionMatrixNode) || representativesNode == sylvan_false) {
                        return;
                    }
                    
                    // If we have moved through all source variables, we must have arrived at a target block encoding.
                    if (sylvan_isconst(variables)) {
                        STORM_LOG_ASSERT(mtbdd_isleaf(transitionMatrixNode), "Expected constant node.");
                        this->addMatrixEntry(sourceOffset, blockToOffset.at(targetPartitionNode), storm::dd::InternalAdd<storm::dd::DdType::Sylvan, ValueType>::getValue(transitionMatrixNode));
                        if (stateOdd) {
                            this->assignRowToState(sourceOffset, stateOffset);
                        }
                    } else {
                        // Determine whether the next variable is a nondeterminism variable.
                        bool nextVariableIsNondeterminismVariable = !sylvan_isconst(nondeterminismVariables) && sylvan_var(nondeterminismVariables) == sylvan_var(variables);
                        
                        if (nextVariableIsNondeterminismVariable) {
                            MTBDD t;
                            MTBDD e;
                            
                            // Determine whether the variable was skipped in the matrix.
                            if (sylvan_var(transitionMatrixNode) == sylvan_var(variables)) {
                                t = sylvan_high(transitionMatrixNode);
                                e = sylvan_low(transitionMatrixNode);
                            } else {
                                t = e = transitionMatrixNode;
                            }
                            
                            STORM_LOG_ASSERT(stateOdd, "Expected separate state ODD.");
                            extractTransitionMatrixRec(e, sourceOdd.getElseSuccessor(), sourceOffset, targetPartitionNode, representativesNode, sylvan_high(variables), sylvan_high(nondeterminismVariables), stateOdd, stateOffset);
                            extractTransitionMatrixRec(t, sourceOdd.getThenSuccessor(), sourceOffset + sourceOdd.getElseOffset(), targetPartitionNode, representativesNode, sylvan_high(variables), sylvan_high(nondeterminismVariables), stateOdd, stateOffset);
                        } else {
                            MTBDD t;
                            MTBDD tt;
                            MTBDD te;
                            MTBDD e;
                            MTBDD et;
                            MTBDD ee;
                            if (sylvan_var(transitionMatrixNode) == sylvan_var(variables)) {
                                // Source node was not skipped in transition matrix.
                                t = sylvan_high(transitionMatrixNode);
                                e = sylvan_low(transitionMatrixNode);
                            } else {
                                t = e = transitionMatrixNode;
                            }
                            
                            if (sylvan_var(t) == sylvan_var(variables) + 1) {
                                // Target node was not skipped in transition matrix.
                                tt = sylvan_high(t);
                                te = sylvan_low(t);
                            } else {
                                // Target node was skipped in transition matrix.
                                tt = te = t;
                            }
                            if (t != e) {
                                if (sylvan_var(e) == sylvan_var(variables) + 1) {
                                    // Target node was not skipped in transition matrix.
                                    et = sylvan_high(e);
                                    ee = sylvan_low(e);
                                } else {
                                    // Target node was skipped in transition matrix.
                                    et = ee = e;
                                }
                            } else {
                                et = tt;
                                ee = te;
                            }
                            
                            BDD targetT;
                            BDD targetE;
                            if (sylvan_var(targetPartitionNode) == sylvan_var(variables) + 1) {
                                // Node was not skipped in target partition.
                                targetT = sylvan_high(targetPartitionNode);
                                targetE = sylvan_low(targetPartitionNode);
                            } else {
                                // Node was skipped in target partition.
                                targetT = targetE = targetPartitionNode;
                            }
                            
                            BDD representativesT;
                            BDD representativesE;
                            if (sylvan_var(representativesNode) == sylvan_var(variables)) {
                                // Node was not skipped in representatives.
                                representativesT = sylvan_high(representativesNode);
                                representativesE = sylvan_low(representativesNode);
                            } else {
                                // Node was skipped in representatives.
                                representativesT = representativesE = representativesNode;
                            }
                            
                            extractTransitionMatrixRec(ee, sourceOdd.getElseSuccessor(), sourceOffset, targetE, representativesE, sylvan_high(variables), nondeterminismVariables, stateOdd ? &stateOdd->getElseSuccessor() : stateOdd, stateOffset);
                            extractTransitionMatrixRec(et, sourceOdd.getElseSuccessor(), sourceOffset, targetT, representativesE, sylvan_high(variables), nondeterminismVariables, stateOdd ? &stateOdd->getElseSuccessor() : stateOdd, stateOffset);
                            extractTransitionMatrixRec(te, sourceOdd.getThenSuccessor(), sourceOffset + sourceOdd.getElseOffset(), targetE, representativesT, sylvan_high(variables), nondeterminismVariables, stateOdd ? &stateOdd->getThenSuccessor() : stateOdd, stateOffset + (stateOdd ? stateOdd->getElseOffset() : 0));
                            extractTransitionMatrixRec(tt, sourceOdd.getThenSuccessor(), sourceOffset + sourceOdd.getElseOffset(), targetT, representativesT, sylvan_high(variables), nondeterminismVariables, stateOdd ? &stateOdd->getThenSuccessor() : stateOdd, stateOffset + (stateOdd ? stateOdd->getElseOffset() : 0));
                        }
                    }
                }
                
                // A mapping from blocks (stored in terms of a DD node) to the offset of the corresponding block.
                spp::sparse_hash_map<BDD, uint64_t> blockToOffset;
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
                auto states = partition.getStates().swapVariables(model.getRowColumnMetaVariablePairs());
                
                storm::dd::Bdd<DdType> partitionAsBdd = partition.storedAsAdd() ? partition.asAdd().toBdd() : partition.asBdd();
                partitionAsBdd = partitionAsBdd.renameVariables(model.getColumnVariables(), model.getRowVariables());

                auto start = std::chrono::high_resolution_clock::now();
                // FIXME: Use partition as BDD in representative computation.
                auto representatives = InternalRepresentativeComputer<DdType, ValueType>(partition, model.getRowVariables(), model.getColumnVariables()).getRepresentatives();
                STORM_LOG_ASSERT(representatives.getNonZeroCount() == partition.getNumberOfBlocks(), "Representatives size does not match that of the partition: " << representatives.getNonZeroCount() << " vs. " << partition.getNumberOfBlocks() << ".");
                STORM_LOG_ASSERT((representatives && partitionAsBdd).existsAbstract(model.getRowVariables()) == partitionAsBdd.existsAbstract(model.getRowVariables()), "Representatives do not cover all blocks.");
                InternalSparseQuotientExtractor<DdType, ValueType> sparseExtractor(model, partition, representatives);
                storm::dd::Odd const& odd = sparseExtractor.getOdd();
                STORM_LOG_ASSERT(odd.getTotalOffset() == representatives.getNonZeroCount(), "Mismatching ODD.");
                storm::storage::SparseMatrix<ValueType> quotientTransitionMatrix = sparseExtractor.extractTransitionMatrix(model.getTransitionMatrix());
                auto end = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Quotient transition matrix extracted in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");
                
                start = std::chrono::high_resolution_clock::now();
                storm::models::sparse::StateLabeling quotientStateLabeling(partition.getNumberOfBlocks());
                quotientStateLabeling.addLabel("init", ((model.getInitialStates() && partitionAsBdd).existsAbstract(model.getRowVariables()) && partitionAsBdd && representatives).existsAbstract({partition.getBlockVariable()}).toVector(odd));
                quotientStateLabeling.addLabel("deadlock", ((model.getDeadlockStates() && partitionAsBdd).existsAbstract(model.getRowVariables()) && partitionAsBdd && representatives).existsAbstract({partition.getBlockVariable()}).toVector(odd));
                
                for (auto const& label : preservationInformation.getLabels()) {
                    quotientStateLabeling.addLabel(label, (model.getStates(label) && representatives).toVector(odd));
                }
                for (auto const& expression : preservationInformation.getExpressions()) {
                    std::stringstream stream;
                    stream << expression;
                    std::string expressionAsString = stream.str();
                    
                    if (quotientStateLabeling.containsLabel(expressionAsString)) {
                        STORM_LOG_WARN("Duplicate label '" << expressionAsString << "', dropping second label definition.");
                    } else {
                        quotientStateLabeling.addLabel(stream.str(), (model.getStates(expression) && representatives).toVector(odd));
                    }
                }
                end = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Quotient labels extracted in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");

                std::shared_ptr<storm::models::sparse::Model<ValueType>> result;
                if (model.getType() == storm::models::ModelType::Dtmc) {
                    result = std::make_shared<storm::models::sparse::Dtmc<ValueType>>(std::move(quotientTransitionMatrix), std::move(quotientStateLabeling));
                } else if (model.getType() == storm::models::ModelType::Ctmc) {
                    result = std::make_shared<storm::models::sparse::Ctmc<ValueType>>(std::move(quotientTransitionMatrix), std::move(quotientStateLabeling));
                } else if (model.getType() == storm::models::ModelType::Mdp) {
                    result = std::make_shared<storm::models::sparse::Mdp<ValueType>>(std::move(quotientTransitionMatrix), std::move(quotientStateLabeling));
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

                    start = std::chrono::high_resolution_clock::now();
                    storm::dd::Add<DdType, ValueType> quotientTransitionMatrix = model.getTransitionMatrix().multiplyMatrix(partitionAsBdd.renameVariables(blockVariableSet, blockPrimeVariableSet), model.getColumnVariables());
                    
                    // Pick a representative from each block.
                    auto representatives = InternalRepresentativeComputer<DdType, ValueType>(partition, model.getRowVariables(), model.getColumnVariables()).getRepresentatives();
                    partitionAsBdd = representatives && partitionAsBdd.renameVariables(model.getColumnVariables(), model.getRowVariables());
                    storm::dd::Add<DdType, ValueType> partitionAsAdd = partitionAsBdd.template toAdd<ValueType>();
                    
                    quotientTransitionMatrix = quotientTransitionMatrix.multiplyMatrix(partitionAsAdd, model.getRowVariables());
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
                        
            template class QuotientExtractor<storm::dd::DdType::CUDD, double>;
            
            template class QuotientExtractor<storm::dd::DdType::Sylvan, double>;
            template class QuotientExtractor<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class QuotientExtractor<storm::dd::DdType::Sylvan, storm::RationalFunction>;
            
        }
    }
}
