#pragma once


#include <boost/any.hpp>

#include "storm/storage/jani/traverser/JaniTraverser.h"

namespace storm {
    namespace jani {
        class ArrayEliminator {
        public:
            ArrayEliminator() = default;
            
            void eliminate(Model& model);

        private:
        
        
        };
    }
}

