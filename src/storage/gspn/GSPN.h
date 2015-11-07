#ifndef STORM_GSPN_H
#define STORM_GSPN_H

#include <vector>
#include "src/storage/gspn/ImmediateTransition.h"
#include "src/storage/gspn/TimedTransition.h"
#include "src/storage/gspn/Marking.h"

namespace storm {
    namespace gspn {
        // Stores a GSPN
        class GSPN {
        public:
            // Later, the rates and probabilities type should become a template, for now, let it be doubles.
            typedef double RateType;
            typedef double WeightType;

            /*!
             * The empty constructor creates an GSPN without transition and places
             */
            GSPN();

            /*!
             * Set the number of tokens for a given place.
             *
             * @param place
             * @param token
             */
            void setInitialTokens(uint64_t place, uint64_t token);
        private:
            // set containing all immediate transitions
            std::vector<storm::gspn::ImmediateTransition<WeightType>> immediateTransitions;

            // set containing all timed transitions
            std::vector<storm::gspn::TimedTransition<RateType>> timedTransitions;

            // initial marking
            storm::gspn::Marking initialMarking;
        };
    }
}

#endif //STORM_GSPN_H
