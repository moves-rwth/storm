#ifndef STORM_STORAGE_EXPRESSIONS_VALUATION_H_
#define STORM_STORAGE_EXPRESSIONS_VALUATION_H_

#include <cstdint>
#include <vector>

namespace storm {
    namespace expressions {
        class ExpressionManager;
        class Variable;
        
        /*!
         * A class to store a valuation of variables. This is, for example, used for evaluating expressions.
         */
        class Valuation {
        public:
            friend class ValuationPointerHash;
            friend class ValuationPointerLess;
            
            /*!
             * Creates a valuation of all non-auxiliary variables managed by the given manager. If the manager is
             * modified in the sense that additional variables are added, all valuations over its variables are
             * invalidated.
             *
             * @param manager The manager of the variables.
             */
            Valuation(ExpressionManager const& manager);
            
            /*!
             * Deep-copies the valuation.
             *
             * @param other The valuation to copy
             */
            Valuation(Valuation const& other);
            
            /*!
             * Checks whether the two valuations are semantically equivalent.
             *
             * @param other The valuation with which to compare.
             * @return True iff the two valuations are semantically equivalent.
             */
            bool operator==(Valuation const& other) const;
            
            /*!
             * Retrieves the value of the given boolean variable.
             *
             * @param booleanVariable The boolean variable whose value to retrieve.
             * @return The value of the boolean variable.
             */
            bool getBooleanValue(Variable const& booleanVariable) const;
            
            /*!
             * Sets the value of the given boolean variable to the provided value.
             *
             * @param booleanVariable The variable whose value to set.
             * @param value The new value of the variable.
             */
            void setBooleanValue(Variable const& booleanVariable, bool value);
            
            /*!
             * Retrieves the value of the given integer variable.
             *
             * @param integerVariable The integer variable whose value to retrieve.
             * @return The value of the integer variable.
             */
            int_fast64_t getIntegerValue(Variable const& integerVariable) const;

            /*!
             * Sets the value of the given boolean variable to the provided value.
             *
             * @param integerVariable The variable whose value to set.
             * @param value The new value of the variable.
             */
            void setIntegerValue(Variable const& integerVariable, int_fast64_t value);
            
            /*!
             * Retrieves the value of the given rational variable.
             *
             * @param rationalVariable The rational variable whose value to retrieve.
             * @return The value of the rational variable.
             */
            double getRationalValue(Variable const& rationalVariable) const;
            
            /*!
             * Sets the value of the given boolean variable to the provided value.
             *
             * @param integerVariable The variable whose value to set.
             * @param value The new value of the variable.
             */
            void setRationalValue(Variable const& rationalVariable, double value);
            
            /*!
             * Retrieves the manager responsible for the variables of this valuation.
             *
             * @return The manager.
             */
            ExpressionManager const& getManager() const;

        private:
            // The manager responsible for the variables of this valuation.
            ExpressionManager const& manager;
            
            // Containers that store the values of the variables of the appropriate type.
            std::unique_ptr<std::vector<bool>> booleanValues;
            std::unique_ptr<std::vector<int_fast64_t>> integerValues;
            std::unique_ptr<std::vector<double>> rationalValues;
        };
        
        /*!
         * A helper class that can pe used as the hash functor for data structures that need to hash valuations given
         * via pointers.
         */
        class ValuationPointerHash {
        public:
            std::size_t operator()(Valuation* valuation) const;
        };
        
        /*!
         * A helper class that can be used as the comparison functor wrt. equality for data structures that need to
         * store pointers to valuations and need to compare the elements wrt. their content (rather than pointer
         * equality).
         */
        class ValuationPointerCompare {
        public:
            bool operator()(Valuation* valuation1, Valuation* valuation2) const;
        };
        
        /*!
         * A helper class that can be used as the comparison functor wrt. "<" for data structures that need to
         * store pointers to valuations and need to compare the elements wrt. their content (rather than pointer
         * equality).
         */
        class ValuationPointerLess {
        public:
            bool operator()(Valuation* valuation1, Valuation* valuation2) const;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_VALUATION_H_ */