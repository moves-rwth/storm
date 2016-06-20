#include "src/storage/prism/CompositionToJaniVisitor.h"
#include "src/storage/prism/Compositions.h"

#include "src/storage/jani/Compositions.h"
#include "src/storage/jani/Model.h"

namespace storm {
    namespace prism {
        
        std::shared_ptr<storm::jani::Composition> CompositionToJaniVisitor::toJani(Composition const& composition, storm::jani::Model const& model) {
            return boost::any_cast<std::shared_ptr<storm::jani::Composition>>(composition.accept(*this, model));
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
            std::set<std::string> allActions;
            for (auto const& action : model.getActions()) {
                allActions.insert(action.getName());
            }
            std::shared_ptr<storm::jani::Composition> result = std::make_shared<storm::jani::ParallelComposition>(leftSubcomposition, rightSubcomposition, allActions);
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