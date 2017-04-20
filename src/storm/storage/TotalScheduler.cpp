#include "storm/storage/TotalScheduler.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/Hash.h"
#include "storm/utility/vector.h"

namespace storm {
    namespace storage {
        TotalScheduler::TotalScheduler(uint_fast64_t numberOfStates) : choices(numberOfStates) {
            // Intentionally left empty.
        }
        
        TotalScheduler::TotalScheduler(std::vector<uint_fast64_t> const& choices) : choices(choices) {
            // Intentionally left empty.
        }
        
        TotalScheduler::TotalScheduler(std::vector<uint_fast64_t>&& choices) : choices(std::move(choices)) {
            // Intentionally left empty.
        }
        
        bool TotalScheduler::operator==(storm::storage::TotalScheduler const& other) const {
            return this->choices == other.choices;
        }
        
        void TotalScheduler::setChoice(uint_fast64_t state, uint_fast64_t choice) {
            if (state > choices.size()) {
                throw storm::exceptions::InvalidArgumentException() << "Invalid call to TotalScheduler::setChoice: scheduler cannot not define a choice for state " << state << ".";
            }
            choices[state] = choice;
        }
        
        bool TotalScheduler::isChoiceDefined(uint_fast64_t state) const {
            return state < choices.size();
        }
        
        uint_fast64_t TotalScheduler::getChoice(uint_fast64_t state) const {
            if (state >= choices.size()) {
                throw storm::exceptions::InvalidArgumentException() << "Invalid call to TotalScheduler::getChoice: scheduler does not define a choice for state " << state << ".";
            }

            return choices[state];
        }
        
        std::vector<uint_fast64_t> const& TotalScheduler::getChoices() const {
            return choices;
        }
        
        TotalScheduler TotalScheduler::getSchedulerForSubsystem(storm::storage::BitVector const& subsystem) const {
            return TotalScheduler(storm::utility::vector::filterVector(choices, subsystem));
        }
        
        std::ostream& operator<<(std::ostream& out, TotalScheduler const& scheduler) {
            out << "total scheduler (defined on " << scheduler.choices.size() << " states) [ ";
            for (uint_fast64_t state = 0; state < scheduler.choices.size() - 1; ++state) {
                out << state << " -> " << scheduler.choices[state] << ", ";
            }
            if (scheduler.choices.size() > 0) {
                out << (scheduler.choices.size() - 1) << " -> " << scheduler.choices[scheduler.choices.size() - 1] << " ]";
            }
            return out;
        }
    }
}

namespace  std {
        std::size_t hash<storm::storage::TotalScheduler>::operator()(storm::storage::TotalScheduler const& totalScheduler) const {
            return storm::utility::Hash<uint_fast64_t>::getHash(totalScheduler.choices);
        }
}
