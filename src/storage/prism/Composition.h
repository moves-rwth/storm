#ifndef STORM_STORAGE_PRISM_COMPOSITION_H_
#define STORM_STORAGE_PRISM_COMPOSITION_H_

#include <ostream>

namespace storm {
    namespace prism {
        class Composition {
        public:
            Composition() = default;
            
            friend std::ostream& operator<<(std::ostream& stream, Composition const& composition);
            
        protected:
            virtual void writeToStream(std::ostream& stream) const = 0;
            
        private:
            
        };
    }
}

#endif /* STORM_STORAGE_PRISM_COMPOSITION_H_ */
