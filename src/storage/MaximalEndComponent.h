#ifndef STORM_STORAGE_MAXIMALENDCOMPONENT_H_
#define STORM_STORAGE_MAXIMALENDCOMPONENT_H_

#include <unordered_map>
#include <vector>

namespace storm {
    namespace storage {
        /*!
         * This class represents a maximal end-component of a nondeterministic model.
         */
        class MaximalEndComponent {
        public:
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
             * Retrieves the choices for the given state that are contained in this MEC under the
             * assumption that the state is in the MEC.
             *
             * @param state The state for which to retrieve the choices.
             * @return A list of choices of the state in the MEC.
             */
            std::vector<uint_fast64_t> const& getChoicesForState(uint_fast64_t state) const;
            
            /*!
             * Retrieves whether the given state is contained in this MEC.
             *
             * @param state The state for which to query membership in the MEC.
             * @return True if the given state is contained in the MEC.
             */
            bool containsState(uint_fast64_t state) const;
            
        private:
            // This stores the mapping from states contained in the MEC to the choices in this MEC.
            std::unordered_map<uint_fast64_t, std::vector<uint_fast64_t>> stateToChoicesMapping;
        };
    }
}

#endif /* STORM_STORAGE_MAXIMALENDCOMPONENT_H_ */
