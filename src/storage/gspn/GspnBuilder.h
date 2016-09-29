#ifndef STORM_STORAGE_GSPN_GSPNBUILDER_H
#define STORM_STORAGE_GSPN_GSPNBUILDER_H

#include <map>
#include <string>
#include <vector>

#include "GSPN.h"

namespace storm {
    namespace gspn {
        class GspnBuilder {
        public:
            /**
             * Add a place to the gspn.
             * @param id The id must be unique for the gspn.
             * @param name The name must be unique for the gspn.
             * @param capacity The capacity is the limit of tokens in the place.
             *                 A capacity of -1 indicates an unbounded place.
             * @param initialTokens The number of inital tokens in the place.
             */
            uint_fast64_t addPlace(int_fast64_t const& capacity = 1, uint_fast64_t const& initialTokens = 0);

            /**
             * Adds an immediate transition to the gspn.
             * @param id The id must be unique for the gspn.
             * @param priority The priority for the transtion.
             * @param weight The weight for the transition.
             */
            uint_fast64_t addImmediateTransition(uint_fast64_t const& priority = 0, double const& weight = 0);

            /**
             * Adds an timed transition to the gspn.
             * @param id The name id be unique for the gspn.
             * @param priority The priority for the transtion.
             * @param weight The weight for the transition.
             */
            uint_fast64_t addTimedTransition(uint_fast64_t const &priority = 0, double const &rate = 0);

            /**
             * Adds an new input arc from a place to an transition.
             * @param from The place from which the arc is originating.
             * @param to The transtion to which the arc goes to.
             * @param multiplicity The multiplicity of the arc.
             */
            void addInputArc(uint_fast64_t const &from, uint_fast64_t const &to, uint_fast64_t const &multiplicity = 1);

            /**
             * Adds an new input arc from a place to an transition.
             * @param from The place from which the arc is originating.
             * @param to The transtion to which the arc goes to.
             * @param multiplicity The multiplicity of the arc.
             */
            void addInhibitionArc(uint_fast64_t const& from, uint_fast64_t const& to, uint_fast64_t const& multiplicity = 1);

            /**
             * Adds an new input arc from a place to an transition.
             * @param from The place from which the arc is originating.
             * @param to The transtion to which the arc goes to.
             * @param multiplicity The multiplicity of the arc.
             */
            void addOutputArc(uint_fast64_t const& from, uint_fast64_t const& to, uint_fast64_t const& multiplicity = 1);

            /**
             *
             * @return The gspn which is constructed by the builder.
             */
            storm::gspn::GSPN const& buildGspn() const;
        private:
            // gspn which is returned
            storm::gspn::GSPN gspn;
            // id for the next state which is added
            uint_fast64_t nextStateID = 0;
            // id for the next transition (timed or immediate) which is added
            uint_fast64_t nextTransitionID = 0;
        };
    }
}



#endif //STORM_STORAGE_GSPN_GSPNBUILDER_H
