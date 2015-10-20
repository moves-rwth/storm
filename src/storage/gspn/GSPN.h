#ifndef STORM_GSPN_H
#define STORM_GSPN_H

#include <set>
#include "ImmediateTransition.h"
#include "TimedTransition.h"
#include "Marking.h"

namespace storm {
    namespace gspn {
        // Stores a GSPN
        class GSPN {
        public:
            // Later, the rates and probabilities type should become a template, for now, let it be doubles.
            typedef double RateType;
            typedef double WeightType;
        private:
            // set containing all immediate transitions
            std::set<storm::gspn::ImmediateTransition<WeightType>> immediateTransitions;

            // set containing all timed transitions
            std::set<storm::gspn::TimedTransition<RateType>> timedTransitions;

            // initial marking
            storm::gspn::Marking initialMarking;
        };
    }
}

#endif //STORM_GSPN_H
