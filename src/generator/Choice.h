#ifndef STORM_GENERATOR_CHOICE_H_
#define STORM_GENERATOR_CHOICE_H_

#include <cstdint>
#include <functional>

#include <boost/optional.hpp>
#include <boost/container/flat_set.hpp>

#include "src/storage/Distribution.h"

namespace storm {
    namespace generator {
        
        // A structure holding information about a particular choice.
        template<typename ValueType, typename StateType=uint32_t>
        struct Choice {
        public:
            typedef boost::container::flat_set<uint_fast64_t> LabelSet;
            
            Choice(uint_fast64_t actionIndex = 0, bool markovian = false);
            
            Choice(Choice const& other) = default;
            Choice& operator=(Choice const& other) = default;
            Choice(Choice&& other) = default;
            Choice& operator=(Choice&& other) = default;
            
            /*!
             * Returns an iterator to the distribution associated with this choice.
             *
             * @return An iterator to the first element of the distribution.
             */
            typename storm::storage::Distribution<ValueType, StateType>::iterator begin();
            
            /*!
             * Returns an iterator to the distribution associated with this choice.
             *
             * @return An iterator to the first element of the distribution.
             */
            typename storm::storage::Distribution<ValueType, StateType>::const_iterator begin() const;
            
            /*!
             * Returns an iterator past the end of the distribution associated with this choice.
             *
             * @return An iterator past the end of the distribution.
             */
            typename storm::storage::Distribution<ValueType, StateType>::iterator end();
            
            /*!
             * Returns an iterator past the end of the distribution associated with this choice.
             *
             * @return An iterator past the end of the distribution.
             */
            typename storm::storage::Distribution<ValueType, StateType>::const_iterator end() const;
            
            /*!
             * Inserts the contents of this object to the given output stream.
             *
             * @param out The stream in which to insert the contents.
             */
            template<typename ValueTypePrime, typename StateTypePrime>
            friend std::ostream& operator<<(std::ostream& out, Choice<ValueTypePrime, StateTypePrime> const& choice);
            
            /*!
             * Adds the given label to the labels associated with this choice.
             *
             * @param label The label to associate with this choice.
             */
            void addChoiceLabel(uint_fast64_t label);
            
            /*!
             * Adds the given label set to the labels associated with this choice.
             *
             * @param labelSet The label set to associate with this choice.
             */
            void addChoiceLabels(LabelSet const& labelSet);
            
            /*!
             * Retrieves the set of labels associated with this choice.
             *
             * @return The set of labels associated with this choice.
             */
            LabelSet const& getChoiceLabels() const;
            
            /*!
             * Retrieves the index of the action of this choice.
             *
             * @return The index of the action of this choice.
             */
            uint_fast64_t getActionIndex() const;
            
            /*!
             * Retrieves the total mass of this choice.
             *
             * @return The total mass.
             */
            ValueType getTotalMass() const;
            
            /*!
             * Retrieves the entry in the choice that is associated with the given state and creates one if none exists,
             * yet.
             *
             * @param state The state for which to add the entry.
             * @return A reference to the entry that is associated with the given state.
             */
            ValueType& getOrAddEntry(StateType const& state);
            
            /*!
             * Retrieves the entry in the choice that is associated with the given state and creates one if none exists,
             * yet.
             *
             * @param state The state for which to add the entry.
             * @return A reference to the entry that is associated with the given state.
             */
            ValueType const& getOrAddEntry(StateType const& state) const;
            
            /*!
             * Adds the given probability value to the given state in the underlying distribution.
             */
            void addProbability(StateType const& state, ValueType const& value);
            
            /*!
             * Adds the given value to the reward associated with this choice.
             */
            void addChoiceReward(ValueType const& value);
            
            /*!
             * Retrieves the size of the distribution associated with this choice.
             */
            std::size_t size() const;
            
        private:
            // A flag indicating whether this choice is Markovian or not.
            bool markovian;

            // The action index associated with this choice.
            uint_fast64_t actionIndex;
            
            // The distribution that is associated with the choice.
            storm::storage::Distribution<ValueType, StateType> distribution;
            
            // The total probability mass (or rates) of this choice.
            ValueType totalMass;
            
            // The reward value associated with this choice.
            ValueType choiceReward;
            
            // The labels that are associated with this choice.
            boost::optional<LabelSet> choiceLabels;
        };

        template<typename ValueType, typename StateType>
        std::ostream& operator<<(std::ostream& out, Choice<ValueType, StateType> const& choice);

    }
}

#endif /* STORM_GENERATOR_CHOICE_H_ */