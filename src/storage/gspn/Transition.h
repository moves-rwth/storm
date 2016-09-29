#ifndef STORM_STORAGE_GSPN_TRANSITION_H_
#define STORM_STORAGE_GSPN_TRANSITION_H_

#include <map>
#include <vector>
#include "src/storage/gspn/Marking.h"
#include "src/storage/gspn/Place.h"

namespace storm {
    namespace gspn {
        /*!
         * This class represents a transition in a gspn.
         */
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
            void setInputArcMultiplicity(storm::gspn::Place const& place, uint_fast64_t multiplicity);

            /*!
             * Removes an input arc connected to a given place.
             *
             * @param place The place from which the input arc is originating.
             * @return True if the arc existed.
             */
            bool removeInputArc(storm::gspn::Place const& place);

            /*!
             * Checks whether the given place is connected to this transition via an input arc.
             *
             * @param place The place which is going to be checked.
             * @return True if the place is connected via an input arc.
             */
            bool existsInputArc(storm::gspn::Place const& place) const;

            /*!
             * Set the multiplicity of the output arc going to the place.
             * If the arc already exists, the former multiplicity is overwritten.
             * If the arc does not yet exists, it is created.
             *
             * @param place The place connected by an output arc.
             * @param multiplicity The multiplicity of the specified arc.
             */
            void setOutputArcMultiplicity(storm::gspn::Place const& place, uint_fast64_t multiplicity);

            /*!
             * Removes an output arc connected to a given place.
             *
             * @param place The place from which the output arc is leading to.
             * @return True if the arc existed.
             */
            bool removeOutputArc(storm::gspn::Place const& place);

            /*!
             * Checks whether the given place is connected to this transition via an output arc.
             *
             * @param place The place which is going to be checked.
             * @return True if the place is connected via an output arc.
             */
            bool existsOutputArc(storm::gspn::Place const& place) const;

            /*!
             * Set the multiplicity of the inhibition arc originating from the place.
             * If the arc already exists, the former multiplicity is overwritten.
             * If the arc does not yet exists, it is created.
             *
             * @param place The place connected by an inhibition arc.
             * @param multiplicity The multiplicity of the specified arc.
             */
            void setInhibitionArcMultiplicity(storm::gspn::Place const& place, uint_fast64_t multiplicity);

            /*!
             * Removes an inhibition arc connected to a given place.
             *
             * @param place The place from which the inhibition arc is originating.
             * @return True if the arc existed.
             */
            bool removeInhibitionArc(storm::gspn::Place const& place);

            /*!
             * Checks whether the given place is connected to this transition via an inhibition arc.
             *
             * @param place The place which is going to be checked.
             * @return True if the place is connected via an inhibition arc.
             */
            bool existsInhibitionArc(storm::gspn::Place const& place) const;

            /*!
             * Checks if the given marking enables the transition.
             *
             * @return True if the transition is enabled.
             */
            bool isEnabled(storm::gspn::Marking const& marking) const;

            /*!
             * Fire the transition if possible.
             *
             * @param marking The current marking before the transition is fired.
             * @return The marking after the transition was fired.
             */
            storm::gspn::Marking fire(storm::gspn::Marking const& marking) const;

            /*!
             * Set the name of the transition.
             *
             * @param name New name of the transition.
             */
            void setName(std::string const& name);

            /*!
             * Returns the name of the transition.
             *
             * @return The name of the transition.
             */
            std::string const& getName() const;

            /*!
             * Returns a list of places which are connected via a input arc.
             * @return
             */
            const std::vector<std::shared_ptr<storm::gspn::Place>> &getInputPlaces() const;

            /*!
             * Returns a list of places which are connected via a output arc.
             * @return
             */
            const std::vector<std::shared_ptr<storm::gspn::Place>> &getOutputPlaces() const;

            /*!
             * Returns a list of places which are connected via a inhibition arc.
             * @return
             */
            const std::vector<std::shared_ptr<storm::gspn::Place>> &getInhibitionPlaces() const;

            /*!
             * Returns the corresponding multiplicity.
             *
             * @param place connected to this transition by an input arc
             * @return cardinality or 0 if the arc does not exists
             */
            uint_fast64_t getInputArcMultiplicity(storm::gspn::Place const& place) const;

            /*!
             * Returns the corresponding multiplicity.
             *
             * @param place connected to this transition by an inhibition arc
             * @return cardinality or 0 if the arc does not exists
             */
            uint_fast64_t getInhibitionArcMultiplicity(storm::gspn::Place const& place) const;

            /*!
             * Returns the corresponding multiplicity.
             *
             * @param place connected to this transition by an output arc
             * @return cardinality or 0 if the arc does not exists
             */
            uint_fast64_t getOutputArcMultiplicity(storm::gspn::Place const& place) const;

            /*!
             * Sets the priority of this transtion.
             *
             * @param priority The new priority.
             */
            void setPriority(uint_fast64_t const& priority);

            /*!
             * Returns the priority of this transition.
             *
             * @return The priority.
             */
            uint_fast64_t getPriority() const;

            void setID(uint_fast64_t const& id) {
                this->id = id;
            }
        private:
            /*!
             * Comparator which defines an total order on places.
             * A place is less than another place if the id is less than the id from the other place.
             */
            struct PlaceComparator {
                bool operator()(uint_fast64_t const& lhs, uint_fast64_t const& rhs) const {
                    return lhs < rhs;
                }
            };

            // maps place ids connected to this transition with an input arc to the corresponding multiplicity
            std::map<uint_fast64_t, uint_fast64_t, storm::gspn::Transition::PlaceComparator> inputMultiplicities;

            // maps place ids connected to this transition with an output arc to the corresponding multiplicities
            std::map<uint_fast64_t, uint_fast64_t, storm::gspn::Transition::PlaceComparator> outputMultiplicities;

            // maps place ids connected to this transition with an inhibition arc to the corresponding multiplicity
            std::map<uint_fast64_t, uint_fast64_t, storm::gspn::Transition::PlaceComparator> inhibitionMultiplicities;

            // name of the transition
            std::string name;

            // list of all places connected to this transition via an input arc
            std::vector<std::shared_ptr<storm::gspn::Place>> inputPlaces;

            // list of all places connected to this transition via an output arc
            std::vector<std::shared_ptr<storm::gspn::Place>> outputPlaces;

            // list of all places connected to this transition via an inhibition arc
            std::vector<std::shared_ptr<storm::gspn::Place>> inhibitionPlaces;

            // priority of this transition
            uint_fast64_t priority = 0;

            uint_fast64_t id;
        };
    }
}

#endif //STORM_STORAGE_GSPN_TRANSITION_H_