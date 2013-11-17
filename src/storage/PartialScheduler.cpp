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
            
            return stateChoicePair->second;
        }
        
        std::ostream& operator<<(std::ostream& out, PartialScheduler const& scheduler) {
            out << "partial scheduler (defined on " << scheduler.choices.size() << " states) [ ";
            uint_fast64_t remainingEntries = scheduler.choices.size();
            for (auto stateChoicePair : scheduler.choices) {
                out << stateChoicePair.first << " -> " << stateChoicePair.second;
                --remainingEntries;
                if (remainingEntries > 0) {
                    out << ", ";
                }
            }
            out << "]";
            return out;
        }
        
    } // namespace storage
} // namespace storm