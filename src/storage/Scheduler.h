#ifndef STORM_STORAGE_SCHEDULER_H_
#define STORM_STORAGE_SCHEDULER_H_

#include <cstdint>

namespace storm {
    namespace storage {
        
        /*
         * This class is the abstract base class of all scheduler classes. Scheduler classes define which action is
         * chosen in a particular state of a non-deterministic model. More concretely, a scheduler maps a state s to i
         * if the scheduler takes the i-th action available in s (i.e. the choices are relative to the states).
         */
        class Scheduler {
        public:
            /*
             * Sets the choice defined by the scheduler for the given state.
             *
             * @param state The state for which to set the choice.
             * @param choice The choice to set for the given state.
             */
            virtual void setChoice(uint_fast64_t state, uint_fast64_t choice) = 0;
            
            /*
             * Retrieves whether this scheduler defines a choice for the given state.
             *
             * @param state The state for which to check whether the scheduler defines a choice.
             * @return True if the scheduler defines a choice for the given state.
             */
            virtual bool isChoiceDefined(uint_fast64_t state) const = 0;
            
            /*!
             * Retrieves the choice for the given state under the assumption that the scheduler defines a proper choice for the state.
             */
            virtual uint_fast64_t getChoice(uint_fast64_t state) const = 0;
        };
    }
}

#endif /* STORM_STORAGE_SCHEDULER_H_ */
