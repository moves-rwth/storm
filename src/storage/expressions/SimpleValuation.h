#ifndef STORM_STORAGE_EXPRESSIONS_SIMPLEVALUATION_H_
#define STORM_STORAGE_EXPRESSIONS_SIMPLEVALUATION_H_

#include <cstdint>
#include <vector>

#include "src/storage/expressions/Valuation.h"

namespace storm {
    namespace expressions {
        
        /*!
         * A simple implementation of the valuation interface.
         */
        class SimpleValuation : public Valuation {
        public:
            friend class SimpleValuationPointerHash;
            friend class SimpleValuationPointerLess;
            
            /*!
             * Creates a new valuation over the non-auxiliary variables of the given manager.
             */
            SimpleValuation(storm::expressions::ExpressionManager const& manager);
            
            /*!
             * Deep-copies the valuation.
             *
             * @param other The valuation to copy
             */
            SimpleValuation(SimpleValuation const& other);

            /*!
             * Checks whether the two valuations are semantically equivalent.
             *
             * @param other The valuation with which to compare.
             * @return True iff the two valuations are semantically equivalent.
             */
            bool operator==(SimpleValuation const& other) const;
            
            // Override virtual functions of base class.
            virtual bool getBooleanValue(Variable const& booleanVariable) const override;
            virtual void setBooleanValue(Variable const& booleanVariable, bool value) override;
            virtual int_fast64_t getIntegerValue(Variable const& integerVariable) const override;
            virtual void setIntegerValue(Variable const& integerVariable, int_fast64_t value) override;
            virtual double getRationalValue(Variable const& rationalVariable) const override;
            virtual void setRationalValue(Variable const& rationalVariable, double value) override;
            
        private:
            // Containers that store the values of the variables of the appropriate type.
            std::unique_ptr<std::vector<bool>> booleanValues;
            std::unique_ptr<std::vector<int_fast64_t>> integerValues;
            std::unique_ptr<std::vector<double>> rationalValues;
        };
        
        /*!
         * A helper class that can pe used as the hash functor for data structures that need to hash valuations given
         * via pointers.
         */
        class SimpleValuationPointerHash {
        public:
            std::size_t operator()(SimpleValuation* valuation) const;
        };
        
        /*!
         * A helper class that can be used as the comparison functor wrt. equality for data structures that need to
         * store pointers to valuations and need to compare the elements wrt. their content (rather than pointer
         * equality).
         */
        class SimpleValuationPointerCompare {
        public:
            bool operator()(SimpleValuation* valuation1, SimpleValuation* valuation2) const;
        };
        
        /*!
         * A helper class that can be used as the comparison functor wrt. "<" for data structures that need to
         * store pointers to valuations and need to compare the elements wrt. their content (rather than pointer
         * equality).
         */
        class SimpleValuationPointerLess {
        public:
            bool operator()(SimpleValuation* valuation1, SimpleValuation* valuation2) const;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_SIMPLEVALUATION_H_ */