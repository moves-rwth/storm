#include "src/storage/jani/Location.h"
#include "src/storage/jani/Assignment.h"
#include "src/exceptions/InvalidJaniException.h"
#include "src/utility/macros.h"

namespace storm {
    namespace jani {
        
        Location::Location(std::string const& name, std::vector<Assignment> const& transientAssignments) : name(name), transientAssignments(transientAssignments) {
            // Intentionally left empty.
        }
        
        std::string const& Location::getName() const {
            return name;
        }
        
        std::vector<Assignment> const& Location::getTransientAssignments() const {
            return transientAssignments;
        }
        
        void Location::checkValid() const {
            for(auto const& assignment : transientAssignments) {
                STORM_LOG_THROW(assignment.isTransientAssignment(), storm::exceptions::InvalidJaniException, "Only transient assignments are allowed in locations.");
            }
        }
        
    }
}