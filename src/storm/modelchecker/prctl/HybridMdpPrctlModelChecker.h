    #ifndef STORM_MODELCHECKER_HYBRIDMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_HYBRIDMDPPRCTLMODELCHECKER_H_

#include "storm/modelchecker/propositional/SymbolicPropositionalModelChecker.h"

#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/storage/dd/DdType.h"
#include "storm/solver/OptimizationDirection.h"

namespace storm {
    
    namespace models {
        namespace symbolic {
            template<storm::dd::DdType Type, typename ValueType>
            class Mdp;
        }
    }
    
    
    namespace modelchecker {
        template<typename ModelType>
        class HybridMdpPrctlModelChecker : public SymbolicPropositionalModelChecker<ModelType> {
        public:
            typedef typename ModelType::ValueType ValueType;
            static const storm::dd::DdType DdType = ModelType::DdType;

            explicit HybridMdpPrctlModelChecker(ModelType const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(Environment const& env, CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(Environment const& env, CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> checkMultiObjectiveFormula(Environment const& env, CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask) override;

        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_HYBRIDMDPPRCTLMODELCHECKER_H_ */
