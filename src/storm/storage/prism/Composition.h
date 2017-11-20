#ifndef STORM_STORAGE_PRISM_COMPOSITION_H_
#define STORM_STORAGE_PRISM_COMPOSITION_H_

#include <ostream>

#include "storm/storage/prism/CompositionVisitor.h"

namespace storm {
    namespace prism {
        class Composition {
        public:
            Composition() = default;
            virtual ~Composition() = default;
            
            friend std::ostream& operator<<(std::ostream& stream, Composition const& composition);
            
            virtual boost::any accept(CompositionVisitor& visitor, boost::any const& data) const = 0;
            
        protected:
            virtual void writeToStream(std::ostream& stream) const = 0;
            
        private:
            
        };
    }
}

#endif /* STORM_STORAGE_PRISM_COMPOSITION_H_ */
