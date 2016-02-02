#ifndef STORM_MODELCHECKER_CHECKSETTINGS_H_
#define STORM_MODELCHECKER_CHECKSETTINGS_H_

#include <boost/optional.hpp>

#include "src/solver/OptimizationDirection.h"
#include "src/logic/ComparisonType.h"

namespace storm {
    namespace modelchecker {
        
        /*
         * This class is used to customize the checking process of a formula.
         */
        template<typename ValueType>
        class CheckSettings {
        public:
            /*!
             * Creates a settings object with the default options.
             */
            CheckSettings();
            
            /*!
             * Creates a settings object with the given options.
             *
             * @param optimizationDirection If set, the probabilities will be minimized/maximized.
             * @param onlyInitialStatesRelevant If set to true, the model checker may decide to only compute the values
             * for the initial states.
             * @param initialStatesBound The bound with which the initial states will be compared. This may only be set
             * together with the flag that indicates only initial states of the model are relevant.
             * @param qualitative A flag specifying whether the property needs to be checked qualitatively, i.e. compared
             * with bounds 0/1.
             * @param produceStrategies If supported by the model checker and the model formalism, strategies to achieve
             * a value will be produced if this flag is set.
             */
            CheckSettings(boost::optional<storm::OptimizationDirection> const& optimizationDirection, bool onlyInitialStatesRelevant, boost::optional<std::pair<storm::logic::ComparisonType, ValueType>> const& initialStatesBound, bool qualitative, bool produceStrategies);
            
        private:
            boost::optional<std::pair<storm::logic::ComparisonType, ValueType>> initialStatesBound;
            boost::optional<storm::OptimizationDirection> optimizationDirection;
            bool qualitative;
            bool produceStrategies;
            bool onlyInitialStatesRelevant;
        };
        
    }
}

#endif /* STORM_MODELCHECKER_CHECKSETTINGS_H_ */