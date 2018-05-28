#pragma once

namespace storm {
    namespace storage {
        class SchedulerClass {
        public:
            SchedulerClass();
            
            bool isDeterministic() const;
            bool isMemoryBounded() const;
            uint64_t getMemoryStates() const;
            
            
            SchedulerClass& setIsDeterministic(bool value);
            SchedulerClass& setMemoryStates(uint64_t value);
            SchedulerClass& unsetMemoryStates();

        private:
            bool deterministic;
            uint64_t memorystates;
        };
    }
}
