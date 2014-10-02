#ifndef STORM_STORAGE_EXPRESSIONS_SIMPLEVALUATION_H_
#define STORM_STORAGE_EXPRESSIONS_SIMPLEVALUATION_H_

#include <boost/container/flat_map.hpp>
#include <boost/variant.hpp>
#include <iostream>

#include "src/storage/expressions/Valuation.h"
#include "src/storage/expressions/ExpressionReturnType.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace expressions {
        class SimpleValuation : public Valuation {
        public:
            friend class SimpleValuationPointerHash;
            friend class SimpleValuationPointerLess;
            
            typedef boost::container::flat_map<std::string, boost::variant<bool, int_fast64_t, double>> map_type;
            
            // Instantiate some constructors and assignments with their default implementations.
            SimpleValuation() = default;
            SimpleValuation(SimpleValuation const&) = default;
            SimpleValuation& operator=(SimpleValuation const&) = default;
#ifndef WINDOWS            
			SimpleValuation(SimpleValuation&&) = default;
            SimpleValuation& operator=(SimpleValuation&&) = default;
#endif
            virtual ~SimpleValuation() = default;

            /*!
             * Compares two simple valuations wrt. equality.
             */
            bool operator==(SimpleValuation const& other) const;
            
            /*!
             * Adds a boolean identifier with the given name.
             *
             * @param name The name of the boolean identifier to add.
             * @param initialValue The initial value of the identifier.
             */
            void addBooleanIdentifier(std::string const& name, bool initialValue = false);
            
            /*!
             * Adds a integer identifier with the given name.
             *
             * @param name The name of the integer identifier to add.
             * @param initialValue The initial value of the identifier.
             */
            void addIntegerIdentifier(std::string const& name, int_fast64_t initialValue = 0);

            /*!
             * Adds a double identifier with the given name.
             *
             * @param name The name of the double identifier to add.
             * @param initialValue The initial value of the identifier.
             */
            void addDoubleIdentifier(std::string const& name, double initialValue = 0);
            
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
            
            /*!
             * Removes the given identifier from this valuation.
             *
             * @param name The name of the identifier that is to be removed.
             */
            void removeIdentifier(std::string const& name);

            /*!
             * Retrieves the type of the identifier with the given name.
             *
             * @param name The name of the identifier whose type to retrieve.
             * @return The type of the identifier with the given name.
             */
            ExpressionReturnType getIdentifierType(std::string const& name) const;
            
            // Override base class methods.
            virtual bool containsBooleanIdentifier(std::string const& name) const override;
            virtual bool containsIntegerIdentifier(std::string const& name) const override;
            virtual bool containsDoubleIdentifier(std::string const& name) const override;
            virtual std::size_t getNumberOfIdentifiers() const override;
            virtual std::set<std::string> getIdentifiers() const override;
            virtual std::set<std::string> getBooleanIdentifiers() const override;
            virtual std::set<std::string> getIntegerIdentifiers() const override;
            virtual std::set<std::string> getDoubleIdentifiers() const override;
            virtual bool getBooleanValue(std::string const& name) const override;
            virtual int_fast64_t getIntegerValue(std::string const& name) const override;
            virtual double getDoubleValue(std::string const& name) const override;
            
            friend std::ostream& operator<<(std::ostream& stream, SimpleValuation const& valuation);

        private:
            // A mapping of boolean identifiers to their local indices in the value container.
            boost::container::flat_map<std::string, boost::variant<bool, int_fast64_t, double>> identifierToValueMap;
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