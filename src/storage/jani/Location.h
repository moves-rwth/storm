#pragma once

#include <string>
#include <vector>
#include "src/storage/jani/Assignment.h"

namespace storm {
    namespace jani {
        
        /**
         * Jani Location:
         * 
         * Whereas Jani Locations also support invariants, we do not have support for them (as we do not support any of the allowed model types)
         */
        class Location {
        public:
            /*!
             * Creates a new location.
             */
            Location(std::string const& name, std::vector<Assignment> const& transientAssignments = {});
            
            /*!
             * Retrieves the name of the location.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves the transient assignments of this location.
             */
            std::vector<Assignment> const& getTransientAssignments() const;
            
            /*!
             * Checks whether the location is valid, that is, whether the assignments are indeed all transient assignments.
             */
            void checkValid() const;
            
        private:
            /// The name of the location.
            std::string name;
            
            /// The transient assignments made in this location.
            std::vector<Assignment> transientAssignments;
        };
        
    }
}