#ifndef STORM_SOLVER_LPSOLVER
#define STORM_SOLVER_LPSOLVER

#include <string>
#include <vector>
#include <cstdint>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace solver {
        /*!
         * An interface that captures the functionality of an LP solver.
         */
        class LpSolver {
        public:
            // An enumeration to represent whether the objective function is to be minimized or maximized.
            enum class ModelSense {
                Minimize,
                Maximize
            };
            
            /*!
             * Creates an empty LP solver. By default the objective function is assumed to be minimized.
             */
            LpSolver() : manager(new storm::expressions::ExpressionManager()), currentModelHasBeenOptimized(false), modelSense(ModelSense::Minimize) {
                // Intentionally left empty.
            }
            
            /*!
             * Creates an empty LP solver with the given model sense.
             *
             * @param modelSense A value indicating whether the objective function of this model is to be minimized or
             * maximized.
             */
            LpSolver(ModelSense const& modelSense) : currentModelHasBeenOptimized(false), modelSense(modelSense) {
                // Intentionally left empty.
            }

            /*!
             * Registers an upper- and lower-bounded continuous variable, i.e. a variable that may take all real values
             * within its bounds.
             *
             * @param name The name of the variable.
             * @param lowerBound The lower bound of the variable.
             * @param upperBound The upper bound of the variable.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If omitted (or set to zero), the variable is irrelevant for the objective value.
             */
            virtual storm::expressions::Variable addBoundedContinuousVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient = 0) = 0;
            
            /*!
             * Registers a lower-bounded continuous variable, i.e. a variable that may take all real values up to its
             * lower bound.
             *
             * @param name The name of the variable.
             * @param lowerBound The lower bound of the variable.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If omitted (or set to zero), the variable is irrelevant for the objective value.
             */
            virtual storm::expressions::Variable addLowerBoundedContinuousVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient = 0) = 0;

            /*!
             * Registers an upper-bounded continuous variable, i.e. a variable that may take all real values up to its
             * upper bound.
             *
             * @param name The name of the variable.
             * @param upperBound The upper bound of the variable.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If omitted (or set to zero), the variable is irrelevant for the objective value.
             */
            virtual storm::expressions::Variable addUpperBoundedContinuousVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient = 0) = 0;
            
            /*!
             * Registers a unbounded continuous variable, i.e. a variable that may take all real values.
             *
             * @param name The name of the variable.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If omitted (or set to zero), the variable is irrelevant for the objective value.
             */
            virtual storm::expressions::Variable addUnboundedContinuousVariable(std::string const& name, double objectiveFunctionCoefficient = 0) = 0;
            
            /*!
             * Registers an upper- and lower-bounded integer variable, i.e. a variable that may take all integer values
             * within its bounds.
             *
             * @param name The name of the variable.
             * @param lowerBound The lower bound of the variable.
             * @param upperBound The upper bound of the variable.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If omitted (or set to zero), the variable is irrelevant for the objective value.
             */
            virtual storm::expressions::Variable addBoundedIntegerVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient = 0) = 0;
            
            /*!
             * Registers a lower-bounded integer variable, i.e. a variable that may take all integer values up to its
             * lower bound.
             *
             * @param name The name of the variable.
             * @param lowerBound The lower bound of the variable.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If omitted (or set to zero), the variable is irrelevant for the objective value.
             */
            virtual storm::expressions::Variable addLowerBoundedIntegerVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient = 0) = 0;
            
            /*!
             * Registers an upper-bounded integer variable, i.e. a variable that may take all integer values up to its
             * lower bound.
             *
             * @param name The name of the variable.
             * @param upperBound The upper bound of the variable.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If omitted (or set to zero), the variable is irrelevant for the objective value.
             */
            virtual storm::expressions::Variable addUpperBoundedIntegerVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient = 0) = 0;

            /*!
             * Registers an unbounded integer variable, i.e. a variable that may take all integer values.
             *
             * @param name The name of the variable.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If omitted (or set to zero), the variable is irrelevant for the objective value.
             */
            virtual storm::expressions::Variable addUnboundedIntegerVariable(std::string const& name, double objectiveFunctionCoefficient = 0) = 0;
            
            /*!
             * Registers a boolean variable, i.e. a variable that may be either 0 or 1.
             *
             * @param name The name of the variable.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If omitted (or set to zero), the variable is irrelevant for the objective value.
             */
            virtual storm::expressions::Variable addBinaryVariable(std::string const& name, double objectiveFunctionCoefficient = 0) = 0;
            
            /*!
             * Updates the model to make the variables that have been declared since the last call to <code>update</code>
             * usable.
             */
            virtual void update() const = 0;
            
            /*!
             * Adds a the given constraint to the LP problem.
             *
             * @param name The name of the constraint. If empty, the constraint has no name.
             * @param constraint An expression that represents the constraint. The given constraint must be a linear
             * (in)equality over the registered variables.
             */
            virtual void addConstraint(std::string const& name, storm::expressions::Expression const& constraint) = 0;
            
            /*!
             * Optimizes the LP problem previously constructed. Afterwards, the methods isInfeasible, isUnbounded and
             * isOptimal can be used to query the optimality status.
             */
            virtual void optimize() const = 0;
            
            /*!
             * Retrieves whether the model was found to be infeasible. This can only be called after the model has been
             * optimized and not modified afterwards. Otherwise, an exception is thrown.
             *
             * @return True if the model was optimized and found to be infeasible.
             */
            virtual bool isInfeasible() const = 0;
            
            /*!
             * Retrieves whether the model was found to be infeasible. This can only be called after the model has been
             * optimized and not modified afterwards. Otherwise, an exception is thrown.
             *
             * @return True if the model was optimized and found to be unbounded.
             */
            virtual bool isUnbounded() const = 0;
            
            /*!
             * Retrieves whether the model was found to be optimal, i.e. neither infeasible nor unbounded. This can only
             * be called after the model has been optimized and not modified afterwards. Otherwise, an exception is
             * thrown.
             *
             * @return True if the model was optimized and found to be neither infeasible nor unbounded.
             */
            virtual bool isOptimal() const = 0;
            
            /*!
             * Retrieves the value of the integer variable with the given name. Note that this may only be called, if
             * the model was found to be optimal, i.e. iff isOptimal() returns true.
             *
             * @param variable The variable whose integer value (in the optimal solution) to retrieve.
             * @return The value of the integer variable in the optimal solution.
             */
            virtual int_fast64_t getIntegerValue(storm::expressions::Variable const& variable) const = 0;
            
            /*!
             * Retrieves the value of the binary variable with the given name. Note that this may only be called, if
             * the model was found to be optimal, i.e. iff isOptimal() returns true.
             *
             * @param variable The variable whose integer value (in the optimal solution) to retrieve.
             * @return The value of the binary variable in the optimal solution.
             */
            virtual bool getBinaryValue(storm::expressions::Variable const& variable) const = 0;
            
            /*!
             * Retrieves the value of the continuous variable with the given name. Note that this may only be called,
             * if the model was found to be optimal, i.e. iff isOptimal() returns true.
             *
             * @param variable The variable whose integer value (in the optimal solution) to retrieve.
             * @return The value of the continuous variable in the optimal solution.
             */
            virtual double getContinuousValue(storm::expressions::Variable const& variable) const = 0;

            /*!
             * Retrieves the value of the objective function. Note that this may only be called, if the model was found
             * to be optimal,  i.e. iff isOptimal() returns true.
             *
             * @return The value of the objective function in the optimal solution.
             */
            virtual double getObjectiveValue() const = 0;
            
            /*!
             * Writes the current LP problem to the given file.
             *
             * @param filename The file to which to write the string representation of the LP.
             */
            virtual void writeModelToFile(std::string const& filename) const = 0;
            
            /*!
             * Sets whether the objective function of this model is to be minimized or maximized.
             *
             * @param modelSense The model sense to use.
             */
            void setModelSense(ModelSense const& modelSense) {
                if (modelSense != this->modelSense) {
                    currentModelHasBeenOptimized = false;
                }
                this->modelSense = modelSense;
            }
            
            /*!
             * Retrieves whether the objective function of this model is to be minimized or maximized.
             *
             * @return A value indicating whether the objective function of this model is to be minimized or maximized.
             */
            ModelSense getModelSense() const {
                return modelSense;
            }
            
        protected:
            // The manager responsible for the variables.
            std::shared_ptr<storm::expressions::ExpressionManager> manager;
            
            // A flag indicating whether the current model has been optimized and not changed afterwards.
            mutable bool currentModelHasBeenOptimized;
            
        private:
            // A flag that indicates the model sense.
            ModelSense modelSense;
        };
    }
}

#endif /* STORM_SOLVER_LPSOLVER */
