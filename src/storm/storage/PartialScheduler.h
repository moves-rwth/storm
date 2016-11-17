#ifndef STORM_STORAGE_PARTIALSCHEDULER_H_
#define STORM_STORAGE_PARTIALSCHEDULER_H_

#include <unordered_map>
#include <ostream>

#include "src/storm/storage/Scheduler.h"

namespace storm {
    namespace storage {
        
        class PartialScheduler : public Scheduler {
        public:
            typedef std::unordered_map<uint_fast64_t, uint_fast64_t> map_type;
            
            void setChoice(uint_fast64_t state, uint_fast64_t choice) override;
            
            bool isChoiceDefined(uint_fast64_t state) const override;
            
            uint_fast64_t getChoice(uint_fast64_t state) const override;
            
            map_type::const_iterator begin() const;
            map_type::const_iterator end() const;
            
            friend std::ostream& operator<<(std::ostream& out, PartialScheduler const& scheduler);

        private:
            // A mapping from all states that have defined choices to their respective choices.
            map_type stateToChoiceMapping;
        };
    }
}

#endif /* STORM_STORAGE_PARTIALSCHEDULER_H_ */
