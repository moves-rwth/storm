#include "storm/storage/dd/bisimulation/SignatureRefiner.h"

#include <cstdio>
#include <mutex>

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
            
            struct InternalSignatureRefinerOptions {
                InternalSignatureRefinerOptions() : InternalSignatureRefinerOptions(true) {
                    // Intentionally left empty.
                }
                
                InternalSignatureRefinerOptions(bool shiftStateVariables) : shiftStateVariables(shiftStateVariables), createChangedStates(true), parallel(false) {
                    auto const& bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();

                    storm::settings::modules::BisimulationSettings::ReuseMode reuseMode = bisimulationSettings.getReuseMode();
                    this->reuseBlockNumbers = reuseMode == storm::settings::modules::BisimulationSettings::ReuseMode::BlockNumbers;
                    
                    storm::settings::modules::BisimulationSettings::RefinementMode refinementMode = bisimulationSettings.getRefinementMode();
                    this->createChangedStates = refinementMode == storm::settings::modules::BisimulationSettings::RefinementMode::ChangedStates;
                    
                    storm::settings::modules::BisimulationSettings::ParallelismMode parallelismMode = bisimulationSettings.getParallelismMode();
                    this->parallel = parallelismMode == storm::settings::modules::BisimulationSettings::ParallelismMode::Parallel;
                }
                
                bool shiftStateVariables;
                bool reuseBlockNumbers;
                bool createChangedStates;
                bool parallel;
            };
            
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
                InternalSignatureRefiner(storm::dd::DdManager<storm::dd::DdType::CUDD> const& manager, storm::expressions::Variable const& blockVariable, std::set<storm::expressions::Variable> const& stateVariables, storm::dd::Bdd<storm::dd::DdType::CUDD> const& nondeterminismVariables, storm::dd::Bdd<storm::dd::DdType::CUDD> const& nonBlockVariables, InternalSignatureRefinerOptions const& options = InternalSignatureRefinerOptions()) : manager(manager), internalDdManager(manager.getInternalDdManager()), ddman(internalDdManager.getCuddManager().getManager()), blockVariable(blockVariable), stateVariables(stateVariables), nondeterminismVariables(nondeterminismVariables), nonBlockVariables(nonBlockVariables), options(options), nextFreeBlockIndex(0), numberOfRefinements(0), lastNumberOfVisitedNodes(10000), signatureCache(lastNumberOfVisitedNodes), reuseBlocksCache(lastNumberOfVisitedNodes) {

                    // Initialize precomputed data.
                    auto const& ddMetaVariable = manager.getMetaVariable(blockVariable);
                    blockDdVariableIndices = ddMetaVariable.getIndices();
                    
                    // Create initialized block encoding where all variables are "don't care".
                    blockEncoding = std::vector<int>(static_cast<uint64_t>(internalDdManager.getCuddManager().ReadSize()), static_cast<int>(2));
                }
                
                Partition<storm::dd::DdType::CUDD, ValueType> refine(Partition<storm::dd::DdType::CUDD, ValueType> const& oldPartition, Signature<storm::dd::DdType::CUDD, ValueType> const& signature) {
                    std::pair<storm::dd::Add<storm::dd::DdType::CUDD, ValueType>, boost::optional<storm::dd::Add<storm::dd::DdType::CUDD, ValueType>>> newPartitionDds = refine(oldPartition, signature.getSignatureAdd());;
                    ++numberOfRefinements;
                    return oldPartition.replacePartition(newPartitionDds.first, nextFreeBlockIndex, nextFreeBlockIndex, newPartitionDds.second);
                }
                
            private:
                void clearCaches() {
                    signatureCache.clear();
                    reuseBlocksCache.clear();
                }
                
                std::pair<storm::dd::Add<storm::dd::DdType::CUDD, ValueType>, boost::optional<storm::dd::Add<storm::dd::DdType::CUDD, ValueType>>> refine(Partition<storm::dd::DdType::CUDD, ValueType> const& oldPartition, storm::dd::Add<storm::dd::DdType::CUDD, ValueType> const& signatureAdd) {
                    STORM_LOG_ASSERT(oldPartition.storedAsAdd(), "Expecting partition to be stored as ADD for CUDD.");

                    nextFreeBlockIndex = options.reuseBlockNumbers ? oldPartition.getNextFreeBlockIndex() : 0;

                    // Perform the actual recursive refinement step.
                    std::pair<DdNodePtr, DdNodePtr> result = refine(oldPartition.asAdd().getInternalAdd().getCuddDdNode(), signatureAdd.getInternalAdd().getCuddDdNode(), nondeterminismVariables.getInternalBdd().getCuddDdNode(), nonBlockVariables.getInternalBdd().getCuddDdNode());

                    // Construct resulting ADD from the obtained node and the meta information.
                    storm::dd::InternalAdd<storm::dd::DdType::CUDD, ValueType> internalNewPartitionAdd(&internalDdManager, cudd::ADD(internalDdManager.getCuddManager(), result.first));
                    storm::dd::Add<storm::dd::DdType::CUDD, ValueType> newPartitionAdd(oldPartition.asAdd().getDdManager(), internalNewPartitionAdd, oldPartition.asAdd().getContainedMetaVariables());
                    
                    boost::optional<storm::dd::Add<storm::dd::DdType::CUDD, ValueType>> optionalChangedAdd;
                    if (result.second) {
                        storm::dd::InternalAdd<storm::dd::DdType::CUDD, ValueType> internalChangedAdd(&internalDdManager, cudd::ADD(internalDdManager.getCuddManager(), result.second));
                        storm::dd::Add<storm::dd::DdType::CUDD, ValueType> changedAdd(oldPartition.asAdd().getDdManager(), internalChangedAdd, stateVariables);
                        optionalChangedAdd = changedAdd;
                    }
                    
                    clearCaches();
                    return std::make_pair(newPartitionAdd, optionalChangedAdd);
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
                
                std::pair<DdNodePtr, DdNodePtr> reuseOrRelabel(DdNode* partitionNode, DdNode* nondeterminismVariablesNode, DdNode* nonBlockVariablesNode) {
                    // If we arrived at the constant zero node, then this was an illegal state encoding.
                    if (partitionNode == Cudd_ReadZero(ddman)) {
                        if (options.createChangedStates) {
                            return std::make_pair(partitionNode, Cudd_ReadZero(ddman));
                        } else {
                            return std::make_pair(partitionNode, nullptr);
                        }
                    }
                    
                    // Check the cache whether we have seen the same node before.
                    auto nodePair = std::make_pair(nullptr, partitionNode);
                    
                    auto it = signatureCache.find(nodePair);
                    if (it != signatureCache.end()) {
                        // If so, we return the corresponding result.
                        return it->second;
                    }
                    
                    // If there are no more non-block variables, we hit the signature.
                    if (Cudd_IsConstant(nonBlockVariablesNode)) {
                        if (options.reuseBlockNumbers) {
                            
                            // If this is the first time (in this traversal) that we encounter this signature, we check
                            // whether we can assign the old block number to it.
                            auto& reuseEntry = reuseBlocksCache[partitionNode];
                            if (!reuseEntry.isReused()) {
                                reuseEntry.setReused();
                                std::pair<DdNodePtr, DdNodePtr> result = std::make_pair(partitionNode, options.createChangedStates ? Cudd_ReadZero(ddman) : nullptr);
                                signatureCache[nodePair] = result;
                                return result;
                            }
                        }
                        
                        std::pair<DdNodePtr, DdNodePtr> result = std::make_pair(encodeBlock(nextFreeBlockIndex++), options.createChangedStates ? Cudd_ReadOne(ddman) : nullptr);
                        signatureCache[nodePair] = result;
                        return result;
                    } else {
                        // If there are more variables that belong to the non-block part of the encoding, we need to recursively descend.
                        
                        bool skipped = true;
                        DdNode* partitionThen;
                        DdNode* partitionElse;
                        short offset;
                        bool isNondeterminismVariable = false;
                        while (skipped && !Cudd_IsConstant(nonBlockVariablesNode)) {
                            // Remember an offset that indicates whether the top variable is a nondeterminism variable or not.
                            offset = options.shiftStateVariables ? 1 : 0;
                            if (!Cudd_IsConstant(nondeterminismVariablesNode) && Cudd_NodeReadIndex(nondeterminismVariablesNode) == Cudd_NodeReadIndex(nonBlockVariablesNode)) {
                                offset = 0;
                                isNondeterminismVariable = true;
                            }
                            
                            if (Cudd_NodeReadIndex(partitionNode) - offset == Cudd_NodeReadIndex(nonBlockVariablesNode)) {
                                partitionThen = Cudd_T(partitionNode);
                                partitionElse = Cudd_E(partitionNode);
                                skipped = false;
                            } else {
                                partitionThen = partitionElse = partitionNode;
                            }

                            // If we skipped the next variable, we fast-forward.
                            if (skipped) {
                                // If the current variable is a nondeterminism variable, we need to advance both variable sets otherwise just the non-block variables.
                                nonBlockVariablesNode = Cudd_T(nonBlockVariablesNode);
                                if (isNondeterminismVariable) {
                                    nondeterminismVariablesNode = Cudd_T(nondeterminismVariablesNode);
                                }
                            }
                        }
                        
                        // If there are no more non-block variables remaining, make a recursive call to enter the base case.
                        if (Cudd_IsConstant(nonBlockVariablesNode)) {
                            return reuseOrRelabel(partitionNode, nondeterminismVariablesNode, nonBlockVariablesNode);
                        }
                        
                        std::pair<DdNodePtr, DdNodePtr> combinedThenResult = reuseOrRelabel(partitionThen, isNondeterminismVariable ? Cudd_T(nondeterminismVariablesNode) : nondeterminismVariablesNode, Cudd_T(nonBlockVariablesNode));
                        DdNodePtr thenResult = combinedThenResult.first;
                        DdNodePtr changedThenResult = combinedThenResult.second;
                        Cudd_Ref(thenResult);
                        if (options.createChangedStates) {
                            Cudd_Ref(changedThenResult);
                        } else {
                            STORM_LOG_ASSERT(!changedThenResult, "Expected not changed state DD.");
                        }
                        std::pair<DdNodePtr, DdNodePtr> combinedElseResult = reuseOrRelabel(partitionElse, isNondeterminismVariable ? Cudd_T(nondeterminismVariablesNode) : nondeterminismVariablesNode, Cudd_T(nonBlockVariablesNode));
                        DdNodePtr elseResult = combinedElseResult.first;
                        DdNodePtr changedElseResult = combinedElseResult.second;
                        Cudd_Ref(elseResult);
                        if (options.createChangedStates) {
                            Cudd_Ref(changedElseResult);
                        } else {
                            STORM_LOG_ASSERT(!changedThenResult, "Expected not changed state DD.");
                        }
                        
                        DdNodePtr result;
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
                        
                        DdNodePtr changedResult = nullptr;
                        if (options.createChangedStates) {
                            if (changedThenResult == changedElseResult) {
                                Cudd_Deref(changedThenResult);
                                Cudd_Deref(changedElseResult);
                                changedResult = changedThenResult;
                            } else {
                                // Get the node to connect the subresults.
                                bool complemented = Cudd_IsComplement(changedThenResult);
                                changedResult = cuddUniqueInter(ddman, Cudd_NodeReadIndex(nonBlockVariablesNode) + offset, Cudd_Regular(changedThenResult), complemented ? Cudd_Not(changedElseResult) : changedElseResult);
                                if (complemented) {
                                    changedResult = Cudd_Not(changedResult);
                                }
                                Cudd_Deref(changedThenResult);
                                Cudd_Deref(changedElseResult);
                            }
                        }
                        
                        // Store the result in the cache.
                        auto pairResult = std::make_pair(result, changedResult);
                        signatureCache[nodePair] = pairResult;
                        return pairResult;
                    }
                }
                
                std::pair<DdNodePtr, DdNodePtr> refine(DdNode* partitionNode, DdNode* signatureNode, DdNode* nondeterminismVariablesNode, DdNode* nonBlockVariablesNode) {
                    // If we arrived at the constant zero node, then this was an illegal state encoding.
                    if (partitionNode == Cudd_ReadZero(ddman)) {
                        if (options.createChangedStates) {
                            return std::make_pair(partitionNode, Cudd_ReadZero(ddman));
                        } else {
                            return std::make_pair(partitionNode, nullptr);
                        }
                    } else if (signatureNode == Cudd_ReadZero(ddman)) {
                        std::pair<DdNodePtr, DdNodePtr> result = reuseOrRelabel(partitionNode, nondeterminismVariablesNode, nonBlockVariablesNode);
                        signatureCache[std::make_pair(signatureNode, partitionNode)] = result;
                        return result;
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
                        if (options.reuseBlockNumbers) {

                            // If this is the first time (in this traversal) that we encounter this signature, we check
                            // whether we can assign the old block number to it.
                            auto& reuseEntry = reuseBlocksCache[partitionNode];
                            if (!reuseEntry.isReused()) {
                                reuseEntry.setReused();
                                std::pair<DdNodePtr, DdNodePtr> result = std::make_pair(partitionNode, options.createChangedStates ? Cudd_ReadZero(ddman) : nullptr);
                                signatureCache[nodePair] = result;
                                return result;
                            }
                        }
                    
                        std::pair<DdNodePtr, DdNodePtr> result = std::make_pair(encodeBlock(nextFreeBlockIndex++), options.createChangedStates ? Cudd_ReadOne(ddman) : nullptr);
                        signatureCache[nodePair] = result;
                        return result;
                    } else {
                        // If there are more variables that belong to the non-block part of the encoding, we need to recursively descend.
                        
                        bool skippedBoth = true;
                        DdNode* partitionThen;
                        DdNode* partitionElse;
                        DdNode* signatureThen;
                        DdNode* signatureElse;
                        short offset;
                        bool isNondeterminismVariable = false;
                        while (skippedBoth && !Cudd_IsConstant(nonBlockVariablesNode)) {
                            // Remember an offset that indicates whether the top variable is a nondeterminism variable or not.
                            offset = options.shiftStateVariables ? 1 : 0;
                            if (!Cudd_IsConstant(nondeterminismVariablesNode) && Cudd_NodeReadIndex(nondeterminismVariablesNode) == Cudd_NodeReadIndex(nonBlockVariablesNode)) {
                                offset = 0;
                                isNondeterminismVariable = true;
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
                                if (isNondeterminismVariable) {
                                    nondeterminismVariablesNode = Cudd_T(nondeterminismVariablesNode);
                                }
                            }
                        }
                        
                        // If there are no more non-block variables remaining, make a recursive call to enter the base case.
                        if (Cudd_IsConstant(nonBlockVariablesNode)) {
                            return refine(partitionNode, signatureNode, nondeterminismVariablesNode, nonBlockVariablesNode);
                        }
                        
                        std::pair<DdNodePtr, DdNodePtr> combinedThenResult = refine(partitionThen, signatureThen, isNondeterminismVariable ? Cudd_T(nondeterminismVariablesNode) : nondeterminismVariablesNode, Cudd_T(nonBlockVariablesNode));
                        DdNodePtr thenResult = combinedThenResult.first;
                        DdNodePtr changedThenResult = combinedThenResult.second;
                        Cudd_Ref(thenResult);
                        if (options.createChangedStates) {
                            Cudd_Ref(changedThenResult);
                        } else {
                            STORM_LOG_ASSERT(!changedThenResult, "Expected not changed state DD.");
                        }
                        std::pair<DdNodePtr, DdNodePtr> combinedElseResult = refine(partitionElse, signatureElse, isNondeterminismVariable ? Cudd_T(nondeterminismVariablesNode) : nondeterminismVariablesNode, Cudd_T(nonBlockVariablesNode));
                        DdNodePtr elseResult = combinedElseResult.first;
                        DdNodePtr changedElseResult = combinedElseResult.second;
                        Cudd_Ref(elseResult);
                        if (options.createChangedStates) {
                            Cudd_Ref(changedElseResult);
                        } else {
                            STORM_LOG_ASSERT(!changedThenResult, "Expected not changed state DD.");
                        }

                        DdNodePtr result;
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
                        
                        DdNodePtr changedResult = nullptr;
                        if (options.createChangedStates) {
                            if (changedThenResult == changedElseResult) {
                                Cudd_Deref(changedThenResult);
                                Cudd_Deref(changedElseResult);
                                changedResult = changedThenResult;
                            } else {
                                // Get the node to connect the subresults.
                                bool complemented = Cudd_IsComplement(changedThenResult);
                                changedResult = cuddUniqueInter(ddman, Cudd_NodeReadIndex(nonBlockVariablesNode) + offset, Cudd_Regular(changedThenResult), complemented ? Cudd_Not(changedElseResult) : changedElseResult);
                                if (complemented) {
                                    changedResult = Cudd_Not(changedResult);
                                }
                                Cudd_Deref(changedThenResult);
                                Cudd_Deref(changedElseResult);
                            }
                        }
                        
                        // Store the result in the cache.
                        auto pairResult = std::make_pair(result, changedResult);
                        signatureCache[nodePair] = pairResult;
                        return pairResult;
                    }
                }
                
                storm::dd::DdManager<storm::dd::DdType::CUDD> const& manager;
                storm::dd::InternalDdManager<storm::dd::DdType::CUDD> const& internalDdManager;
                ::DdManager* ddman;
                storm::expressions::Variable blockVariable;
                std::set<storm::expressions::Variable> stateVariables;
                
                // The cubes representing all non-block and all nondeterminism variables, respectively.
                storm::dd::Bdd<storm::dd::DdType::CUDD> nondeterminismVariables;
                storm::dd::Bdd<storm::dd::DdType::CUDD> nonBlockVariables;

                // The provided options.
                InternalSignatureRefinerOptions options;
                
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
                spp::sparse_hash_map<std::pair<DdNode const*, DdNode const*>, std::pair<DdNodePtr, DdNodePtr>, CuddPointerPairHash> signatureCache;
                
                // The cache used to identify which old block numbers have already been reused.
                spp::sparse_hash_map<DdNode const*, ReuseWrapper> reuseBlocksCache;
            };
            
            TASK_DECL_5(BDD, refine_double, BDD, MTBDD, BDD, BDD, void*);

            template<storm::dd::DdType DdType>
            class InternalSignatureRefinerBase;
            
            template<>
            class InternalSignatureRefinerBase<storm::dd::DdType::Sylvan> {
            public:
                InternalSignatureRefinerBase(storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager, storm::expressions::Variable const& blockVariable, std::set<storm::expressions::Variable> const& stateVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nondeterminismVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nonBlockVariables, InternalSignatureRefinerOptions const& options) : manager(manager), internalDdManager(manager.getInternalDdManager()), blockVariable(blockVariable), stateVariables(stateVariables), nondeterminismVariables(nondeterminismVariables), nonBlockVariables(nonBlockVariables), options(options), numberOfBlockVariables(manager.getMetaVariable(blockVariable).getNumberOfDdVariables()), blockCube(manager.getMetaVariable(blockVariable).getCube()), nextFreeBlockIndex(0), numberOfRefinements(0), signatureCache() {
                    
                    // Perform garbage collection to clean up stuff not needed anymore.
                    LACE_ME;
                    sylvan_gc();
                }
                
                BDD encodeBlock(uint64_t blockIndex) {
                    std::vector<uint8_t> e(numberOfBlockVariables);
                    for (uint64_t i = 0; i < numberOfBlockVariables; ++i) {
                        e[i] = blockIndex & 1 ? 1 : 0;
                        blockIndex >>= 1;
                    }
                    return sylvan_cube(blockCube.getInternalBdd().getSylvanBdd().GetBDD(), e.data());
                }

                storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager;
                storm::dd::InternalDdManager<storm::dd::DdType::Sylvan> const& internalDdManager;
                storm::expressions::Variable blockVariable;
                std::set<storm::expressions::Variable> stateVariables;
                
                storm::dd::Bdd<storm::dd::DdType::Sylvan> nondeterminismVariables;
                storm::dd::Bdd<storm::dd::DdType::Sylvan> nonBlockVariables;
                
                // The provided options.
                InternalSignatureRefinerOptions options;
                
                uint64_t numberOfBlockVariables;
                
                storm::dd::Bdd<storm::dd::DdType::Sylvan> blockCube;
                
                // The current number of blocks of the new partition.
                uint64_t nextFreeBlockIndex;
                
                // The number of completed refinements.
                uint64_t numberOfRefinements;
                
                // The cache used to identify states with identical signature.
                spp::sparse_hash_map<std::pair<MTBDD, MTBDD>, std::pair<BDD, BDD>, SylvanMTBDDPairHash> signatureCache;
                spp::sparse_hash_map<std::pair<MTBDD, MTBDD>, BDD, SylvanMTBDDPairHash> signatureCacheSingle;
                
                // The cache used to identify which old block numbers have already been reused.
                spp::sparse_hash_map<MTBDD, ReuseWrapper> reuseBlocksCache;

                // A mutex that can be used to synchronize concurrent accesses to the members.
                std::mutex mutex;
            };

            template<typename ValueType>
            class InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType> : public InternalSignatureRefinerBase<storm::dd::DdType::Sylvan> {
            public:
                InternalSignatureRefiner(storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager, storm::expressions::Variable const& blockVariable, std::set<storm::expressions::Variable> const& stateVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nondeterminismVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nonBlockVariables, InternalSignatureRefinerOptions const& options) : InternalSignatureRefinerBase<storm::dd::DdType::Sylvan>(manager, blockVariable, stateVariables, nondeterminismVariables, nonBlockVariables, options) {
                    
                    // Intentionally left empty.
                }
                
                Partition<storm::dd::DdType::Sylvan, ValueType> refine(Partition<storm::dd::DdType::Sylvan, ValueType> const& oldPartition, Signature<storm::dd::DdType::Sylvan, ValueType> const& signature) {
                    std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, boost::optional<storm::dd::Bdd<storm::dd::DdType::Sylvan>>> newPartitionDds = refine(oldPartition, signature.getSignatureAdd());
                    ++numberOfRefinements;
                    return oldPartition.replacePartition(newPartitionDds.first, nextFreeBlockIndex, nextFreeBlockIndex, newPartitionDds.second);
                }
                
                void clearCaches() {
                    signatureCache.clear();
                    signatureCacheSingle.clear();
                    reuseBlocksCache.clear();
                }
                
                std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, boost::optional<storm::dd::Bdd<storm::dd::DdType::Sylvan>>> refine(Partition<storm::dd::DdType::Sylvan, ValueType> const& oldPartition, storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> const& signatureAdd) {
                    STORM_LOG_ASSERT(oldPartition.storedAsBdd(), "Expecting partition to be stored as BDD for Sylvan.");
                    
                    nextFreeBlockIndex = options.reuseBlockNumbers ? oldPartition.getNextFreeBlockIndex() : 0;
                    
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
                    storm::dd::InternalBdd<storm::dd::DdType::Sylvan> internalNewPartitionBdd(&internalDdManager, sylvan::Bdd(result.first));
                    storm::dd::Bdd<storm::dd::DdType::Sylvan> newPartitionBdd(oldPartition.asBdd().getDdManager(), internalNewPartitionBdd, oldPartition.asBdd().getContainedMetaVariables());
                    
                    boost::optional<storm::dd::Bdd<storm::dd::DdType::Sylvan>> optionalChangedBdd;
                    if (options.createChangedStates && result.second != 0) {
                        storm::dd::InternalBdd<storm::dd::DdType::Sylvan> internalChangedBdd(&internalDdManager, sylvan::Bdd(result.second));
                        storm::dd::Bdd<storm::dd::DdType::Sylvan> changedBdd(oldPartition.asBdd().getDdManager(), internalChangedBdd, stateVariables);
                        optionalChangedBdd = changedBdd;
                    }
                    
                    clearCaches();
                    return std::make_pair(newPartitionBdd, optionalChangedBdd);
                }
                
                std::pair<BDD, BDD> reuseOrRelabel(BDD partitionNode, BDD nondeterminismVariablesNode, BDD nonBlockVariablesNode) {
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
                                reuseBlocksCache.emplace(partitionNode, true);
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
                
                std::pair<BDD, BDD> refineParallel(BDD partitionNode, MTBDD signatureNode, BDD nondeterminismVariablesNode, BDD nonBlockVariablesNode) {
                    LACE_ME;
                    return std::make_pair(CALL(refine_double, partitionNode, signatureNode, nondeterminismVariablesNode, nonBlockVariablesNode, this), 0);
                }
                
                std::pair<BDD, BDD> refineSequential(BDD partitionNode, MTBDD signatureNode, BDD nondeterminismVariablesNode, BDD nonBlockVariablesNode) {
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
                                reuseBlocksCache.emplace(partitionNode, true);
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
                
            };
            
            TASK_IMPL_5(BDD, refine_double, BDD, partitionNode, MTBDD, signatureNode, BDD, nondeterminismVariablesNode, BDD, nonBlockVariablesNode, void*, refinerPtr)
            {
                auto& refiner = *static_cast<InternalSignatureRefinerBase<storm::dd::DdType::Sylvan>*>(refinerPtr);
                std::unique_lock<std::mutex> lock(refiner.mutex, std::defer_lock);

                // If we arrived at the constant zero node, then this was an illegal state encoding (we require
                // all states to be non-deadlock).
                if (partitionNode == sylvan_false) {
                    return sylvan_false;
                }
                
                STORM_LOG_ASSERT(partitionNode != mtbdd_false, "Expected non-false node.");
                
                BDD result;

                if (sylvan_isconst(nonBlockVariablesNode)) {
                    // Get the lock so we can modify the signature cache.
                    lock.lock();
                    
                    // Check the signature cache whether we have seen this signature before.
                    auto nodePair = std::make_pair(signatureNode, partitionNode);
                    auto it = refiner.signatureCacheSingle.find(nodePair);
                    if (it != refiner.signatureCacheSingle.end()) {
                        return it->second;
                    }
                    
                    if (refiner.options.reuseBlockNumbers) {
                        // If this is the first time (in this traversal) that we encounter this signature, we check
                        // whether we can assign the old block number to it.
                        
                        auto& reuseBlockEntry = refiner.reuseBlocksCache[partitionNode];
                        if (!reuseBlockEntry.isReused()) {
                            reuseBlockEntry.setReused();
                            refiner.reuseBlocksCache.emplace(partitionNode, true);
                            refiner.signatureCacheSingle[nodePair] = partitionNode;
                            return partitionNode;
                        }
                    }
                    
                    // Encode new block and give up lock before 
                    result = refiner.encodeBlock(refiner.nextFreeBlockIndex++);
                    refiner.signatureCacheSingle[nodePair] = result;
                    lock.unlock();
                    return result;
                }
                
                // Check the cache whether we have seen the same node before.
                if (cache_get(signatureNode|(256LL<<42), nonBlockVariablesNode, partitionNode|(refiner.numberOfRefinements<<40), &result)) {
                    return result;
                }
                
                sylvan_gc_test();
                
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
                    offset = refiner.options.shiftStateVariables ? 1 : 0;
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
                    return CALL(refine_double, partitionNode, signatureNode, nondeterminismVariablesNode, nonBlockVariablesNode, refinerPtr);
                }
                
                BDD nextNondeterminismVariables = isNondeterminismVariable ? sylvan_set_next(nondeterminismVariablesNode) : nondeterminismVariablesNode;
                BDD nextNonBlockVariables = sylvan_set_next(nonBlockVariablesNode);
                bdd_refs_spawn(SPAWN(refine_double, partitionThen, signatureThen, nextNondeterminismVariables, nextNonBlockVariables, refinerPtr));
                
                BDD elseResult = bdd_refs_push(CALL(refine_double, partitionElse, signatureElse, nextNondeterminismVariables, nextNonBlockVariables, refinerPtr));
                BDD thenResult = bdd_refs_sync(SYNC(refine_double));
                bdd_refs_pop(1);
                
                if (thenResult == elseResult) {
                    result = thenResult;
                } else {
                    // Get the node to connect the subresults.
                    result = sylvan_makenode(sylvan_var(nonBlockVariablesNode) + offset, elseResult, thenResult);
                }
                
                // Store the result in the cache.
                cache_put(signatureNode|(256LL<<42), nonBlockVariablesNode, partitionNode|(refiner.numberOfRefinements<<40), result);
                
                return result;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            SignatureRefiner<DdType, ValueType>::SignatureRefiner(storm::dd::DdManager<DdType> const& manager, storm::expressions::Variable const& blockVariable, std::set<storm::expressions::Variable> const& stateRowVariables, std::set<storm::expressions::Variable> const& stateColumnVariables, bool shiftStateVariables, std::set<storm::expressions::Variable> const& nondeterminismVariables) : manager(&manager) {
                
                storm::dd::Bdd<DdType> nonBlockVariablesCube = manager.getBddOne();
                storm::dd::Bdd<DdType> nondeterminismVariablesCube = manager.getBddOne();
                for (auto const& var : nondeterminismVariables) {
                    auto cube = manager.getMetaVariable(var).getCube();
                    nonBlockVariablesCube &= cube;
                    nondeterminismVariablesCube &= cube;
                }
                for (auto const& var : stateRowVariables) {
                    auto cube = manager.getMetaVariable(var).getCube();
                    nonBlockVariablesCube &= cube;
                }
                
                internalRefiner = std::make_unique<InternalSignatureRefiner<DdType, ValueType>>(manager, blockVariable, shiftStateVariables ? stateColumnVariables : stateRowVariables, nondeterminismVariablesCube, nonBlockVariablesCube, InternalSignatureRefinerOptions(shiftStateVariables));
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
