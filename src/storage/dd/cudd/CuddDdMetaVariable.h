#ifndef STORM_STORAGE_DD_DDMETAVARIABLE_H_
#define STORM_STORAGE_DD_DDMETAVARIABLE_H_

#include <memory>
#include <vector>
#include <cstdint>
#include <string>

#include "utility/OsDetection.h"
#include "src/storage/dd/cudd/CuddBdd.h"
#include "src/storage/dd/DdMetaVariable.h"
#include "src/storage/dd/cudd/CuddDdForwardIterator.h"


namespace storm {
    namespace dd {
        // Forward-declare some classes.
        template<DdType Type> class DdManager;
        template<DdType Type> class Odd;
        template<DdType Type> class Add;
        
        template<>
        class DdMetaVariable<DdType::CUDD> {
        public:
            // Declare the DdManager class as friend so it can access the internals of a meta variable.
            friend class DdManager<DdType::CUDD>;
            friend class Dd<DdType::CUDD>;
            friend class Bdd<DdType::CUDD>;
            friend class Add<DdType::CUDD>;
            friend class Odd<DdType::CUDD>;
            friend class DdForwardIterator<DdType::CUDD>;
            
            // An enumeration for all legal types of meta variables.
            enum class MetaVariableType { Bool, Int };
            
            /*!
             * Creates an integer meta variable with the given name and range bounds.
             *
             * @param name The name of the meta variable.
             * @param low The lowest value of the range of the variable.
             * @param high The highest value of the range of the variable.
             * @param ddVariables The vector of variables used to encode this variable.
             * @param manager A pointer to the manager that is responsible for this meta variable.
             */
            DdMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high, std::vector<Bdd<DdType::CUDD>> const& ddVariables, std::shared_ptr<DdManager<DdType::CUDD>> manager);
            
            /*!
             * Creates a boolean meta variable with the given name.
             * @param name The name of the meta variable.
             * @param ddVariables The vector of variables used to encode this variable.
             * @param manager A pointer to the manager that is responsible for this meta variable.
             */
            DdMetaVariable(std::string const& name, std::vector<Bdd<DdType::CUDD>> const& ddVariables, std::shared_ptr<DdManager<DdType::CUDD>> manager);
            
            // Explictly generate all default versions of copy/move constructors/assignments.
            DdMetaVariable(DdMetaVariable const& other) = default;
			DdMetaVariable& operator=(DdMetaVariable const& other) = default;
#ifndef WINDOWS
            DdMetaVariable(DdMetaVariable&& other) = default;
            DdMetaVariable& operator=(DdMetaVariable&& other) = default;
#endif
            
            /*!
             * Retrieves the name of the meta variable.
             *
             * @return The name of the variable.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves the type of the meta variable.
             *
             * @return The type of the meta variable.
             */
            MetaVariableType getType() const;
            
            /*!
             * Retrieves the lowest value of the range of the variable.
             *
             * @return The lowest value of the range of the variable.
             */
            int_fast64_t getLow() const;
            
            /*!
             * Retrieves the highest value of the range of the variable.
             *
             * @return The highest value of the range of the variable.
             */
            int_fast64_t getHigh() const;
            
            /*!
             * Retrieves the manager that is responsible for this meta variable.
             *
             * A pointer to the manager that is responsible for this meta variable.
             */
            std::shared_ptr<DdManager<DdType::CUDD>> getDdManager() const;
            
            /*!
             * Retrieves the number of DD variables for this meta variable.
             *
             * @return The number of DD variables for this meta variable.
             */
            std::size_t getNumberOfDdVariables() const;
            
        private:
            /*!
             * Retrieves the variables used to encode the meta variable.
             *
             * @return A vector of variables used to encode the meta variable.
             */
            std::vector<Bdd<DdType::CUDD>> const& getDdVariables() const;
            
            /*!
             * Retrieves the cube of all variables that encode this meta variable.
             *
             * @return The cube of all variables that encode this meta variable.
             */
            Bdd<DdType::CUDD> const& getCube() const;
            
            // The name of the meta variable.
            std::string name;
            
            // The type of the variable.
            MetaVariableType type;
            
            // The lowest value of the range of the variable.
            int_fast64_t low;
            
            // The highest value of the range of the variable.
            int_fast64_t high;
            
            // The vector of variables that are used to encode the meta variable.
            std::vector<Bdd<DdType::CUDD>> ddVariables;
            
            // The cube consisting of all variables that encode the meta variable.
            Bdd<DdType::CUDD> cube;
            
            // A pointer to the manager responsible for this meta variable.
            std::shared_ptr<DdManager<DdType::CUDD>> manager;
        };
    }
}

#endif /* STORM_STORAGE_DD_DDMETAVARIABLE_H_ */