#pragma once

#include "storm/modelchecker/AbstractModelChecker.h"

#include "storm/storage/dd/DdType.h"

namespace storm {
    namespace dd {
        template <storm::dd::DdType DdType>
        class Bdd;
    }
    
    namespace models {
        template <typename ValueType>
        class Model;

        namespace symbolic {
            template <storm::dd::DdType DdType, typename ValueType>
            class Dtmc;
            
            template <storm::dd::DdType DdType, typename ValueType>
            class Mdp;

            template <storm::dd::DdType DdType, typename ValueType>
            class StochasticTwoPlayerGame;
        }
    }
    
    namespace modelchecker {
        
        template<typename ModelType>
        class PartialBisimulationMdpModelChecker : public AbstractModelChecker<ModelType> {
        public:
            typedef typename ModelType::ValueType ValueType;
            static const storm::dd::DdType DdType = ModelType::DdType;

            /*!
             * Constructs a model checker for the given model.
             */
            explicit PartialBisimulationMdpModelChecker(ModelType const& model);
            
            /// Overridden methods from super class.
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;

        private:
            std::unique_ptr<CheckResult> computeValuesAbstractionRefinement(bool rewards, CheckTask<storm::logic::Formula> const& checkTask);
            
            // Methods to check for convergence and postprocessing the result.
            bool checkBoundsSufficientlyClose(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds);
            std::unique_ptr<CheckResult> getAverageOfBounds(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds);
            void printBoundsInformation(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds);
            ValueType getLargestDifference(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds);

            // Methods to compute bounds on the partial quotient.
            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeBoundsPartialQuotient(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask);
            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeBoundsPartialQuotient(storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask);
            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeBoundsPartialQuotient(storm::models::Model<ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask);
            
            // Methods to solve the query on the full quotient.
            std::unique_ptr<CheckResult> computeResultFullQuotient(storm::models::symbolic::Dtmc<DdType, ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask);
            std::unique_ptr<CheckResult> computeResultFullQuotient(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask);
            std::unique_ptr<CheckResult> computeResultFullQuotient(storm::models::Model<ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask);
            
            // The non-abstracted model.
            ModelType const& model;
        };
    }
}
