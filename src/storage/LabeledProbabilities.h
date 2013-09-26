/*
 * LabeledProbabilities.h
 *
 *  Created on: 26.09.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_STORAGE_LABELEDPROBABILITIES_H
#define STORM_STORAGE_LABELEDPROBABILITIES_H

namespace storm {
    namespace storage {
    
        // This class provides the functionality to store a list of probabilities, each of which is labeled with a list
        // of labels.
        template<class Container, class ValueType>
        class LabeledProbabilities {
        public:
            /*!
             * Default-constructs an empty object.
             */
            LabeledProbabilities() : probabilityLabelList() {
                // Intentionally left empty.
            }
            
            /*!
             * Adds a probability to the list of labeled probabilities.
             *
             * @return A reference to the list of labels that is associated with the given probability.
             */
            Container<uint_fast64_t>& addProbability(ValueType probability) {
                probabilityLabelList.emplace_back(probability, Container<uint_fast64_t>());
                return probabilityLabelList.back().second;
            }
            
            /*!
             * Returns an iterator pointing to the first labeled probability.
             *
             * @return An iterator pointing to the first labeled probability.
             */
            Container<std::pair<ValueType, Container<uint_fast64_t>>>::iterator begin() {
                return probabilityLabelList.begin();
            }
            
            /*!
             * Returns an iterator pointing past the last labeled probability.
             *
             * @return An iterator pointing past the last labeled probability.
             */
            Container<std::pair<ValueType, Container<uint_fast64_t>>>::const_iterator end() {
                return probabilityLabelList.end();
            }
            
            /*!
             * Returns a const iterator pointing to the first labeled probability.
             *
             * @return A const iterator pointing to the first labeled probability.
             */
            Container<std::pair<ValueType, Container<uint_fast64_t>>>::const_iterator begin() const {
                return probabilityLabelList.begin();
            }
            
            /*!
             * Returns a const iterator pointing past the last labeled probability.
             *
             * @return A const iterator pointing past the last labeled probability.
             */
            Container<std::pair<ValueType, Container<uint_fast64_t>>>::const_iterator end() const {
                return probabilityLabelList.end();
            }
            
        private:
            // The actual storage used to store the list of probabilities and the associated labels.
            Container<std::pair<ValueType, Container<uint_fast64_t>>> probabilityLabelList;
        };
    }
}

#endif /* STORM_STORAGE_LABELEDPROBABILITIES_H */
