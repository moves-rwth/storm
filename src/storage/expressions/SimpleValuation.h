#ifndef STORM_STORAGE_EXPRESSIONS_SIMPLEVALUATION_H_
#define STORM_STORAGE_EXPRESSIONS_SIMPLEVALUATION_H_

#include <memory>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "src/storage/expressions/Valuation.h"

namespace storm {
    namespace expressions {
        class SimpleValuation : public Valuation {
        public:
            friend class SimpleValuationPointerHash;
            friend class SimpleValuationPointerLess;
            
            /*!
             * Creates a simple valuation that can hold the given number of boolean, integer and double variables.
             *
             * @param booleanVariableCount The number of boolean variables in the valuation.
             * @param integerVariableCount The number of integer variables in the valuation.
             * @param doubleVariableCount The number of double variables in the valuation.
             */
            SimpleValuation(std::size_t booleanVariableCount, std::size_t integerVariableCount, std::size_t doubleVariableCount);
            
            /*!
             * Creates a simple evaluation based on the given identifier to index map and value containers for the
             * different types of variables.
             *
             * @param identifierToIndexMap A shared pointer to a mapping from identifier to their local indices in the
             * value containers.
             * @param booleanValues The value container for all boolean identifiers.
             * @param integerValues The value container for all integer identifiers.
             * @param doubleValues The value container for all double identifiers.
             */
            SimpleValuation(std::shared_ptr<std::unordered_map<std::string, uint_fast64_t>> identifierToIndexMap, std::vector<bool> booleanValues, std::vector<int_fast64_t> integerValues, std::vector<double> doubleValues);

            // Instantiate some constructors and assignments with their default implementations.
            SimpleValuation(SimpleValuation const&) = default;
            SimpleValuation(SimpleValuation&&) = default;
            SimpleValuation& operator=(SimpleValuation const&) = default;
            SimpleValuation& operator=(SimpleValuation&&) = default;
            virtual ~SimpleValuation() = default;

            /*!
             * Compares two simple valuations wrt. equality.
             */
            bool operator==(SimpleValuation const& other) const;
            
            /*!
             * Sets the index of the identifier with the given name to the given value.
             *
             * @param name The name of the identifier for which to set the index.
             * @param index The new index of the identifier.
             */
            void setIdentifierIndex(std::string const& name, uint_fast64_t index);
            
            /*!
             * Sets the value of the boolean identifier with the given name to the given value.
             *
             * @param name The name of the boolean identifier whose value to set.
             * @param value The new value of the boolean identifier.
             */
            void setBooleanValue(std::string const& name, bool value);

            /*!
             * Sets the value of the integer identifier with the given name to the given value.
             *
             * @param name The name of the integer identifier whose value to set.
             * @param value The new value of the integer identifier.
             */
            void setIntegerValue(std::string const& name, int_fast64_t value);

            /*!
             * Sets the value of the double identifier with the given name to the given value.
             *
             * @param name The name of the double identifier whose value to set.
             * @param value The new value of the double identifier.
             */
            void setDoubleValue(std::string const& name, double value);
            
            // Override base class methods.
            virtual bool getBooleanValue(std::string const& name) const override;
            virtual int_fast64_t getIntegerValue(std::string const& name) const override;
            virtual double getDoubleValue(std::string const& name) const override;
            
            friend std::ostream& operator<<(std::ostream& stream, SimpleValuation const& valuation);

        private:
            // A mapping of identifiers to their local indices in the value containers.
            std::shared_ptr<std::unordered_map<std::string, uint_fast64_t>> identifierToIndexMap;
            
            // The value container for all boolean identifiers.
            std::vector<bool> booleanValues;
            
            // The value container for all integer identifiers.
            std::vector<int_fast64_t> integerValues;
            
            // The value container for all double identifiers.
            std::vector<double> doubleValues;
        };
        
        /*!
         * A helper class that can pe used as the hash functor for data structures that need to hash a simple valuations
         * given via pointers.
         */
        class SimpleValuationPointerHash {
        public:
            std::size_t operator()(SimpleValuation* valuation) const;
        };
        
        /*!
         * A helper class that can be used as the comparison functor wrt. equality for data structures that need to
         * store pointers to a simple valuations and need to compare the elements wrt. their content (rather than
         * pointer equality).
         */
        class SimpleValuationPointerCompare {
        public:
            bool operator()(SimpleValuation* valuation1, SimpleValuation* valuation2) const;
        };
        
        /*!
         * A helper class that can be used as the comparison functor wrt. "<" for data structures that need to
         * store pointers to a simple valuations and need to compare the elements wrt. their content (rather than
         * pointer equality).
         */
        class SimpleValuationPointerLess {
        public:
            bool operator()(SimpleValuation* valuation1, SimpleValuation* valuation2) const;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_SIMPLEVALUATION_H_ */