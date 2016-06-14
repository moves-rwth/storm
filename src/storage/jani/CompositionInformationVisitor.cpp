#include "src/storage/jani/CompositionInformationVisitor.h"

#include "src/storage/jani/Model.h"
#include "src/storage/jani/Compositions.h"

namespace storm {
    namespace jani {
        
        CompositionInformation::CompositionInformation(std::map<std::string, uint64_t> const& automatonNameToMultiplicity, std::set<std::string> const& nonsilentActions, bool containsRenameComposition, bool containsRestrictedParallelComposition) : automatonNameToMultiplicity(automatonNameToMultiplicity), nonsilentActions(nonsilentActions), renameComposition(containsRenameComposition), restrictedParallelComposition(containsRestrictedParallelComposition) {
            // Intentionally left empty.
        }
        
        void CompositionInformation::increaseAutomatonMultiplicity(std::string const& automatonName, uint64_t count) {
            automatonNameToMultiplicity[automatonName] += count;
        }
        
        void CompositionInformation::addNonsilentAction(std::string const& actionName) {
            // FIXME
        }
        
        std::set<std::string> const& CompositionInformation::getNonsilentActions() const {
            // FIXME
        }
        
        void CompositionInformation::renameNonsilentActions(std::map<std::string, std::string> const& renaming) {
            // FIXME
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
            CompositionInformation result;
            result.increaseAutomatonMultiplicity(composition.getAutomatonName());
            return result;
        }
        
        boost::any CompositionInformationVisitor::visit(RenameComposition const& composition, boost::any const& data) {
            CompositionInformation subresult = boost::any_cast<CompositionInformation>(composition.getSubcomposition().accept(*this, data));
            subresult.setContainsRenameComposition();
            return subresult;
        }
        
        boost::any CompositionInformationVisitor::visit(ParallelComposition const& composition, boost::any const& data) {
            Model const& model = boost::any_cast<Model const&>(data);
            CompositionInformation left = boost::any_cast<CompositionInformation>(composition.getLeftSubcomposition().accept(*this, data));
            CompositionInformation right = boost::any_cast<CompositionInformation>(composition.getRightSubcomposition().accept(*this, data));

            // Join the information from both sides.
            bool containsRenameComposition = left.containsRenameComposition() || right.containsRenameComposition();
            bool containsRestrictedParallelComposition = left.containsRestrictedParallelComposition() || right.containsRestrictedParallelComposition();
            std::map<std::string, uint64_t> joinedAutomatonToMultiplicity = CompositionInformation::joinMultiplicityMaps(left.getAutomatonToMultiplicityMap(), right.getAutomatonToMultiplicityMap());
            
            // If there was no restricted parallel composition yet, maybe the current composition is one, so check it.
            if (!containsRestrictedParallelComposition) {
                // FIXME.
            }
            
            return CompositionInformation(joinedAutomatonToMultiplicity, containsRenameComposition, containsRestrictedParallelComposition);
        }
        
    }
}