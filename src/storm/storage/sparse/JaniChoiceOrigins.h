#pragma once

#include <memory>
#include <string>

#include "storm/storage/sparse/ChoiceOrigins.h"


namespace storm {
    namespace storage {
        namespace sparse {
            
            
            /*!
             * This class represents for each choice the origin in the jani specification
             */
            class JaniChoiceOrigins : public ChoiceOrigins {
            public:
                
                /*!
                 * Creates a new representation of the choice indices to their origin in the Jani specification
                 * // TODO complete this
                 */
                JaniChoiceOrigins(std::vector<uint_fast64_t> const& indexToIdentifierMapping, std::vector<std::string> const& identifierToInfoMapping);
                
                virtual ~JaniChoiceOrigins() = default;
                
                virtual bool isJaniChoiceOrigins() const override ;
                
                /*
                 * Returns a copy of this object where the mapping of choice indices to origin identifiers is replaced by the given one.
                 */
                virtual std::shared_ptr<ChoiceOrigins> cloneWithNewIndexToIdentifierMapping(std::vector<uint_fast64_t>&& indexToIdentifierMapping) const override;
                
            private:
                
            };
        }
    }
}