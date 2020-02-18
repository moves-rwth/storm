#include "storm/utility/Portfolio.h"

#include "storm/storage/SymbolicModelDescription.h"
#include "storm/storage/jani/Property.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace utility {
        Portfolio::Portfolio() : engine(storm::utility::Engine::Unknown), bisimulation(false), exact(false) {
            // Intentionally left empty
        }
        
        void Portfolio::predict(storm::storage::SymbolicModelDescription const& modelDescription, storm::jani::Property const& property) {
        
        }
        
        void Portfolio::predict(storm::storage::SymbolicModelDescription const& modelDescription, storm::jani::Property const& property, uint64_t stateEstimate) {
        
        }

        storm::utility::Engine Portfolio::getEngine() const {
            STORM_LOG_THROW(engine != storm::utility::Engine::Unknown, storm::exceptions::InvalidOperationException, "Tried to get the engine but apparently no prediction was done before.");
            return engine;
        }
        
        bool Portfolio::enableBisimulation() const {
            return bisimulation;
        }
        
        bool Portfolio::enableExact() const {
            return exact;
        }
        
    }
}