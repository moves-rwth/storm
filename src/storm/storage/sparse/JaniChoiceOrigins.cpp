#include "storm/storage/sparse/JaniChoiceOrigins.h"

#include "storm/utility/macros.h"

namespace storm {
    namespace storage {
        namespace sparse {
            
            JaniChoiceOrigins::JaniChoiceOrigins(std::vector<uint_fast64_t> const& indexToIdentifierMapping) : ChoiceOrigins(indexToIdentifierMapping) {
				// Intentionally left empty
                STORM_LOG_ASSERT(false, "Jani choice origins not properly implemented");
			}

            bool JaniChoiceOrigins::isJaniChoiceOrigins() const  {
            	return true;
            }
            
            uint_fast64_t JaniChoiceOrigins::getNumberOfIdentifiers() const {
                return 0;
            }
             
            std::shared_ptr<ChoiceOrigins> JaniChoiceOrigins::cloneWithNewIndexToIdentifierMapping(std::vector<uint_fast64_t>&& indexToIdentifierMapping) const {
                return std::make_shared<JaniChoiceOrigins>(std::move(indexToIdentifierMapping));
            }
            
            void JaniChoiceOrigins::computeIdentifierInfos() const {
                
            }
            
        }
    }
}