#include "storm/storage/SchedulerClass.h"

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
            assert(isMemoryBounded());
        }
        
        
        SchedulerClass& SchedulerClass::setIsDeterministic(bool value) {
            deterministic = value;
            return *this;
        }
        
        SchedulerClass& SchedulerClass::setMemoryStates(uint64_t value) {
            assert(value > 0);
            memorystates = value;
            return *this;
        }
        
        SchedulerClass& SchedulerClass::unsetMemoryStates() {
            memorystates = 0;
            return *this;
        }
        
        
    }
}
