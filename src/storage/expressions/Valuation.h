#ifndef STORM_STORAGE_EXPRESSIONS_VALUATION_H_
#define STORM_STORAGE_EXPRESSIONS_VALUATION_H_

#include <string>

namespace storm {
    namespace expressions {
        /*!
         * The base class of all valuations where a valuation assigns a concrete value to all identifiers. This is, for
         * example, used for evaluating expressions.
         */
        class Valuation {
        public:
            /*!
             * Retrieves the boolean value of the identifier with the given name.
             *
             * @param name The name of the boolean identifier whose value to retrieve.
             * @return The value of the boolean identifier.
             */
            virtual bool getBooleanValue(std::string const& name) const = 0;
            
            /*!
             * Retrieves the integer value of the identifier with the given name.
             *
             * @param name The name of the integer identifier whose value to retrieve.
             * @return The value of the integer identifier.
             */
            virtual int_fast64_t getIntegerValue(std::string const& name) const = 0;
            
            /*!
             * Retrieves the double value of the identifier with the given name.
             *
             * @param name The name of the double identifier whose value to retrieve.
             * @return The value of the double identifier.
             */
            virtual double getDoubleValue(std::string const& name) const = 0;
            
            /*!
             * Retrieves whether there exists a boolean identifier with the given name in the valuation.
             *
             * @param name The name of the boolean identifier to query.
             * @return True iff the identifier exists and is of boolean type.
             */
            virtual bool containsBooleanIdentifier(std::string const& name) const = 0;
            
            /*!
             * Retrieves whether there exists a integer identifier with the given name in the valuation.
             *
             * @param name The name of the integer identifier to query.
             * @return True iff the identifier exists and is of boolean type.
             */
            virtual bool containsIntegerIdentifier(std::string const& name) const = 0;
            
            /*!
             * Retrieves whether there exists a double identifier with the given name in the valuation.
             *
             * @param name The name of the double identifier to query.
             * @return True iff the identifier exists and is of boolean type.
             */
            virtual bool containsDoubleIdentifier(std::string const& name) const = 0;
            
            /*!
             * Retrieves the number of identifiers in this valuation.
             *
             * @return The number of identifiers in this valuation.
             */
            virtual std::size_t getNumberOfIdentifiers() const = 0;
            
            /*!
             * Retrieves the set of all identifiers contained in this valuation.
             *
             * @return The set of all identifiers contained in this valuation.
             */
            virtual std::set<std::string> getIdentifiers() const = 0;
            
            /*!
             * Retrieves the set of boolean identifiers contained in this valuation.
             *
             * @return The set of boolean identifiers contained in this valuation.
             */
            virtual std::set<std::string> getBooleanIdentifiers() const = 0;

            /*!
             * Retrieves the set of integer identifiers contained in this valuation.
             *
             * @return The set of integer identifiers contained in this valuation.
             */
            virtual std::set<std::string> getIntegerIdentifiers() const = 0;

            /*!
             * Retrieves the set of double identifiers contained in this valuation.
             *
             * @return The set of double identifiers contained in this valuation.
             */
            virtual std::set<std::string> getDoubleIdentifiers() const = 0;

        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_VALUATION_H_ */