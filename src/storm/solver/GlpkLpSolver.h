#ifndef STORM_SOLVER_GLPKLPSOLVER_H_
#define STORM_SOLVER_GLPKLPSOLVER_H_

#include <map>
#include "storm/exceptions/NotImplementedException.h"
#include "storm/solver/LpSolver.h"

// To detect whether the usage of glpk is possible, this include is neccessary.
#include "storm-config.h"

#ifdef STORM_HAVE_GLPK
#include <glpk.h>
#endif

namespace storm {
namespace solver {
#ifdef STORM_HAVE_GLPK
/*!
 * A class that implements the LpSolver interface using glpk as the background solver.
 */
template<typename ValueType, bool RawMode = false>
class GlpkLpSolver : public LpSolver<ValueType, RawMode> {
   public:
    using VariableType = typename LpSolver<ValueType, RawMode>::VariableType;
    using Variable = typename LpSolver<ValueType, RawMode>::Variable;
    using Constant = typename LpSolver<ValueType, RawMode>::Constant;
    using Constraint = typename LpSolver<ValueType, RawMode>::Constraint;

    /*!
     * Constructs a solver with the given name and model sense.
     *
     * @param name The name of the LP problem.
     * @param optDir A value indicating whether the value of the objective function is to be minimized or
     * maximized.
     */
    GlpkLpSolver(std::string const& name, OptimizationDirection const& optDir);

    /*!
     * Constructs a solver with the given name. By default the objective function is assumed to be minimized,
     * but this may be altered later using a call to setOptimizationDirection.
     *
     * @param name The name of the LP problem.
     */
    GlpkLpSolver(std::string const& name);

    /*!
     * Constructs a solver without a name and the given model sense.
     *
     * @param optDir A value indicating whether the value of the objective function is to be minimized or
     * maximized.
     */
    GlpkLpSolver(OptimizationDirection const& optDir);

    /*!
     * Constructs a solver without a name. By default the objective function is assumed to be minimized,
     * but this may be altered later using a call to setOptimizationDirection.
     */
    GlpkLpSolver();

    /*!
     * Destructs a solver by freeing the pointers to glpk's structures.
     */
    virtual ~GlpkLpSolver();

    virtual Variable addVariable(std::string const& name, VariableType const& type, std::optional<ValueType> const& lowerBound = std::nullopt,
                                 std::optional<ValueType> const& upperBound = std::nullopt, ValueType objectiveFunctionCoefficient = 0) override;

    // Methods to incorporate recent changes.
    virtual void update() const override;

    // Methods to add constraints
    virtual void addConstraint(std::string const& name, Constraint const& constraint) override;
    virtual void addIndicatorConstraint(std::string const& name, Variable indicatorVariable, bool indicatorValue, Constraint const& constraint) override;

    // Methods to optimize and retrieve optimality status.
    virtual void optimize() const override;
    virtual bool isInfeasible() const override;
    virtual bool isUnbounded() const override;
    virtual bool isOptimal() const override;

    // Methods to retrieve values of variables and the objective function in the optimal solutions.
    virtual ValueType getContinuousValue(Variable const& name) const override;
    virtual int_fast64_t getIntegerValue(Variable const& name) const override;
    virtual bool getBinaryValue(Variable const& name) const override;
    virtual ValueType getObjectiveValue() const override;

    // Methods to print the LP problem to a file.
    virtual void writeModelToFile(std::string const& filename) const override;

    virtual void push() override;
    virtual void pop() override;

    virtual void setMaximalMILPGap(ValueType const& gap, bool relative) override;
    virtual ValueType getMILPGap(bool relative) const override;

   private:
    // The glpk LP problem.
    glp_prob* lp;

    // A mapping from variables to their indices.
    std::conditional_t<RawMode, std::vector<int>, std::map<storm::expressions::Variable, int>> variableToIndexMap;

    // A flag storing whether the model is an LP or an MILP.
    bool modelContainsIntegerVariables;

    // Flags that store whether the MILP was found to be infeasible or unbounded.
    mutable bool isInfeasibleFlag;
    mutable bool isUnboundedFlag;

    mutable double maxMILPGap;
    mutable bool maxMILPGapRelative;
    mutable double actualRelativeMILPGap;

    struct IncrementalLevel {
        std::vector<Variable> variables;
        int firstConstraintIndex;
    };
    std::vector<IncrementalLevel> incrementalData;
};
#else
// If glpk is not available, we provide a stub implementation that emits an error if any of its methods is called.
class GlpkLpSolver : public LpSolver {
   public:
    GlpkLpSolver(std::string const& name, OptimizationDirection const& modelSense) {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    GlpkLpSolver(std::string const& name) : LpSolver(MINIMIZE) {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    GlpkLpSolver(OptimizationDirection const& modelSense) : LpSolver(modelSense) {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    GlpkLpSolver() : LpSolver(MINIMIZE) {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual ~GlpkLpSolver() {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual void update() const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual void addConstraint(std::string const& name, storm::expressions::Expression const& constraint) override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual void optimize() const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual bool isInfeasible() const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual bool isUnbounded() const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual bool isOptimal() const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual ValueType getContinuousValue(storm::expressions::Variable const& variable) const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual int_fast64_t getIntegerValue(storm::expressions::Variable const& variable) const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual bool getBinaryValue(storm::expressions::Variable const& variable) const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual ValueType getObjectiveValue() const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual void writeModelToFile(std::string const& filename) const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual void push() override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual void pop() override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual ValueType getMILPGap(bool relative) const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }

    virtual ValueType getMILPGap(bool relative) const override {
        throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for glpk. Yet, a method was called that "
                                                              "requires this support. Please choose a version of support with glpk support.";
    }
};
#endif
}  // namespace solver
}  // namespace storm

#endif /* STORM_SOLVER_GLPKLPSOLVER_H_ */
