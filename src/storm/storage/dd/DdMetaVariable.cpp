#include "storm/storage/dd/DdMetaVariable.h"

#include "storm/utility/macros.h"

namespace storm {
    namespace dd {
        template<DdType LibraryType>
        DdMetaVariable<LibraryType>::DdMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high, std::vector<Bdd<LibraryType>> const& ddVariables) : name(name), type(MetaVariableType::Int), low(low), high(high), ddVariables(ddVariables) {
            this->createCube();
        }
        
        template<DdType LibraryType>
        DdMetaVariable<LibraryType>::DdMetaVariable(std::string const& name, std::vector<Bdd<LibraryType>> const& ddVariables) : name(name), type(MetaVariableType::Bool), low(0), high(1), ddVariables(ddVariables) {
            this->createCube();
        }
        
        template<DdType LibraryType>
        std::string const& DdMetaVariable<LibraryType>::getName() const {
            return this->name;
        }
        
        template<DdType LibraryType>
        MetaVariableType DdMetaVariable<LibraryType>::getType() const {
            return this->type;
        }
        
        template<DdType LibraryType>
        int_fast64_t DdMetaVariable<LibraryType>::getLow() const {
            return this->low;
        }
        
        template<DdType LibraryType>
        int_fast64_t DdMetaVariable<LibraryType>::getHigh() const {
            return this->high;
        }
        
        template<DdType LibraryType>
        std::size_t DdMetaVariable<LibraryType>::getNumberOfDdVariables() const {
            return this->ddVariables.size();
        }
        
        template<DdType LibraryType>
        std::vector<Bdd<LibraryType>> const& DdMetaVariable<LibraryType>::getDdVariables() const {
            return this->ddVariables;
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> const& DdMetaVariable<LibraryType>::getCube() const {
            return this->cube;
        }
        
        template<DdType LibraryType>
        std::vector<uint64_t> DdMetaVariable<LibraryType>::getIndices() const {
            std::vector<std::pair<uint64_t, uint64_t>> indicesAndLevels = this->getIndicesAndLevels();
            std::sort(indicesAndLevels.begin(), indicesAndLevels.end(), [] (std::pair<uint64_t, uint64_t> const& a, std::pair<uint64_t, uint64_t> const& b) { return a.second < b.second; });
            
            std::vector<uint64_t> indices;
            for (auto const& e : indicesAndLevels) {
                indices.emplace_back(e.first);
            }
            
            return indices;
        }
        
        template<DdType LibraryType>
        std::vector<std::pair<uint64_t, uint64_t>> DdMetaVariable<LibraryType>::getIndicesAndLevels() const {
            std::vector<std::pair<uint64_t, uint64_t>> indicesAndLevels;
            for (auto const& v : ddVariables) {
                indicesAndLevels.emplace_back(v.getIndex(), v.getLevel());
            }
            return indicesAndLevels;
        }
        
        template<DdType LibraryType>
        uint64_t DdMetaVariable<LibraryType>::getHighestLevel() const {
            uint64_t result = 0;
            bool first = true;
            for (auto const& v : ddVariables) {
                if (first) {
                    result = v.getLevel();
                } else {
                    result = std::max(result, v.getLevel());
                }
            }
            
            return result;
        }
        
        template<DdType LibraryType>
        void DdMetaVariable<LibraryType>::createCube() {
            STORM_LOG_ASSERT(!this->ddVariables.empty(), "The DD variables must not be empty.");
            auto it = this->ddVariables.begin();
            this->cube = *it;
            ++it;
            for (auto ite = this->ddVariables.end(); it != ite; ++it) {
                this->cube &= *it;
            }
        }
        
        template class DdMetaVariable<DdType::CUDD>;
        template class DdMetaVariable<DdType::Sylvan>;
    }
}
