#ifndef STORM_STORAGE_PRISM_HIDINGCOMPOSITION_H_
#define STORM_STORAGE_PRISM_HIDINGCOMPOSITION_H_

#include <set>
#include <string>

#include "src/storage/prism/Composition.h"

namespace storm {
    namespace prism {
        class HidingComposition : public Composition {
        public:
            HidingComposition(std::shared_ptr<Composition> const& sub, std::set<std::string> const& actionsToHide);
            
            virtual boost::any accept(CompositionVisitor& visitor) const override;
            
            Composition const& getSubcomposition() const;
            
        protected:
            virtual void writeToStream(std::ostream& stream) const override;
            
        private:
            std::shared_ptr<Composition> sub;
            
            // The actions to hide.
            std::set<std::string> actionsToHide;
        };
    }
}

#endif /* STORM_STORAGE_PRISM_HIDINGCOMPOSITION_H_ */
