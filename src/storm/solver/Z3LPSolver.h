#ifndef STORM_SOLVER_Z3LPSOLVER
#define STORM_SOLVER_Z3LPSOLVER

#include <map>
#include "storm/solver/LpSolver.h"
// To detect whether the usage of Z3 is possible, this include is neccessary.
#include "storm-config.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/adapters/Z3ExpressionAdapter.h"


#ifdef STORM_HAVE_Z3
    #include "z3++.h"
    #include "z3.h"
#endif


namespace storm {
    namespace solver {

        /*!
         * A class that implements the LpSolver interface using Z3.
         */
        class Z3LpSolver : public LpSolver {
        public:
            /*!
             * Constructs a solver with the given name and optimization direction
             *
             * @param name The name of the LP problem.
             * @param optDir A value indicating whether the value of the objective function is to be minimized or
             * maximized.
             */
            Z3LpSolver(std::string const& name, OptimizationDirection const& optDir);
            
            /*!
             * Constructs a solver with the given name. By default the objective function is assumed to be minimized,
             * but this may be altered later using a call to setOptimizationDirection.
             *
             * @param name The name of the LP problem.
             */
            Z3LpSolver(std::string const& name);
            
            /*!
             * Constructs a solver without a name and the given optimization direction.
             *
             * @param optDir A value indicating whether the value of the objective function is to be minimized or
             * maximized.
             */
            Z3LpSolver(OptimizationDirection const& optDir);
            
            /*!
             * Constructs a solver without a name. By default the objective function is assumed to be minimized,
             * but this may be altered later using a call to setModelSense.
             */
            Z3LpSolver();
            
            /*!
             * Destructs a solver by freeing the pointers to Z3's structures.
             */
            virtual ~Z3LpSolver();
            
            // Methods to add continuous variables.
            virtual storm::expressions::Variable addBoundedContinuousVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient = 0) override;
            virtual storm::expressions::Variable addLowerBoundedContinuousVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient = 0) override;
            virtual storm::expressions::Variable addUpperBoundedContinuousVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient = 0) override;
            virtual storm::expressions::Variable addUnboundedContinuousVariable(std::string const& name, double objectiveFunctionCoefficient = 0) override;
            
            // Methods to add integer variables.
            virtual storm::expressions::Variable addBoundedIntegerVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient = 0) override;
            virtual storm::expressions::Variable addLowerBoundedIntegerVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient = 0) override;
            virtual storm::expressions::Variable addUpperBoundedIntegerVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient = 0) override;
            virtual storm::expressions::Variable addUnboundedIntegerVariable(std::string const& name, double objectiveFunctionCoefficient = 0) override;
            
            // Methods to add binary variables.
            virtual storm::expressions::Variable addBinaryVariable(std::string const& name, double objectiveFunctionCoefficient = 0) override;
            
            // Methods to incorporate recent changes.
            virtual void update() const override;
            
            // Methods to add constraints
            virtual void addConstraint(std::string const& name, storm::expressions::Expression const& constraint) override;
            
            // Methods to optimize and retrieve optimality status.
            virtual void optimize() const override;
            virtual bool isInfeasible() const override;
            virtual bool isUnbounded() const override;
            virtual bool isOptimal() const override;
            
            // Methods to retrieve values of variables and the objective function in the optimal solutions.
            virtual double getContinuousValue(storm::expressions::Variable const& name) const override;
            virtual int_fast64_t getIntegerValue(storm::expressions::Variable const& name) const override;
            virtual bool getBinaryValue(storm::expressions::Variable const& name) const override;
            virtual double getObjectiveValue() const override;
            
            // Methods to print the LP problem to a file.
            virtual void writeModelToFile(std::string const& filename) const override;

        private:
             virtual storm::expressions::Expression getValue(storm::expressions::Variable const& name) const;


#ifdef STORM_HAVE_Z3

            // The context used by the solver.
            std::unique_ptr<z3::context> context;

            // The actual solver object.
            std::unique_ptr<z3::optimize> solver;

            // The result of the most recent check
            mutable z3::check_result z3CheckResult;

            // The handle for the most recent optimization call
            mutable z3::optimize::handle z3Handle;

            // The model for the most recent optimization call
            mutable std::unique_ptr<z3::model> z3Model;

            // An expression adapter that is used for translating the expression into Z3's format.
            std::unique_ptr<storm::adapters::Z3ExpressionAdapter> expressionAdapter;

            // The function that is to be optimized
            storm::expressions::Expression optimizationFunction;

#endif

            // The name of the solver
            std::string name;
        };

    }
}

#endif /* STORM_SOLVER_Z3LPSOLVER */