#include "storm/storage/sparse/ChoiceOrigins.h"

#include "storm/storage/sparse/PrismChoiceOrigins.h"
#include "storm/storage/sparse/JaniChoiceOrigins.h"
#include "storm/utility/vector.h"


namespace storm {
    namespace storage {
        namespace sparse {
            
            ChoiceOrigins::ChoiceOrigins(std::vector<uint_fast64_t> const& indexToIdentifierMapping, std::vector<std::string> const& identifierToInfoMapping) : indexToIdentifier(indexToIdentifierMapping), identifierToInfo(identifierToInfoMapping) {
                // Intentionally left empty
            }
            
            ChoiceOrigins::ChoiceOrigins(std::vector<uint_fast64_t>&& indexToIdentifierMapping, std::vector<std::string>&& identifierToInfoMapping) : indexToIdentifier(std::move(indexToIdentifierMapping)), identifierToInfo(std::move(identifierToInfoMapping)) {
                // Intentionally left empty
            }
            
            bool ChoiceOrigins::isPrismChoiceOrigins() const {
                return false;
            }
            
            bool ChoiceOrigins::isJaniChoiceOrigins() const {
                return false;
            }
                
            PrismChoiceOrigins& ChoiceOrigins::asPrismChoiceOrigins() {
                return dynamic_cast<PrismChoiceOrigins&>(*this);
            }
            
            PrismChoiceOrigins const& ChoiceOrigins::asPrismChoiceOrigins() const {
                return dynamic_cast<PrismChoiceOrigins const&>(*this);
            }
                
            JaniChoiceOrigins& ChoiceOrigins::asJaniChoiceOrigins() {
                return dynamic_cast<JaniChoiceOrigins&>(*this);
            }
            
            JaniChoiceOrigins const& ChoiceOrigins::asJaniChoiceOrigins() const {
                return dynamic_cast<JaniChoiceOrigins const&>(*this);
            }
            			    
            uint_fast64_t ChoiceOrigins::getIdentifier(uint_fast64_t choiceIndex) const {
            	return  indexToIdentifier[choiceIndex];
            }
            
			uint_fast64_t ChoiceOrigins::getIdentifierForChoicesWithNoOrigin() {
				return 0;
			}
                
			std::string const& ChoiceOrigins::getIdentifierInfo(uint_fast64_t identifier) const {
				return identifierToInfo[identifier];
			}
                
			std::string const& ChoiceOrigins::getChoiceInfo(uint_fast64_t choiceIndex) const {
				return getIdentifierInfo(getIdentifier(choiceIndex));
			}
            
            std::shared_ptr<ChoiceOrigins> ChoiceOrigins::selectChoices(storm::storage::BitVector const& selectedChoices) const {
                std::vector<uint_fast64_t> indexToIdentifierMapping(selectedChoices.getNumberOfSetBits());
                storm::utility::vector::selectVectorValues(indexToIdentifierMapping, selectedChoices, indexToIdentifier);
                return cloneWithNewIndexToIdentifierMapping(std::move(indexToIdentifierMapping));
            }
            
            std::shared_ptr<ChoiceOrigins> ChoiceOrigins::selectChoices(std::vector<uint_fast64_t> const& selectedChoices) const {
                std::vector<uint_fast64_t> indexToIdentifierMapping;
                indexToIdentifierMapping.reserve(selectedChoices.size());
                for (auto const& selectedChoice : selectedChoices){
                    if (selectedChoice < this->indexToIdentifier.size()) {
                        indexToIdentifierMapping.push_back(indexToIdentifier[selectedChoice]);
                    } else {
                        indexToIdentifierMapping.push_back(getIdentifierForChoicesWithNoOrigin());
                    }
                }
                return cloneWithNewIndexToIdentifierMapping(std::move(indexToIdentifierMapping));
            }
        }
    }
}