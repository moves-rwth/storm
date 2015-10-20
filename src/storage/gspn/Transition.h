#ifndef STORM_TRANSITION_H
#define STORM_TRANSITION_H

#include <map>
#include "Marking.h"

namespace storm {
    namespace gspn {
        class Transition {
        public:
            /*!
             * Set the multiplicity of the input arc originating from the place.
             * If the arc already exists, the former multiplicity is overwritten.
             * If the arc does not yet exists, it is created.
             *
             * @param place The place connected by an input arc.
             * @param multiplicity The multiplicity of the specified arc.
             */
            void setInputArcCardinality(uint_fast64_t place, uint_fast64_t multiplicity);

            /*!
             * Removes an input arc connected to a given place.
             *
             * @param place The place from which the input arc is originating.
             * @return True if the arc existed.
             */
            bool removeInputArc(uint_fast64_t place);

            /*!
             * Checks whether the given place is connected to this transition via an input arc.
             *
             * @param place The place which is going to be checked.
             * @return True if the place is connected via an input arc.
             */
            bool existsInputArc(uint_fast64_t place);

            /*!
             * Set the multiplicity of the output arc going to the place.
             * If the arc already exists, the former multiplicity is overwritten.
             * If the arc does not yet exists, it is created.
             *
             * @param place The place connected by an output arc.
             * @param multiplicity The multiplicity of the specified arc.
             */
            void setOutputArcCardinality(uint_fast64_t place, uint_fast64_t multiplicity);

            /*!
             * Removes an output arc connected to a given place.
             *
             * @param place The place from which the output arc is leading to.
             * @return True if the arc existed.
             */
            bool removeOutputArc(uint_fast64_t place);

            /*!
             * Checks whether the given place is connected to this transition via an output arc.
             *
             * @param place The place which is going to be checked.
             * @return True if the place is connected via an output arc.
             */
            bool existsOutputArc(uint_fast64_t place);

            /*!
             * Set the multiplicity of the inhibition arc originating from the place.
             * If the arc already exists, the former multiplicity is overwritten.
             * If the arc does not yet exists, it is created.
             *
             * @param place The place connected by an inhibition arc.
             * @param multiplicity The multiplicity of the specified arc.
             */
            void setInhibitionArcCardinality(uint_fast64_t place, uint_fast64_t multiplicity);

            /*!
             * Removes an inhibition arc connected to a given place.
             *
             * @param place The place from which the inhibition arc is originating.
             * @return True if the arc existed.
             */
            bool removeInhibitionArc(uint_fast64_t place);

            /*!
             * Checks whether the given place is connected to this transition via an inhibition arc.
             *
             * @param place The place which is going to be checked.
             * @return True if the place is connected via an inhibition arc.
             */
            bool existsInhibitionArc(uint_fast64_t place);

            /*!
             * Checks if the given marking enables the transition.
             *
             * @return True if the transition is enabled.
             */
            bool isEnabled(storm::gspn::Marking marking);

            /*!
             * Fire the transition if possible.
             *
             * @param marking The current marking before the transition is fired.
             * @return The marking after the transition was fired.
             */
            storm::gspn::Marking fire(const storm::gspn::Marking marking);
        private:
            // maps places connected to this transition with an input arc to the corresponding multiplicity
            std::map<uint_fast64_t, uint_fast64_t> inputMultiplicities;

            // maps places connected to this transition with an output arc to the corresponding multiplicities
            std::map<uint_fast64_t, uint_fast64_t> outputMultiplicities;

            // maps places connected to this transition with an inhibition arc to the corresponding multiplicity
            std::map<uint_fast64_t, uint_fast64_t> inhibitionMultiplicities;
        };
    }
}

#endif //STORM_TRANSITION_H