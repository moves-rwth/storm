#ifndef STORM_STORAGE_PRISM_PARALLELCOMPOSITION_H_
#define STORM_STORAGE_PRISM_PARALLELCOMPOSITION_H_

#include <set>
#include <string>

#include "src/storage/prism/Composition.h"

namespace storm {
    namespace prism {
        class ParallelComposition : Composition {
        public:
            ParallelComposition(std::shared_ptr<Composition> const& left, std::set<std::string> const& synchronizingActions, std::shared_ptr<Composition> const& right);
            
        protected:
            virtual void writeToStream(std::ostream& stream) const override;
            
        private:
            std::shared_ptr<Composition> left;
            std::set<std::string> synchronizingActions;
            std::shared_ptr<Composition> right;
        };
    }
}

#endif /* STORM_STORAGE_PRISM_PARALLELCOMPOSITION_H_ */
