#ifndef STORM_STORAGE_PRISM_RESTRICTEDPARALLELCOMPOSITION_H_
#define STORM_STORAGE_PRISM_RESTRICTEDPARALLELCOMPOSITION_H_

#include <set>
#include <string>

#include "src/storage/prism/ParallelComposition.h"

namespace storm {
    namespace prism {
        class RestrictedParallelComposition : public ParallelComposition {
        public:
            RestrictedParallelComposition(std::shared_ptr<Composition> const& left, std::set<std::string> const& synchronizingActions, std::shared_ptr<Composition> const& right);
            
            virtual boost::any accept(CompositionVisitor& visitor) const override;

        protected:
            virtual void writeToStream(std::ostream& stream) const override;
            
        private:
            std::shared_ptr<Composition> left;
            std::set<std::string> synchronizingActions;
            std::shared_ptr<Composition> right;
        };
    }
}

#endif /* STORM_STORAGE_PRISM_RESTRICTEDPARALLELCOMPOSITION_H_ */
