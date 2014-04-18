#ifndef STORM_STORAGE_DD_CUDDDDFORWARDITERATOR_H_
#define STORM_STORAGE_DD_CUDDDDFORWARDITERATOR_H_

#include <memory>
#include <cstdint>
#include <set>
#include <tuple>
#include <utility>

#include "src/storage/dd/DdForwardIterator.h"
#include "src/storage/expressions/SimpleValuation.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        // Forward-declare the DdManager class.
        template<DdType Type> class DdManager;
        template<DdType Type> class Dd;
        
        template<>
        class DdForwardIterator<DdType::CUDD> {
        public:
            friend class Dd<DdType::CUDD>;

            // Default-instantiate the constructor.
            DdForwardIterator() = default;
            
            // Forbid copy-construction and copy assignment, because ownership of the internal pointer is unclear then.
            DdForwardIterator(DdForwardIterator<DdType::CUDD> const& other) = delete;
            DdForwardIterator& operator=(DdForwardIterator<DdType::CUDD> const& other) = delete;
            
            // Provide move-construction and move-assignment, though.
            DdForwardIterator(DdForwardIterator<DdType::CUDD>&& other);
            DdForwardIterator& operator=(DdForwardIterator<DdType::CUDD>&& other);
            
            /*!
             * Destroys the forward iterator and frees the generator as well as the cube if they are not the nullptr.
             */
            ~DdForwardIterator();
            
            /*!
             * Moves the iterator one position forward.
             */
            DdForwardIterator<DdType::CUDD>& operator++();
            
            /*!
             * Returns a pair consisting of a valuation of meta variables and the value to which this valuation is
             * mapped. Note that the result is returned by value.
             *
             * @return A pair of a valuation and the function value.
             */
            std::pair<storm::expressions::SimpleValuation, double> operator*() const;
            
            /*!
             * Compares the iterator with the given one. Two iterators are considered equal when all their underlying
             * data members are the same or they both are at their end.
             *
             * @param other The iterator with which to compare.
             * @return True if the two iterators are considered equal.
             */
            bool operator==(DdForwardIterator<DdType::CUDD> const& other) const;
            
            /*!
             * Compares the iterator with the given one. Two iterators are considered unequal iff they are not
             * considered equal.
             *
             * @param other The iterator with which to compare.
             * @return True if the two iterators are considered unequal.
             */
            bool operator!=(DdForwardIterator<DdType::CUDD> const& other) const;
            
        private:
            /*!
             * Constructs a forward iterator using the given generator with the given set of relevant meta variables.
             *
             * @param ddManager The manager responsible for the DD over which to iterate.
             * @param generator The generator used to enumerate the cubes of the DD.
             * @param cube The cube as represented by CUDD.
             * @param value The value the cube is mapped to.
             * @param isAtEnd A flag that indicates whether the iterator is at its end and may not be moved forward any
             * more.
             * @param metaVariables The meta variables that appear in the DD.
             */
            DdForwardIterator(std::shared_ptr<DdManager<DdType::CUDD>> ddManager, DdGen* generator, int* cube, double value, bool isAtEnd, std::set<std::string> const* metaVariables = nullptr);
            
            /*!
             * Recreates the internal information when a new cube needs to be treated.
             */
            void treatNewCube();
            
            /*!
             * Updates the internal information when the next solution of the current cube needs to be treated.
             */
            void treatNextInCube();
            
            // The manager responsible for the meta variables (and therefore the underlying DD).
            std::shared_ptr<DdManager<DdType::CUDD>> ddManager;
            
            // The CUDD generator used to enumerate the cubes of the DD.
            DdGen* generator;
            
            // The currently considered cube of the DD.
            int* cube;

            // The function value of the current cube.
            double value;
            
            // A flag that indicates whether the iterator is at its end and may not be moved further. This is also used
            // for the check against the end iterator.
            bool isAtEnd;
            
            // The set of meta variables appearing in the DD.
            std::set<std::string> const* metaVariables;
            
            // A number that represents how many assignments of the current cube have already been returned previously.
            // This is needed, because cubes may represent many assignments (if they have don't care variables).
            uint_fast64_t cubeCounter;
            
            // A vector of tuples of the form <variable, metaVariableName, bitIndex>.
            std::vector<std::tuple<ADD, std::string, uint_fast64_t>> relevantDontCareDdVariables;
            
            // The current valuation of meta variables.
            storm::expressions::SimpleValuation currentValuation;
        };
    }
}

#endif /* STORM_STORAGE_DD_CUDDDDFORWARDITERATOR_H_ */