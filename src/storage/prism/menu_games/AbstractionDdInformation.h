#ifndef STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTIONDDINFORMATION_H_
#define STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTIONDDINFORMATION_H_

#include <memory>
#include <vector>

#include "src/storage/dd/DdType.h"
#include "src/storage/expressions/Variable.h"

namespace storm {
    namespace dd {
        template <storm::dd::DdType DdType>
        class DdManager;
    }

    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            struct AbstractionDdInformation {
            public:
                /*!
                 * Creates a new DdInformation that uses the given manager.
                 *
                 * @param manager The manager to use.
                 */
                AbstractionDdInformation(std::shared_ptr<storm::dd::DdManager<DdType>> const& manager);
                
                // The manager responsible for the DDs.
                std::shared_ptr<storm::dd::DdManager<DdType>> ddManager;
                
                // The DD variables corresponding to the predicates.
                std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> predicateDdVariables;
                
                // The DD variable encoding the command (i.e., the nondeterministic choices of player 1).
                storm::expressions::Variable commandDdVariable;
                
                // The DD variable encoding the update IDs for all actions.
                storm::expressions::Variable updateDdVariable;
                
                // The DD variables encoding the nondeterministic choices of player 2.
                std::vector<storm::expressions::Variable> optionDdVariables;
            };
            
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTIONDDINFORMATION_H_ */
