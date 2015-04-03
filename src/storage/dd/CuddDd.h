#ifndef STORM_STORAGE_DD_CUDDDD_H_
#define STORM_STORAGE_DD_CUDDDD_H_

#include <set>
#include <memory>

#include "src/storage/dd/Dd.h"
#include "src/storage/expressions/Variable.h"
#include "src/utility/OsDetection.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        // Forward-declare some classes.
        template<DdType Type> class DdManager;
        template<DdType Type> class Odd;
        template<DdType Type> class Bdd;
        
        template<>
        class Dd<DdType::CUDD> {
        public:
            // Declare the DdManager so it can access the internals of a DD.
            friend class DdManager<DdType::CUDD>;
            
            // Instantiate all copy/move constructors/assignments with the default implementation.
            Dd() = default;
            Dd(Dd<DdType::CUDD> const& other) = default;
			Dd& operator=(Dd<DdType::CUDD> const& other) = default;
#ifndef WINDOWS
            Dd(Dd<DdType::CUDD>&& other) = default;
            Dd& operator=(Dd<DdType::CUDD>&& other) = default;
#endif
            
            /*!
             * Retrieves the support of the current DD.
             *
             * @return The support represented as a BDD.
             */
            virtual Bdd<DdType::CUDD> getSupport() const = 0;
            
            /*!
             * Retrieves the number of encodings that are mapped to a non-zero value.
             *
             * @return The number of encodings that are mapped to a non-zero value.
             */
            virtual uint_fast64_t getNonZeroCount() const = 0;
            
            /*!
             * Retrieves the number of leaves of the DD.
             *
             * @return The number of leaves of the DD.
             */
            virtual uint_fast64_t getLeafCount() const = 0;
            
            /*!
             * Retrieves the number of nodes necessary to represent the DD.
             *
             * @return The number of nodes in this DD.
             */
            virtual uint_fast64_t getNodeCount() const = 0;
            
            /*!
             * Retrieves the index of the topmost variable in the DD.
             *
             * @return The index of the topmost variable in DD.
             */
            virtual uint_fast64_t getIndex() const = 0;
            
            /*!
             * Retrieves whether the given meta variable is contained in the DD.
             *
             * @param metaVariable The meta variable for which to query membership.
             * @return True iff the meta variable is contained in the DD.
             */
            bool containsMetaVariable(storm::expressions::Variable const& metaVariable) const;
            
            /*!
             * Retrieves whether the given meta variables are all contained in the DD.
             *
             * @param metaVariables The meta variables for which to query membership.
             * @return True iff all meta variables are contained in the DD.
             */
            bool containsMetaVariables(std::set<storm::expressions::Variable> const& metaVariables) const;
            
            /*!
             * Retrieves the set of all meta variables contained in the DD.
             *
             * @return The set of all meta variables contained in the DD.
             */
            std::set<storm::expressions::Variable> const& getContainedMetaVariables() const;
            
            /*!
             * Exports the DD to the given file in the dot format.
             *
             * @param filename The name of the file to which the DD is to be exported.
             */
            virtual void exportToDot(std::string const& filename = "") const = 0;
            
            /*!
             * Retrieves the manager that is responsible for this DD.
             *
             * A pointer to the manager that is responsible for this DD.
             */
            std::shared_ptr<DdManager<DdType::CUDD> const> getDdManager() const;
            
        protected:
            
            /*!
             * Retrieves the (sorted) list of the variable indices of DD variables contained in this DD.
             *
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
            static std::vector<uint_fast64_t> getSortedVariableIndices(DdManager<DdType::CUDD> const& manager, std::set<storm::expressions::Variable> const& metaVariables);
            
            /*!
             * Retrieves the set of all meta variables contained in the DD.
             *
             * @return The set of all meta variables contained in the DD.
             */
            std::set<storm::expressions::Variable>& getContainedMetaVariables();
            
            /*!
             * Adds the given set of meta variables to the DD.
             *
             * @param metaVariables The set of meta variables to add.
             */
            void addMetaVariables(std::set<storm::expressions::Variable> const& metaVariables);
            
            /*!
             * Adds the given meta variable to the set of meta variables that are contained in this DD.
             *
             * @param metaVariable The name of the meta variable to add.
             */
            void addMetaVariable(storm::expressions::Variable const& metaVariable);
            
            /*!
             * Removes the given meta variable to the set of meta variables that are contained in this DD.
             *
             * @param metaVariable The name of the meta variable to remove.
             */
            void removeMetaVariable(storm::expressions::Variable const& metaVariable);
            
            /*!
             * Removes the given set of meta variables from the DD.
             *
             * @param metaVariables The set of meta variables to remove.
             */
            void removeMetaVariables(std::set<storm::expressions::Variable> const& metaVariables);
            
        protected:
            
            /*!
             * Creates a DD with the given manager and meta variables.
             *
             * @param ddManager The manager responsible for this DD.
             * @param containedMetaVariables The meta variables that appear in the DD.
             */
            Dd(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, std::set<storm::expressions::Variable> const& containedMetaVariables = std::set<storm::expressions::Variable>());
            
        private:
            
            // A pointer to the manager responsible for this DD.
            std::shared_ptr<DdManager<DdType::CUDD> const> ddManager;
            
            // The meta variables that appear in this DD.
            std::set<storm::expressions::Variable> containedMetaVariables;
        };
    }
}

#endif /* STORM_STORAGE_DD_CUDDDD_H_ */