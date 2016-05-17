#ifndef STORM_STORAGE_PRISM_RENAMINGCOMPOSITION_H_
#define STORM_STORAGE_PRISM_RENAMINGCOMPOSITION_H_

#include <string>
#include <map>
#include <boost/optional.hpp>

#include "src/storage/prism/Composition.h"

namespace storm {
    namespace prism {
        class RenamingComposition : Composition {
        public:
            RenamingComposition(std::shared_ptr<Composition> const& left, std::map<std::string, boost::optional<std::string>> const& actionRenaming);
            
        protected:
            virtual void writeToStream(std::ostream& stream) const override;
            
        private:
            std::shared_ptr<Composition> left;
            
            // The renaming of action indices to apply. If the target name is none, the action is hidden.
            std::map<std::string, boost::optional<std::string>> actionRenaming;
        };
    }
}

#endif /* STORM_STORAGE_PRISM_RENAMINGCOMPOSITION_H_ */
