#ifndef STORM_STORAGE_PARTIALSCHEDULER_H_
#define STORM_STORAGE_PARTIALSCHEDULER_H_

#include <unordered_map>
#include <ostream>

#include "src/storage/Scheduler.h"

namespace storm {
    namespace storage {
        
        class PartialScheduler : public Scheduler {
        public:
            void setChoice(uint_fast64_t state, uint_fast64_t choice) override;
            
            bool isChoiceDefined(uint_fast64_t state) const override;
            
            uint_fast64_t getChoice(uint_fast64_t state) const override;
            
            friend std::ostream& operator<<(std::ostream& out, PartialScheduler const& scheduler);

        private:
            // A mapping from all states that have defined choices to their respective choices.
            std::unordered_map<uint_fast64_t, uint_fast64_t> choices;
        };
    }
}

#endif /* STORM_STORAGE_PARTIALSCHEDULER_H_ */
