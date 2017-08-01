#include "storm/storage/dd/bisimulation/SignatureRefiner.h"

#include <unordered_map>
#include <boost/container/flat_map.hpp>

#include "storm/storage/dd/DdManager.h"

#include "storm/storage/dd/cudd/InternalCuddDdManager.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/NotImplementedException.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BisimulationSettings.h"

#include <sparsepp/spp.h>

#include "sylvan_cache.h"
#include "sylvan_table.h"
#include "sylvan_int.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            struct CuddPointerPairHash {
                std::size_t operator()(std::pair<DdNode const*, DdNode const*> const& pair) const {
                    std::size_t seed = std::hash<DdNode const*>()(pair.first);
                    spp::hash_combine(seed, pair.second);
                    return seed;
                }
            };
            
            struct SylvanMTBDDPairHash {
                std::size_t operator()(std::pair<MTBDD, MTBDD> const& pair) const {
                    std::size_t seed = std::hash<MTBDD>()(pair.first);
                    spp::hash_combine(seed, pair.second);
                    return seed;
                }
            };
            
            struct SylvanMTBDDPairLess {
                std::size_t operator()(std::pair<MTBDD, MTBDD> const& a, std::pair<MTBDD, MTBDD> const& b) const {
                    if (a.first < b.first) {
                        return true;
                    } else if (a.first == b.first && a.second < b.second) {
                        return true;
                    }
                    return false;
                }
            };
            
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
                InternalSignatureRefiner(storm::dd::DdManager<storm::dd::DdType::CUDD> const& manager, storm::expressions::Variable const& blockVariable, uint64_t lastStateLevel) : manager(manager), internalDdManager(manager.getInternalDdManager()), blockVariable(blockVariable), lastStateLevel(lastStateLevel), nextFreeBlockIndex(0), numberOfRefinements(0), lastNumberOfVisitedNodes(10000), signatureCache(lastNumberOfVisitedNodes), reuseBlocksCache(lastNumberOfVisitedNodes) {
                    // Intentionally left empty.
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
                    DdNodePtr result = refine(oldPartition.asAdd().getInternalAdd().getCuddDdNode(), signatureAdd.getInternalAdd().getCuddDdNode());

                    // Construct resulting ADD from the obtained node and the meta information.
                    storm::dd::InternalAdd<storm::dd::DdType::CUDD, ValueType> internalNewPartitionAdd(&internalDdManager, cudd::ADD(internalDdManager.getCuddManager(), result));
                    storm::dd::Add<storm::dd::DdType::CUDD, ValueType> newPartitionAdd(oldPartition.asAdd().getDdManager(), internalNewPartitionAdd, oldPartition.asAdd().getContainedMetaVariables());
                    
                    return newPartitionAdd;
                }
                
                DdNodePtr refine(DdNode* partitionNode, DdNode* signatureNode) {
                    ::DdManager* ddman = internalDdManager.getCuddManager().getManager();
                    
                    // If we arrived at the constant zero node, then this was an illegal state encoding (we require
                    // all states to be non-deadlock).
                    if (partitionNode == Cudd_ReadZero(ddman)) {
                        return partitionNode;
                    }
                    
                    // Check the cache whether we have seen the same node before.
                    std::unique_ptr<DdNode*>& sigCacheEntrySmartPtr = signatureCache[std::make_pair(signatureNode, partitionNode)];
                    if (sigCacheEntrySmartPtr) {
                        // If so, we return the corresponding result.
                        return *sigCacheEntrySmartPtr;
                    }
                    
                    DdNode** newEntryPtr = new DdNode*;
                    sigCacheEntrySmartPtr.reset(newEntryPtr);
                    
                    // Determine the levels in the DDs.
                    uint64_t partitionVariable = Cudd_NodeReadIndex(partitionNode) - 1;
                    uint64_t signatureVariable = Cudd_NodeReadIndex(signatureNode);
                    uint64_t partitionLevel = Cudd_ReadPerm(ddman, partitionVariable);
                    uint64_t signatureLevel = Cudd_ReadPerm(ddman, signatureVariable);
                    uint64_t topLevel = std::min(partitionLevel, signatureLevel);
                    uint64_t topVariable = topLevel == partitionLevel ? partitionVariable : signatureVariable;
                    
                    // Check whether the top variable is still within the state encoding.
                    if (topLevel <= lastStateLevel) {
                        // Determine subresults by recursive descent.
                        DdNodePtr thenResult;
                        DdNodePtr elseResult;
                        if (partitionLevel < signatureLevel) {
                            thenResult = refine(Cudd_T(partitionNode), signatureNode);
                            Cudd_Ref(thenResult);
                            elseResult = refine(Cudd_E(partitionNode), signatureNode);
                            Cudd_Ref(elseResult);
                        } else if (partitionLevel > signatureLevel) {
                            thenResult = refine(partitionNode, Cudd_T(signatureNode));
                            Cudd_Ref(thenResult);
                            elseResult = refine(partitionNode, Cudd_E(signatureNode));
                            Cudd_Ref(elseResult);
                        } else {
                            thenResult = refine(Cudd_T(partitionNode), Cudd_T(signatureNode));
                            Cudd_Ref(thenResult);
                            elseResult = refine(Cudd_E(partitionNode), Cudd_E(signatureNode));
                            Cudd_Ref(elseResult);
                        }

                        DdNode* result;
                        if (thenResult == elseResult) {
                            Cudd_Deref(thenResult);
                            Cudd_Deref(elseResult);
                            result = thenResult;
                        } else {
                            // Get the node to connect the subresults.
                            DdNode* var = Cudd_addIthVar(ddman, topVariable + 1);
                            Cudd_Ref(var);
                            result = Cudd_addIte(ddman, var, thenResult, elseResult);
                            Cudd_Ref(result);
                            Cudd_RecursiveDeref(ddman, var);
                            Cudd_Deref(thenResult);
                            Cudd_Deref(elseResult);
                        }
                        
                        // Store the result in the cache.
                        *newEntryPtr = result;
                        Cudd_Deref(result);
                        
                        return result;
                    } else {
                        
                        // If we are not within the state encoding any more, we hit the signature itself.
                        
                        // If this is the first time (in this traversal) that we encounter this signature, we check
                        // whether we can assign the old block number to it.
                        auto& reuseEntry = reuseBlocksCache[partitionNode];
                        if (!reuseEntry.isReused()) {
                            reuseEntry.setReused();
                            *newEntryPtr = partitionNode;
                            return partitionNode;
                        } else {
                            DdNode* result;
                            {
                                storm::dd::Add<storm::dd::DdType::CUDD, ValueType> blockEncoding = manager.getEncoding(blockVariable, nextFreeBlockIndex, false).template toAdd<ValueType>();
                                ++nextFreeBlockIndex;
                                result = blockEncoding.getInternalAdd().getCuddDdNode();
                                Cudd_Ref(result);
                            }
                            *newEntryPtr = result;
                            Cudd_Deref(result);
                            return result;
                        }
                    }
                }
                
                storm::dd::DdManager<storm::dd::DdType::CUDD> const& manager;
                storm::dd::InternalDdManager<storm::dd::DdType::CUDD> const& internalDdManager;
                storm::expressions::Variable const& blockVariable;
                
                // The last level that belongs to the state encoding in the DDs.
                uint64_t lastStateLevel;
                
                // The current number of blocks of the new partition.
                uint64_t nextFreeBlockIndex;
                
                // The number of completed refinements.
                uint64_t numberOfRefinements;
                
                // The number of nodes visited in the last refinement operation.
                uint64_t lastNumberOfVisitedNodes;
                
                // The cache used to identify states with identical signature.
                spp::sparse_hash_map<std::pair<DdNode const*, DdNode const*>, std::unique_ptr<DdNode*>, CuddPointerPairHash> signatureCache;
                
                // The cache used to identify which old block numbers have already been reused.
                spp::sparse_hash_map<DdNode const*, ReuseWrapper> reuseBlocksCache;
            };
            
            template<typename ValueType>
            class InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType> {
            public:
                InternalSignatureRefiner(storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager, storm::expressions::Variable const& blockVariable, uint64_t lastStateLevel) : manager(manager), internalDdManager(manager.getInternalDdManager()), blockVariable(blockVariable), numberOfBlockVariables(manager.getMetaVariable(blockVariable).getNumberOfDdVariables()), blockCube(manager.getMetaVariable(blockVariable).getCube()), lastStateLevel(lastStateLevel), nextFreeBlockIndex(0), numberOfRefinements(0), signatureCache() {
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
                    BDD result = refine(oldPartition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), signatureAdd.getInternalAdd().getSylvanMtbdd().GetMTBDD());

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
                
                BDD refine(BDD partitionNode, MTBDD signatureNode) {
                    LACE_ME;
                    
                    // If we arrived at the constant zero node, then this was an illegal state encoding (we require
                    // all states to be non-deadlock).
                    if (partitionNode == sylvan_false) {
                        return partitionNode;
                    }
                    
                    STORM_LOG_ASSERT(partitionNode != mtbdd_false, "Expected non-false node.");

//                    ++numberOfVisitedNodes;

                    // Check the cache whether we have seen the same node before.
//                    ++signatureCacheLookups;
//                    auto start = std::chrono::high_resolution_clock::now();
                    std::unique_ptr<MTBDD>& sigCacheEntrySmartPtr = signatureCache[std::make_pair(signatureNode, partitionNode)];
                    if (sigCacheEntrySmartPtr) {
//                        ++signatureCacheHits;
                        // If so, we return the corresponding result.
//                        auto end = std::chrono::high_resolution_clock::now();
//                        totalSignatureCacheLookupTime += end - start;
                        return *sigCacheEntrySmartPtr;
                    }
//                    auto end = std::chrono::high_resolution_clock::now();
//                    totalSignatureCacheLookupTime += end - start;
                    
                    MTBDD* newEntryPtr = new MTBDD;
                    sigCacheEntrySmartPtr.reset(newEntryPtr);
                    
                    sylvan_gc_test();

                    // Determine levels in the DDs.
//                    start = std::chrono::high_resolution_clock::now();
                    BDDVAR signatureVariable = sylvan_isconst(signatureNode) ? 0xffffffff : sylvan_var(signatureNode);
                    BDDVAR partitionVariable = sylvan_var(partitionNode) - 1;
                    BDDVAR topVariable = std::min(signatureVariable, partitionVariable);
//                    end = std::chrono::high_resolution_clock::now();
//                    totalLevelLookupTime += end - start;
                    
                    // Check whether the top variable is still within the state encoding.
                    if (topVariable <= lastStateLevel) {
                        // Determine subresults by recursive descent.
                        BDD thenResult;
                        BDD elseResult;
                        if (partitionVariable < signatureVariable) {
                            elseResult = refine(sylvan_low(partitionNode), signatureNode);
                            thenResult = refine(sylvan_high(partitionNode), signatureNode);
                        } else if (partitionVariable > signatureVariable) {
                            elseResult = refine(partitionNode, sylvan_low(signatureNode));
                            thenResult = refine(partitionNode, sylvan_high(signatureNode));
                        } else {
                            elseResult = refine(sylvan_low(partitionNode), sylvan_low(signatureNode));
                            thenResult = refine(sylvan_high(partitionNode), sylvan_high(signatureNode));
                        }

                        BDD result;
                        if (thenResult == elseResult) {
                            result = thenResult;
                        } else {
                            // Get the node to connect the subresults.
//                            start = std::chrono::high_resolution_clock::now();
                            result = sylvan_makenode(topVariable + 1, elseResult, thenResult);
//                            end = std::chrono::high_resolution_clock::now();
//                            totalMakeNodeTime += end - start;
                        }
                        
                        // Store the result in the cache.
//                        start = std::chrono::high_resolution_clock::now();
                        *newEntryPtr = result;
//                        end = std::chrono::high_resolution_clock::now();
//                        totalSignatureCacheStoreTime += end - start;

                        return result;
                    } else {
                        
                        // If we are not within the state encoding any more, we hit the signature itself.
                        
                        // If this is the first time (in this traversal) that we encounter this signature, we check
                        // whether we can assign the old block number to it.
//                        start = std::chrono::high_resolution_clock::now();
                        auto& reuseBlockEntry = reuseBlocksCache[partitionNode];
//                        end = std::chrono::high_resolution_clock::now();
//                        totalReuseBlocksLookupTime += end - start;
                        if (!reuseBlockEntry.isReused()) {
                            reuseBlockEntry.setReused();
                            reuseBlocksCache.emplace(partitionNode, true);
//                            start = std::chrono::high_resolution_clock::now();
                            *newEntryPtr = partitionNode;
//                            end = std::chrono::high_resolution_clock::now();
//                            totalSignatureCacheStoreTime += end - start;
                            return partitionNode;
                        } else {
//                            start = std::chrono::high_resolution_clock::now();
                            BDD result = encodeBlock(nextFreeBlockIndex++);
//                            end = std::chrono::high_resolution_clock::now();
//                            totalBlockEncodingTime += end - start;
                            
//                            start = std::chrono::high_resolution_clock::now();
                            *newEntryPtr = result;
//                            end = std::chrono::high_resolution_clock::now();
//                            totalSignatureCacheStoreTime += end - start;
                            return result;
                        }
                    }
                }
                
                storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager;
                storm::dd::InternalDdManager<storm::dd::DdType::Sylvan> const& internalDdManager;
                storm::expressions::Variable const& blockVariable;
                
                uint64_t numberOfBlockVariables;
                
                storm::dd::Bdd<storm::dd::DdType::Sylvan> blockCube;
                
                // The last level that belongs to the state encoding in the DDs.
                uint64_t lastStateLevel;
                
                // The current number of blocks of the new partition.
                uint64_t nextFreeBlockIndex;
                
                // The number of completed refinements.
                uint64_t numberOfRefinements;
                
                // The cache used to identify states with identical signature.
                spp::sparse_hash_map<std::pair<MTBDD, MTBDD>, std::unique_ptr<MTBDD>, SylvanMTBDDPairHash> signatureCache;
                
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
            SignatureRefiner<DdType, ValueType>::SignatureRefiner(storm::dd::DdManager<DdType> const& manager, storm::expressions::Variable const& blockVariable, std::set<storm::expressions::Variable> const& stateVariables) : manager(&manager), stateVariables(stateVariables) {
                uint64_t lastStateLevel = 0;
                for (auto const& stateVariable : stateVariables) {
                    lastStateLevel = std::max(lastStateLevel, manager.getMetaVariable(stateVariable).getHighestLevel());
                }
                
                internalRefiner = std::make_unique<InternalSignatureRefiner<DdType, ValueType>>(manager, blockVariable, lastStateLevel);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            SignatureRefiner<DdType, ValueType>::~SignatureRefiner() = default;
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> SignatureRefiner<DdType, ValueType>::refine(Partition<DdType, ValueType> const& oldPartition, Signature<DdType, ValueType> const& signature) {
                return internalRefiner->refine(oldPartition, signature);
            }
            
            template class SignatureRefiner<storm::dd::DdType::CUDD, double>;
            
            template class SignatureRefiner<storm::dd::DdType::Sylvan, double>;
            template class SignatureRefiner<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class SignatureRefiner<storm::dd::DdType::Sylvan, storm::RationalFunction>;
            
        }
    }
}
