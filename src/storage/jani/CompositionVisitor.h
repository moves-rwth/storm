#pragma once

#include <boost/any.hpp>

namespace storm {
    namespace jani {
        
        class Composition;
        class AutomatonComposition;
        class RenameComposition;
        class ParallelComposition;
        
        class CompositionVisitor {
        public:
            virtual boost::any visit(AutomatonComposition const& composition, boost::any const& data) = 0;
            virtual boost::any visit(RenameComposition const& composition, boost::any const& data) = 0;
            virtual boost::any visit(ParallelComposition const& composition, boost::any const& data) = 0;
        };
        
    }
}