#pragma once

#include <string>

namespace storm {
    namespace jani {
        
        class Location {
        public:
            /*!
             * Creates a new location.
             */
            Location(std::string const& name);
            
            /*!
             * Retrieves the name of the location.
             */
            std::string const& getName() const;
            
        private:
            // The name of the location.
            std::string name;
        };
        
    }
}