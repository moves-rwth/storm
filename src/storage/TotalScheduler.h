#ifndef STORM_STORAGE_TOTALSCHEDULER_H_
#define STORM_STORAGE_TOTALSCHEDULER_H_

#include <vector>
#include <ostream>

#include "src/storage/Scheduler.h"

namespace storm {
    namespace storage {
        
        class TotalScheduler : public Scheduler {
        public:
            /*!
             * Creates a total scheduler that defines a choice for the given number of states. By default, all choices
             * are initialized to zero.
             *
             * @param numberOfStates The number of states for which the scheduler defines a choice.
             */
            TotalScheduler(uint_fast64_t numberOfStates = 0);
            
            /*!
             * Creates a total scheduler that defines the choices for states according to the given vector.
             *
             * @param choices A vector whose i-th entry defines the choice of state i.
             */
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
