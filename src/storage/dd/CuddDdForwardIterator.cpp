#include "src/storage/dd/CuddDdForwardIterator.h"

namespace storm {
    namespace dd {
        DdForwardIterator<DdType::CUDD>::DdForwardIterator(std::shared_ptr<DdManager<DdType::CUDD>> ddManager, ADD cuddAdd) : ddManager(ddManager), value(0), cube(nullptr), cuddAdd(cuddAdd), isAtEnd(false), generator(nullptr) {
            // Start by getting the first cube.
            this->generator = this->cuddAdd.FirstCube(&cube, &value);
            
            // If the generator is already empty, we set the corresponding flag.
            this->isAtEnd = Cudd_IsGenEmpty(generator);
        }
        
        DdForwardIterator<DdType::CUDD>& DdForwardIterator<DdType::CUDD>::operator++() {
            // TODO: eliminate current
        }
        
        bool DdForwardIterator<DdType::CUDD>::operator==(DdForwardIterator<DdType::CUDD> const& other) const {
            if (this->isAtEnd && other.isAtEnd) {
                return true;
            } else {
                return this->cuddAdd == other.cuddAdd;
            }
        }
        
        bool DdForwardIterator<DdType::CUDD>::operator!=(DdForwardIterator<DdType::CUDD> const& other) const {
            return !(*this == other);
        }
        
        storm::expressions::SimpleValuation DdForwardIterator<DdType::CUDD>::operator*() const {
            // FIXME: construct valuation and return it.
        }
    }
}