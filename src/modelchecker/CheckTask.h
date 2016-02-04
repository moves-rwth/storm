#ifndef STORM_MODELCHECKER_CHECKTASK_H_
#define STORM_MODELCHECKER_CHECKTASK_H_

#include <boost/optional.hpp>

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
            CheckTask();
            
            /*!
             * Creates a task object with the default options for the given formula.
             */
            CheckTask(FormulaType const& formula);
            
            /*!
             * Converts the check task object to one with the given new formula type.
             */
            template<typename NewFormulaType>
            CheckTask<NewFormulaType, ValueType> convert();
            
            /*!
             * Retrieves whether this task is associated with a formula.
             */
            bool hasFormula() const;
            
            /*!
             * Retrieves the formula from this task.
             */
            FormulaType const& getFormula() const;
            
            /*!
             * Retrieves whether an optimization direction was set.
             */
            bool isOptimizationDirectionSet() const;
            
            /*!
             * Retrieves the optimization direction (if set).
             */
            storm::OptimizationDirection const& getOptimizationDirection() const;
            
            /*!
             * Retrieves whether a reward model was set.
             */
            bool isRewardModelSet() const;
            
            /*!
             * Retrieves the reward model over which to perform the checking (if set).
             */
            std::string const& getRewardModel() const;
            
            /*!
             * Retrieves whether only the initial states are relevant in the computation.
             */
            bool isOnlyInitialStatesRelevantSet() const;
            
            /*!
             * Retrieves whether there is a bound with which the values for the initial states will be compared.
             */
            bool isInitialStatesBoundSet() const;
            
            /*!
             * Retrieves the bound for the initial states (if set).
             */
            std::pair<storm::logic::ComparisonType, ValueType> const& getInitialStatesBound() const;
            
            /*!
             * Retrieves whether the computation only needs to be performed qualitatively, because the values will only
             * be compared to 0/1.
             */
            bool isQualitativeSet() const;
            
            /*!
             * Retrieves whether strategies are to be produced (if supported).
             */
            bool isProduceStrategiesSet() const;
            
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
            CheckTask(boost::optional<std::reference_wrapper<FormulaType>> const& formula, boost::optional<storm::OptimizationDirection> const& optimizationDirection, boost::optional<std::string> const& rewardModel, bool onlyInitialStatesRelevant, boost::optional<std::pair<storm::logic::ComparisonType, ValueType>> const& initialStatesBound, bool qualitative, bool produceStrategies);
            
            // The formula that is to be checked.
            boost::optional<std::reference_wrapper<FormulaType>> formula;
            
            // If set, the probabilities will be minimized/maximized.
            boost::optional<storm::OptimizationDirection> optimizationDirection;

            // If set, the reward property has to be interpreted over this model.
            boost::optional<std::string> rewardModel;
            
            // If set to true, the model checker may decide to only compute the values for the initial states.
            bool onlyInitialStatesRelevant;

            // The bound with which the initial states will be compared.
            boost::optional<std::pair<storm::logic::ComparisonType, ValueType>> initialStatesBound;
            
            // A flag specifying whether the property needs to be checked qualitatively, i.e. compared with bounds 0/1.
            bool qualitative;
            
            // If supported by the model checker and the model formalism, strategies to achieve a value will be produced
            // if this flag is set.
            bool produceStrategies;
        };
        
    }
}

#endif /* STORM_MODELCHECKER_CHECKTASK_H_ */