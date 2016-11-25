#include "storm/abstraction/prism/PrismMenuGameAbstractor.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/AbstractionSettings.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType, typename ValueType>
            PrismMenuGameAbstractor<DdType, ValueType>::PrismMenuGameAbstractor(storm::prism::Program const& program, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory) : abstractProgram(program, smtSolverFactory, storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isAddAllGuardsSet(), storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isSplitPredicatesSet()) {
                // Intentionally left empty.
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::abstraction::MenuGame<DdType, ValueType> PrismMenuGameAbstractor<DdType, ValueType>::abstract() {
                return abstractProgram.getAbstractGame();
            }
            
            AbstractionInformation<DdType> const& PrismMenuGameAbstractor<DdType, ValueType>::getAbstractionInformation() const {
                return abstractProgram.getAbstractionInformation();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void PrismMenuGameAbstractor<DdType, ValueType>::refine(std::vector<storm::expressions::Expression> const& predicates) {
                std::vector<std::pair<storm::expressions::Expression, bool>> predicatesWithFlags;
                for (auto const& predicate : predicates) {
                    predicatesWithFlags.emplace_back(predicate, true);
                }
                abstractProgram.refine(predicatesWithFlags);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void PrismMenuGameAbstractor<DdType, ValueType>::refine(storm::dd::Bdd<DdType> const& pivotState, storm::dd::Bdd<DdType> const& player1Choice, storm::dd::Bdd<DdType> const& lowerChoice, storm::dd::Bdd<DdType> const& upperChoice) {
                abstractProgram.refine(pivotState, player1Choice, lowerChoice, upperChoice);
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
