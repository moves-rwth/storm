#include "src/storage/jani/Location.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidJaniException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace jani {
        
        Location::Location(std::string const& name, std::vector<Assignment> const& transientAssignments) : name(name), transientAssignments(transientAssignments) {
            // Intentionally left empty.
        }
        
        std::string const& Location::getName() const {
            return name;
        }
        
        OrderedAssignments const& Location::getTransientAssignments() const {
            return transientAssignments;
        }
        
        void Location::addTransientAssignment(storm::jani::Assignment const& assignment) {
            STORM_LOG_THROW(assignment.isTransient(), storm::exceptions::InvalidArgumentException, "Must not add non-transient assignment to location.");
            transientAssignments.add(assignment);
        }
        
        void Location::checkValid() const {
            // Intentionally left empty.
        }
        
    }
}