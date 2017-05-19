#include "storm/storage/sparse/JaniChoiceOrigins.h"


namespace storm {
    namespace storage {
        namespace sparse {
            
            JaniChoiceOrigins::JaniChoiceOrigins(std::vector<uint_fast64_t> const& indexToIdentifierMapping, std::vector<std::string> const& identifierToInfoMapping) : ChoiceOrigins(indexToIdentifierMapping, identifierToInfoMapping) {
				// Intentionally left empty
			}

            bool JaniChoiceOrigins::isJaniChoiceOrigins() const  {
            	return true;
            }
             
            std::shared_ptr<ChoiceOrigins> JaniChoiceOrigins::cloneWithNewIndexToIdentifierMapping(std::vector<uint_fast64_t>&& indexToIdentifierMapping) const {
                std::vector<std::string> identifierToInfoMapping = this->identifierToInfo;
                return std::make_shared<JaniChoiceOrigins>(std::move(indexToIdentifierMapping), std::move(identifierToInfoMapping));
            }
            
        }
    }
}