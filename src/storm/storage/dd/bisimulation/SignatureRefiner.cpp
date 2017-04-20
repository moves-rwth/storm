#include "storm/storage/dd/bisimulation/SignatureRefiner.h"

#include <unordered_map>

#include "storm/storage/dd/DdManager.h"

#include "storm/storage/dd/cudd/InternalCuddDdManager.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"

#include "resources/3rdparty/sparsepp/sparsepp.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            struct CuddPointerPairHash {
                std::size_t operator()(std::pair<DdNode const*, DdNode const*> const& pair) const {
                    std::size_t seed = std::hash<DdNode const*>()(pair.first);
                    boost::hash_combine(seed, pair.second);
                    return seed;
                }
            };
            
            struct SylvanMTBDDPairHash {
                std::size_t operator()(std::pair<MTBDD, MTBDD> const& pair) const {
                    std::size_t seed = std::hash<MTBDD>()(pair.first);
                    boost::hash_combine(seed, pair.second);
                    return seed;
                }
            };
            
            template<storm::dd::DdType DdType, typename ValueType>
            class InternalSignatureRefiner;
            
            template<typename ValueType>
            class InternalSignatureRefiner<storm::dd::DdType::CUDD, ValueType> {
            public:
                InternalSignatureRefiner(storm::dd::DdManager<storm::dd::DdType::CUDD> const& manager, storm::expressions::Variable const& blockVariable, uint64_t lastStateLevel) : manager(manager), internalDdManager(manager.getInternalDdManager()), blockVariable(blockVariable), lastStateLevel(lastStateLevel), nextFreeBlockIndex(0), numberOfRefinements(0) {
                    // Intentionally left empty.
                }
                
                Partition<storm::dd::DdType::CUDD, ValueType> refine(Partition<storm::dd::DdType::CUDD, ValueType> const& oldPartition, Signature<storm::dd::DdType::CUDD, ValueType> const& signature) {
                    storm::dd::Add<storm::dd::DdType::CUDD, ValueType> newPartitionAdd = refine(oldPartition, signature.getSignatureAdd());
                    ++numberOfRefinements;
                    return oldPartition.replacePartitionAdd(newPartitionAdd, nextFreeBlockIndex);
                }
                
            private:
                storm::dd::Add<storm::dd::DdType::CUDD, ValueType> refine(Partition<storm::dd::DdType::CUDD, ValueType> const& oldPartition, storm::dd::Add<storm::dd::DdType::CUDD, ValueType> const& signatureAdd) {
                    // Clear the caches.
                    signatureCache.clear();
                    reuseBlocksCache.clear();
                    nextFreeBlockIndex = oldPartition.getNextFreeBlockIndex();
                    
                    // Perform the actual recursive refinement step.
                    DdNodePtr result = refine(oldPartition.getPartitionAdd().getInternalAdd().getCuddDdNode(), signatureAdd.getInternalAdd().getCuddDdNode());

                    // Construct resulting ADD from the obtained node and the meta information.
                    storm::dd::InternalAdd<storm::dd::DdType::CUDD, ValueType> internalNewPartitionAdd(&internalDdManager, cudd::ADD(internalDdManager.getCuddManager(), result));
                    storm::dd::Add<storm::dd::DdType::CUDD, ValueType> newPartitionAdd(oldPartition.getPartitionAdd().getDdManager(), internalNewPartitionAdd, oldPartition.getPartitionAdd().getContainedMetaVariables());
                    
                    return newPartitionAdd;
                }
                
                DdNodePtr refine(DdNode* partitionNode, DdNode* signatureNode) {
                    ::DdManager* ddman = internalDdManager.getCuddManager().getManager();
                    
                    // Check the cache whether we have seen the same node before.
                    auto sigCacheIt = signatureCache.find(std::make_pair(signatureNode, partitionNode));
                    if (sigCacheIt != signatureCache.end()) {
                        // If so, we return the corresponding result.
                        return sigCacheIt->second;
                    }
                    
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
                        
                        if (thenResult == elseResult) {
                            Cudd_Deref(thenResult);
                            Cudd_Deref(elseResult);
                            return thenResult;
                        }
                        
                        // Get the node to connect the subresults.
                        DdNode* var = Cudd_addIthVar(ddman, topVariable + 1);
                        Cudd_Ref(var);
                        DdNode* result = Cudd_addIte(ddman, var, thenResult, elseResult);
                        Cudd_Ref(result);
                        Cudd_RecursiveDeref(ddman, var);
                        Cudd_Deref(thenResult);
                        Cudd_Deref(elseResult);
                        
                        // Store the result in the cache.
                        signatureCache[std::make_pair(signatureNode, partitionNode)] = result;
                        
                        Cudd_Deref(result);
                        return result;
                    } else {
                        
                        // If we are not within the state encoding any more, we hit the signature itself.
                        
                        // If we arrived at the constant zero node, then this was an illegal state encoding (we require
                        // all states to be non-deadlock).
                        if (signatureNode == Cudd_ReadZero(ddman)) {
                            return signatureNode;
                        }
                        
                        // If this is the first time (in this traversal) that we encounter this signature, we check
                        // whether we can assign the old block number to it.
                        auto reuseCacheIt = reuseBlocksCache.find(partitionNode);
                        if (reuseCacheIt == reuseBlocksCache.end()) {
                            reuseBlocksCache.emplace(partitionNode, true);
                            signatureCache[std::make_pair(signatureNode, partitionNode)] = partitionNode;
                            return partitionNode;
                        } else {
                            DdNode* result;
                            {
                                storm::dd::Add<storm::dd::DdType::CUDD, ValueType> blockEncoding = manager.getEncoding(blockVariable, nextFreeBlockIndex, false).template toAdd<ValueType>();
                                ++nextFreeBlockIndex;
                                result = blockEncoding.getInternalAdd().getCuddDdNode();
                                Cudd_Ref(result);
                            }
                            signatureCache[std::make_pair(signatureNode, partitionNode)] = result;
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
                
                // The cache used to identify states with identical signature.
                spp::sparse_hash_map<std::pair<DdNode const*, DdNode const*>, DdNode*, CuddPointerPairHash> signatureCache;
                
                // The cache used to identify which old block numbers have already been reused.
                spp::sparse_hash_map<DdNode const*, bool> reuseBlocksCache;
            };
                        
            template<typename ValueType>
            class InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType> {
            public:
                InternalSignatureRefiner(storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager, storm::expressions::Variable const& blockVariable, uint64_t lastStateLevel) : manager(manager), internalDdManager(manager.getInternalDdManager()), blockVariable(blockVariable), lastStateLevel(lastStateLevel), nextFreeBlockIndex(0), numberOfRefinements(0) {
                    // Intentionally left empty.
                }
                
                Partition<storm::dd::DdType::Sylvan, ValueType> refine(Partition<storm::dd::DdType::Sylvan, ValueType> const& oldPartition, Signature<storm::dd::DdType::Sylvan, ValueType> const& signature) {
                    storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> newPartitionAdd = refine(oldPartition, signature.getSignatureAdd());
                    ++numberOfRefinements;
                    return oldPartition.replacePartitionAdd(newPartitionAdd, nextFreeBlockIndex);
                }
                
            private:
                storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> refine(Partition<storm::dd::DdType::Sylvan, ValueType> const& oldPartition, storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> const& signatureAdd) {
                    // Clear the caches.
                    signatureCache.clear();
                    reuseBlocksCache.clear();
                    nextFreeBlockIndex = oldPartition.getNextFreeBlockIndex();
                    
                    // Perform the actual recursive refinement step.
                    MTBDD result = refine(oldPartition.getPartitionAdd().getInternalAdd().getSylvanMtbdd().GetMTBDD(), signatureAdd.getInternalAdd().getSylvanMtbdd().GetMTBDD());
                    
                    // Construct resulting ADD from the obtained node and the meta information.
                    storm::dd::InternalAdd<storm::dd::DdType::Sylvan, ValueType> internalNewPartitionAdd(&internalDdManager, sylvan::Mtbdd(result));
                    storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> newPartitionAdd(oldPartition.getPartitionAdd().getDdManager(), internalNewPartitionAdd, oldPartition.getPartitionAdd().getContainedMetaVariables());
                    
                    return newPartitionAdd;
                }
                
                MTBDD refine(MTBDD partitionNode, MTBDD signatureNode) {
                    LACE_ME;
                    
                    // Check the cache whether we have seen the same node before.
                    auto sigCacheIt = signatureCache.find(std::make_pair(signatureNode, partitionNode));
                    if (sigCacheIt != signatureCache.end()) {
                        // If so, we return the corresponding result.
                        return sigCacheIt->second;
                    }

                    // Determine levels in the DDs.
                    BDDVAR signatureVariable = mtbdd_isleaf(signatureNode) ? 0xffffffff : sylvan_var(signatureNode);
                    BDDVAR partitionVariable = mtbdd_isleaf(signatureNode) ? 0xffffffff : sylvan_var(partitionNode) - 1;
                    BDDVAR topVariable = std::min(signatureVariable, partitionVariable);

                    // Check whether the top variable is still within the state encoding.
                    if (topVariable <= lastStateLevel) {
                        // Determine subresults by recursive descent.
                        MTBDD thenResult;
                        MTBDD elseResult;
                        if (partitionVariable < signatureVariable) {
                            thenResult = refine(sylvan_high(partitionNode), signatureNode);
                            sylvan_protect(&thenResult);
                            elseResult = refine(sylvan_low(partitionNode), signatureNode);
                            sylvan_protect(&elseResult);
                        } else if (partitionVariable > signatureVariable) {
                            thenResult = refine(partitionNode, sylvan_high(signatureNode));
                            sylvan_protect(&thenResult);
                            elseResult = refine(partitionNode, sylvan_low(signatureNode));
                            sylvan_protect(&elseResult);
                        } else {
                            thenResult = refine(sylvan_high(partitionNode), sylvan_high(signatureNode));
                            sylvan_protect(&thenResult);
                            elseResult = refine(sylvan_low(partitionNode), sylvan_low(signatureNode));
                            sylvan_protect(&elseResult);
                        }
                        
                        if (thenResult == elseResult) {
                            sylvan_unprotect(&thenResult);
                            sylvan_unprotect(&elseResult);
                            return thenResult;
                        }
                        
                        // Get the node to connect the subresults.
                        MTBDD result = sylvan_makenode(topVariable + 1, elseResult, thenResult);// mtbdd_ite(sylvan_ithvar(topVariable), thenResult, elseResult);
                        sylvan_protect(&result);
                        sylvan_unprotect(&thenResult);
                        sylvan_unprotect(&elseResult);
                        
                        // Store the result in the cache.
                        signatureCache[std::make_pair(signatureNode, partitionNode)] = result;

                        sylvan_unprotect(&result);
                        return result;
                    } else {
                        
                        // If we are not within the state encoding any more, we hit the signature itself.

                        // If we arrived at the constant zero node, then this was an illegal state encoding (we require
                        // all states to be non-deadlock).
                        if (mtbdd_iszero(signatureNode)) {
                            return signatureNode;
                        }
                        
                        // If this is the first time (in this traversal) that we encounter this signature, we check
                        // whether we can assign the old block number to it.
                        auto reuseCacheIt = reuseBlocksCache.find(partitionNode);
                        if (reuseCacheIt == reuseBlocksCache.end()) {
                            reuseBlocksCache.emplace(partitionNode, true);
                            signatureCache[std::make_pair(signatureNode, partitionNode)] = partitionNode;
                            return partitionNode;
                        } else {
                            MTBDD result;
                            {
                                storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> blockEncoding = manager.getEncoding(blockVariable, nextFreeBlockIndex, false).template toAdd<ValueType>();
                                ++nextFreeBlockIndex;
                                result = blockEncoding.getInternalAdd().getSylvanMtbdd().GetMTBDD();
                                sylvan_protect(&result);
                            }
                            signatureCache[std::make_pair(signatureNode, partitionNode)] = result;
                            sylvan_unprotect(&result);
                            return result;
                        }
                    }
                }
                
                storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager;
                storm::dd::InternalDdManager<storm::dd::DdType::Sylvan> const& internalDdManager;
                storm::expressions::Variable const& blockVariable;
                
                // The last level that belongs to the state encoding in the DDs.
                uint64_t lastStateLevel;
                
                // The current number of blocks of the new partition.
                uint64_t nextFreeBlockIndex;
                
                // The number of completed refinements.
                uint64_t numberOfRefinements;
                
                // The cache used to identify states with identical signature.
                spp::sparse_hash_map<std::pair<MTBDD, MTBDD>, MTBDD, SylvanMTBDDPairHash> signatureCache;
                
                // The cache used to identify which old block numbers have already been reused.
                spp::sparse_hash_map<MTBDD, bool> reuseBlocksCache;
            };
            
            template<storm::dd::DdType DdType, typename ValueType>
            SignatureRefiner<DdType, ValueType>::SignatureRefiner(storm::dd::DdManager<DdType> const& manager, storm::expressions::Variable const& blockVariable, std::set<storm::expressions::Variable> const& stateVariables) : manager(manager), stateVariables(stateVariables) {
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
