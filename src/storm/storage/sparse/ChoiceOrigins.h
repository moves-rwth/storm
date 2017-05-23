#pragma once

#include <vector>
#include <string>
#include "storm/storage/BitVector.h"
#include "storm/models/sparse/ChoiceLabeling.h"


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
                 * Returns the largest identifier that is used by this object.
                 * This can be used to, e.g., loop over all identifiers.
                 */
                virtual uint_fast64_t getLargestIdentifier() const = 0;
                /*
                 * Returns the identifier that is used for choices without an origin in the input specification
                 * E.g., Selfloops introduced on deadlock states
                 */
                static uint_fast64_t getIdentifierForChoicesWithNoOrigin();
                
                /*
                 * Returns the information for the given choice origin identifier as a (human readable) string
                 */
                std::string const& getIdentifierInfo(uint_fast64_t identifier) const;
                
                /*
                 * Returns the choice origin information as a (human readable) string.
                 */
                std::string const& getChoiceInfo(uint_fast64_t choiceIndex) const;
                
                /*
                 * Derive new choice origins from this by selecting the given choices.
                 */
                std::shared_ptr<ChoiceOrigins> selectChoices(storm::storage::BitVector const& selectedChoices) const;
                
                /*
                 * Derive new choice origins from this by selecting the given choices.
                 * If an invalid choice index is selected, the corresponding choice will get the identifier for choices with no origin.
                 */
                std::shared_ptr<ChoiceOrigins> selectChoices(std::vector<uint_fast64_t> const& selectedChoiceIndices) const;
                
                storm::models::sparse::ChoiceLabeling toChoiceLabeling() const;

            protected:
                ChoiceOrigins(std::vector<uint_fast64_t> const& indexToIdentifierMapping);
                ChoiceOrigins(std::vector<uint_fast64_t>&& indexToIdentifierMapping);
                
                /*
                 * Returns a copy of this object where the mapping of choice indices to origin identifiers is replaced by the given one.
                 */
                virtual std::shared_ptr<ChoiceOrigins> cloneWithNewIndexToIdentifierMapping(std::vector<uint_fast64_t>&& indexToIdentifierMapping) const = 0;
                
                /*
                 * Computes the identifier infos (i.e., human readable strings representing the choice origins).
                 */
                virtual void computeIdentifierInfos() const = 0;
                
                std::vector<uint_fast64_t> const indexToIdentifier;
                mutable std::vector<std::string> identifierToInfo;

            };
        }
    }
}