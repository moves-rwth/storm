#include "storm/modelchecker/abstraction/BisimulationAbstractionRefinementModelChecker.h"

#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Mdp.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ModelType>
        const std::string BisimulationAbstractionRefinementModelChecker<ModelType>::name = "bisimulation-based astraction refinement";
        
        template<typename ModelType>
        BisimulationAbstractionRefinementModelChecker<ModelType>::BisimulationAbstractionRefinementModelChecker(ModelType const& model) : model(model) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        BisimulationAbstractionRefinementModelChecker<ModelType>::~BisimulationAbstractionRefinementModelChecker() {
            // Intentionally left empty.
        }
        
//        template<typename ModelType>
//        bool BisimulationAbstractionRefinementModelChecker<ModelType>::supportsReachabilityRewards() const {
//            return true;
//        }
//        
//        template<typename ModelType>
//        std::string const& BisimulationAbstractionRefinementModelChecker<ModelType>::getName() const {
//            return name;
//        }
//        
//        template<typename ModelType>
//        void BisimulationAbstractionRefinementModelChecker<ModelType>::initializeAbstractionRefinement() {
//            
//        }
//        
//        template<typename ModelType>
//        std::shared_ptr<storm::models::Model<typename BisimulationAbstractionRefinementModelChecker<ModelType>::ValueType>> BisimulationAbstractionRefinementModelChecker<ModelType>::getAbstractModel() {
//            
//        }
//        
//        template<typename ModelType>
//        std::pair<std::unique_ptr<storm::abstraction::StateSet>, std::unique_ptr<storm::abstraction::StateSet>> BisimulationAbstractionRefinementModelChecker<ModelType>::getConstraintAndTargetStates(storm::models::Model<ValueType> const& abstractModel) {
//            
//        }
//        
//        template<typename ModelType>
//        uint64_t BisimulationAbstractionRefinementModelChecker<ModelType>::getAbstractionPlayer() const {
//            return 1;
//        }
//        
//        template<typename ModelType>
//        bool BisimulationAbstractionRefinementModelChecker<ModelType>::requiresSchedulerSynthesis() const {
//            return false;
//        }
//        
//        template<typename ModelType>
//        void BisimulationAbstractionRefinementModelChecker<ModelType>::refineAbstractModel() {
//            
//        }
        
        template class BisimulationAbstractionRefinementModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>>;
        template class BisimulationAbstractionRefinementModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>;
        template class BisimulationAbstractionRefinementModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>>;
        template class BisimulationAbstractionRefinementModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>;

    }
}
