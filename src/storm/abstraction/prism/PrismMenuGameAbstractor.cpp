#include "storm/abstraction/prism/PrismMenuGameAbstractor.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/AbstractionSettings.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType, typename ValueType>
            PrismMenuGameAbstractor<DdType, ValueType>::PrismMenuGameAbstractor(storm::prism::Program const& program, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory) : abstractProgram(program, smtSolverFactory, storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isAddAllGuardsSet()) {
                // Intentionally left empty.
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::abstraction::MenuGame<DdType, ValueType> PrismMenuGameAbstractor<DdType, ValueType>::abstract() {
                return abstractProgram.getAbstractGame();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractionInformation<DdType> const& PrismMenuGameAbstractor<DdType, ValueType>::getAbstractionInformation() const {
                return abstractProgram.getAbstractionInformation();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::expressions::Expression const& PrismMenuGameAbstractor<DdType, ValueType>::getGuard(uint64_t player1Choice) const {
                return abstractProgram.getGuard(player1Choice);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::map<storm::expressions::Variable, storm::expressions::Expression> PrismMenuGameAbstractor<DdType, ValueType>::getVariableUpdates(uint64_t player1Choice, uint64_t auxiliaryChoice) const {
                return abstractProgram.getVariableUpdates(player1Choice, auxiliaryChoice);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void PrismMenuGameAbstractor<DdType, ValueType>::refine(std::vector<storm::expressions::Expression> const& predicates) {
                abstractProgram.refine(predicates);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void PrismMenuGameAbstractor<DdType, ValueType>::exportToDot(std::string const& filename, storm::dd::Bdd<DdType> const& highlightStates, storm::dd::Bdd<DdType> const& filter) const {
                abstractProgram.exportToDot(filename, highlightStates, filter);
            }
         
            template class PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double>;
            template class PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double>;
        }
    }
}
