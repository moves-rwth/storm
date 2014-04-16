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
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_VALUATION_H_ */