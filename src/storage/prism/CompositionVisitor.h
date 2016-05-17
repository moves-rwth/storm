#ifndef STORM_STORAGE_PRISM_COMPOSITIONVISITOR_H_
#define STORM_STORAGE_PRISM_COMPOSITIONVISITOR_H_

#include <boost/any.hpp>

namespace storm {
    namespace prism {
        
        class ModuleComposition;
        class RenamingComposition;
        class ParallelComposition;
        
        class CompositionVisitor {
        public:
            virtual boost::any visit(ModuleComposition const& composition) = 0;
            virtual boost::any visit(RenamingComposition const& composition) = 0;
            virtual boost::any visit(ParallelComposition const& composition) = 0;
        };
    }
}

#endif /* STORM_STORAGE_PRISM_COMPOSITIONVISITOR_H_ */