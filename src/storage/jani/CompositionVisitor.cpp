#include "src/storage/jani/CompositionVisitor.h"

#include "src/storage/jani/AutomatonComposition.h"
#include "src/storage/jani/RenameComposition.h"
#include "src/storage/jani/ParallelComposition.h"

namespace storm {
    namespace jani {
        
        boost::any CompositionVisitor::visit(AutomatonComposition const& composition, boost::any const& data) {
            return data;
        }
        
        boost::any CompositionVisitor::visit(RenameComposition const& composition, boost::any const& data) {
            return composition.getSubcomposition().accept(*this, data);
        }
        
        boost::any CompositionVisitor::visit(ParallelComposition const& composition, boost::any const& data) {
            return join(composition.getLeftSubcomposition().accept(*this, data), composition.getRightSubcomposition().accept(*this, data));
        }
        
        boost::any CompositionVisitor::join(boost::any const& first, boost::any const& second) {
            return first;
        }
        
    }
}
