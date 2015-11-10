#include "src/storage/dd/cudd/CuddDdMetaVariable.h"
#include "src/storage/dd/cudd/CuddDdManager.h"

namespace storm {
    namespace dd {
        DdMetaVariable<DdType::CUDD>::DdMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high, std::vector<Bdd<DdType::CUDD>> const& ddVariables, std::shared_ptr<DdManager<DdType::CUDD>> manager) : name(name), type(MetaVariableType::Int), low(low), high(high), ddVariables(ddVariables), cube(manager->getBddOne()), manager(manager) {
            // Create the cube of all variables of this meta variable.
            for (auto const& ddVariable : this->ddVariables) {
                this->cube &= ddVariable;
            }
        }
        
        DdMetaVariable<DdType::CUDD>::DdMetaVariable(std::string const& name, std::vector<Bdd<DdType::CUDD>> const& ddVariables, std::shared_ptr<DdManager<DdType::CUDD>> manager) : name(name), type(MetaVariableType::Bool), low(0), high(1), ddVariables(ddVariables), cube(manager->getBddOne()), manager(manager) {
            // Create the cube of all variables of this meta variable.
            for (auto const& ddVariable : this->ddVariables) {
                this->cube &= ddVariable;
            }
        }
        
        std::string const& DdMetaVariable<DdType::CUDD>::getName() const {
            return this->name;
        }
        
        DdMetaVariable<DdType::CUDD>::MetaVariableType DdMetaVariable<DdType::CUDD>::getType() const {
            return this->type;
        }
        
        int_fast64_t DdMetaVariable<DdType::CUDD>::getLow() const {
            return this->low;
        }

        int_fast64_t DdMetaVariable<DdType::CUDD>::getHigh() const {
            return this->high;
        }
        
        std::size_t DdMetaVariable<DdType::CUDD>::getNumberOfDdVariables() const {
            return this->ddVariables.size();
        }
        
        std::shared_ptr<DdManager<DdType::CUDD>> DdMetaVariable<DdType::CUDD>::getDdManager() const {
            return this->manager;
        }

        std::vector<Bdd<DdType::CUDD>> const& DdMetaVariable<DdType::CUDD>::getDdVariables() const {
            return this->ddVariables;
        }
        
        Bdd<DdType::CUDD> const& DdMetaVariable<DdType::CUDD>::getCube() const {
            return this->cube;
        }
    }
}