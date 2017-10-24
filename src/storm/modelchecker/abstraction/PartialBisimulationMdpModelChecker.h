#pragma once

#include "storm/modelchecker/AbstractModelChecker.h"

#include "storm/storage/dd/DdType.h"

#include "storm/solver/OptimizationDirection.h"

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
    
    namespace abstraction {
        template <storm::dd::DdType DdType>
        struct QualitativeMdpResultMinMax;
    }
    
    namespace modelchecker {
        template <typename ModelType>
        class SymbolicMdpPrctlModelChecker;
        
        template <typename ValueType>
        class QuantitativeCheckResult;

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
            std::unique_ptr<CheckResult> computeValuesAbstractionRefinement(CheckTask<storm::logic::Formula> const& checkTask);
            
            // Methods to check for convergence and postprocessing the result.
            bool checkBoundsSufficientlyClose(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds);
            std::unique_ptr<CheckResult> getAverageOfBounds(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds);
            void printBoundsInformation(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds);

            // Methods related to the qualitative solution.
            storm::abstraction::QualitativeMdpResultMinMax<DdType> computeQualitativeResult(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask, storm::dd::Bdd<DdType> const& constraintStates, storm::dd::Bdd<DdType> const& targetStates);
            std::unique_ptr<CheckResult> checkForResult(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, storm::abstraction::QualitativeMdpResultMinMax<DdType> const& qualitativeResults, CheckTask<storm::logic::Formula> const& checkTask);
            bool skipQuantitativeSolution(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, storm::abstraction::QualitativeMdpResultMinMax<DdType> const& qualitativeResults, CheckTask<storm::logic::Formula> const& checkTask);

            // Methods related to the quantitative solution.
            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeQuantitativeResult(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask, storm::dd::Bdd<DdType> const& constraintStates, storm::dd::Bdd<DdType> const& targetStates, storm::abstraction::QualitativeMdpResultMinMax<DdType> const& qualitativeResults);
            
            // Retrieves the constraint and target states of the quotient wrt. to the formula in the check task.
            std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> getConstraintAndTargetStates(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask);
            
            // Retrieves the extremal bound (wrt. to the optimization direction) of the quantitative check result.
            ValueType getExtremalBound(storm::OptimizationDirection dir, QuantitativeCheckResult<ValueType> const& result);
            
            // Retrieves whether the quantitative bounds are sufficient to answer the the query given by the bound (comparison
            // type and threshold).
            bool boundsSufficient(storm::models::Model<ValueType> const& quotient, bool lowerBounds, QuantitativeCheckResult<ValueType> const& result, storm::logic::ComparisonType comparisonType, ValueType const& threshold);
            
            // Methods to compute bounds on the partial quotient.
            std::unique_ptr<CheckResult> computeBoundsPartialQuotient(SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<DdType, ValueType>>& checker, storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, storm::OptimizationDirection const& dir, CheckTask<storm::logic::Formula>& checkTask);
            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeBoundsPartialQuotient(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask);
            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeBoundsPartialQuotient(storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask);
            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeBoundsPartialQuotient(storm::models::Model<ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask);
            
            // Methods to solve the query on the full quotient.
            std::unique_ptr<CheckResult> computeResultFullQuotient(storm::models::symbolic::Dtmc<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask);
            std::unique_ptr<CheckResult> computeResultFullQuotient(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask);
            std::unique_ptr<CheckResult> computeResultFullQuotient(storm::models::Model<ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask);
            
            // The non-abstracted model.
            ModelType const& model;
        };
    }
}
