#ifndef STORM_SOLVER_GUROBILPSOLVER
#define STORM_SOLVER_GUROBILPSOLVER

#include <map>
#include <optional>
#include "storm/solver/LpSolver.h"
// To detect whether the usage of Gurobi is possible, this include is neccessary.
#include "storm-config.h"

#ifdef STORM_HAVE_GUROBI
extern "C" {
#include "gurobi_c.h"

int __stdcall GRBislp(GRBenv**, const char*, const char*, const char*, const char*);
}
#endif

namespace storm {
namespace solver {

class GurobiEnvironment {
   public:
    GurobiEnvironment() = default;
    GurobiEnvironment(GurobiEnvironment const&) = delete;
    GurobiEnvironment& operator=(GurobiEnvironment const&) = delete;
    virtual ~GurobiEnvironment();
    /*!
     * Sets some properties of the Gurobi environment according to parameters given by the options.
     */
    void initialize();
    void setOutput(bool set = false);
#ifdef STORM_HAVE_GUROBI
    GRBenv* operator*();
#endif
   private:
    bool initialized = false;
#ifdef STORM_HAVE_GUROBI
    GRBenv* env = nullptr;
#endif
};

/*!
 * A class that implements the LpSolver interface using Gurobi.
 */
template<typename ValueType>
class GurobiLpSolver : public LpSolver<ValueType> {
   public:
    /*!
     * Constructs a solver with the given name and model sense.
     *
     * @param name The name of the LP problem.
     * @param modelSense A value indicating whether the value of the objective function is to be minimized or
     * maximized.
     */
    GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const& environment, std::string const& name, OptimizationDirection const& optDir);

    /*!
     * Constructs a solver with the given name. By default the objective function is assumed to be minimized,
     * but this may be altered later using a call to setModelSense.
     *
     * @param name The name of the LP problem.
     */
    GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const& environment, std::string const& name);

    /*!
     * Constructs a solver without a name and the given model sense.
     *
     * @param modelSense A value indicating whether the value of the objective function is to be minimized or
     * maximized.
     */
    GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const& environment, OptimizationDirection const& optDir);

    /*!
     * Constructs a solver without a name. By default the objective function is assumed to be minimized,
     * but this may be altered later using a call to setModelSense.
     */
    GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const& environment);

    /*!
     * Creates a (deep) copy of this solver.
     * @param other
     */
    GurobiLpSolver(GurobiLpSolver<ValueType> const& other);

    /*!
     * Destructs a solver by freeing the pointers to Gurobi's structures.
     */
    virtual ~GurobiLpSolver();

    // Methods to add continuous variables.
    virtual storm::expressions::Variable addBoundedContinuousVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                      ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addLowerBoundedContinuousVariable(std::string const& name, ValueType lowerBound,
                                                                           ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addUpperBoundedContinuousVariable(std::string const& name, ValueType upperBound,
                                                                           ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addUnboundedContinuousVariable(std::string const& name, ValueType objectiveFunctionCoefficient = 0) override;

    // Methods to add integer variables.
    virtual storm::expressions::Variable addBoundedIntegerVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                   ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addLowerBoundedIntegerVariable(std::string const& name, ValueType lowerBound,
                                                                        ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addUpperBoundedIntegerVariable(std::string const& name, ValueType upperBound,
                                                                        ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addUnboundedIntegerVariable(std::string const& name, ValueType objectiveFunctionCoefficient = 0) override;

    // Methods to add binary variables.
    virtual storm::expressions::Variable addBinaryVariable(std::string const& name, ValueType objectiveFunctionCoefficient = 0) override;

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
    virtual ValueType getContinuousValue(storm::expressions::Variable const& name) const override;
    virtual int_fast64_t getIntegerValue(storm::expressions::Variable const& name) const override;
    virtual bool getBinaryValue(storm::expressions::Variable const& name) const override;
    virtual ValueType getObjectiveValue() const override;
    // Methods to print the LP problem to a file.
    virtual void writeModelToFile(std::string const& filename) const override;

    virtual void push() override;
    virtual void pop() override;

    virtual void setMaximalMILPGap(ValueType const& gap, bool relative) override;
    virtual ValueType getMILPGap(bool relative) const override;

    // Methods to retrieve values of sub-optimal solutions found along the way.
    void setMaximalSolutionCount(uint64_t value);  // How many solutions will be stored (at max)
    uint64_t getSolutionCount() const;             // How many solutions have been found
    ValueType getContinuousValue(storm::expressions::Variable const& name, uint64_t const& solutionIndex) const;
    int_fast64_t getIntegerValue(storm::expressions::Variable const& name, uint64_t const& solutionIndex) const;
    bool getBinaryValue(storm::expressions::Variable const& name, uint64_t const& solutionIndex) const;
    ValueType getObjectiveValue(uint64_t const& solutionIndex) const;

   private:
    /*!
     * Adds a variable with the given name, type, lower and upper bound and objective function coefficient.
     *
     * @param variable The variable to add.
     * @param variableType The type of the variable in terms of Gurobi's constants.
     * @param lowerBound The lower bound of the range of the variable.
     * @param upperBound The upper bound of the range of the variable.
     * @param objectiveFunctionCoefficient The coefficient of the variable in the objective function.
     */
    void addVariable(storm::expressions::Variable const& variable, char variableType, double lowerBound, double upperBound,
                     ValueType objectiveFunctionCoefficient);
#ifdef STORM_HAVE_GUROBI
    // The Gurobi model.
    GRBmodel* model;
#endif

    std::shared_ptr<GurobiEnvironment> environment;

    // The index of the next variable.
    int nextVariableIndex;

    // The index of the next constraint.
    int nextConstraintIndex;

    // A mapping from variables to their indices.
    std::map<storm::expressions::Variable, int> variableToIndexMap;

    struct IncrementalLevel {
        std::vector<storm::expressions::Variable> variables;
        int firstConstraintIndex;
    };
    std::vector<IncrementalLevel> incrementalData;
};

enum class GurobiSolverMethod { AUTOMATIC = -1, PRIMALSIMPLEX = 0, DUALSIMPLEX = 1, BARRIER = 2, CONCURRENT = 3, DETCONCURRENT = 4, DETCONCURRENTSIMPLEX = 5 };

/**
 * Yields a string representation of the GurobiSolverMethod
 * @param method
 * @return
 */
std::string toString(GurobiSolverMethod const& method);
std::optional<GurobiSolverMethod> gurobiSolverMethodFromString(std::string const&);
std::vector<GurobiSolverMethod> getGurobiSolverMethods();

}  // namespace solver
}  // namespace storm

#endif /* STORM_SOLVER_GUROBILPSOLVER */
