#ifndef STORM_UTILITY_PRISMUTILITY
#define STORM_UTILITY_PRISMUTILITY

#include "src/storage/LabeledValues.h"
#include "src/storage/prism/Program.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace utility {
        namespace prism {
            // A structure holding information about a particular choice.
            template<typename ValueType, typename KeyType=uint_fast64_t, typename Compare=std::less<uint_fast64_t>>
            struct Choice {
            public:
                Choice(std::string const& actionLabel) : distribution(), actionLabel(actionLabel), choiceLabels() {
                    // Intentionally left empty.
                }
                
                /*!
                 * Returns an iterator to the first element of this choice.
                 *
                 * @return An iterator to the first element of this choice.
                 */
                typename std::map<KeyType, ValueType>::iterator begin() {
                    return distribution.begin();
                }
                
                /*!
                 * Returns an iterator to the first element of this choice.
                 *
                 * @return An iterator to the first element of this choice.
                 */
                typename std::map<KeyType, ValueType>::const_iterator begin() const {
                    return distribution.cbegin();
                }
                
                /*!
                 * Returns an iterator that points past the elements of this choice.
                 *
                 * @return An iterator that points past the elements of this choice.
                 */
                typename std::map<KeyType, ValueType>::iterator end() {
                    return distribution.end();
                }
                
                /*!
                 * Returns an iterator that points past the elements of this choice.
                 *
                 * @return An iterator that points past the elements of this choice.
                 */
                typename std::map<KeyType, ValueType>::const_iterator end() const {
                    return distribution.cend();
                }
                
                /*!
                 * Returns an iterator to the element with the given key, if there is one. Otherwise, the iterator points to
                 * distribution.end().
                 *
                 * @param value The value to find.
                 * @return An iterator to the element with the given key, if there is one.
                 */
                typename std::map<KeyType, ValueType>::iterator find(uint_fast64_t value) {
                    return distribution.find(value);
                }
                
                /*!
                 * Inserts the contents of this object to the given output stream.
                 *
                 * @param out The stream in which to insert the contents.
                 */
                friend std::ostream& operator<<(std::ostream& out, Choice<ValueType> const& choice) {
                    out << "<";
                    for (auto const& stateProbabilityPair : choice.distribution) {
                        out << stateProbabilityPair.first << " : " << stateProbabilityPair.second << ", ";
                    }
                    out << ">";
                    return out;
                }
                
                /*!
                 * Adds the given label to the labels associated with this choice.
                 *
                 * @param label The label to associate with this choice.
                 */
                void addChoiceLabel(uint_fast64_t label) {
                    choiceLabels.insert(label);
                }
                
                /*!
                 * Adds the given label set to the labels associated with this choice.
                 *
                 * @param labelSet The label set to associate with this choice.
                 */
                void addChoiceLabels(boost::container::flat_set<uint_fast64_t> const& labelSet) {
                    for (uint_fast64_t label : labelSet) {
                        addChoiceLabel(label);
                    }
                }
                
                /*!
                 * Retrieves the set of labels associated with this choice.
                 *
                 * @return The set of labels associated with this choice.
                 */
                boost::container::flat_set<uint_fast64_t> const& getChoiceLabels() const {
                    return choiceLabels;
                }
                
                /*!
                 * Retrieves the action label of this choice.
                 *
                 * @return The action label of this choice.
                 */
                std::string const& getActionLabel() const {
                    return actionLabel;
                }
                
                /*!
                 * Retrieves the entry in the choice that is associated with the given state and creates one if none exists,
                 * yet.
                 *
                 * @param state The state for which to add the entry.
                 * @return A reference to the entry that is associated with the given state.
                 */
                ValueType& getOrAddEntry(uint_fast64_t state) {
                    auto stateProbabilityPair = distribution.find(state);
                    
                    if (stateProbabilityPair == distribution.end()) {
                        distribution[state] = ValueType();
                    }
                    return distribution.at(state);
                }
                
                /*!
                 * Retrieves the entry in the choice that is associated with the given state and creates one if none exists,
                 * yet.
                 *
                 * @param state The state for which to add the entry.
                 * @return A reference to the entry that is associated with the given state.
                 */
                ValueType const& getOrAddEntry(uint_fast64_t state) const {
                    auto stateProbabilityPair = distribution.find(state);
                    
                    if (stateProbabilityPair == distribution.end()) {
                        distribution[state] = ValueType();
                    }
                    return distribution.at(state);
                }
                
            private:
                // The distribution that is associated with the choice.
                std::map<KeyType, ValueType, Compare> distribution;
                
                // The label of the choice.
                std::string actionLabel;
                
                // The labels that are associated with this choice.
                boost::container::flat_set<uint_fast64_t> choiceLabels;
            };
            
            /*!
             * Adds the target state and probability to the given choice and ignores the labels. This function overloads with
             * other functions to ensure the proper treatment of labels.
             *
             * @param choice The choice to which to add the target state and probability.
             * @param state The target state of the probability.
             * @param probability The probability to reach the target state in one step.
             * @param labels A set of labels that is supposed to be associated with this state and probability. NOTE: this
             * is ignored by this particular function but not by the overloaded functions.
             */
            template<typename ValueType>
            void addProbabilityToChoice(Choice<ValueType>& choice, uint_fast64_t state, ValueType probability, boost::container::flat_set<uint_fast64_t> const& labels) {
                choice.getOrAddEntry(state) += probability;
            }
            
            /*!
             * Adds the target state and probability to the given choice and labels it accordingly. This function overloads
             * with other functions to ensure the proper treatment of labels.
             *
             * @param choice The choice to which to add the target state and probability.
             * @param state The target state of the probability.
             * @param probability The probability to reach the target state in one step.
             * @param labels A set of labels that is supposed to be associated with this state and probability.
             */
            template<typename ValueType>
            void addProbabilityToChoice(Choice<storm::storage::LabeledValues<ValueType>>& choice, uint_fast64_t state, ValueType probability, boost::container::flat_set<uint_fast64_t> const& labels) {
                auto& labeledEntry = choice.getOrAddEntry(state);
                labeledEntry.addValue(probability, labels);
            }
        } // namespace prism
    } // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_PRISMUTILITY_H_ */
