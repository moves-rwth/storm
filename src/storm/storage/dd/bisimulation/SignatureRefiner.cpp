#include "storm/storage/dd/bisimulation/SignatureRefiner.h"

#include <cstdio>

#include <unordered_map>
#include <boost/container/flat_map.hpp>

#include "storm/storage/dd/DdManager.h"

#include "storm/storage/dd/cudd/InternalCuddDdManager.h"

#include "storm/storage/dd/cudd/utility.h"
#include "storm/storage/dd/sylvan/utility.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BisimulationSettings.h"

#include <sparsepp/spp.h>

#include "sylvan_cache.h"
#include "sylvan_table.h"
#include "sylvan_int.h"

// FIXME: remove
#include "storm/storage/dd/DdManager.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class InternalSignatureRefiner;
            
            class ReuseWrapper {
            public:
                ReuseWrapper() : ReuseWrapper(false) {
                    // Intentionally left empty.
                }
                
                ReuseWrapper(bool value) : value(value) {
                    // Intentionally left empty.
                }
                
                bool isReused() const {
                    return value;
                }
                
                void setReused() {
                    value = true;
                }
                
            private:
                bool value;
            };
            
            template<typename ValueType>
            class InternalSignatureRefiner<storm::dd::DdType::CUDD, ValueType> {
            public:
                InternalSignatureRefiner(storm::dd::DdManager<storm::dd::DdType::CUDD> const& manager, storm::expressions::Variable const& blockVariable, storm::dd::Bdd<storm::dd::DdType::CUDD> const& nondeterminismVariables, storm::dd::Bdd<storm::dd::DdType::CUDD> const& nonBlockVariables) : manager(manager), internalDdManager(manager.getInternalDdManager()), ddman(internalDdManager.getCuddManager().getManager()), blockVariable(blockVariable), nondeterminismVariables(nondeterminismVariables), nonBlockVariables(nonBlockVariables), nextFreeBlockIndex(0), numberOfRefinements(0), lastNumberOfVisitedNodes(10000), signatureCache(lastNumberOfVisitedNodes), reuseBlocksCache(lastNumberOfVisitedNodes) {

                    // Initialize precomputed data.
                    auto const& ddMetaVariable = manager.getMetaVariable(blockVariable);
                    blockDdVariableIndices = ddMetaVariable.getIndices();
                    
                    // Create initialized block encoding where all variables are "don't care".
                    blockEncoding = std::vector<int>(static_cast<uint64_t>(internalDdManager.getCuddManager().ReadSize()), static_cast<int>(2));
                }
                
                Partition<storm::dd::DdType::CUDD, ValueType> refine(Partition<storm::dd::DdType::CUDD, ValueType> const& oldPartition, Signature<storm::dd::DdType::CUDD, ValueType> const& signature) {
                    storm::dd::Add<storm::dd::DdType::CUDD, ValueType> newPartitionAdd = refine(oldPartition, signature.getSignatureAdd());
                    ++numberOfRefinements;
                    return oldPartition.replacePartition(newPartitionAdd, nextFreeBlockIndex);
                }
                
            private:
                storm::dd::Add<storm::dd::DdType::CUDD, ValueType> refine(Partition<storm::dd::DdType::CUDD, ValueType> const& oldPartition, storm::dd::Add<storm::dd::DdType::CUDD, ValueType> const& signatureAdd) {
                    STORM_LOG_ASSERT(oldPartition.storedAsAdd(), "Expecting partition to be stored as ADD for CUDD.");
                    
                    // Clear the caches.
                    signatureCache.clear();
                    reuseBlocksCache.clear();
                    nextFreeBlockIndex = oldPartition.getNextFreeBlockIndex();

                    // Perform the actual recursive refinement step.
                    DdNodePtr result = refine(oldPartition.asAdd().getInternalAdd().getCuddDdNode(), signatureAdd.getInternalAdd().getCuddDdNode(), nondeterminismVariables.getInternalBdd().getCuddDdNode(), nonBlockVariables.getInternalBdd().getCuddDdNode());

                    // Construct resulting ADD from the obtained node and the meta information.
                    storm::dd::InternalAdd<storm::dd::DdType::CUDD, ValueType> internalNewPartitionAdd(&internalDdManager, cudd::ADD(internalDdManager.getCuddManager(), result));
                    storm::dd::Add<storm::dd::DdType::CUDD, ValueType> newPartitionAdd(oldPartition.asAdd().getDdManager(), internalNewPartitionAdd, oldPartition.asAdd().getContainedMetaVariables());
                    
                    return newPartitionAdd;
                }
                
                DdNodePtr encodeBlock(uint64_t blockIndex) {
                    for (auto const& blockDdVariableIndex : blockDdVariableIndices) {
                        blockEncoding[blockDdVariableIndex] = blockIndex & 1 ? 1 : 0;
                        blockIndex >>= 1;
                    }
                    DdNodePtr bddEncoding = Cudd_CubeArrayToBdd(ddman, blockEncoding.data());
                    Cudd_Ref(bddEncoding);
                    DdNodePtr result = Cudd_BddToAdd(ddman, bddEncoding);
                    Cudd_Ref(result);
                    Cudd_RecursiveDeref(ddman, bddEncoding);
                    Cudd_Deref(result);
                    return result;
                }
                
                DdNodePtr refine(DdNode* partitionNode, DdNode* signatureNode, DdNode* nondeterminismVariablesNode, DdNode* nonBlockVariablesNode) {
                    // If we arrived at the constant zero node, then this was an illegal state encoding (we require
                    // all states to be non-deadlock).
                    if (partitionNode == Cudd_ReadZero(ddman)) {
                        return partitionNode;
                    }
                    
                    // Check the cache whether we have seen the same node before.
                    auto nodePair = std::make_pair(signatureNode, partitionNode);
                    auto it = signatureCache.find(nodePair);
                    if (it != signatureCache.end()) {
                        // If so, we return the corresponding result.
                        return it->second;
                    }
                    
                    // If there are no more non-block variables, we hit the signature.
                    if (Cudd_IsConstant(nonBlockVariablesNode)) {
                        // If this is the first time (in this traversal) that we encounter this signature, we check
                        // whether we can assign the old block number to it.
                        auto& reuseEntry = reuseBlocksCache[partitionNode];
                        if (!reuseEntry.isReused()) {
                            reuseEntry.setReused();
                            signatureCache[nodePair] = partitionNode;
                            return partitionNode;
                        } else {
                            DdNode* result = encodeBlock(nextFreeBlockIndex++);
                            signatureCache[nodePair] = result;
                            return result;
                        }
                    } else {
                        // If there are more variables that belong to the non-block part of the encoding, we need to recursively descend.
                        
                        bool skippedBoth = true;
                        DdNode* partitionThen;
                        DdNode* partitionElse;
                        DdNode* signatureThen;
                        DdNode* signatureElse;
                        short offset = 1;
                        while (skippedBoth && !Cudd_IsConstant(nonBlockVariablesNode)) {
                            // Remember an offset that indicates whether the top variable is a nondeterminism variable or not.
                            offset = 1;
                            if (!Cudd_IsConstant(nondeterminismVariablesNode) && Cudd_NodeReadIndex(nondeterminismVariablesNode) == Cudd_NodeReadIndex(nonBlockVariablesNode)) {
                                offset = 0;
                            }
                            
                            if (Cudd_NodeReadIndex(partitionNode) - offset == Cudd_NodeReadIndex(nonBlockVariablesNode)) {
                                partitionThen = Cudd_T(partitionNode);
                                partitionElse = Cudd_E(partitionNode);
                                skippedBoth = false;
                            } else {
                                partitionThen = partitionElse = partitionNode;
                            }
                            
                            if (Cudd_NodeReadIndex(signatureNode) == Cudd_NodeReadIndex(nonBlockVariablesNode)) {
                                signatureThen = Cudd_T(signatureNode);
                                signatureElse = Cudd_E(signatureNode);
                                skippedBoth = false;
                            } else {
                                signatureThen = signatureElse = signatureNode;
                            }
                            
                            // If both (signature and partition) skipped the next variable, we fast-forward.
                            if (skippedBoth) {
                                // If the current variable is a nondeterminism variable, we need to advance both variable sets otherwise just the non-block variables.
                                nonBlockVariablesNode = Cudd_T(nonBlockVariablesNode);
                                if (offset == 0) {
                                    nondeterminismVariablesNode = Cudd_T(nondeterminismVariablesNode);
                                }
                            }
                        }
                        
                        // If there are no more non-block variables remaining, make a recursive call to enter the base case.
                        if (Cudd_IsConstant(nonBlockVariablesNode)) {
                            return refine(partitionNode, signatureNode, nondeterminismVariablesNode, nonBlockVariablesNode);
                        }
                        
                        DdNode* thenResult = refine(partitionThen, signatureThen, offset == 0 ? Cudd_T(nondeterminismVariablesNode) : nondeterminismVariablesNode, Cudd_T(nonBlockVariablesNode));
                        Cudd_Ref(thenResult);
                        DdNode* elseResult = refine(partitionElse, signatureElse, offset == 0 ? Cudd_T(nondeterminismVariablesNode) : nondeterminismVariablesNode, Cudd_T(nonBlockVariablesNode));
                        Cudd_Ref(elseResult);

                        DdNode* result;
                        if (thenResult == elseResult) {
                            Cudd_Deref(thenResult);
                            Cudd_Deref(elseResult);
                            result = thenResult;
                        } else {
                            // Get the node to connect the subresults.
                            bool complemented = Cudd_IsComplement(thenResult);
                            result = cuddUniqueInter(ddman, Cudd_NodeReadIndex(nonBlockVariablesNode) + offset, Cudd_Regular(thenResult), complemented ? Cudd_Not(elseResult) : elseResult);
                            if (complemented) {
                                result = Cudd_Not(result);
                            }
                            Cudd_Deref(thenResult);
                            Cudd_Deref(elseResult);
                        }
                        
                        // Store the result in the cache.
                        signatureCache[nodePair] = result;
                        
                        return result;
                    }
                }
                
                storm::dd::DdManager<storm::dd::DdType::CUDD> const& manager;
                storm::dd::InternalDdManager<storm::dd::DdType::CUDD> const& internalDdManager;
                ::DdManager* ddman;
                storm::expressions::Variable const& blockVariable;
                
                // The cubes representing all non-block and all nondeterminism variables, respectively.
                storm::dd::Bdd<storm::dd::DdType::CUDD> nondeterminismVariables;
                storm::dd::Bdd<storm::dd::DdType::CUDD> nonBlockVariables;
                
                // The indices of the DD variables associated with the block variable.
                std::vector<uint64_t> blockDdVariableIndices;
                
                // A vector used for encoding block indices.
                std::vector<int> blockEncoding;
                
                // The current number of blocks of the new partition.
                uint64_t nextFreeBlockIndex;
                
                // The number of completed refinements.
                uint64_t numberOfRefinements;
                
                // The number of nodes visited in the last refinement operation.
                uint64_t lastNumberOfVisitedNodes;
                
                // The cache used to identify states with identical signature.
                spp::sparse_hash_map<std::pair<DdNode const*, DdNode const*>, DdNode*, CuddPointerPairHash> signatureCache;
                
                // The cache used to identify which old block numbers have already been reused.
                spp::sparse_hash_map<DdNode const*, ReuseWrapper> reuseBlocksCache;
            };
            
            template<typename ValueType>
            class InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType> {
            public:
                InternalSignatureRefiner(storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager, storm::expressions::Variable const& blockVariable, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nondeterminismVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nonBlockVariables) : manager(manager), internalDdManager(manager.getInternalDdManager()), blockVariable(blockVariable), nondeterminismVariables(nondeterminismVariables), nonBlockVariables(nonBlockVariables), numberOfBlockVariables(manager.getMetaVariable(blockVariable).getNumberOfDdVariables()), blockCube(manager.getMetaVariable(blockVariable).getCube()), nextFreeBlockIndex(0), numberOfRefinements(0), signatureCache() {
                    // Perform garbage collection to clean up stuff not needed anymore.
                    LACE_ME;
                    sylvan_gc();
                }
                
                Partition<storm::dd::DdType::Sylvan, ValueType> refine(Partition<storm::dd::DdType::Sylvan, ValueType> const& oldPartition, Signature<storm::dd::DdType::Sylvan, ValueType> const& signature) {
                    storm::dd::Bdd<storm::dd::DdType::Sylvan> newPartitionBdd = refine(oldPartition, signature.getSignatureAdd());
                    return oldPartition.replacePartition(newPartitionBdd, nextFreeBlockIndex);
                }
                
            private:
                storm::dd::Bdd<storm::dd::DdType::Sylvan> refine(Partition<storm::dd::DdType::Sylvan, ValueType> const& oldPartition, storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> const& signatureAdd) {
                    STORM_LOG_ASSERT(oldPartition.storedAsBdd(), "Expecting partition to be stored as BDD for Sylvan.");

                    LACE_ME;
                    
                    // Set up next refinement.
                    ++numberOfRefinements;
                    
                    // Clear the caches.
                    std::size_t oldSize = signatureCache.size();
                    signatureCache.clear();
                    signatureCache.reserve(3 * oldSize);
                    reuseBlocksCache.clear();
                    reuseBlocksCache.reserve(3 * oldPartition.getNumberOfBlocks());
                    nextFreeBlockIndex = oldPartition.getNextFreeBlockIndex();

                    // Clear performance counters.
//                    signatureCacheLookups = 0;
//                    signatureCacheHits = 0;
//                    numberOfVisitedNodes = 0;
//                    totalSignatureCacheLookupTime = std::chrono::high_resolution_clock::duration(0);
//                    totalSignatureCacheStoreTime = std::chrono::high_resolution_clock::duration(0);
//                    totalReuseBlocksLookupTime = std::chrono::high_resolution_clock::duration(0);
//                    totalLevelLookupTime = std::chrono::high_resolution_clock::duration(0);
//                    totalBlockEncodingTime = std::chrono::high_resolution_clock::duration(0);
//                    totalMakeNodeTime = std::chrono::high_resolution_clock::duration(0);
                    
                    // Perform the actual recursive refinement step.
                    BDD result = refine(oldPartition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), signatureAdd.getInternalAdd().getSylvanMtbdd().GetMTBDD(), nondeterminismVariables.getInternalBdd().getSylvanBdd().GetBDD(), nonBlockVariables.getInternalBdd().getSylvanBdd().GetBDD());

                    // Construct resulting BDD from the obtained node and the meta information.
                    storm::dd::InternalBdd<storm::dd::DdType::Sylvan> internalNewPartitionBdd(&internalDdManager, sylvan::Bdd(result));
                    storm::dd::Bdd<storm::dd::DdType::Sylvan> newPartitionBdd(oldPartition.asBdd().getDdManager(), internalNewPartitionBdd, oldPartition.asBdd().getContainedMetaVariables());
                    
//                    // Display some statistics.
//                    STORM_LOG_TRACE("Refinement visited " << numberOfVisitedNodes << " nodes.");
//                    STORM_LOG_TRACE("Current #nodes in table: " << llmsset_count_marked(nodes) << " of " << llmsset_get_size(nodes) << ", cache: " << cache_getused() << " of " << cache_getsize() << ".");
//                    STORM_LOG_TRACE("Signature cache hits: " << signatureCacheHits << ", misses: " << (signatureCacheLookups - signatureCacheHits) << ".");
//                    STORM_LOG_TRACE("Signature cache lookup time: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalSignatureCacheLookupTime).count() << "ms");
//                    STORM_LOG_TRACE("Signature cache store time: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalSignatureCacheStoreTime).count() << "ms");
//                    STORM_LOG_TRACE("Signature cache total time: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalSignatureCacheStoreTime + totalSignatureCacheLookupTime).count() << "ms");
//                    STORM_LOG_TRACE("Reuse blocks lookup time: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalReuseBlocksLookupTime).count() << "ms");
//                    STORM_LOG_TRACE("Level lookup time: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalLevelLookupTime).count() << "ms");
//                    STORM_LOG_TRACE("Block encoding time: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalBlockEncodingTime).count() << "ms");
//                    STORM_LOG_TRACE("Make node time: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalMakeNodeTime).count() << "ms");
                    
                    return newPartitionBdd;
                }
                
                BDD encodeBlock(uint64_t blockIndex) {
                    std::vector<uint8_t> e(numberOfBlockVariables);
                    for (uint64_t i = 0; i < numberOfBlockVariables; ++i) {
                        e[i] = blockIndex & 1 ? 1 : 0;
                        blockIndex >>= 1;
                    }
                    return sylvan_cube(blockCube.getInternalBdd().getSylvanBdd().GetBDD(), e.data());
                }
                
                BDD refine(BDD partitionNode, MTBDD signatureNode, BDD nondeterminismVariablesNode, BDD nonBlockVariablesNode) {
                    LACE_ME;
                    
                    // If we arrived at the constant zero node, then this was an illegal state encoding (we require
                    // all states to be non-deadlock).
                    if (partitionNode == sylvan_false) {
                        return partitionNode;
                    }
                    
                    STORM_LOG_ASSERT(partitionNode != mtbdd_false, "Expected non-false node.");

                    // Check the cache whether we have seen the same node before.
                    auto nodePair = std::make_pair(signatureNode, partitionNode);
                    auto it = signatureCache.find(nodePair);
                    if (it != signatureCache.end()) {
                        // If so, we return the corresponding result.
                        return it->second;
                    }
                    
                    sylvan_gc_test();
                    
                    // If there are no more non-block variables, we hit the signature.
                    if (sylvan_isconst(nonBlockVariablesNode)) {
                        // If this is the first time (in this traversal) that we encounter this signature, we check
                        // whether we can assign the old block number to it.

                        auto& reuseBlockEntry = reuseBlocksCache[partitionNode];
                        if (!reuseBlockEntry.isReused()) {
                            reuseBlockEntry.setReused();
                            reuseBlocksCache.emplace(partitionNode, true);
                            signatureCache[nodePair] = partitionNode;
                            return partitionNode;
                        } else {
                            BDD result = encodeBlock(nextFreeBlockIndex++);
                            signatureCache[nodePair] = result;
                            return result;
                        }
                    } else {
                        // If there are more variables that belong to the non-block part of the encoding, we need to recursively descend.
                        
                        bool skippedBoth = true;
                        BDD partitionThen;
                        BDD partitionElse;
                        MTBDD signatureThen;
                        MTBDD signatureElse;
                        short offset = 1;
                        while (skippedBoth && !sylvan_isconst(nonBlockVariablesNode)) {
                            // Remember an offset that indicates whether the top variable is a nondeterminism variable or not.
                            offset = 1;
                            if (!sylvan_isconst(nondeterminismVariablesNode) && sylvan_var(nondeterminismVariablesNode) == sylvan_var(nonBlockVariablesNode)) {
                                offset = 0;
                            }

                            if (sylvan_var(partitionNode) - offset == sylvan_var(nonBlockVariablesNode)) {
                                partitionThen = sylvan_high(partitionNode);
                                partitionElse = sylvan_low(partitionNode);
                                skippedBoth = false;
                            } else {
                                partitionThen = partitionElse = partitionNode;
                            }
                            
                            if (sylvan_var(signatureNode) == sylvan_var(nonBlockVariablesNode)) {
                                signatureThen = sylvan_high(signatureNode);
                                signatureElse = sylvan_low(signatureNode);
                                skippedBoth = false;
                            } else {
                                signatureThen = signatureElse = signatureNode;
                            }
                            
                            // If both (signature and partition) skipped the next variable, we fast-forward.
                            if (skippedBoth) {
                                // If the current variable is a nondeterminism variable, we need to advance both variable sets otherwise just the non-block variables.
                                nonBlockVariablesNode = sylvan_high(nonBlockVariablesNode);
                                if (offset == 0) {
                                    nondeterminismVariablesNode = sylvan_high(nondeterminismVariablesNode);
                                }
                            }
                        }
                        
                        // If there are no more non-block variables remaining, make a recursive call to enter the base case.
                        if (sylvan_isconst(nonBlockVariablesNode)) {
                            return refine(partitionNode, signatureNode, nondeterminismVariablesNode, nonBlockVariablesNode);
                        }
                        
                        BDD thenResult = refine(partitionThen, signatureThen, offset == 0 ? sylvan_high(nondeterminismVariablesNode) : nondeterminismVariablesNode, sylvan_high(nonBlockVariablesNode));
                        bdd_refs_push(thenResult);
                        BDD elseResult = refine(partitionElse, signatureElse, offset == 0 ? sylvan_high(nondeterminismVariablesNode) : nondeterminismVariablesNode, sylvan_high(nonBlockVariablesNode));
                        bdd_refs_push(elseResult);
                        
                        BDD result;
                        if (thenResult == elseResult) {
                            result = thenResult;
                        } else {
                            // Get the node to connect the subresults.
                            result = sylvan_makenode(sylvan_var(nonBlockVariablesNode) + offset, elseResult, thenResult);
                        }
                        
                        // Dispose of the intermediate results.
                        bdd_refs_pop(2);
                        
                        // Store the result in the cache.
                        signatureCache[nodePair] = result;
                        
                        return result;
                    }
                }
                
                storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager;
                storm::dd::InternalDdManager<storm::dd::DdType::Sylvan> const& internalDdManager;
                storm::expressions::Variable const& blockVariable;

                storm::dd::Bdd<storm::dd::DdType::Sylvan> nondeterminismVariables;
                storm::dd::Bdd<storm::dd::DdType::Sylvan> nonBlockVariables;
                
                uint64_t numberOfBlockVariables;
                
                storm::dd::Bdd<storm::dd::DdType::Sylvan> blockCube;
                
                // The current number of blocks of the new partition.
                uint64_t nextFreeBlockIndex;
                
                // The number of completed refinements.
                uint64_t numberOfRefinements;
                
                // The cache used to identify states with identical signature.
                spp::sparse_hash_map<std::pair<MTBDD, MTBDD>, MTBDD, SylvanMTBDDPairHash> signatureCache;
                
                // The cache used to identify which old block numbers have already been reused.
                spp::sparse_hash_map<MTBDD, ReuseWrapper> reuseBlocksCache;
                
                // Performance counters.
//                uint64_t signatureCacheLookups;
//                uint64_t signatureCacheHits;
//                uint64_t numberOfVisitedNodes;
//                std::chrono::high_resolution_clock::duration totalSignatureCacheLookupTime;
//                std::chrono::high_resolution_clock::duration totalSignatureCacheStoreTime;
//                std::chrono::high_resolution_clock::duration totalReuseBlocksLookupTime;
//                std::chrono::high_resolution_clock::duration totalLevelLookupTime;
//                std::chrono::high_resolution_clock::duration totalBlockEncodingTime;
//                std::chrono::high_resolution_clock::duration totalMakeNodeTime;
                
            };
            
            template<storm::dd::DdType DdType, typename ValueType>
            SignatureRefiner<DdType, ValueType>::SignatureRefiner(storm::dd::DdManager<DdType> const& manager, storm::expressions::Variable const& blockVariable, std::set<storm::expressions::Variable> const& stateVariables, std::set<storm::expressions::Variable> const& nondeterminismVariables) : manager(&manager), stateVariables(stateVariables) {
                
                storm::dd::Bdd<DdType> nonBlockVariablesCube = manager.getBddOne();
                storm::dd::Bdd<DdType> nondeterminismVariablesCube = manager.getBddOne();
                for (auto const& var : nondeterminismVariables) {
                    auto cube = manager.getMetaVariable(var).getCube();
                    nonBlockVariablesCube &= cube;
                    nondeterminismVariablesCube &= cube;
                }
                for (auto const& var : stateVariables) {
                    auto cube = manager.getMetaVariable(var).getCube();
                    nonBlockVariablesCube &= cube;
                }
                
                internalRefiner = std::make_unique<InternalSignatureRefiner<DdType, ValueType>>(manager, blockVariable, nondeterminismVariablesCube, nonBlockVariablesCube);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> SignatureRefiner<DdType, ValueType>::refine(Partition<DdType, ValueType> const& oldPartition, Signature<DdType, ValueType> const& signature) {
                Partition<DdType, ValueType> result = internalRefiner->refine(oldPartition, signature);
                return result;
            }
            
            template class SignatureRefiner<storm::dd::DdType::CUDD, double>;
            
            template class SignatureRefiner<storm::dd::DdType::Sylvan, double>;
            template class SignatureRefiner<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class SignatureRefiner<storm::dd::DdType::Sylvan, storm::RationalFunction>;
            
        }
    }
}
