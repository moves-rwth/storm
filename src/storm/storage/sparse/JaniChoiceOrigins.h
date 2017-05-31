#pragma once

#include <memory>
#include <string>

#include "storm/storage/sparse/ChoiceOrigins.h"


namespace storm {
    namespace storage {
        namespace sparse {
            
            
            /*!
             * This class represents for each choice the origin in the jani specification
             * // TODO complete this
             */
            class JaniChoiceOrigins : public ChoiceOrigins {
            public:
                
                /*!
                 * Creates a new representation of the choice indices to their origin in the Jani specification
                 */
                JaniChoiceOrigins(std::vector<uint_fast64_t> const& indexToIdentifierMapping);
                
                virtual ~JaniChoiceOrigins() = default;
                
                virtual bool isJaniChoiceOrigins() const override ;
                
                /*
                 * Returns the number of identifiers that are used by this object.
                 * This can be used to, e.g., loop over all identifiers.
                 */
                virtual uint_fast64_t getNumberOfIdentifiers() const override;
                
                /*
                 * Returns a copy of this object where the mapping of choice indices to origin identifiers is replaced by the given one.
                 */
                virtual std::shared_ptr<ChoiceOrigins> cloneWithNewIndexToIdentifierMapping(std::vector<uint_fast64_t>&& indexToIdentifierMapping) const override;
                
                /*
                 * Computes the identifier infos (i.e., human readable strings representing the choice origins).
                 */
                virtual void computeIdentifierInfos() const override;
                
            private:
                
            };
        }
    }
}