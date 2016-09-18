#pragma once

#include <ostream>

#include "src/storage/jani/CompositionVisitor.h"

namespace storm {
    namespace jani {
        
        class Composition {
        public:
            virtual bool isAutomaton() const { return false; }
            
            virtual boost::any accept(CompositionVisitor& visitor, boost::any const& data) const = 0;
            
            virtual void write(std::ostream& stream) const = 0;
            
            friend std::ostream& operator<<(std::ostream& stream, Composition const& composition);
        };
        
    }
}