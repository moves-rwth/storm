#ifndef STORM_STORAGE_PRISM_SYNCHRONIZINGPARALLELCOMPOSITION_H_
#define STORM_STORAGE_PRISM_SYNCHRONIZINGPARALLELCOMPOSITION_H_

#include "src/storage/prism/ParallelComposition.h"

namespace storm {
    namespace prism {
        class SynchronizingParallelComposition : public ParallelComposition {
        public:
            SynchronizingParallelComposition(std::shared_ptr<Composition> const& left, std::shared_ptr<Composition> const& right);
            
            virtual boost::any accept(CompositionVisitor& visitor) const override;
            
        protected:
            virtual void writeToStream(std::ostream& stream) const override;
            
        private:
            std::shared_ptr<Composition> left;
            std::shared_ptr<Composition> right;
        };
    }
}

#endif /* STORM_STORAGE_PRISM_SYNCHRONIZINGPARALLELCOMPOSITION_H_ */
