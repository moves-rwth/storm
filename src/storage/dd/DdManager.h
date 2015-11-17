#ifndef STORM_STORAGE_DD_DDMANAGER_H_
#define STORM_STORAGE_DD_DDMANAGER_H_

#include <set>
#include <unordered_map>

#include "src/storage/dd/DdType.h"
#include "src/storage/dd/DdMetaVariable.h"
#include "src/storage/dd/Bdd.h"
#include "src/storage/dd/Add.h"
#include "src/storage/dd/AddIterator.h"

#include "src/storage/expressions/Variable.h"

#include "src/storage/dd/cudd/InternalCuddDdManager.h"

namespace storm {
    namespace dd {
        template<DdType LibraryType>
        class Odd;
        
        // Declare DdManager class so we can then specialize it for the different DD types.
        template<DdType LibraryType>
        class DdManager : public std::enable_shared_from_this<DdManager<LibraryType>> {
        public:
            friend class Bdd<LibraryType>;
            friend class Add<LibraryType, double>;
            friend class AddIterator<LibraryType, double>;
            friend class Odd<LibraryType>;
            
            /*!
             * Creates an empty manager without any meta variables.
             */
            DdManager();
            
            // Explictly forbid copying a DdManager, but allow moving it.
            DdManager(DdManager<LibraryType> const& other) = delete;
            DdManager<LibraryType>& operator=(DdManager<LibraryType> const& other) = delete;
            DdManager(DdManager<LibraryType>&& other) = default;
            DdManager<LibraryType>& operator=(DdManager<LibraryType>&& other) = default;
            
            /*!
             * Retrieves a BDD representing the constant one function.
             *
             * @return A BDD representing the constant one function.
             */
            Bdd<LibraryType> getBddOne() const;
            
            /*!
             * Retrieves an ADD representing the constant one function.
             *
             * @return An ADD representing the constant one function.
             */
            template<typename ValueType>
            Add<LibraryType, ValueType> getAddOne() const;
            
            /*!
             * Retrieves a BDD representing the constant zero function.
             *
             * @return A BDD representing the constant zero function.
             */
            Bdd<LibraryType> getBddZero() const;
            
            /*!
             * Retrieves an ADD representing the constant zero function.
             *
             * @return An ADD representing the constant zero function.
             */
            template<typename ValueType>
            Add<LibraryType, ValueType> getAddZero() const;
            
            /*!
             * Retrieves an ADD representing the constant function with the given value.
             *
             * @return An ADD representing the constant function with the given value.
             */
            template<typename ValueType>
            Add<LibraryType, ValueType> getConstant(ValueType const& value) const;
            
            /*!
             * Retrieves the BDD representing the function that maps all inputs which have the given meta variable equal
             * to the given value one.
             *
             * @param variable The expression variable associated with the meta variable.
             * @param value The value the meta variable is supposed to have.
             * @return The DD representing the function that maps all inputs which have the given meta variable equal
             * to the given value one.
             */
            Bdd<LibraryType> getEncoding(storm::expressions::Variable const& variable, int_fast64_t value) const;
            
            /*!
             * Retrieves the BDD representing the range of the meta variable, i.e., a function that maps all legal values
             * of the range of the meta variable to one.
             *
             * @param variable The expression variable associated with the meta variable.
             * @return The range of the meta variable.
             */
            Bdd<LibraryType> getRange(storm::expressions::Variable const& variable) const;
            
            /*!
             * Retrieves the ADD representing the identity of the meta variable, i.e., a function that maps all legal
             * values of the range of the meta variable to themselves.
             *
             * @param variable The expression variable associated with the meta variable.
             * @return The identity of the meta variable.
             */
            template<typename ValueType>
            Add<LibraryType, ValueType> getIdentity(storm::expressions::Variable const& variable) const;
            
            /*!
             * Adds an integer meta variable with the given range.
             *
             * @param variableName The name of the new variable.
             * @param low The lowest value of the range of the variable.
             * @param high The highest value of the range of the variable.
             */
            std::pair<storm::expressions::Variable, storm::expressions::Variable> addMetaVariable(std::string const& variableName, int_fast64_t low, int_fast64_t high);
            
            /*!
             * Adds a boolean meta variable.
             *
             * @param variableName The name of the new variable.
             */
            std::pair<storm::expressions::Variable, storm::expressions::Variable> addMetaVariable(std::string const& variableName);
            
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
             * @param variableName The name of the variable.
             * @return True if the given meta variable name is managed by this manager.
             */
            bool hasMetaVariable(std::string const& variableName) const;
            
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
            
            /*!
             * Retrieves the meta variable with the given name if it exists.
             *
             * @param variable The expression variable associated with the meta variable.
             * @return The corresponding meta variable.
             */
            DdMetaVariable<LibraryType> const& getMetaVariable(storm::expressions::Variable const& variable) const;
            
            /*!
             * Retrieves the manager as a shared pointer.
             *
             * @return A shared pointer to the manager.
             */
            std::shared_ptr<DdManager<LibraryType> const> asSharedPointer() const;
            
            std::set<storm::expressions::Variable> getAllMetaVariables() const;
            
            /*!
             * Retrieves the (sorted) list of the variable indices of the DD variables given by the meta variable set.
             *
             * @param manager The manager responsible for the DD.
             * @param metaVariable The set of meta variables for which to retrieve the index list.
             * @return The sorted list of variable indices.
             */
            std::vector<uint_fast64_t> getSortedVariableIndices() const;
            
            /*!
             * Retrieves the (sorted) list of the variable indices of the DD variables given by the meta variable set.
             *
             * @param manager The manager responsible for the DD.
             * @param metaVariable The set of meta variables for which to retrieve the index list.
             * @return The sorted list of variable indices.
             */
            std::vector<uint_fast64_t> getSortedVariableIndices(std::set<storm::expressions::Variable> const& metaVariables) const;
            
            /*!
             * Retrieves the internal DD manager.
             *
             * @return The internal DD manager.
             */
            InternalDdManager<LibraryType>& getInternalDdManager();

            /*!
             * Retrieves the internal DD manager.
             *
             * @return The internal DD manager.
             */
            InternalDdManager<LibraryType> const& getInternalDdManager() const;

        private:
            /*!
             * Retrieves a list of names of the DD variables in the order of their index.
             *
             * @return A list of DD variable names.
             */
            std::vector<std::string> getDdVariableNames() const;
            
            /*!
             * Retrieves a list of expression variables in the order of their index.
             *
             * @return A list of DD variables.
             */
            std::vector<storm::expressions::Variable> getDdVariables() const;
            
            /*!
             * Retrieves the underlying expression manager.
             *
             * @return The underlying expression manager.
             */
            storm::expressions::ExpressionManager const& getExpressionManager() const;
            
            /*!
             * Retrieves the underlying expression manager.
             *
             * @return The underlying expression manager.
             */
            storm::expressions::ExpressionManager& getExpressionManager();
            
            InternalDdManager<LibraryType>* getInternalDdManagerPointer();
            InternalDdManager<LibraryType> const* getInternalDdManagerPointer() const;
            
            // A mapping from variables to the meta variable information.
            std::unordered_map<storm::expressions::Variable, DdMetaVariable<LibraryType>> metaVariableMap;
            
            // The manager responsible for the variables.
            std::shared_ptr<storm::expressions::ExpressionManager> manager;
            
            // The DD manager that is customized according to the selected library type.
            InternalDdManager<LibraryType> internalDdManager;
        };
    }
}

#endif /* STORM_STORAGE_DD_DDMANAGER_H_ */