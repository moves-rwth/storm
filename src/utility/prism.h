#ifndef STORM_UTILITY_PRISM_H_
#define STORM_UTILITY_PRISM_H_

#include <memory>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/container/flat_set.hpp>

#include "src/utility/OsDetection.h"


namespace storm {
    namespace expressions {
        class Variable;
        class Expression;
    }
    
    namespace prism {
        class Program;
    }
    
    namespace utility {
        namespace prism {
            // A structure holding information about a particular choice.
            template<typename ValueType, typename KeyType=uint32_t, typename Compare=std::less<uint32_t>>
            struct Choice {
            public:
                Choice(uint_fast64_t actionIndex = 0, bool createChoiceLabels = false) : distribution(), actionIndex(actionIndex), choiceLabels(nullptr) {
                    if (createChoiceLabels) {
                        choiceLabels = std::shared_ptr<boost::container::flat_set<uint_fast64_t>>(new boost::container::flat_set<uint_fast64_t>());
                    }
                }
                
                Choice(Choice const& other) = default;
                Choice& operator=(Choice const& other) = default;
#ifndef WINDOWS
                Choice(Choice&& other) = default;
                Choice& operator=(Choice&& other) = default;
#endif
                
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
                    choiceLabels->insert(label);
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
                    return *choiceLabels;
                }
                
                /*!
                 * Retrieves the index of the action of this choice.
                 *
                 * @return The index of the action of this choice.
                 */
                uint_fast64_t getActionIndex() const {
                    return actionIndex;
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
                
                void addProbability(KeyType state, ValueType value) {
                    distribution[state] += value;
                }
                
                std::size_t size() const {
                    return distribution.size();
                }
                
            private:
                // The distribution that is associated with the choice.
                std::map<KeyType, ValueType, Compare> distribution;
                
                // The index of the action name.
                uint_fast64_t actionIndex;
                
                // The labels that are associated with this choice.
                std::shared_ptr<boost::container::flat_set<uint_fast64_t>> choiceLabels;
            };
            
            std::map<storm::expressions::Variable, storm::expressions::Expression> parseConstantDefinitionString(storm::prism::Program const& program, std::string const& constantDefinitionString);
            
        } // namespace prism
    } // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_PRISM_H_ */
