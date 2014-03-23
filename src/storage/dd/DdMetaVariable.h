#ifndef STORM_STORAGE_DD_DDMETAVARIABLE_H_
#define STORM_STORAGE_DD_DDMETAVARIABLE_H_

#include <memory>
#include <vector>
#include <cstdint>
#include <string>

#include "src/storage/dd/CuddDd.h"

namespace storm {
    namespace dd {
        // Forward-declare the DdManager class.
        template<DdType Type> class DdManager;
        
        template <DdType Type>
        class DdMetaVariable {
        public:
            // Declare the DdManager class as friend so it can access the internals of a meta variable.
            friend class DdManager<Type>;
            friend class Dd<Type>;
            
            /*!
             * Creates a meta variable with the given name, range bounds.
             *
             * @param name The name of the meta variable.
             * @param low The lowest value of the range of the variable.
             * @param high The highest value of the range of the variable.
             * @param ddVariables The vector of variables used to encode this variable.
             * @param manager A pointer to the manager that is responsible for this meta variable.
             */
            DdMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high, std::vector<Dd<Type>> const& ddVariables, std::shared_ptr<DdManager<Type>> manager) noexcept;
            
            // Explictly generate all default versions of copy/move constructors/assignments.
            DdMetaVariable(DdMetaVariable const& other) = default;
            DdMetaVariable(DdMetaVariable&& other) = default;
            DdMetaVariable& operator=(DdMetaVariable const& other) = default;
            DdMetaVariable& operator=(DdMetaVariable&& other) = default;
            
            /*!
             * Retrieves the name of the meta variable.
             *
             * @return The name of the variable.
             */
            std::string const& getName() const;
            
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
            std::shared_ptr<DdManager<Type>> getDdManager() const;
            
            
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
            std::vector<Dd<Type>> const& getDdVariables() const;
            
            /*!
             * Retrieves the cube of all variables that encode this meta variable.
             *
             * @return The cube of all variables that encode this meta variable.
             */
            Dd<Type> const& getCube() const;

            // The name of the meta variable.
            std::string name;
            
            // The lowest value of the range of the variable.
            int_fast64_t low;
            
            // The highest value of the range of the variable.
            int_fast64_t high;
            
            // The vector of variables that are used to encode the meta variable.
            std::vector<Dd<Type>> ddVariables;
            
            // The cube consisting of all variables that encode the meta variable.
            Dd<Type> cube;
            
            // A pointer to the manager responsible for this meta variable.
            std::shared_ptr<DdManager<Type>> manager;
        };
    }
}

#endif /* STORM_STORAGE_DD_DDMETAVARIABLE_H_ */