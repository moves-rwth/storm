#ifndef STORM_IMMEDIATETRANSITION_H
#define STORM_IMMEDIATETRANSITION_H

#include "src/storage/gspn/Transition.h"

namespace storm {
    namespace gspn {
        template <typename WeightType>
        class ImmediateTransition : public storm::gspn::Transition {
        public:
            /*!
             * Sets the weight of this transition to the given value.
             *
             * @param weight The new weight for this transition.
             */
            void setWeight(WeightType weight) {
                this->weight = weight;
            }

            /*!
             * Retrieves the weight of this transition.
             *
             * @return The weight of this transition.
             */
            WeightType getWeight() {
                return this->weight;
            }
        private:
            // the weight of the transition
           WeightType weight;
        };
    }
}

#endif //STORM_IMMEDIATETRANSITION_H
