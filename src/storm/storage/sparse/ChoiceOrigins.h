#pragma once

#include <vector>
#include <string>
#include "storm/storage/BitVector.h"

namespace storm {
    namespace storage {
        namespace sparse {
            
            class PrismChoiceOrigins;
            class JaniChoiceOrigins;
            
            /*!
             * This class represents the origin of the choices of a model in terms of the input model specification
             * (e.g., the Prism commands that induced the choice).
             */
            class ChoiceOrigins {
            public:
                
                virtual ~ChoiceOrigins() = default;
                
                virtual bool isPrismChoiceOrigins() const;
                virtual bool isJaniChoiceOrigins() const;
                
                PrismChoiceOrigins& asPrismChoiceOrigins();
                PrismChoiceOrigins const& asPrismChoiceOrigins() const;
                
                JaniChoiceOrigins& asJaniChoiceOrigins();
                JaniChoiceOrigins const& asJaniChoiceOrigins() const;
                
                /*
                 * Returns a unique identifier of the origin of the given choice which can be used to e.g. check whether two choices have the same origin
                 */
                uint_fast64_t getIdentifier(uint_fast64_t choiceIndex) const;
                
                /*
                 * Returns the information for the given choice origin identifier as a (human readable) string
                 */
                std::string const& getIdentifierInfo(uint_fast64_t identifier) const;
                
                /*
                 * Returns the choice origin information as a (human readable) string.
                 */
                std::string const& getChoiceInfo(uint_fast64_t choiceIndex) const;
                
                
                std::shared_ptr<ChoiceOrigins> selectChoices(storm::storage::BitVector const& selectedChoices) const;
                std::shared_ptr<ChoiceOrigins> selectChoices(std::vector<uint_fast64_t> const& selectedChoiceIndices) const;

            protected:
                ChoiceOrigins(std::vector<uint_fast64_t> const& indexToIdentifierMapping, std::vector<std::string> const& identifierToInfoMapping);
                ChoiceOrigins(std::vector<uint_fast64_t>&& indexToIdentifierMapping, std::vector<std::string>&& identifierToInfoMapping);
                
                /*
                 * Returns a copy of this object where the mapping of choice indices to origin identifiers is replaced by the given one.
                 */
                virtual std::shared_ptr<ChoiceOrigins> cloneWithNewIndexToIdentifierMapping(std::vector<uint_fast64_t>&& indexToIdentifierMapping) const = 0;
                
                std::vector<uint_fast64_t> indexToIdentifier;
                std::vector<std::string> identifierToInfo;
            };
        }
    }
}