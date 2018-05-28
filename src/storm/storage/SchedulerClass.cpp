#include "storm/storage/SchedulerClass.h"

#include "storm/utility/macros.h"

namespace storm {
    namespace storage {
        SchedulerClass::SchedulerClass() : deterministic(false), memorystates(0) {
            // Intentionally left empty
        }
        
        bool SchedulerClass::isDeterministic() const {
            return deterministic;
        }
        
        bool SchedulerClass::isMemoryBounded() const {
            return memorystates > 0;
        }
        
        uint64_t SchedulerClass::getMemoryStates() const {
            STORM_LOG_ASSERT(isMemoryBounded(), "Tried to retrieve the number of memory states although it is not bounded.");
            return memorystates;
        }
        
        
        SchedulerClass& SchedulerClass::setIsDeterministic(bool value) {
            deterministic = value;
            return *this;
        }
        
        SchedulerClass& SchedulerClass::setMemoryStates(uint64_t value) {
            STORM_LOG_ASSERT(value > 0, "Can not set the number of memory states to zero.");
            memorystates = value;
            return *this;
        }
        
        SchedulerClass& SchedulerClass::unsetMemoryStates() {
            memorystates = 0;
            return *this;
        }
        
        
    }
}
