#include "storm/storage/dd/bisimulation/InternalSylvanSignatureRefiner.h"

#include "storm/storage/dd/DdManager.h"

#include "storm/storage/dd/bisimulation/Partition.h"
#include "storm/storage/dd/bisimulation/Signature.h"

#include "sylvan_cache.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            InternalSylvanSignatureRefinerBase::InternalSylvanSignatureRefinerBase(storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager, storm::expressions::Variable const& blockVariable, std::set<storm::expressions::Variable> const& stateVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nondeterminismVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nonBlockVariables, InternalSignatureRefinerOptions const& options) : manager(manager), blockVariable(blockVariable), stateVariables(stateVariables), nondeterminismVariables(nondeterminismVariables), nonBlockVariables(nonBlockVariables), options(options), numberOfBlockVariables(manager.getMetaVariable(blockVariable).getNumberOfDdVariables()), blockCube(manager.getMetaVariable(blockVariable).getCube()), nextFreeBlockIndex(0), numberOfRefinements(0), signatureCache(), resize(0) {
                
                // Perform garbage collection to clean up stuff not needed anymore.
                LACE_ME;
                sylvan_gc();
                
                table.resize(3 * 8 * (1ull << 14));
            }
            
            BDD InternalSylvanSignatureRefinerBase::encodeBlock(uint64_t blockIndex) {
                std::vector<uint8_t> e(numberOfBlockVariables);
                for (uint64_t i = 0; i < numberOfBlockVariables; ++i) {
                    e[i] = blockIndex & 1 ? 1 : 0;
                    blockIndex >>= 1;
                }
                return sylvan_cube(blockCube.getInternalBdd().getSylvanBdd().GetBDD(), e.data());
            }
            
            
            template<typename ValueType>
            InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType>::InternalSignatureRefiner(storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager, storm::expressions::Variable const& blockVariable, std::set<storm::expressions::Variable> const& stateVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nondeterminismVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nonBlockVariables, InternalSignatureRefinerOptions const& options) : storm::dd::bisimulation::InternalSylvanSignatureRefinerBase(manager, blockVariable, stateVariables, nondeterminismVariables, nonBlockVariables, options) {
                
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            Partition<storm::dd::DdType::Sylvan, ValueType> InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType>::refine(Partition<storm::dd::DdType::Sylvan, ValueType> const& oldPartition, Signature<storm::dd::DdType::Sylvan, ValueType> const& signature) {
                std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, boost::optional<storm::dd::Bdd<storm::dd::DdType::Sylvan>>> newPartitionDds = refine(oldPartition, signature.getSignatureAdd());
                ++numberOfRefinements;
                return oldPartition.replacePartition(newPartitionDds.first, nextFreeBlockIndex, nextFreeBlockIndex, newPartitionDds.second);
            }
            
            template<typename ValueType>
            void InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType>::clearCaches() {
                signatureCache.clear();
                reuseBlocksCache.clear();
                this->table = std::vector<uint64_t>(table.size());
                this->signatures = std::vector<uint64_t>(signatures.size());
            }
            
            template<typename ValueType>
            std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, boost::optional<storm::dd::Bdd<storm::dd::DdType::Sylvan>>> InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType>::refine(Partition<storm::dd::DdType::Sylvan, ValueType> const& oldPartition, storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> const& signatureAdd) {
                STORM_LOG_ASSERT(oldPartition.storedAsBdd(), "Expecting partition to be stored as BDD for Sylvan.");
                
                nextFreeBlockIndex = options.reuseBlockNumbers ? oldPartition.getNextFreeBlockIndex() : 0;
                signatures.resize(nextFreeBlockIndex);
                
                // Perform the actual recursive refinement step.
                std::pair<BDD, BDD> result;
                if (options.parallel) {
                    STORM_LOG_TRACE("Using parallel refine.");
                    result = refineParallel(oldPartition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), signatureAdd.getInternalAdd().getSylvanMtbdd().GetMTBDD(), nondeterminismVariables.getInternalBdd().getSylvanBdd().GetBDD(), nonBlockVariables.getInternalBdd().getSylvanBdd().GetBDD());
                } else {
                    STORM_LOG_TRACE("Using sequential refine.");
                    result = refineSequential(oldPartition.asBdd().getInternalBdd().getSylvanBdd().GetBDD(), signatureAdd.getInternalAdd().getSylvanMtbdd().GetMTBDD(), nondeterminismVariables.getInternalBdd().getSylvanBdd().GetBDD(), nonBlockVariables.getInternalBdd().getSylvanBdd().GetBDD());
                }
                
                // Construct resulting BDD from the obtained node and the meta information.
                storm::dd::InternalBdd<storm::dd::DdType::Sylvan> internalNewPartitionBdd(&manager.getInternalDdManager(), sylvan::Bdd(result.first));
                storm::dd::Bdd<storm::dd::DdType::Sylvan> newPartitionBdd(oldPartition.asBdd().getDdManager(), internalNewPartitionBdd, oldPartition.asBdd().getContainedMetaVariables());
                
                boost::optional<storm::dd::Bdd<storm::dd::DdType::Sylvan>> optionalChangedBdd;
                if (options.createChangedStates && result.second != 0) {
                    storm::dd::InternalBdd<storm::dd::DdType::Sylvan> internalChangedBdd(&manager.getInternalDdManager(), sylvan::Bdd(result.second));
                    storm::dd::Bdd<storm::dd::DdType::Sylvan> changedBdd(oldPartition.asBdd().getDdManager(), internalChangedBdd, stateVariables);
                    optionalChangedBdd = changedBdd;
                }
                
                clearCaches();
                return std::make_pair(newPartitionBdd, optionalChangedBdd);
            }
            
            template<typename ValueType>
            std::pair<BDD, BDD> InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType>::reuseOrRelabel(BDD partitionNode, BDD nondeterminismVariablesNode, BDD nonBlockVariablesNode) {
                LACE_ME;
                
                if (partitionNode == sylvan_false) {
                    if (options.createChangedStates) {
                        return std::make_pair(sylvan_false, sylvan_false);
                    } else {
                        return std::make_pair(sylvan_false, 0);
                    }
                }
                
                // Check the cache whether we have seen the same node before.
                auto nodePair = std::make_pair(0, partitionNode);
                auto it = signatureCache.find(nodePair);
                if (it != signatureCache.end()) {
                    // If so, we return the corresponding result.
                    return it->second;
                }
                
                sylvan_gc_test();
                
                // If there are no more non-block variables, we hit the signature.
                if (sylvan_isconst(nonBlockVariablesNode)) {
                    if (options.reuseBlockNumbers) {
                        // If this is the first time (in this traversal) that we encounter this signature, we check
                        // whether we can assign the old block number to it.
                        
                        auto& reuseBlockEntry = reuseBlocksCache[partitionNode];
                        if (!reuseBlockEntry.isReused()) {
                            reuseBlockEntry.setReused();
                            std::pair<BDD, BDD> result;
                            if (options.createChangedStates) {
                                result = std::make_pair(partitionNode, sylvan_false);
                            } else {
                                result = std::make_pair(partitionNode, 0);
                            }
                            signatureCache[nodePair] = result;
                            return result;
                        }
                    }
                    
                    std::pair<BDD, BDD> result;
                    if (options.createChangedStates) {
                        result = std::make_pair(encodeBlock(nextFreeBlockIndex++), sylvan_true);
                    } else {
                        result = std::make_pair(encodeBlock(nextFreeBlockIndex++), 0);
                    }
                    signatureCache[nodePair] = result;
                    return result;
                } else {
                    // If there are more variables that belong to the non-block part of the encoding, we need to recursively descend.
                    
                    bool skipped = true;
                    BDD partitionThen;
                    BDD partitionElse;
                    short offset;
                    bool isNondeterminismVariable = false;
                    while (skipped && !sylvan_isconst(nonBlockVariablesNode)) {
                        // Remember an offset that indicates whether the top variable is a nondeterminism variable or not.
                        offset = options.shiftStateVariables ? 1 : 0;
                        if (!sylvan_isconst(nondeterminismVariablesNode) && sylvan_var(nondeterminismVariablesNode) == sylvan_var(nonBlockVariablesNode)) {
                            offset = 0;
                            isNondeterminismVariable = true;
                        }
                        
                        if (sylvan_mtbdd_matches_variable_index(partitionNode, sylvan_var(nonBlockVariablesNode), -offset)) {
                            partitionThen = sylvan_high(partitionNode);
                            partitionElse = sylvan_low(partitionNode);
                            skipped = false;
                        } else {
                            partitionThen = partitionElse = partitionNode;
                        }
                        
                        // If we skipped the next variable, we fast-forward.
                        if (skipped) {
                            // If the current variable is a nondeterminism variable, we need to advance both variable sets otherwise just the non-block variables.
                            nonBlockVariablesNode = sylvan_high(nonBlockVariablesNode);
                            if (isNondeterminismVariable) {
                                nondeterminismVariablesNode = sylvan_high(nondeterminismVariablesNode);
                            }
                        }
                    }
                    
                    // If there are no more non-block variables remaining, make a recursive call to enter the base case.
                    if (sylvan_isconst(nonBlockVariablesNode)) {
                        return reuseOrRelabel(partitionNode, nondeterminismVariablesNode, nonBlockVariablesNode);
                    }
                    
                    std::pair<BDD, BDD> combinedThenResult = reuseOrRelabel(partitionThen, isNondeterminismVariable ? sylvan_high(nondeterminismVariablesNode) : nondeterminismVariablesNode, sylvan_high(nonBlockVariablesNode));
                    BDD thenResult = combinedThenResult.first;
                    bdd_refs_push(thenResult);
                    BDD changedThenResult = combinedThenResult.second;
                    if (options.createChangedStates) {
                        bdd_refs_push(changedThenResult);
                    }
                    std::pair<BDD, BDD> combinedElseResult = reuseOrRelabel(partitionElse, isNondeterminismVariable ? sylvan_high(nondeterminismVariablesNode) : nondeterminismVariablesNode, sylvan_high(nonBlockVariablesNode));
                    BDD elseResult = combinedElseResult.first;
                    bdd_refs_push(elseResult);
                    BDD changedElseResult = combinedElseResult.second;
                    if (options.createChangedStates) {
                        bdd_refs_push(changedElseResult);
                    }
                    
                    std::pair<BDD, BDD> result;
                    if (thenResult == elseResult) {
                        result.first = thenResult;
                    } else {
                        // Get the node to connect the subresults.
                        result.first = sylvan_makenode(sylvan_var(nonBlockVariablesNode) + offset, elseResult, thenResult);
                    }
                    
                    result.second = 0;
                    if (options.createChangedStates) {
                        if (changedThenResult == changedElseResult) {
                            result.second = changedThenResult;
                        } else {
                            // Get the node to connect the subresults.
                            result.second = sylvan_makenode(sylvan_var(nonBlockVariablesNode) + offset, changedElseResult, changedThenResult);
                        }
                    }
                    
                    // Dispose of the intermediate results.
                    if (options.createChangedStates) {
                        bdd_refs_pop(4);
                    } else {
                        bdd_refs_pop(2);
                    }
                    
                    // Store the result in the cache.
                    signatureCache[nodePair] = result;
                    return result;
                }
            }
            
            template<typename ValueType>
            std::pair<BDD, BDD> InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType>::refineParallel(BDD partitionNode, MTBDD signatureNode, BDD nondeterminismVariablesNode, BDD nonBlockVariablesNode) {
                LACE_ME;
                // return std::make_pair(CALL(refine_parallel, partitionNode, signatureNode, nondeterminismVariablesNode, nonBlockVariablesNode, this), 0);
                return std::make_pair(CALL(sylvan_refine_partition, signatureNode, nonBlockVariablesNode, partitionNode, this), 0);
            }
            
            template<typename ValueType>
            std::pair<BDD, BDD> InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType>::refineSequential(BDD partitionNode, MTBDD signatureNode, BDD nondeterminismVariablesNode, BDD nonBlockVariablesNode) {
                LACE_ME;
                
                // If we arrived at the constant zero node, then this was an illegal state encoding (we require
                // all states to be non-deadlock).
                if (partitionNode == sylvan_false) {
                    if (options.createChangedStates) {
                        return std::make_pair(sylvan_false, sylvan_false);
                    } else {
                        return std::make_pair(sylvan_false, 0);
                    }
                } else if (mtbdd_iszero(signatureNode)) {
                    std::pair<BDD, BDD> result = reuseOrRelabel(partitionNode, nondeterminismVariablesNode, nonBlockVariablesNode);
                    signatureCache[std::make_pair(signatureNode, partitionNode)] = result;
                    return result;
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
                    if (options.reuseBlockNumbers) {
                        // If this is the first time (in this traversal) that we encounter this signature, we check
                        // whether we can assign the old block number to it.
                        
                        auto& reuseBlockEntry = reuseBlocksCache[partitionNode];
                        if (!reuseBlockEntry.isReused()) {
                            reuseBlockEntry.setReused();
                            std::pair<BDD, BDD> result;
                            if (options.createChangedStates) {
                                result = std::make_pair(partitionNode, sylvan_false);
                            } else {
                                result = std::make_pair(partitionNode, 0);
                            }
                            signatureCache[nodePair] = result;
                            return result;
                        }
                    }
                    
                    std::pair<BDD, BDD> result;
                    if (options.createChangedStates) {
                        result = std::make_pair(encodeBlock(nextFreeBlockIndex++), sylvan_true);
                    } else {
                        result = std::make_pair(encodeBlock(nextFreeBlockIndex++), 0);
                    }
                    signatureCache[nodePair] = result;
                    return result;
                } else {
                    // If there are more variables that belong to the non-block part of the encoding, we need to recursively descend.
                    
                    bool skippedBoth = true;
                    BDD partitionThen;
                    BDD partitionElse;
                    MTBDD signatureThen;
                    MTBDD signatureElse;
                    short offset;
                    bool isNondeterminismVariable = false;
                    while (skippedBoth && !sylvan_isconst(nonBlockVariablesNode)) {
                        // Remember an offset that indicates whether the top variable is a nondeterminism variable or not.
                        offset = options.shiftStateVariables ? 1 : 0;
                        if (!sylvan_isconst(nondeterminismVariablesNode) && sylvan_var(nondeterminismVariablesNode) == sylvan_var(nonBlockVariablesNode)) {
                            offset = 0;
                            isNondeterminismVariable = true;
                        }
                        
                        if (sylvan_mtbdd_matches_variable_index(partitionNode, sylvan_var(nonBlockVariablesNode), -offset)) {
                            partitionThen = sylvan_high(partitionNode);
                            partitionElse = sylvan_low(partitionNode);
                            skippedBoth = false;
                        } else {
                            partitionThen = partitionElse = partitionNode;
                        }
                        
                        if (sylvan_mtbdd_matches_variable_index(signatureNode, sylvan_var(nonBlockVariablesNode))) {
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
                            if (isNondeterminismVariable) {
                                nondeterminismVariablesNode = sylvan_high(nondeterminismVariablesNode);
                            }
                        }
                    }
                    
                    // If there are no more non-block variables remaining, make a recursive call to enter the base case.
                    if (sylvan_isconst(nonBlockVariablesNode)) {
                        return refineSequential(partitionNode, signatureNode, nondeterminismVariablesNode, nonBlockVariablesNode);
                    }
                    
                    std::pair<BDD, BDD> combinedThenResult = refineSequential(partitionThen, signatureThen, isNondeterminismVariable ? sylvan_high(nondeterminismVariablesNode) : nondeterminismVariablesNode, sylvan_high(nonBlockVariablesNode));
                    BDD thenResult = combinedThenResult.first;
                    bdd_refs_push(thenResult);
                    BDD changedThenResult = combinedThenResult.second;
                    if (options.createChangedStates) {
                        bdd_refs_push(changedThenResult);
                    }
                    std::pair<BDD, BDD> combinedElseResult = refineSequential(partitionElse, signatureElse, isNondeterminismVariable ? sylvan_high(nondeterminismVariablesNode) : nondeterminismVariablesNode, sylvan_high(nonBlockVariablesNode));
                    BDD elseResult = combinedElseResult.first;
                    bdd_refs_push(elseResult);
                    BDD changedElseResult = combinedElseResult.second;
                    if (options.createChangedStates) {
                        bdd_refs_push(changedElseResult);
                    }
                    
                    std::pair<BDD, BDD> result;
                    if (thenResult == elseResult) {
                        result.first = thenResult;
                    } else {
                        // Get the node to connect the subresults.
                        result.first = sylvan_makenode(sylvan_var(nonBlockVariablesNode) + offset, elseResult, thenResult);
                    }
                    
                    result.second = 0;
                    if (options.createChangedStates) {
                        if (changedThenResult == changedElseResult) {
                            result.second = changedThenResult;
                        } else {
                            // Get the node to connect the subresults.
                            result.second = sylvan_makenode(sylvan_var(nonBlockVariablesNode) + offset, changedElseResult, changedThenResult);
                        }
                    }
                    
                    // Dispose of the intermediate results.
                    if (options.createChangedStates) {
                        bdd_refs_pop(4);
                    } else {
                        bdd_refs_pop(2);
                    }
                    
                    // Store the result in the cache.
                    signatureCache[nodePair] = result;
                    
                    return result;
                }
            }
            
#define cas(ptr, old, new) (__sync_bool_compare_and_swap((ptr),(old),(new)))
#define ATOMIC_READ(x) (*(volatile decltype(x) *)&(x))
            
            /* Rotating 64-bit FNV-1a hash */
            static uint64_t
            sylvan_hash(uint64_t a, uint64_t b)
            {
                const uint64_t prime = 1099511628211;
                uint64_t hash = 14695981039346656037LLU;
                hash = (hash ^ (a>>32));
                hash = (hash ^ a) * prime;
                hash = (hash ^ b) * prime;
                return hash ^ (hash>>32);
            }
            
            VOID_TASK_3(sylvan_rehash, size_t, first, size_t, count, void*, refinerPtr)
            {
                auto& refiner = *static_cast<InternalSylvanSignatureRefinerBase*>(refinerPtr);
                
                if (count > 128) {
                    SPAWN(sylvan_rehash, first, count/2, refinerPtr);
                    CALL(sylvan_rehash, first+count/2, count-count/2, refinerPtr);
                    SYNC(sylvan_rehash);
                    return;
                }
                
                while (count--) {
                    uint64_t *old_ptr = refiner.oldTable.data() + first*3;
                    uint64_t a = old_ptr[0];
                    uint64_t b = old_ptr[1];
                    uint64_t c = old_ptr[2];
                    
                    uint64_t hash = sylvan_hash(a, b);
                    uint64_t pos = hash % (refiner.table.size() / 3);
                    
                    volatile uint64_t *ptr = 0;
                    for (;;) {
                        ptr = refiner.table.data() + pos*3;
                        if (*ptr == 0) {
                            if (cas(ptr, 0, a)) {
                                ptr[1] = b;
                                ptr[2] = c;
                                break;
                            }
                        }
                        pos++;
                        if (pos >= (refiner.table.size() / 3)) pos = 0;
                    }
                    
                    first++;
                }
            }
            
            VOID_TASK_1(sylvan_grow_it, void*, refinerPtr)
            {
                auto& refiner = *static_cast<InternalSylvanSignatureRefinerBase*>(refinerPtr);
                
                refiner.oldTable = std::move(refiner.table);
                
                refiner.table = std::vector<uint64_t>(refiner.oldTable.size() << 1);
                
                CALL(sylvan_rehash, 0, refiner.oldTable.size() / 3, refinerPtr);
                
                refiner.oldTable.clear();
            }
            
            VOID_TASK_1(sylvan_grow, void*, refinerPtr)
            {
                auto& refiner = *static_cast<InternalSylvanSignatureRefinerBase*>(refinerPtr);
                
                if (cas(&refiner.resize, 0, 1)) {
                    NEWFRAME(sylvan_grow_it, refinerPtr);
                    refiner.resize = 0;
                } else {
                    /* wait for new frame to appear */
                    while (ATOMIC_READ(lace_newframe.t) == 0) {}
                    lace_yield(__lace_worker, __lace_dq_head);
                }
            }
            
            static uint64_t
            sylvan_search_or_insert(uint64_t sig, uint64_t previous_block, void* refinerPtr)
            {
                auto& refiner = *static_cast<InternalSylvanSignatureRefinerBase*>(refinerPtr);
                
                uint64_t hash = sylvan_hash(sig, previous_block);
                uint64_t pos = hash % (refiner.table.size() / 3);
                
                volatile uint64_t *ptr = 0;
                uint64_t a, b, c;
                int count = 0;
                for (;;) {
                    ptr = refiner.table.data() + pos*3;
                    a = *ptr;
                    if (a == sig) {
                        while ((b=ptr[1]) == 0) continue;
                        if (b == previous_block) {
                            while ((c=ptr[2]) == 0) continue;
                            return c;
                        }
                    } else if (a == 0) {
                        if (cas(ptr, 0, sig)) {
                            c = ptr[2] = __sync_fetch_and_add(&refiner.nextFreeBlockIndex, 1);
                            ptr[1] = previous_block;
                            return c;
                        } else {
                            continue;
                        }
                    }
                    pos++;
                    if (pos >= (refiner.table.size() / 3)) pos = 0;
                    if (++count >= 128) return 0;
                }
            }
            
            TASK_1(uint64_t, sylvan_decode_block, BDD, block)
            {
                uint64_t result = 0;
                uint64_t mask = 1;
                while (block != sylvan_true) {
                    BDD b_low = sylvan_low(block);
                    if (b_low == sylvan_false) {
                        result |= mask;
                        block = sylvan_high(block);
                    } else {
                        block = b_low;
                    }
                    mask <<= 1;
                }
                return result;
            }
            
            TASK_3(BDD, sylvan_assign_block, BDD, sig, BDD, previous_block, void*, refinerPtr)
            {
                assert(previous_block != mtbdd_false); // if so, incorrect call!
                
                auto& refiner = *static_cast<InternalSylvanSignatureRefinerBase*>(refinerPtr);
                
                // maybe do garbage collection
                sylvan_gc_test();
                
                if (sig == sylvan_false) {
                    // slightly different handling because sylvan_false == 0
                    sig = (uint64_t)-1;
                }
                
                // try to claim previous block number
                const uint64_t p_b = CALL(sylvan_decode_block, previous_block);
                assert(p_b != 0);
                
                for (;;) {
                    BDD cur = *(volatile BDD*)&refiner.signatures[p_b];
                    if (cur == sig) return previous_block;
                    if (cur != 0) break;
                    if (cas(&refiner.signatures[p_b], 0, sig)) return previous_block;
                }
                
                // no previous block number, search or insert
                uint64_t c;
                while ((c = sylvan_search_or_insert(sig, previous_block, refinerPtr)) == 0) CALL(sylvan_grow, refinerPtr);
                
                //                if (c >= refiner.signatures.size()) {
                //                    fprintf(stderr, "Out of cheese exception, no more blocks available\n");
                //                    exit(1);
                //                }
                
                //                return CALL(refiner.encodeBlock(c));
                return refiner.encodeBlock(c);
            }
            
            TASK_4(BDD, sylvan_refine_partition, BDD, dd, BDD, vars, BDD, previous_partition, void*, refinerPtr)
            {
                auto& refiner = *static_cast<InternalSylvanSignatureRefinerBase*>(refinerPtr);
                
                /* expecting dd as in s,a,B */
                /* expecting vars to be conjunction of variables in s */
                /* expecting previous_partition as in t,B */
                
                if (previous_partition == sylvan_false) {
                    /* it had no block in the previous iteration, therefore also not now */
                    return sylvan_false;
                }
                
                if (sylvan_set_isempty(vars)) {
                    BDD result;
                    if (cache_get(dd|(256LL<<42), vars, previous_partition|(refiner.numberOfRefinements<<40), &result)) return result;
                    result = CALL(sylvan_assign_block, dd, previous_partition, refinerPtr);
                    cache_put(dd|(256LL<<42), vars, previous_partition|(refiner.numberOfRefinements<<40), result);
                    return result;
                }
                
                sylvan_gc_test();
                
                /* vars != sylvan_false */
                /* dd cannot be sylvan_true - if vars != sylvan_true, then dd is in a,B */
                
                BDDVAR dd_var = sylvan_isconst(dd) ? 0xffffffff : sylvan_var(dd);
                BDDVAR pp_var = sylvan_var(previous_partition);
                BDDVAR vars_var = sylvan_var(vars);
                
                while (vars_var < dd_var && vars_var+1 < pp_var) {
                    vars = sylvan_set_next(vars);
                    if (sylvan_set_isempty(vars)) return CALL(sylvan_refine_partition, dd, vars, previous_partition, refinerPtr);
                    vars_var = sylvan_var(vars);
                }
                
                /* Consult cache */
                BDD result;
                if (cache_get(dd|(256LL<<42), vars, previous_partition|(refiner.numberOfRefinements<<40), &result)) {
                    return result;
                }
                
                /* Compute cofactors */
                BDD dd_low, dd_high;
                if (vars_var == dd_var) {
                    dd_low = sylvan_low(dd);
                    dd_high = sylvan_high(dd);
                } else {
                    dd_low = dd_high = dd;
                }
                
                BDD pp_low, pp_high;
                if (vars_var+1 == pp_var) {
                    pp_low = sylvan_low(previous_partition);
                    pp_high = sylvan_high(previous_partition);
                } else {
                    pp_low = pp_high = previous_partition;
                }
                
                /* Recursive steps */
                BDD next_vars = sylvan_set_next(vars);
                bdd_refs_spawn(SPAWN(sylvan_refine_partition, dd_low, next_vars, pp_low, refinerPtr));
                BDD high = bdd_refs_push(CALL(sylvan_refine_partition, dd_high, next_vars, pp_high, refinerPtr));
                BDD low = bdd_refs_sync(SYNC(sylvan_refine_partition));
                bdd_refs_pop(1);
                
                /* rename from s to t */
                result = sylvan_makenode(vars_var+1, low, high);
                
                /* Write to cache */
                cache_put(dd|(256LL<<42), vars, previous_partition|(refiner.numberOfRefinements<<40), result);
                return result;
            }
            
            template class InternalSignatureRefiner<storm::dd::DdType::Sylvan, double>;
            template class InternalSignatureRefiner<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class InternalSignatureRefiner<storm::dd::DdType::Sylvan, storm::RationalFunction>;

        }
    }
}
