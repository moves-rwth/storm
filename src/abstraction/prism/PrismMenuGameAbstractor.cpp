#include "src/abstraction/prism/PrismMenuGameAbstractor.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/AbstractionSettings.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType, typename ValueType>
            PrismMenuGameAbstractor<DdType, ValueType>::PrismMenuGameAbstractor(storm::prism::Program const& program, std::vector<storm::expressions::Expression> const& initialPredicates, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory) : abstractProgram(program, initialPredicates, smtSolverFactory, storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isAddAllGuardsSet()) {
                // Intentionally left empty.
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::abstraction::MenuGame<DdType, ValueType> PrismMenuGameAbstractor<DdType, ValueType>::abstract() {
                return abstractProgram.getAbstractGame();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void PrismMenuGameAbstractor<DdType, ValueType>::refine(std::vector<storm::expressions::Expression> const& predicates) {
                abstractProgram.refine(predicates);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void PrismMenuGameAbstractor<DdType, ValueType>::refine(storm::dd::Bdd<DdType> const& pivotState, storm::dd::Bdd<DdType> const& player1Choice, storm::dd::Bdd<DdType> const& lowerChoice, storm::dd::Bdd<DdType> const& upperChoice) {
                abstractProgram.refine(pivotState, player1Choice, lowerChoice, upperChoice);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void PrismMenuGameAbstractor<DdType, ValueType>::exportToDot(std::string const& filename) const {
                abstractProgram.exportToDot(filename);
            }
         
            template class PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double>;
            template class PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double>;
        }
    }
}