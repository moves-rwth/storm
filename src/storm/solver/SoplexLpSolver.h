#pragma once
#include "soplex.h"

#include <map>
#include <optional>
#include "storm/solver/LpSolver.h"
// To detect whether the usage of Gurobi is possible, this include is neccessary.
#include "storm-config.h"

namespace storm::solver {

template<typename ValueType>
class SoplexLpSolver : public LpSolver<ValueType> {
   public:
    /*!
     * Constructs a solver with the given name and model sense.
     *
     * @param name The name of the LP problem.
     * @param modelSense A value indicating whether the value of the objective function is to be minimized or
     * maximized.
     */
    SoplexLpSolver(std::string const& name, OptimizationDirection const& optDir);

    /*!
     * Constructs a solver with the given name. By default the objective function is assumed to be minimized,
     * but this may be altered later using a call to setModelSense.
     *
     * @param name The name of the LP problem.
     */
    SoplexLpSolver(std::string const& name);

    /*!
     * Constructs a solver without a name and the given model sense.
     *
     * @param modelSense A value indicating whether the value of the objective function is to be minimized or
     * maximized.
     */
    SoplexLpSolver(OptimizationDirection const& optDir = OptimizationDirection::Minimize);

    /*!
     * Creates a (deep) copy of this solver.
     * @param other
     */
    SoplexLpSolver(SoplexLpSolver<ValueType> const& other);

    /*!
     * Destructs a solver by freeing the pointers to Gurobi's structures.
     */
    virtual ~SoplexLpSolver();

    // Methods to add continuous variables.
    virtual storm::expressions::Variable addBoundedContinuousVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                      ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addLowerBoundedContinuousVariable(std::string const& name, ValueType lowerBound,
                                                                           ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addUpperBoundedContinuousVariable(std::string const& name, ValueType upperBound,
                                                                           ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addUnboundedContinuousVariable(std::string const& name, ValueType objectiveFunctionCoefficient = 0) override;

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
    virtual ValueType getObjectiveValue() const override;
    // Methods to print the LP problem to a file.
    virtual void writeModelToFile(std::string const& filename) const override;

    virtual void push() override;
    virtual void pop() override;

    //////////////////
    // MILP related methods not supported by SoPlex
    //////////////////
    virtual storm::expressions::Variable addBoundedIntegerVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                   ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addLowerBoundedIntegerVariable(std::string const& name, ValueType lowerBound,
                                                                        ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addUpperBoundedIntegerVariable(std::string const& name, ValueType upperBound,
                                                                        ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addUnboundedIntegerVariable(std::string const& name, ValueType objectiveFunctionCoefficient = 0) override;
    virtual storm::expressions::Variable addBinaryVariable(std::string const& name, ValueType objectiveFunctionCoefficient = 0) override;
    virtual void setMaximalMILPGap(ValueType const& gap, bool relative) override;
    virtual ValueType getMILPGap(bool relative) const override;
    virtual int_fast64_t getIntegerValue(storm::expressions::Variable const& name) const override;
    virtual bool getBinaryValue(storm::expressions::Variable const& name) const override;

   private:
    void ensureSolved() const;
    void addVariable(storm::expressions::Variable const& variable, ValueType const& lowerBound, ValueType const& upperBound,
                     ValueType const& objectiveFunctionCoefficient);

#ifdef STORM_HAVE_SOPLEX
    typedef std::conditional_t<std::is_same_v<ValueType, double>, soplex::DVector, soplex::DVectorRational> TypedDVector;
    typedef std::conditional_t<std::is_same_v<ValueType, double>, soplex::DSVector, soplex::DSVectorRational> TypedDSVector;

    uint64_t nextVariableIndex = 0;
    uint64_t nextConstraintIndex = 0;
    // Mutable because signature requires optimize to be const...
    mutable soplex::SoPlex solver;

    mutable soplex::SPxSolver::Status status;

    //
    mutable TypedDVector primalSolution;

    // Variables;
    TypedDSVector variables = TypedDSVector(0);
    // A mapping from variables to their indices.
    std::map<storm::expressions::Variable, uint64_t> variableToIndexMap;
#endif
};
}  // namespace storm::solver