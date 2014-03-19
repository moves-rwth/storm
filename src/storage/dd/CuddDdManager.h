#ifndef STORM_STORAGE_DD_CUDDDDMANAGER_H_
#define STORM_STORAGE_DD_CUDDDDMANAGER_H_

#include <unordered_set>
#include <unordered_map>

#include "src/storage/dd/DdManager.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        // To break the cylic dependencies, we need to forward-declare the other DD-related classes.
        template<DdType Type> class DdMetaVariable;
        template<DdType Type> class Dd;
        
        template<>
        class DdManager<CUDD> : std::enable_shared_from_this<DdManager<CUDD>> {
            /*!
             * Creates an empty manager without any meta variables.
             */
            DdManager();
            
            // Explictly forbid copying a DdManager, but allow moving it.
            DdManager(DdManager<CUDD> const& other) = delete;
            DdManager(DdManager<CUDD>&& other) = default;
            DdManager<CUDD>& operator=(DdManager<CUDD> const& other) = delete;
            DdManager<CUDD>& operator=(DdManager<CUDD>&& other) = default;
            
            /*!
             * Retrieves a DD representing the constant one function.
             *
             * @return A DD representing the constant one function.
             */
            Dd<CUDD> getOne();
            
            /*!
             * Retrieves a DD representing the constant zero function.
             *
             * @return A DD representing the constant zero function.
             */
            Dd<CUDD> getZero();
            
            /*!
             * Retrieves a DD representing the constant function with the given value.
             *
             * @return A DD representing the constant function with the given value.
             */
            Dd<CUDD> getConstant(double value);

            /*!
             * Adds a meta variable with the given name and range.
             *
             * @param name The name of the meta variable.
             * @param low The lowest value of the range of the variable.
             * @param high The highest value of the range of the variable.
             * @param addSuccessorVariable If set, a second meta variable is added. This can then be used, for example,
             * to encode the value of the meta variable in a successor state.
             * @param useInterleavedVariableOrdering If set, the variables used for the successor meta variable are
             * interleaved with the ones for the added meta variable.
             */
            void addMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high, bool addSuccessorVariable = false, bool useInterleavedVariableOrdering = true);
            
            /*!
             * Retrieves the meta variable with the given name if it exists.
             *
             * @param metaVariableName The name of the meta variable to retrieve.
             * @return The meta variable with the given name.
             */
            DdMetaVariable<CUDD> const& getMetaVariable(std::string const& metaVariableName) const;
            
            /*!
             * Retrieves the successor meta variable of the one with the given name if it exists.
             *
             * @param metaVariableName The name of the meta variable whose successor meta variable to retrieve.
             * @return The successor meta variable of the one with the given name.
             */
            DdMetaVariable<CUDD> const& getSuccessorMetaVariable(std::string const& metaVariableName) const;
            
            /*!
             * Retrieves the names of all meta variables that have been added to the manager.
             *
             * @return The set of all meta variable names of the manager.
             */
            std::unordered_set<std::string> getAllMetaVariableNames();
            
        private:
            // A mapping from variable names to the meta variable information.
            std::unordered_map<std::string, DdMetaVariable<CUDD>> metaVariableMap;
            
            // The manager responsible for the DDs created/modified with this DdManager.
            Cudd cuddManager;
        };
    }
}

#endif /* STORM_STORAGE_DD_CUDDDDMANAGER_H_ */