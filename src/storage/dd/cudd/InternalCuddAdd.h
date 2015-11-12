#ifndef STORM_STORAGE_DD_CUDD_INTERNALCUDDADD_H_
#define STORM_STORAGE_DD_CUDD_INTERNALCUDDADD_H_

#include <set>

#include "src/storage/dd/DdType.h"
#include "src/storage/dd/InternalAdd.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace storage {
        template<typename T> class SparseMatrix;
        class BitVector;
        template<typename E, typename V> class MatrixEntry;
    }
    
    namespace dd {
        // Forward-declare some classes.
        template<DdType Type> class DdManager;
        template<DdType Type> class Odd;
        template<DdType Type> class Bdd;
        
        template<typename ValueType>
        class InternalAdd<DdType::CUDD, ValueType> : public Dd<DdType::CUDD> {
        public:
            /*!
             * Creates an ADD that encapsulates the given CUDD ADD.
             *
             * @param ddManager The manager responsible for this DD.
             * @param cuddAdd The CUDD ADD to store.
             * @param containedMetaVariables The meta variables that appear in the DD.
             */
            Add(InternalDdManager<DdType::CUDD> const* ddManager, ADD cuddAdd);
            
            
        private:
            /*!
             * Retrieves the CUDD ADD object associated with this ADD.
             *
             * @return The CUDD ADD object associated with this ADD.
             */
            ADD getCuddAdd() const;
            
            /*!
             * Retrieves the raw DD node of CUDD associated with this ADD.
             *
             * @return The DD node of CUDD associated with this ADD.
             */
            DdNode* getCuddDdNode() const;
            
            InternalDdManager<DdType::CUDD> const* ddManager;
            
            ADD cuddBdd;
        }
    }
}

#endif /* STORM_STORAGE_DD_CUDD_INTERNALCUDDADD_H_ */