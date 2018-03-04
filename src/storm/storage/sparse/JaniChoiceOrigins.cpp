#include "storm/storage/sparse/JaniChoiceOrigins.h"

#include "storm/storage/jani/Model.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace storage {
        namespace sparse {
            
            JaniChoiceOrigins::JaniChoiceOrigins(std::shared_ptr<storm::jani::Model const> const& janiModel, std::vector<uint_fast64_t> const& indexToIdentifierMapping, std::vector<EdgeIndexSet> const& identifierToEdgeIndexSetMapping) : ChoiceOrigins(indexToIdentifierMapping), model(janiModel), identifierToEdgeIndexSet(identifierToEdgeIndexSetMapping) {
                STORM_LOG_THROW(identifierToEdgeIndexSet[this->getIdentifierForChoicesWithNoOrigin()].empty(), storm::exceptions::InvalidArgumentException, "The given edge set for the choices without origin is non-empty");
			}

            bool JaniChoiceOrigins::isJaniChoiceOrigins() const  {
            	return true;
            }
            
            uint_fast64_t JaniChoiceOrigins::getNumberOfIdentifiers() const {
                return identifierToEdgeIndexSet.size();
            }
            
            storm::jani::Model const& JaniChoiceOrigins::getModel() const {
                return *model;
            }
            
            JaniChoiceOrigins::EdgeIndexSet const& JaniChoiceOrigins::getEdgeIndexSet(uint_fast64_t choiceIndex) const {
                return identifierToEdgeIndexSet[this->getIdentifier(choiceIndex)];
            }
            
            std::shared_ptr<ChoiceOrigins> JaniChoiceOrigins::cloneWithNewIndexToIdentifierMapping(std::vector<uint_fast64_t>&& indexToIdentifierMapping) const {
                auto result = std::make_shared<JaniChoiceOrigins>(this->model, std::move(indexToIdentifierMapping), std::move(identifierToEdgeIndexSet));
                result->identifierToInfo = this->identifierToInfo;
                return result;
            }
            
            void JaniChoiceOrigins::computeIdentifierInfos() const {
                STORM_LOG_ASSERT(false, "Jani choice origins not properly implemented");
            }
            
        }
    }
}
