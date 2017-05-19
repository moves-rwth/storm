#include "storm/storage/sparse/PrismChoiceOrigins.h"


namespace storm {
    namespace storage {
        namespace sparse {
            
            PrismChoiceOrigins::PrismChoiceOrigins(std::shared_ptr<storm::prism::Program const> const& prismProgram, std::vector<uint_fast64_t> const& indexToIdentifierMapping, std::vector<std::string> const& identifierToInfoMapping, std::vector<CommandSet> const& identifierToCommandSetMapping) : ChoiceOrigins(indexToIdentifierMapping, identifierToInfoMapping), program(prismProgram), identifierToCommandSet(identifierToCommandSetMapping) {
				// Intentionally left empty
			}

            PrismChoiceOrigins::PrismChoiceOrigins(std::shared_ptr<storm::prism::Program const> const& prismProgram, std::vector<uint_fast64_t>&& indexToIdentifierMapping, std::vector<std::string>&& identifierToInfoMapping, std::vector<CommandSet>&& identifierToCommandSetMapping) : ChoiceOrigins(std::move(indexToIdentifierMapping), std::move(identifierToInfoMapping)), program(prismProgram), identifierToCommandSet(std::move(identifierToCommandSetMapping)) {
				// Intentionally left empty
            }
                
            bool PrismChoiceOrigins::isPrismChoiceOrigins() const  {
            	return true;
            }
             
            storm::prism::Program const& PrismChoiceOrigins::getProgram() const {
            	return *program;
            }
                
            PrismChoiceOrigins::CommandSet const& PrismChoiceOrigins::getCommandSet(uint_fast64_t choiceIndex) const {
                return identifierToCommandSet[this->getIdentifier(choiceIndex)];
			}
            
            std::shared_ptr<ChoiceOrigins> PrismChoiceOrigins::cloneWithNewIndexToIdentifierMapping(std::vector<uint_fast64_t>&& indexToIdentifierMapping) const {
                std::vector<CommandSet> identifierToCommandSetMapping = this->identifierToCommandSet;
                std::vector<std::string> identifierToInfoMapping = this->identifierToInfo;
                return std::make_shared<PrismChoiceOrigins>(this->program, std::move(indexToIdentifierMapping), std::move(identifierToInfoMapping), std::move(identifierToCommandSetMapping));
            }

            
        }
    }
}