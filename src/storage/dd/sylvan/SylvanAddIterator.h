#ifndef STORM_STORAGE_DD_SYLVANADDITERATOR_H_
#define STORM_STORAGE_DD_SYLVANADDITERATOR_H_

#include "src/storage/dd/AddIterator.h"
#include "src/storage/expressions/SimpleValuation.h"

namespace storm {
    namespace dd {
        // Forward-declare the DdManager class.
        template<DdType Type>
        class DdManager;
        
        template<DdType Type, typename ValueType>
        class InternalAdd;
        
        template<typename ValueType>
        class AddIterator<DdType::Sylvan, ValueType> {
        public:
            friend class InternalAdd<DdType::Sylvan, ValueType>;
            
            // Default-instantiate the constructor.
            AddIterator();
            
            // Forbid copy-construction and copy assignment, because ownership of the internal pointer is unclear then.
            AddIterator(AddIterator<DdType::Sylvan, ValueType> const& other) = delete;
            AddIterator& operator=(AddIterator<DdType::Sylvan, ValueType> const& other) = delete;
            
            // Provide move-construction and move-assignment, though.
            AddIterator(AddIterator<DdType::Sylvan, ValueType>&& other) = default;
            AddIterator& operator=(AddIterator<DdType::Sylvan, ValueType>&& other) = default;
            
            /*!
             * Moves the iterator one position forward.
             */
            AddIterator<DdType::Sylvan, ValueType>& operator++();
            
            /*!
             * Returns a pair consisting of a valuation of meta variables and the value to which this valuation is
             * mapped. Note that the result is returned by value.
             *
             * @return A pair of a valuation and the function value.
             */
            std::pair<storm::expressions::SimpleValuation, ValueType> operator*() const;
            
            /*!
             * Compares the iterator with the given one. Two iterators are considered equal when all their underlying
             * data members are the same or they both are at their end.
             *
             * @param other The iterator with which to compare.
             * @return True if the two iterators are considered equal.
             */
            bool operator==(AddIterator<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Compares the iterator with the given one. Two iterators are considered unequal iff they are not
             * considered equal.
             *
             * @param other The iterator with which to compare.
             * @return True if the two iterators are considered unequal.
             */
            bool operator!=(AddIterator<DdType::Sylvan, ValueType> const& other) const;
            
        private:
            
        };
    }
}

#endif /* STORM_STORAGE_DD_SYLVANADDITERATOR_H_ */