#include "initialize.h"

#include "src/utility/macros.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/DebugSettings.h"

int storm_runtime_loglevel = STORM_LOGLEVEL_WARN;

namespace storm {
    namespace utility {
        
        void initializeLogger() {
            // Intentionally left empty.
        }
        
        void setUp() {
            initializeLogger();
            std::cout.precision(10);
        }
        
        void cleanUp() {
            // Intentionally left empty.
        }
        
        void initializeFileLogging() {
            // FIXME.
        }
        
    }
}
