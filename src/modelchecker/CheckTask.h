#ifndef STORM_MODELCHECKER_CHECKTASK_H_
#define STORM_MODELCHECKER_CHECKTASK_H_

#include <boost/optional.hpp>

#include "src/logic/Formulas.h"
#include "src/utility/constants.h"

#include "src/solver/OptimizationDirection.h"
#include "src/logic/ComparisonType.h"

namespace storm {
    namespace logic {
        class Formula;
    }
    
    namespace modelchecker {
        
        /*
         * This class is used to customize the checking process of a formula.
         */
        template<typename FormulaType, typename ValueType = double>
        class CheckTask {
        public:
            /*!
             * Creates an empty task object with the default options.
             */
            CheckTask() : CheckTask(boost::none, boost::none, boost::none, false, boost::none, false, false) {
                // Intentionally left empty.
            }
            
            /*!
             * Creates a task object with the default options for the given formula.
             */
            CheckTask(FormulaType const& formula)  {
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
            
            /*!
             * Copies the check task from the source while replacing the formula with the new one. In particular, this
             * changes the formula type of the check task object.
             */
            template<typename NewFormulaType>
            CheckTask<NewFormulaType, ValueType> replaceFormula(NewFormulaType const& newFormula) const {
                return CheckTask<NewFormulaType, ValueType>(newFormula, this->optimizationDirection, this->rewardModel, this->onlyInitialStatesRelevant, this->initialStatesBound, this->qualitative, this->produceStrategies);
            }
            
            /*!
             * Retrieves whether this task is associated with a formula.
             */
            bool hasFormula() const {
                return static_cast<bool>(formula);
            }
            
            /*!
             * Retrieves the formula from this task.
             */
            FormulaType const& getFormula() const {
                return formula.get().get();
            }
            
            /*!
             * Retrieves whether an optimization direction was set.
             */
            bool isOptimizationDirectionSet() const {
                return static_cast<bool>(optimizationDirection);
            }
            
            /*!
             * Retrieves the optimization direction (if set).
             */
            storm::OptimizationDirection const& getOptimizationDirection() const {
                return optimizationDirection.get();
            }
            
            /*!
             * Retrieves whether a reward model was set.
             */
            bool isRewardModelSet() const {
                return static_cast<bool>(rewardModel);
            }
            
            /*!
             * Retrieves the reward model over which to perform the checking (if set).
             */
            std::string const& getRewardModel() const {
                return rewardModel.get();
            }
            
            /*!
             * Retrieves whether only the initial states are relevant in the computation.
             */
            bool isOnlyInitialStatesRelevantSet() const {
                return onlyInitialStatesRelevant;
            }
            
            /*!
             * Retrieves whether there is a bound with which the values for the states will be compared.
             */
            bool isBoundSet() const {
                return static_cast<bool>(bound);
            }
            
            /*!
             * Retrieves the value of the bound (if set).
             */
            ValueType const& getBoundValue() const {
                return bound.get().second;
            }
            
            /*!
             * Retrieves the comparison type of the bound (if set).
             */
            storm::logic::ComparisonType const& getBoundComparisonType() const {
                return bound.get().first;
            }
            
            /*!
             * Retrieves the bound for the initial states (if set).
             */
            std::pair<storm::logic::ComparisonType, ValueType> const& getBound() const {
                return bound.get();
            }
            
            /*!
             * Retrieves whether the computation only needs to be performed qualitatively, because the values will only
             * be compared to 0/1.
             */
            bool isQualitativeSet() const {
                return qualitative;
            }
            
            /*!
             * Retrieves whether strategies are to be produced (if supported).
             */
            bool isProduceStrategiesSet() const {
                return produceStrategies;
            }
            
        private:
            /*!
             * Creates a task object with the given options.
             *
             * @param formula The formula to attach to the task.
             * @param optimizationDirection If set, the probabilities will be minimized/maximized.
             * @param rewardModelName If given, the checking has to be done wrt. to this reward model.
             * @param onlyInitialStatesRelevant If set to true, the model checker may decide to only compute the values
             * for the initial states.
             * @param initialStatesBound The bound with which the initial states will be compared. This may only be set
             * together with the flag that indicates only initial states of the model are relevant.
             * @param qualitative A flag specifying whether the property needs to be checked qualitatively, i.e. compared
             * with bounds 0/1.
             * @param produceStrategies If supported by the model checker and the model formalism, strategies to achieve
             * a value will be produced if this flag is set.
             */
            CheckTask(boost::optional<std::reference_wrapper<FormulaType>> const& formula, boost::optional<storm::OptimizationDirection> const& optimizationDirection, boost::optional<std::string> const& rewardModel, bool onlyInitialStatesRelevant, boost::optional<std::pair<storm::logic::ComparisonType, ValueType>> const& bound, bool qualitative, bool produceStrategies) : formula(formula), optimizationDirection(optimizationDirection), rewardModel(rewardModel), onlyInitialStatesRelevant(onlyInitialStatesRelevant), bound(bound), qualitative(qualitative), produceStrategies(produceStrategies) {
                // Intentionally left empty.
            }
            
            // The formula that is to be checked.
            boost::optional<std::reference_wrapper<FormulaType>> formula;
            
            // If set, the probabilities will be minimized/maximized.
            boost::optional<storm::OptimizationDirection> optimizationDirection;

            // If set, the reward property has to be interpreted over this model.
            boost::optional<std::string> rewardModel;
            
            // If set to true, the model checker may decide to only compute the values for the initial states.
            bool onlyInitialStatesRelevant;

            // The bound with which the initial states will be compared.
            boost::optional<std::pair<storm::logic::ComparisonType, ValueType>> bound;
            
            // A flag specifying whether the property needs to be checked qualitatively, i.e. compared with bounds 0/1.
            bool qualitative;
            
            // If supported by the model checker and the model formalism, strategies to achieve a value will be produced
            // if this flag is set.
            bool produceStrategies;
        };
        
    }
}

#endif /* STORM_MODELCHECKER_CHECKTASK_H_ */