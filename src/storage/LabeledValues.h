/*
 * LabeledValues.h
 *
 *  Created on: 26.09.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_STORAGE_LABELEDVALUES_H
#define STORM_STORAGE_LABELEDVALUES_H

#include <list>
#include "src/storage/VectorSet.h"

namespace storm {
    namespace utility {
        template<class ValueType>
        static ValueType constantZero();
    }
    
    namespace storage {
        // This class provides the functionality to store a list of values, each of which is labeled with possibly several
        // labels.
        template<class ValueType>
        class LabeledValues {
        public:
            /*!
             * Default-constructs an empty object.
             */
            explicit LabeledValues() : valueLabelList() {
                // Intentionally left empty.
            }
            
            /*!
             * Constructs an object that stores the single probability value without any label.
             *
             * @param value The probability to sto
             */
            explicit LabeledValues(ValueType value) : valueLabelList() {
                addValue(value);
            }
            
            /*!
             * Adds an (unlabeled) value to the list of labeled values.
             *
             * @param value The value to add.
             * @return A reference to the list of labels that is associated with the given value.
             */
            storm::storage::VectorSet<uint_fast64_t>& addValue(ValueType value) {
                valueLabelList.emplace_back(value, storm::storage::VectorSet<uint_fast64_t>());
                return valueLabelList.back().second;
            }
            
            /*!
             * Adds a labeled value to the list of labeled values.
             *
             * @param value The value to add.
             * @param labels The labels to associate with this value.
             * @return A reference to the list of labels that is associated with the given value.
             */
            storm::storage::VectorSet<uint_fast64_t>& addValue(ValueType value, storm::storage::VectorSet<uint_fast64_t> const& labels) {
                valueLabelList.emplace_back(value, labels);
                return valueLabelList.back().second;
            }
            
            /*!
             * Returns an iterator pointing to the first labeled probability.
             *
             * @return An iterator pointing to the first labeled probability.
             */
            typename std::list<std::pair<ValueType, storm::storage::VectorSet<uint_fast64_t>>>::iterator begin() {
                return valueLabelList.begin();
            }
            
            /*!
             * Returns an iterator pointing past the last labeled probability.
             *
             * @return An iterator pointing past the last labeled probability.
             */
            typename std::list<std::pair<ValueType, storm::storage::VectorSet<uint_fast64_t>>>::const_iterator end() {
                return valueLabelList.end();
            }
            
            /*!
             * Returns a const iterator pointing to the first labeled probability.
             *
             * @return A const iterator pointing to the first labeled probability.
             */
            typename std::list<std::pair<ValueType, storm::storage::VectorSet<uint_fast64_t>>>::const_iterator begin() const {
                return valueLabelList.begin();
            }
            
            /*!
             * Returns a const iterator pointing past the last labeled probability.
             *
             * @return A const iterator pointing past the last labeled probability.
             */
            typename std::list<std::pair<ValueType, storm::storage::VectorSet<uint_fast64_t>>>::const_iterator end() const {
                return valueLabelList.end();
            }
            
            /*!
             * Inserts the contents of this object to the given output stream.
             *
             * @param out The stream in which to insert the contents.
             */
            friend std::ostream& operator<<(std::ostream& out, LabeledValues const& labeledValues) {
                out << "[";
                for (auto const& element : labeledValues) {
                    out << element.first << " (";
                    for (auto const& label : element.second) {
                        out << label << ", ";
                    }
                    out << ") ";
                }
                out << "]";
                return out;
            }
            
            /*!
             * Adds all labeled values of the given object to the current one.
             *
             * @param labeledValues The labeled values to add to the object.
             */
            LabeledValues<ValueType>& operator+=(LabeledValues<ValueType> const& labeledValues) {
                for (auto const& valueLabelListPair : labeledValues) {
                    this->valueLabelList.push_back(valueLabelListPair);
                }
                return *this;
            }
            
            /*!
             * Divides the values by the given value.
             *
             * @param value The value by which to divide.
             * @return A collection of labeled values that have the same labels as the current object, but whose values
             * are divided by the given one.
             */
            LabeledValues<ValueType> operator/(ValueType value) const {
                LabeledValues<ValueType> result;
                for (auto const& valueLabelListPair : valueLabelList) {
                    result.addValue(valueLabelListPair.first / value, valueLabelListPair.second);
                }
                return result;
            }
            
            /*!
             * Divides the values by the given unsigned integer value.
             *
             * @param value The unsigned integer value by which to divide.
             * @return A collection of labeled values that have the same labels as the current object, but whose values
             * are divided by the given one.
             */
            LabeledValues<ValueType> operator/(uint_fast64_t value) const {
                LabeledValues<ValueType> result;
                for (auto const& valueLabelListPair : valueLabelList) {
                    result.addValue(valueLabelListPair.first / value, valueLabelListPair.second);
                }
                return result;
            }
            
            /*!
             * Converts the object into the value type by returning the sum.
             *
             * @return The sum of the values.
             */
            operator ValueType() const {
                return this->getSum();
            }
            
            /*!
             * Retrieves the number of separate entries in this object.
             *
             * @return The number of separate entries in this object.
             */
            size_t size() const {
                return this->valueLabelList.size();
            }
            
        private:
            // The actual storage used to store the list of values and the associated labels.
            std::list<std::pair<ValueType, storm::storage::VectorSet<uint_fast64_t>>> valueLabelList;
            
            /*!
             * Returns the sum of the values.
             *
             * @return The sum of the values.
             */
            ValueType getSum() const {
                ValueType sum = storm::utility::constantZero<ValueType>();
                for (auto const& valueLabelListPair : *this) {
                    sum += valueLabelListPair.first;
                }
                return sum;
            }
        };
        
        /*!
         * Computes the hash value of a given labeled probabilities object.
         *
         * @param labeledProbabilities The labeled probabilities object for which to compute the hash value.
         * @return A hash value for the labeled probabilities object.
         */
        template<typename ValueType>
        std::size_t hash_value(LabeledValues<ValueType> const& labeledValues) {
            return labeledValues.size();
        }

    }
}

#endif /* STORM_STORAGE_LABELEDVALUES_H */
