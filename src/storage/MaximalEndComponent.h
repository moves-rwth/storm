#ifndef STORM_STORAGE_MAXIMALENDCOMPONENT_H_
#define STORM_STORAGE_MAXIMALENDCOMPONENT_H_

#include <unordered_map>
#include <boost/container/flat_set.hpp>

namespace storm {
    namespace storage {
        /*!
         * This class represents a maximal end-component of a nondeterministic model.
         */
        class MaximalEndComponent {
        public:
            typedef std::unordered_map<uint_fast64_t, boost::container::flat_set<uint_fast64_t>>::iterator iterator;
            typedef std::unordered_map<uint_fast64_t, boost::container::flat_set<uint_fast64_t>>::const_iterator const_iterator;
            
            /*!
             * Creates an empty MEC.
             */
            MaximalEndComponent();
            
            MaximalEndComponent(MaximalEndComponent const& other);
            
            MaximalEndComponent& operator=(MaximalEndComponent const& other);
            
            MaximalEndComponent(MaximalEndComponent&& other);
            
            MaximalEndComponent& operator=(MaximalEndComponent&& other);
            
            /*!
             * Adds the given state and the given choices to the MEC.
             *
             * @param state The state for which to add the choices.
             * @param choices The choices to add for the state.
             */
            void addState(uint_fast64_t state, std::vector<uint_fast64_t> const& choices);

            /*!
             * Adds the given state and the given choices to the MEC.
             *
             * @param state The state for which to add the choices.
             * @param choices The choices to add for the state.
             */
            void addState(uint_fast64_t state, std::vector<uint_fast64_t>&& choices);
            
            /*!
             * Retrieves the choices for the given state that are contained in this MEC under the
             * assumption that the state is in the MEC.
             *
             * @param state The state for which to retrieve the choices.
             * @return A list of choices of the state in the MEC.
             */
            boost::container::flat_set<uint_fast64_t> const& getChoicesForState(uint_fast64_t state) const;
            
            /*!
             * Removes the given choice from the list of choices of the named state.
             *
             * @param state The state for which to remove the choice.
             * @param choice The choice to remove from the state's choices.
             */
            void removeChoice(uint_fast64_t state, uint_fast64_t choice);
            
            /*!
             * Removes the given state and all of its choices from the MEC.
             *
             * @param state The state to remove froom the MEC.
             */
            void removeState(uint_fast64_t state);
            
            /*!
             * Retrieves whether the given state is contained in this MEC.
             *
             * @param state The state for which to query membership in the MEC.
             * @return True if the given state is contained in the MEC.
             */
            bool containsState(uint_fast64_t state) const;
            
            /*!
             * Retrievs whether the given choice for the given state is contained in the MEC.
             *
             * @param state The state for which to check whether the given choice is contained in the MEC.
             * @param choice The choice for which to check whether it is contained in the MEC.
             * @return True if the given choice is contained in the MEC.
             */
            bool containsChoice(uint_fast64_t state, uint_fast64_t choice) const;
            
            /*!
             * Retrieves the set of states contained in the MEC.
             *
             * @return The set of states contained in the MEC.
             */
            boost::container::flat_set<uint_fast64_t> getStateSet() const;
            
            iterator begin();
            
            iterator end();
            
            const_iterator begin() const;
            
            const_iterator end() const;
            
            friend std::ostream& operator<<(std::ostream& out, MaximalEndComponent const& component);
            
        private:
            // This stores the mapping from states contained in the MEC to the choices in this MEC.
            std::unordered_map<uint_fast64_t, boost::container::flat_set<uint_fast64_t>> stateToChoicesMapping;
        };
    }
}

#endif /* STORM_STORAGE_MAXIMALENDCOMPONENT_H_ */
