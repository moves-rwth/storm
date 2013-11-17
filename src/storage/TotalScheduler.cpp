#include "src/storage/TotalScheduler.h"

namespace storm {
    namespace storage {
        TotalScheduler::TotalScheduler(uint_fast64_t numberOfStates) : choices(numberOfStates) {
            // Intentionally left empty.
        }
        
        void setChoice(uint_fast64_t state, uint_fast64_t choice) override {
            choices[state] = choice;
        }
        
        bool isChoiceDefined(uint_fast64_t state) const override {
            return true;
        }
        
        uint_fast64_t getChoice(uint_fast64_t state) const override {
            return choices[state];
        }
    }
}