#ifndef STORM_STORAGE_DD_CUDDDDFORWARDITERATOR_H_
#define STORM_STORAGE_DD_CUDDDDFORWARDITERATOR_H_

#include <memory>
#include <cstdint>

#include "src/storage/dd/DdForwardIterator.h"
#include "src/storage/expressions/SimpleValuation.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        template<>
        class DdForwardIterator<DdType::CUDD> {
        public:
            // Forward-declare the DdManager class.
            template<DdType Type> class DdManager;
            
            DdForwardIterator<DdType::CUDD>& operator++();
            storm::expressions::SimpleValuation operator*() const;
            bool operator==(DdForwardIterator<DdType::CUDD> const& other) const;
            bool operator!=(DdForwardIterator<DdType::CUDD> const& other) const;
            
        private:
            DdForwardIterator(std::shared_ptr<DdManager<DdType::CUDD>> ddManager, ADD cuddAdd, std::vector<ADD> const& relevantDdVariables);
            
            std::shared_ptr<DdManager<DdType::CUDD>> ddManager;
            
            double value;
            
            int* cube;
            
            uint_fast64_t positionInCube;

            ADD cuddAdd;
            
            bool isAtEnd;
            
            DdGen* generator;
        };
    }
}

#endif /* STORM_STORAGE_DD_CUDDDDFORWARDITERATOR_H_ */