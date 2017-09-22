#ifndef STORM_SOLVER_ABSTRACTEQUATIONSOLVER_H_
#define STORM_SOLVER_ABSTRACTEQUATIONSOLVER_H_

#include <memory>

#include <boost/optional.hpp>

#include "storm/solver/TerminationCondition.h"

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
            void setTerminationCondition(std::unique_ptr<TerminationCondition<ValueType>> terminationCondition);
            
            /*!
             * Removes a previously set custom termination condition.
             */
            void resetTerminationCondition();

            /*!
             * Retrieves whether a custom termination condition has been set.
             */
            bool hasCustomTerminationCondition() const;
            
            /*!
             * Checks whether the solver can terminate wrt. to its termination condition. If no termination condition,
             * this will yield false.
             */
            bool terminateNow(std::vector<ValueType> const& values, SolverGuarantee const& guarantee) const;
            
            /*!
             * Retrieves whether this solver has particularly relevant values.
             */
            bool hasRelevantValues() const;
            
            /*!
             * Retrieves the relevant values (if there are any).
             */
            storm::storage::BitVector const& getRelevantValues() const;
            
            /*!
             * Sets the relevant values.
             */
            void setRelevantValues(storm::storage::BitVector&& valuesOfInterest);

            /*!
             * Removes the values of interest (if there were any).
             */
            void clearRelevantValues();
            
            enum class BoundType {
                Global,
                Local,
                Any
            };
            
            /*!
             * Retrieves whether this solver has a lower bound.
             */
            bool hasLowerBound(BoundType const& type = BoundType::Any) const;
            
            /*!
             * Retrieves whether this solver has an upper bound.
             */
            bool hasUpperBound(BoundType const& type = BoundType::Any) const;
            
            /*!
             * Sets a lower bound for the solution that can potentially be used by the solver.
             */
            void setLowerBound(ValueType const& value);
            
            /*!
             * Sets an upper bound for the solution that can potentially be used by the solver.
             */
            void setUpperBound(ValueType const& value);
            
            /*!
             * Sets bounds for the solution that can potentially be used by the solver.
             */
            void setBounds(ValueType const& lower, ValueType const& upper);
            
            /*!
             * Retrieves the lower bound (if there is any).
             */
            ValueType const& getLowerBound() const;
            
            /*!
             * Retrieves the upper bound (if there is any).
             */
            ValueType const& getUpperBound() const;
            
            /*!
             * Retrieves a vector containing the lower bounds (if there are any).
             */
            std::vector<ValueType> const& getLowerBounds() const;
            
            /*!
             * Retrieves a vector containing the upper bounds (if there are any).
             */
            std::vector<ValueType> const& getUpperBounds() const;
            
            /*!
             * Sets lower bounds for the solution that can potentially be used by the solver.
             */
            void setLowerBounds(std::vector<ValueType> const& values);
            
            /*!
             * Sets upper bounds for the solution that can potentially be used by the solver.
             */
            void setUpperBounds(std::vector<ValueType> const& values);
            
            /*!
             * Sets upper bounds for the solution that can potentially be used by the solver.
             */
            void setUpperBounds(std::vector<ValueType>&& values);
            
            /*!
             * Sets bounds for the solution that can potentially be used by the solver.
             */
            void setBounds(std::vector<ValueType> const& lower, std::vector<ValueType> const& upper);
            
        protected:
            /*!
             * Retrieves the custom termination condition (if any was set).
             *
             * @return The custom termination condition.
             */
            TerminationCondition<ValueType> const& getTerminationCondition() const;
            
            void createUpperBoundsVector(std::unique_ptr<std::vector<ValueType>>& upperBoundsVector, uint64_t length) const;
            void createLowerBoundsVector(std::vector<ValueType>& lowerBoundsVector) const;

            // A termination condition to be used (can be unset).
            std::unique_ptr<TerminationCondition<ValueType>> terminationCondition;
            
            // A bit vector containing the indices of the relevant values if they were set.
            boost::optional<storm::storage::BitVector> relevantValues;
            
            // A lower bound if one was set.
            boost::optional<ValueType> lowerBound;
            
            // An upper bound if one was set.
            boost::optional<ValueType> upperBound;
            
            // Lower bounds if they were set.
            boost::optional<std::vector<ValueType>> lowerBounds;
            
            // Lower bounds if they were set.
            boost::optional<std::vector<ValueType>> upperBounds;
        };
        
    }
}

#endif /* STORM_SOLVER_ABSTRACTEQUATIONSOLVER_H_ */
