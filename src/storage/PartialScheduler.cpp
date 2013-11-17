#include "src/storage/PartialScheduler.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace storage {
        
        void PartialScheduler::setChoice(uint_fast64_t state, uint_fast64_t choice) {
            choices[state] = choice;
        }
        
        bool PartialScheduler::isChoiceDefined(uint_fast64_t state) const {
            return choices.find(state) != choices.end();
        }
        
        uint_fast64_t PartialScheduler::getChoice(uint_fast64_t state) const {
            auto stateChoicePair = choices.find(state);
            
            if (stateChoicePair == choices.end()) {
                throw storm::exceptions::InvalidArgumentException() << "Scheduler does not define a choice for state " << state;
            }
        }
        
    } // namespace storage
} // namespace storm