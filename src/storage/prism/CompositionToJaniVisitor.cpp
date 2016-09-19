#include "src/storage/prism/CompositionToJaniVisitor.h"
#include "src/storage/prism/Compositions.h"

#include "src/storage/jani/Compositions.h"
#include "src/storage/jani/CompositionActionInformationVisitor.h"
#include "src/storage/jani/Model.h"

namespace storm {
    namespace prism {
        
        std::shared_ptr<storm::jani::Composition> CompositionToJaniVisitor::toJani(Composition const& composition, storm::jani::Model const& model) {
            auto result = boost::any_cast<std::shared_ptr<storm::jani::Composition>>(composition.accept(*this, model));
            std::cout << "got composition " << *result << std::endl;
            return result;
        }
        
        boost::any CompositionToJaniVisitor::visit(ModuleComposition const& composition, boost::any const& data) {
            std::shared_ptr<storm::jani::Composition> result = std::make_shared<storm::jani::AutomatonComposition>(composition.getModuleName());
            return result;
        }
        
        boost::any CompositionToJaniVisitor::visit(RenamingComposition const& composition, boost::any const& data) {
            std::map<std::string, boost::optional<std::string>> newRenaming;
            for (auto const& renamingPair : composition.getActionRenaming()) {
                newRenaming.emplace(renamingPair.first, renamingPair.second);
            }
            auto subcomposition = boost::any_cast<std::shared_ptr<storm::jani::Composition>>(composition.getSubcomposition().accept(*this, data));
            std::shared_ptr<storm::jani::Composition> result = std::make_shared<storm::jani::RenameComposition>(subcomposition, newRenaming);
            return result;
        }
        
        boost::any CompositionToJaniVisitor::visit(HidingComposition const& composition, boost::any const& data) {
            std::map<std::string, boost::optional<std::string>> newRenaming;
            for (auto const& action : composition.getActionsToHide()) {
                newRenaming.emplace(action, boost::none);
            }
            auto subcomposition = boost::any_cast<std::shared_ptr<storm::jani::Composition>>(composition.getSubcomposition().accept(*this, data));
            std::shared_ptr<storm::jani::Composition> result = std::make_shared<storm::jani::RenameComposition>(subcomposition, newRenaming);
            return result;
        }
        
        boost::any CompositionToJaniVisitor::visit(SynchronizingParallelComposition const& composition, boost::any const& data) {
            auto leftSubcomposition = boost::any_cast<std::shared_ptr<storm::jani::Composition>>(composition.getLeftSubcomposition().accept(*this, data));
            auto rightSubcomposition = boost::any_cast<std::shared_ptr<storm::jani::Composition>>(composition.getRightSubcomposition().accept(*this, data));
            
            storm::jani::Model const& model = boost::any_cast<storm::jani::Model const&>(data);
            storm::jani::CompositionActionInformationVisitor visitor(model);
            storm::jani::ActionInformation leftActionInformation = visitor.getActionInformation(*leftSubcomposition);
            storm::jani::ActionInformation rightActionInformation = visitor.getActionInformation(*rightSubcomposition);
            
            std::set<std::string> leftActions;
            for (auto const& actionIndex : leftActionInformation.getNonSilentActionIndices()) {
                leftActions.insert(leftActionInformation.getActionName(actionIndex));
            }
            std::set<std::string> rightActions;
            for (auto const& actionIndex : rightActionInformation.getNonSilentActionIndices()) {
                rightActions.insert(rightActionInformation.getActionName(actionIndex));
            }
            
            std::set<std::string> commonActions;
            std::set_intersection(leftActions.begin(), leftActions.end(), rightActions.begin(), rightActions.end(), std::inserter(commonActions, commonActions.begin()));

            std::shared_ptr<storm::jani::Composition> result = std::make_shared<storm::jani::ParallelComposition>(leftSubcomposition, rightSubcomposition, commonActions);
            return result;
        }
        
        boost::any CompositionToJaniVisitor::visit(InterleavingParallelComposition const& composition, boost::any const& data) {
            auto leftSubcomposition = boost::any_cast<std::shared_ptr<storm::jani::Composition>>(composition.getLeftSubcomposition().accept(*this, data));
            auto rightSubcomposition = boost::any_cast<std::shared_ptr<storm::jani::Composition>>(composition.getRightSubcomposition().accept(*this, data));
            std::shared_ptr<storm::jani::Composition> result = std::make_shared<storm::jani::ParallelComposition>(leftSubcomposition, rightSubcomposition, std::set<std::string>());
            return result;
        }
        
        boost::any CompositionToJaniVisitor::visit(RestrictedParallelComposition const& composition, boost::any const& data) {
            auto leftSubcomposition = boost::any_cast<std::shared_ptr<storm::jani::Composition>>(composition.getLeftSubcomposition().accept(*this, data));
            auto rightSubcomposition = boost::any_cast<std::shared_ptr<storm::jani::Composition>>(composition.getRightSubcomposition().accept(*this, data));
            std::shared_ptr<storm::jani::Composition> result = std::make_shared<storm::jani::ParallelComposition>(leftSubcomposition, rightSubcomposition, composition.getSynchronizingActions());
            return result;
        }
    }
}
