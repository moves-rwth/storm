#ifndef STORM_SOLVER_LPSOLVER
#define STORM_SOLVER_LPSOLVER

#include <string>
#include <vector>

namespace storm {
    namespace solver {

        /*!
         * An interface that captures the functionality of an LP solver.
         */
        class LpSolver {
        public:
            // An enumeration to represent the possible types of variables. Variables may be either unbounded, have only
            // a lower or an upper bound, respectively, or be bounded from below and from above.
            enum VariableType {
                UNBOUNDED,
                LOWER_BOUND,
                UPPER_BOUND,
                BOUNDED,
            };
            
            // An enumeration to represent the possible types of constraints.
            enum BoundType {
                LESS,
                LESS_EQUAL,
                GREATER,
                GREATER_EQUAL,
                EQUAL
            };
            
            // An enumeration to represent whether the objective function is to be minimized or maximized.
            enum ModelSense {
                MINIMIZE,
                MAXIMIZE
            };
            
            /*!
             * Creates an empty LP solver. By default the objective function is assumed to be minimized.
             */
            LpSolver() : currentModelHasBeenOptimized(false), modelSense(MINIMIZE) {
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
             * Creates a continuous variable, i.e. a variable that may take all real values within its bounds.
             *
             * @param name The name of the variable.
             * @param variableType The type of the variable.
             * @param lowerBound The lower bound of the variable. Note that this parameter is ignored if the variable 
             * is not bounded from below.
             * @param upperBound The upper bound of the variable. Note that this parameter is ignored if the variable
             * is not bounded from above.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If set to zero, the variable is irrelevant for the objective value.
             * @return The index of the newly created variable. This index can be used to retrieve the variables value
             * in a solution after the model has been optimized.
             */
            virtual uint_fast64_t createContinuousVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) = 0;
            
            /*!
             * Creates an integer variable.
             *
             * @param name The name of the variable.
             * @param variableType The type of the variable.
             * @param lowerBound The lower bound of the variable. Note that this parameter is ignored if the variable
             * is not bounded from below.
             * @param upperBound The upper bound of the variable. Note that this parameter is ignored if the variable
             * is not bounded from above.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If set to zero, the variable is irrelevant for the objective value.
             * @return The index of the newly created variable. This index can be used to retrieve the variables value
             * in a solution after the model has been optimized.
             */
            virtual uint_fast64_t createIntegerVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) = 0;
            
            /*!
             * Creates a binary variable, i.e. a variable that may be either zero or one.
             *
             * @param name The name of the variable.
             * @param variableType The type of the variable.
             * @param lowerBound The lower bound of the variable. Note that this parameter is ignored if the variable
             * is not bounded from below.
             * @param upperBound The upper bound of the variable. Note that this parameter is ignored if the variable
             * is not bounded from above.
             * @param objectiveFunctionCoefficient The coefficient with which the variable appears in the objective
             * function. If set to zero, the variable is irrelevant for the objective value.
             * @return The index of the newly created variable. This index can be used to retrieve the variables value
             * in a solution after the model has been optimized.
             */
            virtual uint_fast64_t createBinaryVariable(std::string const& name, double objectiveFunctionCoefficient) = 0;
            
            /*!
             * Updates the model to make the variables that have been declared since the last call to <code>update</code>
             * usable.
             */
            virtual void update() const = 0;
            
            /*!
             * Adds a constraint of the form
             *      a_1*x_1 + ... + a_n*x_n  op  c
             * to the LP problem.
             *
             * @param name The name of the constraint. If empty, the constraint has no name.
             * @param variables A vector of variable indices that define the appearing variables x_1, ..., x_n.
             * @param coefficients A vector of coefficients that define the a_1, ..., a_n. The i-ith entry specifies the
             * coefficient of the variable whose index appears at the i-th position in the vector of variable indices.
             * @param boundType The bound type specifies the operator op used in the constraint.
             * @param rhs This defines the value of the constant appearing on the right-hand side of the constraint.
             */
            virtual void addConstraint(std::string const& name, std::vector<uint_fast64_t> const& variables, std::vector<double> const& coefficients, BoundType const& boundType, double rhs) = 0;
            
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
             * Retrieves the value of the integer variable with the given index. Note that this may only be called, if
             * the model was found to be optimal, i.e. iff isOptimal() returns true.
             *
             * @param variableIndex The index of the integer variable whose value to query. If this index does not
             * belong to a previously declared integer variable, the behaviour is undefined.
             * @return The value of the integer variable in the optimal solution.
             */
            virtual int_fast64_t getIntegerValue(uint_fast64_t variableIndex) const = 0;
            
            /*!
             * Retrieves the value of the binary variable with the given index. Note that this may only be called, if
             * the model was found to be optimal, i.e. iff isOptimal() returns true.
             *
             * @param variableIndex The index of the binary variable whose value to query. If this index does not
             * belong to a previously declared binary variable, the behaviour is undefined.
             * @return The value of the binary variable in the optimal solution.
             */
            virtual bool getBinaryValue(uint_fast64_t variableIndex) const = 0;
            
            /*!
             * Retrieves the value of the continuous variable with the given index. Note that this may only be called,
             * if the model was found to be optimal, i.e. iff isOptimal() returns true.
             *
             * @param variableIndex The index of the continuous variable whose value to query. If this index does not
             * belong to a previously declared continuous variable, the behaviour is undefined.
             * @return The value of the continuous variable in the optimal solution.
             */
            virtual double getContinuousValue(uint_fast64_t variableIndex) const = 0;

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
            // A flag indicating whether the current model has been optimized and not changed afterwards.
            mutable bool currentModelHasBeenOptimized;
            
        private:
            // A flag that indicates the model sense.
            ModelSense modelSense;
        };
    }
}

#endif /* STORM_SOLVER_LPSOLVER */
