#include <algorithm>

#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddDdManager.h"

namespace storm {
    namespace dd {
        Dd<DdType::CUDD>::Dd(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, std::set<storm::expressions::Variable> const& containedMetaVariables) : ddManager(ddManager), containedMetaVariables(containedMetaVariables) {
            // Intentionally left empty.
        }
                
        bool Dd<DdType::CUDD>::containsMetaVariable(storm::expressions::Variable const& metaVariable) const {
            return containedMetaVariables.find(metaVariable) != containedMetaVariables.end();
        }
        
        bool Dd<DdType::CUDD>::containsMetaVariables(std::set<storm::expressions::Variable> const& metaVariables) const {
            return std::includes(containedMetaVariables.begin(), containedMetaVariables.end(), metaVariables.begin(), metaVariables.end());
        }
        
        std::set<storm::expressions::Variable> const& Dd<DdType::CUDD>::getContainedMetaVariables() const {
            return this->containedMetaVariables;
        }
        
        std::set<storm::expressions::Variable>& Dd<DdType::CUDD>::getContainedMetaVariables() {
            return this->containedMetaVariables;
        }
        
        std::shared_ptr<DdManager<DdType::CUDD> const> Dd<DdType::CUDD>::getDdManager() const {
            return this->ddManager;
        }

        void Dd<DdType::CUDD>::addMetaVariables(std::set<storm::expressions::Variable> const& metaVariables) {
            std::set<storm::expressions::Variable> result;
            std::set_union(containedMetaVariables.begin(), containedMetaVariables.end(), metaVariables.begin(), metaVariables.end(), std::inserter(result, result.begin()));
            containedMetaVariables = std::move(result);
        }
    
        void Dd<DdType::CUDD>::addMetaVariable(storm::expressions::Variable const& metaVariable) {
            this->getContainedMetaVariables().insert(metaVariable);
        }

        void Dd<DdType::CUDD>::removeMetaVariable(storm::expressions::Variable const& metaVariable) {
            this->getContainedMetaVariables().erase(metaVariable);
        }

        void Dd<DdType::CUDD>::removeMetaVariables(std::set<storm::expressions::Variable> const& metaVariables) {
            std::set<storm::expressions::Variable> result;
            std::set_difference(containedMetaVariables.begin(), containedMetaVariables.end(), metaVariables.begin(), metaVariables.end(), std::inserter(result, result.begin()));
            containedMetaVariables = std::move(result);
        }
    }
}