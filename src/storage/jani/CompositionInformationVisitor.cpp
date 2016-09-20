#include "src/storage/jani/CompositionInformationVisitor.h"

#include "src/storage/jani/Model.h"
#include "src/storage/jani/Compositions.h"

namespace storm {
    namespace jani {
        
        CompositionInformation::CompositionInformation() : automatonNameToMultiplicity(), nonsilentActions(), nonStandardParallelComposition(false) {
            // Intentionally left empty.
        }
        
        CompositionInformation::CompositionInformation(std::map<std::string, uint64_t> const& automatonNameToMultiplicity, std::set<std::string> const& nonsilentActions, bool nonStandardParallelComposition) : automatonNameToMultiplicity(automatonNameToMultiplicity), nonsilentActions(nonsilentActions), nonStandardParallelComposition(nonStandardParallelComposition) {
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
        
        boost::any CompositionInformationVisitor::visit(ParallelComposition const& composition, boost::any const& data) {
            std::vector<CompositionInformation> subinformation;
            
            std::set<std::string> nonsilentSubActions;
            for (auto const& subcomposition : composition.getSubcompositions()) {
                subinformation.push_back(boost::any_cast<CompositionInformation>(subcomposition->accept(*this, data)));
                nonsilentSubActions.insert(subinformation.back().getNonsilentActions().begin(), subinformation.back().getNonsilentActions().end());
            }
            
            std::map<std::string, uint64_t> joinedAutomatonToMultiplicityMap;
            bool containsNonStandardParallelComposition = false;
            
            for (auto const& subinfo : subinformation) {
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
                std::set<std::string> actionsInSynch;
                for (uint64_t synchVectorIndex = 0; synchVectorIndex < composition.getNumberOfSynchronizationVectors(); ++synchVectorIndex) {
                    auto const& synchVector = composition.getSynchronizationVector(synchVectorIndex);
                    if (synchVector.getInput(infoIndex) != SynchronizationVector::getNoActionInput()) {
                        for (auto const& nonsilentAction : subinfo.getNonsilentActions()) {
                            if (synchVector.getInput(infoIndex) == nonsilentAction) {
                                enabledSynchVectors.insert(synchVectorIndex);
                                actionsInSynch.insert(nonsilentAction);
                            }
                        }
                    } else {
                        enabledSynchVectors.insert(synchVectorIndex);
                    }
                }
                
                std::set_difference(subinfo.getNonsilentActions().begin(), subinfo.getNonsilentActions().end(), actionsInSynch.begin(), actionsInSynch.end(), std::inserter(nonsilentActions, nonsilentActions.begin()));

                std::set<uint64_t> newEffectiveSynchVectors;
                std::set_intersection(effectiveSynchVectors.begin(), effectiveSynchVectors.end(), enabledSynchVectors.begin(), enabledSynchVectors.end(), std::inserter(newEffectiveSynchVectors, newEffectiveSynchVectors.begin()));
                effectiveSynchVectors = std::move(newEffectiveSynchVectors);
            }

            // Finally check whether it's a non-standard parallel composition. We do that by first constructing a set of
            // all effective synchronization vectors and then checking whether this set is fully contained within the
            // set of expected synchronization vectors.
            
            std::set<storm::jani::SynchronizationVector, storm::jani::SynchronizationVectorLexicographicalLess> synchVectorSet;
            for (auto synchVectorIndex : effectiveSynchVectors) {
                synchVectorSet.insert(composition.getSynchronizationVector(synchVectorIndex));
            }

            // Construct the set of expected synchronization vectors.
            std::set<storm::jani::SynchronizationVector, storm::jani::SynchronizationVectorLexicographicalLess> expectedSynchVectorSetUnderApprox;
            std::set<storm::jani::SynchronizationVector, storm::jani::SynchronizationVectorLexicographicalLess> expectedSynchVectorSetOverApprox;
            for (auto action : nonsilentSubActions) {
                std::vector<std::string> input;
                uint64_t numberOfParticipatingAutomata = 0;
                for (auto const& subcomposition : subinformation) {
                    if (subcomposition.getNonsilentActions().find(action) != subcomposition.getNonsilentActions().end()) {
                        input.push_back(action);
                        ++numberOfParticipatingAutomata;
                    } else {
                        input.push_back(SynchronizationVector::getNoActionInput());
                    }
                }
                
                storm::jani::SynchronizationVector newSynchVector(input, action);
                expectedSynchVectorSetOverApprox.insert(newSynchVector);
                if (numberOfParticipatingAutomata > 1) {
                    expectedSynchVectorSetUnderApprox.insert(newSynchVector);
                }
            }
            
            containsNonStandardParallelComposition |= !std::includes(expectedSynchVectorSetOverApprox.begin(), expectedSynchVectorSetOverApprox.end(), synchVectorSet.begin(), synchVectorSet.end(), SynchronizationVectorLexicographicalLess());

            containsNonStandardParallelComposition |= !std::includes(synchVectorSet.begin(), synchVectorSet.end(), expectedSynchVectorSetUnderApprox.begin(), expectedSynchVectorSetUnderApprox.end(), SynchronizationVectorLexicographicalLess());

            return CompositionInformation(joinedAutomatonToMultiplicityMap, nonsilentActions, containsNonStandardParallelComposition);
        }
        
    }
}
