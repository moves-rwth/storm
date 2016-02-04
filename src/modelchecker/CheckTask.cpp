#include "src/modelchecker/CheckTask.h"

#include "src/logic/Formulas.h"

#include "src/utility/constants.h"

namespace storm {
    namespace modelchecker {

        template<typename FormulaType, typename ValueType>
        CheckTask<FormulaType, ValueType>::CheckTask() : CheckTask(boost::none, boost::none, boost::none, false, boost::none, false, false) {
            // Intentionally left empty.
        }
        
        template<typename FormulaType, typename ValueType>
        CheckTask<FormulaType, ValueType>::CheckTask(boost::optional<std::reference_wrapper<FormulaType>> const& formula, boost::optional<storm::OptimizationDirection> const& optimizationDirection, boost::optional<std::string> const& rewardModel, bool onlyInitialStatesRelevant, boost::optional<std::pair<storm::logic::ComparisonType, ValueType>> const& initialStatesBound, bool qualitative, bool produceStrategies) : formula(formula), optimizationDirection(optimizationDirection), rewardModel(rewardModel), onlyInitialStatesRelevant(onlyInitialStatesRelevant), initialStatesBound(initialStatesBound), qualitative(qualitative), produceStrategies(produceStrategies) {
            // Intentionally left empty.
        }
        
        template<typename FormulaType, typename ValueType>
        CheckTask<FormulaType, ValueType>::CheckTask(FormulaType const& formula) {
            this->onlyInitialStatesRelevant = false;
            this->produceStrategies = true;
            this->qualitative = false;
            
            if (formula.isProbabilityOperatorFormula()) {
                storm::logic::ProbabilityOperatorFormula const& probabilityOperatorFormula = formula.asProbabilityOperatorFormula();
                if (probabilityOperatorFormula.hasOptimalityType()) {
                    this->optimizationDirection = probabilityOperatorFormula.getOptimalityType();
                }
                
                if (probabilityOperatorFormula.hasBound()) {
                    if (onlyInitialStatesRelevant) {
                        this->initialStatesBound = std::make_pair(probabilityOperatorFormula.getComparisonType(), static_cast<ValueType>(probabilityOperatorFormula.getBound()));
                    }
                    if (probabilityOperatorFormula.getBound() == storm::utility::zero<ValueType>() || probabilityOperatorFormula.getBound() == storm::utility::one<ValueType>()) {
                        this->qualitative = true;
                    }
                    if (!optimizationDirection) {
                        this->optimizationDirection = probabilityOperatorFormula.getComparisonType() == storm::logic::ComparisonType::Less || probabilityOperatorFormula.getComparisonType() == storm::logic::ComparisonType::LessEqual ? OptimizationDirection::Maximize : OptimizationDirection::Minimize;
                    }
                }
            } else if (formula.isRewardOperatorFormula()) {
                storm::logic::RewardOperatorFormula const& rewardOperatorFormula = formula.asRewardOperatorFormula();
                this->rewardModel = rewardOperatorFormula.getOptionalRewardModelName();

                if (rewardOperatorFormula.hasOptimalityType()) {
                    this->optimizationDirection = rewardOperatorFormula.getOptimalityType();
                }
                
                if (rewardOperatorFormula.hasBound()) {
                    if (onlyInitialStatesRelevant) {
                        this->initialStatesBound = std::make_pair(rewardOperatorFormula.getComparisonType(), static_cast<ValueType>(rewardOperatorFormula.getBound()));
                    }
                    if (rewardOperatorFormula.getBound() == storm::utility::zero<ValueType>()) {
                        this->qualitative = true;
                    }
                    if (!optimizationDirection) {
                        this->optimizationDirection = rewardOperatorFormula.getComparisonType() == storm::logic::ComparisonType::Less || rewardOperatorFormula.getComparisonType() == storm::logic::ComparisonType::LessEqual ? OptimizationDirection::Maximize : OptimizationDirection::Minimize;
                    }
                }
            }
        }
        
        template<typename FormulaType, typename ValueType>
        template<typename NewFormulaType>
        CheckTask<NewFormulaType, ValueType> CheckTask<FormulaType, ValueType>::convert() {
            return CheckTask<NewFormulaType, ValueType>();
        }
        
        template<typename FormulaType, typename ValueType>
        bool CheckTask<FormulaType, ValueType>::hasFormula() const {
            return static_cast<bool>(formula);
        }
        
        template<typename FormulaType, typename ValueType>
        FormulaType const& CheckTask<FormulaType, ValueType>::getFormula() const {
            return formula.get().get();
        }
        
        template<typename FormulaType, typename ValueType>
        bool CheckTask<FormulaType, ValueType>::isOptimizationDirectionSet() const {
            return static_cast<bool>(optimizationDirection);
        }
        
        template<typename FormulaType, typename ValueType>
        storm::OptimizationDirection const& CheckTask<FormulaType, ValueType>::getOptimizationDirection() const {
            return optimizationDirection.get();
        }
        
        template<typename FormulaType, typename ValueType>
        bool CheckTask<FormulaType, ValueType>::isRewardModelSet() const {
            return static_cast<bool>(rewardModel);
        }
        
        template<typename FormulaType, typename ValueType>
        std::string const& CheckTask<FormulaType, ValueType>::getRewardModel() const {
            return rewardModel.get();
        }
        
        template<typename FormulaType, typename ValueType>
        bool CheckTask<FormulaType, ValueType>::isOnlyInitialStatesRelevantSet() const {
            return onlyInitialStatesRelevant;
        }
        
        template<typename FormulaType, typename ValueType>
        bool CheckTask<FormulaType, ValueType>::isInitialStatesBoundSet() const {
            return static_cast<bool>(initialStatesBound);
        }
        
        template<typename FormulaType, typename ValueType>
        std::pair<storm::logic::ComparisonType, ValueType> const& CheckTask<FormulaType, ValueType>::getInitialStatesBound() const {
            return initialStatesBound.get();
        }
        
        template<typename FormulaType, typename ValueType>
        bool CheckTask<FormulaType, ValueType>::isQualitativeSet() const {
            return qualitative;
        }
        
        template<typename FormulaType, typename ValueType>
        bool CheckTask<FormulaType, ValueType>::isProduceStrategiesSet() const {
            return produceStrategies;
        }
        
        template class CheckTask<storm::logic::ProbabilityOperatorFormula, double>;
        
    }
}