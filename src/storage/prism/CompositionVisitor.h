#ifndef STORM_STORAGE_PRISM_COMPOSITIONVISITOR_H_
#define STORM_STORAGE_PRISM_COMPOSITIONVISITOR_H_

#include <boost/any.hpp>

namespace storm {
    namespace prism {
        
        class ModuleComposition;
        class RenamingComposition;
        class HidingComposition;
        class SynchronizingParallelComposition;
        class InterleavingParallelComposition;
        class RestrictedParallelComposition;
        
        class CompositionVisitor {
        public:
            virtual boost::any visit(ModuleComposition const& composition) = 0;
            virtual boost::any visit(RenamingComposition const& composition) = 0;
            virtual boost::any visit(HidingComposition const& composition) = 0;
            virtual boost::any visit(SynchronizingParallelComposition const& composition) = 0;
            virtual boost::any visit(InterleavingParallelComposition const& composition) = 0;
            virtual boost::any visit(RestrictedParallelComposition const& composition) = 0;
        };
    }
}

#endif /* STORM_STORAGE_PRISM_COMPOSITIONVISITOR_H_ */