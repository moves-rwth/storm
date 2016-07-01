#include "src/storage/jani/CompositionInformationVisitor.h"

#include "src/storage/jani/Model.h"
#include "src/storage/jani/Compositions.h"

namespace storm {
    namespace jani {
        
        CompositionInformation::CompositionInformation() : automatonNameToMultiplicity(), nonsilentActions(), renameComposition(false), restrictedParallelComposition(false) {
            // Intentionally left empty.
        }
        
        CompositionInformation::CompositionInformation(std::map<std::string, uint64_t> const& automatonNameToMultiplicity, std::set<std::string> const& nonsilentActions, bool containsRenameComposition, bool containsRestrictedParallelComposition) : automatonNameToMultiplicity(automatonNameToMultiplicity), nonsilentActions(nonsilentActions), renameComposition(containsRenameComposition), restrictedParallelComposition(containsRestrictedParallelComposition) {
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
        
        void CompositionInformation::setContainsRestrictedParallelComposition() {
            restrictedParallelComposition = true;
        }
        
        bool CompositionInformation::containsRestrictedParallelComposition() const {
            return restrictedParallelComposition;
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
            return CompositionInformation(subresult.getAutomatonToMultiplicityMap(), nonsilentActions, true, subresult.containsRestrictedParallelComposition());
        }
        
        boost::any CompositionInformationVisitor::visit(ParallelComposition const& composition, boost::any const& data) {
            CompositionInformation left = boost::any_cast<CompositionInformation>(composition.getLeftSubcomposition().accept(*this, data));
            CompositionInformation right = boost::any_cast<CompositionInformation>(composition.getRightSubcomposition().accept(*this, data));

            // Join the information from both sides.
            bool containsRenameComposition = left.containsRenameComposition() || right.containsRenameComposition();
            bool containsRestrictedParallelComposition = left.containsRestrictedParallelComposition() || right.containsRestrictedParallelComposition();
            std::map<std::string, uint64_t> joinedAutomatonToMultiplicity = CompositionInformation::joinMultiplicityMaps(left.getAutomatonToMultiplicityMap(), right.getAutomatonToMultiplicityMap());
            
            std::set<std::string> nonsilentActions;
            std::set_union(left.getNonsilentActions().begin(), left.getNonsilentActions().end(), right.getNonsilentActions().begin(), right.getNonsilentActions().end(), std::inserter(nonsilentActions, nonsilentActions.begin()));
            
            // If there was no restricted parallel composition yet, maybe the current composition is one, so check it.
            if (!containsRestrictedParallelComposition) {
                std::set<std::string> commonNonsilentActions;
                std::set_intersection(left.getNonsilentActions().begin(), left.getNonsilentActions().end(), right.getNonsilentActions().begin(), right.getNonsilentActions().end(), std::inserter(commonNonsilentActions, commonNonsilentActions.begin()));
                bool allCommonActionsIncluded = std::includes(commonNonsilentActions.begin(), commonNonsilentActions.end(), composition.getSynchronizationAlphabet().begin(), composition.getSynchronizationAlphabet().end());
                containsRestrictedParallelComposition = !allCommonActionsIncluded;
            }
            
            return CompositionInformation(joinedAutomatonToMultiplicity, nonsilentActions, containsRenameComposition, containsRestrictedParallelComposition);
        }
        
    }
}