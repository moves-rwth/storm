#ifndef STORM_STORAGE_TOTALSCHEDULER_H_
#define STORM_STORAGE_TOTALSCHEDULER_H_

#include <vector>
#include <ostream>

#include "storm/storage/Scheduler.h"
#include "storm/storage/BitVector.h"

namespace storm {
    namespace storage {
        
        class TotalScheduler : public Scheduler {
        public:
            
            TotalScheduler(TotalScheduler const& other) = default;
            TotalScheduler(TotalScheduler&& other) = default;
            TotalScheduler& operator=(TotalScheduler const& other) = default;
            TotalScheduler& operator=(TotalScheduler&& other) = default;
            
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

            /*!
             * Creates a total scheduler that defines the choices for states according to the given vector.
             *
             * @param choices A vector whose i-th entry defines the choice of state i.
             */
            TotalScheduler(std::vector<uint_fast64_t>&& choices);
            
            bool operator==(TotalScheduler const& other) const;

            void setChoice(uint_fast64_t state, uint_fast64_t choice) override;
            
            bool isChoiceDefined(uint_fast64_t state) const override;
            
            uint_fast64_t getChoice(uint_fast64_t state) const override;
            
            std::vector<uint_fast64_t> const& getChoices() const;
            
            /*!
             * Constructs the scheduler for the subsystem indicated by the given BitVector
             *
             * @param subsystem A BitVector whose i-th entry is true iff state i is part of the subsystem
             */
            TotalScheduler getSchedulerForSubsystem(storm::storage::BitVector const& subsystem) const;
            
            friend std::ostream& operator<<(std::ostream& out, TotalScheduler const& scheduler);
            friend struct std::hash<storm::storage::TotalScheduler>;
            
        private:
            // A vector that stores the choice for each state.
            std::vector<uint_fast64_t> choices;
        };
    } // namespace storage
} // namespace storm


namespace std {
    template <>
    struct hash<storm::storage::TotalScheduler> {
        std::size_t operator()(storm::storage::TotalScheduler const& totalScheduler) const;
    };
}

#endif /* STORM_STORAGE_TOTALSCHEDULER_H_ */
