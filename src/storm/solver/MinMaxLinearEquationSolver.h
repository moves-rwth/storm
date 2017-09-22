#ifndef STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_

#include <vector>
#include <cstdint>
#include <memory>

#include <boost/optional.hpp>

#include "storm/solver/AbstractEquationSolver.h"
#include "storm/solver/SolverSelectionOptions.h"
#include "storm/storage/sparse/StateType.h"
#include "storm/storage/Scheduler.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/MinMaxLinearEquationSolverSystemType.h"
#include "storm/solver/MinMaxLinearEquationSolverRequirements.h"

#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/utility/macros.h"

namespace storm {
    namespace storage {
        template<typename T> class SparseMatrix;
    }
    
    namespace solver {
        
        /*!
         * A class representing the interface that all min-max linear equation solvers shall implement.
         */
        template<class ValueType>
        class MinMaxLinearEquationSolver : public AbstractEquationSolver<ValueType> {
        protected:
            MinMaxLinearEquationSolver(OptimizationDirectionSetting direction = OptimizationDirectionSetting::Unset);
            
        public:
            virtual ~MinMaxLinearEquationSolver();
            
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& matrix) = 0;
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& matrix) = 0;
            
            /*!
             * Solves the equation system x = min/max(A*x + b) given by the parameters. Note that the matrix A has
             * to be given upon construction time of the solver object.
             *
             * @param d For minimum, all the value of a group of rows is the taken as the minimum over all rows and as
             * the maximum otherwise.
             * @param x The solution vector x. The initial values of x represent a guess of the real values to the
             * solver, but may be ignored.
             * @param b The vector to add after matrix-vector multiplication.
             */
            bool solveEquations(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            
            /*!
             * Behaves the same as the other variant of <code>solveEquations</code>, with the distinction that
             * instead of providing the optimization direction as an argument, the internally set optimization direction
             * is used. Note: this method can only be called after setting the optimization direction.
             */
            void solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            
            /*!
             * Performs (repeated) matrix-vector multiplication with the given parameters, i.e. computes
             * x[i+1] = min/max(A*x[i] + b) until x[n], where x[0] = x. After each multiplication and addition, the
             * minimal/maximal value out of each row group is selected to reduce the resulting vector to obtain the
             * vector for the next iteration. Note that the matrix A has to be given upon construction time of the
             * solver object.
             *
             * @param d For minimum, all the value of a group of rows is the taken as the minimum over all rows and as
             * the maximum otherwise.
             * @param x The initial vector that is to be multiplied with the matrix. This is also the output parameter,
             * i.e. after the method returns, this vector will contain the computed values.
             * @param b If not null, this vector is added after each multiplication.
             * @param n Specifies the number of iterations the matrix-vector multiplication is performed.
             * @param multiplyResult If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the number of rows of A.
             * @return The result of the repeated matrix-vector multiplication as the content of the vector x.
             */
            virtual void repeatedMultiply(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n = 1) const = 0;
            
            /*!
             * Behaves the same as the other variant of <code>multiply</code>, with the
             * distinction that instead of providing the optimization direction as an argument, the internally set
             * optimization direction is used. Note: this method can only be called after setting the optimization direction.
             */
            virtual void repeatedMultiply(std::vector<ValueType>& x, std::vector<ValueType>* b , uint_fast64_t n) const;
            
            /*!
             * Sets an optimization direction to use for calls to methods that do not explicitly provide one.
             */
            void setOptimizationDirection(OptimizationDirection direction);
            
            /*!
             * Unsets the optimization direction to use for calls to methods that do not explicitly provide one.
             */
            void unsetOptimizationDirection();
            
            /*!
             * Sets whether schedulers are generated when solving equation systems. If the argument is false, the currently
             * stored scheduler (if any) is deleted.
             */
            void setTrackScheduler(bool trackScheduler = true);
            
            /*!
             * Retrieves whether this solver is set to generate schedulers.
             */
            bool isTrackSchedulerSet() const;
            
            /*!
             * Retrieves whether the solver generated a scheduler.
             */
            bool hasScheduler() const;
            
            /*!
             * Retrieves the generated scheduler. Note: it is only legal to call this function if a scheduler was generated.
             */
            storm::storage::Scheduler<ValueType> computeScheduler() const;
            
            /*!
             * Retrieves the generated (deterministic) choices of the optimal scheduler. Note: it is only legal to call this function if a scheduler was generated.
             */
            std::vector<uint_fast64_t> const& getSchedulerChoices() const;
            
            /*!
             * Sets whether some of the generated data during solver calls should be cached.
             * This possibly decreases the runtime of subsequent calls but also increases memory consumption.
             */
            void setCachingEnabled(bool value);
            
            /*!
             * Retrieves whether some of the generated data during solver calls should be cached.
             */
            bool isCachingEnabled() const;
            
            /*
             * Clears the currently cached data that has been stored during previous calls of the solver.
             */
            virtual void clearCache() const;
            
            /*!
             * Sets a lower bound for the solution that can potentially used by the solver.
             */
            void setLowerBound(ValueType const& value);
            
            /*!
             * Sets an upper bound for the solution that can potentially used by the solver.
             */
            void setUpperBound(ValueType const& value);
            
            /*!
             * Sets bounds for the solution that can potentially used by the solver.
             */
            void setBounds(ValueType const& lower, ValueType const& upper);
            
            /*!
             * Sets a valid initial scheduler that is required by some solvers (see requirements of solvers).
             */
            void setInitialScheduler(std::vector<uint_fast64_t>&& choices);
            
            /*!
             * Returns true iff an initial scheduler is set.
             */
            bool hasInitialScheduler() const;
            
            /*!
             * Retrieves the initial scheduler if one was set.
             */
            std::vector<uint_fast64_t> const& getInitialScheduler() const;
            
            /*!
             * Retrieves the requirements of this solver for solving equations with the current settings. The requirements
             * are guaranteed to be ordered according to their appearance in the SolverRequirement type.
             */
            virtual MinMaxLinearEquationSolverRequirements getRequirements(MinMaxLinearEquationSolverSystemType const& equationSystemType, boost::optional<storm::solver::OptimizationDirection> const& direction = boost::none) const;
            
            /*!
             * Notifies the solver that the requirements for solving equations have been checked. If this has not been
             * done before solving equations, the solver might issue a warning, perform the checks itself or even fail.
             */
            void setRequirementsChecked(bool value = true);
            
            /*!
             * Retrieves whether the solver is aware that the requirements were checked.
             */
            bool isRequirementsCheckedSet() const;
            
        protected:
            virtual bool internalSolveEquations(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const& b) const = 0;
            
            /// The optimization direction to use for calls to functions that do not provide it explicitly. Can also be unset.
            OptimizationDirectionSetting direction;
            
            /// Whether we generate a scheduler during solving.
            bool trackScheduler;
            
            /// The scheduler choices that induce the optimal values (if they could be successfully generated).
            mutable boost::optional<std::vector<uint_fast64_t>> schedulerChoices;
            
            // A lower bound if one was set.
            boost::optional<ValueType> lowerBound;
            
            // An upper bound if one was set.
            boost::optional<ValueType> upperBound;
            
            // A scheduler that can be used by solvers that require a valid initial scheduler.
            boost::optional<std::vector<uint_fast64_t>> initialScheduler;
            
        private:
            /// Whether some of the generated data during solver calls should be cached.
            bool cachingEnabled;
            
            /// A flag storing whether the requirements of the solver were checked.
            bool requirementsChecked;
        };
        
        template<typename ValueType>
        class MinMaxLinearEquationSolverFactory {
        public:
            MinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
            
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const;
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType>&& matrix) const;
            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create() const = 0;

            void setTrackScheduler(bool value);
            bool isTrackSchedulerSet() const;
            
            virtual void setMinMaxMethod(MinMaxMethodSelection const& newMethod);
            virtual void setMinMaxMethod(MinMaxMethod const& newMethod);
            
            MinMaxMethod const& getMinMaxMethod() const;
            
            /*!
             * Retrieves the requirements of the solver that would be created when calling create() right now. The
             * requirements are guaranteed to be ordered according to their appearance in the SolverRequirement type.
             */
            MinMaxLinearEquationSolverRequirements getRequirements(MinMaxLinearEquationSolverSystemType const& equationSystemType, boost::optional<storm::solver::OptimizationDirection> const& direction = boost::none) const;
            void setRequirementsChecked(bool value = true);
            bool isRequirementsCheckedSet() const;

        private:
            bool trackScheduler;
            MinMaxMethod method;
            bool requirementsChecked;
        };
        
        template<typename ValueType>
        class GeneralMinMaxLinearEquationSolverFactory : public MinMaxLinearEquationSolverFactory<ValueType> {
        public:
            GeneralMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
            
            // Make the other create methods visible.
            using MinMaxLinearEquationSolverFactory<ValueType>::create;
            
            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create() const override;
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_ */
