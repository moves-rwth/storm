#include "src/storage/jani/CompositionInformationVisitor.h"

#include "src/storage/jani/Model.h"
#include "src/storage/jani/Compositions.h"

namespace storm {
    namespace jani {
        
        CompositionInformation::CompositionInformation() : automatonNameToMultiplicity(), nonsilentActions(), renameComposition(false), nonStandardParallelComposition(false) {
            // Intentionally left empty.
        }
        
        CompositionInformation::CompositionInformation(std::map<std::string, uint64_t> const& automatonNameToMultiplicity, std::set<std::string> const& nonsilentActions, bool containsRenameComposition, bool nonStandardParallelComposition) : automatonNameToMultiplicity(automatonNameToMultiplicity), nonsilentActions(nonsilentActions), renameComposition(containsRenameComposition), nonStandardParallelComposition(nonStandardParallelComposition) {
            // Intentionally left empty.
        }
        
        void CompositionInformation::increaseAutomatonMultiplicity(std::string const& automatonName, uint64_t count) {
            automatonNameToMultiplicity[automatonName] += count;
        }
        
        void CompositionInformation::addNonsilentAction(std::string const& actionName) {
            nonsilentActions.insert(actionName);
        }
        
        std::set<std::string> const& CompositionInformation::getNonsilentActions() const {
            return nonsilentActions;
        }
        
        std::set<std::string> CompositionInformation::renameNonsilentActions(std::set<std::string> const& nonsilentActions, std::map<std::string, boost::optional<std::string>> const& renaming) {
            std::set<std::string> newNonsilentActions;
            for (auto const& entry : nonsilentActions) {
                auto it = renaming.find(entry);
                if (it != renaming.end()) {
                    if (it->second) {
                        newNonsilentActions.insert(it->second.get());
                    }
                } else {
                    newNonsilentActions.insert(entry);
                }
            }
            return newNonsilentActions;
        }
        
        void CompositionInformation::setContainsRenameComposition() {
            renameComposition = true;
        }
        
        bool CompositionInformation::containsRenameComposition() const {
            return renameComposition;
        }
        
        void CompositionInformation::setContainsNonStandardParallelComposition() {
            nonStandardParallelComposition = true;
        }
        
        bool CompositionInformation::containsNonStandardParallelComposition() const {
            return nonStandardParallelComposition;
        }
        
        std::map<std::string, uint64_t> CompositionInformation::joinMultiplicityMaps(std::map<std::string, uint64_t> const& first, std::map<std::string, uint64_t> const& second) {
            std::map<std::string, uint64_t> result = first;
            for (auto const& element : second) {
                result[element.first] += element.second;
            }
            return result;
        }
        
        std::map<std::string, uint64_t> const& CompositionInformation::getAutomatonToMultiplicityMap() const {
            return automatonNameToMultiplicity;
        }
        
        CompositionInformation CompositionInformationVisitor::getInformation(Composition const& composition, Model const& model) {
            return boost::any_cast<CompositionInformation>(composition.accept(*this, model));
        }
        
        boost::any CompositionInformationVisitor::visit(AutomatonComposition const& composition, boost::any const& data) {
            Model const& model = boost::any_cast<Model const&>(data);
            Automaton const& automaton = model.getAutomaton(composition.getAutomatonName());
            
            CompositionInformation result;
            result.increaseAutomatonMultiplicity(composition.getAutomatonName());
            for (auto const& actionIndex : automaton.getActionIndices()) {
                if (actionIndex != model.getSilentActionIndex()) {
                    result.addNonsilentAction(model.getAction(actionIndex).getName());
                }
            }
            return result;
        }
        
        boost::any CompositionInformationVisitor::visit(RenameComposition const& composition, boost::any const& data) {
            CompositionInformation subresult = boost::any_cast<CompositionInformation>(composition.getSubcomposition().accept(*this, data));
            std::set<std::string> nonsilentActions = CompositionInformation::renameNonsilentActions(subresult.getNonsilentActions(), composition.getRenaming());
            return CompositionInformation(subresult.getAutomatonToMultiplicityMap(), nonsilentActions, true, subresult.containsNonStandardParallelComposition());
        }
        
        boost::any CompositionInformationVisitor::visit(ParallelComposition const& composition, boost::any const& data) {
            std::vector<CompositionInformation> subinformation;
            
            for (auto const& subcomposition : composition.getSubcompositions()) {
                subinformation.push_back(boost::any_cast<CompositionInformation>(subcomposition->accept(*this, data)));
            }
            
            std::map<std::string, uint64_t> joinedAutomatonToMultiplicityMap;
            bool containsRenameComposition = false;
            bool containsNonStandardParallelComposition = false;
            
            for (auto const& subinfo : subinformation) {
                containsRenameComposition |= subinfo.containsRenameComposition();
                containsNonStandardParallelComposition |= subinfo.containsNonStandardParallelComposition();
                joinedAutomatonToMultiplicityMap = CompositionInformation::joinMultiplicityMaps(joinedAutomatonToMultiplicityMap, subinfo.getAutomatonToMultiplicityMap());
                
            }
            
            // Keep track of the synchronization vectors that are effective, meaning that the subcompositions all have
            // the non-silent actions that are referred to.
            std::set<uint64_t> effectiveSynchVectors;
            for (uint64_t synchVectorIndex = 0; synchVectorIndex < composition.getNumberOfSynchronizationVectors(); ++synchVectorIndex) {
                effectiveSynchVectors.insert(synchVectorIndex);
            }
            
            // Now compute non-silent actions.
            std::set<std::string> nonsilentActions;
            for (uint_fast64_t infoIndex = 0; infoIndex < subinformation.size(); ++infoIndex) {
                auto const& subinfo = subinformation[infoIndex];
                
                std::set<uint64_t> enabledSynchVectors;
                for (auto const& nonsilentAction : subinfo.getNonsilentActions()) {
                    bool appearsInSomeSynchVector = false;
                    for (uint64_t synchVectorIndex = 0; synchVectorIndex < composition.getNumberOfSynchronizationVectors(); ++synchVectorIndex) {
                        auto const& synchVector = composition.getSynchronizationVector(synchVectorIndex);
                        if (synchVector.getInput(infoIndex) == nonsilentAction) {
                            appearsInSomeSynchVector = true;
                            enabledSynchVectors.insert(synchVectorIndex);
                        }
                    }
                    
                    if (!appearsInSomeSynchVector) {
                        nonsilentActions.insert(nonsilentAction);
                    }
                }
                
                std::set<uint64_t> newEffectiveSynchVectors;
                std::set_intersection(effectiveSynchVectors.begin(), effectiveSynchVectors.end(), enabledSynchVectors.begin(), enabledSynchVectors.end(), std::inserter(newEffectiveSynchVectors, newEffectiveSynchVectors.begin()));
                effectiveSynchVectors = std::move(newEffectiveSynchVectors);
            }

            return CompositionInformation(joinedAutomatonToMultiplicityMap, nonsilentActions, containsRenameComposition, containsNonStandardParallelComposition);
        }
        
    }
}