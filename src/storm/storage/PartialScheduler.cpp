#include "storm/storage/PartialScheduler.h"
#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace storage {
        
        void PartialScheduler::setChoice(uint_fast64_t state, uint_fast64_t choice) {
            stateToChoiceMapping[state] = choice;
        }
        
        bool PartialScheduler::isChoiceDefined(uint_fast64_t state) const {
            return stateToChoiceMapping.find(state) != stateToChoiceMapping.end();
        }
        
        uint_fast64_t PartialScheduler::getChoice(uint_fast64_t state) const {
            auto stateChoicePair = stateToChoiceMapping.find(state);
            
            if (stateChoicePair == stateToChoiceMapping.end()) {
                throw storm::exceptions::InvalidArgumentException() << "Invalid call to PartialScheduler::getChoice: scheduler does not define a choice for state " << state << ".";
            }
            
            return stateChoicePair->second;
        }
        
        PartialScheduler::map_type::const_iterator PartialScheduler::begin() const {
            return stateToChoiceMapping.begin();
        }
        
        PartialScheduler::map_type::const_iterator PartialScheduler::end() const {
            return stateToChoiceMapping.end();
        }
        
        std::ostream& operator<<(std::ostream& out, PartialScheduler const& scheduler) {
            out << "partial scheduler (defined on " << scheduler.stateToChoiceMapping.size() << " states) [ ";
            uint_fast64_t remainingEntries = scheduler.stateToChoiceMapping.size();
            for (auto const& stateChoicePair : scheduler.stateToChoiceMapping) {
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
