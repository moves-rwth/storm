#ifndef STORM_STORAGE_GSPN_GSPN_H
#define STORM_STORAGE_GSPN_GSPN_H

#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory>

#include "src/storage/gspn/ImmediateTransition.h"
#include "src/storage/gspn/Marking.h"
#include "src/storage/gspn/Place.h"
#include "src/storage/gspn/TimedTransition.h"

namespace storm {
    namespace gspn {
        // Stores a GSPN
        class GSPN {
        public:
            // Later, the rates and probabilities type should become a template, for now, let it be doubles.
            typedef double RateType;
            typedef double WeightType;

            /*!
             * Adds an immediate transition to the gspn.
             *
             * @param transition The transition which is added to the gspn.
             */
            void addImmediateTransition(ImmediateTransition<WeightType> const& transition);

            /*!
             * Adds a timed transition to the gspn.
             *
             * @param transition The transition which is added to the gspn.
             */
            void addTimedTransition(TimedTransition<RateType> const& transition);

            /*!
             * Adds a place to the gspn.
             *
             * @param place The place which is added to the gspn.
             */
            void addPlace(Place const& place);

            /*!
             * Returns the number of places in this gspn.
             *
             * @return The number of places.
             */
            uint_fast64_t getNumberOfPlaces() const;

            /*!
             * Returns the vector of timed transitions in this gspn.
             *
             * @return The vector of timed transitions.
             */
            std::vector<std::shared_ptr<TimedTransition<GSPN::RateType>>> const& getTimedTransitions() const;

            /*!
             * Returns the vector of immediate transitions in this gspn.
             *
             * @return The vector of immediate tansitions.
             */
            std::vector<std::shared_ptr<ImmediateTransition<GSPN::WeightType>>> const& getImmediateTransitions() const;

            /*!
             * Returns the places of this gspn
             */
            std::vector<storm::gspn::Place> const& getPlaces() const;

            /*
             * Computes the initial marking of the gspn.
             *
             * @param map The Map determines the number of bits for each place.
             * @return The initial Marking
             */
            std::shared_ptr<storm::gspn::Marking> getInitialMarking(std::map<uint_fast64_t, uint_fast64_t>& numberOfBits, uint_fast64_t const& numberOfTotalBits) const;

            /*!
             * Returns the place with the corresponding id.
             *
             * @param id The ID of the place.
             * @return The first element is true if the place was found.
             *         If the first element is true, then the second element is the wanted place.
             *         If the first element is false, then the second element is not defined.
             */
            std::pair<bool, storm::gspn::Place> getPlace(uint_fast64_t const& id) const;

            std::pair<bool, storm::gspn::Place> getPlace(std::string const& id) const;
            /*!
             * Returns the timed transition with the corresponding id.
             *
             * @param id The ID of the timed transition.
             * @return The first element is true if the transition was found.
             *         If the first element is true, then the second element is the wanted transition.
             *         If the first element is false, then the second element is the nullptr.
             */
            std::pair<bool, std::shared_ptr<storm::gspn::TimedTransition<GSPN::RateType>> const> getTimedTransition(std::string const& id) const;

            /*!
             * Returns the immediate transition with the corresponding id.
             *
             * @param id The ID of the timed transition.
             * @return The first element is true if the transition was found.
             *         If the first element is true, then the second element is the wanted transition.
             *         If the first element is false, then the second element is the nullptr.
             */
            std::pair<bool, std::shared_ptr<storm::gspn::ImmediateTransition<GSPN::WeightType>> const> getImmediateTransition(std::string const& id) const;

            /*!
             * Returns the transition with the corresponding id.
             *
             * @param id The ID of the transition.
             * @return Pointer to the corresponding transition or nullptr if the place does not exists.
             */
            //            std::shared_ptr<storm::gspn::Transition> getTransition(std::string const& id) const;
            std::pair<bool, std::shared_ptr<storm::gspn::Transition> const> getTransition(std::string const& id) const;

            /*!
             * Write the gspn in a dot(graphviz) configuration.
             *
             * @param outStream The stream to which the output is written to.
             */
            void writeDotToStream(std::ostream& outStream);

            /*!
             * Set the name of the gspn to the given name.
             *
             * @param name The new name.
             */
            void setName(std::string const& name);

            /*!
             * Returns the name of the gspn.
             *
             * @return The name.
             */
            std::string const& getName() const;

            /*!
             * Performe some checks
             * - testPlaces()
             * - testTransitions()
             *
             * @return true if no errors are found
             */
            bool isValid() const;
            // TODO doc
            void toPnpro(std::ostream &stream) const;
            // TODO doc
            void toPnml(std::ostream &stream) const;
        private:
            /*!
             * Test
             *  - if places are unique (ids and names)
             *  - if the capacity is greater than the number of initial tokens
             *
             * @return true if no errors found
             */
            bool testPlaces() const;

            /*!
             * Test
             * - if transition have at least on input/inhibitor and one output place
             *
             * @return true if no errors found
             */
            bool testTransitions() const;

            // set containing all immediate transitions
            std::vector<std::shared_ptr<storm::gspn::ImmediateTransition<WeightType>>> immediateTransitions;

            // set containing all timed transitions
            std::vector<std::shared_ptr<storm::gspn::TimedTransition<RateType>>> timedTransitions;

            // set containing all places
            std::vector<storm::gspn::Place> places;

            // name of the gspn
            std::string name;
        };
    }
}

#endif //STORM_STORAGE_GSPN_GSPN_H
