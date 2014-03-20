#include "src/storage/dd/DdMetaVariable.h"

namespace storm {
    namespace dd {
        template<DdType Type>
        DdMetaVariable<Type>::DdMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high, std::vector<Dd<Type>> const& ddVariables, std::shared_ptr<DdManager<Type>> manager) noexcept : name(name), low(low), high(high), ddVariables(ddVariables), manager(manager) {
            // Create the cube of all variables of this meta variable.
            this->cube = this->getDdManager()->getOne();
            for (auto const& ddVariable : this->ddVariables) {
                this->cube *= ddVariable;
            }
        }
        
        template<DdType Type>
        std::string const& DdMetaVariable<Type>::getName() const {
            return this->name;
        }
        
        template<DdType Type>
        int_fast64_t DdMetaVariable<Type>::getLow() const {
            return this->low;
        }

        template<DdType Type>
        int_fast64_t DdMetaVariable<Type>::getHigh() const {
            return this->high;
        }
        
        template<DdType Type>
        std::shared_ptr<DdManager<Type>> DdMetaVariable<Type>::getDdManager() const {
            return this->manager;
        }

        template<DdType Type>
        std::vector<Dd<Type>> const& DdMetaVariable<Type>::getDdVariables() const {
            return this->ddVariables;
        }
        
        template<DdType Type>
        Dd<Type> const& DdMetaVariable<Type>::getCube() const {
            return this->cube;
        }
        
        // Explicitly instantiate DdMetaVariable.
        template<> class DdMetaVariable<CUDD>;
    }
}