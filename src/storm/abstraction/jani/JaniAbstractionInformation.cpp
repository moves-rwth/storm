#include "storm/abstraction/jani/JaniAbstractionInformation.h"

#include "storm/storage/dd/DdManager.h"

namespace storm {
    namespace abstraction {
        namespace jani {
            
            template<storm::dd::DdType DdType>
            JaniAbstractionInformation<DdType>::JaniAbstractionInformation(storm::expressions::ExpressionManager& expressionManager, std::set<storm::expressions::Variable> const& allVariables, uint64_t numberOfLocations, std::unique_ptr<storm::solver::SmtSolver>&& smtSolver, std::shared_ptr<storm::dd::DdManager<DdType>> ddManager) : AbstractionInformation<DdType>(expressionManager, allVariables, std::move(smtSolver), ddManager) {
                
                // Create the location variable to have the appropriate dimension.
                if (numberOfLocations > 1) {
                    locationVariables = ddManager->addMetaVariable("loc", 0, numberOfLocations - 1);
                    sourceLocationVariables.insert(locationVariables.get().first);
                    successorLocationVariables.insert(locationVariables.get().second);
                }
            }
            
            template<storm::dd::DdType DdType>
            storm::dd::Bdd<DdType> JaniAbstractionInformation<DdType>::encodeLocation(uint64_t locationIndex, bool source) const {
                if (locationVariables) {
                    if (source) {
                        return this->getDdManager().getEncoding(locationVariables.get().first, locationIndex);
                    } else {
                        return this->getDdManager().getEncoding(locationVariables.get().second, locationIndex);
                    }
                } else {
                    return this->getDdManager().getBddOne();
                }
            }
            
            template<storm::dd::DdType DdType>
            std::set<storm::expressions::Variable> const& JaniAbstractionInformation<DdType>::getSourceLocationVariables() const {
                return sourceLocationVariables;
            }
            
            template<storm::dd::DdType DdType>
            std::set<storm::expressions::Variable> const& JaniAbstractionInformation<DdType>::getSuccessorLocationVariables() const {
                return successorLocationVariables;
            }

            template class JaniAbstractionInformation<storm::dd::DdType::CUDD>;
            template class JaniAbstractionInformation<storm::dd::DdType::Sylvan>;
            
        }
    }
}
