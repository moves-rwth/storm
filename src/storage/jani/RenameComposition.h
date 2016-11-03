#pragma once

#include <memory>
#include <map>

#include <boost/optional.hpp>

#include "src/storage/jani/Composition.h"

namespace storm {
    namespace jani {
        
        class RenameComposition : public Composition {
        public:
            RenameComposition(std::shared_ptr<Composition> const& subcomposition, std::map<std::string, boost::optional<std::string>> const& renaming = {});
            
            /*!
             * Retrieves the subcomposition of this rename composition.
             */
            Composition const& getSubcomposition() const;
            
            /*!
             * Retrieves the renaming of actions. If some action name is mapped to none, this indicates the action is to
             * be renamed to the silent action.
             */
            std::map<std::string, boost::optional<std::string>> const& getRenaming() const;
            
            virtual boost::any accept(CompositionVisitor& visitor, boost::any const& data) const override;
            
            virtual void write(std::ostream& stream) const override;
            
        private:
            // The subcomposition.
            std::shared_ptr<Composition> subcomposition;
            
            // The renaming to perform. If one cation is mapped to none, this means that the action is to be renamed to
            // the silent action.
            std::map<std::string, boost::optional<std::string>> renaming;
        };
        
    }
}