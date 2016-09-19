#include "src/storage/jani/CompositionActionInformationVisitor.h"

#include "src/storage/jani/Model.h"
#include "src/storage/jani/Compositions.h"

namespace storm {
    namespace jani {
        
        ActionInformation::ActionInformation(std::set<uint64_t> const& nonsilentActionIndices, std::map<uint64_t, std::string> const& indexToNameMap, std::map<std::string, uint64_t> const& nameToIndexMap, uint64_t silentActionIndex) : silentActionIndex(silentActionIndex), nonsilentActionIndices(nonsilentActionIndices), indexToNameMap(indexToNameMap), nameToIndexMap(nameToIndexMap) {
            // Intentionally left empty.
        }
        
        std::string const& ActionInformation::getActionName(uint64_t index) const {
            return indexToNameMap.at(index);
        }
        
        uint64_t ActionInformation::getActionIndex(std::string const& name) const {
            return nameToIndexMap.at(name);
        }
        
        std::set<uint64_t> const& ActionInformation::getNonSilentActionIndices() const {
            return nonsilentActionIndices;
        }
        
        uint64_t ActionInformation::getSilentActionIndex() const {
            return silentActionIndex;
        }
     
        CompositionActionInformationVisitor::CompositionActionInformationVisitor(storm::jani::Model const& model) : model(model), nextFreeActionIndex(0), nameToIndexMap(), indexToNameMap() {
            // Intentionally left empty.
        }
        
        ActionInformation CompositionActionInformationVisitor::getActionInformation(storm::jani::Composition const& composition) {
            indexToNameMap.clear();
            nameToIndexMap.clear();

            // Determine the next free index we can give out to a new action.
            for (auto const& action : model.getActions()) {
                uint64_t actionIndex = model.getActionIndex(action.getName());
                
                nameToIndexMap[action.getName()] = model.getActionIndex(action.getName());
                indexToNameMap[actionIndex] = action.getName();

                nextFreeActionIndex = std::max(nextFreeActionIndex, model.getActionIndex(action.getName()));
            }
            ++nextFreeActionIndex;
            
            std::set<uint64_t> nonSilentActionIndices = boost::any_cast<std::set<uint64_t>>(composition.accept(*this, boost::none));
            
            return ActionInformation(nonSilentActionIndices, indexToNameMap, nameToIndexMap);
        }
        
        boost::any CompositionActionInformationVisitor::visit(AutomatonComposition const& composition, boost::any const& data) {
            std::set<uint64_t> result = model.getAutomaton(composition.getAutomatonName()).getUsedActionIndices();
            result.erase(model.getSilentActionIndex());
            return result;
        }
        
        boost::any CompositionActionInformationVisitor::visit(RenameComposition const& composition, boost::any const& data) {
            std::set<uint64_t> usedActions = boost::any_cast<std::set<uint64_t>>(composition.getSubcomposition().accept(*this, boost::none));
            
            std::set<uint64_t> newUsedActions;
            for (auto const& index : usedActions) {
                auto renamingIt = composition.getRenaming().find(indexToNameMap.at(index));
                if (renamingIt != composition.getRenaming().end()) {
                    if (renamingIt->second) {
                        newUsedActions.insert(addOrGetActionIndex(renamingIt->second.get()));

                        auto actionIndexIt = nameToIndexMap.find(renamingIt->second.get());
                        if (actionIndexIt != nameToIndexMap.end()) {
                            newUsedActions.insert(actionIndexIt->second);
                        } else {
                            nameToIndexMap[renamingIt->second.get()] = nextFreeActionIndex;
                            indexToNameMap[nextFreeActionIndex] = renamingIt->second.get();
                            ++nextFreeActionIndex;
                        }
                    }
                } else {
                    newUsedActions.insert(index);
                }
            }
            
            return newUsedActions;
        }
        
        boost::any CompositionActionInformationVisitor::visit(ParallelComposition const& composition, boost::any const& data) {
            std::vector<std::set<uint64_t>> subresults;
            for (auto const& subcomposition : composition.getSubcompositions()) {
                subresults.push_back(boost::any_cast<std::set<uint64_t>>(subcomposition->accept(*this, boost::none)));
            }

            std::set<uint64_t> effectiveSynchronizationVectors;
            for (uint64_t index = 0; index < composition.getNumberOfSynchronizationVectors(); ++index) {
                effectiveSynchronizationVectors.insert(index);
            }
            
            // Determine all actions that do not take part in synchronization vectors.
            std::set<uint64_t> result;
            for (uint64_t subresultIndex = 0; subresultIndex < subresults.size(); ++subresultIndex) {
                
                std::set<uint64_t> actionsInSynch;
                std::set<uint64_t> localEffectiveSynchVectors;
                for (uint64_t synchVectorIndex = 0; synchVectorIndex < composition.getNumberOfSynchronizationVectors(); ++synchVectorIndex) {
                    auto const& synchVector = composition.getSynchronizationVector(synchVectorIndex);
                    
                    if (synchVector.isNoActionInput(synchVector.getInput(subresultIndex))) {
                        effectiveSynchronizationVectors.insert(synchVectorIndex);
                    } else {
                        uint64_t synchVectorActionIndex = nameToIndexMap.at(synchVector.getInput(subresultIndex));
                        actionsInSynch.insert(synchVectorActionIndex);
                        
                        // If the action of they synchronization vector at this position is one that is actually contained
                        // in the corresponding subcomposition, the synchronization vector is effective.
                        if (subresults[subresultIndex].find(synchVectorActionIndex) != subresults[subresultIndex].end()) {
                            effectiveSynchronizationVectors.insert(synchVectorIndex);
                        }
                    }
                }
                
                std::set_difference(subresults[subresultIndex].begin(), subresults[subresultIndex].end(), actionsInSynch.begin(), actionsInSynch.end(), std::inserter(result, result.begin()));
                
                // Intersect the previously effective synchronization vectors with the ones that were derived to be
                // effective for the current subcomposition.
                std::set<uint64_t> newEffectiveSynchVectors;
                std::set_intersection(effectiveSynchronizationVectors.begin(), effectiveSynchronizationVectors.end(), newEffectiveSynchVectors.begin(), newEffectiveSynchVectors.end(), std::inserter(newEffectiveSynchVectors, newEffectiveSynchVectors.begin()));
                effectiveSynchronizationVectors = std::move(newEffectiveSynchVectors);
            }
            
            // Now add all outputs of synchronization vectors.
            for (auto const& synchVector : composition.getSynchronizationVectors()) {
                result.insert(addOrGetActionIndex(synchVector.getOutput()));
            }
            
            return result;
        }

        uint64_t CompositionActionInformationVisitor::addOrGetActionIndex(std::string const& name) {
            auto it = nameToIndexMap.find(name);
            if (it != nameToIndexMap.end()) {
                return it->second;
            } else {
                nameToIndexMap[name] = nextFreeActionIndex;
                indexToNameMap[nextFreeActionIndex] = name;
                return nextFreeActionIndex++;
            }
        }
        
    }
}
