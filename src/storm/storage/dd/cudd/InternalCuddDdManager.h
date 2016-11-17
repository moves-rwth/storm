#ifndef STORM_STORAGE_DD_INTERNALCUDDDDMANAGER_H_
#define STORM_STORAGE_DD_INTERNALCUDDDDMANAGER_H_

#include "src/storage/dd/DdType.h"
#include "src/storage/dd/InternalDdManager.h"

#include "src/storage/dd/cudd/InternalCuddBdd.h"
#include "src/storage/dd/cudd/InternalCuddAdd.h"

#include "cuddObj.hh"

namespace storm {
    namespace dd {
        template<DdType LibraryType, typename ValueType>
        class InternalAdd;

        template<DdType LibraryType>
        class InternalBdd;

        template<>
        class InternalDdManager<DdType::CUDD> {
        public:
            friend class InternalBdd<DdType::CUDD>;

            template<DdType LibraryType, typename ValueType>
            friend class InternalAdd;

            /*!
             * Creates a new internal manager for CUDD DDs.
             */
            InternalDdManager();
            
            /*!
             * Destroys the CUDD manager.
             */
            ~InternalDdManager();
            
            /*!
             * Retrieves a BDD representing the constant one function.
             *
             * @return A BDD representing the constant one function.
             */
            InternalBdd<DdType::CUDD> getBddOne() const;
            
            /*!
             * Retrieves an ADD representing the constant one function.
             *
             * @return An ADD representing the constant one function.
             */
            template<typename ValueType>
            InternalAdd<DdType::CUDD, ValueType> getAddOne() const;
            
            /*!
             * Retrieves a BDD representing the constant zero function.
             *
             * @return A BDD representing the constant zero function.
             */
            InternalBdd<DdType::CUDD> getBddZero() const;
            
            /*!
             * Retrieves an ADD representing the constant zero function.
             *
             * @return An ADD representing the constant zero function.
             */
            template<typename ValueType>
            InternalAdd<DdType::CUDD, ValueType> getAddZero() const;
            
            /*!
             * Retrieves an ADD representing the constant function with the given value.
             *
             * @return An ADD representing the constant function with the given value.
             */
            template<typename ValueType>
            InternalAdd<DdType::CUDD, ValueType> getConstant(ValueType const& value) const;
            
            /*!
             * Creates a new pair of DD variables and returns the two cubes as a result.
             *
             * @return The two cubes belonging to the DD variables.
             */
            std::pair<InternalBdd<DdType::CUDD>, InternalBdd<DdType::CUDD>> createNewDdVariablePair();
            
            /*!
             * Sets whether or not dynamic reordering is allowed for the DDs managed by this manager.
             *
             * @param value If set to true, dynamic reordering is allowed and forbidden otherwise.
             */
            void allowDynamicReordering(bool value);
            
            /*!
             * Retrieves whether dynamic reordering is currently allowed.
             *
             * @return True iff dynamic reordering is currently allowed.
             */
            bool isDynamicReorderingAllowed() const;
            
            /*!
             * Triggers a reordering of the DDs managed by this manager.
             */
            void triggerReordering();
            
        private:
            /*!
             * Retrieves the underlying CUDD manager.
             *
             * @return The underlying CUDD manager.
             */
            cudd::Cudd& getCuddManager();
            
            /*!
             * Retrieves the underlying CUDD manager.
             *
             * @return The underlying CUDD manager.
             */
            cudd::Cudd const& getCuddManager() const;
            
            // The manager responsible for the DDs created/modified with this DdManager.
            cudd::Cudd cuddManager;
            
            // The technique that is used for dynamic reordering.
            Cudd_ReorderingType reorderingTechnique;
        };        
    }
}

#endif /* STORM_STORAGE_DD_INTERNALCUDDDDMANAGER_H_ */