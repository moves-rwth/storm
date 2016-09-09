#include "src/storage/SymbolicModelDescription.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace storage {
        
        SymbolicModelDescription::SymbolicModelDescription(storm::jani::Model const& model) {
            // Intentionally left empty.
        }
        
        SymbolicModelDescription::SymbolicModelDescription(storm::prism::Program const& program) {
            // Intentionally left empty.
        }
                
        bool SymbolicModelDescription::isJaniModel() const {
            return modelDescription.which() == 0;
        }
        
        bool SymbolicModelDescription::isPrismProgram() const {
            return modelDescription.which() == 1;
        }
        
        storm::jani::Model const& SymbolicModelDescription::asJaniModel() const {
            STORM_LOG_THROW(isJaniModel(), storm::exceptions::InvalidOperationException, "Cannot retrieve JANI model, because the symbolic description has a different type.");
            return boost::get<storm::jani::Model>(modelDescription);
        }
        
        storm::prism::Program const& SymbolicModelDescription::asPrismProgram() const {
            STORM_LOG_THROW(isPrismProgram(), storm::exceptions::InvalidOperationException, "Cannot retrieve JANI model, because the symbolic description has a different type.");
            return boost::get<storm::prism::Program>(modelDescription);
        }
        
    }
}