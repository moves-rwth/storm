#ifndef STORM_STORAGE_DD_ADD_H_
#define STORM_STORAGE_DD_ADD_H_

#include "src/storage/dd/Dd.h"
#include "src/storage/dd/DdType.h"

#include "src/storage/dd/cudd/InternalCuddAdd.h"

namespace storm {
    namespace dd {
        template<DdType LibraryType, typename ValueType>
        class Add {
            friend class DdManager<LibraryType>;
            
            // Instantiate all copy/move constructors/assignments with the default implementation.
            Add() = default;
            Add(Add<LibraryType> const& other) = default;
            Add& operator=(Add<LibraryType> const& other) = default;
            Add(Add<LibraryType>&& other) = default;
            Add& operator=(Add<LibraryType>&& other) = default;
        private:
            /*!
             * We provide a conversion operator from the BDD to its internal type to ease calling the internal functions.
             */
            operator InternalAdd<LibraryType, ValueType>();
            operator InternalAdd<LibraryType, ValueType> const() const;
            
            // The internal ADD that depends on the chosen library.
            InternalAdd<LibraryType> internalAdd;
        };
    }
}

#endif /* STORM_STORAGE_DD_ADD_H_ */