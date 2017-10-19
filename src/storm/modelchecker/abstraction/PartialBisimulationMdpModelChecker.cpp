#include "storm/modelchecker/abstraction/PartialBisimulationMdpModelChecker.h"

#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Mdp.h"

namespace storm {
    namespace modelchecker {
        
        template<storm::dd::DdType Type, typename ModelType>
        PartialBisimulationMdpModelChecker<Type, ModelType>::PartialBisimulationMdpModelChecker(ModelType const& model) : model(model) {

        }
        
        template class PartialBisimulationMdpModelChecker<storm::dd::DdType::CUDD, storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>>;
        template class PartialBisimulationMdpModelChecker<storm::dd::DdType::CUDD, storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>;
        template class PartialBisimulationMdpModelChecker<storm::dd::DdType::Sylvan, storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>>;
        template class PartialBisimulationMdpModelChecker<storm::dd::DdType::Sylvan, storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>;
    }
}
