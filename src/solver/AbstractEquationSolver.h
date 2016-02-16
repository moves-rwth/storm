#ifndef STORM_SOLVER_ABSTRACTEQUATIONSOLVER_H_
#define STORM_SOLVER_ABSTRACTEQUATIONSOLVER_H_

#include "src/solver/TerminationCondition.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        class AbstractEquationSolver {
        public:
            /*!
             * Sets a custom termination condition that is used together with the regular termination condition of the
             * solver.
             *
             * @param terminationCondition An object that can be queried whether to terminate early or not.
             */
            void setTerminationCondition(std::unique_ptr<TerminationCondition<ValueType>> terminationCondition) {
                this->terminationCondition = std::move(terminationCondition);
            }
            
            /*!
             * Removes a previously set custom termination condition.
             */
            void resetTerminationCondition() {
                this->terminationCondition = nullptr;
            }

            /*!
             * Retrieves whether a custom termination condition has been set.
             */
            bool hasCustomTerminationCondition() const {
                return static_cast<bool>(this->terminationCondition);
            }
            
            /*!
             * Retrieves the custom termination condition (if any was set).
             * 
             * @return The custom termination condition.
             */
            TerminationCondition<ValueType> const& getTerminationCondition() const {
                return *terminationCondition;
            }
            
        protected:
            // A termination condition to be used (can be unset).
            std::unique_ptr<TerminationCondition<ValueType>> terminationCondition;
        };
        
    }
}

#endif /* STORM_SOLVER_ABSTRACTEQUATIONSOLVER_H_ */