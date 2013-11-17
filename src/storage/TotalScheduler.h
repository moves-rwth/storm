#ifndef STORM_STORAGE_TOTALSCHEDULER_H_
#define STORM_STORAGE_TOTALSCHEDULER_H_

#include <vector>
#include <ostream>

#include "src/storage/Scheduler.h"

namespace storm {
    namespace storage {
        
        class TotalScheduler : public Scheduler {
        public:
            TotalScheduler(uint_fast64_t numberOfStates);
            
            TotalScheduler(std::vector<uint_fast64_t> const& choices);
            
            void setChoice(uint_fast64_t state, uint_fast64_t choice) override;
            
            bool isChoiceDefined(uint_fast64_t state) const override;
            
            uint_fast64_t getChoice(uint_fast64_t state) const override;
            
            friend std::ostream& operator<<(std::ostream& out, TotalScheduler const& scheduler);
            
        private:
            // A vector that stores the choice for each state.
            std::vector<uint_fast64_t> choices;
        };
    } // namespace storage
} // namespace storm

#endif /* STORM_STORAGE_TOTALSCHEDULER_H_ */
