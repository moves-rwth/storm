#ifndef STORM_STORAGE_DD_CUDDDDMANAGER_H_
#define STORM_STORAGE_DD_CUDDDDMANAGER_H_

#include <unordered_map>

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/CuddDdMetaVariable.h"
#include "src/utility/OsDetection.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        template<>
        class DdManager<DdType::CUDD> : public std::enable_shared_from_this<DdManager<DdType::CUDD>> {
        public:
            friend class Dd<DdType::CUDD>;
            friend class DdForwardIterator<DdType::CUDD>;
            
            /*!
             * Creates an empty manager without any meta variables.
             */
            DdManager();
            
            // Explictly forbid copying a DdManager, but allow moving it.
            DdManager(DdManager<DdType::CUDD> const& other) = delete;
			DdManager<DdType::CUDD>& operator=(DdManager<DdType::CUDD> const& other) = delete;
#ifndef WINDOWS
            DdManager(DdManager<DdType::CUDD>&& other) = default;
            DdManager<DdType::CUDD>& operator=(DdManager<DdType::CUDD>&& other) = default;
#endif
            
            /*!
             * Retrieves a DD representing the constant one function.
             *
             * @return A DD representing the constant one function.
             */
            Dd<DdType::CUDD> getOne();
            
            /*!
             * Retrieves a DD representing the constant zero function.
             *
             * @return A DD representing the constant zero function.
             */
            Dd<DdType::CUDD> getZero();
            
            /*!
             * Retrieves a DD representing the constant function with the given value.
             *
             * @return A DD representing the constant function with the given value.
             */
            Dd<DdType::CUDD> getConstant(double value);
            
            /*!
             * Retrieves the DD representing the function that maps all inputs which have the given meta variable equal
             * to the given value one.
             *
             * @param metaVariableName The meta variable that is supposed to have the given value.
             * @param value The value the meta variable is supposed to have.
             * @return The DD representing the function that maps all inputs which have the given meta variable equal
             * to the given value one.
             */
            Dd<DdType::CUDD> getEncoding(std::string const& metaVariableName, int_fast64_t value);
            
            /*!
             * Retrieves the DD representing the range of the meta variable, i.e., a function that maps all legal values
             * of the range of the meta variable to one.
             *
             * @param metaVariableName The name of the meta variable whose range to retrieve.
             * @return The range of the meta variable.
             */
            Dd<DdType::CUDD> getRange(std::string const& metaVariableName);

            /*!
             * Retrieves the DD representing the identity of the meta variable, i.e., a function that maps all legal
             * values of the range of the meta variable to themselves.
             *
             * @param metaVariableName The name of the meta variable whose identity to retrieve.
             * @return The identity of the meta variable.
             */
            Dd<DdType::CUDD> getIdentity(std::string const& metaVariableName);
            
            /*!
             * Adds an integer meta variable with the given name and range.
             *
             * @param name The (non-empty) name of the meta variable.
             * @param low The lowest value of the range of the variable.
             * @param high The highest value of the range of the variable.
             */
            void addMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high);
            
            /*!
             * Adds a boolean meta variable with the given name.
             *
             * @param name The (non-empty) name of the meta variable.
             */
            void addMetaVariable(std::string const& name);
            
            /*!
             * Retrieves the names of all meta variables that have been added to the manager.
             *
             * @return The set of all meta variable names of the manager.
             */
            std::set<std::string> getAllMetaVariableNames() const;
            
            /*!
             * Retrieves the number of meta variables that are contained in this manager.
             *
             * @return The number of meta variables contained in this manager.
             */
            std::size_t getNumberOfMetaVariables() const;
            
            /*!
             * Retrieves whether the given meta variable name is already in use.
             *
             * @param metaVariableName The meta variable name whose membership to query.
             * @return True if the given meta variable name is managed by this manager.
             */
            bool hasMetaVariable(std::string const& metaVariableName) const;
            
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
            
        protected:
            /*!
             * Retrieves the meta variable with the given name if it exists.
             *
             * @param metaVariableName The name of the meta variable to retrieve.
             * @return The meta variable with the given name.
             */
            DdMetaVariable<DdType::CUDD> const& getMetaVariable(std::string const& metaVariableName) const;
            
        private:
            /*!
             * Retrieves a list of names of the DD variables in the order of their index.
             *
             * @return A list of DD variable names.
             */
            std::vector<std::string> getDdVariableNames() const;
            
            /*!
             * Retrieves the underlying CUDD manager.
             *
             * @return The underlying CUDD manager.
             */
            Cudd& getCuddManager();
            
            /*!
             * Retrieves the underlying CUDD manager.
             *
             * @return The underlying CUDD manager.
             */
            Cudd const& getCuddManager() const;
            
            // A mapping from variable names to the meta variable information.
            std::unordered_map<std::string, DdMetaVariable<DdType::CUDD>> metaVariableMap;
            
            // The manager responsible for the DDs created/modified with this DdManager.
            Cudd cuddManager;
        };
    }
}

#endif /* STORM_STORAGE_DD_CUDDDDMANAGER_H_ */