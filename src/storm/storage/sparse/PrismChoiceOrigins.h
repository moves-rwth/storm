#pragma once

#include <memory>
#include <string>
#include <boost/container/flat_set.hpp>

#include "storm/storage/sparse/ChoiceOrigins.h"
#include "storm/storage/prism/Program.h"


namespace storm {
    namespace storage {
        namespace sparse {
            
            
            /*!
             * This class represents for each choice the set of prism commands that induced the choice
             */
            class PrismChoiceOrigins : public ChoiceOrigins {
            public:
                
                typedef boost::container::flat_set<uint_fast64_t> CommandSet;
                
                /*!
                 * Creates a new representation of the choice indices to their origin in the prism program
                 * @param prismProgram The associated prism program
                 * @param indexToIdentifierMapping maps a choice index to the internally used identifier of the choice origin
                 * @param identifierToInfoMapping maps an origin identifier to a string representation of the origin
                 * @param identifierToCommandSetMapping maps an origin identifier to the set of global indices of the corresponding prism commands
                 */
                PrismChoiceOrigins(std::shared_ptr<storm::prism::Program const> const& prismProgram, std::vector<uint_fast64_t> const& indexToIdentifierMapping, std::vector<std::string> const& identifierToInfoMapping, std::vector<CommandSet> const& identifierToCommandSetMapping);
                PrismChoiceOrigins(std::shared_ptr<storm::prism::Program const> const& prismProgram, std::vector<uint_fast64_t>&& indexToIdentifierMapping, std::vector<std::string>&& identifierToInfoMapping, std::vector<CommandSet>&& identifierToCommandSetMapping);
                
                virtual ~PrismChoiceOrigins() = default;
                
                virtual bool isPrismChoiceOrigins() const override ;
                
                /*
                 * Returns the prism program associated with this
                 */
                storm::prism::Program const& getProgram() const;
                
                /*
                 * Returns the set of prism commands that induced the choice with the given index.
                 * The command set is represented by a set of global command indices
                 */
                CommandSet const& getCommandSet(uint_fast64_t choiceIndex) const;
                
                /*
                 * Returns a copy of this object where the mapping of choice indices to origin identifiers is replaced by the given one.
                 */
                virtual std::shared_ptr<ChoiceOrigins> cloneWithNewIndexToIdentifierMapping(std::vector<uint_fast64_t>&& indexToIdentifierMapping) const override;
                
            private:
                
                std::shared_ptr<storm::prism::Program const> program;
                std::vector<CommandSet> identifierToCommandSet;
                
            };
        }
    }
}