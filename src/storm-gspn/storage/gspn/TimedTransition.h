#pragma once

#include "storm-gspn/storage/gspn/Transition.h"

namespace storm {
    namespace gspn {
        template <typename RateType>
        class TimedTransition : public storm::gspn::Transition {
        public:
            /*!
             * Sets the rate of this transition to the given value.
             *
             * @param rate The new rate for this transition.
             */
            void setRate(RateType const& rate) {
                this->rate = rate;
            }

            /*!
             * Retrieves the rate of this transition.
             *
             * @return The rate of this transition.
             */
            RateType getRate() const {
                return this->rate;
            }

        private:
            // the rate of the transition
            RateType rate;
        };
    }
}